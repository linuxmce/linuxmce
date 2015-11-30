/*
 * vo_frameoutput.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_frameoutput.c,v 1.1 2012-03-07 08:27:40 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/xine_internal.h>
#include <xine/video_out.h>

#include "vo_hook.h"

#define LOG_MODULENAME "[frame_out] "
#include "../logdefs.h"

#include "vo_frameoutput.h"


/*
 *  frameoutput_hook_t
 */
typedef struct {
  vo_driver_hook_t h;

  /* callback */
  void *handle;
  void (*cb)(void *, vo_frame_t *);

} frameoutput_hook_t;

/*
 * interface
 */

/*
 * override display_frame()
 */

static void display_frame(vo_driver_t *self, vo_frame_t *vo_img)
{
  frameoutput_hook_t *this = (frameoutput_hook_t*)self;

  ASSERT_RET(self,   return);
  ASSERT_RET(vo_img, return);

  if (this->cb)
    this->cb(this->handle, vo_img);

  this->h.orig_driver->display_frame(this->h.orig_driver, vo_img);
}


/*
 * init()
 */
vo_driver_t *vo_frameoutput_init(void *handle, void (*cb)(void*, vo_frame_t*))
{
  frameoutput_hook_t *this = calloc(1, sizeof(frameoutput_hook_t));

  this->h.vo.display_frame = display_frame;

  this->handle = handle;
  this->cb     = cb;

  return &this->h.vo;
}

