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

namespace ASTERISK {

Thread::Thread()
	: m_bQuit(false), isrunning(false)
{
	threadid = 0;
}


Thread::~Thread() {
	Wait();
}

bool
Thread::isRunning() {
	return isrunning;
}

int 
Thread::Run(bool wait) {
	isrunning = true;
	
	handleStartup();
	
	int ret = pthread_create(&threadid, NULL, _threadproc, (void*)this);
	if(ret == 0) {
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread %d created", threadid);
	} else {
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error creating thread");
	}
	
	if(wait) {
		Wait();
	}
	return ret;
}

void 
Thread::Wait() {
	m_bQuit = true;

	if(threadid)
	{
		pthread_join(threadid, 0);
		threadid = 0;
	}
}

void* 
Thread::_Run() {
	return 0;
}

void* Thread::_threadproc(void *arg) {
	Thread* pme = (Thread*)arg;

	void* pret = pme->_Run();
	pme->isrunning = false;
	pme->handleTerminate();
	pthread_exit(pret);
	return 0;
}

};
