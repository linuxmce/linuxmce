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
#include "condition.h"

namespace ASTERISK {

Condition::Condition()
{
	pthread_cond_init(&cond, 0);
}


Condition::~Condition() {
	pthread_cond_destroy(&cond);
}

void Condition::Wait() {
	pthread_cond_wait(&cond, &m.getHandle());
}

void Condition::Signal() {
	pthread_cond_signal(&cond);
}

};
