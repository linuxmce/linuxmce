/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file event.h
Header for Event class.
*/
//
// C++ Interface: event
//
// Description:
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSEVENT_H
#define EIBBUSEVENT_H

#include <pthread.h>

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

/** @class Event
For Events.
*/
class Event
{
public:
    Event();
    ~Event();

public:
	void Signal();
	bool Wait(int milisecs = -1);

private:
	pthread_cond_t event_;
	pthread_mutex_t mutex_;
};

};

#endif
