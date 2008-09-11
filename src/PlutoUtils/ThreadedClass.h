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

/*

	This is a helper class you can derive from if you want a class that has a Run() method.
	Call the StartThread() function to spawn the thread and call Run(), and the StopThread()
	when you want it to shutdown, which is does by setting m_bQuit.  You should implement Run()
	and exit when m_bQuit is set to true;

*/

/**
 * @file ThreadedClass.h
 Header file for Threading class.
 */
#ifndef ThreadedClass_h
#define ThreadedClass_h

#include <string>
using namespace std;
#include "PlutoUtils/MultiThreadIncludes.h"

/**
@namespace nsThreadedClass
For ???.
*/
namespace nsThreadedClass
{

/** @class ThreadedClass
 Class for Threading
 */
	class ThreadedClass
	{
	private:
		bool m_bThreadRunning; // Use this rather than relying solely on pthread_join so we can have a timeout

	protected:
		bool m_bQuit; // Set to true to make the thread stop
		int m_iTimeoutSecondsForStopThread; // How long StopThread should wait for the thread to quit, or 0 for indefinate
		pthread_t m_pthread; /** < the thread id for the tread that's processing the jobs */
		pthread_cond_t m_ThreadCondition; /** < condition to wakeup when something has changed */
	    pluto_pthread_mutex_t m_ThreadMutex;
	    pthread_mutexattr_t m_ThreadAttribute;

	public:
		ThreadedClass();
		virtual ~ThreadedClass();

		bool StartThread();  // Returns false if it fails to start the thread
		bool StopThread(int iTimeout=-1);  // Returns false if it fails to stop the thread.  Don't specify a timeout to use the default
		void SetTimeoutForStopThread(int iSeconds) { m_iTimeoutSecondsForStopThread=iSeconds; }

		virtual void InternalRun();
		bool ThreadRunning() { return m_bThreadRunning; }
		void BroadcastCond() { pthread_cond_broadcast( &m_ThreadCondition ); }
		virtual void Run()=0;

		pluto_pthread_mutex_t *m_ThreadMutex_get() { return &m_ThreadMutex; }
	};
};

#endif
