/*
 * vo_osdreorder.c: OSD re-ordering video-out post plugin
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_osdreorder.c,v 1.2 2012-03-07 08:25:37 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/video_out.h>

#include "vo_props.h"
#include "vo_hook.h"

#include "vo_osdreorder.h"

/*
 *  osdreorder_hook_t
 */
typedef struct {
  vo_driver_hook_t h;

  /* currently showing OSDs in new drawing order */
  vo_overlay_t *overlay[50];

} osdreorder_hook_t;

/*
 *
 */

static int osd_level(vo_overlay_t *overlay)
{
  if (overlay->hili_rgb_clut != VDR_OSD_MAGIC /* not from vdr */) {
    return 9999;
  }

  /* VDR input plugin stores some control data in hili clut area */
  vdr_osd_extradata_t *data = (vdr_osd_extradata_t *)overlay->hili_color;
  return data->layer;
}

/*
 * interface
 */

/*
 * override overlay_blend()
 */
static void osdreorder_overlay_blend (vo_driver_t *self, vo_frame_t *frame, vo_overlay_t *overlay)
{
  osdreorder_hook_t *this = (osdreorder_hook_t*)self;
  int my_level = osd_level(overlay);
  int i;

  /* search position */
  for (i = 0; this->overlay[i] && osd_level(this->overlay[i]) >= my_level; i++)
    ;

  /* make room */
  if (this->overlay[i])
    memmove(&this->overlay[i+1], &this->overlay[i], (49-i)*sizeof(vo_overlay_t*));

  this->overlay[i] = overlay;
  return;
}

/*
 * override overlay_end()
 */
static void osdreorder_overlay_end (vo_driver_t *self, vo_frame_t *vo_img)
{
  osdreorder_hook_t *this = (osdreorder_hook_t*)self;
  int i;

  for (i = 0; this->overlay[i]; i++) {
    this->h.orig_driver->overlay_blend(this->h.orig_driver, vo_img, this->overlay[i]);
    this->overlay[i] = NULL;
  }

  /* redirect */
  if (this->h.orig_driver->overlay_end)
    this->h.orig_driver->overlay_end(this->h.orig_driver, vo_img);
}

/*
 * init()
 */
vo_driver_t *osd_reorder_init(void)
{
  osdreorder_hook_t *this = calloc(1, sizeof(osdreorder_hook_t));

  /* OSD interface */
  this->h.vo.overlay_blend    = osdreorder_overlay_blend;
  this->h.vo.overlay_end      = osdreorder_overlay_end;

  return &this->h.vo;
}

