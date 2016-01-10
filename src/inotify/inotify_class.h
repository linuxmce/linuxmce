/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef WIN32
	#include "inotify.h"
#else
	#define __s32 long
	#define __u32 unsigned long
#endif

#include <string>
#include <list>
using namespace std;

class cpp_inotify_event
{
	public:
		__s32 wd;
		__u32 mask;
		string name;

		string tostring();
};

static const int inotify_buffer_size = 4096;

class inotify
{
	private:
		int inotify_fd;
		list<cpp_inotify_event> event_queue;

		void convert_buffer(char * buffer, int bytes);
		
	public:
		inotify();
		~inotify();
		int watch(string path, __u32 mask);
		void unwatch(int watch_id);
		bool pending_events();
		cpp_inotify_event get_event();
};
