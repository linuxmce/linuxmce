/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "NetworkIOPool.h"

namespace DCE {

NetworkIOPool::NetworkIOPool(bool usemain) 
	: IOPool(usemain), defstate_(this)
{
	setState(&defstate_);
}


NetworkIOPool::~NetworkIOPool()
{
}

IOConnection* 
NetworkIOPool::getConnection() {
	return &netconn;
}


bool 
NetworkIOPool::handleIteration() {
	return IOPool::handleIteration();
}


void 
NetworkIOPool::NetworkIOState::handleOpen(IOConnection* pconn) {
}

void 
NetworkIOPool::NetworkIOState::handleRead(IOConnection* pconn) {
}

void 
NetworkIOPool::NetworkIOState::handleClose(IOConnection* pconn) {
}

};
