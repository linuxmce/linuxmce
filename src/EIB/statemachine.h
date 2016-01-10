/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file statemachine.h
Header for StateMachine class.
*/
//
// C++ Interface: statemachine
//
// Description:
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSSTATEMACHINE_H
#define EIBBUSSTATEMACHINE_H

#include <list>

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

#define MIN_DEPTH		1
#define DEF_DEPTH		10

/** @class StateMachine
Some sort of simple state machine.

@todo find out how this is used and document it.
*/
class StateMachine {
protected:
	class State;

public:
	StateMachine(int depth = DEF_DEPTH);
	virtual ~StateMachine();

public:
	void setState(State *pstate);
	State *getState();
	void rollState();

	void Handle(void* p = NULL); /*has an optional param*/

protected:
	virtual void handleNewState() {};

private:
	void regState(State *pstate);
	void unregState(State *pstate);

private:
	unsigned int depth_;
	std::list<State*> statequeue_;

protected:
	/*State class*/
	class State {
		friend class StateMachine;
	public:
		State(StateMachine* psm);
		virtual ~State();

	public:
		StateMachine* getStateMachine();

		void setState(State* pstate);
		State* getState();

	protected:
		virtual void Handle(void* p)
		{};

	private:
		StateMachine* psm_;
	};
	/**/

private:
	State unknstate_;
};

};

#endif
