/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     Author: Nosilla99
     Name  : CM15A
     Descr : Active Home CM15A USB X10 PC Interface and RF transceiver
             DCE Device Handler
     Date  : 10.11.2009
     File  : CM15A/cm15usb.h

*/

#ifndef CM15USB_H
#define CM15USB_H

#define EP_OUT                        0x02
#define EP_IN                         0x81
#define USB_TIMEOUT                   1000

#define X10_ADDRESS	              0x04
#define X10_FUNCTION                  0x06

#define X10_FUNC_ALL_U_OFF            0x00
#define X10_FUNC_ALL_L_ON             0x01
#define X10_FUNC_ON                   0x02
#define X10_FUNC_OFF                  0x03
#define X10_FUNC_DIM                  0x04
#define X10_FUNC_BRIGHT               0x05
#define X10_FUNC_ALL_L_OFF            0x06
#define X10_FUNC_EXT                  0x07
#define X10_FUNC_HREQ                 0x08
#define X10_FUNC_HACK                 0x09
#define X10_FUNC_PSD1                 0x0A
#define X10_FUNC_PSD2                 0x0B
#define X10_FUNC_EXTDT                0x0C
#define X10_FUNC_STATON               0x0D
#define X10_FUNC_STATOFF              0x0E
#define X10_FUNC_STATREQ              0x0F
#define X10_MAX_DIM_LEVEL               22
#define X10_FUNC_TIMEOUT             30000

#define MAX_QUEUE_SIZE  1000

#define CM15A_MOR                     0x00
#define CM15A_ACK                     0x55
#define CM15A_MT                      0x5b
#define CM15A_PLC                     0x5a
#define CM15A_RF                      0x5d
#define CM15A_STATUS                  0x8b
#define CM15A_SET_TIME                0x9b
#define CM15A_CR                      0xA5
#define CM15A_SLEEP                   1000
#define CM15A_ERROR_SLEEP             5100
#define CM15A_READ_SIZE               0x08

#include <list>
#include <map>
#include "thread.h"
#include "mutex.h"
#include "message.h"
#include "Gen_Devices/CM15ABase.h"
#include <usb.h>
namespace CM15ADEV {

class CM15usb : public Thread
{
public:
    /** Standard constructor */
    CM15usb();
    /** Standard destructor */
    ~CM15usb();

	inline void setCM15A(DCE::CM15A_Command *cm15a) {
		real_cm15a = cm15a;
	}

public:
	int SendRequest(const Message* pMesg);
        int Locate();
        const char *Byte2Binary(int x);
        int GetStatus();
	std::map <std::string, int> device_status;

protected:
	virtual void* _Run();

	int SendPacket(unsigned char highByte, unsigned char lowByte);
	int SendClock();

	virtual int SendAddress(const Message* pMesg);
	virtual int SendFunction(const Message* pMesg);
	virtual int ReadIncoming();

private:
	Mutex mq_;
	std::list<Message> msgqueue_;
	DCE::CM15A_Command *real_cm15a;
	std::map <std::string, int> inverse_device_map;
	DCE::VectDeviceData_Impl kids;
	DCE::Message *pMessageOut;
	list<std::string> last_used_addresses;
	struct timeval timestampNow;
	struct timeval timestampLast;
        struct usb_dev_handle *CM15A_handle;
        struct usb_bus *bus;
        struct usb_device *dev;
        usb_dev_handle *device_handle;
        int status;
};

};

#endif
