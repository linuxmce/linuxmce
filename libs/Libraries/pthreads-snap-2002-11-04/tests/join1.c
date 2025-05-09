/*
 * Test for pthread_join().
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
 * Depends on API functions: pthread_create(), pthread_join(), pthread_exit().
 */

#include "test.h"

void *
func(void * arg)
{
    int i = (int) arg;

    Sleep(i * 500);

    pthread_exit(arg);

    /* Never reached. */
    exit(1);
}

int
main(int argc, char * argv[])
{
	pthread_t id[4];
	int i;
	int result;

	/* Create a few threads and then exit. */
	for (i = 0; i < 4; i++)
	  {
	    assert(pthread_create(&id[i], NULL, func, (void *) i) == 0);
	  }

	/* Some threads will finish before they are joined, some after. */
	Sleep(1000);

	for (i = 0; i < 4; i++)
	  {
	    assert(pthread_join(id[i], (void **) &result) == 0);
#if ! defined (__MINGW32__) || defined (__MSVCRT__)
	    assert(result == i);
#else
# warning pthread_join not fully supported in this configuration.
	    assert(result == 0);
#endif
	  }

	/* Success. */
	return 0;
}
