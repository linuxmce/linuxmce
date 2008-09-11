/*
 Lighting_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

//<-dceag-d-b->
#include "Lighting_Plugin.h"
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
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_FloorplanObjectType_Color.h"

// Alarm types
#define	RESTORE_LIGHTING_STATE		1

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Lighting_Plugin::Lighting_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Lighting_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_LightingMutex("Lighting Mutex")
{
	m_LightingMutex.Init(NULL);
	m_pAlarmManager=NULL;
}

//<-dceag-getconfig-b->
bool Lighting_Plugin::GetConfig()
{
	if( !Lighting_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_iCameraTimeout = atoi(DATA_Get_Timeout().c_str());
	if( !m_iCameraTimeout )
		m_iCameraTimeout = 30;

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(1);      // number of worker threads
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Lighting_Plugin::~Lighting_Plugin()
//<-dceag-dest-e->
{
}

void Lighting_Plugin::PrepareToDelete()
{
	PLUTO_SAFETY_LOCK(lm,m_LightingMutex);
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Lighting_Plugin::Register()
//<-dceag-reg-e->
{
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::LightingCommand ), 0, 0, 0, DEVICECATEGORY_Lighting_Device_CONST, MESSAGETYPE_COMMAND, 0 );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::DeviceState ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_OnOff_CONST );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::DeviceState ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_State_Changed_CONST );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::GetVideoFrame ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Get_Video_Frame_CONST );

	m_pListDeviceData_Router_Lights = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Lighting_Device_CONST);

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
void Lighting_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Lighting_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

bool Lighting_Plugin::DeviceState( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    // This only runs as a plug-in so we can safely cast it
    class DeviceData_Router *pDevice_RouterFrom = (class DeviceData_Router *) pDeviceFrom;
	string sLevel = "";    

	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "DEBUG...pMessage->m_mapParameters = %d",pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]);
	if( pMessage->m_dwID == EVENT_Device_OnOff_CONST )
	{
		sLevel = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST];
		if(sLevel == "0") {
			SetLightState( pMessage->m_dwPK_Device_From, false, 0 );
		} else if(sLevel == "1") {
			SetLightState( pMessage->m_dwPK_Device_From, true, 100 );
		} else {
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received OnOff event with wrong parameter value %s",sLevel.c_str());
		}
	} else if (pMessage->m_dwID == EVENT_State_Changed_CONST) {
		//Add support for event based dimming
		sLevel = pMessage->m_mapParameters[EVENTPARAMETER_State_CONST];
		int int_sLevel = atoi(sLevel.c_str());
		
		if(int_sLevel>0) {
			SetLightState(pMessage->m_dwPK_Device_From,true,int_sLevel);
		} else {
			SetLightState(pMessage->m_dwPK_Device_From,false,int_sLevel);
		}
	}
	
	return false;
}

bool Lighting_Plugin::LightingCommand( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	// This only runs as a plug-in so we can safely cast it
	class DeviceData_Router *pDevice_RouterTo = (class DeviceData_Router *) pDeviceTo;
	PreprocessLightingMessage(pDevice_RouterTo,pMessage);

	if( pMessage->m_sPK_Device_List_To.length() ) 
	{
		int PK_Device;  size_t pos=0;
		while( true )
		{
			PK_Device=atoi(StringUtils::Tokenize(pMessage->m_sPK_Device_List_To,",",pos).c_str());
			DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
			PreprocessLightingMessage(pDeviceData_Router,pMessage);
			if( pos>=pMessage->m_sPK_Device_List_To.length() || pos==string::npos )
				break;
		}
	}

	return false;
}

bool Lighting_Plugin::GetVideoFrame( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( !pDeviceTo )
		return false;

	bool bGotOne=false;
	DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceTo;  // We're a plugin, so this is a router instance
	for(map<int,DeviceRelation *>::iterator it=pDevice->m_mapDeviceRelation.begin();it!=pDevice->m_mapDeviceRelation.end();++it)
	{
		DeviceRelation *pDeviceRelation = (*it).second;
		DeviceData_Router *pDevice_Light = pDeviceRelation->m_pDevice;
		PLUTO_SAFETY_LOCK(lm,m_LightingMutex);
		if( pDevice_Light->WithinCategory(DEVICECATEGORY_Lighting_Device_CONST)==false )
			continue;

		bGotOne=true;

		map<int, pair<time_t,string> >::iterator it2 = m_mapLightsToRestore.find(pDevice_Light->m_dwPK_Device);
		if( it2!=m_mapLightsToRestore.end() )
		{
			it2->second.first=time(NULL)+m_iCameraTimeout;
			continue;
		}

		// We've got a light.  Restore it's current state in 30 seconds

		string sState = pDevice_Light->m_sState_get();

		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Lighting_Plugin::GetVideoFrame We've got a light with state %s for %d",sState.c_str(),pDevice_Light->m_dwPK_Device);
	
		if( sState.empty()==false )
			m_mapLightsToRestore[ pDevice_Light->m_dwPK_Device ] = make_pair<time_t,string> ( time(NULL)+m_iCameraTimeout, sState );

		DCE::CMD_On CMD_On(m_dwPK_Device,pDevice_Light->m_dwPK_Device,0,"");
		DCE::CMD_Set_Level CMD_Set_Level(m_dwPK_Device,pDevice_Light->m_dwPK_Device,"100");
		CMD_On.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST]="1";  // Means don't process it in the interceptor
		CMD_Set_Level.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST]="1";  // Means don't process it in the interceptor
		CMD_On.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Level.m_pMessage);
		SendCommand(CMD_On);
		
		// set the new light status
		SetLightState(pDevice_Light->m_dwPK_Device, true, 100, false);
	}

	if( bGotOne )
		SetLightingAlarm();
	return false;
}

//<-dceag-sample-b->!

/**

	@brief COMMANDS TO IMPLEMENT

*/


//<-dceag-createinst-b->!
//<-dceag-c184-b->

	/** @brief COMMAND: #184 - Set Level */
	/** Went sent by an orbiter, the plugin will adjust all lights in the Orbiter's room */
		/** @param #76 Level */
			/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

void Lighting_Plugin::CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c184-e->
{
}

void Lighting_Plugin::PreprocessLightingMessage(DeviceData_Router *pDevice,Message *pMessage)
{
	// If there's a parameter COMMANDPARAMETER_Advanced_options_CONST, that's an internal indicator which will mean nothing to the lighting device
	// But tells us that we sent the message ourselves and not to do anything with it here
	if( !pDevice || !pMessage || pMessage->m_mapParameters.find(COMMANDPARAMETER_Advanced_options_CONST)!=pMessage->m_mapParameters.end() )
		return;

	if( pMessage->m_dwID==COMMAND_Set_Level_CONST )
	{
		string sLevel = pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST];
		if( sLevel.size()==0 )
			pMessage->m_dwID=COMMAND_Generic_Off_CONST;
		else
		{
			int iLevel = atoi(sLevel.c_str());
			if( sLevel[0]=='+' )
				iLevel = min(100, GetLightingLevel(pDevice,0)+iLevel);
			else if( sLevel[0]=='-' )
				iLevel = max(0, GetLightingLevel(pDevice,0)+iLevel);

			if( pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_Light_Switch_onoff_CONST )
			{
				pMessage->m_dwID = iLevel<50 ? COMMAND_Generic_Off_CONST : COMMAND_Generic_On_CONST;
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Lighting_Plugin::PreprocessLightingMessage light %d Changed dim to %s", 
					pDevice->m_dwPK_Device, pMessage->m_dwID == COMMAND_Generic_Off_CONST ? "OFF" : "ON" );
			}
			else if( iLevel==0 )
				pMessage->m_dwID=COMMAND_Generic_Off_CONST;
			else
			{
				if( StringUtils::StartsWith(pDevice->m_sState_get(),"OFF",true) )
				{
					DCE::CMD_On CMD_On(m_dwPK_Device,pDevice->m_dwPK_Device,0,"");
					CMD_On.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST] = "1";
					SendCommand(CMD_On);

					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Lighting_Plugin::PreprocessLightingMessage turning on light before dim %d",
						pDevice->m_dwPK_Device);
				}

				SetLightState( pDevice->m_dwPK_Device, true, iLevel );
				pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST] = StringUtils::itos(iLevel);
			}
		}
	}

	if( pMessage->m_dwID==COMMAND_Generic_On_CONST )
		SetLightState( pDevice->m_dwPK_Device, true );
	else if( pMessage->m_dwID==COMMAND_Generic_Off_CONST )
		SetLightState( pDevice->m_dwPK_Device, false );
}

int Lighting_Plugin::GetLightingLevel(DeviceData_Router *pDevice,int iLevel_Default)
{
	string sState = pDevice->m_sState_get();
	string::size_type pos = sState.find("/");
	if( pos<sState.size()-1 && pos!=string::npos )
		return atoi(sState.substr(pos+1).c_str());
	else
		return iLevel_Default;
}

void Lighting_Plugin::SetLightState(int PK_Device,bool bIsOn,int Level, bool bRestore)
{
	DeviceData_Router *pDevice =  m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
	if( !pDevice )
		return; // Shouldn't happen
	if( Level==-1 )
		Level = GetLightingLevel( pDevice );

	pDevice->m_sState_set( (bIsOn ? "ON" : "OFF") + string("/") + StringUtils::itos(Level));

	if( bRestore )
	{
		PLUTO_SAFETY_LOCK(lm,m_LightingMutex);
		map<int, pair<time_t,string> >::iterator it=m_mapLightsToRestore.find(PK_Device);
		if( it!=m_mapLightsToRestore.end() )
		{
			m_mapLightsToRestore.erase(it);
			lm.Release();
			SetLightingAlarm();
		}
	}
}

void Lighting_Plugin::SetLightingAlarm()
{
	time_t tNextTime=0;
	PLUTO_SAFETY_LOCK(lm,m_LightingMutex);

	// Find the next alarm
	for(map<int, pair<time_t,string> >::iterator it=m_mapLightsToRestore.begin();it!=m_mapLightsToRestore.end();++it)
	{
		if( !tNextTime || it->second.first<tNextTime )
			tNextTime = it->second.first;
	}
	m_pAlarmManager->CancelAlarmByType(RESTORE_LIGHTING_STATE);
	if( tNextTime )
		m_pAlarmManager->AddAbsoluteAlarm(tNextTime,this,RESTORE_LIGHTING_STATE,NULL);
}

void Lighting_Plugin::AlarmCallback(int id, void* param)
{
	PLUTO_SAFETY_LOCK(lm,m_LightingMutex);

	time_t tNow = time(NULL);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Lighting_Plugin::AlarmCallback at time %d with map size %d",tNow,m_mapLightsToRestore.size());
	// Find the next alarm
	for(map<int, pair<time_t,string> >::iterator it=m_mapLightsToRestore.begin();it!=m_mapLightsToRestore.end();)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Lighting_Plugin::AlarmCallback We've got a light for %d with state %s and time %d",it->first,(it->second.second).c_str(),(int) it->second.first);
		if( it->second.first<=tNow )
		{
			string sState = it->second.second;
			string::size_type pos = sState.find("/");
			if( pos==string::npos )
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Lighting_Plugin::AlarmCallback %s is badly formed",sState.c_str());
			else
			{
				int iLevel = atoi( sState.substr( pos + 1 ).c_str() );
				if( sState[1]=='F' ) // It's an off
				{
					DCE::CMD_Set_Level CMD_Set_Level(m_dwPK_Device,it->first,StringUtils::itos(iLevel));
					DCE::CMD_Off CMD_Off(m_dwPK_Device,it->first,0);
					CMD_Off.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST]="1";  // Means don't process it in the interceptor
					CMD_Set_Level.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST]="1";  // Means don't process it in the interceptor
					CMD_Set_Level.m_pMessage->m_vectExtraMessages.push_back(CMD_Off.m_pMessage);
					SendCommand(CMD_Set_Level);
					
					// set the light state
					SetLightState(it->first, false, 0, false);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Lighting_Plugin::AlarmCallback We've got a light for %d with state %s",it->first,sState.c_str());

					DCE::CMD_On CMD_On(m_dwPK_Device,it->first,0,"");
					DCE::CMD_Set_Level CMD_Set_Level(m_dwPK_Device,it->first,StringUtils::itos(iLevel));
					CMD_On.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST]="1";  // Means don't process it in the interceptor
					CMD_Set_Level.m_pMessage->m_mapParameters[COMMANDPARAMETER_Advanced_options_CONST]="1";  // Means don't process it in the interceptor
					CMD_On.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Level.m_pMessage);
					SendCommand(CMD_On);
					
					// set the light state
					SetLightState(it->first, true, 100, false);
				}
			}
			m_mapLightsToRestore.erase(it++);
		}
		else
			++it;
	}

	lm.Release();
	SetLightingAlarm();
}
