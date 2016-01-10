/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file thread.h
Handle the X10 CM11A Thread base class.
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
#ifndef MYTHTVTHREAD_H
#define MYTHTVTHREAD_H

#include <pthread.h>

/**
@namespace CM11ADEV
The X10 CM11A Controller device namespace.
*/
namespace CM11ADEV {

/**
@class Thread
This is a thread class that is used only by the CM11A module.

It is probably redundant code that could benefit from refactoring.
*/
class Thread {
public:

    /** Standard constructor */
    Thread();

    /** Standard Constructoor */
    virtual ~Thread();

    /** The start method for the thread
    @param wait is a flag to have the thread wait.
    */
	virtual int Run(bool wait = true);

    /** Wait for thread completion
    @param requeststop is a flag to stop the thread from further running.
    */
	void Wait(bool requeststop = false);

    /** Test the thread to see if it is active/Running
    @returns the running state
    */
	bool isRunning();

protected:
	virtual void* _Run();

	bool isStopRequested() {
		return requeststop_;
	};

protected:
    /** Handle the thread startup
    */
	virtual bool handleStartup() {
		return true;
	};
    /** Handle the thread termination
    */
	virtual void handleTerminate() {};

private:
    /** requested stop flag
    */
	bool requeststop_;

    /** A place to hold the thread ID.
    */
	pthread_t threadid_;

    /** _threadproc
    @param arg a private void pointer to the Thread class instance.


    */
	static void* _threadproc(void *arg);
};

};

#endif
