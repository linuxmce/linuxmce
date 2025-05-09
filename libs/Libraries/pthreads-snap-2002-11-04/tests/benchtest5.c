/*
 * benchtest5.c
 *
 *
 * --------------------------------------------------------------------------
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
 *
 * --------------------------------------------------------------------------
 *
 * Measure time taken to complete an elementary operation.
 *
 * - Semaphore
 *   Single thread iteration over post/wait for a semaphore.
 */

#include "test.h"
#include <sys/timeb.h>

#ifdef __GNUC__
#include <stdlib.h>
#endif

#include "benchtest.h"

#define ITERATIONS      10000000L

sem_t sema;

struct _timeb currSysTimeStart;
struct _timeb currSysTimeStop;
long durationMilliSecs;
long overHeadMilliSecs = 0;

#define GetDurationMilliSecs(_TStart, _TStop) ((_TStop.time*1000+_TStop.millitm) \
                                               - (_TStart.time*1000+_TStart.millitm))

/*
 * Dummy use of j, otherwise the loop may be removed by the optimiser
 * when doing the overhead timing with an empty loop.
 */
#define TESTSTART \
  { int i, j = 0, k = 0; _ftime(&currSysTimeStart); for (i = 0; i < ITERATIONS; i++) { j++;

#define TESTSTOP \
  }; _ftime(&currSysTimeStop); if (j + k == i) j++; }


void
reportTest (char * testNameString)
{
  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;

  printf( "%-40s %15ld %15.3f\n",
	    testNameString,
          durationMilliSecs,
          (float) durationMilliSecs * 1E3 / ITERATIONS);
}


int
main (int argc, char *argv[])
{
  printf( "========================================================================\n");
  printf( "\nOperations on a semaphore.\n%ld iterations\n\n",
          ITERATIONS);
  printf( "%-40s %15s %15s\n",
	    "Test",
	    "Total(msec)",
	    "average(usec)");
  printf( "------------------------------------------------------------------------\n");

  /*
   * Time the loop overhead so we can subtract it from the actual test times.
   */

  TESTSTART
  assert(1 == 1);
  TESTSTOP

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;
  overHeadMilliSecs = durationMilliSecs;


  /*
   * Now we can start the actual tests
   */
  assert(sem_init(&sema, 0, 0) == 0);
  TESTSTART
  assert(sem_post(&sema) == 0);
  TESTSTOP
  assert(sem_destroy(&sema) == 0);

  reportTest("Post");


  assert(sem_init(&sema, 0, ITERATIONS) == 0);
  TESTSTART
  assert(sem_wait(&sema) == 0);
  TESTSTOP
  assert(sem_destroy(&sema) == 0);

  reportTest("Wait without blocking");


  /*
   * Time the loop overhead so we can subtract it from the actual test times.
   */

  TESTSTART
  assert(1 == 1);
  assert(1 == 1);
  TESTSTOP

  durationMilliSecs = GetDurationMilliSecs(currSysTimeStart, currSysTimeStop) - overHeadMilliSecs;
  overHeadMilliSecs = durationMilliSecs;


  /*
   * Now we can start the actual tests
   */
  assert(sem_init(&sema, 0, 0) == 0);
  TESTSTART
  assert(sem_post(&sema) == 0);
  assert(sem_wait(&sema) == 0);
  TESTSTOP
  assert(sem_destroy(&sema) == 0);

  reportTest("Post then Wait without blocking");


  assert(sem_init(&sema, 0, 1) == 0);
  TESTSTART
  assert(sem_wait(&sema) == 0);
  assert(sem_post(&sema) == 0);
  TESTSTOP
  assert(sem_destroy(&sema) == 0);

  reportTest("Wait then Post without blocking");

  printf( "========================================================================\n");

  /*
   * End of tests.
   */

  return 0;
}
