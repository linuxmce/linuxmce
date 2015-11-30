/*
 * section_lock.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: section_lock.h,v 1.1 2010-03-15 12:07:44 phintuka Exp $
 *
 */

#ifndef __SECTION_LOCK_H
#define __SECTION_LOCK_H

#include <vdr/thread.h>

class cSectionLock {
  private:
    cMutex& mutex;
  public:
    cSectionLock(cMutex& Mutex) : mutex(Mutex) { mutex.Lock(); };
    ~cSectionLock() { mutex.Unlock(); };
};

#endif // __SECTION_LOCK_H
