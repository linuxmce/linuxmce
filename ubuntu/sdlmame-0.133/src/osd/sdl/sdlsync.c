//============================================================
//
//  sdlsync.c - SDL core synchronization functions
//
//  Copyright (c) 1996-2007, Nicola Salmoria and the MAME Team.
//  Visit http://mamedev.org for licensing and usage restrictions.
//
//  SDLMAME by Olivier Galibert and R. Belmont
//
//============================================================

#ifndef SDLMAME_WIN32
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 	// for PTHREAD_MUTEX_RECURSIVE; needs to be here before other glibc headers are included
#endif

#include "SDL/SDL.h"

#ifdef SDLMAME_MACOSX
#include <mach/mach.h>
#endif

// standard C headers
#include <math.h>
#include <unistd.h>

// MAME headers
#include "osdcore.h"
#include "osinline.h"
#include "sdlsync.h"

#include "eminline.h"

#ifndef SDLMAME_OS2
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#else
#define INCL_DOS
#include <os2.h>

#include <stdlib.h>
#define pthread_t       int
#define pthread_self    _gettid
#endif

#ifndef NO_THREAD_COOPERATIVE
typedef struct _hidden_mutex_t hidden_mutex_t;
struct _hidden_mutex_t {
	pthread_mutex_t id;
};
#else
struct _osd_lock {
 	volatile pthread_t	holder;
	INT32				count;
#ifdef PTR64
	INT8				padding[52];	// Fill a 64-byte cache line
#else
	INT8				padding[56];	// A bit more padding
#endif
};
#endif
 
#ifndef SDLMAME_OS2
struct _osd_event {
	pthread_mutex_t 	mutex;
	pthread_cond_t 		cond;
	volatile INT32		autoreset;
	volatile INT32		signalled;
#ifdef PTR64
	INT8				padding[40];	// Fill a 64-byte cache line
#else
	INT8				padding[48];	// A bit more padding
#endif
};
#else
struct _osd_event {
    HMTX                hmtx;
    HEV                 hev;
    volatile INT32      autoreset;
    INT8                padding[52];    // Fill a 64-byte cache line
};
#endif

struct _osd_thread {
	pthread_t			thread;
};


#ifndef NO_THREAD_COOPERATIVE

//============================================================
//  osd_lock_alloc
//============================================================

osd_lock *osd_lock_alloc(void)
{
	hidden_mutex_t *mutex;
	pthread_mutexattr_t mtxattr;

	mutex = (hidden_mutex_t *)calloc(1, sizeof(hidden_mutex_t));

	pthread_mutexattr_init(&mtxattr);
	pthread_mutexattr_settype(&mtxattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex->id, &mtxattr);

	return (osd_lock *)mutex;
}

//============================================================
//  osd_lock_acquire
//============================================================

void osd_lock_acquire(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;
	int r;
	
	r =	pthread_mutex_lock(&mutex->id);
	if (r==0)
		return;
	//mame_printf_error("Error on lock: %d: %s\n", r, strerror(r));
}

//============================================================
//  osd_lock_try
//============================================================

int osd_lock_try(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;
	int r;
	
	r = pthread_mutex_trylock(&mutex->id);
	if (r==0)
		return 1;
	//if (r!=EBUSY)
    //	mame_printf_error("Error on trylock: %d: %s\n", r, strerror(r));
	return 0;
}

//============================================================
//  osd_lock_release
//============================================================

void osd_lock_release(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;

	pthread_mutex_unlock(&mutex->id);
}

//============================================================
//  osd_lock_free
//============================================================

void osd_lock_free(osd_lock *lock)
{
	hidden_mutex_t *mutex = (hidden_mutex_t *) lock;

	pthread_mutex_unlock(&mutex->id);
	pthread_mutex_destroy(&mutex->id);
	free(mutex);
}
#else
 
INLINE pthread_t osd_compare_exchange_pthread_t(pthread_t volatile *ptr, pthread_t compare, pthread_t exchange)
{
#ifdef PTR64
	INT64 result = compare_exchange64((INT64 volatile *)ptr, (INT64)compare, (INT64)exchange);
#else
	INT32 result = compare_exchange32((INT32 volatile *)ptr, (INT32)compare, (INT32)exchange);
#endif
	return (pthread_t)result;
}

INLINE pthread_t osd_exchange_pthread_t(pthread_t volatile *ptr, pthread_t exchange)
{
#ifdef PTR64
	INT64 result = osd_exchange64((INT64 volatile *)ptr, (INT64)exchange);
#else
	INT32 result = atomic_exchange32((INT32 volatile *)ptr, (INT32)exchange);
#endif
	return (pthread_t)result;
}


//============================================================
//  osd_lock_alloc
//============================================================

osd_lock *osd_lock_alloc(void)
{
	osd_lock *lock;

	lock = (osd_lock *)calloc(1, sizeof(osd_lock));

	lock->holder = 0;
	lock->count = 0;

	return lock;
}

//============================================================
//  osd_lock_acquire
//============================================================

void osd_lock_acquire(osd_lock *lock)
{
	pthread_t current, prev;

	current = pthread_self();
	prev = osd_compare_exchange_pthread_t(&lock->holder, 0, current);
	if (prev != (size_t)NULL && prev != current)
	{
		do {
			register INT32 spin = 10000; // Convenient spin count
			register pthread_t tmp;
#if defined(__i386__) || defined(__x86_64__)
			__asm__ __volatile__ (
				"1: pause                    ;"
				"   mov    %[holder], %[tmp] ;"
				"   test   %[tmp], %[tmp]    ;"
				"   loopne 1b                ;"
				: [spin]   "+c"  (spin)
				, [tmp]    "=&r" (tmp)
				: [holder] "m"   (lock->holder)
				: "%cc"
			);
#elif defined(__ppc__) || defined(__PPC__)
			__asm__ __volatile__ (
				"1: nop                        \n"
				"   nop                        \n"
				"   lwzx  %[tmp], 0, %[holder] \n"
				"   cmpwi %[tmp], 0            \n"
				"   bdnzt eq, 1b               \n"
				: [spin]   "+c"  (spin)
				, [tmp]    "=&r" (tmp)
				: [holder] "r"   (&lock->holder)
				: "cr0"
			);
#elif defined(__ppc64__) || defined(__PPC64__)
			__asm__ __volatile__ (
				"1: nop                        \n"
				"   nop                        \n"
				"   ldx   %[tmp], 0, %[holder] \n"
				"   cmpdi %[tmp], 0            \n"
				"   bdnzt eq, 1b               \n"
				: [spin]   "+c"  (spin)
				, [tmp]    "=&r" (tmp)
				: [holder] "r"   (&lock->holder)
				: "cr0"
			);
#else
			while (--spin > 0 && lock->holder != NULL)
				osd_yield_processor();
#endif
#if 0
			/* If you mean to use locks as a blocking mechanism for extended
			 * periods of time, you should do something like this.  However,
			 * it kills the performance of gaelco3d.
			 */
			if (spin == 0)
			{
				struct timespec sleep = { 0, 100000 }, remaining;
				nanosleep(&sleep, &remaining); // sleep for 100us
			}
#endif
		} while (osd_compare_exchange_pthread_t(&lock->holder, 0, current) != (size_t)NULL);
	}
	lock->count++;
}

//============================================================
//  osd_lock_try
//============================================================

int osd_lock_try(osd_lock *lock)
{
	pthread_t current, prev;

	current = pthread_self();
	prev = osd_compare_exchange_pthread_t(&lock->holder, 0, current);
	if (prev == (size_t)NULL || prev == current)
	{
		lock->count++;
		return 1;
	}
	return 0;
}

//============================================================
//  osd_lock_release
//============================================================

void osd_lock_release(osd_lock *lock)
{
	pthread_t current;

	current = pthread_self();
	if (lock->holder == current)
	{
		if (--lock->count == 0)
#if defined(__ppc__) || defined(__PPC__) || defined(__ppc64__) || defined(__PPC64__)
		lock->holder = 0;
		__asm__ __volatile__( " eieio " : : );
#else
		osd_exchange_pthread_t(&lock->holder, 0);
#endif
		return;
	}

	// trying to release a lock you don't hold is bad!
//	assert(lock->holder == pthread_self());
}

//============================================================
//  osd_lock_free
//============================================================

void osd_lock_free(osd_lock *lock)
{
	free(lock);
}
#endif

#ifndef SDLMAME_OS2
//============================================================
//  osd_num_processors
//============================================================

int osd_num_processors(void)
{
	int processors = 1;

#ifdef SDLMAME_MACOSX
	{
		struct host_basic_info host_basic_info;
		unsigned int count;
		kern_return_t r;
		mach_port_t my_mach_host_self;
		
		count = HOST_BASIC_INFO_COUNT;
		my_mach_host_self = mach_host_self();
		if ( ( r = host_info(my_mach_host_self, HOST_BASIC_INFO, (host_info_t)(&host_basic_info), &count)) == KERN_SUCCESS )
		{
			processors = host_basic_info.avail_cpus;
		}
		mach_port_deallocate(mach_task_self(), my_mach_host_self);
	}
#elif defined(_SC_NPROCESSORS_ONLN)
	processors = sysconf(_SC_NPROCESSORS_ONLN);
#endif
	return processors;
}

//============================================================
//  osd_event_alloc
//============================================================

osd_event *osd_event_alloc(int manualreset, int initialstate)
{
	osd_event *ev;
	pthread_mutexattr_t mtxattr;

	ev = (osd_event *)calloc(1, sizeof(osd_event));

	pthread_mutexattr_init(&mtxattr);
	pthread_mutex_init(&ev->mutex, &mtxattr);
	pthread_cond_init(&ev->cond, NULL);
	ev->signalled = initialstate;
	ev->autoreset = !manualreset;
		
	return ev;
}

//============================================================
//  osd_event_free
//============================================================

void osd_event_free(osd_event *event)
{
	pthread_mutex_destroy(&event->mutex);
	pthread_cond_destroy(&event->cond);
	free(event);
}

//============================================================
//  osd_event_set
//============================================================

#if 1
void osd_event_set(osd_event *event)
{
	pthread_mutex_lock(&event->mutex);
	if (event->signalled == FALSE)
	{
		event->signalled = TRUE;
		if (event->autoreset)
			pthread_cond_signal(&event->cond);
		else
			pthread_cond_broadcast(&event->cond);
	}
	pthread_mutex_unlock(&event->mutex);
}

//============================================================
//  osd_event_reset
//============================================================

void osd_event_reset(osd_event *event)
{
	pthread_mutex_lock(&event->mutex);
	event->signalled = FALSE;
	pthread_mutex_unlock(&event->mutex);
}

//============================================================
//  osd_event_wait
//============================================================

int osd_event_wait(osd_event *event, osd_ticks_t timeout)
{
	pthread_mutex_lock(&event->mutex);
	if (!timeout)
	{
		if (!event->signalled)
		{
				pthread_mutex_unlock(&event->mutex);
				return FALSE;
		}
	}
	else
	{
		if (!event->signalled)
		{
			struct timespec   ts;
			struct timeval    tp;
			UINT64 msec = timeout * 1000 / osd_ticks_per_second();
			UINT64 nsec;
			
			gettimeofday(&tp, NULL);
	
			ts.tv_sec  = tp.tv_sec;
			nsec = (UINT64) tp.tv_usec * (UINT64) 1000 + (msec * (UINT64) 1000000);
			ts.tv_nsec = nsec % (UINT64) 1000000000;
			ts.tv_sec += nsec / (UINT64) 1000000000;

			do {
				int ret = pthread_cond_timedwait(&event->cond, &event->mutex, &ts);
				if ( ret == ETIMEDOUT )
				{
					if (!event->signalled)
					{
						pthread_mutex_unlock(&event->mutex);
						return FALSE;
					}
					else
						break;
				}
				if (ret == 0)
					break;
				if ( ret != EINTR)
				{
					printf("Error %d while waiting for pthread_cond_timedwait:  %s\n", ret, strerror(ret));
				}
				
			} while (TRUE);
		}
	}

	if (event->autoreset)
		event->signalled = 0;

	pthread_mutex_unlock(&event->mutex);

	return TRUE;
}
#else

/*
 * The following code has been added
 * for demonstration purposes only.
 * It implements events without the need
 * for pthreads. However, it horribly fails, if
 * threads > num processors as is the case if you 
 * enable "-mt"
 */

void osd_event_set(osd_event *event)
{
//	while (compare_exchange32(&event->signalled, 0, 1) == 1)
//		osd_yield_processor();
	atomic_exchange32(&event->signalled, 1);
}

void osd_event_reset(osd_event *event)
{
	compare_exchange32(&event->signalled, 1, 0);
}

int osd_event_wait(osd_event *event, osd_ticks_t timeout)
{
	osd_ticks_t stopspin;
	osd_ticks_t gotosleep;
	//struct timespec sleep = { 0, 100000 }, remaining;
	
	if (!timeout && !event->signalled)
		return FALSE;

	stopspin = osd_ticks() + timeout;
	gotosleep = osd_ticks() + osd_ticks_per_second()/1000;

	if (event->autoreset)
	{
		while (compare_exchange32(&event->signalled, 1, 0) == 1)
		{
			if (osd_ticks()>=stopspin)
				return FALSE;
			osd_yield_processor();
			//if (osd_ticks()>=gotosleep)
				//usleep(100);
				//nanosleep(&sleep, &remaining); // sleep for 100us
		}
	}
	else
	{
		while (!event->signalled)
		{
			if (osd_ticks()>=stopspin)
				return FALSE;
			osd_yield_processor();
			//if (osd_ticks()>=gotosleep)
				//usleep(100);
				//nanosleep(&sleep, &remaining); // sleep for 100us
		}
	}
	return TRUE;
}
#endif

//============================================================
//  osd_thread_create
//============================================================

osd_thread *osd_thread_create(osd_thread_callback callback, void *cbparam)
{
	osd_thread *thread;
	pthread_attr_t	attr;

	thread = (osd_thread *)calloc(1, sizeof(osd_thread));
	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	if ( pthread_create(&thread->thread, &attr, callback, cbparam) != 0 )
	{
		free(thread);
		return NULL;
	}
	return thread;
}

//============================================================
//  osd_thread_adjust_priority
//============================================================

int osd_thread_adjust_priority(osd_thread *thread, int adjust)
{
	struct sched_param	sched;
	int					policy;
			
	if ( pthread_getschedparam( thread->thread, &policy, &sched ) == 0 )
	{
		sched.sched_priority += adjust;
		if ( pthread_setschedparam(thread->thread, policy, &sched ) == 0)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

//============================================================
//  osd_thread_cpu_affinity
//============================================================

int osd_thread_cpu_affinity(osd_thread *thread, UINT32 mask)
{
#if !defined(NO_THREAD_COOPERATIVE) && !defined(NO_AFFINITY_NP)
	cpu_set_t	cmask;
	pthread_t	lthread;
	int			bitnum;

	CPU_ZERO(&cmask);
	for (bitnum=0; bitnum<32; bitnum++)
		if (mask & (1<<bitnum))
			CPU_SET(bitnum, &cmask);
	
	if (thread == NULL)
		lthread = pthread_self();
	else
		lthread = thread->thread;
    
	if (pthread_setaffinity_np(lthread, sizeof(cmask), &cmask) <0)
	{
		/* Not available during link in all targets */
		fprintf(stderr, "error %d setting cpu affinity to mask %08x", errno, mask);
		return FALSE;
	}
	else
		return TRUE;
#else
	return TRUE;
#endif
}

//============================================================
//  osd_thread_wait_free
//============================================================

void osd_thread_wait_free(osd_thread *thread)
{
	pthread_join(thread->thread, NULL);
	free(thread);
}

#else

//============================================================
//  osd_num_processors
//============================================================

int osd_num_processors(void)
{
    ULONG numprocs = 1;

    DosQuerySysInfo(QSV_NUMPROCESSORS, QSV_NUMPROCESSORS, &numprocs, sizeof(numprocs));

    return numprocs;
}

//============================================================
//  osd_event_alloc
//============================================================

osd_event *osd_event_alloc(int manualreset, int initialstate)
{
    osd_event *ev;

    ev = (osd_event *)calloc(1, sizeof(osd_event));

    DosCreateMutexSem(NULL, &ev->hmtx, 0, FALSE);
    DosCreateEventSem(NULL, &ev->hev, 0, initialstate);
    ev->autoreset = !manualreset;

    return ev;
}

//============================================================
//  osd_event_free
//============================================================

void osd_event_free(osd_event *event)
{
    DosCloseMutexSem(event->hmtx);
    DosCloseEventSem(event->hev);
    free(event);
}

//============================================================
//  osd_event_set
//============================================================

void osd_event_set(osd_event *event)
{
    DosPostEventSem(event->hev);
}

//============================================================
//  osd_event_reset
//============================================================

void osd_event_reset(osd_event *event)
{
    ULONG ulCount;

    DosResetEventSem(event->hev, &ulCount);
}

//============================================================
//  osd_event_wait
//============================================================

int osd_event_wait(osd_event *event, osd_ticks_t timeout)
{
    ULONG rc;

    if(event->autoreset)
        DosRequestMutexSem(event->hmtx, -1);

    rc = DosWaitEventSem(event->hev, timeout * 1000 / osd_ticks_per_second());

    if(event->autoreset)
    {
        ULONG ulCount;

        DosResetEventSem(event->hev, &ulCount);
        DosReleaseMutexSem(event->hmtx);
    }

    return (rc == 0);
}

//============================================================
//  osd_thread_create
//============================================================

osd_thread *osd_thread_create(osd_thread_callback callback, void *cbparam)
{
    osd_thread *thread;

    thread = (osd_thread *)calloc(1, sizeof(osd_thread));
    thread->thread = _beginthread(callback, NULL, 65535, cbparam);
    if ( thread->thread == -1 )
    {
        free(thread);
        return NULL;
    }
    return thread;
}

//============================================================
//  osd_thread_adjust_priority
//============================================================

int osd_thread_adjust_priority(osd_thread *thread, int adjust)
{
    PTIB ptib;

    DosGetInfoBlocks(&ptib, NULL);

    if ( DosSetPriority(PRTYS_THREAD, PRTYC_NOCHANGE,
                        ((BYTE)ptib->tib_ptib2->tib2_ulpri) + adjust, thread->thread ))
        return FALSE;


    return TRUE;
}

//============================================================
//  osd_thread_cpu_affinity
//============================================================

int osd_thread_cpu_affinity(osd_thread *thread, UINT32 mask)
{
    return TRUE;
}

//============================================================
//  osd_thread_wait_free
//============================================================

void osd_thread_wait_free(osd_thread *thread)
{
    TID tid = thread->thread;

    DosWaitThread(&tid, 0);
    free(thread);
}

#endif  /* SDLMAME_OS2 */
#else	/* SDLMAME_WIN32 */
#include "../windows/winsync.c"
#endif

