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
// C++ Implementation: runtimeconfig
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "runtimeconfig.h"

using namespace std;

namespace ASTERISK {

RuntimeConfig* RuntimeConfig::sinstance = 0;

RuntimeConfig::RuntimeConfig() {}
RuntimeConfig::~RuntimeConfig() {}

RuntimeConfig* RuntimeConfig::getInstance() {
	static RuntimeConfig config;
	
	LOCK_OBJ(config); /* perform lock on config as it is static */
	
	if(sinstance == 0) {
		sinstance = &config;
	}

	UNLOCK_OBJ(config);
			
	return sinstance;
}

void RuntimeConfig::regOriginateCall(string phoneid) {
	LOCKED_OP(
		origsrcs[phoneid] = phoneid;
	);
}

void RuntimeConfig::unregOriginateCall(string phoneid) {
	map<string, string>::iterator it;
	LOCKED_OP(	
		it = origsrcs.find(phoneid);
		if(it != origsrcs.end()) {
			origsrcs.erase(it);
		}
	);
}

bool RuntimeConfig::isCallOriginating(string phoneid) {
	bool ret = false;
	map<string, string>::iterator it;
	
	LOCKED_OP(
		it = origsrcs.find(phoneid);
		if(it != origsrcs.end()) {
			ret = true;
		}
	);
	return ret;
}


};
