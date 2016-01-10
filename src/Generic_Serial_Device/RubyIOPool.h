/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyIOPool.h
Base classe for IOEvent.
*/
#ifndef DCERUBYIOPOOLMANAGER_H
#define DCERUBYIOPOOLMANAGER_H

#include "IOPool.h"
#include "RubyDeviceWrapper.h"
#include "RubyDCECodeSupplier.h"
#include "RubyDCEEmbededClass.h"
#include "RubyDCEConnector.h"
#include "RubyDCEDeviceNode.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

class DeviceData_Impl;

/** @class RubyIOPool
For ?
*/
class RubyIOPool : protected IOPool, public RubyDCEDeviceNode {
	friend class RubyIOManager;

public:
	RubyIOPool(IOPool* ppool, bool relpool = true);
    virtual ~RubyIOPool();

public:
	bool Init(RubyDCECodeSupplier* pcs);
	void Cleanup();

protected:
	/*delegated methods*/
	IOConnection* getConnection() {
		return ppool_->getConnection();
	}
	bool handleStartup();
	bool handleNoMessage();
	void handleTerminate();

protected:
	class RubyIOState : public IOPoolState {
		friend class RubyIOPool;
	protected:
		RubyIOState(RubyIOPool* psm)
			: IOPoolState(psm) {};
		virtual void handleRead(IOConnection* pconn);
	} defstate_;

private:
	IOPool* ppool_;
	bool relpool_;
};

};
#endif
