/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file IOThread.h
Threads?
*/
#ifndef MYTHTVTHREAD_H
#define MYTHTVTHREAD_H

#include <pthread.h>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class IOThread
base class for?
*/
class IOThread {
public:
    IOThread(bool usemain = false); // set true to use main thread
    virtual ~IOThread();

	int Run(bool wait = true);
	void Wait(bool requeststop = false);
	bool isRunning();

protected:
	virtual void* _Run();

	bool isStopRequested() {
		return requeststop_;
	};

protected:
	virtual bool handleBeforeStartup() { return true; };
	virtual void handleAfterTerminate() {};
	virtual bool handleStartup() { return true; };
	virtual void handleTerminate() {};

private:
	bool usemain_;
	pthread_t threadid_;
	bool requeststop_;
	static void* _threadproc(void *arg);
};

};

#endif
