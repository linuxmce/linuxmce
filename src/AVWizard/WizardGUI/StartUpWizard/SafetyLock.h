/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef SAFETYLOCK_H
#define SAFETYLOCK_H

/**
Safety lock via mutexes class

	@author Ciprian Mustiata <ciprian.m@plutohome.com>
*/
#include <pthread.h>

class SafetyLock
{
	pthread_mutex_t* Mutex;
public:
	SafetyLock(pthread_mutex_t* Mutex);
	virtual ~SafetyLock();
};

#endif
