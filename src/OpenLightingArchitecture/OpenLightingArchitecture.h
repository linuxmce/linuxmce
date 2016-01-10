/*
     Copyright (C) 2011 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef OpenLightingArchitecture_h
#define OpenLightingArchitecture_h

//	DCE Implemenation for #2184 OpenLightingArchitecture

#include "Gen_Devices/OpenLightingArchitectureBase.h"
//<-dceag-d-e->

// fix collision with definition from mysql ( /usr/include/ola/network/Socket.h:103: error: expected unqualified-id before '-' token )
#undef INVALID_SOCKET

#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/OlaClientWrapper.h>

//<-dceag-decl-b->
namespace DCE
{
	class OpenLightingArchitecture : public OpenLightingArchitecture_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		OpenLightingArchitecture(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~OpenLightingArchitecture();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		ola::DmxBuffer dmxBuffer;
		ola::client::OlaClientWrapper olaClientWrapper;
		ola::client::OlaClient *olaClient;

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		OpenLightingArchitecture(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
