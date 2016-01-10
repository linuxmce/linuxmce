/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file devicepoll.h
 *@brief Handle device poll class.
 */
//
// C++ Interface: devicepoll
//
// Description:
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CM11DEVICEPOLL_H
#define CM11DEVICEPOLL_H

#include <list>
#include <map>
#include "thread.h"
#include "mutex.h"
#include "message.h"
#include "Serial/SerialPort.h"
#include "Gen_Devices/CM11ABase.h"

/**
@namespace CM11ADEV
The X10 CM11A Controller device namespace.
*/
namespace CM11ADEV {

/**
@class CM11ADEV::DevicePoll
This class is used to communicate with an X-10 controller ( CM11A ).
*/
class DevicePoll : public Thread
{
public:
    /** Standard constructor */
    DevicePoll();
    /** Standard destructor */
    ~DevicePoll();

    /** setSerialPort
    @param serport is a string naming the port
    */
	inline void setSerialPort(const char* serport) {
		serport_ = serport;
	}

    /** getSerialPort
    returns a string that is the ports name.
    */
	inline const char* getSerialPort() {
		return serport_.c_str();
	}
	inline void setCM11A(DCE::CM11A_Command *cm11a) {
		real_cm11a = cm11a;
	}

public:
	int SendRequest(const Message* pMesg);
	std::map <std::string, int> device_status;

protected:
	virtual void* _Run();

	int SendPacket(CSerialPort* pport,
			unsigned char highByte, unsigned char lowByte);
	int SendClock(CSerialPort* pport);

	virtual int SendAddress(CSerialPort* pport, const Message* pMesg);
	virtual int SendFunction(CSerialPort* pport, const Message* pMesg);
	virtual int ReadIncoming(CSerialPort* pport);

private:
	Mutex mq_;
	std::list<Message> msgqueue_;
	std::string serport_;
	DCE::CM11A_Command *real_cm11a;
	std::map <std::string, int> inverse_device_map;
	DCE::VectDeviceData_Impl kids;
	DCE::Message *pMessageOut;
	list<std::string> last_used_addresses;
	struct timeval timestampNow;
	struct timeval timestampLast;
};

};

#endif
