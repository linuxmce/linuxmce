/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "LoopStateMachine.h"

#include <unistd.h>
#include <stdio.h>

#define DEFAULT_IDLE_DELAY	1000

namespace DCE {

LoopStateMachine::LoopStateMachine(bool usemain)
	: idledelay_(DEFAULT_IDLE_DELAY)
{
	lastidle_.tv_sec = lastidle_.tv_nsec = 0;
}


LoopStateMachine::~LoopStateMachine()
{
}

bool 
LoopStateMachine::handleStartup() {
	LoopState* pstate = reinterpret_cast<LoopState*>(getState());
	if(pstate != NULL) {
		pstate->handleStart();
	}
	return true;
}

bool 
LoopStateMachine::handleIteration() {
	LoopState* pstate = reinterpret_cast<LoopState*>(getState());
	if(pstate != NULL) {
		struct timespec timespec;
		clock_gettime(CLOCK_REALTIME, &timespec);
		
		if( (timespec.tv_sec - lastidle_.tv_sec) * 1000  
				+ ((timespec.tv_nsec - lastidle_.tv_nsec) / 1000000) >= (int)idledelay_) {
			lastidle_ = timespec;
			pstate->handleIdle();
		}
	}
	return true;
}

void 
LoopStateMachine::handleTerminate() {
	LoopState* pstate = reinterpret_cast<LoopState*>(getState());
	if(pstate != NULL) {
		pstate->handleStop();
	}
}

/*
void* 
LoopStateMachine::_Run() {
	while(!isStopRequested()) {
		LoopState* pstate = reinterpret_cast<LoopState*>(getState());
		if(!handleIteration()) {
			if(pstate != NULL) {
				pstate->handleIdle();
				usleep(IDLE_SLEEP_TIME * 1000);
			}
		}
	}	
	return 0;
}
*/
};
