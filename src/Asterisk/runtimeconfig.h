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
// C++ Interface: runtimeconfig
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKRUNTIMECONFIG_H
#define ASTERISKRUNTIMECONFIG_H

#include "mutex.h"
#include <map>
#include <string>

#include "threadsafeobj.h"

namespace ASTERISK {

/**
@author 
*/
class RuntimeConfig : public ThreadSafeObj {
public:
    RuntimeConfig();
    ~RuntimeConfig();

public:
	static RuntimeConfig* getInstance();

public:
	void regOriginateCall(std::string phoneid);
	void unregOriginateCall(std::string phoneid);
	bool isCallOriginating(std::string phoneid);
	
private:
	std::map<std::string, std::string> origsrcs;

// runtime config instance
private:
	static RuntimeConfig* sinstance;
};

};

#endif
