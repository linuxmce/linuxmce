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
#include "DCE/Logger.h"

using namespace DCE;

namespace CTX35DEV {

Thread::Thread()
	: threadid_(0), requeststop_(false)
{
}


Thread::~Thread() {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Destroying Thread - before Wait");
	Wait(true);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Destroying Thread - After Wait");
}

bool
Thread::isRunning() {
	return (threadid_ != 0);
}

int 
Thread::Run(bool wait) {
	if(!handleStartup()) {
		return false;	
	}
	
	int ret = pthread_create(&threadid_, NULL, _threadproc, (void*)this);
	if(ret == 0) {
//		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread %d created", threadid);
	} else {
//		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error creating thread");
		threadid_ = 0;
	}
	
	if(wait) {
		Wait();
	}
	return ret;
}

void 
Thread::Wait(bool requeststop) {
	if(threadid_ != 0) {
		requeststop_ = requeststop;
		pthread_join(threadid_, 0);
		threadid_ = 0;
	}
}

void* 
Thread::_Run() {
	return 0;
}

void* Thread::_threadproc(void *arg) {
	Thread* pme = (Thread*)arg;

	void* pret = pme->_Run();
	pme->threadid_ = 0;
	pme->requeststop_ = false;
	pme->handleTerminate();
	pthread_exit(pret);
	return 0;
}

};
