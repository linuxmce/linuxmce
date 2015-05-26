/*
 * vo_hook.h: Intercept video driver
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_hook.h,v 1.3 2008-12-14 01:14:54 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_VO_HOOK_H
#define _XINELIBOUTPUT_VO_HOOK_H

#include <xine/video_out.h>

/*
 * synchronous video post plugins
 * internal API
 */


/*
 * vo_driver_hook_t
 *
 * Used as base for video driver hooks
 */

typedef struct driver_hook_s {
  vo_driver_t     vo;             /* public part */
  vo_driver_t    *orig_driver;
} vo_driver_hook_t;

/* proxy handlers: forward call to original driver */

uint32_t vo_def_get_capabilities(vo_driver_t *self);
int      vo_def_get_property(vo_driver_t *self, int prop);
int      vo_def_set_property(vo_driver_t *self, int prop, int val);

void     vo_def_dispose(vo_driver_t *self);


#endif /* _XINELIBOUTPUT_VO_HOOK_H */
