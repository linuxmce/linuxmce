/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file SerialIOPool.h
Classes for state machines.
*/
#ifndef DCESERIALIOPOOL_H
#define DCESERIALIOPOOL_H

#include "IOPool.h"
#include "SerialIOConnection.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class SerialIOPool
Class for ?
*/
class SerialIOPool : public IOPool {
public:
    SerialIOPool(bool usemain = false);
    virtual ~SerialIOPool();

public:
	virtual IOConnection* getConnection();

protected:
	/*callback from IOPool*/
	virtual bool handleIteration();

protected:

    /** @class SerialIOState
Class for ?
*/
	class SerialIOState : public IOPoolState {
		friend class SerialIOPool;
	protected:
		SerialIOState(SerialIOPool* psm)
			: IOPoolState(psm) {};
		virtual void handleOpen(IOConnection* pconn);
		virtual void handleRead(IOConnection* pconn);
		virtual void handleClose(IOConnection* pconn);
	} defstate_;

protected:
	SerialIOConnection serconn;
};

};

#endif
