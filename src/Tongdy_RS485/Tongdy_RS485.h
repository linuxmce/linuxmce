/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Tongdy_RS485_h
#define Tongdy_RS485_h

#define ERR_OPEN                        1
#define ERR_TIMEOUT                     4
#define ERR_FLOCK                       22
#define ERR_STTY                        23

#define BAUDRATE   B19200


//	DCE Implemenation for #2008 Tongdy RS485

#include "Gen_Devices/Tongdy_RS485Base.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Tongdy_RS485 : public Tongdy_RS485_Command
	{
//<-dceag-decl-e->
		// Private member variables
		int port;

		// Private methods
public:
		void *receiveFunction();
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Tongdy_RS485(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Tongdy_RS485();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void decodeFrame(unsigned char *buf, int length);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Tongdy_RS485(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

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
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
