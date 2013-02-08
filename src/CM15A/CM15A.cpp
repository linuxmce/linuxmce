/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

     Author: Nosilla99
     Name  : CM15A 
     Descr : Active Home CM15A USB X10 PC Interface and RF transceiver
             DCE Device Handler
     Date  : 10.11.2009
     File  : CM15A/CM15A.cpp

*/
//<-dceag-d-b->
#include "CM15A.h"
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
CM15A::CM15A(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: CM15A_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
CM15A::~CM15A()
//<-dceag-dest-e->
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Destroying CM15A");	
}

//<-dceag-getconfig-b->
bool CM15A::GetConfig()
{
	if( !CM15A_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool CM15A::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool CM15A::Connect(int iPK_DeviceTemplate) {
        if(!CM15A_Command::Connect(iPK_DeviceTemplate)) {
                return false;
        }
                
        cm15proc.setCM15A(this);
        if (cm15proc.Locate()){ 
            cm15proc.GetStatus();
            cm15proc.Run(false);
        } else
           return false;
        return true;
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void CM15A::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
        string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Child device %d has channel %s.", pMessage->m_dwPK_Device_To, sChannel.c_str());

        switch(pMessage->m_dwID) {
                case COMMAND_Generic_On_CONST: {
                           LoggerWrapper::GetInstance()->Write(LV_STATUS, 
                                         "Will Turn ON %s.", sChannel.c_str());
                           CM15ADEV::Message msg;
                           msg.setAddress(sChannel);
                           if(cm15proc.device_status[sChannel]==0)
                           {
                                msg.setFunctionCode(X10_FUNC_ON);
                           }
                           else
                           {
                                msg.setFunctionCode(X10_FUNC_BRIGHT);
                                msg.setDimmLevel(100);
                           }
                           cm15proc.SendRequest(&msg);
                           cm15proc.device_status[sChannel]=100; 
                        }
                        break;

                case COMMAND_Generic_Off_CONST: {
                           LoggerWrapper::GetInstance()->Write(LV_STATUS,   
                                         "Will Turn OFF %s.", sChannel.c_str());
                           CM15ADEV::Message msg;
                           msg.setAddress(sChannel);
                           msg.setFunctionCode(X10_FUNC_OFF);
                           cm15proc.SendRequest(&msg);
                           cm15proc.device_status[sChannel]=0; 
                        }
                        break;

                case COMMAND_Set_Level_CONST: {
                           LoggerWrapper::GetInstance()->Write(LV_STATUS,   
                                         "Will Adjust %s.", sChannel.c_str());
                           CM15ADEV::Message msg;
                           msg.setAddress(sChannel);
                           int oldLevel=cm15proc.device_status[sChannel];
                           int newLevel = oldLevel;
                           if(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()[0]=='+') {
                                newLevel+=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()+1);
                                if(newLevel>100)
                                    newLevel=100;
                                }
                           else {
                                if(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()[0]=='-') {
                                    newLevel-=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str()+1);
                                    if(newLevel<=0)
                                        newLevel=0;
                                }
                                else {
                                    newLevel = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST].c_str());
                                }
                           }

                           if(oldLevel==0) {
                                //turn on first
                                CM15ADEV::Message msg1;
                                msg1.setAddress(sChannel);
                                msg1.setFunctionCode(X10_FUNC_ON);
                                cm15proc.SendRequest(&msg1);
                                oldLevel=100;
                                //then dim to right level
                           }

                           if(newLevel<oldLevel) {
                                //dim
                                msg.setFunctionCode(X10_FUNC_DIM);
                                msg.setDimmLevel(oldLevel-newLevel);
                           } else {
                                //bright
                                msg.setFunctionCode(X10_FUNC_BRIGHT);
                                msg.setDimmLevel(newLevel-oldLevel);
                           }
                           cm15proc.SendRequest(&msg);
                           cm15proc.device_status[sChannel] = newLevel;//store latest level
                        }
                        break;
                default:
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, 
                              "Unknown command %d received.", pMessage->m_dwID);
        }

	sCMD_Result = "OK";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void CM15A::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received Command for Uknown Child.");
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
/*

	COMMANDS TO IMPLEMENT

*/

