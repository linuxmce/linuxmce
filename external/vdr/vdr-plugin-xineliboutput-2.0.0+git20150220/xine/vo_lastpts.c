/*
 * vo_lastpts.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: vo_lastpts.c,v 1.7 2012-03-10 20:56:30 phintuka Exp $
 *
 */

#include <stdlib.h>

#include <xine/xine_internal.h>
#include <xine/video_out.h>
#include <xine/metronom.h>

#include "xvdr_metronom.h"

#include "vo_hook.h"

#define LOG_MODULENAME "[lastpts  ] "
#include "../logdefs.h"

#include "vo_lastpts.h"

/*
 *  lastpts_hook_t
 */
typedef struct {
  vo_driver_hook_t  h;
  xine_stream_t    *prev_stream;
  xine_stream_t    *xvdr_stream;
  metronom_t       *xvdr_metronom;
} lastpts_hook_t;

static void detect_xvdr_metronom(lastpts_hook_t *this, xine_stream_t *stream)
{
  if (stream->metronom->get_option(stream->metronom, XVDR_METRONOM_ID) == XVDR_METRONOM_ID) {
    LOGDBG("new stream is vdr stream");
    this->xvdr_metronom = stream->metronom;
    this->xvdr_stream   = stream;
  }
}

/*
 * interface
 */

/*
 * override display_frame()
 */

static void lastpts_display_frame(vo_driver_t *self, vo_frame_t *vo_img)
{
  lastpts_hook_t *this = (lastpts_hook_t*)self;

  ASSERT_RET(self,   return);
  ASSERT_RET(vo_img, return);

  if (!vo_img->stream || vo_img->stream == XINE_ANON_STREAM || vo_img->pts <= 0) {
    this->h.orig_driver->display_frame(this->h.orig_driver, vo_img);
    return;
  }

  /* detect intercepted metronom with XVDR_* option support.
   * This prevents flooding log with "unknown option in set_option" messages
   */
  if (vo_img->stream != this->prev_stream) {
    LOGDBG("stream changed from %p to %p", this->prev_stream, vo_img->stream);
    this->prev_stream = vo_img->stream;

    detect_xvdr_metronom(this, vo_img->stream);
  }

  if (vo_img->stream == this->xvdr_stream) {

    if (this->xvdr_metronom != vo_img->stream->metronom) {
      LOGMSG("detected new metronom");
      this->xvdr_metronom = NULL;
      detect_xvdr_metronom(this, vo_img->stream);
    }

    if (this->xvdr_metronom) {
      ASSERT_RET(this->xvdr_metronom->set_option, return);

      LOGVERBOSE("last pts %"PRId64, vo_img->pts);

      this->xvdr_metronom->set_option(this->xvdr_metronom, XVDR_METRONOM_LAST_VO_PTS, vo_img->pts);
    }
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

