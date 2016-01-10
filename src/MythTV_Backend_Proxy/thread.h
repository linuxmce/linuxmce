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

namespace MYTHTV
{
	/**
	@author
	*/
	class Thread
	{
	public:
		Thread();
		virtual ~Thread();

		virtual int Run(bool wait = true);
		void Wait(bool requeststop = false);
		bool isRunning();

	protected:
		virtual void* _Run();
		bool isStopRequested();

	protected:
		virtual bool handleStartup();
		virtual void handleTerminate() = 0;

	private:
		bool requeststop_;
		pthread_t threadid_;
		static void* _threadproc(void *arg);
	};
};

#endif
