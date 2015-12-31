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
#ifndef agocontrol_Bridge_h
#define agocontrol_Bridge_h

//	DCE Implemenation for #2211 agocontrol Bridge

#include "Gen_Devices/agocontrol_BridgeBase.h"
//<-dceag-d-e->

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Address.h>

#include <uuid/uuid.h>

//<-dceag-decl-b->
namespace DCE
{
	class agocontrol_Bridge : public agocontrol_Bridge_Command
	{
//<-dceag-decl-e->
		// Private member variables
		qpid::messaging::Connection agoConnection;
		qpid::messaging::Session agoSession;
		qpid::messaging::Receiver agoReceiver;
		qpid::messaging::Sender agoSender;

		static pthread_t receiveThread;
		void *mySelf;
	
		// Private methods
		DeviceData_Impl *InternalIDToDevice(string sInternalID);
		vector<DeviceData_Impl*> FindDevicesForNode(string sInternalID);
		bool DeleteDevicesForNode(string sInternalID);
		int AddDevice(int parent, string sInternalID,int PK_DeviceTemplate, string sName, string sRoom);




public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		agocontrol_Bridge(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~agocontrol_Bridge();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		void receiveFunction();

		void SendTemperatureChangedEvent(unsigned int PK_Device, float value);
		void SendSensorTrippedEvent(unsigned int PK_Device, bool value);
		void SendLightChangedEvent(unsigned int PK_Device, int value);
		void SendPowerUsageChangedEvent(unsigned int PK_Device, int value);
		void SendVoltageChangedEvent(unsigned int PK_Device, int value);
		void SendOnOffEvent(unsigned int PK_Device, int value);
		void SendBrightnessChangedEvent(unsigned int PK_Device, int value);
		void SendFireAlarmEvent(unsigned int PK_Device);
		void SendHumidityChangedEvent(unsigned int PK_Device, float value);



//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		agocontrol_Bridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_TCP_Address();
	string DATA_Get_Username();
	string DATA_Get_Password();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
