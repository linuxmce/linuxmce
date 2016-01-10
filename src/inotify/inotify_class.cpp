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
#include <sys/ioctl.h>
#endif

#include <cstdlib>
#include <cstring>

#include "inotify_class.h"

#ifndef WIN32

#include "inotify.h"
#include "inotify-syscalls.h"

#include <errno.h>
#include <stdio.h>

inotify::inotify()
{
	inotify_fd = inotify_init();
	if (inotify_fd == -1)
		throw string("Unable to get inotify handler");
}

inotify::~inotify()
{
	close(inotify_fd);
}

int inotify::watch(string path, __u32 mask)
{
	int wd = inotify_add_watch(inotify_fd, path.c_str(), mask);
	if (wd == -1)
		throw string("Failed to set watch");
	return wd;
}

void inotify::unwatch(int watch_id)
{
	if (inotify_rm_watch(inotify_fd, watch_id) == -1)
		throw string("Failed to remove watch");
}

bool inotify::pending_events()
{
	if (event_queue.size() > 0)
		return true;
	int available_bytes = 0;
	if (ioctl(inotify_fd, FIONREAD, &available_bytes) == -1)
		throw string("ioctl failed");
	if (available_bytes > 0)
		return true;
	return false;
}

cpp_inotify_event inotify::get_event()
{
	if (event_queue.size() == 0)
	{
		char buffer[inotify_buffer_size];

		memset(buffer, 0, inotify_buffer_size);
		int bytes_read;
		do
		{
			errno = 0;
			if ((bytes_read = read(inotify_fd, buffer, inotify_buffer_size)) == -1 && errno != EINTR)
				throw string("read failed");
		} while (errno == EINTR);
		convert_buffer(buffer, bytes_read);
	}

	cpp_inotify_event retval = event_queue.front();
	event_queue.pop_front();
	return retval;
}

/*private*/ void inotify::convert_buffer(char * buffer, int bytes)
{
	struct inotify_event * pevent = NULL;
	cpp_inotify_event cpp_event;
	int buffer_pos = 0;

	while (buffer_pos < bytes)
	{
		pevent = (struct inotify_event *) &buffer[buffer_pos];
		cpp_event.wd = pevent->wd;
		cpp_event.mask = pevent->mask;
		if (pevent->len == 0)
			cpp_event.name = string("");
		else
			cpp_event.name = string(pevent->name);
		
		event_queue.push_back(cpp_event);
		buffer_pos += sizeof(struct inotify_event) + pevent->len;
	}
}

string cpp_inotify_event::tostring()
{
	char buffer[1024];
	snprintf(buffer, 1024, "WD: %d; Name: %s;", wd, name.c_str());
	
	string result = string(buffer);
	if (mask & IN_ACCESS)
		result += " ACCESS";
	if (mask & IN_MODIFY)
		result += " MODIFY";
	if (mask & IN_ATTRIB)
		result += " ATTRIB";
	if (mask & IN_CLOSE)
		result += " CLOSE";
	if (mask & IN_OPEN)
		result += " OPEN";
	if (mask & IN_MOVED_FROM)
		result += " MOVE_FROM";
	if (mask & IN_MOVED_TO)
		result += " MOVE_TO";
	if (mask & IN_DELETE)
		result += " DELETE";
	if (mask & IN_CREATE)
		result += " CREATE";
	if (mask & IN_DELETE_SELF)
		result += " DELETE_SELF";
	if (mask & IN_UNMOUNT)
		result += " UNMOUNT";
	if (mask & IN_Q_OVERFLOW)
		result += " Q_OVERFLOW";
	if (mask & IN_IGNORED)
		result += " IGNORED" ;

	if (mask & IN_ISDIR)
		result += " (dir)";
	else
		result += " (file)";

	return result;
}

#else

	//dummy implementation
	void inotify::convert_buffer(char * buffer, int bytes){}
	inotify::inotify() {}
	inotify::~inotify(){}
	int inotify::watch(string path, __u32 mask) {return 0;}
	void inotify::unwatch(int watch_id) {}
	bool inotify::pending_events() {return false;}
	cpp_inotify_event inotify::get_event() {cpp_inotify_event retval; return retval;}

#endif

