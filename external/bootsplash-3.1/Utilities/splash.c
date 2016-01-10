/* Splash control program.
 * 
 * (c) 2001, 2002, 2003 by Stefan Reinauer <stepan@suse.de>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <getopt.h>


#define PROCSPLASH "/proc/splash"
#define BUFSIZE 4096
#if 1
#define DEBUG(x...)
#else
#define DEBUG(x...) printf(x)
#endif

#define MAXBOX 512

long unit = 0;
long version = 1;
long state = -1;
long fgcolor = -1;
long bgcolor = -1;
long tx = -1;
long ty = -1;
long tw = -1;
long th = -1;
long px = 0;
long py = 0;
long pw = 0;
long ph = 0;
long pr = 0;
long pg = 0;
long pb = 0;
long pa = 160;
long pf = 1;
long pc = 0x141464;
long ptl = 0xffff;
long pbr = 0x00;
long ax = 0;
long ay = 0;
long progress = 0;
long overpaintok = 0;
long percent = 0;
long text = 0;
char *jpgfile = 0;
char *silentjpgfile = 0;
char splashdscname[512];

int boxcount;
unsigned char boxes[12 * MAXBOX];
int sboxcount;
unsigned char sboxes[12 * MAXBOX];

struct parstab {
	char *name;
	long *val;
	char **str;
} parstab[] = {
	{ "unit", &unit }, 
	{ "version", &version }, 
	{ "state", &state }, 
	{ "fgcolor", &fgcolor }, 
	{ "bgcolor", &bgcolor },
	    /* v1+ */
	{ "tx", &tx }, 
	{ "ty", &ty }, 
	{ "tw", &tw }, 
	{ "th", &th },
	    /* v2+ */
	{ "px", &px }, 
	{ "py", &py }, 
	{ "pw", &pw }, 
	{ "ph", &ph }, 
	{ "pr", &pr }, 
	{ "pg", &pg }, 
	{ "pb", &pb }, 
	{ "ax", &ax }, 
	{ "ay", &ay },
	    /* v3+ */
	    /* independant */
	{ "progress_enable", &progress }, 
	{ "progress_x", &progress }, 
	{ "progress_y", &progress }, 
	{ "progress_width", &progress }, 
	{ "progress_height", &progress }, 
	{ "progress_fgcolor", &progress }, 
	{ "progress_bgcolor", &progress }, 
	{ "text_size", &text }, 
	{ "text_color", &text }, 
	{ "text_x", &text }, 
	{ "text_y", &text }, 
	{ "text_font", (long *) 0 }, 
	{ "silentjpeg", (long *) 0, &silentjpgfile }, 
	{ "percent", &percent }, 
	{ "overpaintok", &overpaintok }, 
	{ "jpeg", (long *) 0, &jpgfile }, 
	{ (char *) 0, (long *) 0 }
};

int fixboxcopy(unsigned char *buf, int cnt)
{
	int boxoff[MAXBOX];
	unsigned char *bp;
	int bc = 0, bc2 = 0;
	int i, ni;

	for (i = 0; i < cnt; i = ni) {
		ni = i + 1;
		bp = buf + 12 * i;
		if (bp[3] > 0x7f)
			ni++;
		if (bp[1] > 0x7f)
			ni += buf[12 * ni + 3] >= 0x7f ? 2 : 1;
		if (bp[0] == 0xff && bp[1] == 0x7f)
			continue;
		boxoff[bc++] = i;
	}
	for (i = 0; i < cnt; i = ni) {
		ni = i + 1;
		bp = buf + 12 * i;
		if (bp[3] > 0x7f)
			ni++;
		if (bp[1] > 0x7f)
			ni += buf[12 * ni + 3] >= 0x7f ? 2 : 1;
		if (bp[0] == 0xff && bp[1] == 0x7f && bp[2] == 0xfe && bp[3] == 0x7f) {
			i = bp[6] | bp[7] << 8;
			if (i > 32767)
				i -= 65536;
			i += bc2;
			if (i < 0 || i >= bc) {
				fprintf(stderr, "box copy out of range\n");
				exit(1);
			}
			i = boxoff[i] - ni;
			bp[6] = i & 0xff;
			bp[7] = (i >> 8) & 0xff;
			continue;
		}
		if (bp[0] == 0xff && bp[1] == 0x7f)
			continue;
		bc2++;
	}
}

void parsebox(char *pl)
{
	int co[4];
	int i, j, c, d = 0;
	char cols[16];
	unsigned char *bp;
	int boxcnt;
	int isibox = 0;
	int *boxcntp;
	unsigned char *boxptr;
	char *p = pl + 3;
	int noover = 0;
	int hidemax = 0;
	int isstip = 0;
	int iscopy = 0;
	int range = 0;
	int anchor = -1;

	boxcntp = &boxcount;
	boxptr = boxes;
	for (i = 0; i < 4; i++) {
		if (*p != ' ' && *p != '\t' && *p != '=') {
			fprintf(stderr, "syntax error: '%c' %s\n", *p, pl);
			exit(1);
		}
		while (*p == ' ' || *p == '\t' || *p == '=')
			p++;
		if (i == 0 && !strncmp(p, "copy", 4)) {
			i = 3 - 1;
			iscopy = 1;
			co[0] = 0x7fff;
			co[1] = 0x7ffe;
			co[2] = co[3] = 0;
			cols[0] = cols[1] = cols[2] = cols[3] = 0;
			p += 4;
			continue;
		}
		if (iscopy && *p == '-') {
			iscopy = -1;
			p++;
		}
		if (i == 0 && !strncmp(p, "stipple", 7)) {
			i = 2 - 1;
			co[0] = co[1] = 0x7fff;
			co[2] = co[3] = 0;
			isstip = 1;
			p += 7;
			continue;
		}
		if (isstip && i == 2 && !strncmp(p, "range", 5)) {
			i--;
			range = 1;
			p += 5;
			continue;
		}
		if (isstip && i == 2 && !strncmp(p, "anchorabs", 9)) {
			i--;
			anchor = 0;
			p += 9;
			continue;
		}
		if (isstip && i == 2 && !strncmp(p, "anchorstart", 11)) {
			i--;
			anchor = -1;
			p += 11;
			continue;
		}
		if (isstip && i == 2 && !strncmp(p, "anchorend", 9)) {
			i--;
			anchor = 1;
			p += 9;
			continue;
		}
		if (i == 0 && !strncmp(p, "silent", 6)) {
			i--;
			boxcntp = &sboxcount;
			boxptr = sboxes;
			p += 6;
			continue;
		}
		if (i == 0 && !strncmp(p, "inter", 5)) {
			i--;
			isibox = 1;
			p += 5;
			continue;
		}
		if (i == 0 && !strncmp(p, "noover", 6)) {
			i--;
			noover = 1;
			p += 6;
			continue;
		}
		if (i == 0 && !strncmp(p, "hidemax", 7)) {
			i--;
			hidemax = 1;
			p += 7;
			continue;
		}
		if (isstip && !range) {
			p--;
			break;
		}
		co[i] = strtol(p, (char **) NULL, 0);
		while (*p == 'x' || (*p >= '0' && *p <= '9')
		       || (*p >= 'a' && *p <= 'f') || (*p >= 'A'
						       && *p <= 'F'))
			p++;
	}
	if (iscopy < 0)
		co[3] = -co[3];
	if (isstip && anchor < 0)
		co[2] = ~co[2];
	if (isstip && anchor > 0)
		co[3] = ~co[3];
	for (i = 0; !iscopy && i < 4; i++) {
		if (*p != ' ' && *p != '\t') {
			fprintf(stderr, "syntax error: %s\n", pl);
			exit(1);
		}
		while (*p == ' ' || *p == '\t')
			p++;
		if (i == 1 && *p == 0)
			break;
		if (*p++ != '#') {
			fprintf(stderr, "syntax error: %s\n", pl);
			exit(1);
		}
		for (j = 0; j < 8; j++) {
			c = *p++;
			if (c >= '0' && c <= '9')
				c -= '0';
			else if (c >= 'a' && c <= 'f')
				c -= 'a' - 10;
			else if (c >= 'A' && c <= 'F')
				c -= 'A' - 10;
			else {
				fprintf(stderr, "syntax error: %s\n", pl);
				exit(1);
			}
			if (!(j & 1)) {
				d = c;
				continue;
			}
			c += d << 4;
			cols[i * 4 + (j >> 1)] = c;
			if (!isstip && j == 5 && (*p == 0 || *p == ' ' || *p == '\t')) {
				cols[i * 4 + 3] = 255;
				break;
			}
		}
		if (*p == 0 && i == 0) {
			i = 1;
			break;
		}
	}
	while (*p == ' ' || *p == '\t')
		p++;
	if (*p) {
		fprintf(stderr, "syntax error: %s\n", pl);
		exit(1);
	}
	boxcnt = *boxcntp;
	if (boxcnt >= MAXBOX - 2) {
		fprintf(stderr, "too many boxes\n");
		exit(1);
	}
	if (isstip || iscopy) {
		isibox = noover = hidemax = 0;
	}
	bp = boxptr + 12 * boxcnt++;
	if (isibox)
		co[0] = ~co[0];
	if (i > 1) {
		co[1] = ~co[1];
		boxcnt++;
	}
	if (noover)
		co[2] = ~co[2];
	if (hidemax)
		co[3] = ~co[3];
	*bp++ = co[0] & 255;
	*bp++ = co[0] >> 8;
	*bp++ = co[1] & 255;
	*bp++ = co[1] >> 8;
	*bp++ = co[2] & 255;
	*bp++ = co[2] >> 8;
	*bp++ = co[3] & 255;
	*bp++ = co[3] >> 8;
	for (j = 0; j < i * 4; j++)
		*bp++ = cols[j];
	*boxcntp = boxcnt;
}

void parsefile(char *cfgfile, FILE *fp, int depth)
{
	char buf[256], *p;
	struct parstab *pt;
	int l;
	FILE *ifp;

	while (fgets(buf, sizeof(buf), fp)) {
		l = strlen(buf);
		if (l == 0)
			continue;
		if (l == sizeof(buf) - 1) {
			fprintf(stderr, "line too long\n");
			exit(1);
		}
		buf[--l] = 0;
		while (l
		       && (buf[l - 1] == ' '
			   || (buf[l - 1] == '\t')))
			buf[--l] = 0;
		for (p = buf; *p == ' ' || *p == '\t'; p++);
		if (*p == 0 || *p == '#')
			continue;
		if (!strncmp(p, "includeprog", 11)) {
			p += 11;
			if (depth > 16) {
				fprintf(stderr, "recursion limit reached\n");
				exit(1);
			}
			while (*p == ' ' || *p == '\t')
				p++;
			if ((ifp = popen(p, "r")) == 0) {
				perror(p);
				exit(1);
			}
			parsefile(p, ifp, depth + 1);
			if ((l = pclose(ifp))) {
				fprintf(stderr, "%s: exit %d\n", p, l > 255 ? (l >> 8) : l);
				exit(1);
			}
			continue;
		}
		if (!strncmp(p, "box", 3)) {
			parsebox(p);
			continue;
		}
		for (pt = parstab; pt->name; pt++) {
			l = strlen(pt->name);
			if (!strncmp(p, pt->name, l)
			    && (p[l] == ' ' || p[l] == '\t'
				|| p[l] == '='))
				break;
		}
		if (!pt->name) {
			fprintf(stderr, "syntax error: %s\n", p);
			exit(1);
		}
		while (p[l] == ' ' || p[l] == '\t')
			l++;
		if (p[l++] != '=') {
			fprintf(stderr, "syntax error: %s\n", p);
			exit(1);
		}
		while (p[l] == ' ' || p[l] == '\t')
			l++;
		if (pt->val)
			*pt->val =
			    strtol(p + l, (char **) NULL, 0);
		else
			*pt->str = strdup(p + l);
	}
}

void setsplash(char *cfgfile, int filterflag, int verbose_only)
{
	struct stat stb;
	char buf[256];
	char *p;
	int l, size, fd;
	FILE *fp = 0;
	FILE *sfp = 0;
	struct parstab *pt;
	unsigned char *pic;
	int silentl = 0;

	if (cfgfile) {
		if ((fp = fopen(cfgfile, "r")) == 0) {
			perror(cfgfile);
			exit(1);
		}
		parsefile(cfgfile, fp, 0);
		fclose(fp);

		if (!jpgfile) {
			l = strlen(cfgfile);
			if (l > 4 && !strcmp(cfgfile + l - 4, ".cfg")) {
				jpgfile = strdup(cfgfile);
				strcpy(jpgfile + l - 4, ".jpg");
				p = strstr(jpgfile, "config");
				if (p)
					strncpy(p, "images", 6);
			}
		}

		if (tx == 0 && ty == 0 && tw == 0 && th == 0) {
			tx = px + 8;
			ty = py + 4;
			tw = pw - 16;
			th = ph - 8;
		}

		if (jpgfile
		    && (*jpgfile == 0 || tx < 0 || ty < 0 || tw <= 0
			|| th <= 0)) {
			free(jpgfile);
			jpgfile = 0;
		}
		fixboxcopy(boxes, boxcount);
		fixboxcopy(sboxes, sboxcount);
	}

	if (verbose_only)
		silentjpgfile = NULL;


	if (silentjpgfile && jpgfile) {
		if ((sfp = fopen(silentjpgfile, "r")) == 0) {
			perror(silentjpgfile);
			exit(1);
		}
		if (fstat(fileno(sfp), &stb) == -1) {
			perror("fstat");
			exit(1);
		}
		silentl = stb.st_size;
	}

	if (jpgfile) {
		if ((fp = fopen(jpgfile, "r")) == 0) {
			perror(jpgfile);
			exit(1);
		}
		if (fstat(fileno(fp), &stb) == -1) {
			perror("fstat");
			exit(1);
		}
		l = stb.st_size;
		pic =
		    calloc(47 + l + silentl + boxcount * 12 +
			   sboxcount * 12, 1);
		if (pic == 0) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}
		if (fread(pic + 47 + boxcount * 12, l, 1, fp) != 1) {
			perror("fread");
			exit(1);
		}
		if (silentjpgfile) {
			if (fread
			    (pic + 47 + l + boxcount * 12 + sboxcount * 12,
			     silentl, 1, sfp) != 1) {
				perror("fread");
				exit(1);
			}
			fclose(sfp);
			if (sboxcount)
				memmove(pic + 47 + l + boxcount * 12,
					sboxes, sboxcount * 12);
			silentl += sboxcount * 12;
			l += silentl;
		}
		fclose(fp);
	} else {
		l = 0;
		pic = calloc(47, 1);
		if (pic == 0) {
			fprintf(stderr, "Out of memory.\n");
			exit(1);
		}
	}
	size = l;
	if (version < 1 || version > 3) {
		fprintf(stderr, "Illegal version: %ld\n", version);
		exit(1);
	}
	if (version == 1 && unit != 0) {
		/* convert to version 2 */
		tx *= 8;
		ty *= 16;
		tw *= 8;
		th *= 16;
		px = tx + 10;
		py = ty + 10;
		pw = tw - 20;
		ph = th - 20;
		pr = pg = pb = 240;
		state = 1;
		fgcolor = 0;
		bgcolor = 15;
		version = 2;
	}
	if (version == 1) {
		/* write version 1 file */
		strcpy(pic, "BOOTSPL1");
		pic[8] = tx;
		pic[9] = tx >> 8;
		pic[10] = ty;
		pic[11] = ty >> 8;
		pic[12] = tw;
		pic[13] = tw >> 8;
		pic[14] = th;
		pic[15] = th >> 8;
		pic[16] = size;
		pic[17] = size >> 8;
		pic[18] = size >> 16;
		pic[19] = size >> 24;
		if (l > 0)
			memmove(pic + 20, pic + 47, l);
		l += 20;
	} else if (version == 2) {
		/* write version 2 file */
		strcpy(pic, "BOOTSPL2");
		pic[8] = unit;
		pic[9] = state;
		pic[10] = fgcolor;
		pic[11] = bgcolor;
		if (cfgfile && size == 0
		    && (state != -1 || fgcolor != -1 || bgcolor != -1))
			size = -1;
		pic[12] = size;
		pic[13] = size >> 8;
		pic[14] = size >> 16;
		pic[15] = size >> 24;
		pic[16] = tx;
		pic[17] = tx >> 8;
		pic[18] = ty;
		pic[19] = ty >> 8;
		pic[20] = tw;
		pic[21] = tw >> 8;
		pic[22] = th;
		pic[23] = th >> 8;
		pic[24] = px;
		pic[25] = px >> 8;
		pic[26] = py;
		pic[27] = py >> 8;
		pic[28] = pw;
		pic[29] = pw >> 8;
		pic[30] = ph;
		pic[31] = ph >> 8;
		pic[32] = pr;
		pic[33] = pg;
		pic[34] = pb;
		if (l > 0)
			memmove(pic + 35, pic + 47, l);
		l += 35;
	} else {		/* version 3 */
		strcpy(pic, "BOOTSPL3");
		pic[8] = unit;
		pic[9] = state;
		pic[10] = fgcolor;
		pic[11] = bgcolor;
		size += boxcount * 12;
		if (cfgfile && size == 0
		    && (state != -1 || fgcolor != -1 || bgcolor != -1))
			size = -1;
		pic[12] = size;
		pic[13] = size >> 8;
		pic[14] = size >> 16;
		pic[15] = size >> 24;
		pic[16] = tx;
		pic[17] = tx >> 8;
		pic[18] = ty;
		pic[19] = ty >> 8;
		pic[20] = tw;
		pic[21] = tw >> 8;
		pic[22] = th;
		pic[23] = th >> 8;
		pic[24] = boxcount;
		pic[25] = boxcount >> 8;
		pic[28] = silentl;
		pic[29] = silentl >> 8;
		pic[30] = silentl >> 16;
		pic[31] = silentl >> 24;
		pic[32] = sboxcount;
		pic[33] = sboxcount >> 8;
		pic[34] = percent;
		pic[35] = percent >> 8;
		pic[36] = overpaintok;
		pic[37] = 0;	/* numpalette */
		if (l > 0)
			memmove(pic + 38 + boxcount * 12,
				pic + 47 + boxcount * 12, l);
		if (boxcount)
			memmove(pic + 38, boxes, boxcount * 12);
		l += 38 + boxcount * 12;
	}

	if (filterflag)
		fd = 1;
	else if ((fd = open(PROCSPLASH, O_WRONLY)) == -1) {
		perror(PROCSPLASH);
		exit(1);
	}
	if (write(fd, pic, l) != l) {
		perror("write");
		exit(1);
	}
	if (!filterflag)
		close(fd);
	free(pic);
}

int main(int argc, char *argv[])
{
	int procsplash, c;
	int setflag = 0;
	int errflag = 0;
	int filterflag = 0;
	int verbose_only = 0;
	unsigned char *readbuffer;

	while ((c = getopt(argc, argv, "fsu:n")) != EOF) {
		switch (c) {
		case 's':
			setflag = 1;
			break;
		case 'f':
			filterflag = 1;
			break;
		case 'u':
			unit = atoi(optarg);
			break;
		case 'n':
			verbose_only = 1;
			break;
		case '?':
			errflag = 1;
			break;
		}
	}

	if (errflag
	    || (argc != optind + 1 && (argc != optind || !setflag))) {
		printf("Usage: %s -s [-u unit] -n [cfgfile]\n", argv[0]);
		return 0;
	}

	readbuffer = malloc(BUFSIZE);

	if (!readbuffer) {
		DEBUG("splash: Not enough memory.\n");
		return -ENOMEM;
	}

	if (!setflag || !filterflag) {
		procsplash = open(PROCSPLASH, O_RDONLY);
		if (procsplash < 0) {
			if (setflag) {
				perror(PROCSPLASH);
				exit(1);
			}
			DEBUG("splash: File %s not available.\n",
			      PROCSPLASH);
			return 0;
		}

		if (read(procsplash, readbuffer, BUFSIZE) < 0) {
			if (setflag)
				perror("read");
			DEBUG("splash: Read error in file %s.\n",
			      PROCSPLASH);
			return -EBUSY;
		}
		close(procsplash);
	}
	if (setflag) {
		if (!filterflag
		    && !strncmp(readbuffer, "Splash screen v0", 16)) {
			fprintf(stderr,
				"Your kernel support for the bootsplash is too old. Get the new version from http://www.bootsplash.org/\n");
			exit(1);
		}
		setsplash(argc > optind ? argv[optind] : 0, filterflag,
			  verbose_only);
		exit(0);
	}

	return 0;
}
