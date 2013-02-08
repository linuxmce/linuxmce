/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file RubySerialWrapper.h
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
#ifndef DCERUBYSERIALWRAPPER_H
#define DCERUBYSERIALWRAPPER_H

#include "RubyDeviceWrapper.h"
#include "RubyCommandWrapper.h"
#include "RubyDCEConnector.h"
#include "RubySerialIOConnectionWrapper.h"

#include "PlutoUtils/StringUtils.h"

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceData.h"


#include <list>
#include <ruby.h>

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class RubySerialWrapper
For ?
*/
class RubySerialWrapper {
public:
    RubySerialWrapper();
    ~RubySerialWrapper();

public:
	void setDevice(const RubyDeviceWrapper& device) {
		device_ = device;
	}
	RubyDeviceWrapper& getDevice() {
		return device_;
	}

	void setDCEConnector(RubyDCEConnector* pdce) {
		pdce_ = pdce;
	}
	RubyDCEConnector* getDCEConnector() {
		return pdce_;
	}

	void setParent(VALUE parent) {
	    parent_ = parent;
	}

	VALUE getParent() {
	    return parent_;
	}

public:
	void SendCommand(RubyCommandWrapper* pcmd);
	void SendReceiveCommand(RubyCommandWrapper* pcmd);

	/*accessed from ruby code*/
public:
	VALUE parent_;
	std::list<VALUE> children_;
	RubyDeviceWrapper device_; /*our device*/

	void DisableDevice( int PK_Device, bool bDisable );

	void SetDeviceDataInDB( int PK_Device, int PK_DeviceData, const char* value );
	const char* GetCurrentDeviceData( int PK_Device, int PK_DeviceData );

private:
	RubyDCEConnector* pdce_;
};

};

#endif
