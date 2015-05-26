/*
 * sys_cap.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: sys_cap.h,v 1.3 2011-04-09 12:50:16 phintuka Exp $
 *
 */

#ifndef _XINELIBOUTPUT_SYS_CAP_H_
#define _XINELIBOUTPUT_SYS_CAP_H_

#include "../features.h"

#include <unistd.h>
#include <sys/types.h>
#ifdef HAVE_LIBCAP
# include <sys/capability.h>
#endif

#ifdef HAVE_LIBCAP
static inline cap_flag_value_t get_cap_flag(cap_value_t cap)
{
  cap_flag_value_t val  = CAP_CLEAR;
  cap_t            caps = cap_get_proc();

  if (!caps) {
    LOGDBG("cap_get_proc() failed");
    return val;
  }

  if (cap_get_flag(caps, cap, CAP_EFFECTIVE, &val)) {
    LOGDBG("cap_get_flag(%d) failed", cap);
  }

  cap_free(caps);

  return val;
}
#endif

static inline int have_cap_sys_nice(void)
{
#ifdef HAVE_LIBCAP
  if (get_cap_flag(CAP_SYS_NICE) == CAP_SET) {
    LOGDBG("Have CAP_SYS_NICE capability");
    return 1;
  }
  LOGDBG("No CAP_SYS_NICE capability");
#endif

  return geteuid() == 0;
}

#endif // _XINELIBOUTPUT_SYS_CAP_H_
