/*
 * pthread_rwlockattr_init.c
 *
 * Description:
 * This translation unit implements read/write lock primitives.
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

#include <errno.h>
#include <limits.h>

#include "pthread.h"
#include "implement.h"

int
pthread_rwlockattr_init (pthread_rwlockattr_t * attr)
     /*
      * ------------------------------------------------------
      * DOCPUBLIC
      *      Initializes a rwlock attributes object with default
      *      attributes.
      *
      * PARAMETERS
      *      attr
      * 	     pointer to an instance of pthread_rwlockattr_t
      *
      *
      * DESCRIPTION
      *      Initializes a rwlock attributes object with default
      *      attributes.
      *
      * RESULTS
      * 	     0		     successfully initialized attr,
      * 	     ENOMEM	     insufficient memory for attr.
      *
      * ------------------------------------------------------
      */
{
  int result = 0;
  pthread_rwlockattr_t rwa;

  rwa = (pthread_rwlockattr_t) calloc (1, sizeof (*rwa));

  if (rwa == NULL)
    {
      result = ENOMEM;
    }
  else
    {
      rwa->pshared = PTHREAD_PROCESS_PRIVATE;
    }

  *attr = rwa;

  return(result);
}				/* pthread_rwlockattr_init */
