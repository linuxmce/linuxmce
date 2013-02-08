/*
 * framebuffer tool, inspired from fbi ((c) 1998,99 Gerd Knorr <kraxel@goldbach.in-berlin.de>)
 *
 * (c) 2002 Florent Villard (warly@mandrakesoft.com)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#define VERSION "0.1.1"

int fd;
static int fb;
struct fb_var_screeninfo fb_var;

struct option fbresol_options[] = {
	{"version", no_argument, NULL, 'v'},	/* version */
	{"help", no_argument, NULL, 'h'},	/* help */
	{0, 0, 0, 0}
};

void version(void)
{
	fprintf(stderr, "fbresolution version " VERSION
		" (c) 2002 Florent Villard <warly@mandrakesoft.com>; compiled on %s.\n",
		__DATE__);
}

void usage(char *name)
{
	char *h;
	h = strrchr(name, '/');
	fprintf(stderr,
		"\n"
		"This program output the current framebuffer resolution.\n"
		"\n"
		"  Usage: %s [ options ]\n"
		"\n"
		"    --help       [-h]      Print this text\n"
		"    --version    [-v]      Show the %s version number\n"
		"\n" "\n", h ? h + 1 : name, h ? h + 1 : name);
}

int main(int argc, char *argv[])
{
	int opt_index = 0;
	int c;
	for (;;) {
		c = getopt_long(argc, argv, "vhnd:t:", fbresol_options,
				&opt_index);
		if (c == -1)
			break;
		switch (c) {
		case 'v':
			version();
			exit(1);
			break;
		default:
		case 'h':
			usage(argv[0]);
			exit(1);
		}
	}
	if (optind > argc) {
		usage(argv[0]);
		exit(1);
	}
	if (-1 == (fb = open("/dev/fb0", O_RDWR /* O_WRONLY */ ))) {
		fprintf(stderr, "open /dev/fb0: %s\n", strerror(errno));
		exit(1);
	}
	if (-1 == ioctl(fb, FBIOGET_VSCREENINFO, &fb_var)) {
		perror("ioctl FBIOGET_VSCREENINFO");
		exit(1);
	}
	printf("%dx%d\n", fb_var.xres, fb_var.yres);
	return 0;
}
