/*
 * xvdr_metronom.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xvdr_metronom.h,v 1.14 2012-03-26 18:49:34 phintuka Exp $
 *
 */

#ifndef XVDR_METRONOM_H
#define XVDR_METRONOM_H

#include <xine/metronom.h>

#ifdef METRONOM_INTERNAL
#  error METRONOM_INTERNAL defined, struct xvdr_metronom_s size will be incorrect
#endif

#define XVDR_METRONOM_OPTION_BASE  0x1001
#define XVDR_METRONOM_LAST_VO_PTS  (XVDR_METRONOM_OPTION_BASE)
#define XVDR_METRONOM_TRICK_SPEED  (XVDR_METRONOM_OPTION_BASE + 1)
#define XVDR_METRONOM_STILL_MODE   (XVDR_METRONOM_OPTION_BASE + 2)
#define XVDR_METRONOM_ID           (XVDR_METRONOM_OPTION_BASE + 3)

#define XVDR_METRONOM_LIVE_BUFFERING   (XVDR_METRONOM_OPTION_BASE + 4)
#define XVDR_METRONOM_STREAM_START     (XVDR_METRONOM_OPTION_BASE + 5)


typedef struct xvdr_metronom_s xvdr_metronom_t;

struct adjustable_scr_s;

struct xvdr_metronom_s {
  /* xine-lib metronom interface */
  metronom_t     metronom;

  /* management interface */
  void (*dispose)     (xvdr_metronom_t *);

  void (*wire)          (xvdr_metronom_t *);
  void (*unwire)        (xvdr_metronom_t *);

  /* master SCR for buffering control */
  struct adjustable_scr_s *scr;

  /* private data */

#ifdef XVDR_METRONOM_COMPILE

  /* original metronom */
  metronom_t    *orig_metronom;
  xine_stream_t *stream;

  int     trickspeed;    /* current trick speed */
  int     still_mode;
  int64_t last_vo_pts;   /* last displayed video frame PTS */
  int     wired;         /* true if currently wired to stream */

  /* initial buffering in live mode */
  uint8_t  buffering;      /* buffering active */
  uint8_t  live_buffering; /* live buffering enabled */
  uint8_t  stream_start;
  int64_t  vid_pts;        /* last seen video pts */
  int64_t  aud_pts;        /* last seen audio pts */
  int64_t  disc_pts;       /* reported discontinuity pts */
  uint64_t buffering_start_time;
  uint64_t first_frame_seen_time;

  pthread_mutex_t mutex;
#endif
};

xvdr_metronom_t *xvdr_metronom_init(xine_stream_t *);


#endif /* XVDR_METRONOM_H */
