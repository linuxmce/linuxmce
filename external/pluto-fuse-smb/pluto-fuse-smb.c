/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/vfs.h>
#include <pthread.h>
#include <libsmbclient.h>
#include <time.h>

#define MY_MAXPATHLEN (MAXPATHLEN + 256)

const char * fusesmb_auth_fn_username = NULL, * fusesmb_auth_fn_password = NULL;

static void fusesmb_auth_fn(const char *server, const char *share,
                            char *workgroup, int wgmaxlen,
                            char *username, int unmaxlen,
                            char *password, int pwmaxlen)
{
    (void)workgroup;
    (void)wgmaxlen;
	(void)share;

    /* Don't authenticate for workgroup listing */
    if (NULL == server || server[0] == '\0')
        return;
	if (fusesmb_auth_fn_username != NULL)
		strncpy(username, fusesmb_auth_fn_username, unmaxlen);
	if (fusesmb_auth_fn_password != NULL)
		strncpy(password, fusesmb_auth_fn_password, pwmaxlen);
	return;
}

/*
 * Create a new libsmbclient context with all necessary options
 */
static SMBCCTX *fusesmb_context(smbc_get_auth_data_fn fn)
{
    /* Initializing libsbmclient */
    SMBCCTX *ctx;
    ctx = smbc_new_context();
    if (ctx == NULL)
        return NULL;

    ctx->callbacks.auth_fn = fn;
    //ctx->debug = 4;
    /* Timeout a bit bigger, by Jim Ramsay */
    ctx->timeout = 10000;       //10 seconds
    /* Kerberos authentication by Esben Nielsen */
#if defined(SMB_CTX_FLAG_USE_KERBEROS) && defined(SMB_CTX_FLAG_FALLBACK_AFTER_KERBEROS)
    ctx->flags |=
        SMB_CTX_FLAG_USE_KERBEROS | SMB_CTX_FLAG_FALLBACK_AFTER_KERBEROS;
#endif
    //ctx->options.one_share_per_server = 1;
    ctx = smbc_init_context(ctx);
    return ctx;
}

SMBCCTX *fusesmb_new_context(const char * username, const char * password)
{
    fusesmb_auth_fn_username = username;
	fusesmb_auth_fn_password = password;
    return fusesmb_context(fusesmb_auth_fn);
}

/* Mutex for locking the Samba context */

/* To prevent deadlock, locking order should be:

[rwd]ctx_mutex -> cfg_mutex -> opts_mutex
[rwd]ctx_mutex -> opts_mutex
*/

static pthread_mutex_t ctx_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t rwd_ctx_mutex = PTHREAD_MUTEX_INITIALIZER;
static SMBCCTX *ctx, *rwd_ctx;
pthread_t cleanup_thread;

struct fusesmb_opt {
    int timeout;
    char *username;
    char *password;
	char *service;
	char *mount_point;
};
/* Read settings from fusesmb.conf and or set default value */
pthread_mutex_t cfg_mutex = PTHREAD_MUTEX_INITIALIZER;
struct fusesmb_opt opts;
pthread_mutex_t opts_mutex = PTHREAD_MUTEX_INITIALIZER;
char fusesmb_cache_bin[MAXPATHLEN];

static int fusesmb_getattr(const char *path, struct stat *stbuf)
{
    char smb_path[MY_MAXPATHLEN] = "smb:";
    memset(stbuf, 0, sizeof(struct stat));

	strcat(smb_path, opts.service);
	strcat(smb_path, path);
	pthread_mutex_lock(&ctx_mutex);
	if (ctx->stat(ctx, smb_path, stbuf) < 0)
	{
		pthread_mutex_unlock(&ctx_mutex);
		return -errno;
	}
	pthread_mutex_unlock(&ctx_mutex);
	return 0;
}

static int fusesmb_opendir(const char *path, struct fuse_file_info *fi)
{
    SMBCFILE *dir;
    char smb_path[MY_MAXPATHLEN] = "smb:";
    strcat(smb_path, opts.service);
	strcat(smb_path, path);
    pthread_mutex_lock(&ctx_mutex);
    dir = ctx->opendir(ctx, smb_path);
    if (dir == NULL)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
    fi->fh = (unsigned long)dir;
    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static int fusesmb_readdir(const char *path, void *h, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
	(void)path;
    struct smbc_dirent *pdirent;
    struct stat st;
    memset(&st, 0, sizeof(st));

    /* Listing contents of a share */
	pthread_mutex_lock(&ctx_mutex);
	while (NULL != (pdirent = ctx->readdir(ctx, (SMBCFILE *)fi->fh)))
	{
		if (pdirent->smbc_type == SMBC_DIR)
		{
			st.st_mode = DT_DIR << 12;
			filler(h, pdirent->name, &st, 0);
		}
		if (pdirent->smbc_type == SMBC_FILE)
		{
			st.st_mode = DT_REG << 12;
			filler(h, pdirent->name, &st, 0);
		}
	}
	pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static int fusesmb_releasedir(const char *path, struct fuse_file_info *fi)
{
    (void) path;

    pthread_mutex_lock(&ctx_mutex);
    ctx->closedir(ctx, (SMBCFILE *)fi->fh);
    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static int fusesmb_open(const char *path, struct fuse_file_info *fi)
{
    SMBCFILE *file;
    char smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
    strcat(smb_path, path);

    pthread_mutex_lock(&rwd_ctx_mutex);
    file = rwd_ctx->open(rwd_ctx, smb_path, fi->flags, 0);

    if (file == NULL)
    {
        pthread_mutex_unlock(&rwd_ctx_mutex);
        return -errno;
    }

    fi->fh = (unsigned long)file;
    pthread_mutex_unlock(&rwd_ctx_mutex);
    return 0;
}

static int fusesmb_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    SMBCFILE *file;
    char smb_path[MY_MAXPATHLEN] = "smb:";

    //printf("%i\n", offset);
    //fflush(stdout);

	strcat(smb_path, opts.service);
    strcat(smb_path, path);

    int tries = 0;              //For number of retries before failing
    ssize_t ssize;              //Returned by ctx->read

    pthread_mutex_lock(&rwd_ctx_mutex);
    /* Ugly goto but it works ;) But IMHO easiest solution for error handling here */
    goto seek;
  reopen:
    if ((file = rwd_ctx->open(rwd_ctx, smb_path, fi->flags, 0)) == NULL)
    {
        /* Trying to reopen when out of memory */
        if (errno == ENOMEM)
        {
            tries++;
            if (tries > 4)
            {
                pthread_mutex_unlock(&rwd_ctx_mutex);
                return -errno;
            }
            goto reopen;
        }
        /* Other errors from docs cannot be recovered from so returning the error */
        else
        {
            pthread_mutex_unlock(&rwd_ctx_mutex);
            return -errno;
        }
    }
    fi->fh = (unsigned long)file;
  seek:

    if (rwd_ctx->lseek(rwd_ctx, (SMBCFILE *)fi->fh, offset, SEEK_SET) == (off_t) - 1)
    {
        /* Bad file descriptor try to reopen */
        if (errno == EBADF)
        {
            goto reopen;
        }
        else
        {
            //SMB Init failed
            pthread_mutex_unlock(&rwd_ctx_mutex);
            return -errno;
        }
    }
    if ((ssize = rwd_ctx->read(rwd_ctx, (SMBCFILE *)fi->fh, buf, size)) < 0)
    {
        /* Bad file descriptor try to reopen */
        if (errno == EBADF)
        {
            goto reopen;
        }
        /* Tried opening a directory / or smb_init failed */
        else
        {
            pthread_mutex_unlock(&rwd_ctx_mutex);
            return -errno;
        }
    }
    pthread_mutex_unlock(&rwd_ctx_mutex);
    return (size_t) ssize;
}

static int fusesmb_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    SMBCFILE *file;
    char smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
    strcat(smb_path, path);

    int tries = 0;              //For number of retries before failing
    ssize_t ssize;              //Returned by ctx->read

    pthread_mutex_lock(&rwd_ctx_mutex);
    /* Ugly goto but it works ;) But IMHO easiest solution for error handling here */
    goto seek;
  reopen:
    if (NULL == (file = rwd_ctx->open(rwd_ctx, smb_path, fi->flags, 0)))
    {
        /* Trying to reopen when out of memory */
        if (errno == ENOMEM)
        {
            tries++;
            if (tries > 4)
            {
                pthread_mutex_unlock(&rwd_ctx_mutex);
                return -errno;
            }
            goto reopen;
        }
        /* Other errors from docs cannot be recovered from so returning the error */
        pthread_mutex_unlock(&rwd_ctx_mutex);
        return -errno;

    }
    fi->fh = (unsigned long)file;
  seek:

    if (rwd_ctx->lseek(rwd_ctx, (SMBCFILE *)fi->fh, offset, SEEK_SET) == (off_t) - 1)
    {
        /* Bad file descriptor try to reopen */
        if (errno == EBADF)
        {
            goto reopen;
        }
        else
        {
            //SMB Init failed
            pthread_mutex_unlock(&rwd_ctx_mutex);
            return -errno;
        }
    }
    if ((ssize = rwd_ctx->write(rwd_ctx, (SMBCFILE *)fi->fh, (void *) buf, size)) < 0)
    {
        /* Bad file descriptor try to reopen */
        if (errno == EBADF)
        {
            goto reopen;
        }
        /* Tried opening a directory / or smb_init failed */
        else
        {
            pthread_mutex_unlock(&rwd_ctx_mutex);
            return -errno;
        }
    }
    pthread_mutex_unlock(&rwd_ctx_mutex);
    return (size_t) ssize;
}

static int fusesmb_release(const char *path, struct fuse_file_info *fi)
{
    (void)path;
    pthread_mutex_lock(&rwd_ctx_mutex);
#ifdef HAVE_LIBSMBCLIENT_CLOSE_FN
    rwd_ctx->close_fn(rwd_ctx, (SMBCFILE *)fi->fh);
#else
    rwd_ctx->close(rwd_ctx, (SMBCFILE *)fi->fh);
#endif
    pthread_mutex_unlock(&rwd_ctx_mutex);
    return 0;

}

static int fusesmb_mknod(const char *path, mode_t mode,
                     __attribute__ ((unused)) dev_t rdev)
{
    char smb_path[MY_MAXPATHLEN] = "smb:";
    SMBCFILE *file;

    /* FIXME:
       Check which rdevs are supported, currently only a file
       is created
     */
    //if (rdev != S_IFREG)
    //  return -EACCES;

	strcat(smb_path, opts.service);
    strcat(smb_path, path);
    pthread_mutex_lock(&ctx_mutex);
    if ((file = ctx->creat(ctx, smb_path, mode)) == NULL)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
#ifdef HAVE_LIBSMBCLIENT_CLOSE_FN
    ctx->close_fn(ctx, file);
#else
    ctx->close(ctx, file);
#endif

    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static int fusesmb_statfs(const char *path, struct statfs *fst)
{
    /* Returning stat of local filesystem, call is too expensive */
    (void)path;
    memset(fst, 0, sizeof(struct statfs));
    if (statfs("/", fst) != 0)
        return -errno;
    return 0;
}

static int fusesmb_unlink(const char *file)
{
    char smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
    strcat(smb_path, file);
    pthread_mutex_lock(&ctx_mutex);
    if (ctx->unlink(ctx, smb_path) < 0)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static int fusesmb_rmdir(const char *path)
{
    char smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
    strcat(smb_path, path);
    pthread_mutex_lock(&ctx_mutex);

    if (ctx->rmdir(ctx, smb_path) < 0)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static int fusesmb_mkdir(const char *path, mode_t mode)
{
    char smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
    strcat(smb_path, path);
    pthread_mutex_lock(&ctx_mutex);
    if (ctx->mkdir(ctx, smb_path, mode) < 0)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
    pthread_mutex_unlock(&ctx_mutex);
    
    return 0;
}

static int fusesmb_utime(const char *path, struct utimbuf *buf)
{
    (void)path;
    (void)buf;
    /* libsmbclient has no equivalent function for this, so
       always returning success
     */
    return 0;
}

static int fusesmb_chmod(const char *path, mode_t mode)
{
    char smb_path[MY_MAXPATHLEN] = "smb:";
	strcat(smb_path, opts.service);
    strcat(smb_path, path);

    pthread_mutex_lock(&ctx_mutex);
    if (ctx->chmod(ctx, smb_path, mode) < 0)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}
static int fusesmb_chown(const char *path, uid_t uid, gid_t gid)
{
    (void)path;
    (void)uid;
    (void)gid;
    /* libsmbclient has no equivalent function for this, so
       always returning success
     */
    return 0;
}

static int fusesmb_truncate(const char *path, off_t size)
{

    //fprintf(stderr, "TRUNCATE %s [%llu]\n", path, size);
    char smb_path[MY_MAXPATHLEN] = "smb:";

    SMBCFILE *file;
	strcat(smb_path, opts.service);
    strcat(smb_path, path);
    if (size == 0)
    {
        pthread_mutex_lock(&ctx_mutex);
        if (NULL == (file = ctx->creat(ctx, smb_path, 0666)))
        {
            pthread_mutex_unlock(&ctx_mutex);
            return -errno;
        }
#ifdef HAVE_LIBSMBCLIENT_CLOSE_FN
        ctx->close_fn(ctx, file);
#else
        ctx->close(ctx, file);
#endif
        pthread_mutex_unlock(&ctx_mutex);
        return 0;
    }
    return -ENOTSUP;
}

static int fusesmb_rename(const char *path, const char *new_path)
{
    char smb_path[MY_MAXPATHLEN]     = "smb:",
         new_smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
    strcat(smb_path, path);
	strcat(new_smb_path, opts.service);
    strcat(new_smb_path, new_path);

    pthread_mutex_lock(&ctx_mutex);
    if (ctx->rename(ctx, smb_path, ctx, new_smb_path) < 0)
    {
        pthread_mutex_unlock(&ctx_mutex);
        return -errno;
    }
    pthread_mutex_unlock(&ctx_mutex);
    return 0;
}

static void *fusesmb_init()
{
    return NULL;
}

static void fusesmb_destroy(void *private_data)
{
    (void)private_data;
}

static struct fuse_operations fusesmb_oper = {
    .getattr    = fusesmb_getattr,
    .readlink   = NULL, //fusesmb_readlink,
    .opendir    = fusesmb_opendir,
    .readdir    = fusesmb_readdir,
    .releasedir = fusesmb_releasedir,
    .mknod      = fusesmb_mknod,
    .mkdir      = fusesmb_mkdir,
    .symlink    = NULL, //fusesmb_symlink,
    .unlink     = fusesmb_unlink,
    .rmdir      = fusesmb_rmdir,
    .rename     = fusesmb_rename,
    .link       = NULL, //fusesmb_link,
    .chmod      = fusesmb_chmod,
    .chown      = fusesmb_chown,
    .truncate   = fusesmb_truncate,
    .utime      = fusesmb_utime,
    .open       = fusesmb_open,
    .read       = fusesmb_read,
    .write      = fusesmb_write,
    .statfs     = fusesmb_statfs,
    .release    = fusesmb_release,
    .fsync      = NULL, //fusesmb_fsync,
    .init       = fusesmb_init,
    .destroy    = fusesmb_destroy,
#ifdef HAVE_SETXATTR
    .setxattr   = fusesmb_setxattr,
    .getxattr   = fusesmb_getxattr,
    .listxattr  = fusesmb_listxattr,
    .removexattr= fusesmb_removexattr,
#endif
};


void usage(const char * cmd)
{
	fprintf(stderr,
			"Usage:\n"
			"\t%s [-U username] [-P password] <service> <mount point> [FUSE options]\n",
			cmd
		);
}

int main(int argc, char *argv[])
{
    /* Workaround for bug in libsmbclient:
       Limit reads to 32 kB
     */
    int i, j, last_option = 0;
	char * cred_user = NULL, * cred_pass = NULL;

	/* Get our parameters first */
	opts.username = NULL;
	opts.password = NULL;
	opts.timeout = 10;

	if (argc < 3)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				case 'U':
					i++;
					opts.username = argv[i];
					break;
				case 'P':
					i++;
					opts.password = argv[i];
					break;
				case 'C':
					i++;
					FILE * f = fopen(argv[i], "r");
					if (f == NULL)
					{
						fprintf(stderr, "ERROR: Failed to open credentials file\n");
						exit(EXIT_FAILURE);
					}
					char buffer[1024];
					memset(buffer, 0, 1024);
					while (fgets(buffer, 1024, f) != NULL)
					{
						int lastchr = strlen(buffer) - 1;
						if (buffer[lastchr] == '\n')
							buffer[lastchr] = 0;
						char * value = strchr(buffer, '=');
						if (value == NULL)
							continue;
						value++;
						for (j = 0; j <= lastchr; j++)
							if (buffer[j] == ' ' || buffer[j] == '=') // XXX: BUG: usernames and passwords which contain the equal sign are BROKEN!!!
								buffer[j] = 0;
						while (* value == 0 && value <= &buffer[lastchr])
							value++;
						if (strstr(buffer, "username") == buffer)
						{
							if (cred_user != NULL)
								free(cred_user);
							cred_user = strdup(value);
						}
						else if (strstr(buffer, "password") == buffer)
						{
							if (cred_pass != NULL)
								free(cred_pass);
							cred_pass = strdup(value);
						}
					}
					fclose(f);
					opts.username = cred_user;
					opts.password = cred_pass;
					break;
				default:
					fprintf(stderr, "ERROR: Unknown parameter '%c'\n", argv[i][0]);
					usage(argv[0]);
					exit(EXIT_FAILURE);
			}
		}
		else /* end of options */
		{
			last_option = i;
			break;
		}
	}
	opts.service = argv[last_option];
	if (opts.service == NULL)
	{
		fprintf(stderr, "ERROR: Service path not given\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	last_option ++;
	opts.mount_point = argv[last_option];
	if (opts.mount_point == NULL)
	{
		fprintf(stderr, "ERROR: Mount point not given\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	int fuse_argc = argc - last_option + 2;
	char ** fuse_argv = (char **) malloc((fuse_argc) * sizeof(char *));
	fuse_argv[0] = argv[0];
	for (i = last_option + 1; i < argc; i++)
	{
		fuse_argv[i - last_option] = argv[i];
	}

    /* libsmbclient doesn't work with reads bigger than 32k */
    char *max_read = "-omax_read=32768";
	fuse_argv[fuse_argc - 2] = max_read;
	fuse_argv[fuse_argc - 1] = opts.mount_point;

    ctx = fusesmb_new_context(opts.username, opts.password);
    rwd_ctx = fusesmb_new_context(opts.username, opts.password);

    if (ctx == NULL || rwd_ctx == NULL)
        exit(EXIT_FAILURE);

	struct stat stbuf;
	char smb_path[MY_MAXPATHLEN] = "smb:";

	strcat(smb_path, opts.service);
	strcat(smb_path, "/");

	if (ctx->stat(ctx, smb_path, &stbuf) < 0)
	{
		exit(EXIT_FAILURE);
	}

    fuse_main(fuse_argc, fuse_argv, &fusesmb_oper);

    smbc_free_context(ctx, 1);
    smbc_free_context(rwd_ctx, 1);

	if (cred_user != NULL)
		free(cred_user);
	if (cred_pass != NULL)
		free(cred_pass);

    exit(EXIT_SUCCESS);
}
