/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file NetworkIOPool.h
Base classe for IOEvent.
*/
#ifndef DCENETWORKIOPOOL_H
#define DCENETWORKIOPOOL_H

#include "IOPool.h"
#include "NetworkIOConnection.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class NetworkIOPool
Network ?
*/
class NetworkIOPool : public IOPool {
public:
    NetworkIOPool(bool usemain = false);
    virtual ~NetworkIOPool();

public:
	virtual IOConnection* getConnection();

protected:
	/** Callback from IOPool
    */
	virtual bool handleIteration();

protected:

    /** @class NetworkIOState
Network ?
*/
	class NetworkIOState : public IOPoolState {
		friend class NetworkIOPool;
	protected:
		NetworkIOState(NetworkIOPool* psm)
			: IOPoolState(psm) {};
		virtual void handleOpen(IOConnection* pconn);
		virtual void handleRead(IOConnection* pconn);
		virtual void handleClose(IOConnection* pconn);
	} defstate_;

protected:
	NetworkIOConnection netconn;
};

};

#endif
