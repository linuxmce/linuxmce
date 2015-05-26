/*
 * adjustable_scr.c:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: adjustable_scr.c,v 1.5 2012-03-22 12:03:37 phintuka Exp $
 *
 */


#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include <xine/metronom.h>

#include "../tools/time_ms.h"

#define LOG_MODULENAME "[scr      ] "
#define SysLogLevel    iSysLogLevel
#include "../logdefs.h"

#include "adjustable_scr.h"

/*
 * SCR code is mostly copied from xine-lib (src/input/input_pvr.c)
 *
 *
 * Copyright (C) 2000-2005 the xine project
 * March 2003 - Miguel Freitas
 * This plugin was sponsored by 1Control
 *
 * This file is part of xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 */

typedef struct scr_impl_s scr_impl_t;

struct scr_impl_s {
  union {
    scr_plugin_t     scr;
    adjustable_scr_t ascr;
  };

  xine_t          *xine;

  struct timeval   cur_time;
  int64_t          cur_pts;
  int              xine_speed;
  int              scr_speed_base;
  double           speed_factor;
  double           speed_tuning;

  int              buffering; /* clock is freezed while buffering */
  uint64_t         buffering_start_time;

  pthread_mutex_t  lock;
};

/* Only call set_pivot when already mutex locked ! */
static void set_pivot (scr_impl_t *this)
{
  struct   timeval tv;
  int64_t pts;
  double   pts_calc;

  if (this->buffering) {
    xine_monotonic_clock(&this->cur_time, NULL);
    return;
  }

  xine_monotonic_clock(&tv,NULL);

  pts_calc = (tv.tv_sec  - this->cur_time.tv_sec) * this->speed_factor;
  pts_calc += (tv.tv_usec - this->cur_time.tv_usec) * this->speed_factor / 1e6;
  pts = this->cur_pts + pts_calc;

  /* This next part introduces a one off inaccuracy
   * to the scr due to rounding tv to pts.
   */
  this->cur_time.tv_sec=tv.tv_sec;
  this->cur_time.tv_usec=tv.tv_usec;
  this->cur_pts=pts;
}

/*
 * xine interface (scr_plugin_t)
 */

static int scr_get_priority (scr_plugin_t *scr)
{
  return 50; /* high priority */
}

static int scr_set_fine_speed (scr_plugin_t *scr, int speed)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_lock (&this->lock);

  set_pivot( this );
  this->xine_speed     = speed;
  this->speed_factor   = (double) speed * (double)this->scr_speed_base /*90000.0*/ /
                         (1.0*XINE_FINE_SPEED_NORMAL) *
                         this->speed_tuning;

  pthread_mutex_unlock (&this->lock);

  return speed;
}

static void scr_adjust (scr_plugin_t *scr, int64_t vpts)
{
  scr_impl_t *this = (scr_impl_t*) scr;
  struct   timeval tv;

  pthread_mutex_lock (&this->lock);

  xine_monotonic_clock(&tv,NULL);
  this->cur_time.tv_sec=tv.tv_sec;
  this->cur_time.tv_usec=tv.tv_usec;
  this->cur_pts = vpts;

  pthread_mutex_unlock (&this->lock);
}

static void scr_start (scr_plugin_t *scr, int64_t start_vpts)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_lock (&this->lock);

  xine_monotonic_clock(&this->cur_time, NULL);
  this->cur_pts = start_vpts;

  pthread_mutex_unlock (&this->lock);

  scr_set_fine_speed (&this->scr, XINE_FINE_SPEED_NORMAL);
}

static int64_t scr_get_current (scr_plugin_t *scr)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  struct   timeval tv;
  int64_t pts;
  double   pts_calc;

  pthread_mutex_lock (&this->lock);

  pts = this->cur_pts;

  if (this->buffering) {
    pthread_mutex_unlock (&this->lock);
    return pts;
  }

  xine_monotonic_clock(&tv,NULL);

  pts_calc = (tv.tv_sec  - this->cur_time.tv_sec) * this->speed_factor;
  pts_calc += (tv.tv_usec - this->cur_time.tv_usec) * this->speed_factor / 1e6;

  pts += pts_calc;

  pthread_mutex_unlock (&this->lock);

  return pts;
}

static void scr_exit (scr_plugin_t *scr)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_destroy (&this->lock);
  free(this);
}

/*
 * adjusteble_scr_t
 */

/*
 * speed_tuning()
 *
 * - fine-tune SCR speed. Actual speed is base_speed * factor.
 */
static void adjustable_scr_speed_tuning (adjustable_scr_t *scr, double factor)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_lock (&this->lock);

  set_pivot( this );
  this->speed_tuning = factor;
  this->speed_factor = (double) this->xine_speed * (double)this->scr_speed_base /*90000.0*/ /
                       (1.0*XINE_FINE_SPEED_NORMAL) *
                       this->speed_tuning;

  pthread_mutex_unlock (&this->lock);
}

/*
 * speed_base()
 *
 * - set base speed of SCR (default is 90kHz)
 */
static void adjustable_scr_speed_base (adjustable_scr_t *scr, int hz)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_lock (&this->lock);

  set_pivot( this );
  this->scr_speed_base = hz;
  this->speed_factor = (double) this->xine_speed * (double)this->scr_speed_base /*90000.0*/ /
                       (1.0*XINE_FINE_SPEED_NORMAL) *
                       this->speed_tuning;

  pthread_mutex_unlock (&this->lock);
}

/*
 * jump()
 *
 * - Move SCR 'pts' ticks forward
 */
static void adjustable_scr_jump (adjustable_scr_t *scr, int pts)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_lock (&this->lock);

  set_pivot( this );
  this->cur_pts += pts;

  pthread_mutex_unlock (&this->lock);
}

/*
 * got_pcr()
 *
 * - Synchronize clock to incoming PCR values
 */
static void adjustable_scr_got_pcr (adjustable_scr_t *scr, int64_t pcr)
{
}

/*
 * set_buffering()
 *
 * - Clock is freezed while buffering
 */
static void adjustable_scr_set_buffering (adjustable_scr_t *scr, int buffering)
{
  scr_impl_t *this = (scr_impl_t*) scr;

  pthread_mutex_lock (&this->lock);

  if (buffering) {
    if (!this->buffering) {
      set_pivot( this );
      this->buffering = 1;
      this->buffering_start_time = time_ms();
      LOGMSG("start buffering at %"PRId64, this->cur_pts);
    }
  } else {
    if (this->buffering) {
      set_pivot( this );
      this->buffering = 0;
      LOGMSG("stop buffering at %"PRId64" (buffering took %"PRIu64" ms)",
	     this->cur_pts, elapsed(this->buffering_start_time));
    }
  }

  pthread_mutex_unlock (&this->lock);
}

/*
 * dispose()
 *
 * - unregister, stop and free resources
 */
static void adjustable_scr_dispose(adjustable_scr_t *scr)
{
  scr_impl_t *this = (scr_impl_t*)scr;

  /* unregister */
  if (this->xine)
    this->xine->clock->unregister_scr(this->xine->clock, &this->scr);

  /* exit and dispose */
  this->scr.exit(&this->scr);
}

/*
 * adjusteble_scr_start()
 *
 */
adjustable_scr_t* adjustable_scr_start (xine_t *xine)
{
  scr_impl_t *this;

  this = calloc(1, sizeof(scr_impl_t));

  /* xine scr plugin interface */
  this->scr.interface_version = 3;
  this->scr.set_fine_speed    = scr_set_fine_speed;
  this->scr.get_priority      = scr_get_priority;
  this->scr.adjust            = scr_adjust;
  this->scr.start             = scr_start;
  this->scr.get_current       = scr_get_current;
  this->scr.exit              = scr_exit;

  /* tuning / management interface */
  this->ascr.set_speed_tuning = adjustable_scr_speed_tuning;
  this->ascr.set_speed_base   = adjustable_scr_speed_base;
  this->ascr.jump             = adjustable_scr_jump;
  this->ascr.got_pcr          = adjustable_scr_got_pcr;
  this->ascr.set_buffering    = adjustable_scr_set_buffering;
  this->ascr.dispose          = adjustable_scr_dispose;

  /* initialize */

  pthread_mutex_init (&this->lock, NULL);

  this->xine = xine;
  this->scr_speed_base = 90000;

  adjustable_scr_speed_tuning(&this->ascr, 1.0 );
  scr_set_fine_speed (&this->scr, XINE_SPEED_PAUSE);

  /* start and register */

  uint64_t time  = xine->clock->get_current_time(xine->clock);
  this->scr.start(&this->scr, time);

  if (xine->clock->register_scr(xine->clock, &this->scr)) {
    scr_exit(&this->scr);
    return NULL;
  }

  return &this->ascr;
}
