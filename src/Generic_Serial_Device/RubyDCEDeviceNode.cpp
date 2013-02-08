/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "RubyDCEDeviceNode.h"

#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "PlutoUtils/FileUtils.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"

#include "RubyEmbeder.h"
#include "RubySerialIOWrapper.h"

#include "TestRubyEmbederCodeSupplier.h"
#include "TestEmbededClass.h"

#include "RubyIOManager.h"

#include <algorithm>

using namespace EMBRUBY;
using namespace std;

#define DEFAULT_IDLE_DELAY	5000

namespace DCE {

RubyDCEDeviceNode::RubyDCEDeviceNode()
	: parent_(NULL), pembclass_(NULL), idledelay_(DEFAULT_IDLE_DELAY), m_bHandleChildrenInParent_(false)
{
	clock_gettime(CLOCK_REALTIME, &lastidle_);	
}


RubyDCEDeviceNode::~RubyDCEDeviceNode()
{
}

bool 
RubyDCEDeviceNode::Init(RubyDCECodeSupplier* pcs) {
	try {
		if(!pdevdata_) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Serial IO pool was not initialized with device Data.");
			return false;
		}
		
		pembclass_ = new RubyDCEEmbededClass(pcs, pdevdata_->m_dwPK_Device);
		VALUE value = pembclass_->getValue();
		
		void* data = DATA_PTR(value);
		RubySerialIOWrapper* pWrapper = reinterpret_cast<RubySerialIOWrapper*>(data);
		if(pWrapper == NULL) {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error instantiating Ruby serial WRAPPER.");
			return false;
		};
		
		pWrapper->setDCEConnector(RubyIOManager::getInstance());
		if(parent_ != NULL) {
		    pWrapper->setParent(parent_->getEmbClass()->getValue());
		}
		
		PopulateDevice(pdevdata_, pWrapper->getDevice());
	} catch(RubyException e) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed instantiating class: %s.", e.getMessage());
	}
	std::list<RubyDCEDeviceNode*>& children = getChildren();
	for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		(*it)->Init(pcs);
	}
	
	return true;
}

void 
RubyDCEDeviceNode::Cleanup() {
        std::list<RubyDCEDeviceNode*>& children = getChildren();
	for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
	    (*it)->Cleanup();
        }
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Serial IO Pool for device %d terminated.", pdevdata_->m_dwPK_Device);
	delete pembclass_;
}

RubyDCEDeviceNode* 
RubyDCEDeviceNode::findChild(unsigned devid) {
    std::list<RubyDCEDeviceNode*>& children = getChildren();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "findChild(%d) : children.size()=%d",devid,children.size());
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		if((*it)->getDeviceData()->m_dwPK_Device == devid) {
			return *it;
		}
    }
	return NULL;
}

bool 
RubyDCEDeviceNode::handleNoMessage() {
	if(!getEmbClass()) {
        return false;
    }
	
	struct timespec timespec;
	clock_gettime(CLOCK_REALTIME, &timespec);
	if( (timespec.tv_sec - lastidle_.tv_sec) * 1000 + ((timespec.tv_nsec - lastidle_.tv_nsec) / 1000000) >= (int)idledelay_) {
		lastidle_ = timespec;
//		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device_%d handleNoMessage sending IDLE command",getDeviceData()->m_dwPK_Device);
		Message idlemsg(0, getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_IDLE_CONST, 0);
		getEmbClass()->CallCmdHandler(&idlemsg);    
	}
	
	std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		(*it)->handleNoMessage();
	}
	return true;
}

bool
RubyDCEDeviceNode::handleMessage(Message* pmsg) {
    if(!getEmbClass()) {
        return false;
    }

    bool ret = false;
    if( getDeviceData()->m_dwPK_Device == (unsigned long)pmsg->m_dwPK_Device_To ||
    	m_bHandleChildrenInParent_ ) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "handleMessage directly");
        if( getEmbClass()->CallCmdHandler(pmsg) )
		{
			ret = true;
		}
    } else {
    	std::list<RubyDCEDeviceNode*>& children = getChildren();
	    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "handleMessage in child");
			ret |= (*it)->handleMessage(pmsg);
			 
	    }
//        if(!ret && findChild(pmsg->m_dwPK_Device_To)) {
        if(!ret && find(all_children_ids.begin(),all_children_ids.end(),pmsg->m_dwPK_Device_To)!=all_children_ids.end()) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "handleMessage for child");
            getEmbClass()->CallCmdForChildHandler(getDeviceData()->m_dwPK_Device, pmsg);
            ret = true;
        }
	}

	return ret;
}

bool
RubyDCEDeviceNode::handleStartup() {
    Message startmsg(0, getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Initialize_CONST, 0);
    if( !getEmbClass()->CallCmdHandler(&startmsg) )
		return false;
    
    std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
        (*it)->handleStartup();
    }
    return true;
}

void 
RubyDCEDeviceNode::handleTerminate() {
    Message stopmsg(0, getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Release_CONST, 0);
	getEmbClass()->CallCmdHandler(&stopmsg);

    std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
        (*it)->handleTerminate();
    }
}

void 
RubyDCEDeviceNode::PopulateDevice(DeviceData_Impl* pdevdata, RubyDeviceWrapper& devwrap) {
	devwrap.setDevId(pdevdata->m_dwPK_Device);
	devwrap.setDevTemplId(pdevdata->m_dwPK_DeviceTemplate);

	int numparams = pdevdata->m_mapParameters.size();	
	devwrap.setData(pdevdata->m_mapParameters);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added %d data params to device %d.", numparams, pdevdata->m_dwPK_Device);
	
	std::map<int, std::string>::iterator itPar = pdevdata->m_mapParameters.find(DEVICEDATA_Process_Child_Commands_In_Pare_CONST);
	if( pdevdata->m_mapParameters.end() != itPar && !(*itPar).second.empty() )
	{
		if( (*itPar).second == "1" )
			m_bHandleChildrenInParent_ = true;
	}
	
	std::map<int, RubyDeviceWrapper>& childdevices = devwrap.getChildDevices();
	std::map<int, std::string>& mapD_PC = devwrap.getMapDevice_PortChannel();
	std::map<std::string, int>& mapPC_D = devwrap.getMapPortChannel_Device();
    VectDeviceData_Impl& vDeviceData = pdevdata->m_vectDeviceData_Impl_Children;
	
    for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		RubyDeviceWrapper& childdevwrap = childdevices[vDeviceData[i]->m_dwPK_Device];
		all_children_ids.push_back(vDeviceData[i]->m_dwPK_Device);
		
		// Eugen, populate the device<->zone maps
		std::map<int, std::string>::iterator itParam = vDeviceData[i]->m_mapParameters.find(DEVICEDATA_PortChannel_Number_CONST);
		if( vDeviceData[i]->m_mapParameters.end() != itParam &&
			!(*itParam).second.empty() )
		{
			mapD_PC[vDeviceData[i]->m_dwPK_Device] = (*itParam).second;
			mapPC_D[(*itParam).second] = vDeviceData[i]->m_dwPK_Device;
		}
		
		PopulateDevice(vDeviceData[i], childdevwrap);
    }
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added %d wrapped child devices to device %d.", vDeviceData.size(), pdevdata->m_dwPK_Device);
}

};
