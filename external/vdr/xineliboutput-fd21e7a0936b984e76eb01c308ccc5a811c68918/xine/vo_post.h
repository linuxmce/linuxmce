/*
 * vo_post.h: Intercept video driver
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_post.h,v 1.3 2008-12-14 01:19:21 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_VO_POST_H
#define _XINELIBOUTPUT_VO_POST_H

#include <xine/video_out.h>

/*
 * synchronous video post plugins
 * public API
 */

/* Wire / unwire hook chain to video port */
int  wire_video_driver(xine_video_port_t *video_port, vo_driver_t *hook);
int  unwire_video_driver(xine_video_port_t *video_port, vo_driver_t *hook, vo_driver_t *video_out);

#endif /* _XINELIBOUTPUT_VO_POST_H */
