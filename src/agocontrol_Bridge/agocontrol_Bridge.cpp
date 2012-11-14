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
#include "agocontrol_Bridge.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

using namespace qpid::messaging;
using namespace qpid::types;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
agocontrol_Bridge::agocontrol_Bridge(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: agocontrol_Bridge_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
agocontrol_Bridge::agocontrol_Bridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: agocontrol_Bridge_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
agocontrol_Bridge::~agocontrol_Bridge()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool agocontrol_Bridge::GetConfig()
{
	if( !agocontrol_Bridge_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	Variant::Map connectionOptions;
	connectionOptions["username"] = DATA_Get_Username();
	connectionOptions["password"] = DATA_Get_Password();
	try {
		agoConnection = Connection(DATA_Get_TCP_Address(), connectionOptions);
		agoConnection.open();
		agoSession = agoConnection.createSession();
		agoCommandSender = agoSession.createSender("ago.commands; {create: always, node: {type: topic}}");
		agoEventReceiver = agoSession.createReceiver("ago.events; {create: always, node: {type: topic}}");
	} catch(const std::exception& error) {
                std::cerr << error.what() << std::endl;
                agoConnection.close();
                printf("could not startup\n");
                return false;
	}
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool agocontrol_Bridge::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
agocontrol_Bridge_Command *Create_agocontrol_Bridge(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new agocontrol_Bridge(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void agocontrol_Bridge::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	qpid::messaging::Message command;
	Variant::Map content;

	string portChannel = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	content["uuid"] = portChannel;

	sCMD_Result = "OK";

	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"ON RECEIVED FOR CHILD %s", portChannel.c_str());
			content["command"] = "on";
			break;
		case COMMAND_Generic_Off_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"OFF RECEIVED FOR CHILD %s", portChannel.c_str());
			content["command"] = "off";
			break;
		case COMMAND_Set_Level_CONST:
			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"SET LEVEL RECEIVED FOR CHILD %s", portChannel.c_str());
			content["command"] = "setlevel";
			content["level"] = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()); 
			break;
	}
	encode(content, command);
	agoCommandSender.send(command);
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void agocontrol_Bridge::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


