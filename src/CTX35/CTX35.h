/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file CTX35.h
Handle the X10 CTX35 interface module.
 */
//<-dceag-d-b->
#ifndef CTX35_h
#define CTX35_h

//	DCE Implemenation for #48 CTX35

#include "Gen_Devices/CTX35Base.h"
//<-dceag-d-e->

#include "devicepoll.h"
#include <map>

/**
@namespace DCE
The Data Commands and Events (DCE) namespace.
*/

/**
@class CTX35
Class to handle the X-10 CTX35 controller.

This module is to communicate to the CTX35 controller.
It uses Serial to communicate to the module via rs232.
Other modules call this with X10 commands such as turn on/turn off/dim/ etc.
It will formulate the actual messages.

The CTX35_Command class comes from Gen_Devices/CTX35Base.h.
*/

//<-dceag-decl-b->

namespace DCE
{

	class CTX35 : public CTX35_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

public:
		/** Constructor
@param DeviceID is an int.
@param ServerAddress is the IP address of the server??
@param bConnectEventHandler is ???
@param bLocalMode is ???
@param pRouter is ???
        */

        /** Standard destructor */

        /** GetConfig
        @returns success flag
        */

        /** Register is used to check if a device is registered
        @returns registered flag
        */

        /** ReceivedCommandForChild is
        @param pDeviceData_Impl is a pointer to ???
        @param sCMD_Result is a string ???
        @param pMessage is a pointer to ???
        */

        /** ReceivedUnknownCommand is called when an error happens.
        @param sCMD_Result is ???
        @param pMessage is the message that was incorrect
        */

//<-dceag-const-b->
public:
		// Constructors/Destructor

		CTX35(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~CTX35();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

        /** Connect will connect to the X10 controller
        @param iPK_DeviceTemplate is the templates ???
        */
		virtual bool Connect(int iPK_DeviceTemplate);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->

private:
    /** devpoll is a private ID */
		CTX35DEV::DevicePoll devpoll;
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
