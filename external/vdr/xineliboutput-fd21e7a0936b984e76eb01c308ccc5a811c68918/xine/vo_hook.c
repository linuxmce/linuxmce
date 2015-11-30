/*
 * vo_hook.c: Intercept video driver
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_hook.c,v 1.6 2011-03-10 10:03:11 durchflieger Exp $
 *
 */

#include <stdlib.h>

#include <xine/video_out.h>

#define LOG_MODULENAME "[xine-vo  ] "
#include "../logdefs.h"

#include "vo_hook.h"
#include "vo_post.h"

/* This module supports only video driver interface version 21 */
#if (VIDEO_OUT_DRIVER_IFACE_VERSION < 21)
#  error xine-lib VIDEO_OUT_DRIVER_IFACE_VERSION < 21
#endif


/*
 * default handlers
 *
 * - Forward function call to original driver
 */

#define DEF_HANDLER3(RET, NAME, ARG1, ARG2, ARG3)                    \
RET vo_def_##NAME (vo_driver_t *self, ARG1 a1, ARG2 a2, ARG3 a3) { \
  vo_driver_hook_t *this = (vo_driver_hook_t *) self;                \
  return this->orig_driver-> NAME (this->orig_driver, a1, a2, a3);   \
}

#define DEF_HANDLER2(RET, NAME, ARG1, ARG2)                      \
RET vo_def_##NAME (vo_driver_t *self, ARG1 a1, ARG2 a2) { \
  vo_driver_hook_t *this = (vo_driver_hook_t *) self;            \
  return this->orig_driver-> NAME (this->orig_driver, a1, a2);   \
}

#define DEF_HANDLER1(RET, NAME, ARG1)                       \
RET vo_def_##NAME (vo_driver_t *self, ARG1 a1) {     \
  vo_driver_hook_t *this = (vo_driver_hook_t *) self;       \
  return this->orig_driver-> NAME (this->orig_driver, a1);  \
}

#define DEF_HANDLER0(RET, NAME)                               \
RET vo_def_##NAME (vo_driver_t *self) {                \
  vo_driver_hook_t *this = (vo_driver_hook_t *) self;         \
  return this->orig_driver-> NAME (this->orig_driver);        \
}

/*
 *
 */

DEF_HANDLER0(uint32_t,    get_capabilities);
DEF_HANDLER0(vo_frame_t*, alloc_frame);

void vo_def_update_frame_format (vo_driver_t *self, vo_frame_t *img,
                                 uint32_t width, uint32_t height,
                                 double ratio, int format, int flags)
{
  vo_driver_hook_t *this = (vo_driver_hook_t *) self;
  return this->orig_driver-> update_frame_format (this->orig_driver, img, width, height, ratio, format, flags);
}

DEF_HANDLER1(void, display_frame,   vo_frame_t * );
DEF_HANDLER2(void, overlay_begin,   vo_frame_t *, int             );
DEF_HANDLER2(void, overlay_blend,   vo_frame_t *, vo_overlay_t *  );
DEF_HANDLER1(void, overlay_end,     vo_frame_t *);
DEF_HANDLER1(int,  get_property,         int);
DEF_HANDLER2(int,  set_property,         int, int);
DEF_HANDLER3(void, get_property_min_max, int, int*, int*);
DEF_HANDLER2(int,  gui_data_exchange,    int, void * );
DEF_HANDLER0(int,  redraw_needed );
#ifdef HAVE_XINE_GRAB_VIDEO_FRAME
DEF_HANDLER0(xine_grab_video_frame_t*,  new_grab_video_frame );
#endif

void vo_def_dispose(vo_driver_t *self)
{
  vo_driver_hook_t *this = (vo_driver_hook_t *) self;
  if (this->orig_driver)
    this->orig_driver->dispose(this->orig_driver);
  free(self);
}

/*
 * vo_def_hooks_init()
 *
 * initialize driver_hook_t with default handlers
 *
 */
static void vo_proxy_hooks_init(vo_driver_t *drv, vo_driver_t *next_driver)
{
  drv->get_capabilities     = drv->get_capabilities     ?: vo_def_get_capabilities;
  drv->alloc_frame          = drv->alloc_frame          ?: vo_def_alloc_frame;
  drv->update_frame_format  = drv->update_frame_format  ?: vo_def_update_frame_format;
  drv->display_frame        = drv->display_frame        ?: vo_def_display_frame;

  drv->get_property         = drv->get_property         ?: vo_def_get_property;
  drv->set_property         = drv->set_property         ?: vo_def_set_property;
  drv->get_property_min_max = drv->get_property_min_max ?: vo_def_get_property_min_max;
  drv->gui_data_exchange    = drv->gui_data_exchange    ?: vo_def_gui_data_exchange;
  drv->redraw_needed        = drv->redraw_needed        ?: vo_def_redraw_needed;
  drv->dispose              = drv->dispose              ?: vo_def_dispose;

#ifdef HAVE_XINE_GRAB_VIDEO_FRAME
  if (!drv->new_grab_video_frame && next_driver->new_grab_video_frame)
    drv->new_grab_video_frame = vo_def_new_grab_video_frame;
#endif

  /* drop old default handlers for OSD (OSD handlers are optional) */
  if (drv->overlay_begin == vo_def_overlay_begin)
    drv->overlay_begin = NULL;
  if (drv->overlay_blend == vo_def_overlay_blend)
    drv->overlay_blend = NULL;
  if (drv->overlay_end == vo_def_overlay_end)
    drv->overlay_end = NULL;

  /* Set proxy handlers for OSD only if next driver has OSD handlers */
  if (!drv->overlay_begin && next_driver->overlay_begin)
    drv->overlay_begin        = vo_def_overlay_begin;
  if (!drv->overlay_blend && next_driver->overlay_blend)
    drv->overlay_blend        = vo_def_overlay_blend;
  if (!drv->overlay_end && next_driver->overlay_end)
    drv->overlay_end          = vo_def_overlay_end;

  drv->node = next_driver->node; /* ??? used only by plugin_loader ? */
}

/*
 *
 */

/* from xine-lib video_out.c */
typedef struct {
  xine_video_port_t         vo; /* public part */
  vo_driver_t              *driver;
  /* ... */
} vos_t;

/*
 * wire_video_driver()
 *
 */
int wire_video_driver(xine_video_port_t *video_port, vo_driver_t *hook)
{
  vo_driver_t *vos_driver = ((vos_t*)video_port)->driver;

  if (video_port->driver != vos_driver) {
    LOGMSG("wire_video_driver() FAILED (vo_driver != vos_driver)");
    return 0;
  }

  /*LOGMSG("wire_video_driver: vo_driver == vos_driver");*/

  /* wire */

  /* set proxy handlers for undefined methods */
  vo_proxy_hooks_init(hook, video_port->driver);

  /* append original driver chain to new driver */
  ((vo_driver_hook_t *)hook)->orig_driver = video_port->driver;

  /* push new driver to start of driver chain */
  video_port->driver  = hook;
  ((vos_t*)video_port)->driver = hook;

  return 1;
}

/*
 * unwire_video_driver()
 *
 */
int unwire_video_driver(xine_video_port_t *video_port, vo_driver_t *hook_gen, vo_driver_t *video_out)
{
  vo_driver_hook_t *hook = (vo_driver_hook_t*)hook_gen;
  vo_driver_hook_t *next = (vo_driver_hook_t*)video_port->driver;

  if (next == hook) {
    /* special handling for first entry */
    video_port->driver           = next->orig_driver;
    /* need to patch video_port private driver pointer too ... */
    ((vos_t*)video_port)->driver = next->orig_driver;
    next->orig_driver            = NULL;
    return 1;
  }

  vo_driver_hook_t *prev = next;
  while (next && next != hook && (vo_driver_t*)next != video_out) {
    prev = next;
    next = (vo_driver_hook_t*)next->orig_driver;
  }

  if (prev && next == hook) {
    prev->orig_driver = next->orig_driver;
    next->orig_driver = NULL;
    return 1;
  }

  return 0;
}
