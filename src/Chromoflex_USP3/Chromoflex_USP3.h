/*
     Copyright (C) 2009 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Chromoflex_USP3_h
#define Chromoflex_USP3_h

//	DCE Implemenation for #1992 Chromoflex USP3

#include "Gen_Devices/Chromoflex_USP3Base.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Chromoflex_USP3 : public Chromoflex_USP3_Command
	{
//<-dceag-decl-e->
		// Private member variables
		int fd; // file desc for device
		unsigned short   usp_crc; // initialise per packet with $FFFF.
		int increment;
		int speed;

		// Private methods
		virtual void process_crc(unsigned char ucData);

public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Chromoflex_USP3(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Chromoflex_USP3();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Chromoflex_USP3(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
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
