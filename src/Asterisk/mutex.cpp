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

namespace ASTERISK {

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
