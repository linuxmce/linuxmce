/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file IOEvent.h
Base classe for IOEvent.
*/
#ifndef EIBBUSEVENT_H
#define EIBBUSEVENT_H

#include <pthread.h>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class IOEvent
For I/O Events
*/
class IOEvent
{
public:
    IOEvent();
    ~IOEvent();

public:
	void Signal();
	bool Wait(int milisecs = -1);

private:
	pthread_cond_t event_;
	pthread_mutex_t mutex_;
};

};

#endif
