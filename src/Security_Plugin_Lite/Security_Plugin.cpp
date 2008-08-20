/*
 Security_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

/**
 *
 * @file  Security_Plugin.cpp
 * @brief source file for the Security_Plugin class
 *
 */
 
//<-dceag-d-b->
#include "Security_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <sstream>

#include "DCERouter.h"
#include "DCE/DataGrid.h"
#include "DCE/DCEConfig.h"

#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_HouseMode.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_FloorplanObjectType_Color.h"
#include "pluto_main/Define_Screen.h"

// Alarm Types
#define PROCESS_SENSOR_TRIPPED				1
#define UPDATE_ALERT_LOG					2
#define PROCESS_CAMERAS						3

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Security_Plugin::Security_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Security_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_SecurityMutex("security")
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_SecurityMutex.Init(&m_MutexAttr);
	m_pAlarmManager=NULL;
	m_iAlertID=0;
}

//<-dceag-getconfig-b->
bool Security_Plugin::GetConfig()
{
	if( !Security_Plugin_Command::GetConfig() )
		return false;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Security_Plugin::GetConfig start");
//<-dceag-getconfig-e->

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      //4 = number of worker threads

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Security_Plugin::GetConfig get path");
	FileUtils::MakeDir(DATA_Get_Path()); // Just be sure it's here

	DCEConfig dce_config("/etc/fiire-ra/fiire-ra.conf");
	string sRADisabled = dce_config.m_mapParameters_Find("RA_DISABLED");
	if( sRADisabled!="1" )
	{
		m_sRA_User = dce_config.m_mapParameters_Find("USERNAME");
		m_sRA_Password = dce_config.m_mapParameters_Find("PASSWORD");
	}

	// Get the cameras for all the sensors
	for(std::map<int, DeviceData_Router *>::iterator it = m_pRouter->DataLayer()->Devices().begin(),
		end = m_pRouter->DataLayer()->Devices().end(); it != end; ++it)
	{
		DeviceData_Router *pDeviceData_Router = it->second;
		if( pDeviceData_Router->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) || pDeviceData_Router->WithinCategory(DEVICECATEGORY_IP_Cameras_CONST) )
		{
			string sSensors = pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_sPK_Device_Relations_For_Creat_CONST);
			string::size_type pos=0;
			while(pos<sSensors.size())
			{
				int iPK_Device = atoi(StringUtils::Tokenize( sSensors, ",\t", pos ).c_str());
				DeviceData_Router *pDevice_Sensor = m_pRouter->m_mapDeviceData_Router_Find( iPK_Device );
				if( pDevice_Sensor && pDevice_Sensor->WithinCategory(DEVICECATEGORY_Security_Device_CONST) )
					m_mapSensorsToCameras[ pDevice_Sensor->m_dwPK_Device ].push_back( pDeviceData_Router->m_dwPK_Device );
			}
		}
	}

	return true;
}

// TODO - handle deletion 


//<-dceag-const2-b->!

//<-dceag-dest-b->
Security_Plugin::~Security_Plugin()
//<-dceag-dest-e->
{
    pthread_mutexattr_destroy(&m_MutexAttr);
}

void Security_Plugin::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Security_Plugin::Register()
//<-dceag-reg-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Security_Plugin::Register start");
    RegisterMsgInterceptor((MessageInterceptorFn)(&Security_Plugin::SensorTrippedEvent) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Sensor_Tripped_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Security_Plugin::Register stop");

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
void Security_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Security_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}


//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c19-b->

	/** @brief COMMAND: #19 - Set House Mode */
	/** Sets the current security setting (at home, away, etc.) for the house */
		/** @param #5 Value To Assign */
			/** A value from the HouseMode table, or -1=toggle monitor mode */
		/** @param #17 PK_Users */
			/** The user setting the mode.  If this is 0, it will match any user who has permission to set the house mode. */
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

void Security_Plugin::CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage)
//<-dceag-c19-e->
{
}

bool Security_Plugin::SensorIsTripped(int PK_HouseMode,DeviceData_Router *pDevice)
{
	if( pDevice->m_sStatus_get()!="TRIPPED" )
		return false;
	return true;
}

bool Security_Plugin::SensorTrippedEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	if( 
		!pDeviceFrom || 
		(
			pDeviceFrom->m_dwPK_DeviceCategory != DEVICECATEGORY_Security_Device_CONST		&& 
			pDeviceFrom->m_dwPK_DeviceCategory != DEVICECATEGORY_Generic_IO_CONST				&&
			pDeviceFrom->m_dwPK_DeviceCategory != DEVICECATEGORY_Surveillance_Cameras_CONST	&&
			pDeviceFrom->m_dwPK_DeviceCategory != DEVICECATEGORY_IP_Cameras_CONST
		) 
	)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Receieved a sensor trip from an unrecognized device: %d",pMessage->m_dwPK_Device_From);
		return false;
	}
	bool bTripped = pMessage->m_mapParameters[EVENTPARAMETER_Tripped_CONST]=="1";
	return SensorTrippedEventHandler((DeviceData_Router *) pDeviceFrom,bTripped);
}

bool Security_Plugin::SensorTrippedEventHandler(DeviceData_Router *pDevice,bool bIsTripped)
{
	// If it's not being tripped, just return
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	pDevice->m_sStatus_set(bIsTripped ? "TRIPPED" : "NORMAL");
	if( bIsTripped==false )
		return false;

	string State = pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);
	string sPK_ModeChange = StringUtils::Tokenize(State,",",pos);

	// If the sensor is not armed, then do nothing
	if( Mode!="ARMED" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Security_Plugin::SensorTrippedEventHandler ignoring unarmed sensors %d",
			pDevice->m_dwPK_Device);
		return false;
	}

	SecurityAlert *pSecurityAlert = new SecurityAlert(m_iAlertID++,pDevice);
	m_mapSecurityAlert[ pSecurityAlert->m_iAlertID ] = pSecurityAlert;

	// Start taking the pictures
	if( m_sRA_User.empty()==false )
		m_pAlarmManager->AddRelativeAlarm( 0, this, PROCESS_CAMERAS, pSecurityAlert );  // This map also turn on lights

	// DelayedProcessSensor
	int iDelay = atoi( pDevice->m_mapParameters_Find(DEVICEDATA_Delay_CONST).c_str() );
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Security_Plugin::SensorTrippedEventHandler process %d in %d seconds",
		pDevice->m_dwPK_Device, iDelay);
	m_pAlarmManager->AddRelativeAlarm( iDelay, this, PROCESS_SENSOR_TRIPPED, pSecurityAlert );

	return false;
}

void Security_Plugin::ProcessSensorTripped(SecurityAlert *pSecurityAlert)
{
	string State = pSecurityAlert->m_pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);

	// If the sensor is not armed, then do nothing
	if( Mode!="ARMED" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Security_Plugin::ProcessSensorTripped sensor %d is now unarmed",
			pSecurityAlert->m_pDevice->m_dwPK_Device);
		pSecurityAlert->m_bSensorDisarmed=true;
	}
	else
	{
		Message *pMessage = new Message(pSecurityAlert->m_pDevice->m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Armed_Sensor_Tripped_CONST, 0);
		m_pEvent->SendMessage(pMessage);
	}

	m_pAlarmManager->AddRelativeAlarm( 0, this, UPDATE_ALERT_LOG, pSecurityAlert );
}

void Security_Plugin::UpdateAlertLog(SecurityAlert *pSecurityAlert)
{
	if( m_sRA_User.empty() )
		return;

	// TODO: use https and m_sRA_User

	string sURL = "http://cisco.fiire.com/AlertLog.php?EK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + 
		"&DateTime=" + StringUtils::itos((int) pSecurityAlert->m_tTime) +
		"&Counter=" + StringUtils::itos(pSecurityAlert->m_iAlertID) +
		"&EK_Device=" + StringUtils::itos(pSecurityAlert->m_pDevice->m_dwPK_Device) +
		"&SensorDisarmed=" + (pSecurityAlert->m_bSensorDisarmed ? "1" : "0");
// http://cisco.fiire.com/AlertLog.php?EK_Installation=1&DateTime=1219198786&Counter=0&EK_Device=17&SensorDisarmed=0

#ifdef WIN32
	size_t size;
	char *pPtr = FileUtils::ReadURL(sURL,size,true);
	if( pPtr==NULL || size==0 || strstr(pPtr,"Added_OK")==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Security_Plugin::UpdateAlertLog failed to update %s %d", pPtr, size);
		m_pAlarmManager->AddRelativeAlarm( 300, this, UPDATE_ALERT_LOG, pSecurityAlert );  // Try again in 5 minutes
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Security_Plugin::UpdateAlertLog success: %s %d", pPtr, size);
#else
	
	char * buffer = NULL;
	int iResult = http_fetch(sUrl.c_str(), &buffer);
#endif
}

void Security_Plugin::AlarmCallback(int id, void* param)
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	if( id==PROCESS_SENSOR_TRIPPED )
		ProcessSensorTripped((SecurityAlert *) param);
	else if( id==UPDATE_ALERT_LOG )
		UpdateAlertLog((SecurityAlert *) param);
	else if( id==PROCESS_CAMERAS )
		ProcessCameras((SecurityAlert *) param);
}

string Security_Plugin::GetModeString(int PK_HouseMode)
{
	switch(PK_HouseMode)
	{

	case HOUSEMODE_Armed_away_CONST:
		return "ARMED";
	case HOUSEMODE_Armed_at_home_CONST:
		return "AT HOME SECURE";
	case HOUSEMODE_Sleeping_CONST:
		return "SLEEPING";
	case HOUSEMODE_Entertaining_CONST:
		return "ENTERTAINING";
	case HOUSEMODE_Armed_Extended_away_CONST:
		return "EXTENDED AWAY";
	case HOUSEMODE_Unarmed_at_home_CONST:
	default:
		return "UNARMED";
	}
}
int Security_Plugin::GetModeID(string Mode)
{
	if( Mode=="ARMED" )
		return HOUSEMODE_Armed_away_CONST;
	else if( Mode=="AT HOME SECURE" )
		return HOUSEMODE_Armed_at_home_CONST;
	else if( Mode=="SLEEPING" )
		return HOUSEMODE_Sleeping_CONST;
	else if( Mode=="ENTERTAINING" )
		return HOUSEMODE_Entertaining_CONST;
	else if( Mode=="EXTENDED AWAY" )
		return HOUSEMODE_Armed_Extended_away_CONST;
	else
		return HOUSEMODE_Unarmed_at_home_CONST;
}

//<-dceag-createinst-b->!

void Security_Plugin::ProcessCameras(SecurityAlert *pSecurityAlert)
{
	bool bGotACamera=false;

	// See what cameras we should still grab frames from
	for( list<int>::iterator it=m_mapSensorsToCameras[pSecurityAlert->m_pDevice->m_dwPK_Device].begin(); it!=m_mapSensorsToCameras[pSecurityAlert->m_pDevice->m_dwPK_Device].end(); ++it )
	{
		DeviceData_Router *pDevice_Camera = m_pRouter->m_mapDeviceData_Router_Find( *it );
		if( pDevice_Camera )
		{
            int iSeconds = atoi( pDevice_Camera->m_mapParameters_Find(DEVICEDATA_Video_settings_CONST).c_str() );
			if( iSeconds==0 )
				iSeconds=5;

			if( pSecurityAlert->m_tTime + iSeconds > time(NULL) )
			{
				ProcessCamera(pSecurityAlert,pDevice_Camera);
				bGotACamera=true;
			}
		}
	}

	if( bGotACamera )  // We still found cameras we're supposed to be checking
		m_pAlarmManager->AddRelativeAlarm( 1, this, PROCESS_CAMERAS, pSecurityAlert ); 
}

void Security_Plugin::ProcessCamera(SecurityAlert *pSecurityAlert,DeviceData_Router *pDevice_Camera)
{
	char *pData=NULL;
	int size=0;
	string format;

	DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame(m_dwPK_Device,pDevice_Camera->m_dwPK_Device, "", 0, 0, 0, &pData, &size, &format);
	if( SendCommand(CMD_Get_Video_Frame) && size>0 && pData!=NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Security_Plugin::ProcessCamera got a %d size image from %d", size, pDevice_Camera->m_dwPK_Device);
		// TODO post this online
	}
}

//<-dceag-c387-b->

	/** @brief COMMAND: #387 - Verify PIN */
	/** Verifies a user's PIN Code */
		/** @param #17 PK_Users */
			/** The user */
		/** @param #40 IsSuccessful */
			/** true if successful, false otherwise */
		/** @param #99 Password */
			/** The pin code, either raw or preferrably in md5 format for security */

void Security_Plugin::CMD_Verify_PIN(int iPK_Users,string sPassword,bool *bIsSuccessful,string &sCMD_Result,Message *pMessage)
//<-dceag-c387-e->
{
}

//<-dceag-c969-b->

	/** @brief COMMAND: #969 - Restore To NonTemp State */
	/** Restore a security device to the state in State_NonTemporary */
		/** @param #2 PK_Device */
			/** The device to restore */

void Security_Plugin::CMD_Restore_To_NonTemp_State(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c969-e->
{
}

//<-dceag-c972-b->

	/** @brief COMMAND: #972 - Set Sensor State */
	/** Sets the state of a sensor */
		/** @param #2 PK_Device */
			/** The sensor */
		/** @param #5 Value To Assign */
			/** Can be: ARMED or UNARMED */

void Security_Plugin::CMD_Set_Sensor_State(int iPK_Device,string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c972-e->
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( pDevice==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Security_Plugin::CMD_Set_Sensor_State unknown device %d", iPK_Device);
		return;
	}

	string State = pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);
	string sPK_ModeChange = StringUtils::Tokenize(State,",",pos);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Security_Plugin::CMD_Set_Sensor_State device %d was %s now %s", 
		iPK_Device, Mode.c_str(), sValue_To_Assign.c_str() );

	pDevice->m_sState_set(sValue_To_Assign + "," + Bypass + "," + sPK_ModeChange);
	m_pRouter->DataLayer()->Save();		
}
