/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "IOEvent.h"

#include <time.h>

namespace DCE {

IOEvent::IOEvent() {
	pthread_cond_init(&event_, 0);
	pthread_mutex_init(&mutex_, 0);
}


IOEvent::~IOEvent() {
	pthread_mutex_destroy(&mutex_);
		pthread_cond_destroy(&event_);
}

void 
IOEvent::Signal() {
	pthread_cond_signal(&event_);
}

bool 
IOEvent::Wait(int milisecs) {
	if(milisecs < 0) {
		pthread_cond_wait(&event_, &mutex_);
		return true;
	} else {
		struct timespec timespec;
		clock_gettime(CLOCK_REALTIME, &timespec);
		
//		return (sem_timedwait(&event_, hrt2ts(clock_gethrtime(CLOCK_REALTIME) + 1000*1000*milisecs)) == 0);
		
		timespec.tv_sec += milisecs / 1000;
		timespec.tv_nsec += (milisecs % 1000) * 1000 * 1000;
		
		return (pthread_cond_timedwait(&event_, &mutex_, &timespec) == 0);
	}
}

};
