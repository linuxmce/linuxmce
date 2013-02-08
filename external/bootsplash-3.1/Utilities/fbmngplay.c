/*
 *	fbmngplay - fb console MNG player.
 *	(c) 2001-2002 by Stefan Reinauer, <stepan@suse.de>
 *  
 *	This program is based on mngplay, written and (C) by
 *	Ralph Giles <giles@ashlu.bc.ca>
 *
 *	This program my be redistributed under the terms of the
 *	GNU General Public Licence, version 2, or at your preference,
 *	any later version.
 */

#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <signal.h>

#include "fbmngplay.h"
#include "messages.h"
#include "console.h"
#include "mng.h"

volatile int run = 1;
int verbose = 0;
int buffered = 0;
int waitsignal = 0;
int delta = 16;
int sconly = 0;

/*
 * Signal handlers
 */

void sigint_handler(int sig)
{
	if (waitsignal)
		return;
	run = 2;
}

void sigterm_handler(int sig)
{
	if (waitsignal)
		return;
	restore_area();
	run = 0;
}

void sigusr1_handler(int sig)
{
	signal(SIGUSR1, SIG_IGN);
	run = 0;
}

/*
 * Main loop
 */

int main(int argc, char *argv[])
{
	int fbdev, c, option_index;
	unsigned int alpha;
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;

	/* Check which console we're running on */
	init_consoles();

	/* allocate our stream data structure */
	mng = (mngstuff *) calloc(1, sizeof(*mng));
	if (mng == NULL) {
		fprintf(stderr, "could not allocate stream structure.\n");
		exit(0);
	}

	alpha = 100;
	mng->alpha = 100;
	mng->fbx = 15;
	mng->fby = 15;
	mng->background = NULL;

	while (1) {
		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"verbose", 0, 0, 'v'},
			{"alpha", 1, 0, 'a'},
			{"buffered", 0, 0, 'b'},
			{"signal", 0, 0, 's'},
			{"delta", 0, 0, 'd'},
			{"version", 0, 0, 'V'},
			{"start-console", 0, 0, 'S'},
			{"console", 1, 0, 'c'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "a:x:y:bh?vsd:VSc:",
				long_options, &option_index);

		if (c == -1)
			break;

		switch (c) {
		case 'a':
			alpha = atoi(optarg);
			if (alpha > 100)
				alpha = 100;
			mng->alpha = alpha;
			break;
		case 'x':
			mng->fbx = atoi(optarg);
			break;
		case 'y':
			mng->fby = atoi(optarg);
			break;
		case 'd':
			delta = atoi(optarg);
			break;
		case '?':
		case 'h':
			usage(argv[0]);
			exit(0);
		case 'v':
			verbose = 1;
			break;
		case 's':
			waitsignal = 1;
			break;
		case 'b':
			buffered = 1;
			break;
		case 'V':
			version();
			exit(0);
		case 'c':
			start_console = atoi(optarg) - 1;
		case 'S':
			sconly = 1;
			break;
		default:
			break;
		}
	}
#if 0
	if (!output && optind >= argc) {
#else
	if (optind >= argc) {
#endif
		printf("Which files do you want to play?\n");
		exit(0);
	}

	/* Initialize framebuffer */
	fbdev = open("/dev/fb0", O_RDWR);
	if (fbdev < 0) {
		fprintf(stderr, "error while opening framebuffer.\n");
		exit(fbdev);
	}

	ioctl(fbdev, FBIOGET_VSCREENINFO, &var);
	mng->fbwidth = var.xres;
	mng->fbheight = var.yres;
	mng->fbbpp = var.bits_per_pixel;

	mng->fbredo = var.red.offset;
	mng->fbgreeno = var.green.offset;
	mng->fbblueo = var.blue.offset;
	mng->fbredl = var.red.length;
	mng->fbgreenl = var.green.length;
	mng->fbbluel = var.blue.length;

	/* Display might have a line length unequal
	 * the visible line.
	 */
	ioctl(fbdev, FBIOGET_FSCREENINFO, &fix);
	mng->fbrow = fix.line_length;

	mng->display =
	    mmap(NULL, mng->fbrow * mng->fbheight,
		 PROT_WRITE | PROT_READ, MAP_SHARED, fbdev, 0);
#if 0
	if (output)
		prepare_output();
#endif
	/* arrange to call the shutdown routine before we exit */
	atexit(&cleanup);

	while (optind < argc) {
		/* leftover arguements are filenames. */
		mng->filename = argv[optind++];

		/* set up the mng decoder for our stream */
		mng->mng =
		    mng_initialize(mng, mngalloc, mngfree, MNG_NULL);
		if (mng->mng == MNG_NULL) {
			fprintf(stderr, "could not initialize libmng.\n");
			exit(1);
		}

		/* set the callbacks */
		mng_setcb_errorproc(mng->mng, mngerror);
		mng_setcb_openstream(mng->mng, mngopenstream);
		mng_setcb_closestream(mng->mng, mngclosestream);
		mng_setcb_readdata(mng->mng, mngreadstream);
		mng_setcb_gettickcount(mng->mng, mnggetticks);
		mng_setcb_settimer(mng->mng, mngsettimer);
		mng_setcb_processheader(mng->mng, mngprocessheader);
		mng_setcb_getcanvasline(mng->mng, mnggetcanvasline);
		mng_setcb_refresh(mng->mng, mngrefresh);

		signal(SIGINT, sigint_handler);
		signal(SIGTERM, sigterm_handler);

		mng_readdisplay(mng->mng);

		/* loop though the frames */
		while (mng->delay && run) {
			usleep(mng->delay * 1000);
			mng->delay = 0;
			mng_display_resume(mng->mng);
			if (run == 2) {
				if (mng->alpha == 0)
					run = 0;
				mng->alpha -= delta;
				if (mng->alpha < 0)
					mng->alpha = 0;
			}
		}

		if (waitsignal && optind < argc) {
			signal(SIGUSR1, sigusr1_handler);
			run = 1;
			while (run) {
				sleep(2);
			}
		}

		memset(mng->copybuffer, 0, 4 * mng->width * mng->height);
		run = 1;
		mng->alpha = alpha;
		if (optind == argc) {	/* last file */
			restore_area();
		}
	}

	/* cleanup and quit */
	return mngquit(mng->mng);
}
