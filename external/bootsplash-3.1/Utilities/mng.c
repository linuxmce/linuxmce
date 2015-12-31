/*
 *	fbmngplay - fb console MNG player.
 *	(c) 2001-2002 by Stefan Reinauer, <stepan@suse.de>
 *
 *	This program is based on mngplay, part of libmng, written and (C) by
 *	Ralph Giles <giles@ashlu.bc.ca>
 *
 *	This program my be redistributed under the terms of the
 *	GNU General Public Licence, version 2, or at your preference,
 *	any later version.
 */

#include <unistd.h>
#include <sys/time.h>

#include "fbmngplay.h"
#include "console.h"
#include "mng.h"

mngstuff *mng;
unsigned char *bufferstream;
unsigned long bufferpos = 0, buffersize = 0;

/* 
 * callbacks for the mng decoder 
 */

/* memory allocation; data must be zeroed */
mng_ptr mngalloc(mng_uint32 size)
{
	return (mng_ptr) calloc(1, size);
}

/* memory deallocation */
void mngfree(mng_ptr p, mng_uint32 size)
{
	free(p);
	return;
}

mng_bool mngopenstream(mng_handle mng)
{
	mngstuff *mymng;

	/* look up our stream struct */
	mymng = (mngstuff *) mng_get_userdata(mng);

	/* open the file */
	mymng->file = fopen(mymng->filename, "rb");
	if (mymng->file == NULL) {
		fprintf(stderr, "unable to open '%s'\n", mymng->filename);
		run = 0;
		return MNG_FALSE;
	}

	if (buffered) {
		unsigned long len;
		fseek(mymng->file, 0, SEEK_END);
		len = ftell(mymng->file);
		rewind(mymng->file);
		bufferstream = malloc(len);
		if (!bufferstream) {
			/* Not enough memory for buffers 
			 * -> we go back to unbuffered mode
			 */
			printf("Reverted to non buffered mode.\n");
			buffered = 0;
			return MNG_TRUE;
		}
		buffersize = len;
		fread(bufferstream, 1, len, mymng->file);
		bufferpos = 0;
		fclose(mymng->file);
		mymng->file = NULL;
	}

	return MNG_TRUE;
}

mng_bool mngclosestream(mng_handle mng)
{
	mngstuff *mymng;

	/* look up our stream struct */
	mymng = (mngstuff *) mng_get_userdata(mng);

	/* close the file */
	if (mymng->file)
		fclose(mymng->file);
	mymng->file = NULL;	/* for safety */

	if (bufferstream) {
		free(bufferstream);
		bufferstream = 0;
		buffersize = 0;
		bufferpos = 0;
	}
	return MNG_TRUE;
}

/* feed data to the decoder */
mng_bool mngreadstream(mng_handle mng, mng_ptr buffer,
		       mng_uint32 size, mng_uint32 * bytesread)
{
	mngstuff *mymng;

	/* look up our stream struct */
	mymng = (mngstuff *) mng_get_userdata(mng);
	if (!buffered) {
		/* read the requested amount of data from the file */
		*bytesread = fread(buffer, 1, size, mymng->file);
	} else {
		*bytesread = (buffersize - bufferpos) <
		    size ? (buffersize - bufferpos) : size;
		memcpy(buffer, bufferstream + bufferpos, *bytesread);
		bufferpos += (*bytesread);
	}
	return MNG_TRUE;
}

mng_bool mnggetbackgroundbuffer(mng_handle mng)
{
	unsigned char *background, *src;
	mngstuff *mymng = mng_get_userdata(mng);
	mng_uint32 width = mymng->width, height = mymng->height;
	int bytes = (mymng->fbbpp >> 3);

	if (mymng->background)
		return MNG_TRUE;

	/* If we're not on the right terminal, don't 
	 * initialize background yet.
	 */
	if (sconly && current_console() != start_console)
		return MNG_FALSE;

	background = (unsigned char *) malloc(width * height * bytes);
	if (background == NULL) {
		fprintf(stderr, "could not allocate background buffer.\n");
		exit(0);
	}

	mymng->background = background;
	src =
	    mymng->display + (mymng->fbwidth * mymng->fby +
			      mymng->fbx) * bytes;

	while (height--) {
		memcpy(background, src, width * bytes);
		background += width * bytes;
		src += mymng->fbrow;
	}

	return MNG_TRUE;
}

/* the header's been read. set up the display stuff */
mng_bool mngprocessheader(mng_handle mng,
			  mng_uint32 width, mng_uint32 height)
{
	mngstuff *mymng;
	unsigned char *copybuffer;

	mymng = (mngstuff *) mng_get_userdata(mng);
	mymng->width = width;
	mymng->height = height;

	copybuffer = (unsigned char *) malloc(width * height * 4);
	if (copybuffer == NULL) {
		fprintf(stderr, "could not allocate copy buffer.\n");
		exit(0);
	}
	mymng->copybuffer = copybuffer;

	/* Try to get background buffer */
	mnggetbackgroundbuffer(mng);

	/* tell the mng decoder about our bit-depth choice */
	/* FIXME: this works on intel. is it correct in general? */
	mng_set_canvasstyle(mng, MNG_CANVAS_BGRA8);

	return MNG_TRUE;
}

/* return a row pointer for the decoder to fill */
mng_ptr mnggetcanvasline(mng_handle mng, mng_uint32 line)
{
	mngstuff *mymng;
	mng_ptr row;

	/* dereference our structure */
	mymng = (mngstuff *) mng_get_userdata(mng);

	/* we assume any necessary locking has happened 
	   outside, in the frame level code */
	row = mymng->copybuffer + mymng->width * 4 * line;

	return (row);
}

/* timer */
mng_uint32 mnggetticks(mng_handle mng)
{
	mng_uint32 ticks;
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	ticks = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

	return (ticks);
}

static inline void copyline(unsigned char *dest, unsigned char *src,
			    unsigned char *background, mngstuff * mymng)
{
	// BGRA8        
	unsigned int i = mymng->width;
	unsigned int fr, fg, fb, br, bg, bb, r, g, b, a;
	unsigned short output, input;

	while (i--) {
		fb = *src++;
		fg = *src++;
		fr = *src++;

		a = *src++;
		a = a * mymng->alpha / 100;
		switch (mymng->fbbpp) {
		case 16:
			input = *((unsigned short *) background)++;

			br = (input >> mng->fbredo) << (8 - mng->fbredl);
			bg = (input >> mng->fbgreeno) << (8 -
							  mng->fbgreenl);
			bb = (input >> mng->fbblueo) << (8 - mng->fbbluel);
			br &= 0xf8;
			bg &= 0xfc;
			bb &= 0xff;
#if 0
			br = (input >> 8) & 0xf8;
			bg = (input >> 3) & 0xfc;
			bb = input << 3 & 0xff;
#endif
			break;
		case 24:
			bb = *background++;
			bg = *background++;
			br = *background++;
			break;
		case 32:
			bb = *background++;
			bg = *background++;
			br = *background++;
			background++;
			break;
		default:
			br = 0;
			bg = 0;
			bb = 0;
			printf("depth not supported.\n");
			run = 0;
			break;
		}

		r = ((fr * a) + (br * (0x100 - a))) >> 8;
		g = ((fg * a) + (bg * (0x100 - a))) >> 8;
		b = ((fb * a) + (bb * (0x100 - a))) >> 8;

		switch (mymng->fbbpp) {
		case 16:
			// dumb 24->16 bit conversion.
			r >>= (8 - mng->fbredl);
			g >>= (8 - mng->fbgreenl);
			b >>= (8 - mng->fbbluel);

			output =
			    (r << mng->fbredo) | (g << mng->
						  fbgreeno) | (b << mng->
							       fbblueo);

			*((unsigned short *) dest)++ = output;
			break;
		case 24:
			*dest++ = b;
			*dest++ = g;
			*dest++ = r;
			break;
		case 32:
			*dest++ = b;
			*dest++ = g;
			*dest++ = r;
			dest++;
			break;
		default:
			break;
		}
	}
}

mng_bool mngrefresh(mng_handle mng, mng_uint32 x, mng_uint32 y,
		    mng_uint32 w, mng_uint32 h)
{
	mngstuff *mymng = mng_get_userdata(mng);
	unsigned char *background;
	unsigned char *dest, *src;
	int bytes = (mymng->fbbpp >> 3);

	if (sconly && current_console() != start_console)
		return MNG_TRUE;

	/* When we read the header, we might still
	 * have been on a different console
	 */

	if (!(mymng->background))
		mnggetbackgroundbuffer(mng);

	background = mymng->background;

	dest =
	    mymng->display + (mymng->fby * mymng->fbwidth +
			      mymng->fbx) * bytes;
	src = mymng->copybuffer;

	/* refresh the screen with the new frame */
	while (h-- > 0) {
		copyline(dest, src, background, mymng);

		dest += mymng->fbrow;
		background += mymng->width * bytes;
		/* 4 bytes per pixel due to RGBA */
		src += 4 * mymng->width;
	}

	/* remove traces in alpha transparent pictures. */
	memset(mymng->copybuffer, 0, 4 * mymng->width * mymng->height);

	return MNG_TRUE;
}

/* interframe delay callback */
mng_bool mngsettimer(mng_handle mng, mng_uint32 msecs)
{
	mngstuff *mymng;

	/* look up our stream struct */
	mymng = (mngstuff *) mng_get_userdata(mng);

	/* set the timer for when the decoder wants to be woken */
	mymng->delay = msecs;

	return MNG_TRUE;
}

mng_bool mngerror(mng_handle mng, mng_int32 code, mng_int8 severity,
		  mng_chunkid chunktype, mng_uint32 chunkseq,
		  mng_int32 extra1, mng_int32 extra2, mng_pchar text)
{
	mngstuff *mymng;
	char chunk[5];

	/* dereference our data so we can get the filename */
	mymng = (mngstuff *) mng_get_userdata(mng);
	/* pull out the chuck type as a string */
	// FIXME: does this assume unsigned char?
	chunk[0] = (char) ((chunktype >> 24) & 0xFF);
	chunk[1] = (char) ((chunktype >> 16) & 0xFF);
	chunk[2] = (char) ((chunktype >> 8) & 0xFF);
	chunk[3] = (char) ((chunktype) & 0xFF);
	chunk[4] = '\0';

	/* output the error */
	fprintf(stderr, "error playing '%s' chunk %s (%d):\n",
		mymng->filename, chunk, chunkseq);
	fprintf(stderr, "%s\n", text);

	return 0;
}

int mngquit(mng_handle mng)
{
	mngstuff *mymng;

	/* dereference our data so we can free it */
	mymng = (mngstuff *) mng_get_userdata(mng);

	/* cleanup. this will call mymngclosestream */
	mng_cleanup(&mng);

	/* free our data */
	free(mymng);

	exit(0);
}

void cleanup(void)
{
	mngquit(mng->mng);
	exit(0);
}

void restore_area(void)
{
	int height, width;
	unsigned char *dest, *background;

	if (sconly && current_console() != start_console)
		return;

	/* when we didn't manage to get a background until 
	 * now, we don't have anything to restore anyways.
	 */

	if (!(mng->background))
		return;

	background = mng->background;
	height = mng->height;
	width = mng->width;
	dest = mng->display + ((mng->fbwidth * mng->fby + mng->fbx) *
			       (mng->fbbpp >> 3));

	while (height--) {
		memcpy(dest, background, width * (mng->fbbpp >> 3));
		background += width * (mng->fbbpp >> 3);
		dest += mng->fbrow;
	}
}
