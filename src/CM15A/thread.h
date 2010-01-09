/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     Author: Nosilla99
     Name  : CM15A
     Descr : Active Home CM15A USB X10 PC Interface and RF transceiver
             DCE Device Handler
     Date  : 11.11.2009
     File  : CM15A/thread.h

*/

#ifndef MYTHTVTHREAD_H
#define MYTHTVTHREAD_H

#include <pthread.h>

namespace CM15ADEV {

class Thread {
public:

    /** Standard constructor */
    Thread();

    /** Standard Constructoor */
    virtual ~Thread();

    /** The start method for the thread */
	virtual int Run(bool wait = true);

    /** Wait for thread completion */
	void Wait(bool requeststop = false);

    /** Test the thread to see if it is active/Running */
	bool isRunning();

protected:
	virtual void* _Run();

	bool isStopRequested() {
		return requeststop_;
	};

protected:
    /** Handle the thread startup */
	virtual bool handleStartup() {
		return true;
	};
    /** Handle the thread termination */
	virtual void handleTerminate() {};

private:
    /** requested stop flag */
	bool requeststop_;

    /** A place to hold the thread ID.  */
	pthread_t threadid_;

    /** _threadproc @param arg a private void pointer to the Thread class instance.  */
	static void* _threadproc(void *arg);
};

};

#endif
