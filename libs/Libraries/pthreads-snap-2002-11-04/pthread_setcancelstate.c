/*
 * pthread_setcancelstate.c
 *
 * Description:
 * POSIX thread functions related to thread cancellation.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2002 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@ise.canberra.edu.au
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "pthread.h"
#include "implement.h"


int
pthread_setcancelstate (int state, int *oldstate)
     /*
      * ------------------------------------------------------
      * DOCPUBLIC
      *      This function atomically sets the calling thread's
      *      cancelability state to 'state' and returns the previous
      *      cancelability state at the location referenced by
      *      'oldstate'
      *
      * PARAMETERS
      *      state,
      *      oldstate
      *              PTHREAD_CANCEL_ENABLE
      *                      cancellation is enabled,
      *
      *              PTHREAD_CANCEL_DISABLE
      *                      cancellation is disabled
      *
      *
      * DESCRIPTION
      *      This function atomically sets the calling thread's
      *      cancelability state to 'state' and returns the previous
      *      cancelability state at the location referenced by
      *      'oldstate'.
      *
      *      NOTES:
      *      1)      Use to disable cancellation around 'atomic' code that
      *              includes cancellation points
      *
      * COMPATIBILITY ADDITIONS
      *      If 'oldstate' is NULL then the previous state is not returned
      *      but the function still succeeds. (Solaris)
      *
      * RESULTS
      *              0               successfully set cancelability type,
      *              EINVAL          'state' is invalid
      *
      * ------------------------------------------------------
      */
{
  int result = 0;
  pthread_t self = pthread_self();

  if (self == NULL
      || (state != PTHREAD_CANCEL_ENABLE
          && state != PTHREAD_CANCEL_DISABLE))
    {
      return EINVAL;
    }

  /*
   * Lock for async-cancel safety.
   */
  (void) pthread_mutex_lock(&self->cancelLock);

  if (oldstate != NULL)
    {
      *oldstate = self->cancelState;
    }

  self->cancelState = state;

  /*
   * Check if there is a pending asynchronous cancel
   */
  if (state == PTHREAD_CANCEL_ENABLE
      && self->cancelType == PTHREAD_CANCEL_ASYNCHRONOUS
      && WaitForSingleObject(self->cancelEvent, 0) == WAIT_OBJECT_0)
    {
      self->state = PThreadStateCanceling;
      self->cancelState = PTHREAD_CANCEL_DISABLE;
      ResetEvent(self->cancelEvent);
      (void) pthread_mutex_unlock(&self->cancelLock);
      ptw32_throw(PTW32_EPS_CANCEL);

      /* Never reached */
    }

  (void) pthread_mutex_unlock(&self->cancelLock);

  return (result);

}                               /* pthread_setcancelstate */

