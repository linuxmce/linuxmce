/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyIOManager.h
Manages instances of serial pools.

Manager was added because of limitation of ruby to run only in process
 context and not in thread context.
Because of this we will not use each Pool's state machine,
 intead will use this class to manage comunication between ruby and DCE.
*/
#ifndef DCERUBYSERIALIOMANAGER_H
#define DCERUBYSERIALIOMANAGER_H

#include <list>

#include "IOThread.h"
#include "IOMutex.h"
#include "IOEvent.h"
#include "RubyDCEConnector.h"
#include "RubyDCECodeSupplier.h"
#include "RubyIOPool.h"
#include "PlutoUtils/MultiThreadIncludes.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

class DeviceData_Base;
class DeviceData_Impl;
class Event_Impl;

/** @class RubyIOManager
Mmanages instances of serial pools.
*/
class RubyIOManager : protected RubyDCEConnector, public IOThread {

    /** Constructor.
    */
    RubyIOManager();

    /** Destructor.
    */
    virtual ~RubyIOManager();

	friend class RubyDCEDeviceNode;

public:
	static RubyIOManager* getInstance();

public:

	void setEventDispatcher(Event_Impl* pevdisp) {
		pevdisp_ = pevdisp;
	}

    Event_Impl* getEventDispatcher() {
		return pevdisp_;
	}

	int addDevice(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata);

    /** NOT Implemented.
    */
	int removeDevice(DeviceData_Impl* pdevdata);

    bool hasDevice(DeviceData_Base* pdevdata);

    RubyDCECodeSupplier getCodeSupplier() {
		return cs_;
	}

public:
	int RouteMessage(DeviceData_Base* pdevdata, Message *pMessage);
	virtual void SendMessage(Message* pmsg);
	virtual void SendString(string str);

    /** This is sort of obsolete, since using it (and moving it's definition back to
the base RubyDCEConnector) whould turn the framework for ruby into a mess.
    */
    virtual void SetDeviceData( int PK_Device,int PK_DeviceData,string Value );

    pluto_pthread_mutex_t m_MsgMutex;

protected:
	/** Methods for comunicating with DCE, accessed by wrappers.
    */
	virtual void SendCommand(RubyCommandWrapper* pcmd);
	virtual void SendReceiveCommand(RubyCommandWrapper* pcmd);

    /** Methods for comunicating with DCE, accessed by wrappers.
    */
	virtual string SendCommandReceiveString(RubyCommandWrapper* pcmd);

protected:
	virtual bool handleStartup();
	virtual void handleTerminate();
	virtual void* _Run();

	enum PORTTYPE {
		PORTTYPE_UNKNOWN = -1,
		PORTTYPE_SERIAL,
		PORTTYPE_NETWORK
	};

private:
	RubyDCEDeviceNode* InstantiateNode(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata);
	RubyDCEDeviceNode* addDevice(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata, RubyDCEDeviceNode* pNode);
	bool hasDevice(DeviceData_Base* pdevdata, RubyDCEDeviceNode* pNode);
	bool DispatchMessage(Message *pmsg, unsigned deviceid, RubyDCEDeviceNode* pNode);

private:
	static RubyIOManager* s_instance_;

private:
	RubyDCECodeSupplier cs_;
	Event_Impl* pevdisp_;
	Command_Impl* pcmdimpl_;
	typedef std::list< std::pair<unsigned, Message*> > MESSAGEQUEUE;
	MESSAGEQUEUE msgqueue_;
	IOMutex mmsg_;
	IOEvent emsg_;

	/*[serial port <--> pool] map*/
	/*
	typedef std::map<std::string, RubyIOPool*> POOLMAP;
	POOLMAP pools_;
	*/
	RubyDCEDeviceNode* rootnode_;
};

};

#endif
