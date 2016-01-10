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
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTHREAD_H
#define ASTERISKTHREAD_H

#include <pthread.h>

namespace ASTERISK {

/**
@author 
*/
class Thread {
public:
    Thread();
    virtual ~Thread();

	virtual int Run(bool wait = true);
	void Wait();
	bool isRunning();
	
protected:
	virtual void* _Run();

protected:
	virtual void handleStartup() {};
	virtual void handleTerminate() {};

protected:
	bool m_bQuit;

private:
	bool isrunning;
	pthread_t threadid;
	static void* _threadproc(void *arg);
};

};

#endif
