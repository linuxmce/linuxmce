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
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mutex.h"

namespace CTX35DEV {

Mutex::Mutex() {
	pthread_mutex_init(&mutexid, 0);
}


Mutex::~Mutex(){
	pthread_mutex_destroy(&mutexid);
}

int 
Mutex::Lock() {
	return pthread_mutex_lock(&mutexid);
}

void 
Mutex::Unlock() {
	pthread_mutex_unlock(&mutexid);
}

};
