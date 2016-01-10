/*
     Copyright (C) 2011 Harald Klein <hari@vt100.at>

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "OpenLightingArchitecture.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
OpenLightingArchitecture::OpenLightingArchitecture(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: OpenLightingArchitecture_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
OpenLightingArchitecture::OpenLightingArchitecture(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: OpenLightingArchitecture_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
OpenLightingArchitecture::~OpenLightingArchitecture()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool OpenLightingArchitecture::GetConfig()
{
	if( !OpenLightingArchitecture_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated

	ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);
	if (!olaClientWrapper.Setup()) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot initialize OLA client wrapper, aborting");
		return false;
	}
	olaClient = olaClientWrapper.GetClient();
	dmxBuffer.Blackout();

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool OpenLightingArchitecture::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
OpenLightingArchitecture_Command *Create_OpenLightingArchitecture(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new OpenLightingArchitecture(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void OpenLightingArchitecture::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	string portChannel = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	int universe = 0;
	int channel = 0;
	int value = 0; 
	int valueR = 0,valueG = 0,valueB = 0;
	int channelR = 0,channelG=0,channelB=0;
	bool isrgb = 0;

	if (portChannel.find("|") != string::npos) {
		vector<string> vect;
		StringUtils::Tokenize(portChannel, "|", vect);
		universe = atoi(vect[0].c_str());
		channel = atoi(vect[1].c_str())-1;
		if (vect.size()==4){
			channelR=atoi(vect[1].c_str())-1;
			channelG=atoi(vect[2].c_str())-1;
			channelB=atoi(vect[3].c_str())-1;
			isrgb = 1;
		}
	} else {
		universe =1;
		channel = atoi(portChannel.c_str())-1;
	}

	ola::client::SendDMXArgs dmxArgs;

	sCMD_Result = "UNHANDLED CHILD";
	switch (pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST:
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ON RECEIVED FOR universe/channel %d/%d",universe,channel);
			if (isrgb) {
				dmxBuffer.SetChannel(channelR, 255);
				dmxBuffer.SetChannel(channelG, 255);
				dmxBuffer.SetChannel(channelB, 255);
			} else {
				dmxBuffer.SetChannel(channel, 255);
			}
			olaClient->SendDMX(universe,dmxBuffer,dmxArgs);
			sCMD_Result = "OK";
			break;
			;;
		case COMMAND_Generic_Off_CONST:
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OFF RECEIVED FOR universe/channel %d/%d",universe,channel);
			if (isrgb) {
				dmxBuffer.SetChannel(channelR, 255);
				dmxBuffer.SetChannel(channelG, 255);
				dmxBuffer.SetChannel(channelB, 255);
			} else {
				dmxBuffer.SetChannel(channel, 0);
			}
			olaClient->SendDMX(universe,dmxBuffer,dmxArgs);
			sCMD_Result = "OK";
			break;
			;;
		case COMMAND_Set_Level_CONST:
			value = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LEVEL RECEIVED FOR universe/channel %d/%d, %d",universe,channel,value);
			if (value <0) { value = 0;};
			if (value >255) { value = 255;};

			if (isrgb) {
				dmxBuffer.SetChannel(channelR, value);
				dmxBuffer.SetChannel(channelG, value);
				dmxBuffer.SetChannel(channelB, value);
			} else {
				dmxBuffer.SetChannel(channel, value);
			}
			olaClient->SendDMX(universe,dmxBuffer,dmxArgs);
			sCMD_Result = "OK";
			break;
			;;
		case COMMAND_Set_Color_RGB_CONST:
			valueR = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Red_Level_CONST].c_str());
			valueG = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Green_Level_CONST].c_str());
			valueB = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Blue_Level_CONST].c_str());
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"RGB LEVEL RECEIVED FOR universe/channel R G B, value R G B %d/%d %d %d, %d %d %d",universe,channelR,channelG,channelB,valueR,valueG,valueB);
			if (valueR >255) { valueR = 255;};
			if (valueR <0) { valueR = 0;};
			if (valueG >255) { valueG = 255;};
			if (valueG <0) { valueG = 0;};
			if (valueB >255) { valueB = 255;};
			if (valueB <0) { valueB = 0;};
			dmxBuffer.SetChannel(channelR, valueR);
			dmxBuffer.SetChannel(channelG, valueG);
			dmxBuffer.SetChannel(channelB, valueB);
			olaClient->SendDMX(universe,dmxBuffer,dmxArgs);
			sCMD_Result = "OK";
			break;
			;;
	}
	
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void OpenLightingArchitecture::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


