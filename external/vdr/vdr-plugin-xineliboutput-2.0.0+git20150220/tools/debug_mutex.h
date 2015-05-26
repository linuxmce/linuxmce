/*
 * debug_mutex.h: debugging wrappers for pthread_mutex_ functions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: debug_mutex.h,v 1.3 2007-03-14 11:50:08 phintuka Exp $
 *
 */

#ifndef DEBUG_MUTEX_H
#define DEBUG_MUTEX_H

#ifndef _PTHREAD_H
#  error pthread.h must be included before debug_mutex.h
#endif

/*
 * Override pthread_mutex_ calls:
 *
 * Change type of each mutex to PTHREAD_MUTEX_ERRORCHECK_NP
 *
 * Store line number of last succeed pthread_mutex_lock call 
 * for each initialized mutex
 *
 * Check every pthread_mutex_ call for errors and log all errors
 *
 * To help detecting deadlocks and minimize logging:
 * - Try locking first in pthread_mutex_lock
 * - If pthread_mutex_trylock fails, log a message and retry.
 * - When trylock failed, log another message when lock is acquired.
 *
 *
 * NOTE: debugging itself is not thread-safe and may indicate wrong line numbers !
 *
 */

#define MAX_DBG_MUTEX 64
static struct {
  pthread_mutex_t *lock;
  int line;
  int tid;
} dbgdata[MAX_DBG_MUTEX+1] = {{NULL,0}};

static void dbg_setdata(pthread_mutex_t *mutex, int line)
{
  int i;
  for(i=0; i<MAX_DBG_MUTEX; i++)
    if(dbgdata[i].lock == mutex) {
      dbgdata[i].line = line;
      dbgdata[i].tid = syscall(__NR_gettid);
      return;
    }

  LOGMSG("********** dbg_setdata: new entry (0x%lx at %d)", (unsigned long int)mutex, line);
  for(i=0; i<MAX_DBG_MUTEX; i++)
    if(!dbgdata[i].lock) {
      dbgdata[i].lock = mutex;
      dbgdata[i].line = line;
      dbgdata[i].tid = syscall(__NR_gettid);
      return;
    }

  LOGMSG("********** dbg_setdata: table full !");
}

static int dbg_getdata(pthread_mutex_t *mutex, int line)
{
  int i;
  for(i=0; i<MAX_DBG_MUTEX; i++)
    if(dbgdata[i].lock == mutex)
      return dbgdata[i].line;

  LOGMSG("********** dbg_getdata: NO ENTRY ! (%d)", line);
  return -1;
}

static void dbg_deldata(pthread_mutex_t *mutex, int line)
{
  int i;
  for(i=0; i<MAX_DBG_MUTEX; i++)
    if(dbgdata[i].lock == mutex) {
      dbgdata[i].lock = NULL;
      return;
    }

  LOGMSG("********** dbg_deldata: NO ENTRY ! (%d)", line);
  return;
}

static int dbg_init(pthread_mutex_t *mutex, pthread_mutexattr_t *pattr, int line)
{
  int r;

  errno = 0;
  if(!pattr) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    r = pthread_mutex_init(mutex, &attr);
  } else {
    LOGMSG("********** dbg_init: mutex attribute already given !");
    r = pthread_mutex_init(mutex, pattr);
  }

  if(r) 
    LOGERR("********** dbg_init: pthread_mutex_init FAILED at %d", line);

  dbg_setdata(mutex, line);

  return r;
}

static int dbg_free(pthread_mutex_t *mutex, int line)
{
  int r;

  errno = 0;
  r = pthread_mutex_destroy(mutex);

  if(r) 
    LOGERR("********** dbg_free: pthread_mutex_destroy FAILED at %d ; last lock at %d", 
	   line, dbg_getdata(mutex, line));
  dbg_deldata(mutex, line);

  return r;
}

static int dbg_lock(pthread_mutex_t *mutex, int line)
{
  int r;
  /*struct timespec abs_timeout;*/

  /* try lock first to reduce logging */
  errno = 0;
  r = pthread_mutex_trylock(mutex);
  if(!r) {
    dbg_setdata(mutex,line);
    return r;
  }

  /* try failed - we're going to wait, so log at wait start and end to detect deadlocks */
  LOGERR("********** dbg_lock: pthread_mutex_trylock failed at %d (locked at %d)", 
	 line, dbg_getdata(mutex, line));

  /*  int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
      const struct timespec *restrict abs_timeout); */

  errno = 0;
  r = pthread_mutex_lock(mutex);

  if(r) 
    LOGERR("********** dbg_lock: pthread_mutex_lock FAILED at %d", line);
  
  dbg_setdata(mutex, line);
  LOGMSG("********** dbg_lock: pthread_mutex_lock done at %d", line);
  
 return r;
}

static int dbg_trylock(pthread_mutex_t *mutex, int line)
{
  int r;
  /*struct timespec abs_timeout;*/

  /* try lock first to reduce logging */
  errno = 0;
  r = pthread_mutex_trylock(mutex);
  if(!r) {
    dbg_setdata(mutex,line);
    return r;
  }

  LOGERR("********** dbg_trylock: pthread_mutex_trylock failed at %d (locked at %d)", 
	 line, dbg_getdata(mutex, line));
  
  return r;
}

static int dbg_unlock(pthread_mutex_t *mutex, int line)
{
  int r;

  errno = 0;
  r = pthread_mutex_unlock(mutex);

  if(r) 
    LOGERR("********** dbg_unlock: pthread_mutex_unlock FAILED at %d (last locket at %d)", 
	   line, dbg_getdata(mutex, line));

  //else
  //  dbg_setdata(mutex, 0);

  return r;
}

/* override pthread_ functions with own ones */
#define pthread_mutex_init(l,a)  dbg_init(l, a, __LINE__)
#define pthread_mutex_lock(l)    dbg_lock(l, __LINE__)
#define pthread_mutex_trylock(l) dbg_trylock(l, __LINE__)
#define pthread_mutex_unlock(l)  dbg_unlock(l, __LINE__)
#define pthread_mutex_destroy(l) dbg_free(l, __LINE__)

#else
#  error debug_mutex.h included twice
#endif /* DEBUG_MUTEX_H */
