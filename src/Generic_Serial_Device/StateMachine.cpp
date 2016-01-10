/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "StateMachine.h"

namespace DCE {

StateMachine::StateMachine()
	: state_(0)
{}

StateMachine::~StateMachine() {
}

void 
StateMachine::setState(StateMachine::State* pstate) {
	if(state_ != 0) {
		state_->handleExit();
	}
	pstate->handleEnter();
	state_ = pstate;
}

StateMachine::State* 
StateMachine::getState() {
	return state_;
}


};
