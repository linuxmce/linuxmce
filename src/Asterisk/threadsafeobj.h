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
// C++ Interface: threadsafeobj
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKTHREADSAFEOBJ_H
#define ASTERISKTHREADSAFEOBJ_H

#include "mutex.h"

namespace ASTERISK {

/**
@author 
*/
class ThreadSafeObj {
public:
    ThreadSafeObj();
    ~ThreadSafeObj();

public:
	void Lock();
	void Unlock();

private:
	Mutex m; // safe guard
};

#define LOCKED_OP(a) Lock();a;Unlock();

#define LOCKED_OP_BEGIN Lock()
#define LOCKED_OP_END Unlock();

#define LOCK_OBJ(a) a.Lock();
#define UNLOCK_OBJ(a) a.Unlock();
};

#endif
