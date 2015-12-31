/* 
MultiThreadIncludes

Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com		

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"	 
#include "DCE/Logger.h" 
#include "PlutoUtils/MultiThreadIncludes.h" 
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#if  defined(WINCE) && !defined(SMARTPHONE2005) && !defined(_VC80_UPGRADE)//--- CHANGED4WM5 ----//
#include _STLP_NATIVE_C_HEADER(time.h)
#include "wince.h"
#elif (defined(SMARTPHONE2005)) && !defined(ARMV4I)
#include <wce_time.h>
#define time		wceex_time
#define localtime	wceex_localtime
#define mktime		wceex_mktime
#endif

using namespace DCE;

MutexTracking::MutexTracking()
{
//	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MutexTracking::MutexTracking %p",  m_pMutexTracking);
	m_p_mapLocks = new map<int,PlutoLock *>;
	m_iNextLock=1;
	m_p_mapLockMutex = new pluto_pthread_mutex_t("maplock");
	m_p_mapLockMutex->Init(NULL);
}

void MutexTracking::Delete()
{
//	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MutexTracking::Delete %p",  m_pMutexTracking);
	if( m_pMutexTracking==NULL )
		return;

	if( m_pMutexTracking->m_p_mapLockMutex )
	{
		pthread_mutex_destroy(&m_pMutexTracking->m_p_mapLockMutex->mutex);
		m_pMutexTracking->m_p_mapLockMutex=NULL;
	}

	if( m_pMutexTracking->m_p_mapLocks )
	{
		m_pMutexTracking->m_p_mapLocks->clear();
		delete m_pMutexTracking->m_p_mapLocks;
		m_pMutexTracking->m_p_mapLocks=NULL;
	}

	delete m_pMutexTracking;
	m_pMutexTracking=NULL;
}

int MutexTracking::AddToMap(int LockNum,PlutoLock *pPlutoLock)
{
	MutexTracking *pMutexTracking = MutexTracking::GetInstance();
	(*pMutexTracking->m_p_mapLocks)[LockNum] = pPlutoLock;
/*
string sAllLocks;
for(map<int,PlutoLock *>::iterator itMapLock2=MutexTracking::GetMap()->begin();itMapLock2!=MutexTracking::GetMap()->end();itMapLock2++)
{
PlutoLock *pSafetyLock = itMapLock2->second;
sAllLocks += StringUtils::itos(itMapLock2->first) + "/" + StringUtils::itos(pSafetyLock->m_LockNum) + " ";
}

	LoggerWrapper::GetInstance()->Write(LV_LOCKING, "MutexTracking::AddToMap (%p) %p (>%d) size: %d inst %p all: %s", 
		&pPlutoLock->m_pMyLock->mutex, m_pMutexTracking,LockNum,(int) (*pMutexTracking->m_p_mapLocks).size(), pMutexTracking,sAllLocks.c_str());
		*/
	return 0;
}

int MutexTracking::RemoveFromMap(int LockNum,PlutoLock *pPlutoLock)
{
	MutexTracking *pMutexTracking = MutexTracking::GetInstance();

	map<int,PlutoLock *>::iterator itMapLock = (*pMutexTracking->m_p_mapLocks).find(LockNum);
/*
string sAllLocks;
for(map<int,PlutoLock *>::iterator itMapLock2=MutexTracking::GetMap()->begin();itMapLock2!=MutexTracking::GetMap()->end();itMapLock2++)
{
PlutoLock *pSafetyLock = itMapLock2->second;
sAllLocks += StringUtils::itos(itMapLock2->first) + "/" + StringUtils::itos(pSafetyLock->m_LockNum) + " ";
}

	LoggerWrapper::GetInstance()->Write(LV_LOCKING, "MutexTracking::RemoveFromMap (%p) %p (>%d) %s size: %d inst %p all: %s",
		&pPlutoLock->m_pMyLock->mutex, m_pMutexTracking,LockNum,(itMapLock==(*pMutexTracking->m_p_mapLocks).end() ? "****FAIL****" : ""),
		(int) (*pMutexTracking->m_p_mapLocks).size(), pMutexTracking,sAllLocks.c_str());
*/
	if( itMapLock==(*pMutexTracking->m_p_mapLocks).end() )
		return -1;

	(*pMutexTracking->m_p_mapLocks).erase(itMapLock);
	return 0;
}

MutexTracking *MutexTracking::m_pMutexTracking=NULL;

// An application can create another handler that gets called instead in the event of a deadlock
void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock)=NULL;

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock)
{
	m_bLogErrorsOnly=true;
	m_bReleased=false;
	m_bIgnoreDeadlock=false;
	m_pMyLock=pLock;
	m_sFileName = "*NONE*";
	m_Line = 9999999;
	m_LockNum = 0;
	m_tTime = 0;
	m_thread = pthread_self();
	pthread_mutex_lock(&m_pMyLock->mutex);
	m_pMyLock->m_NumLocks++;
}

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly,string Message)
{
	m_sMessage=Message;
	m_pMyLock=pLock;
	m_bIgnoreDeadlock=false;
	m_bLogErrorsOnly=bLogErrorsOnly;
	m_sFileName=File;
	m_Line=Line;
	m_tTime = time(NULL);
	m_bGotLock = false;
	m_thread = pthread_self();

	MutexTracking::Lock();
	m_LockNum = MutexTracking::GetNextLock();
	MutexTracking::AddToMap(m_LockNum,this);
	MutexTracking::UnLock();
	DoLock();
	m_pMyLock->m_NumLocks++;
}

PlutoLock::PlutoLock(pluto_pthread_mutex_t *pLock,string File,int Line,bool bLogErrorsOnly)
{
	m_pMyLock=pLock;
	m_bLogErrorsOnly=bLogErrorsOnly;
	m_bIgnoreDeadlock=false;
	m_sFileName=File;
	m_Line=Line;
	m_tTime = time(NULL);
	m_bGotLock = false;
	m_thread = pthread_self();

	MutexTracking::Lock();
	m_LockNum = MutexTracking::GetNextLock();
	MutexTracking::AddToMap(m_LockNum,this);
	MutexTracking::UnLock();
	DoLock();
	m_pMyLock->m_NumLocks++;
}

PlutoLock::~PlutoLock()
{
	Release();
	if( m_LockNum )
	{
		MutexTracking::Lock();
#ifdef THREAD_LOG
		int size1 = MutexTracking::GetSize();
#endif		
		if( MutexTracking::RemoveFromMap(m_LockNum,this)!=0 )
		{
			MutexTracking::UnLock();
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot find self in maplock! (%p) (>%d) %s: %s:%d %s", 
				&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
			DumpOutstandingLocks();
		}
		else
		{
#ifdef THREAD_LOG
			int size2 = MutexTracking::GetSize();
#endif
			
			MutexTracking::UnLock();

#ifdef THREAD_LOG
			if( !m_bLogErrorsOnly )
				LoggerWrapper::GetInstance()->Write(LV_LOCKING, "removed from map (%p) #%d (>%d) %s: %s:%d %s was: %d size, now %d Rel: %s Got: %s", 
				&m_pMyLock->mutex, m_pMyLock->m_NumLocks, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),
				m_Line,m_sMessage.c_str(),size1,size2,(m_bReleased ? "Y" : "N"),(m_bGotLock ? "Y" : "N"));
#endif

		}
	}
}	

// Confirms there are no outstanding locks, log if there are
void PlutoLock::ConfirmNoLocks(string File,int Line)
{
	map<int,PlutoLock *>::iterator itMapLock;
	MutexTracking::Lock();
	if( MutexTracking::GetSize()==0 )
	{
		MutexTracking::UnLock();
		return;
	}

	string sProblems="Check requested in file: " + File + ":" + StringUtils::itos(Line) + "\n";
	int iNumProblems=0;
	for(itMapLock=MutexTracking::GetMap()->begin();itMapLock!=MutexTracking::GetMap()->end();++itMapLock)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( pSafetyLock->m_bReleased )
			continue;
		++iNumProblems;
		sProblems+=pSafetyLock->m_sFileName + ":" + StringUtils::itos(pSafetyLock->m_Line) + pSafetyLock->m_pMyLock->m_sName + "\n";
	}
	MutexTracking::UnLock();
	if( !iNumProblems )
		return;

	/*
	#ifdef WIN32
	const char *cfn = sProblems.c_str();
	const TCHAR *tfn = TCHARNEW(cfn);
	::MessageBox(NULL, tfn, _T("LocksFound"), MB_OK);	
	TCHARDELETE(tfn);
	#else
	*/

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Locks found: %s",sProblems.c_str());
	//#endif
}


void PlutoLock::CheckLocks()
{
	map<int,PlutoLock *>::iterator itMapLock;
	MutexTracking::Lock();

	PlutoLock *pSafetyLock_Problem=NULL;
	for(itMapLock=MutexTracking::GetMap()->begin();itMapLock!=MutexTracking::GetMap()->end();++itMapLock)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;
		if( time(NULL)-pSafetyLock->m_tTime>DEADLOCK_TIMEOUT_ERROR )
		{
#ifndef WINCE
			cout << "Found a safety lock problem: " << pSafetyLock << endl;
#endif
			pSafetyLock_Problem=pSafetyLock;
			break;
		}

	}
	MutexTracking::UnLock();
	if( pSafetyLock_Problem )
	{
		DumpOutstandingLocks();
#ifndef WINCE
		cout << "Calling deadlock handler 2" << endl;
		cerr << "Calling deadlock handler 2" << endl;
#endif
		if( g_pDeadlockHandler )
			(*g_pDeadlockHandler)(pSafetyLock_Problem);
	}
}


void PlutoLock::DumpOutstandingLocks()
{
#ifdef WINCE
	//this is crashing under ce ?
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"DumpOutstandingLocks ... exiting");
	return;
#endif

	LoggerWrapper::GetInstance()->Write(LV_LOCKING,"Dumping %d locks",(int) MutexTracking::GetSize());

	MutexTracking::Lock();

	//the mutex is locked, it's ok to access the size() method of (*g_pmapLocks).

	LoggerWrapper::GetInstance()->Write(LV_LOCKING,"Size of locks: %d\n", MutexTracking::GetSize());

	// We cannot use a logger in here because the logger will need to get at the map mutex if logging is over a socket
	map<int,PlutoLock *>::iterator itMapLock;
	list<string> listMessages;
	list<pthread_t> listKilledPthreads;

	// We can't use the logger from here on because if it's a socket logger it will need the (*g_pmapLocks).  Store
	// all messages in listMessages and log after we remove the mutex lock
	listMessages.push_back(string("Dumping " + StringUtils::itos(MutexTracking::GetSize()) + " locks"));
	for(itMapLock=MutexTracking::GetMap()->begin();itMapLock!=MutexTracking::GetMap()->end();)
	{
		PlutoLock *pSafetyLock = (*itMapLock).second;

		// Somehow, for some reason, pSafetyLock->m_pMyLock->m_sName occasionally throws an exception, 
		// like one of the values has been deleted.  But that should be impossible.  This map is protected
		// with a mutex and entries are deleted from the map prior to deleting the objects.  Here's a test
		// AB 4/8/2004 -- the following code is temporary.  remove it when this bug is fixed.
		// Dont' do this in CE
#ifndef UNDER_CE
		pluto_pthread_mutex_t *testLock=NULL;
		string Name="";
		bool bThrewException=false;
		int LockNum=0;
		try
		{
			LockNum=pSafetyLock->m_LockNum;
			testLock=pSafetyLock->m_pMyLock;
			Name=pSafetyLock->m_pMyLock->m_sName;
		}
		catch(...)
		{
			listMessages.push_back(string("caught an exception for exceptions"));
			bThrewException=true;
		}
		listMessages.push_back(string("finished check for exceptions"));

		if( bThrewException )
		{
			printf("^01\t****EXCEPTION with lock***** ptr: %p name: %s locknum: %d\n",testLock,Name.c_str(),LockNum);
			fflush(stdout);

			try
			{
				char Message[1024];

#ifdef PTHREAD2
#ifdef WIN32
#pragma warning(disable: 4311)
#endif
				sprintf(Message,"^01\t (>%d) %s l:%d time: (%d s) thread: %lu Rel: %s Got: %s",
					pSafetyLock->m_LockNum,pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,(int) (time(NULL)-pSafetyLock->m_tTime),
                                        (unsigned long)pSafetyLock->m_thread.p,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#ifdef WIN32
#pragma warning(default: 4311)
#endif
#else

                                sprintf(Message,"^01\t (>%d) %s l:%d time: (%d s) thread: %lu Rel: %s Got: %s",
                                        pSafetyLock->m_LockNum,pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,(int) (time(NULL)-pSafetyLock->m_tTime),
                                        (unsigned long)pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#endif

				listMessages.push_back(Message);

			}
			catch(...)
			{
				printf("^01\tDamn you.  Threw it again!!\n");
				fflush(stdout);
			}
			++itMapLock;
			continue;
		}
		// AB 4/8/2004 - end of temporary code
#endif

		if( pSafetyLock->m_bIgnoreDeadlock )
		{
			++itMapLock;
			continue;
		}

		struct tm ptm;
#ifdef WIN32
		_localtime64_s(&ptm, &pSafetyLock->m_tTime);
#else
		localtime_r(&pSafetyLock->m_tTime,&ptm);
#endif
		string sTime = (((int) time(NULL)-pSafetyLock->m_tTime)>=TRYLOCK_TIMEOUT_WARNING && pSafetyLock->m_bGotLock && !pSafetyLock->m_bReleased ? "*****DL******" : "") + 
			StringUtils::itos(ptm.tm_hour==0 ? 12 : (ptm.tm_hour>13 ? ptm.tm_hour-12 : ptm.tm_hour)) + ":" + 
			(ptm.tm_min<10 ? "0" : "") + StringUtils::itos(ptm.tm_min) + (ptm.tm_sec<10 ? ":0" : ":") + 
			StringUtils::itos(ptm.tm_sec) + (ptm.tm_hour>11 ? "p" : "a");

		char Message[400];

#ifdef PTHREAD2
#ifdef WIN32
#pragma warning(disable: 4311)
#endif
		sprintf(Message,"OL: (%p) (>%d) %s %s l:%d time: %s (%d s) thread: %lu Rel: %s Got: %s",
			&pSafetyLock->m_pMyLock->mutex,pSafetyLock->m_LockNum,pSafetyLock->m_pMyLock->m_sName.c_str(),pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,sTime.c_str(),(int) (time(NULL)-pSafetyLock->m_tTime),
                        (unsigned long)pSafetyLock->m_thread.p,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#ifdef WIN32
#pragma warning(default: 4311)
#endif
#else

                sprintf(Message,"OL: (%p) (>%d) %s %s l:%d time: %s (%d s) thread: %lu Rel: %s Got: %s",
                        &pSafetyLock->m_pMyLock->mutex,pSafetyLock->m_LockNum,pSafetyLock->m_pMyLock->m_sName.c_str(),pSafetyLock->m_sFileName.c_str(),pSafetyLock->m_Line,sTime.c_str(),(int) (time(NULL)-pSafetyLock->m_tTime),
                        (unsigned long)pSafetyLock->m_thread,(pSafetyLock->m_bReleased ? "Y" : "N"),(pSafetyLock->m_bGotLock ? "Y" : "N"));
#endif

		if( ((int) time(NULL)-pSafetyLock->m_tTime)>=KILL_THREAD_TIMEOUT && pSafetyLock->m_bGotLock && !pSafetyLock->m_bReleased )
		{
			strcat(Message,"**RELEASING**");
			listMessages.push_back(string(Message));

			pSafetyLock->m_bReleased=true;
			pSafetyLock->m_pMyLock->m_NumLocks--;
			pthread_mutex_unlock(&pSafetyLock->m_pMyLock->mutex);
			/*

			AB 19 Apr 05.  Disable the killing of threads.  Just release the lock as above.
			bool bKilledAlready=false;
			list<pthread_t>::iterator itKilledThreads;
			for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
			{
			if( (*itKilledThreads) == pSafetyLock->m_thread )
			{
			bKilledAlready=true;
			break;
			}
			}
			#ifdef WIN32
			sprintf(Message,"killing thread: %p (>%d), killed already: %s",pSafetyLock->m_thread,pSafetyLock->m_LockNum,bKilledAlready ? "Y" : "N");
			#else
			sprintf(Message,"killing thread %ld (>%d), killed already: %s",
			pSafetyLock->m_thread,pSafetyLock->m_LockNum,bKilledAlready ? "Y" : "N");
			#endif

			if( !bKilledAlready )
			{
			listKilledPthreads.push_back(pSafetyLock->m_thread);
			}

			(*g_pmapLocks).erase(itMapLock++);
			*/
		}
		else
			++itMapLock;
		listMessages.push_back(string(Message));
	}

	// If we killed threads we have to go through the whole map all over again 
	// for each thread since there could have been other entries in the map
	if( listKilledPthreads.size()>0 )
	{

#ifdef UNDER_CE
#ifdef CENET
#ifndef DISABLE_REBOOTS
		KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
#endif
#endif
#endif
		listMessages.push_back("Killed thread: " + StringUtils::itos((int) listKilledPthreads.size()));
		list<pthread_t>::iterator itKilledThreads;
		for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
		{
			for(itMapLock=MutexTracking::GetMap()->begin();itMapLock!=MutexTracking::GetMap()->end();)
			{
				PlutoLock *pSafetyLock = (*itMapLock).second;
                                if( pthread_equal(pSafetyLock->m_thread, (*itKilledThreads) ))
				{
					listMessages.push_back("already killed (>" + StringUtils::itos(pSafetyLock->m_LockNum) + ")");
					MutexTracking::GetMap()->erase(itMapLock++);
				}
				else
					++itMapLock;
			}
		}


		for(itKilledThreads=listKilledPthreads.begin();itKilledThreads!=listKilledPthreads.end();++itKilledThreads)
		{
			pthread_t thread = (*itKilledThreads);
#ifndef UNDER_CE
#ifndef ANDROID
			pthread_cancel(thread);
#endif
#endif
		}

	}
	MutexTracking::UnLock();

#ifndef WINCE
	printf("ready to dump locks using logger: %p\n",LoggerWrapper::GetInstance());
#endif

	list<string>::iterator itMessages;
	for(itMessages=listMessages.begin();itMessages!=listMessages.end();++itMessages)
	{

		LoggerWrapper::GetInstance()->Write(LV_LOCKING,"logging message %s\n",(*itMessages).c_str());

		LoggerWrapper::GetInstance()->Write(LV_WARNING,(*itMessages).c_str());
	}
}

void PlutoLock::Relock()
{
	if( !m_bReleased )
	{

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failure relocking(%p): (>%d) %s:%d %s",&m_pMyLock->mutex, m_LockNum, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 

		return; // We don't have a lock
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{

		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "relocking (%p) (>%d) %s", &m_pMyLock->mutex,m_LockNum,m_sMessage.c_str());

		DoLock();
	}
	else
	{
		m_bReleased=false;
		pthread_mutex_lock(&m_pMyLock->mutex);
	}
#else
	m_bReleased=false;
	pthread_mutex_lock(&m_pMyLock->mutex);
#endif
	m_pMyLock->m_NumLocks++;
}


int PlutoLock::CondWait()
{
	if( !m_pMyLock->m_pthread_cond_t )
	{

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failure cond wait -- no condition (%p): %s:%d %s",&m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
		return EINVAL;
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "start cond wait %s:%d (%p) %s", m_sFileName.c_str(),m_Line,&m_pMyLock->mutex,m_sMessage.c_str());
#endif
	m_tTime = 0;
	m_pMyLock->m_NumLocks--;
	m_bReleased=true;
	int iResult = pthread_cond_wait(m_pMyLock->m_pthread_cond_t,&m_pMyLock->mutex); // This will unlock the mutex and lock it on awakening
	m_bReleased=false;
	m_pMyLock->m_NumLocks++;
	m_tTime = time(NULL);
	m_bGotLock=true;

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "end cond wait %s:%d (%p) %s", m_sFileName.c_str(),m_Line,&m_pMyLock->mutex,m_sMessage.c_str());
#endif
	return iResult;
}

int PlutoLock::TimedCondWait(timespec &ts)
{
	valid_timespec(ts);

	if( !m_pMyLock->m_pthread_cond_t )
	{

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failure cond timed wait -- no condition (%p): %s:%d %s",
			&m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
		return EINVAL;
	}

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
		timespec ts_now;
		gettimeofday(&ts_now,NULL);

		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "start cond %p timed wait till %u.%u now %u.%u %s:%d (%p) %s", 
			m_pMyLock->m_pthread_cond_t,
			ts.tv_sec,ts.tv_nsec,ts_now.tv_sec,ts_now.tv_nsec,
			m_sFileName.c_str(),m_Line, &m_pMyLock->mutex,m_sMessage.c_str());
	}
#endif
	m_tTime = 0;
	m_pMyLock->m_NumLocks--;
	m_bReleased=true;
	int iResult = pthread_cond_timedwait(m_pMyLock->m_pthread_cond_t,&m_pMyLock->mutex,&ts); // This will unlock the mutex and lock it on awakening
	m_bReleased=false;
	m_pMyLock->m_NumLocks++;
	m_tTime = time(NULL);
	m_bGotLock=true;

#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
		timespec ts_now;
		gettimeofday(&ts_now,NULL);
		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "end cond timed at %u.%u wait result: %d %s:%d (%p) %s", 
			ts_now.tv_sec,ts_now.tv_nsec,
			iResult, m_sFileName.c_str(),m_Line, &m_pMyLock->mutex,m_sMessage.c_str());
	}
#endif

	return iResult;
}

int PlutoLock::TimedCondWait(int Seconds,int Nanoseconds)
{
	timespec ts;
	gettimeofday(&ts,NULL);
#ifdef THREAD_LOG
	timespec ts_now = ts;
#endif
	ts.tv_sec+=Seconds;
	ts.tv_nsec+=Nanoseconds;
#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
	{
		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "TimedCondWait now %u.%u wait %u.%u till %u.%u (%p): %s:%d %s",
			ts_now.tv_sec,ts_now.tv_nsec,
			Seconds,Nanoseconds,
			ts.tv_sec,ts.tv_nsec,
			&m_pMyLock->mutex, m_sFileName.c_str(),m_Line,m_sMessage.c_str()); 
	}
#endif
	return TimedCondWait(ts);
}

void PlutoLock::DoLock()
{
	m_bGotLock=false;
	m_bReleased=false;
	if( !m_pMyLock->m_bInitialized )
	{

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "attempting to use un-initialized lock (%p) (>%d) %s: %s:%d %s",
			&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
		return;  // We just won't get the lock this first time
	}
#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "lock(%p) (>%d) %s: %s:%d %s",&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
#endif
	time_t t = time(NULL);

	// Try lock doesn't work under CE for some reason
#ifdef UNDER_CE
	pthread_mutex_lock(&m_pMyLock->mutex);
	m_bGotLock=true;
#else
	while( t+TRYLOCK_TIMEOUT_WARNING>time(NULL) && !m_bGotLock)
	{
		int iResult = pthread_mutex_trylock(&m_pMyLock->mutex);
		if( iResult==0 )
			m_bGotLock=true;
		else if( iResult!= EBUSY )
		{
			// Some other error condition
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "pthread_mutex_trylock returned %d  (%p) (>%d) %s: %s:%d %s", iResult,
				&m_pMyLock->mutex, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
			Sleep(1000);  // Something went terribly wrong.  Pause a second so we don't log these errors a zillion times in 5 seconds
		}

		if( m_bGotLock )
			break;
		Sleep(10);
	}
#endif
	if( !m_bGotLock )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get lock(%p) %s: %s:%d last used %s:%d thread: %p (>%d) %s",
			&m_pMyLock->mutex, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_pMyLock->m_sFileName.c_str(),m_pMyLock->m_Line,m_pMyLock->m_thread,m_pMyLock->m_LockNum,m_sMessage.c_str()); 
		DumpOutstandingLocks();
#ifndef WINCE
		cout << "Calling deadlock handler 1" << endl;
		cerr << "Calling deadlock handler 1" << endl;
#endif
		if( g_pDeadlockHandler )
			(*g_pDeadlockHandler)(this);
		pthread_mutex_lock(&m_pMyLock->mutex);
	}
	m_pMyLock->m_Line=m_Line;
	m_pMyLock->m_sFileName=m_sFileName;
	m_pMyLock->m_thread=pthread_self();
	m_pMyLock->m_LockNum=m_LockNum;
#ifdef THREAD_LOG
	if( !m_bLogErrorsOnly )
		LoggerWrapper::GetInstance()->Write(LV_LOCKING, "acquired(%p) #%d (>%d) %s %s:%d %s", 
			&m_pMyLock->mutex,m_pMyLock->m_NumLocks,m_LockNum,m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
#endif
}

void PlutoLock::Release()
{
	if( !m_bReleased )
	{
		pthread_mutex_unlock(&m_pMyLock->mutex);
		m_pMyLock->m_NumLocks--;
#ifdef THREAD_LOG
		if( !m_bLogErrorsOnly )
			LoggerWrapper::GetInstance()->Write(LV_LOCKING, "unlock(%p) #%d (>%d) %s: %s:%d %s", &m_pMyLock->mutex, m_pMyLock->m_NumLocks, m_LockNum, m_pMyLock->m_sName.c_str(), m_sFileName.c_str(),m_Line,m_sMessage.c_str());
#endif
	}
	m_bReleased=true;
}

#ifdef WIN32
void filetime_to_timespec(const FILETIME *ft, struct timespec *ts)
/*
* -------------------------------------------------------------------
* converts FILETIME (as set by GetSystemTimeAsFileTime), where the time is
* expressed in 100 nanoseconds from Jan 1, 1601,
* into struct timespec
* where the time is expressed in seconds and nanoseconds from Jan 1, 1970.
* -------------------------------------------------------------------
*/
{
	ts->tv_sec = (int)((*(LONGLONG *)ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET) / 10000000);
	ts->tv_nsec = (int)((*(LONGLONG *)ft - PTW32_TIMESPEC_TO_FILETIME_OFFSET - ((LONGLONG)ts->tv_sec * (LONGLONG)10000000)) * 100);
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	SYSTEMTIME st;
	/* Get the current system time */
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	timespec ts;
	filetime_to_timespec(&ft,&ts);
	timespec_to_timeval(&ts,tv);
	return 0;
}
#endif

int gettimeofday(struct timespec *ts, struct timezone *tz)
{
	timeval tv_current;
	int i=gettimeofday(&tv_current,NULL);
	timeval_to_timespec(&tv_current,ts);
	return i;
}

// timespec operators
bool operator < (const timespec & t1, const timespec & t2)
{
	if (t1.tv_sec != t2.tv_sec)
		return t1.tv_sec < t2.tv_sec;
	return t1.tv_nsec < t2.tv_nsec;
}

bool operator == (const timespec & t1, const timespec & t2)
{
	return t1.tv_sec == t2.tv_sec && t1.tv_nsec == t2.tv_nsec;
}

bool operator <= (const timespec & t1, const timespec & t2)
{
	return t1 == t2 || t1 < t2;
}

bool operator > (const timespec & t1, const timespec & t2)
{
	return ! (t1 <= t2);
}

bool operator >= (const timespec & t1, const timespec & t2)
{
	return t1 == t2 || t1 > t2;
}

bool operator != (const timespec & t1, const timespec & t2)
{
	return ! (t1 == t2);
}

timespec & operator += (timespec & t1, const timespec & t2)
{
	t1.tv_sec += t2.tv_sec;
	t1.tv_nsec += t2.tv_nsec;

	t1.tv_sec += t1.tv_nsec / 1000000000;
	t1.tv_nsec = t1.tv_nsec % 1000000000;

	return t1;
}

timespec & operator -= (timespec & t1, const timespec & t2)
{
	t1.tv_sec -= t2.tv_sec;
	t1.tv_nsec -= t2.tv_nsec;

	if( t1.tv_nsec<0 )
	{
		t1.tv_sec--;
		t1.tv_nsec += 1000000000;
	}

	return t1;
}

timespec operator + (const timespec & t1, const timespec & t2)
{
	timespec result = { 0, 0 };

	result += t1;
	result += t2;

	return result;
}

timespec ms_to_timespec(unsigned long ts)
{
	timespec result;

	result.tv_sec = ts / 1000;
	result.tv_nsec = ts % 1000 * 1000000;

	return result;
}

timespec & operator += (timespec & t1, long milliseconds)
{
	timespec t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000000 };
	return t1 += t2;
}

timespec & operator -= (timespec & t1, long milliseconds)
{
	timespec t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000000 };
	return t1 -= t2;
}

timespec operator - (const timespec & t1, const timespec & t2)
{
	timespec result = { 0, 0 };

	result += t1;
	result -= t2;

	return result;
}

// timeval operators
bool operator < (const timeval & t1, const timeval & t2)
{
	if (t1.tv_sec != t2.tv_sec)
		return t1.tv_sec < t2.tv_sec;
	return t1.tv_usec < t2.tv_usec;
}

bool operator == (const timeval & t1, const timeval & t2)
{
	return t1.tv_sec == t2.tv_sec && t1.tv_usec == t2.tv_usec;
}

bool operator <= (const timeval & t1, const timeval & t2)
{
	return t1 == t2 || t1 < t2;
}

bool operator > (const timeval & t1, const timeval & t2)
{
	return ! (t1 <= t2);
}

bool operator >= (const timeval & t1, const timeval & t2)
{
	return t1 == t2 || t1 > t2;
}

bool operator != (const timeval & t1, const timeval & t2)
{
	return ! (t1 == t2);
}

timeval & operator += (timeval & t1, const timeval & t2)
{
	t1.tv_sec += t2.tv_sec;
	t1.tv_usec += t2.tv_usec;

	t1.tv_sec += t1.tv_usec / 1000000;
	t1.tv_usec = t1.tv_usec % 1000000;

	return t1;
}

timeval & operator -= (timeval & t1, const timeval & t2)
{
	t1.tv_sec -= t2.tv_sec;
	t1.tv_usec -= t2.tv_usec;

	if( t1.tv_usec<0 )
	{
		t1.tv_sec--;
		t1.tv_usec += 1000000;
	}

	return t1;
}

timeval operator + (const timeval & t1, const timeval & t2)
{
	timeval result = { 0, 0 };

	result += t1;
	result += t2;

	return result;
}

timeval ms_to_timeval(unsigned long ts)
{
	timeval result;

	result.tv_sec = ts / 1000;
	result.tv_usec = ts % 1000 * 1000;

	return result;
}

timeval & operator += (timeval & t1, long milliseconds)
{
	timeval t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000 };
	return t1 += t2;
}

timeval & operator -= (timeval & t1, long milliseconds)
{
	timeval t2 = { milliseconds / 1000, (milliseconds % 1000) * 1000 };
	return t1 -= t2;
}

timeval operator - (const timeval & t1, const timeval & t2)
{
	timeval result = { 0, 0 };

	result += t1;
	result -= t2;

	return result;
}

// timespec <-> timeval
void timespec_to_timeval(const timespec *ts_source,timeval *tv_dest)
{
	tv_dest->tv_sec = ts_source->tv_sec;
	tv_dest->tv_usec = ts_source->tv_nsec / 1000;
}

void timeval_to_timespec(const timeval *tv_source,timespec *ts_dest)
{
	ts_dest->tv_sec = tv_source->tv_sec;
	ts_dest->tv_nsec = tv_source->tv_usec * 1000;
}

void valid_timespec(struct timespec &ts)
{
	if(ts.tv_nsec > 1000 * 1000000)
	{
		ts.tv_sec += ts.tv_nsec / (1000 * 1000000);
		ts.tv_nsec = ts.tv_nsec % (1000 * 1000000);
	}
}

pluto_pthread_mutex_t MutexTracking::m_MutexForGetInstance("MutexTracking::GetInstance");  // Used just once so there's only one instance
