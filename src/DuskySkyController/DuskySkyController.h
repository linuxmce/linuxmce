/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef DuskySkyController_h
#define DuskySkyController_h

#define USB_TIMEOUT 1000 /* ms */

//	DCE Implemenation for #2199 DuskySkyController

#include "Gen_Devices/DuskySkyControllerBase.h"
#include "sky-control-lib.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class DuskySkyController : public DuskySkyController_Command
	{
//<-dceag-decl-e->
		// Private member variables
		sky_usb *m_sky;

		enum eSKY_boxtype {sky = 0x0000, skyplus = 0x0c00, skyplushd = 0x5c00};
		eSKY_boxtype m_boxtype;	

		// Private methods
public:
		struct DuskyChild
		{
			sky_usb *m_sky;
			eSKY_boxtype m_boxtype;
			short port;
		};
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		DuskySkyController(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~DuskySkyController();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		void SendSkyCommand(int op, int cmd);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		DuskySkyController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	int DATA_Get_Number_of_ports();
	string DATA_Get_Connector();
	string DATA_Get_COM_Port_ParityBitStop();
	string DATA_Get_COM_Port_BaudRate();
	bool DATA_Get_Autoassign_to_parents_room();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
