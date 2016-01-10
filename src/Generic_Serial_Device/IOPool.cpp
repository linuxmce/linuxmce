/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "IOPool.h"
#include "DCE/Logger.h"

#define FAIL_SLEEP_TIME				2000
#define DATA_AVAILABLE_WAIT_TIME	100

namespace DCE {

IOPool::IOPool(bool usemain)
	: LoopStateMachine(usemain) 
{}

IOPool::~IOPool() {
}

bool 
IOPool::handleStartup() {
	IOConnection* pconn = getConnection();
	if(pconn) {
		if( pconn->Open()==false )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IOPool::handleStartup Open() failed");
	}
	return LoopStateMachine::handleStartup();
}

bool 
IOPool::handleIteration() {
	IOConnection* pconn = getConnection();
	if(!pconn) {
		return LoopStateMachine::handleIteration();
	}
	
	IOPoolState* pstate = reinterpret_cast<IOPoolState*>(getState());
	if(pstate != NULL) {
		if(!pconn->isOpened()) {
			if(!pconn->Open()) {	
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IOPool::handleIteration Open() failed");
				usleep(FAIL_SLEEP_TIME * 1000);
				return true;
			} else {
				if(pstate != NULL) {
					pstate->handleOpen(pconn);
					return true;
				}
			}
		}
		IOConnection* pconn = getConnection();
		if(pconn && pconn->isDataAvailable(DATA_AVAILABLE_WAIT_TIME)) {
			if(pstate != NULL) {
				pstate->handleRead(pconn);
				return true;
			}
		}
	}
	return LoopStateMachine::handleIteration();
}

void 
IOPool::handleTerminate() {
	LoopStateMachine::handleTerminate();
	IOConnection* pconn = getConnection();
	if(pconn) {
		pconn->Close();
	}
}

};
