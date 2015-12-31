/*
 * readlink	Implementation of the readlink program that has a
 *		proper canonicalize function.
 *
 *		The canonicalized path is printed if the entire path
 *		exists, but _also_ if the last path element is missing
 *		and the path up till there is a directory.
 *		
 *		If you specify -f or --canonicalize twice (aka
 *		canonicalize-me-harder) then the "most canonical" path
 *		is printed. Even if a part of it doesn't exist, though
 *		the exit status will be non-zero in that case.
 *
 * realpath	If argv[0] is (^|/)realpath$, this program will behave
 *		as the realpath utility (-s not implemented).
 *		Note that 'readlink -ff' and 'realpath' are equivalent.
 *
 * Author:	Miquel van Smoorenburg.
 *
 * Version:	@(#)readlink  1.02  21-Mar-2004  miquels@cistron.nl
 *
 *		This file is part of the sysvinit suite,
 *		Copyright 2004 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 *		The realpath_sz() function in this file may also be
 *		redistributed under the terms of the LGPL:
 *
 *		This library is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU Library General Public
 *		License as published by the Free Software Foundation; either
 *		version 2 of the License, or (at your option) any later version.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/param.h>

struct option readlink_options[] = {
  {  "canonicalize",	0, NULL, 'f' },
  {  "no-newline",	0, NULL, 'n' },
  {  "quiet",		0, NULL, 'q' },
  {  "silent",		0, NULL, 's' },
  {  "help",		0, NULL, 'h' },
  {  "verbose",		0, NULL, 'v' },
  {  "version",		0, NULL, 'V' },
  {  NULL,		0, NULL, 0   },
};

struct option realpath_options [] = {
  {  "no-newline",	0, NULL, 'n' },
  {  "help",		0, NULL, 'h' },
  {  "verbose",		0, NULL, 'v' },
  {  "version",		0, NULL, 'V' },
  {  NULL,		0, NULL, 0   },
};

static int system_path_max(char *name)
{
	int		path_max;
#ifdef PATH_MAX
	path_max = PATH_MAX;
#else
	if ((path_max = pathconf (name, _PC_PATH_MAX)) <= 0)
		path_max = 1024;
#endif
	return path_max;
}

/*
 *	Error. Try to build the path till here into resolved,
 *	return -1 if that works, -2 if failed (that means that
 *	the contents of "resolved" make no sense whatsoever).
 */
static int path_err(char *resolved, int reslen, char *restpath)
{
	if (strlen(resolved) + strlen(restpath) + 1 >= reslen) {
		errno = ENAMETOOLONG;
		return -2;
	}
	strcat(resolved, "/");
	strcat(resolved, restpath);
	return -1;
}

/*
 *	Ye olde realpath() functionne.
 *
 *	It takes an extra size argument for the resolved argument.
 *
 *	Exit status:	 0 path resolved
 *			-1 path not fully resolved (part not accessible)
 *			   but canonicalized as far as possible.
 *			-2 error, "resolved" is invalid.
 *
 *	If exit status < 0 errno is set.
 */
static int realpath_sz(char *path, char *resolved, int reslen)
{
	struct stat	st;
	char		*p, *s;
	char		*buf, *buf2;
	int		c, n;
	int		exists = 1;
	int		link_max = _POSIX_LINK_MAX;
	int		links = 0;
	int		st_mode;

	/*
	 *	Some basic checks and allocations.
	 */
	if (strlen(path) + 2 >= reslen) {
		errno = ENAMETOOLONG;
		return -2;
	}
	if ((buf = alloca(reslen)) == NULL)
		return -2;
	if ((buf2 = alloca(reslen)) == NULL)
		return -2;

	/*
	 *	Start with the current working directory
	 *	if the path is relative.
	 */
	if (*path != '/') {
		if (getcwd(resolved, reslen - 1) == NULL) {
			if (errno == ERANGE)
				errno = ENAMETOOLONG;
			return -2;
		}
		strcpy(buf2, "/");
		strcat(buf2, path);
		path = buf2;
	} else {
		strcpy(resolved, "/");
	}

	st.st_mode = S_IFDIR;

	while (1) {

		/*
		 *	Skip multiple preceding slashes.
		 */
		while (*path == '/' && path[1] == '/') path++;

		/*
		 *	At this point, "resolved" contains the path up
		 *	till here without a trailing slash, and "path"
		 *	contains the rest of the path starting with a /.
		 *
		 *	One exception: if path is empty, then resolved
		 *	can contain a single trailing slash (happens if
		 *	path was just "/" in the previous round)
		 */
		st_mode = st.st_mode;
		st.st_mode = S_IFREG;
		if (exists > 0 && lstat(resolved, &st) < 0) {
			if (errno != ENOENT && errno != ENOTDIR &&
			    errno != EACCES)
				return path_err(resolved, reslen, path);
			exists--;
			if (errno != ENOENT) exists--;
		}

		/*
		 *	Remove trailing slash if this is a directory.
		 */
		if ((s = strrchr(resolved, '/')) != NULL && s[1] == 0 &&
		     s > resolved && S_ISDIR(st.st_mode))
			*s = 0;

		if (exists > 0 && S_ISLNK(st.st_mode)) {
			/*
			 *	It's a link. Prepend the rest of the
			 *	remaining path with the contents of
			 *	the link, remove the last element
			 *	from resolved, and loop.
			 */
			if (++links > link_max) {
				errno = ELOOP;
				return path_err(resolved, reslen, path);
			}

			if ((n = readlink(resolved, buf, reslen - 1)) < 0)
				return path_err(resolved, reslen, path);
			buf[n] = 0;

			if (buf[0] == '/')
				p = resolved + 1;
			else {
				p = strrchr(resolved, '/');
				if (p == resolved) p++;
			}
			c = *p;
			*p = 0;
			if (strlen(resolved) + strlen(buf) +
			    strlen(path) + 3 >= reslen) {
				*p = c;
				return path_err(resolved, reslen, path);
			}
			if (*path)
				strcat(buf, path);
			buf2[0] = 0;
			if (buf[0] != '/') strcat(buf2, "/");
			strcat(buf2, buf);
			path = buf2;

			continue;
		}

		/*
		 *	Are we done?
		 */
		if (*path == 0)
			break;

		/*
		 *	repeating /./ sequences can be skipped.
		 */
		while (path[0] == '/' && path[1] == '.' && path[2] == '/') {
			path += 2;
			while (*path == '/' && path[1] == '/') path++;
		}

		/*
		 *	Is it a trailing / or /. or /./ ?
		 */
		if (exists > 0 && (path[1] == 0 ||
		    (path[1] == '.' && (path[2] == 0 || path[2] == '/')))) {
			
			if (S_ISDIR(st.st_mode)) {
				/*
				 *	Skip . or ./
				 */
				path++;
				if (*path == '.') path++;
				continue;
			}

			/*
			 *	Yup, ".", but that doesn't work if the
			 *	parent path isn't a directory.
			 */
			errno = ENOTDIR;
			exists = -1;
		}

		/*
		 *	Is it ../ ?
		 */
		if (exists > 0 && path[1] == '.' && path[2] == '.' &&
			      (path[3] == 0 || path[3] == '/')) {

			if (S_ISDIR(st.st_mode)) {
				/*
				 *	Back up one element.
				 */
				p = strrchr(resolved, '/');
				if (p == resolved) p++;
				*p = 0;
				path += 3;
				continue;
			}

			/*
			 *	Yup, "..", but that doesn't work if the
			 *	parent path isn't a directory.
			 */
			errno = ENOTDIR;
			exists = -1;
		}

		/*
		 *	Okay add this element to the resolved path.
		 */
		for (p = path + 1; *p && *p != '/'; p++)
			;
		if (strlen(resolved) + (p - path) + 2 >= reslen) {
			errno = ENAMETOOLONG;
			return -2;
		}
		c = *p;
		*p = 0;
		if (resolved[0] == '/' && resolved[1] == 0) path++;
		strcat(resolved, path);
		*p = c;
		path = p;
	}

	return (exists > 0 || (exists == 0 && S_ISDIR(st_mode))) ? 0 : -1;
}

void usage(char *progname)
{
	fprintf(stderr, "Usage: %s: [OPTION]... FILE\n", progname);
	exit(1);
}

void version(char *progname)
{
	printf("%s version 1.0 (sysvinit 2.85)\n", progname);
	exit(1);
}

char *mystrerror(int e)
{
	if (e == EINVAL)
		return "Not a symbolic link";
	else
		return strerror(e);
}

int cmd_readlink(int argc, char **argv, char *progname)
{
	char		*buf;
	char		*path;
	int		c, l, e;
	int		opt_canon = 0;
	int		opt_nonl = 0;
	int		opt_verbose = 0;
	int		path_max;

	while ((c = getopt_long(argc, argv, "fnqsv", readlink_options, NULL)) != EOF) switch(c) {
		case 'f':
			opt_canon++;
			break;
		case 'n':
			opt_nonl++;
			break;
		case 'q':
		case 's':
			opt_verbose = 0;
			break;
		case 'v':
			opt_verbose = 1;
			break;
		case 'V':
			version(progname);
			break;
		default:
			usage(progname);
			break;
	}

	if (optind != argc - 1)
		usage(progname);
	path = argv[optind];

	/*
	 *	Rules on printing stdout, stderr and exit status:
	 *
	 *	simple readlink: succes: print value on stdout, exit 0
	 *			 fail:   print error on stderr, exit 1
	 *
	 *	canonicalize:	 succes: print path on stdout, exit 0
	 *			 fail:   print path on stdout if -ff
	 *				 print error on stderr if verbose
	 *				 exit 1 if path was still canonicalized
	 *				 exit 254 if not completely canon'ed
	 */

	l = opt_canon ? -2 : -1;

	path_max = system_path_max(path);
	if ((buf = alloca(path_max)) != NULL) {
		if (opt_canon) {
			l = realpath_sz(path, buf, path_max);
		} else {
			l = readlink(path, buf, path_max - 1);
			if (l >= 0)
				buf[l] = 0;
			else
				l = -2;
		}
	}

	e = errno;

	if (l >= 0 || opt_canon > 1)
		printf("%s%s", (l == -2) ? path : buf, opt_nonl ? "" : "\n");

	if (l < 0 && opt_verbose)
		fprintf(stderr, "%s: %s: %s\n", progname,
			(l == -2) ? path : buf, mystrerror(e));

	if (l < 0) return (l == -2) ? 254 : 1;
	return 0;
}

int cmd_realpath(int argc, char **argv, char *progname)
{
	char		*buf;
	char		*path;
	int		c, l, e;
	int		opt_nonl = 0;
	int		opt_verbose = 0;
	int		path_max;

	while ((c = getopt_long(argc, argv, "nv", realpath_options, NULL)) != EOF) switch(c) {
		case 'n':
			opt_nonl++;
			break;
		case 'v':
			opt_verbose = 1;
			break;
		case 'V':
			version(progname);
			break;
		default:
			usage(progname);
			break;
	}

	if (optind != argc - 1)
		usage(progname);
	path = argv[optind];

	path_max = system_path_max(path);
	if ((buf = alloca(path_max)) != NULL)
		l = realpath_sz(path, buf, path_max);
	else
		l = -2;

	e = errno;

	printf("%s%s", (l == -2) ? path : buf, opt_nonl ? "" : "\n");

	if (l < 0 && opt_verbose)
		fprintf(stderr, "%s: %s: %s\n", progname,
			(l == -2) ? path : buf, mystrerror(e));

	if (l < 0) return (l == -2) ? 254 : 1;
	return 0;
}

int main(int argc, char **argv)
{
	char		*p;

	if ((p = strrchr(argv[0], '/')) != NULL)
		p++;
	else
		p = argv[0];

	if (strcmp(p, "realpath") == 0)
		return cmd_realpath(argc, argv, p);

	return cmd_readlink(argc, argv, "readlink");
}

