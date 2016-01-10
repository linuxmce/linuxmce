/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file StateMachine.h
Base classes for state machines.
*/
#ifndef DCESTATEMACHINE_H
#define DCESTATEMACHINE_H


/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class StateMachine
Base class for ?
*/
class StateMachine {
public:

    /** Constructor.
    */
    StateMachine();

    /** Destructor.
    */
    virtual ~StateMachine();

	/*state access*/
protected:
	class State;

    /** setState.
    @param pstate is a pointer to a state.
    */
	void setState(State* pstate);

    /** getState
    @returns a pointer to a state.
    */
	State* getState();

	/*states*/
protected:


    /** @class State
    Class for States.
    */
	class State {
		friend class StateMachine;
		friend class StateImpl;
	protected:
        /** Constructor.
        @param psm is a pointer to a StateMachine instance.
        */
		State(StateMachine* psm) : psm_(psm) {};

        /** getSM
        @returns a pointer to a StateMachine instance.
        */
		StateMachine* getSM() {
			return psm_;
		}

	protected:
        /** handleEnter
        Handle entry into a state.
        */
		virtual void handleEnter()
		{};

        /** handleExit
        Handle exit from a state.
        */
		virtual void handleExit()
		{};

        private:
		StateMachine* psm_; /**< Holds a State machine pointer. */
	};

public:
	State* state_; /**< Holds a state. */
};

};

#endif
