/*
 * vo_lastpts.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_lastpts.c,v 1.2 2010/01/30 19:26:16 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/xine_internal.h>
#include <xine/video_out.h>
#include <xine/metronom.h>

#include "xvdr_metronom.h"

#include "vo_hook.h"

/*
 *  lastpts_hook_t
 */
typedef struct {
  vo_driver_hook_t h;
} lastpts_hook_t;

/*
 * interface
 */

/*
 * override display_frame()
 */

static void lastpts_display_frame(vo_driver_t *self, vo_frame_t *vo_img)
{
  lastpts_hook_t *this = (lastpts_hook_t*)self;

  if (vo_img->stream) {
    vo_img->stream->metronom->set_option(vo_img->stream->metronom, XVDR_METRONOM_LAST_VO_PTS, vo_img->pts);
  }

  this->h.orig_driver->display_frame(this->h.orig_driver, vo_img);
}


/*
 * init()
 */
vo_driver_t *vo_lastpts_init(void)
{
  lastpts_hook_t *this = calloc(1, sizeof(lastpts_hook_t));

  this->h.vo.display_frame = lastpts_display_frame;

  return &this->h.vo;
}

