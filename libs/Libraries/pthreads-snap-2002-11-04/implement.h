/*
 * implement.h
 *
 * Definitions that don't need to be public.
 *
 * Keeps all the internals out of pthread.h
 *
 * --------------------------------------------------------------------------
 *
 *	Pthreads-win32 - POSIX Threads Library for Win32
 *	Copyright(C) 1998 John E. Bossom
 *	Copyright(C) 1999,2002 Pthreads-win32 contributors
 * 
 *	Contact Email: rpj@ise.canberra.edu.au
 * 
 *	The current list of contributors is contained
 *	in the file CONTRIBUTORS included with the source
 *	code distribution. The list can also be seen at the
 *	following World Wide Web location:
 *	http://sources.redhat.com/pthreads-win32/contributors.html
 * 
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation; either
 *	version 2 of the License, or (at your option) any later version.
 * 
 *	This library is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	Lesser General Public License for more details.
 * 
 *	You should have received a copy of the GNU Lesser General Public
 *	License along with this library in the file COPYING.LIB;
 *	if not, write to the Free Software Foundation, Inc.,
 *	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef _IMPLEMENT_H
#define _IMPLEMENT_H

/*
 * note: ETIMEDOUT is correctly defined in winsock.h
 */
#include <windows.h>
#include <winsock.h>

/*
 * In case ETIMEDOUT hasn't been defined above somehow.
 */
#ifndef ETIMEDOUT
#  define ETIMEDOUT 10060     /* This is the value in winsock.h. */
#endif

#if !defined(malloc)
#include <malloc.h>
#endif

#if !defined(INT_MAX)
#include <limits.h>
#endif

/* use local include files during development */
#include "semaphore.h"
#include "sched.h"

#if defined(HAVE_C_INLINE) || defined(__cplusplus)
#define INLINE inline
#else
#define INLINE
#endif

#if defined (__MINGW32__) || (_MSC_VER >= 1300)
#define PTW32_INTERLOCKED_LONG long
#define PTW32_INTERLOCKED_LPLONG long*
#else
#define PTW32_INTERLOCKED_LONG PVOID
#define PTW32_INTERLOCKED_LPLONG PVOID*
#endif

typedef enum {
  /*
   * This enumeration represents the state of the thread;
   * The thread is still "alive" if the numeric value of the
   * state is greater or equal "PThreadStateRunning".
   */
  PThreadStateInitial = 0,	/* Thread not running			*/
  PThreadStateRunning,		/* Thread alive & kicking		*/
  PThreadStateSuspended,	/* Thread alive but suspended		*/
  PThreadStateCanceling,	/* Thread alive but and is		*/
				/* in the process of terminating	*/
				/* due to a cancellation request	*/
  PThreadStateException,	/* Thread alive but exiting		*/
				/* due to an exception			*/
  PThreadStateLast
}
PThreadState;


typedef enum {
  /*
   * This enumeration represents the reason why a thread has
   * terminated/is terminating.
   */
  PThreadDemisePeaceful = 0,	/* Death due natural causes	*/
  PThreadDemiseCancelled,	/* Death due to user cancel	*/
  PThreadDemiseException,	/* Death due to unhandled	*/
				/* exception			*/
  PThreadDemiseNotDead	/* I'm not dead!		*/
}
PThreadDemise;

struct pthread_t_ {
#ifdef _UWIN
  DWORD dummy[5];
#endif
  DWORD thread;
  HANDLE threadH;
  PThreadState state;
  PThreadDemise demise;
  void *exitStatus;
  void *parms;
  int ptErrno;
  int detachState;
  pthread_mutex_t cancelLock;  /* Used for async-cancel safety */
  int cancelState;
  int cancelType;
  HANDLE cancelEvent;
#ifdef __CLEANUP_C
  jmp_buf start_mark;
#endif /* __CLEANUP_C */
#if HAVE_SIGSET_T
  sigset_t sigmask;
#endif /* HAVE_SIGSET_T */
  int implicit:1;
  void *keys;
};


/* 
 * Special value to mark attribute objects as valid.
 */
#define PTW32_ATTR_VALID ((unsigned long) 0xC4C0FFEE)

struct pthread_attr_t_ {
  unsigned long valid;
  void *stackaddr;
  size_t stacksize;
  int detachstate;
  struct sched_param param;
  int inheritsched;
  int contentionscope;
#if HAVE_SIGSET_T
  sigset_t sigmask;
#endif /* HAVE_SIGSET_T */
};


/*
 * ====================
 * ====================
 * Semaphores, Mutexes and Condition Variables
 * ====================
 * ====================
 */

struct sem_t_ {
#ifdef NEED_SEM
  unsigned int	value;
  CRITICAL_SECTION sem_lock_cs;
  HANDLE	event;
#else /* NEED_SEM */
  HANDLE sem;
#endif /* NEED_SEM */
};

#define PTW32_OBJECT_AUTO_INIT ((void *) -1)
#define PTW32_OBJECT_INVALID   NULL

struct pthread_mutex_t_ {
  LONG lock_idx;	       /* Provides exclusive access to mutex state
				  via the Interlocked* mechanism, as well
				  as a count of the number of threads
				  waiting on the mutex. */
  int recursive_count;	       /* Number of unlocks a thread needs to perform
				  before the lock is released (recursive
				  mutexes only). */
  int kind;		       /* Mutex type. */
  pthread_t ownerThread;
  sem_t wait_sema;	       /* Mutex release notification to waiting
				  threads. */
  CRITICAL_SECTION wait_cs;    /* Serialise lock_idx decrement after mutex
				  timeout. */
};

struct pthread_mutexattr_t_ {
  int pshared;
  int kind;
};

/*
 * Possible values, other than PTW32_OBJECT_INVALID,
 * for the "interlock" element in a spinlock.
 *
 * In this implementation, when a spinlock is initialised,
 * the number of cpus available to the process is checked.
 * If there is only one cpu then "interlock" is set equal to
 * PTW32_SPIN_USE_MUTEX and u.mutex is a initialised mutex.
 * If the number of cpus is greater than 1 then "interlock"
 * is set equal to PTW32_SPIN_UNLOCKED and the number is
 * stored in u.cpus. This arrangement allows the spinlock
 * routines to attempt an InterlockedCompareExchange on "interlock"
 * immediately and, if that fails, to try the inferior mutex.
 *
 * "u.cpus" isn't used for anything yet, but could be used at
 * some point to optimise spinlock behaviour.
 */
#define PTW32_SPIN_UNLOCKED    (1)
#define PTW32_SPIN_LOCKED      (2)
#define PTW32_SPIN_USE_MUTEX   (3)

struct pthread_spinlock_t_ {
  long interlock;	       /* Locking element for multi-cpus. */
  union {
    int cpus;		       /* No. of cpus if multi cpus, or   */
    pthread_mutex_t mutex;     /* mutex if single cpu.		  */
  } u;
};

struct pthread_barrier_t_ {
  unsigned int nCurrentBarrierHeight;
  unsigned int nInitialBarrierHeight;
  int iStep;
  int pshared;
  sem_t semBarrierBreeched[2];
};

struct pthread_barrierattr_t_ {
  int pshared;
};

struct pthread_key_t_ {
  DWORD key;
  void (*destructor) (void *);
  pthread_mutex_t threadsLock;
  void *threads;
};


typedef struct ThreadParms ThreadParms;
typedef struct ThreadKeyAssoc ThreadKeyAssoc;

struct ThreadParms {
  pthread_t tid;
  void *(*start) (void *);
  void *arg;
};


struct pthread_cond_t_ {
  long		  nWaitersBlocked;   /* Number of threads blocked	     */
  long		  nWaitersGone;      /* Number of threads timed out	     */
  long		  nWaitersToUnblock; /* Number of threads to unblock	     */
  sem_t 	  semBlockQueue;     /* Queue up threads waiting for the     */
				     /*   condition to become signalled      */
  sem_t 	  semBlockLock;      /* Semaphore that guards access to      */
				     /* | waiters blocked count/block queue  */
				     /* +-> Mandatory Sync.LEVEL-1	     */
  pthread_mutex_t mtxUnblockLock;    /* Mutex that guards access to	     */
				     /* | waiters (to)unblock(ed) counts     */
				     /* +-> Optional* Sync.LEVEL-2	     */
  pthread_cond_t next;		     /* Doubly linked list		     */
  pthread_cond_t prev;
};


struct pthread_condattr_t_ {
  int pshared;
};

#define PTW32_RWLOCK_MAGIC 0xfacade2

struct pthread_rwlock_t_ {
  pthread_mutex_t   mtxExclusiveAccess;
  pthread_mutex_t   mtxSharedAccessCompleted;
  pthread_cond_t    cndSharedAccessCompleted;
  int		    nSharedAccessCount;
  int		    nExclusiveAccessCount;
  int		    nCompletedSharedAccessCount;
  int		    nMagic;
};

struct pthread_rwlockattr_t_ {
  int		    pshared;
};


struct ThreadKeyAssoc {
  /*
   * Purpose:
   *	  This structure creates an association between a
   *	  thread and a key.
   *	  It is used to implement the implicit invocation
   *	  of a user defined destroy routine for thread
   *	  specific data registered by a user upon exiting a
   *	  thread.
   *
   * Attributes:
   *	  lock
   *		  protects access to the rest of the structure
   *
   *	  thread
   *		  reference to the thread that owns the association.
   *		  As long as this is not NULL, the association remains
   *		  referenced by the pthread_t.
   *
   *	  key
   *		  reference to the key that owns the association.
   *		  As long as this is not NULL, the association remains
   *		  referenced by the pthread_key_t.
   *
   *	  nextKey
   *		  The pthread_t->keys attribute is the head of a
   *		  chain of associations that runs through the nextKey
   *		  link. This chain provides the 1 to many relationship
   *		  between a pthread_t and all pthread_key_t on which
   *		  it called pthread_setspecific.
   *
   *	  nextThread
   *		  The pthread_key_t->threads attribute is the head of
   *		  a chain of assoctiations that runs through the
   *		  nextThreads link. This chain provides the 1 to many
   *		  relationship between a pthread_key_t and all the 
   *		  PThreads that have called pthread_setspecific for
   *		  this pthread_key_t.
   *
   *
   * Notes:
   *	  1)	  As long as one of the attributes, thread or key, is
   *		  not NULL, the association is being referenced; once
   *		  both are NULL, the association must be released.
   *
   *	  2)	  Under WIN32, an association is only created by
   *		  pthread_setspecific if the user provided a
   *		  destroyRoutine when they created the key.
   *
   *
   */
  pthread_mutex_t lock;
  pthread_t thread;
  pthread_key_t key;
  ThreadKeyAssoc *nextKey;
  ThreadKeyAssoc *nextThread;
};


#ifdef __CLEANUP_SEH
/*
 * --------------------------------------------------------------
 * MAKE_SOFTWARE_EXCEPTION
 *	This macro constructs a software exception code following
 *	the same format as the standard Win32 error codes as defined
 *	in WINERROR.H
 *  Values are 32 bit values layed out as follows:
 *
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +---+-+-+-----------------------+-------------------------------+
 *  |Sev|C|R|	  Facility	    |		    Code	    |
 *  +---+-+-+-----------------------+-------------------------------+
 *
 * Severity Values:
 */
#define SE_SUCCESS		0x00
#define SE_INFORMATION		0x01
#define SE_WARNING		0x02
#define SE_ERROR		0x03

#define MAKE_SOFTWARE_EXCEPTION( _severity, _facility, _exception ) \
( (DWORD) ( ( (_severity) << 30 ) |	/* Severity code	*/ \
	    ( 1 << 29 ) |		/* MS=0, User=1 	*/ \
	    ( 0 << 28 ) |		/* Reserved		*/ \
	    ( (_facility) << 16 ) |	/* Facility Code	*/ \
	    ( (_exception) <<  0 )	/* Exception Code	*/ \
	    ) )

/*
 * We choose one specific Facility/Error code combination to
 * identify our software exceptions vs. WIN32 exceptions.
 * We store our actual component and error code within
 * the optional information array.
 */
#define EXCEPTION_PTW32_SERVICES	\
     MAKE_SOFTWARE_EXCEPTION( SE_ERROR, \
			      PTW32_SERVICES_FACILITY, \
			      PTW32_SERVICES_ERROR )

#define PTW32_SERVICES_FACILITY 	0xBAD
#define PTW32_SERVICES_ERROR		0xDEED

#endif /* __CLEANUP_SEH */

/*
 * Services available through EXCEPTION_PTW32_SERVICES
 * and also used [as parameters to ptw32_throw()] as
 * generic exception selectors.
 */

#define PTW32_EPS_EXIT			(1)
#define PTW32_EPS_CANCEL		(2)

/* Mutex constants */
enum {
  PTW32_MUTEX_LOCK_IDX_INIT	= -1,
  PTW32_MUTEX_OWNER_ANONYMOUS = 1
};


/* Useful macros */
#define PTW32_MAX(a,b)	((a)<(b)?(b):(a))
#define PTW32_MIN(a,b)	((a)>(b)?(b):(a))


/* Declared in global.c */
extern PTW32_INTERLOCKED_LONG (WINAPI *ptw32_interlocked_compare_exchange)(PTW32_INTERLOCKED_LPLONG,
									   PTW32_INTERLOCKED_LONG,
									   PTW32_INTERLOCKED_LONG);

extern int ptw32_processInitialized;
extern pthread_key_t ptw32_selfThreadKey;
extern pthread_key_t ptw32_cleanupKey;
extern pthread_cond_t ptw32_cond_list_head;
extern pthread_cond_t ptw32_cond_list_tail;

extern int ptw32_mutex_default_kind;

extern int ptw32_concurrency;

extern CRITICAL_SECTION ptw32_mutex_test_init_lock;
extern CRITICAL_SECTION ptw32_cond_list_lock;
extern CRITICAL_SECTION ptw32_cond_test_init_lock;
extern CRITICAL_SECTION ptw32_rwlock_test_init_lock;
extern CRITICAL_SECTION ptw32_spinlock_test_init_lock;

#ifdef _UWIN
extern int pthread_count;
#endif

/* Declared in misc.c */
#ifdef NEED_CALLOC
#define calloc(n, s) ptw32_calloc(n, s)
void *ptw32_calloc(size_t n, size_t s);
#endif

/* Declared in private.c */
void ptw32_throw(DWORD exception);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * =====================
 * =====================
 * Forward Declarations
 * =====================
 * =====================
 */

int ptw32_is_attr (const pthread_attr_t *attr);

int ptw32_cond_check_need_init(pthread_cond_t *cond);
int ptw32_mutex_check_need_init(pthread_mutex_t *mutex);
int ptw32_rwlock_check_need_init(pthread_rwlock_t *rwlock);

PTW32_INTERLOCKED_LONG WINAPI
ptw32_InterlockedCompareExchange(PTW32_INTERLOCKED_LPLONG location,
				 PTW32_INTERLOCKED_LONG   value,
				 PTW32_INTERLOCKED_LONG   comparand);

int ptw32_processInitialize (void);

void ptw32_processTerminate (void);

void ptw32_threadDestroy (pthread_t tid);

void ptw32_pop_cleanup_all (int execute);

pthread_t ptw32_new (void);

int ptw32_getprocessors(int * count);

#if ! defined (__MINGW32__) || defined (__MSVCRT__)
unsigned __stdcall
#else
void
#endif
ptw32_threadStart (void * vthreadParms);

void ptw32_callUserDestroyRoutines (pthread_t thread);

int ptw32_tkAssocCreate (ThreadKeyAssoc ** assocP,
			    pthread_t thread,
			    pthread_key_t key);

void ptw32_tkAssocDestroy (ThreadKeyAssoc * assoc);


#ifdef NEED_SEM
void ptw32_decrease_semaphore(sem_t * sem);
BOOL ptw32_increase_semaphore(sem_t * sem,
				 unsigned int n);
#endif /* NEED_SEM */

#ifdef NEED_FTIME
void ptw32_timespec_to_filetime(const struct timespec *ts, FILETIME *ft);
void ptw32_filetime_to_timespec(const FILETIME *ft, struct timespec *ts);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#ifdef _UWIN_
#   ifdef	_MT
#	ifdef __cplusplus
	extern "C" {
#	endif
	_CRTIMP unsigned long  __cdecl _beginthread (void (__cdecl *) (void *),
		unsigned, void *);
	_CRTIMP void __cdecl _endthread(void);
	_CRTIMP unsigned long __cdecl _beginthreadex(void *, unsigned,
		unsigned (__stdcall *) (void *), void *, unsigned, unsigned *);
	_CRTIMP void __cdecl _endthreadex(unsigned);
#	ifdef __cplusplus
	}
#	endif
#   endif
#else
//#   include <process.h>
#endif

/*
 * Check for old and new versions of cygwin. See the FAQ file:
 *
 * Question 1 - How do I get pthreads-win32 to link under Cygwin or Mingw32?
 *
 * Patch by Anders Norlander <anorland@hem2.passagen.se>
 */
#if defined(__CYGWIN32__) || defined(__CYGWIN__) || defined(NEED_CREATETHREAD)

/* 
 * Macro uses args so we can cast start_proc to LPTHREAD_START_ROUTINE
 * in order to avoid warnings because of return type
 */

#define _beginthreadex(security, \
		       stack_size, \
		       start_proc, \
		       arg, \
		       flags, \
		       pid) \
	CreateThread(security, \
		     stack_size, \
		     (LPTHREAD_START_ROUTINE) start_proc, \
		     arg, \
		     flags, \
		     pid)

#define _endthreadex ExitThread

#endif /* __CYGWIN32__ || __CYGWIN__ || NEED_CREATETHREAD*/


#endif /* _IMPLEMENT_H */

