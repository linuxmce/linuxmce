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
 *@brief Handle the X10 CTX35 mutex.
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
#ifndef CTX35MUTEX_H
#define CTX35MUTEX_H

#include <pthread.h>

/**
@namespace CTX35DEV
The X10 CTX35 Controller device namespace.
*/
namespace CTX35DEV {

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
