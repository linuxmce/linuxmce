/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyDeviceWrapper.h"

namespace DCE {

RubyDeviceWrapper::RubyDeviceWrapper()
	: devid_(0), parent_(NULL)
{
}


RubyDeviceWrapper::~RubyDeviceWrapper()
{
}

RubyDeviceWrapper*
RubyDeviceWrapper::FindChildDevice(int devid) {
	for(std::map<int, RubyDeviceWrapper>::iterator it = childdevices_.begin();
				it != childdevices_.end(); it++) {
		if((*it).first == devid) {
			return &(*it).second;
		}
	}
	return NULL;
}

};
