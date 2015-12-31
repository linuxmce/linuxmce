/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file IOMutex.h
Mutex class.
*/
#ifndef CM11AMUTEX_H
#define CM11AMUTEX_H

#include <pthread.h>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class IOMutex
I/O Mutex.
*/
class IOMutex {
public:
    IOMutex();
    ~IOMutex();

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
