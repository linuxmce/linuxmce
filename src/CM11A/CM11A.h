/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file CM11A.h
Handle the X10 CM11A interface module.
 */
//<-dceag-d-b->
#ifndef CM11A_h
#define CM11A_h

//	DCE Implemenation for #48 CM11A

#include "Gen_Devices/CM11ABase.h"
//<-dceag-d-e->

#include "devicepoll.h"
#include <map>

/**
@namespace DCE
The Data Commands and Events (DCE) namespace.
*/

/**
@class CM11A
Class to handle the X-10 CM11A controller.

This module is to communicate to the CM11A controller.
It uses Serial to communicate to the module via rs232.
Other modules call this with X10 commands such as turn on/turn off/dim/ etc.
It will formulate the actual messages.

The CM11A_Command class comes from Gen_Devices/CM11ABase.h.
*/

//<-dceag-decl-b->

namespace DCE
{

	class CM11A : public CM11A_Command
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

		CM11A(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~CM11A();
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
		CM11ADEV::DevicePoll devpoll;
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
