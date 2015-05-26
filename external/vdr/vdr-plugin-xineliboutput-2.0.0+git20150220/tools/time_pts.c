/*
 * time_pts.c: Adjustable clock in PTS units
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: time_pts.c,v 1.4 2008-04-28 20:48:05 phintuka Exp $
 *
 */

#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS 
#include <inttypes.h>
#include <time.h>

#include <vdr/config.h>

#include "../logdefs.h"            // logging

#include "time_pts.h"


#define MAX_SCR ((int64_t)0x1ffffffffLL)

#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
#else
#  warning Posix monotonic clock not available
#endif

int cTimePts::m_Monotonic = -1;

void cTimePts::Init(void)
{
#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
  if(m_Monotonic >= 0)
    return;

  m_Monotonic = 0;

  struct timespec resolution;   
  if(clock_getres(CLOCK_MONOTONIC, &resolution)) {
    LOGERR("cTimePts: clock_getres(CLOCK_MONOTONIC) failed");
    return;
  }

  LOGDBG("cTimePts: clock_gettime(CLOCK_MONOTONIC): clock resolution %d us",
	 ((int)resolution.tv_nsec) / 1000);

  if( resolution.tv_sec == 0 && resolution.tv_nsec <= 1000000 ) {
    struct timespec tp;
    if(clock_gettime(CLOCK_MONOTONIC, &tp)) {
      LOGERR("cTimePts: clock_gettime(CLOCK_MONOTONIC) failed");
    } else {
      LOGDBG("cTimePts: using monotonic clock");
      m_Monotonic = 1;
    }
  }
#endif
}

cTimePts::cTimePts(void)
{
  m_Paused     = false;
  m_ScrSpeed   = 90000;
  m_Multiplier = 90000;

  Init();

  Set();
}

int64_t cTimePts::Now(void) const
{
  if(m_Paused)
    return begin;

  struct timeval t;

#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
  if(m_Monotonic) {
    struct timespec tp;

    if(clock_gettime(CLOCK_MONOTONIC, &tp)) {
      LOGERR("cTimePts: clock_gettime(CLOCK_MONOTONIC) failed");
      return -1;
    }

    t.tv_sec  = tp.tv_sec;
    t.tv_usec = tp.tv_nsec/1000;

  } else if (gettimeofday(&t, NULL)) {
    LOGERR("cTimePts: gettimeofday() failed");
    return -1;
  }
#else
  if (gettimeofday(&t, NULL)) {
    LOGERR("cTimePts: gettimeofday() failed");
    return -1;
  }
#endif

  t.tv_sec -= tbegin.tv_sec;
  if(t.tv_usec < tbegin.tv_usec) {
    t.tv_sec--;
    t.tv_usec += 1000000;
  }
  t.tv_usec -= tbegin.tv_usec;

  int64_t pts = 0;
  pts += (int64_t)t.tv_sec  * (int64_t)m_ScrSpeed;
  pts += (int64_t)t.tv_usec * (int64_t)m_ScrSpeed / INT64_C(1000000);

  if(m_Multiplier != 90000)
    pts = pts * m_Multiplier / INT64_C(90000);

  return ( pts + begin ) & MAX_SCR;
}

void cTimePts::Set(int64_t Pts)
{
  begin = Pts;

#if _POSIX_TIMERS > 0 && defined(_POSIX_MONOTONIC_CLOCK)
  if(m_Monotonic) {
    struct timespec tp;

    if(!clock_gettime(CLOCK_MONOTONIC, &tp)) {
      tbegin.tv_sec  = tp.tv_sec;
      tbegin.tv_usec = tp.tv_nsec/1000;
      return;
    }

    LOGERR("cTimePts: clock_gettime(CLOCL_MONOTONIC) failed");
    m_Monotonic = 0;
  }
#endif

  gettimeofday(&tbegin, NULL);
}

void cTimePts::Pause(void)
{
  Set(Now());
  m_Paused = true;
}

void cTimePts::Resume(void)
{
  if(m_Paused) {
    Set(begin);
    m_Paused = false;
  }
}

void cTimePts::TrickSpeed(const int Multiplier)
{
  Set(Now());

  if(Multiplier < 0)
    m_Multiplier = 90000 * (-Multiplier);
  else if(Multiplier > 0)
    m_Multiplier = 90000 / Multiplier;
  else
    LOGERR("cTimePts::SetSpeed: Multiplier=%d", Multiplier);
}

void cTimePts::SetScrSpeed(const int ScrSpeed)
{
  Set(Now());

  m_ScrSpeed = ScrSpeed;
}
