/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file IOPool.h
Classes for pool state machines.
*/
#ifndef DCEIOPOOL_H
#define DCEIOPOOL_H

#include "LoopStateMachine.h"
#include "IOConnection.h"

#include <list>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class IOPool
Class for ???
*/
class IOPool : public LoopStateMachine {
public:
    IOPool(bool usemain = false);
    ~IOPool();

public:
	virtual IOConnection* getConnection() = 0;

	/*pool handling*/
public:
	virtual bool handleStartup();
	virtual bool handleIteration();
	virtual void handleTerminate();

	/*states*/
protected:

/** @class IOPoolState
Class for ???
*/
	class IOPoolState : public LoopState {
		friend class IOPool;
	protected:
		IOPoolState(IOPool* psm)
			: LoopState(psm) {};
		virtual void handleOpen(IOConnection* pconn)
		{};
		virtual void handleRead(IOConnection* pconn)
		{};
		virtual void handleClose(IOConnection* pconn)
		{};
	};

private:
};

};

#endif
