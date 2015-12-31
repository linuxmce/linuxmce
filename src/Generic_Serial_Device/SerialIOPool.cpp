/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "SerialIOPool.h"

#include "DCE/Logger.h"

namespace DCE {

SerialIOPool::SerialIOPool(bool usemain) 
	: IOPool(usemain), defstate_(this)
{
	setState(&defstate_);
}

SerialIOPool::~SerialIOPool() {
}

IOConnection* 
SerialIOPool::getConnection() {
	return &serconn;
}


bool 
SerialIOPool::handleIteration() {
	return IOPool::handleIteration();
}


void 
SerialIOPool::SerialIOState::handleOpen(IOConnection* pconn) {
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Serial connection opened.");
}

void 
SerialIOPool::SerialIOState::handleRead(IOConnection* pconn) {
//    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to read from Serial Port.");
}

void 
SerialIOPool::SerialIOState::handleClose(IOConnection* pconn) {
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Serial connection closed.");
}

};
