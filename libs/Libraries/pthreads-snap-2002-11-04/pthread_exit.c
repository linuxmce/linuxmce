/*
 * pthread_exit.c
 *
 * Description:
 * This translation unit implements routines associated with exiting from
 * a thread.
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

void
pthread_exit (void *value_ptr)
     /*
      * ------------------------------------------------------
      * DOCPUBLIC
      *      This function terminates the calling thread, returning
      *      the value 'value_ptr' to any joining thread.
      *
      * PARAMETERS
      *      value_ptr
      *              a generic data value (i.e. not the address of a value)
      *
      *
      * DESCRIPTION
      *      This function terminates the calling thread, returning
      *      the value 'value_ptr' to any joining thread.
      *      NOTE: thread should be joinable.
      *
      * RESULTS
      *              N/A
      *
      * ------------------------------------------------------
      */
{
  pthread_t self;

  /* If the current thread is implicit it was not started through
     pthread_create(), therefore we cleanup and end the thread
     here. Otherwise we raise an exception to unwind the exception
     stack. The exception will be caught by ptw32_threadStart(),
     which will cleanup and end the thread for us.
   */

  self = (pthread_t) pthread_getspecific (ptw32_selfThreadKey);
#ifdef _UWIN
	 if(--pthread_count <= 0)
		exit((int)value_ptr);
#endif

  if (self == NULL || self->implicit)
    {
      ptw32_callUserDestroyRoutines(self);

#if ! defined (__MINGW32__) || defined (__MSVCRT__)
      _endthreadex ((unsigned) value_ptr);
#else
      _endthread ();
#endif
      
      /* Never reached */
    }

  self->exitStatus = value_ptr;

  ptw32_throw(PTW32_EPS_EXIT);

  /* Never reached. */

}
