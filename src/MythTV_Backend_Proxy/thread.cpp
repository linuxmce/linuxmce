/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "thread.h"
//#include "DCE/Logger.h"

namespace MYTHTV
{
	Thread::Thread()
		: requeststop_(false), threadid_(0)
	{
	}


	Thread::~Thread()
	{
	}

	bool Thread::isRunning()
	{
		return (threadid_ != 0);
	}

	bool Thread::isStopRequested()
	{
		return requeststop_;
	};

	int Thread::Run(bool wait)
	{
		if(!handleStartup())
			return false;

		int ret = pthread_create(&threadid_, NULL, _threadproc, (void*)this);

		if(ret != 0)
			threadid_ = 0;

		if(wait)
			Wait();

		return ret;
	}

	void Thread::Wait(bool requeststop)
	{
		if(threadid_ != 0) {
			requeststop_ = requeststop;
			pthread_join(threadid_, 0);
			threadid_ = 0;
		}
	}

	void* Thread::_Run()
	{
		return 0;
	}

	void* Thread::_threadproc(void *arg)
	{
		Thread* pme = (Thread*)arg;

		void* pret = pme->_Run();
		pme->threadid_ = 0;
		pme->requeststop_ = false;
		pme->handleTerminate();
		pthread_exit(pret);
		return 0;
	}

	bool Thread::handleStartup()
	{
		return true;
	};
};
