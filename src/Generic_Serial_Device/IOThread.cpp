/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "IOThread.h"
//#include "DCE/Logger.h"

namespace DCE {

IOThread::IOThread(bool usemain)
	: usemain_(usemain), threadid_(0), requeststop_(false)
{
}


IOThread::~IOThread() {
}

bool
IOThread::isRunning() {
	return (threadid_ != 0);
}

int 
IOThread::Run(bool wait) {
	int ret = 0;
	if(!handleBeforeStartup()) {
		return -1;
	}
	
	if(!usemain_) {
		ret = pthread_create(&threadid_, NULL, _threadproc, (void*)this);
	} else {
		_threadproc(this);
	}
	
	if(ret != 0) {
		threadid_ = 0;
	}
	
	if(wait) {
		Wait();
	}
	return ret;
}

void 
IOThread::Wait(bool requeststop) {
	if(usemain_) {
		return;
	}
	if(threadid_ != 0) {
		requeststop_ = requeststop;
		pthread_join(threadid_, 0);
		threadid_ = 0;
	}
	
	handleAfterTerminate();
}

void* 
IOThread::_Run() {
	return 0;
}

void* IOThread::_threadproc(void *arg) {
	IOThread* pme = (IOThread*)arg;
	
	if(pme->handleStartup()) {
		void* pret = pme->_Run();
		pme->threadid_ = 0;
		pme->requeststop_ = false;
		pme->handleTerminate();
		if(pme->threadid_ != 0) {
			pthread_exit(pret);
		}
	}
	return 0;
}

};
