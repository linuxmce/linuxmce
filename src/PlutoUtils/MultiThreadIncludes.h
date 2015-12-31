/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file MultiThreadIncludes.h
 Header file for ??? .
 */
#ifndef MultiThreadIncludes_h
#define MultiThreadIncludes_h

#include <pthread.h>
#include "PlutoUtils/CommonIncludes.h"

// Todo: temporarily increase these values for valgrid
#define TRYLOCK_TIMEOUT_WARNING		200 // normally 5
#define DEADLOCK_TIMEOUT_ERROR		2000 // normally 15
#define KILL_THREAD_TIMEOUT			2000 // normally 60 -- the thread will be killed if it blocks for this many seconds

#ifdef WIN32
	#define PTW32_TIMESPEC_TO_FILETIME_OFFSET   ( ((LONGLONG) 27111902 << 32) + (LONGLONG) 3577643008 )

	void filetime_to_timespec(const FILETIME *ft, struct timespec *ts);
	int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

bool operator < (const timespec & t1, const timespec & t2);
bool operator == (const timespec & t1, const timespec & t2);
bool operator <= (const timespec & t1, const timespec & t2);
bool operator > (const timespec & t1, const timespec & t2);
bool operator >= (const timespec & t1, const timespec & t2);
bool operator != (const timespec & t1, const timespec & t2);
timespec & operator += (timespec & t1, const timespec & t2);
timespec & operator += (timespec & t1, long milliseconds);
timespec & operator -= (timespec & t1, long milliseconds);
timespec operator + (const timespec & t1, const timespec & t2);
timespec & operator -= (timespec & t1, const timespec & t2);
timespec operator - (const timespec & t1, const timespec & t2);

bool operator < (const timeval & t1, const timeval & t2);
bool operator == (const timeval & t1, const timeval & t2);
bool operator <= (const timeval & t1, const timeval & t2);
bool operator > (const timeval & t1, const timeval & t2);
bool operator >= (const timeval & t1, const timeval & t2);
bool operator != (const timeval & t1, const timeval & t2);
timeval & operator += (timeval & t1, const timeval & t2);
timeval & operator += (timeval & t1, long milliseconds);
timeval & operator -= (timeval & t1, long milliseconds);
timeval operator + (const timeval & t1, const timeval & t2);
timeval & operator -= (timeval & t1, const timeval & t2);
timeval operator - (const timeval & t1, const timeval & t2);

void timespec_to_timeval(const timespec *ts_source,timeval *tv_dest);
void timeval_to_timespec(const timeval *tv_source,timespec *ts_dest);
timespec ms_to_timespec(unsigned long ts);
int gettimeofday(struct timespec *ts, struct timezone *tz);
void valid_timespec(struct timespec &ts);

// Four levels of thread debugging with combinations of Debug and Thread_LOG
// Maximum debugging, DEBUG && THREAD_LOG means all locks and releases are logged

// x=the name of the instance, y=the mutex, z=optional information to log if lock fails
#ifdef DEBUG
	#ifdef THREAD_LOG
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y,__FILE__,__LINE__,false);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y,__FILE__,__LINE__,false,z);
	#else
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y,__FILE__,__LINE__,true,z);
	#endif
#else
	#ifdef THREAD_LOG
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y,__FILE__,__LINE__,true);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y,__FILE__,__LINE__,true,z);
	#else
		#define PLUTO_SAFETY_LOCK(x,y) PlutoLock x(&y);
		#define PLUTO_SAFETY_LOCK_MESSAGE(x,y,z) PlutoLock x(&y);
		#define PLUTO_SAFETY_LOCK_ERRORSONLY(x,y) PlutoLock x(&y);
	#endif
#endif


/** @class pluto_pthread_mutex_t
A Wrapper for pthread mutex.
*/
class pluto_pthread_mutex_t
{
public:
    bool m_bInitialized;
    int m_Line,m_LockNum;
	pthread_mutex_t mutex;
	string m_sFileName;
	pthread_t m_thread;
	string m_sName;
	int m_NumLocks;  // For tracking recursive
	pthread_cond_t *m_pthread_cond_t; // non null if there's a condition associated with this
	pluto_pthread_mutex_t(string Name,bool bAutoInit=false)
		: m_bInitialized(false), m_Line(0), m_LockNum(0), m_sFileName("NONE"), m_thread(pthread_self()), m_sName(Name)
	{
		m_NumLocks=0;
		m_pthread_cond_t=NULL;
		if( bAutoInit )
			Init(NULL);
	}
	virtual ~pluto_pthread_mutex_t() { if( m_bInitialized ) pthread_mutex_destroy(&mutex); }
	int Init(pthread_mutexattr_t *type,pthread_cond_t *pthread_cond_t=NULL)
	{
		int iResult=pthread_mutex_init(&mutex, type);
		m_bInitialized=true;
		m_pthread_cond_t=pthread_cond_t;
		return iResult;
	}
};


/** @class PlutoLock
A ?
*/
class PlutoLock
{
public:
	string m_sFileName;
	int m_Line,m_LockNum;
	bool m_bReleased,m_bLog,m_bLogErrorsOnly,m_bGotLock,m_bIgnoreDeadlock;

	string m_sMessage;
	pluto_pthread_mutex_t *m_pMyLock;
	time_t m_tTime;
	pthread_t m_thread;

public:
	PlutoLock(pluto_pthread_mutex_t *pLock);
	PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly);
	PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly,string Message);

	~PlutoLock();
	void Relock();
	void DoLock();
	void Release();
	int CondWait();
	int TimedCondWait(timespec &ts);
	int TimedCondWait(int Seconds,int Nanoseconds);
	static void DumpOutstandingLocks();
	static void CheckLocks();
	static void ConfirmNoLocks(string File,int Line);
};

/** @class MutexTracking
A ?
*/
class MutexTracking
{
private:
	int m_iNextLock;  // A counter to keep track of locks

	static pluto_pthread_mutex_t m_MutexForGetInstance;  // Used just once so there's only one instance

	// Make this a pointer, rather than an instance.  When it's an instance, sometimes when the app
	// exits it destroys it before all the threads using it have died
	map<int,PlutoLock *> *m_p_mapLocks;

	pluto_pthread_mutex_t *m_p_mapLockMutex;

	static MutexTracking *m_pMutexTracking;

	static MutexTracking *GetInstance()
	{
		if( m_pMutexTracking==NULL )
		{
			pthread_mutex_lock(&m_MutexForGetInstance.mutex);
			if( m_pMutexTracking==NULL )
			{
				m_pMutexTracking = new MutexTracking();
			}
			pthread_mutex_unlock(&m_MutexForGetInstance.mutex);
		}
		return m_pMutexTracking;
	}

public:

	MutexTracking();
	static void Delete();

	static int GetNextLock()
	{
		MutexTracking *pMutexTracking = MutexTracking::GetInstance();
		return pMutexTracking->m_iNextLock++;
	}

	static int Lock()
	{
		MutexTracking *pMutexTracking = MutexTracking::GetInstance();
		return pthread_mutex_lock(&pMutexTracking->m_p_mapLockMutex->mutex);
	}

	static int UnLock()
	{
		MutexTracking *pMutexTracking = MutexTracking::GetInstance();
		return pthread_mutex_unlock(&pMutexTracking->m_p_mapLockMutex->mutex);
	}

	static int AddToMap(int LockNum,PlutoLock *pPlutoLock);
	static int RemoveFromMap(int LockNum,PlutoLock *pPlutoLock);

	static int GetSize()
	{
		MutexTracking *pMutexTracking = MutexTracking::GetInstance();
		return (int) (*pMutexTracking->m_p_mapLocks).size();
	}

	static map<int,PlutoLock *> *GetMap()
	{
		MutexTracking *pMutexTracking = MutexTracking::GetInstance();
		return pMutexTracking->m_p_mapLocks;
	}
};

#endif


