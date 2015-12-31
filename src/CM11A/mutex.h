/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file mutex.h
 *@brief Handle the X10 CM11A mutex.
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
#ifndef CM11AMUTEX_H
#define CM11AMUTEX_H

#include <pthread.h>

/**
@namespace CM11ADEV
The X10 CM11A Controller device namespace.
*/
namespace CM11ADEV {

/**
@class Mutex
This class implements Mutexes for X-10 controller CM11a.
*/
class Mutex{
public:
    /** Standard constructor */
    Mutex();
    /** Standard destructor */
    ~Mutex();

public:
    int Lock();
    void Unlock();

    pthread_mutex_t getHandle() const {
		return mutexid;
    }

public:
	pthread_mutex_t mutexid;
};

};

#endif
