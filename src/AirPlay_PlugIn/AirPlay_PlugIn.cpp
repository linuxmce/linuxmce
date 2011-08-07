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
#include "AirPlay_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
AirPlay_PlugIn::AirPlay_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: AirPlay_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
  	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::Starting up...");
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
AirPlay_PlugIn::AirPlay_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: AirPlay_PlugIn_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
AirPlay_PlugIn::~AirPlay_PlugIn()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool AirPlay_PlugIn::GetConfig()
{
	if( !AirPlay_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool AirPlay_PlugIn::Register()
//<-dceag-reg-e->
{
  	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::Register()");
	//m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
  	//m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
  	/*if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
    	{
      		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to AirPlay_PlugIn");
      		return false;
    	}*/

  	//vector<int> vectPK_DeviceTemplate;
  	//vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Hulu_Player_CONST);
  	//m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

  	//LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Registered device %d",DEVICETEMPLATE_Hulu_Player_CONST);

  	//RegisterMsgInterceptor(( MessageInterceptorFn )( &AirPlay_PlugIn::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );


	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
AirPlay_PlugIn_Command *Create_AirPlay_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new AirPlay_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void AirPlay_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void AirPlay_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


