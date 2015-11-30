/*
 * xvdr_metronom.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xvdr_metronom.c,v 1.21 2012-03-26 18:49:34 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/xine_internal.h>
#include <xine/metronom.h>

#include "../tools/time_ms.h"

#define LOG_MODULENAME "[metronom ] "
#define SysLogLevel    iSysLogLevel
#include "../logdefs.h"

#include "adjustable_scr.h"

#define XVDR_METRONOM_COMPILE
#include "xvdr_metronom.h"

static int warnings = 0;

static int64_t absdiff(int64_t a, int64_t b) { int64_t diff = a-b; if (diff<0) diff = -diff; return diff; }
static int64_t min64(int64_t a, int64_t b) { return a < b ? a : b; }

static void check_buffering_done(xvdr_metronom_t *this)
{
  /* both audio and video timestamps seen ? */
  if (this->vid_pts && this->aud_pts) {
    int64_t da = this->aud_pts - this->disc_pts;
    int64_t dv = this->vid_pts - this->disc_pts;
    int64_t d_min = min64(da, dv);
    LOGMSG("  stream A-V diff %d ms", (int)(this->vid_pts - this->aud_pts)/90);
    LOGMSG("  reported stream start at pts %"PRId64, this->disc_pts);
    LOGMSG("  output fifo end at: audio %"PRId64" video %"PRId64, this->aud_pts, this->vid_pts);
    LOGMSG("  dA %"PRId64" dV %"PRId64, da, dv);
    if (d_min < 0 && d_min > -10*90000) {
      LOGMSG("  *** output is late %"PRId64" ticks (%"PRId64" ms) ***", d_min, -d_min/90);
      this->scr->jump(this->scr, d_min);
    }
    this->buffering = 0;
    this->stream_start = 0;
    this->scr->set_buffering(this->scr, 0);
    return;
  }

  if (this->first_frame_seen_time) {
    int64_t ms_since_first_frame = elapsed(this->first_frame_seen_time);

    if (ms_since_first_frame > 1000) {

      this->stream_start = 0;

      /* abort buffering if no audio */
      if (this->vid_pts && !this->aud_pts) {
	LOGMSG("buffering stopped: NO AUDIO ? elapsed time %d ms", (int)ms_since_first_frame);
	this->buffering = 0;
	this->scr->set_buffering(this->scr, 0);
	return;
      }

      /* abort buffering if no video */
      if (!this->vid_pts && this->aud_pts) {
	LOGMSG("buffering stopped: NO VIDEO ? elapsed time %d ms", (int)ms_since_first_frame);
	this->buffering = 0;
	this->scr->set_buffering(this->scr, 0);
	return;
      }
    }
  }
}

static void got_video_frame(metronom_t *metronom, vo_frame_t *frame)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;
  int64_t          pts  = frame->pts;

#if 1 /* xine-lib master-slave metronom causes some problems ... */
  if (metronom->got_video_frame != got_video_frame) {
    if (!warnings++)
      LOGMSG("got_video_frame: invalid object");
    return;
  }
  warnings = 0;
#endif

  if (this->still_mode) {
    LOGVERBOSE("Still frame, type %d", frame->picture_coding_type);
    frame->pts       = 0;
  }

  if (this->trickspeed) {
    frame->pts       = 0;
    frame->duration *= 12; /* GOP */
  }

  /* initial buffering */
  pthread_mutex_lock(&this->mutex);
  if (this->buffering && !frame->bad_frame) {

    /* track video pts */
    if (pts) {
      if (this->vid_pts && (absdiff(this->vid_pts, pts) > 5*90000)) {
        LOGMSG("buffering: video jump resetted audio pts");
        this->aud_pts = 0;
      }
      if (this->vid_pts && this->aud_pts && (absdiff(this->vid_pts, this->aud_pts) > 5*90000)) {
        LOGMSG("buffering: A-V diff resetted audio pts");
        this->aud_pts = 0;
      }
      if (!this->vid_pts) {
        LOGMSG("got video pts, frame type %d (@%d ms)", frame->picture_coding_type, (int)elapsed(this->buffering_start_time));
	this->first_frame_seen_time = time_ms(); 
      }
      this->vid_pts = pts;
    }

    /* some logging */
    if (!pts) {
      LOGMSG("got video, pts 0, buffering, frame type %d, bad_frame %d", frame->picture_coding_type, frame->bad_frame);
    }
    if (pts && !frame->pts) {
      LOGMSG("*** ERROR: hiding video pts while buffering ***");
    }

    check_buffering_done(this);
  }

  pthread_mutex_unlock(&this->mutex);

  this->orig_metronom->got_video_frame (this->orig_metronom, frame);

  frame->pts = pts;
}

static int64_t got_audio_samples(metronom_t *metronom, int64_t pts, int nsamples)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;

  pthread_mutex_lock(&this->mutex);

  /* initial buffering */
  if (this->buffering) {

    /* track audio pts */
    if (pts) {
      if (this->aud_pts && (this->aud_pts > pts || absdiff(pts, this->aud_pts) > 5*90000)) {
	LOGMSG("audio jump resetted video pts");
	this->vid_pts = 0;
      }
      if (this->aud_pts && this->vid_pts && (absdiff(this->vid_pts, this->aud_pts) > 5*90000)) {
	LOGMSG("buffering: A-V diff resetted video pts");
	this->vid_pts = 0;
      }
      if (!this->aud_pts) {
	LOGMSG("got audio pts (@%d ms)", (int)elapsed(this->buffering_start_time));
	this->first_frame_seen_time = time_ms();
      }
      this->aud_pts = pts;
    }

    /* some logging */
    if (!pts && !this->aud_pts) {
      LOGMSG("got audio, pts 0, buffering");
    }

    check_buffering_done(this);
  }

  pthread_mutex_unlock(&this->mutex);

  return this->orig_metronom->got_audio_samples (this->orig_metronom, pts, nsamples);
}

/*
 * dummy hooks
 */

static int64_t got_spu_packet(metronom_t *metronom, int64_t pts)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;
  return this->orig_metronom->got_spu_packet(this->orig_metronom, pts);
}

static void start_buffering(xvdr_metronom_t *this, int64_t disc_off)
{
  if (this->live_buffering && this->stream_start && disc_off) {
    if (!this->buffering) {
      LOGMSG("live mode buffering started (@%d ms)", (int)elapsed(this->buffering_start_time));

      this->aud_pts  = 0;
      this->vid_pts  = 0;
      this->disc_pts = disc_off;

      this->first_frame_seen_time = 0;

      this->buffering = 1;
      this->scr->set_buffering(this->scr, 1);
    }
  } else {
    if (this->buffering) {
      LOGMSG("live mode buffering aborted (@%d ms)", (int)elapsed(this->buffering_start_time));
      this->buffering = 0;
      this->scr->set_buffering(this->scr, 0);
    }
  }
}

static void handle_audio_discontinuity(metronom_t *metronom, int type, int64_t disc_off)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;

  start_buffering(this, disc_off);

  this->orig_metronom->handle_audio_discontinuity(this->orig_metronom, type, disc_off);
}

static void handle_video_discontinuity(metronom_t *metronom, int type, int64_t disc_off)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;

  start_buffering(this, disc_off);

  this->orig_metronom->handle_video_discontinuity(this->orig_metronom, type, disc_off);
}

static void set_audio_rate(metronom_t *metronom, int64_t pts_per_smpls)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;
  this->orig_metronom->set_audio_rate(this->orig_metronom, pts_per_smpls);
}

static void set_option(metronom_t *metronom, int option, int64_t value)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;

  if (option == XVDR_METRONOM_LAST_VO_PTS) {
    if (value > 0) {
      pthread_mutex_lock(&this->mutex);
      this->last_vo_pts = value;
      pthread_mutex_unlock(&this->mutex);
    }
    return;
  }

  if (option == XVDR_METRONOM_LIVE_BUFFERING) {
    pthread_mutex_lock(&this->mutex);
    this->live_buffering = value;
    pthread_mutex_unlock(&this->mutex);
    return;
  }

  if (option == XVDR_METRONOM_STREAM_START) {
    pthread_mutex_lock(&this->mutex);
    this->stream_start = 1;
    this->buffering_start_time = time_ms();
    pthread_mutex_unlock(&this->mutex);
    return;
  }

  if (option == XVDR_METRONOM_TRICK_SPEED) {
    this->trickspeed = value;
    return;
  }

  if (option == XVDR_METRONOM_STILL_MODE) {
    this->still_mode = value;
    return;
  }

  this->orig_metronom->set_option(this->orig_metronom, option, value);
}

static int64_t get_option(metronom_t *metronom, int option)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;

  if (option == XVDR_METRONOM_LAST_VO_PTS) {
    int64_t pts;
    pthread_mutex_lock(&this->mutex);
    pts = this->last_vo_pts;
    pthread_mutex_unlock(&this->mutex);
    return pts;
  }
  if (option == XVDR_METRONOM_TRICK_SPEED) {
    return this->trickspeed;
  }
  if (option == XVDR_METRONOM_STILL_MODE) {
    return this->still_mode;
  }
  if (option == XVDR_METRONOM_ID) {
    return XVDR_METRONOM_ID;
  }

  return this->orig_metronom->get_option(this->orig_metronom, option);
}

static void set_master(metronom_t *metronom, metronom_t *master)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;
  this->orig_metronom->set_master(this->orig_metronom, master);
}

static void metronom_exit(metronom_t *metronom)
{
  xvdr_metronom_t *this = (xvdr_metronom_t *)metronom;

  LOGMSG("xvdr_metronom: metronom_exit() called !");

  /* un-hook */
  this->unwire(this);
  this->stream = NULL;

  if (this->orig_metronom) {
    metronom_t *orig_metronom = this->orig_metronom;
    this->orig_metronom = NULL;

    orig_metronom->exit(orig_metronom);
  }
}

/*
 * xvdr_metronom_t
 */

static void xvdr_metronom_wire(xvdr_metronom_t *this)
{
  if (!this->stream) {
    LOGMSG("xvdr_metronom_wire(): stream == NULL !");
    return;
  }
  if (!this->stream->metronom) {
    LOGMSG("xvdr_metronom_wire(): stream->metronom == NULL !");
    return;
  }

  if (!this->wired) {
    this->wired = 1;

    /* attach to stream */
    this->orig_metronom = this->stream->metronom;
    this->stream->metronom = &this->metronom;
  }
}

static void xvdr_metronom_unwire(xvdr_metronom_t *this)
{
  if (this->stream && this->wired) {
    this->wired = 0;

    /* detach from stream */
    this->stream->metronom = this->orig_metronom;
  }
}

static void xvdr_metronom_dispose(xvdr_metronom_t *this)
{
  xvdr_metronom_unwire(this);

  pthread_mutex_destroy(&this->mutex);

  free(this);
}

/*
 * init
 */

xvdr_metronom_t *xvdr_metronom_init(xine_stream_t *stream)
{
  if (stream->metronom->get_option(stream->metronom, XVDR_METRONOM_ID) == XVDR_METRONOM_ID) {
    LOGMSG("xvdr_metronom_init(): stream already hooked !");
    return (xvdr_metronom_t*)stream->metronom;
  }

  xvdr_metronom_t *this = calloc(1, sizeof(xvdr_metronom_t));

  this->stream        = stream;
  this->orig_metronom = stream->metronom;

  this->wire           = xvdr_metronom_wire;
  this->unwire         = xvdr_metronom_unwire;
  this->dispose        = xvdr_metronom_dispose;

  this->metronom.set_audio_rate    = set_audio_rate;
  this->metronom.got_video_frame   = got_video_frame;
  this->metronom.got_audio_samples = got_audio_samples;
  this->metronom.got_spu_packet    = got_spu_packet;
  this->metronom.handle_audio_discontinuity = handle_audio_discontinuity;
  this->metronom.handle_video_discontinuity = handle_video_discontinuity;
  this->metronom.set_option = set_option;
  this->metronom.get_option = get_option;
  this->metronom.set_master = set_master;

  this->metronom.exit = metronom_exit;

  pthread_mutex_init(&this->mutex, NULL);

  xvdr_metronom_wire(this);

  return this;
}
