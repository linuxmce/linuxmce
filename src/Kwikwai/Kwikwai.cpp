/*
     Copyright (C) 2011 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Kwikwai.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "PlutoUtils/LinuxSerialUSB.h"

//<-dceag-const-b->!
// The primary constructor when the class is created as a stand-alone device
Kwikwai::Kwikwai(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Kwikwai_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
	, IRReceiverBase(this)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!
// The constructor when the class is created as an embedded instance within another stand-alone device
Kwikwai::Kwikwai(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Kwikwai_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

// Must override so we can call IRBase::Start() after creating children
void Kwikwai::CreateChildren()
{
	Kwikwai_Command::CreateChildren();
	Start();
}

void Kwikwai::SendIR(string Port, string IRCode,int iRepeat)
{


}
//<-dceag-dest-b->
Kwikwai::~Kwikwai()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Kwikwai::GetConfig()
{
	if( !Kwikwai_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	string port = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	if (port.length()==0) {
		return false;
	}

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Kwikwai::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Kwikwai_Command *Create_Kwikwai(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Kwikwai(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Kwikwai::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	if ( IRBase::ProcessMessage(pMessage) ) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Message %d processed by IRBase class",pMessage->m_dwID);
		sCMD_Result = "OK";
	} else {
		sCMD_Result = "UNHANDLED CHILD";
	}
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Kwikwai::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


