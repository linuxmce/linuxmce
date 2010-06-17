/*
 * xvdr_metronom.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xvdr_metronom.h,v 1.6 2010/02/19 13:44:57 phintuka Exp $
 *
 */

#ifndef XVDR_METRONOM_H
#define XVDR_METRONOM_H


#define XVDR_METRONOM_LAST_VO_PTS  0x1001
#define XVDR_METRONOM_TRICK_SPEED  0x1002
#define XVDR_METRONOM_STILL_MODE   0x1003


typedef struct xvdr_metronom_s xvdr_metronom_t;

struct xvdr_metronom_s {
  /* xine-lib metronom interface */
  metronom_t     metronom;

  /* management interface */
  void (*set_cb)      (xvdr_metronom_t *,
                       void (*cb) (void *, uint, uint),
                       void *);
  void (*reset_frames)(xvdr_metronom_t *);
  void (*dispose)     (xvdr_metronom_t *);

  void (*set_trickspeed)(xvdr_metronom_t *, int);
  void (*set_still_mode)(xvdr_metronom_t *, int);

  void (*wire)          (xvdr_metronom_t *);
  void (*unwire)        (xvdr_metronom_t *);

  /* accumulated frame data */
  volatile uint video_frames;
  volatile uint audio_frames;

  /* private data */

#ifdef XVDR_METRONOM_COMPILE

  /* original metronom */
  metronom_t    *orig_metronom;
  xine_stream_t *stream;

  /* callback */
  void *handle;
  void (*frame_decoded)(void *handle, uint video_count, uint audio_count);

  int     trickspeed;    /* current trick speed */
  int     still_mode;
  int64_t last_vo_pts;   /* last displayed video frame PTS */
  int     wired;         /* true if currently wired to stream */
#endif
};

xvdr_metronom_t *xvdr_metronom_init(xine_stream_t *);


#endif /* XVDR_METRONOM_H */
