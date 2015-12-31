/*
     Copyright (C) 2010 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef PLCBUS_h
#define PLCBUS_h

//	DCE Implemenation for #1920 PLCBUS

#include "Gen_Devices/PLCBUSBase.h"
//<-dceag-d-e->

#include <deque>
#include <map>
#include <algorithm>


//<-dceag-decl-b->
namespace DCE
{
	class PLCBUS : public PLCBUS_Command
	{
//<-dceag-decl-e->
		// Private member variables
		int fd; // file desc for device
		static pthread_t readThread;
		pthread_mutex_t mutexSendQueue;
		struct PLCBUSJob {

			char buffer[1024];
			size_t len;
			time_t timeout;
			int sendcount;
			int usercode;
			int homeunit;
			int command;
			int data1;
			int data2;
		};

		std::deque < PLCBUSJob *>PLCBUSSendQueue;	


		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		PLCBUS(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~PLCBUS();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void receiveFunction();
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		PLCBUS(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	bool DATA_Get_3_Phase();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
