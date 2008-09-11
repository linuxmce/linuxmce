/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "ThreadedClass.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace nsThreadedClass;
using namespace DCE;

ThreadedClass::ThreadedClass() : m_ThreadMutex("ThreadedClass", true)
{
	m_bQuit=m_bThreadRunning=false;
	m_pthread=0;
	m_iTimeoutSecondsForStopThread=0;
	pthread_cond_init( &m_ThreadCondition, NULL );
    pthread_mutexattr_init( &m_ThreadAttribute );
    pthread_mutexattr_settype( &m_ThreadAttribute, PTHREAD_MUTEX_RECURSIVE_NP );
	m_ThreadMutex.Init( &m_ThreadAttribute, &m_ThreadCondition );
}

ThreadedClass::~ThreadedClass()
{
	StopThread();
	pthread_mutex_destroy(&m_ThreadMutex.mutex);
    pthread_mutexattr_destroy(&m_ThreadAttribute);
}

bool ThreadedClass::StopThread(int iTimeout)
{
	PLUTO_SAFETY_LOCK(tm,m_ThreadMutex);

	m_bQuit=true;
	if( m_pthread==0 )
		return true;  // The thread's not running anyway

	if( iTimeout==-1 )
		iTimeout=m_iTimeoutSecondsForStopThread;

	BroadcastCond();
	if( iTimeout<1 )
	{
		tm.Release();
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ThreadedClass::StopThread waiting to join");
		pthread_join(m_pthread, NULL);
		m_pthread=0;
		return true;
	}

	time_t tTimeout = time(NULL) + iTimeout;
	while(tTimeout>=time(NULL))
	{
		if( m_bThreadRunning==false)
			return true; // Okay the thread ended on it's own

		// Keep trying to wake it up
		BroadcastCond();
		tm.Release();
		Sleep(10); // Wait 10ms
		tm.Relock();
	}

	if( m_bThreadRunning==false )
		pthread_join(m_pthread, NULL);  // Cleanup this thread
	return m_bThreadRunning==false; // If the thread is still running we failed to kill it
}

void* ThreadedClassThread( void* param ) // renamed to cancel link-time name collision in MS C++ 7.0 / VS .NET 2002
{
	ThreadedClass *p = (ThreadedClass*)param;
	LoggerWrapper::GetInstance()->Write(LV_SOCKET,"ThreadedClassThread::ThreadedClassThread running internal run");
	p->InternalRun();
	
	LoggerWrapper::GetInstance()->Write(LV_SOCKET,"Exiting ThreadedClassThread thread...");
	return NULL;
}

bool ThreadedClass::StartThread()
{
	m_bThreadRunning=true;
	if(pthread_create( &m_pthread, NULL, ThreadedClassThread, (void*)this) )
	{
		m_bThreadRunning=false;
		m_pthread=0;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create ThreadedClass::StartThread thread" );
		return false;
	}
	return true;
}

void ThreadedClass::InternalRun()
{
	if( !m_bQuit && m_bThreadRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_SOCKET,"ThreadedClass::InternalRun");
		Run();
	}
	LoggerWrapper::GetInstance()->Write(LV_SOCKET,"ThreadedClass::InternalRun ended");
	m_bThreadRunning=false;
}
