/*
 * xine_frontend_internal.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: xine_frontend_internal.h,v 1.11 2012-03-19 11:51:21 phintuka Exp $
 *
 */

#ifndef XINE_FRONTEND_INTERNAL_H
#define XINE_FRONTEND_INTERNAL_H

#include <xine.h>
#include <xine/input_plugin.h>

#include "xine_frontend.h"
#include "xine_input_vdr.h"
#include "xine/post.h"

typedef struct fe_s {
  /* base class */
  frontend_t          fe;

  /* from xine_frontend.c */
  double (*dest_pixel_aspect)   (const struct fe_s *, 
				 double video_pixel_aspect,
				 int video_width, int video_height);
  void   (*frame_output_handler)(void *data,
				 int video_width, int video_height,
				 double video_pixel_aspect,
				 int *dest_x, int *dest_y,
				 int *dest_width, int *dest_height,
				 double *dest_pixel_aspect,
				 int *win_x, int *win_y);

  /* called from xine_frontend.c */
  void   (*update_display_size_cb) (struct fe_s *);
  void   (*toggle_fullscreen_cb)   (struct fe_s *, int);

  /* if set before xine_init(), will be called by video driver wrapper for each frame */
  void   (*frame_draw_cb)(void *, vo_frame_t *);

  /* xine stuff */
  xine_t             *xine;
  xine_stream_t      *stream;
  xine_stream_t      *slave_stream;
  vdr_input_plugin_if_t *input_plugin;
  xine_video_port_t  *video_port;
  xine_video_port_t  *video_port_none;
  xine_audio_port_t  *audio_port;
  xine_audio_port_t  *audio_port_none;
  xine_event_queue_t *event_queue;

  post_plugins_t     *postplugins;
  char               *video_port_name;   /* frame buffer device */
  char               *aspect_controller; /* path to external HW aspect ratio controller */
  char               *configfile;        /* path of our config file */

  int                 xine_visual_type;
  union {
    void             *vis;
    fb_visual_t       vis_fb;
    x11_visual_t      vis_x11;
    raw_visual_t      vis_raw;
  };

  /* frontend */
  double      video_aspect;    /* aspect ratio of video frame */
  double      display_ratio;   /* aspect ratio of video window */
  uint        terminate_key_pressed;
  uint16_t    xpos, ypos;      /* position of video window */
  uint16_t    width;           /* size of video window */
  uint16_t    height;          /* */
  uint16_t    video_width;     /* size of video frame */
  uint16_t    video_height;    /* */
  uint16_t    pes_buffers;     /* max. number of PES packets in video fifo */
  uint8_t     aspect;          /* aspect ratio of video window (user setting) */
  uint8_t     overscan;        /* overscan in % (crop video borders) */
/*uint8_t     cropping : 1;*/
  uint8_t     scale_video : 1; /* enable/disable all video scaling */
  uint8_t     playback_finished : 1;
  uint8_t     slave_playback_finished : 1;

  char       *shutdown_cmd;
  int         shutdown_timeout;
  time_t      shutdown_time;
} fe_t;

/* setup function pointers */
void init_fe(fe_t *fe);

char *strn0cpy(char *dest, const char *src, int n);

#endif /* XINE_FRONTEND_INTERNAL_H */
