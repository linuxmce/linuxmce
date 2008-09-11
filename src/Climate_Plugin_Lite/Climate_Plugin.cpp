/*
 Climate_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

 
//<-dceag-d-b->
#include "Climate_Plugin.h"
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

#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_FloorplanObjectType_Color.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Climate_Plugin::Climate_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Climate_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-getconfig-b->
bool Climate_Plugin::GetConfig()
{
	if( !Climate_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Climate_Plugin::~Climate_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Climate_Plugin::Register()
//<-dceag-reg-e->
{
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Climate_Plugin::ClimateCommand ), 0, 0, 0, DEVICECATEGORY_Climate_Device_CONST, 0, 0 );

	return Connect(PK_DeviceTemplate_get()); 
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
void Climate_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Climate_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-createinst-b->!

bool Climate_Plugin::ClimateCommand( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	// This only runs as a plug-in so we can safely cast it
	class DeviceData_Router *pDevice_RouterTo = (class DeviceData_Router *) pDeviceTo;
	PreprocessClimateMessage(pDevice_RouterTo,pMessage);

	class DeviceData_Router *pDevice_RouterFrom = (class DeviceData_Router *) pDeviceFrom;
	PreprocessClimateMessage(pDevice_RouterFrom,pMessage);

	if( pMessage->m_sPK_Device_List_To.length() ) 
	{
		int PK_Device;  size_t pos=0;
		while( true )
		{
			PK_Device=atoi(StringUtils::Tokenize(pMessage->m_sPK_Device_List_To,",",pos).c_str());
			DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
			PreprocessClimateMessage(pDeviceData_Router,pMessage);
			if( pos>=pMessage->m_sPK_Device_List_To.length() || pos==string::npos )
				break;
		}
	}

	return false;
}

void Climate_Plugin::PreprocessClimateMessage(DeviceData_Router *pDevice,Message *pMessage)
{
	if( !pDevice || !pMessage || pDevice->m_dwPK_DeviceCategory != DEVICECATEGORY_Climate_Device_CONST)
		return;

	string sOn;
	string sMode;
	string sFan;
	string sSetPoint;
	string sTemp;
	GetStateVar(pDevice, sOn, sMode, sFan, sSetPoint, sTemp);
	
	// The State is in the format ON|OFF/SET TEMP (CURRENT TEMP)
	if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND )
	{
		if( pMessage->m_dwID==COMMAND_Set_Level_CONST )
		{
			string sLevel = pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST];
			if( sLevel.size()==0 )
				pMessage->m_dwID=COMMAND_Generic_Off_CONST;
			else
			{
				int iLevel = atoi(sLevel.c_str());
				if( sLevel[0]=='+' )
					iLevel = min(100, GetClimateLevel(pDevice,0)+iLevel);
				else if( sLevel[0]=='-' )
					iLevel = max(0, GetClimateLevel(pDevice,0)+iLevel);

				if( iLevel==0 )
					pMessage->m_dwID=COMMAND_Generic_Off_CONST;
				else
				{
					pDevice->m_sState_set("ON/" + StringUtils::itos(iLevel) + GetTemperature(pDevice));
					pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST] = StringUtils::itos(iLevel);
				}
			}
		}

		if( pMessage->m_dwID==COMMAND_Generic_On_CONST )
			SetStateValue(pDevice, "ON", sMode, sFan, sSetPoint, sTemp);
		else if( pMessage->m_dwID==COMMAND_Generic_Off_CONST )
			SetStateValue(pDevice, "OFF", sMode, sFan, sSetPoint, sTemp);
		else if( pMessage->m_dwID==COMMAND_Set_HeatCool_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Climate_Plugin: COMMAND_Set_HeatCool_CONST !");
			string sState = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
			if( sState=="H" )
				SetStateValue(pDevice, sOn, "HEAT", sFan, sSetPoint, sTemp);
			else if( sState=="C" )
				SetStateValue(pDevice, sOn, "COOL", sFan, sSetPoint, sTemp);
			else if( sState=="F" )
				SetStateValue(pDevice, sOn, "FAN_ONLY", sFan, sSetPoint, sTemp);
			else
				SetStateValue(pDevice, sOn, "AUTO", sFan, sSetPoint, sTemp);
		}
		else if( pMessage->m_dwID == COMMAND_Set_Fan_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Climate_Plugin: COMMAND_Set_Fan_CONST !");
			string sState = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
			if( 1 == atoi(sState.c_str()) )
			{
				SetStateValue(pDevice, sOn, "FAN_ONLY", "HIGH", sSetPoint, sTemp);
			}
			else
			{
				SetStateValue(pDevice, sOn, "AUTO", "AUTO", sSetPoint, sTemp);
			}
		}
	}
	else if( pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT )
	{
		if( pMessage->m_dwID == EVENT_Temperature_Changed_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Climate_Plugin: EVENT_Temperature_Changed_CONST !");
			// Replace the current temp
			string sLevel = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			SetStateValue(pDevice, sOn, sMode, sFan, sSetPoint, sLevel);
		}
		else if( pMessage->m_dwID == EVENT_Thermostat_Set_Point_Chan_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Climate_Plugin: EVENT_Thermostat_Set_Point_Chan_CONST !");
			// Replace the current temp
			string sLevel = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			SetStateValue(pDevice, sOn, sMode, sFan, sLevel, sTemp);
		}
		else if( pMessage->m_dwID == EVENT_Fan_Mode_Changed_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Climate_Plugin: EVENT_Thermostat_Set_Point_Chan_CONST !");
			// Replace the current temp
			string sLevel = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
			int iMode = atoi(sLevel.c_str());
			switch(iMode)
			{
				default:
				case 0:
				case 1:
					SetStateValue(pDevice, sOn, sMode, "AUTO", sSetPoint, sTemp);
					break;
					
				case 2:
				case 3:
					SetStateValue(pDevice, sOn, sMode, "HIGH", sSetPoint, sTemp);
					break;
			}
		}
		else if( pMessage->m_dwID == EVENT_Thermostat_Mode_Changed_CONST )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Climate_Plugin: EVENT_Thermostat_Set_Point_Chan_CONST !");
			// Replace the current temp
			string sLevel = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
				
			int iMode = atoi(sLevel.c_str());
			switch(iMode)
			{
				default:
				case 10:
					SetStateValue(pDevice, sOn, "AUTO", sFan, sSetPoint, sTemp);
					break;
			
				case 1:
					SetStateValue(pDevice, sOn, "HEAT", sFan, sSetPoint, sTemp);
					break;
			
				case 2:
					SetStateValue(pDevice, sOn, "COOL", sFan, sSetPoint, sTemp);
					break;
			
				case 6:
					SetStateValue(pDevice, sOn, "FAN_ONLY", sFan, sSetPoint, sTemp);
					break;
			}
		}
	}
}

int Climate_Plugin::GetClimateLevel(DeviceData_Router *pDevice,int iLevel_Default)
{
	string sState = pDevice->m_sState_get();
	string::size_type pos = sState.rfind("/");
	if( pos<sState.size()-1 && pos!=string::npos )
		return atoi(sState.substr(pos+1).c_str());
	else
		return iLevel_Default;
}

string Climate_Plugin::GetTemperature(DeviceData_Router *pDevice)
{
	// Replace the current temp
	string sCurrentState = pDevice->m_sState_get();
	string::size_type pos = sCurrentState.rfind('(');
	if( pos!=string::npos )
		return sCurrentState.substr(pos); // Get rid of the current temp
	else
		return "";
}

void Climate_Plugin::GetStateVar(DeviceData_Router *pDevice,
	string& sOn, string& sMode, string& sFan, string& sSetPoint, string& sTemp)
{
	string sCurrentState = pDevice->m_sState_get();
	
	// temperature
	string::size_type pos1 = sCurrentState.rfind('(');
	string::size_type pos2 = sCurrentState.rfind(')');
	if( pos1!=string::npos && pos2!=string::npos && pos1 < pos2 )
	{
		sTemp = sCurrentState.substr(pos1+1,pos2-pos1-1);
		sCurrentState = sCurrentState.substr(0, pos1);
	}
	else
	{
		sTemp = "25";
	}
	
	// on/off
	if( sCurrentState.substr(0,3) == "OFF" )
	{
		sOn = "OFF";
	}
	else
	{
		sOn = "ON";
	}
	
	// mode and fan
	sMode = "AUTO";
	sFan = "AUTO";
	pos1 = sCurrentState.find('/');
	pos2 = sCurrentState.rfind('/');
	if( pos1!=string::npos && pos2!=string::npos && pos1 < pos2 )
	{
		string::size_type pos3 = sCurrentState.find('/', pos1 + 1);
		if( pos3!=string::npos && pos1 < pos3 && pos3 < pos2 )
		{
			sMode = sCurrentState.substr(pos1+1, pos3-pos1-1);
			sFan = sCurrentState.substr(pos3+1, pos2-pos3-1);
		}
	}
	else if( pos1!=string::npos )
	{
		if( sCurrentState.c_str()[0] != 'O' )
		{
			sMode = sCurrentState.substr(0, pos1);
		}
	}
	
	// set point
	sSetPoint = "25";
	if( pos2!=string::npos && pos2+1<sCurrentState.size() )
	{
		int iSetPoint = atoi( sCurrentState.substr(pos2+1).c_str() );
		if( 0 != iSetPoint )
		{
			sSetPoint = StringUtils::itos(iSetPoint);
		}
	}
}

void Climate_Plugin::SetStateValue(DeviceData_Router *pDevice,
	   string sOn, string sMode, string sFan, string sSetPoint, string sTemp)
{
	pDevice->m_sState_set(sOn + "/" + sMode + "/" + sFan + "/" + sSetPoint + " (" + sTemp + ")");
}
