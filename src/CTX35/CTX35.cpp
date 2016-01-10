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
#include "CTX35.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "ctx35consts.h"
#include "PlutoUtils/LinuxSerialUSB.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
CTX35::CTX35(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: CTX35_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}
//<-dceag-getconfig-b->
bool CTX35::GetConfig()
{
	if( !CTX35_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
CTX35::~CTX35()
//<-dceag-dest-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Destroying CTX35");
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool CTX35::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool CTX35::Connect(int iPK_DeviceTemplate) {
	if(!CTX35_Command::Connect(iPK_DeviceTemplate)) {
		return false;
	}
	
	string sPort = TranslateSerialUSB(DATA_Get_COM_Port_on_PC());
	
	if(sPort.length() > 0) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using serial port: %s.", sPort.c_str());
		devpoll.setSerialPort(sPort.c_str());
	}
	devpoll.setCTX35(this);
	devpoll.Run(false);
	return true;
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void CTX35::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Child device %d has channel %s.", pMessage->m_dwPK_Device_To, sChannel.c_str());
	
	switch(pMessage->m_dwID) {
		case COMMAND_Generic_On_CONST: {
				CTX35DEV::Message msg;
				msg.setAddress(sChannel);
				if(devpoll.device_status[sChannel]==0)
				{
					msg.setFunctionCode(CTX35_FUNC_ON);
				}
				else
				{
					msg.setFunctionCode(CTX35_FUNC_BRIGHT);
					msg.setDimmLevel(100);
				}
				devpoll.SendRequest(&msg);
				devpoll.device_status[sChannel]=100;//store latest levet
			}
			break;
		case COMMAND_Generic_Off_CONST: {
				CTX35DEV::Message msg;
				msg.setAddress(sChannel);
				msg.setFunctionCode(CTX35_FUNC_OFF);
				devpoll.SendRequest(&msg);
				devpoll.device_status[sChannel]=0;//store latest levet
			}
			break;
		case COMMAND_Set_Level_CONST: {
				CTX35DEV::Message msg;
				msg.setAddress(sChannel);
				int oldLevel=devpoll.device_status[sChannel];
				int newLevel = oldLevel;
				if(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()[0]=='+')
				{
					newLevel+=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()+1);
					if(newLevel>100)
						newLevel=100;
				}
				else
				{
					if(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()[0]=='-')
					{
						newLevel-=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()+1);
						if(newLevel<=0)
							newLevel=0;
					}
					else
					{
						newLevel = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
					}
				}

				if(oldLevel==0)
				{
					//turn on first
					CTX35DEV::Message msg1;
					msg1.setAddress(sChannel);
					msg1.setFunctionCode(CTX35_FUNC_ON);
					devpoll.SendRequest(&msg1);
					oldLevel=100;
					//then dim to right level
				}
				if(newLevel<oldLevel)
				{
					//dim
					msg.setFunctionCode(CTX35_FUNC_DIM);
					msg.setDimmLevel(oldLevel-newLevel);
				}
				else
				{
					//bright
					msg.setFunctionCode(CTX35_FUNC_BRIGHT);
					msg.setDimmLevel(newLevel-oldLevel);
				}
				devpoll.SendRequest(&msg);
				devpoll.device_status[sChannel] = newLevel;//store latest levet
			}
			break;
		default:
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unknown command %d received.", pMessage->m_dwID);
	}
		
//	string sSrcPhone = pDeviceData_Base->mapParameters_Find(1);
	
	sCMD_Result = "OK";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void CTX35::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
/*

	COMMANDS TO IMPLEMENT

*/
//<-dceag-createinst-b->!
