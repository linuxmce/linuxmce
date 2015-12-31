/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file RubyDCEDeviceNode.h
For ?
*/
//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCERUBYDCEDEVICENODE_H
#define DCERUBYDCEDEVICENODE_H

#include <list>

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

/** @class RubyDCEDeviceNode
For ???
*/
class RubyDCEDeviceNode {
	friend class RubyIOManager;
public:
    RubyDCEDeviceNode();
    virtual ~RubyDCEDeviceNode();

public:
	virtual bool Init(RubyDCECodeSupplier* pcs);
	virtual void Cleanup();

	void setDeviceData(DeviceData_Impl* pdevdata) {
		pdevdata_ = pdevdata;
	}
	DeviceData_Impl* getDeviceData() {
		return pdevdata_;
	}

	RubyDCEDeviceNode* getParent() {
		return parent_;
	}
	void setParent(RubyDCEDeviceNode* parent) {
		parent_ = parent;
	}

	int addChild(RubyDCEDeviceNode* pnode) {
		pnode->setParent(this);
		children_.push_back(pnode);
		return children_.size();
	}
	std::list<RubyDCEDeviceNode*>& getChildren() {
		return children_;
	}
	RubyDCEDeviceNode* findChild(unsigned devid);

public:
	void setIdleDelay(unsigned idledelay) {
		idledelay_ = idledelay;
	}
	unsigned getIdleDelay() {
		return idledelay_;
	}

protected:
	virtual bool handleNoMessage();
	virtual bool handleMessage(Message* pmsg);
	virtual bool handleStartup();
	virtual void handleTerminate();

protected:
	RubyDCEEmbededClass* getEmbClass() {
		return pembclass_;
	}

private:
	void PopulateDevice(DeviceData_Impl* pdevdata, RubyDeviceWrapper& devwrap);

private:
	RubyDCEDeviceNode* parent_;
	std::list<RubyDCEDeviceNode*> children_;
	std::list<int> all_children_ids;
private:
	unsigned idledelay_;
	struct timespec lastidle_;

private:
	DeviceData_Impl* pdevdata_;
	RubyDCEEmbededClass* pembclass_; // Ruby class instance
	bool m_bHandleChildrenInParent_;
};

};

#endif
