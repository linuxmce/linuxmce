
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
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"
#include "Notification.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
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
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_FloorplanObjectType_Color.h"
#include "pluto_main/Define_Screen.h"

#include "pluto_security/Table_Alert.h"
#include "pluto_security/Table_Alert_Device.h"
#include "pluto_security/Table_AlertType.h"
#include "pluto_security/Table_ModeChange.h"

#include "Gen_Devices/AllScreens.h"

// Alarm Types
#define PROCESS_ALERT					1
#define PROCESS_COUNTDOWN_BEFORE_ALARM	2
#define PROCESS_COUNTDOWN_BEFORE_ARMED	3
#define PROCESS_MODE_CHANGE				4

// Non-alert types
#define ALERTTYPE_DONOTHING				0
#define ALERTTYPE_ANNOUNCMENT			-1
#define ALERTTYPE_PHOTO					-2

#define RAPID_COUNTDOWN					10  // For all countdowns, once it gets below this # of seconds, it will only state the number alone

#define COUNTDOWN_INTERVAL_1			10  // How many seconds to wait when making the initial, full announcement
#define COUNTDOWN_INTERVAL_2			5	// How many seconds to wait for subsequence announcement, while still not in RAPID_COUNTDOWN
#define COUNTDOWN_INTERVAL_3			2	// How many seconds to wait for the RAPID_COUNTDOWN

void* StartNotification( void* param ) 
{
	Notification *pNotification = (Notification*)param;
	pNotification->DoIt();
	delete pNotification;
	return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Security_Plugin::Security_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Security_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_SecurityMutex("security")
{
	m_tExitTime=0;
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_SecurityMutex.Init(&m_MutexAttr);
	m_pDatabase_pluto_main=NULL;
	m_pDatabase_pluto_security=NULL;
	m_pDeviceData_Router_this=NULL;
	m_pAlarmManager=NULL;
	m_bBabySitterMode = false;
}

//<-dceag-getconfig-b->
bool Security_Plugin::GetConfig()
{
	if( !Security_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		m_bQuit_set(true);
		return false;
	}

	m_pDatabase_pluto_security = new Database_pluto_security(LoggerWrapper::GetInstance());
	if( !m_pDatabase_pluto_security->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_security", m_pRouter->iDBPort_get( ) ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		m_bQuit_set(true);
		return false;
	}

	// Get the last mode change for each zone
	string sql = "select max(PK_ModeChange),EK_DeviceGroup FROM ModeChange GROUP BY EK_DeviceGroup";
	// Don't worry about multi-threading--this won't be a concern until we register

	PlutoSqlResult result;
    DB_ROW row;
	if( (result.r = m_pDatabase_pluto_security->db_wrapper_query_result(sql.c_str())) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
			m_mapRow_ModeChange_Last[row[1] ? atoi(row[1]) : 0] = m_pDatabase_pluto_security->ModeChange_get()->GetRow( atoi(row[0]) );
	}

	m_pDeviceData_Router_this = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);
	m_bMonitorMode = m_pDeviceData_Router_this->m_sStatus_get()=="1";

	Row_Text_LS *pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_alarm_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sCountdownBeforeAlarm = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_alarm_CONST,1);
		if( pRow_Text_LS )
			m_sCountdownBeforeAlarm = pRow_Text_LS->Description_get();
		else
			m_sCountdownBeforeAlarm = "Alarm will sound in <%=SEC%> seconds.";
	}

	pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_armed_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sCountdownBeforeArmed = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Countdown_before_armed_CONST,1);
		if( pRow_Text_LS )
			m_sCountdownBeforeArmed = pRow_Text_LS->Description_get();
		else
			m_sCountdownBeforeArmed = "Alarm will be active in <%=SEC%> seconds.";
	}

	pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_alarm_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sShortCountdownBeforeAlarm = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_alarm_CONST,1);
		if( pRow_Text_LS )
			m_sShortCountdownBeforeAlarm = pRow_Text_LS->Description_get();
		else
			m_sShortCountdownBeforeAlarm = "<%=SEC%> seconds.";
	}

	pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_armed_CONST,m_pRouter->iPK_Language_get());
	if( pRow_Text_LS )
		m_sShortCountdownBeforeArmed = pRow_Text_LS->Description_get();
	else
	{
		pRow_Text_LS = m_pDatabase_pluto_main->Text_LS_get()->GetRow(TEXT_Short_countdown_before_armed_CONST,1);
		if( pRow_Text_LS )
			m_sShortCountdownBeforeArmed = pRow_Text_LS->Description_get();
		else
			m_sShortCountdownBeforeArmed = "<%=SEC%> seconds.";
	}

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      //4 = number of worker threads

	DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Text_To_Speech_CONST);
	m_PK_Device_TextToSpeach = pDeviceData_Base ? pDeviceData_Base->m_dwPK_Device : 0;

	FileUtils::MakeDir(DATA_Get_Path()); // Just be sure it's here

	// Get the zones
    for(map<int,class DeviceGroup *>::const_iterator it=m_pRouter->m_mapDeviceGroup_get()->begin();it!=m_pRouter->m_mapDeviceGroup_get()->end();++it)
	{
		DeviceGroup *pDeviceGroup = it->second;
		if( pDeviceGroup->m_Type==1 )
			m_mapDeviceGroup[pDeviceGroup->m_dwPK_DeviceGroup]=pDeviceGroup;
	}

	GetHouseModes();
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Security_Plugin::~Security_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	delete m_pDatabase_pluto_security;

    pthread_mutexattr_destroy(&m_MutexAttr);
}

void Security_Plugin::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;

	if( m_mapNotification.size() )
	{
		for( map<pthread_t,Notification *>::iterator it=m_mapNotification.begin();it!=m_mapNotification.end();++it)
			pthread_cancel(it->first);
	}

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Security_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pTelecom_Plugin=( Telecom_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Telecom_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin || !m_pTelecom_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to security plugin");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator( 
		new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Security_Plugin::SecurityScenariosGrid ) )
		, DATAGRID_Security_Scenarios_CONST,PK_DeviceTemplate_get() );

    RegisterMsgInterceptor((MessageInterceptorFn)(&Security_Plugin::SensorTrippedEvent) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Sensor_Tripped_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Security_Plugin::OrbiterRegistered) ,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Orbiter_Registered_CONST);

	RegisterMsgInterceptor((MessageInterceptorFn)(&Security_Plugin::PanelChangeState) ,0,0,0,0,
		MESSAGETYPE_EVENT,67);

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

class DataGridTable *Security_Plugin::SecurityScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	vector<Row_CommandGroup *> vectRowCommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get( )->GetRows( COMMANDGROUP_FK_ARRAY_FIELD + string( "=" ) + StringUtils::itos( ARRAY_Security_Scenarios_CONST ) + " AND " 
			+ COMMANDGROUP_FK_INSTALLATION_FIELD + "=" + StringUtils::itos( m_pRouter->iPK_Installation_get( ) ), &vectRowCommandGroup );
	for( size_t s=0;s<vectRowCommandGroup.size( );++s )
	{
		Row_CommandGroup *pRow_CommandGroup = vectRowCommandGroup[s];
		pCell = new DataGridCell( pRow_CommandGroup->Description_get( ), StringUtils::itos( pRow_CommandGroup->PK_CommandGroup_get( ) ) );
		pDataGrid->SetData( 0, ( int ) s, pCell );
	}

	return pDataGrid;
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
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	int PK_HouseMode = atoi(sValue_To_Assign.c_str());

	if( sPassword.size()>4 )
	{
		string head = sPassword.substr( 0 , 4 );
		if( head == "1119" )            //Baby sitter mode
		{
			sPassword = sPassword.substr( 4 , sPassword.size() - 1);
			m_bBabySitterMode = true;
		}
		else
			m_bBabySitterMode = false;
	}
	else
		m_bBabySitterMode = false;

	// The password can either be the password or teh PIN code, and either plain text or md5.  iPK_Users is optional
	ostringstream sql;
	sql << "SELECT PK_Users,Username FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation="
		<< m_pRouter->iPK_Installation_get() << " AND userCanChangeHouseMode=1 AND (Password='" << sPassword 
		<< "' OR Password='" << m_pRouter->md5(sPassword) << "' OR PINCode='" << sPassword << "' OR PINCode='" << m_pRouter->md5(sPassword) << "')";

	if( iPK_Users )
		sql << " AND PK_Users=" << iPK_Users;

	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( ( result_set.r=m_pRouter->db_wrapper_query_result( sql.str( ) ) )==0 || ( row = db_wrapper_fetch_row( result_set.r ) )==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"User: %d failed to set house mode: %d",iPK_Users,PK_HouseMode);
		LoggerWrapper::GetInstance()->Write(LV_WARNING,sql.str().c_str());
		DCE::CMD_Set_Text CMD_Set_Text( 0, pMessage->m_dwPK_Device_From, "", "***Invalid PIN***", TEXT_PIN_Code_CONST );
		DCE::CMD_Set_Variable CMD_Set_Variable( 0, pMessage->m_dwPK_Device_From, VARIABLE_PasswordPin_CONST, "" );
		CMD_Set_Text.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable.m_pMessage);
		SendCommand( CMD_Set_Text );
		return;		
	}

	m_tExitTime=0;
	m_pAlarmManager->CancelAlarmByType(PROCESS_COUNTDOWN_BEFORE_ARMED);
	m_pAlarmManager->CancelAlarmByType(PROCESS_MODE_CHANGE);
	m_pAlarmManager->CancelAlarmByType(PROCESS_COUNTDOWN_BEFORE_ALARM);
	m_pAlarmManager->CancelAlarmByType(PROCESS_ALERT);

	if( PK_HouseMode==-1 )
	{
		m_bMonitorMode=!m_bMonitorMode;
		SetMonitorModeBoundIcon();
		return;
	}

	if( PK_HouseMode<HOUSEMODE_Unarmed_at_home_CONST || PK_HouseMode>HOUSEMODE_Armed_Extended_away_CONST )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Attempt to set invalid house mode: %d",PK_HouseMode);
		return;
	}

	list<DeviceData_Router *> listDevice_Blocked; // All devices that are blocked
	bool bSensorsActive=false; // Will be set to true if any sensors are set to active/arme
	map<int,DeviceData_Router *> mapResetDevices; // Keep track of the devices we change so we can reset any pending alarms that are affected

	DeviceGroup *pDeviceGroup=NULL;
	if( iPK_DeviceGroup )
	{
		pDeviceGroup = m_pRouter->m_mapDeviceGroup_Find(iPK_DeviceGroup);
		if( !pDeviceGroup )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trying to set house mode with invalid device group: %d",iPK_DeviceGroup);
			return;
		}

		for(size_t s=0;s<pDeviceGroup->m_vectDeviceData_Base.size();++s)
		{
			DeviceData_Router *pDevice = (DeviceData_Router *) pDeviceGroup->m_vectDeviceData_Base[s];  // it's coming from the router, so it's safe to cast it
			if( pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_Security_Device_CONST && pDevice->m_dwPK_DeviceCategory!=DEVICECATEGORY_Generic_IO_CONST )
				continue;
			mapResetDevices[pDevice->m_dwPK_Device] = pDevice;
			if( !SetHouseMode(pDevice,iPK_Users,PK_HouseMode,sHandling_Instructions) )
				listDevice_Blocked.push_back(pDevice);
			if( !bSensorsActive && GetAlertType(PK_HouseMode,pDevice)==ALERTTYPE_Security_CONST )
				bSensorsActive=true;
		}
	}
	else
	{
		for(int i=0;i<2;++i)
		{
			ListDeviceData_Router *pListDeviceData_Router = 
				(i==0 ? m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Security_Device_CONST) : m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Generic_IO_CONST) );
			if( pListDeviceData_Router )
			{
				for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
				{
					DeviceData_Router *pDevice = *it;
					mapResetDevices[pDevice->m_dwPK_Device] = pDevice;
					if( !SetHouseMode(pDevice,iPK_Users,PK_HouseMode,sHandling_Instructions) )
						listDevice_Blocked.push_back(pDevice);
					if( !bSensorsActive && GetAlertType(PK_HouseMode,pDevice)==ALERTTYPE_Security_CONST )
						bSensorsActive=true;
				}
			}
		}
	}
	if( listDevice_Blocked.size() )
	{
		string sText;
		for(list<DeviceData_Router *>::iterator it=listDevice_Blocked.begin();it!=listDevice_Blocked.end();++it)
			sText += (*it)->m_sDescription + "  ";

		DCE::SCREEN_SensorsNotReady SCREEN_SensorsNotReady(0, pMessage->m_dwPK_Device_From, sText);
		SendCommand(SCREEN_SensorsNotReady);

		return;
	}

	string sPreviousHouseMode = StringUtils::ltos(m_mapPK_HouseMode[iPK_DeviceGroup]);
	m_mapPK_HouseMode[iPK_DeviceGroup]=PK_HouseMode;
	SaveHouseModes();
	Row_AlertType *pRow_AlertType = m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_Security_CONST);

	bool bSecurityOrFire;
	string sAlerts = AlertsSinceLastChange(iPK_DeviceGroup,bSecurityOrFire);

	SetHouseModeBoundIcon(iPK_DeviceGroup);
	
	string sPK_HouseMode = StringUtils::ltos(PK_HouseMode);
	string sHouseModeTime = pRow_AlertType->ExitDelay_get() && bSensorsActive ? "<%=CD%> seconds" : "IMMEDIATELY";
	string sExitDelay;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Set House Mode.  Sensors active: %d exit delay: %d",(int) bSensorsActive,pRow_AlertType->ExitDelay_get());
	if( pRow_AlertType->ExitDelay_get() )
	{
		sExitDelay = StringUtils::itos(pRow_AlertType->ExitDelay_get());

		if( bSensorsActive && DATA_Get_PK_Device().size() )
		{
			m_bExitDelay_New=true;
			m_tExitTime=time(NULL) + pRow_AlertType->ExitDelay_get();
			// Even if there's a delay, don't do the countdown unless the user is leaving the house
			if( PK_HouseMode==HOUSEMODE_Armed_away_CONST || PK_HouseMode==HOUSEMODE_Armed_Extended_away_CONST )
				m_pAlarmManager->AddRelativeAlarm(0,this,PROCESS_COUNTDOWN_BEFORE_ARMED,NULL);
		}
	}

	// We want to be sure to fire tripped status's for any sensors that may have gotten tripped during the countdown
	m_pAlarmManager->AddRelativeAlarm(pRow_AlertType->ExitDelay_get(),this,PROCESS_MODE_CHANGE,NULL);

	//Send to all orbiter the message
	DCE::SCREEN_ModeChanged SCREEN_ModeChanged(0, pMessage->m_dwPK_Device_From, sPK_HouseMode, 
		sHouseModeTime, sExitDelay, sAlerts);
	SendCommand(SCREEN_ModeChanged);

	Row_ModeChange *pRow_ModeChange = m_pDatabase_pluto_security->ModeChange_get()->AddRow();
	pRow_ModeChange->EK_HouseMode_set(PK_HouseMode);
	pRow_ModeChange->ChangeTime_set(StringUtils::SQLDateTime(time(NULL)));
	if( iPK_Users )
		pRow_ModeChange->EK_Users_set( iPK_Users );
	pRow_ModeChange->Table_ModeChange_get()->Commit();
	m_mapRow_ModeChange_Last[pDeviceGroup ? pDeviceGroup->m_dwPK_DeviceGroup : 0]=pRow_ModeChange;

	string NewMode = GetModeString(PK_HouseMode);
	for(map<int,DeviceData_Router *>::iterator it=mapResetDevices.begin();it!=mapResetDevices.end();++it)
	{
		DeviceData_Router *pDevice = (*it).second;

		string State = pDevice->m_sState_get();
		string::size_type pos=0;
		string Mode = StringUtils::Tokenize(State,",",pos);
		string Bypass = StringUtils::Tokenize(State,",",pos);
		pDevice->m_sState_set(NewMode + "," + Bypass + "," + StringUtils::itos(pRow_ModeChange->PK_ModeChange_get()));
	}

	for(vector<Row_Alert *>::iterator it=m_vectPendingAlerts.begin();it!=m_vectPendingAlerts.end();)
	{
		Row_Alert *pRow_Alert = *it;
		if( mapResetDevices.find(pRow_Alert->EK_Device_get())!=mapResetDevices.end() )
		{
			// We've reset this device
			pRow_Alert->ResetTime_set(StringUtils::SQLDateTime(time(NULL)));
			pRow_Alert->Table_Alert_get()->Commit();
			it=m_vectPendingAlerts.erase(it);
		}
		else 
			it++;
	}

	//also send the command to the alarm panel
	vector<Row_Device *> vectAlarmPanelDevices;
	m_pDatabase_pluto_main->Device_get()->GetRows(
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE (FK_DeviceCategory = " + StringUtils::ltos(DEVICECATEGORY_Security_Interface_CONST) + " OR FK_DeviceCategory = " + StringUtils::ltos(DEVICECATEGORY_Specialized_CONST) + ")" +
		" AND FK_Installation = " + StringUtils::ltos(m_pRouter->iPK_Installation_get()), &vectAlarmPanelDevices);

	if(vectAlarmPanelDevices.size())
	{
		string sDevices;
		for(vector<Row_Device *>::iterator it = vectAlarmPanelDevices.begin(); it != vectAlarmPanelDevices.end(); ++it)
		{
			if(it != vectAlarmPanelDevices.begin())
				sDevices += ",";

			Row_Device *pRow_Device = *it;
			sDevices += StringUtils::ltos(pRow_Device->PK_Device_get());
		}

		DCE::CMD_Set_House_Mode_DL CMD_Set_House_Mode_DL_(m_dwPK_Device, sDevices, sValue_To_Assign,
			iPK_Users, sPassword, iPK_DeviceGroup, sHandling_Instructions);
		SendCommand(CMD_Set_House_Mode_DL_);
	}

	if( bSecurityOrFire )
		EVENT_Reset_Alarm();  // There were breaches, which likely caused media to start playing.  Reset the alarm
	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(m_dwPK_Device,m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,
		"",SCREEN_SecurityPanel_CONST);
	SendCommand(CMD_Remove_Screen_From_History_DL);
	EVENT_House_Mode_Changed(iPK_DeviceGroup,PK_HouseMode,sPreviousHouseMode);
}

string Security_Plugin::AlertsSinceLastChange(int PK_DeviceGroup,bool &bSecurityOrFire)
{
	bSecurityOrFire=false;
	Row_ModeChange *pRow_ModeChange = m_mapRow_ModeChange_Last_Find(PK_DeviceGroup);
	if( !pRow_ModeChange )
		return "";

	string sAlert;
	vector<Row_Alert *> vectRow_Alert;
	this->m_pDatabase_pluto_security->Alert_get()->GetRows("DetectionTime>='" + pRow_ModeChange->ChangeTime_get() + "'",&vectRow_Alert);
	for(size_t s=0;s<vectRow_Alert.size();++s)
	{
		Row_Alert *pRow_Alert = vectRow_Alert[s];
		if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Security_CONST || pRow_Alert->FK_AlertType_get()==ALERTTYPE_Fire_CONST || pRow_Alert->FK_AlertType_get()==ALERTTYPE_Air_Quality_CONST )
			bSecurityOrFire=true;

		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(pRow_Alert->EK_Device_get());
		sAlert += pRow_Alert->DetectionTime_get() + " ";
		if( pDevice )
			sAlert += pDevice->m_sDescription;
		if( pRow_Alert->PhotoOnly_get()==1 )
			sAlert += " (photo only)";
		if( pRow_Alert->AnnouncementOnly_get()==1 )
			sAlert += " (announce only)";

		sAlert+= "\n";
	}

	return sAlert;
}

bool Security_Plugin::SetHouseMode(DeviceData_Router *pDevice,int iPK_Users,int PK_HouseMode,string sHandlingInstructions) 
{
	// Don't set the new mode, our caller will do that when it confirms all the devices can be set
	string State = pDevice->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);
	string sPK_ModeChange = StringUtils::Tokenize(State,",",pos);

	if( Bypass=="PERMBYPASS" )
	{
		pDevice->m_sState_set(Mode + "," + Bypass);
		return true; // We're bypassing this one
	}

	if( SensorIsTripped(PK_HouseMode,pDevice) )
	{
		if( sHandlingInstructions!="W" && sHandlingInstructions!="B" )
			return false;  // Sensors are tripped.  The user needs to figure out what to do

		if( sHandlingInstructions=="W" )
			Bypass = "WAIT";
		else if( sHandlingInstructions=="B" )
			Bypass = "BYPASS";
		else
			Bypass = "";
	}
	else
		Bypass = "";

	pDevice->m_sState_set(Mode + "," + Bypass + "," + sPK_ModeChange);

	return true;
}

bool Security_Plugin::SensorIsTripped(int PK_HouseMode,DeviceData_Router *pDevice)
{
	if( pDevice->m_sStatus_get()!="TRIPPED" )
		return false;

	int PK_AlertType = GetAlertType(PK_HouseMode,pDevice);
	return PK_AlertType>0;
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

bool Security_Plugin::PanelChangeState(class Socket *pSocket,class Message *pMessage,
class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	string sPartNo,sActualState,sErrorMsg;
	string aux;
	string::size_type pos=0;

	sPartNo = pMessage->m_mapParameters[EVENTPARAMETER_ZoneNo_CONST];
	aux = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
	sActualState = StringUtils::Tokenize(aux, ",", pos);
	sErrorMsg = StringUtils::Tokenize(aux, ",", pos);

	//for now only inform orbiter about error
	if( !sErrorMsg.empty() )
	{
		SCREEN_PopupMessage_DL SCREEN_PopupMessage_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(),
			sErrorMsg, // Main message
			"", // Command Line
			"security_error", // Description
			"0", // sPromptToResetRouter
			"0", // sTimeout
			"1"); // sCannotGoBack
		SendCommand(SCREEN_PopupMessage_DL);
	}

	return false;
}

bool Security_Plugin::SensorTrippedEventHandler(DeviceData_Router *pDevice,bool bIsTripped)
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	pDevice->m_sStatus_set(bIsTripped ? "TRIPPED" : "NORMAL");

	string State = pDevice->m_sState_get();
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "XXX: Device: %d, State: %s", pDevice->m_dwPK_Device, State.c_str());
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(State,",",pos);
	string Bypass = StringUtils::Tokenize(State,",",pos);
	string sPK_ModeChange = StringUtils::Tokenize(State,",",pos);

	int PK_HouseMode = GetModeID(Mode);

	string sConfiguration = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_dwPK_Device,DEVICEDATA_Configuration_CONST)->IK_DeviceData_get();
	pos = 0;
	StringUtils::Tokenize(sConfiguration, ",", pos);
	string HouseMode = StringUtils::Tokenize(sConfiguration, ",", pos);
	PK_HouseMode = atoi(HouseMode.c_str());

	if( Bypass=="WAIT" || bIsTripped==false )
	{
		if( bIsTripped==false )  // We're not waiting any more
			pDevice->m_sState_set(Mode + ",," + sPK_ModeChange );
		return false;
	}

	// if the BabySitter mode is on and is a door log the alert
	if( m_bBabySitterMode   ) 
	{
		int nObjectType = atoi(pDevice->m_mapParameters[DEVICEDATA_PK_FloorplanObjectType_CONST].c_str());
		if( nObjectType == 1 )  // DOOR
		{
			Row_Alert *pRow_Alert = LogAlert( m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_BabySitter_mode_CONST),pDevice,true,true);
			if( pRow_Alert )
				ProcessAlert(pRow_Alert);
		}
	}

	// The first digit in the alert data is 0/1 for monitor mode
	if( m_bMonitorMode && atoi(pDevice->m_mapParameters[DEVICEDATA_Alert_CONST].c_str()) )
	{
		// Monitor mode is on, so no matter what notify the appropriate users
		Row_Alert *pRow_Alert = LogAlert( m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_Monitor_mode_CONST),pDevice,true,true);
		if( pRow_Alert )
			ProcessAlert(pRow_Alert);
	}


	bool bNotify=false,bAnnouncementOnly=false;
	int PK_AlertType = GetAlertType(PK_HouseMode,pDevice,&bNotify);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "XXX: PK_AlertType: %d, PK_HouseMode: %d", PK_AlertType, PK_HouseMode);
	if( PK_AlertType<=0 )
	{
		bAnnouncementOnly=true; // This is something information only, like an announcement, or only has a notification and do nothing
		// It's some type of information alert
		if( PK_AlertType==ALERTTYPE_ANNOUNCMENT )
			AnnounceAlert(pDevice);
		if( PK_AlertType!=ALERTTYPE_DONOTHING && !bNotify )  // If there's a notification, we're going to process it anyway
			LogAlert( m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_Information_CONST),pDevice,bAnnouncementOnly,bNotify);
		if( !bNotify )
			return false;
		else
			// Get the real alert type.  We already set bAnnouncementOnly=true so we know not to fire the event
			PK_AlertType = atoi(pDevice->m_mapParameters[DEVICEDATA_EK_AlertType_CONST].c_str());
	}

	// What type of alert is this?  Maybe it's just informational
	Row_AlertType *pRow_AlertType = m_pDatabase_pluto_security->AlertType_get()->GetRow(PK_AlertType);
	if( !pRow_AlertType )
		pRow_AlertType = m_pDatabase_pluto_security->AlertType_get()->GetRow(ALERTTYPE_Information_CONST);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Alert type is %d %p, delay %s #pending: %d",
		PK_AlertType,pRow_AlertType,sPK_ModeChange.c_str(),(int) m_vectPendingAlerts.size());

	if( atoi(sPK_ModeChange.c_str()) && pRow_AlertType->ExitDelay_get() )
	{
		Row_ModeChange *pRow_ModeChange = m_pDatabase_pluto_security->ModeChange_get()->GetRow(atoi(sPK_ModeChange.c_str()));
		if( pRow_ModeChange )
		{
			time_t tChange = StringUtils::SQLDateTime(pRow_ModeChange->ChangeTime_get());
	time_t tnow = time(NULL);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Alert was %d-%d=%d seconds ago",
				(int) tnow, (int) tChange, (int) tChange-tnow);
			if( tChange + pRow_AlertType->ExitDelay_get() > time(NULL) )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Still in exit delay");
				return false;
			}
		}
	}

	bool bSensorIsTripped = SensorIsTripped(PK_HouseMode,pDevice);
	if( Bypass!="BYPASS" && Bypass!="PERMBYPASS" && (bSensorIsTripped || bNotify))
	{
		Row_Alert *pRow_Alert=LogAlert(pRow_AlertType,pDevice,bAnnouncementOnly,bNotify);
		if( pRow_Alert )
		{
			m_pAlarmManager->AddAbsoluteAlarm(StringUtils::SQLDateTime(pRow_Alert->ExpirationTime_get()),this,PROCESS_ALERT,(void *) pRow_Alert);
			if( bSensorIsTripped && !bAnnouncementOnly &&
				(pRow_AlertType->PK_AlertType_get()==ALERTTYPE_Security_CONST ||
				pRow_AlertType->PK_AlertType_get()==ALERTTYPE_Fire_CONST || 
				pRow_AlertType->PK_AlertType_get()==ALERTTYPE_Air_Quality_CONST) )
			{
				m_pAlarmManager->AddRelativeAlarm(0,this,PROCESS_COUNTDOWN_BEFORE_ALARM,(void *) pRow_Alert);

				SCREEN_SecurityPanel_DL SCREEN_SecurityPanel_DL_(m_dwPK_Device,m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters);
				SendCommand(SCREEN_SecurityPanel_DL_);
			}
		}
	}

	return false;
}

int Security_Plugin::GetAlertType(int PK_HouseMode,DeviceData_Router *pDevice,bool *bNotify)
{
	// 0=Do nothing, 1=fire alert, 2=make announcement, 3=snap phto
	int Alert=0;
	string sAlertData = pDevice->m_mapParameters[DEVICEDATA_Alert_CONST];
	//monitor mode, disarmed, armed - away, armed - at home, sleeping, entertaining, extended away
	if( sAlertData.length()<13 )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Alert data not specified for %d",pDevice->m_dwPK_Device);
		return 0;
	}

	string::size_type pos=0;
	string s;
	for( int i=0;i<=PK_HouseMode;++i )
		s=StringUtils::Tokenize(sAlertData,",",pos); // Go to the Xth digit
	Alert = s.size() && s[0]=='N' ? atoi(s.substr(1).c_str()) : atoi(s.c_str());
	if( bNotify )
		*bNotify = s.find('N')!=string::npos;

	if( Alert==0 )
		return ALERTTYPE_DONOTHING;
	else if( Alert==2 )
		return ALERTTYPE_ANNOUNCMENT;
	else if( Alert==3 )
		return ALERTTYPE_PHOTO;
	return atoi( pDevice->m_mapParameters[DEVICEDATA_EK_AlertType_CONST].c_str() );
}

void Security_Plugin::AlarmCallback(int id, void* param)
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	if( id==PROCESS_ALERT )
		ProcessAlert((Row_Alert *) param);
	else if( id==PROCESS_COUNTDOWN_BEFORE_ALARM )
		ProcessCountdown(id,(Row_Alert *) param);
	else if( id==PROCESS_COUNTDOWN_BEFORE_ARMED )
		ProcessCountdown(id,NULL);
	else if( id==PROCESS_MODE_CHANGE )
	{
		for(int i=0;i<2;++i)
		{
			ListDeviceData_Router *pListDeviceData_Router = 
				(i==0 ? m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Security_Device_CONST) : m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Generic_IO_CONST) );
			if( pListDeviceData_Router )
			{
				for( ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
				{
					DeviceData_Router *pDeviceData_Router = *it;
					string sState = pDeviceData_Router->m_sState_get();
					string::size_type pos=0;
					string Mode = StringUtils::Tokenize(sState,",",pos);
					int PK_HouseMode = GetModeID(Mode);
					if( SensorIsTripped(PK_HouseMode,pDeviceData_Router) )
						SensorTrippedEventHandler(pDeviceData_Router,true);
				}
			}
		}
	}
}

void Security_Plugin::ProcessAlert(Row_Alert *pRow_Alert)
{
	if( !pRow_Alert->ResetTime_isNull() )
		return; // It was already reset
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Processing alert %d type %d",pRow_Alert->PK_Alert_get(),pRow_Alert->FK_AlertType_get());
	if( !pRow_Alert->AnnouncementOnly_get() )
	{
		if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Security_CONST )
			EVENT_Security_Breach(pRow_Alert->EK_Device_get());
		else if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Fire_CONST )
			EVENT_Fire_Alarm(pRow_Alert->EK_Device_get());
		else if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Air_Quality_CONST )
			EVENT_Air_Quality(pRow_Alert->EK_Device_get());
		else if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Doorbell_CONST )
			EVENT_Doorbell(pRow_Alert->EK_Device_get());
		else if( pRow_Alert->FK_AlertType_get()==ALERTTYPE_Monitor_mode_CONST )
			EVENT_Monitor_Mode(pRow_Alert->EK_Device_get());
	}

	if( pRow_Alert->Notification_get() )
	{
		PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
		Notification *pNotification = new Notification(this,m_pTelecom_Plugin,m_pRouter,pRow_Alert);
		pthread_t pthread_id; 
		if(pthread_create( &pthread_id, NULL, StartNotification, (void*)pNotification) )
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create Notification thread" );
		else
			m_mapNotification[pthread_id]=pNotification;
	}
}

void Security_Plugin::ProcessCountdown(int id,Row_Alert *pRow_Alert)
{
	if( DATA_Get_PK_Device().size()==0 || !m_PK_Device_TextToSpeach )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Cannot make security announcements.  Need to have a Text_To_Speach device %d and media devices %s",m_PK_Device_TextToSpeach,DATA_Get_PK_Device().c_str());
		return; // Nothing to do
	}

	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	int iRemainingSeconds;
	if( pRow_Alert )
		iRemainingSeconds = StringUtils::SQLDateTime(pRow_Alert->ExpirationTime_get())-time(NULL);
	else
		iRemainingSeconds = m_tExitTime-time(NULL);

	if( iRemainingSeconds<1 )
		return;  // It already expired

	bool bRapidCountdown = iRemainingSeconds<RAPID_COUNTDOWN;
	string sAnnouncement;
	if( (!pRow_Alert && m_bExitDelay_New) || (pRow_Alert && m_mapAlarm_New[pRow_Alert->PK_Alert_get()]) )
	{
		if( pRow_Alert )
			sAnnouncement = StringUtils::Replace(m_sCountdownBeforeAlarm,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
		else
			sAnnouncement = StringUtils::Replace(m_sCountdownBeforeArmed,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
	}
	else if( bRapidCountdown )
		sAnnouncement = StringUtils::itos(iRemainingSeconds);
	else
	{
		if( pRow_Alert )
			sAnnouncement = StringUtils::Replace(m_sShortCountdownBeforeAlarm,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
		else
			sAnnouncement = StringUtils::Replace(m_sShortCountdownBeforeArmed,"<%=SEC%>",StringUtils::itos(iRemainingSeconds) );
	}

	SayToDevices(sAnnouncement,NULL);

	if(	(!pRow_Alert && m_bExitDelay_New) || (pRow_Alert && m_mapAlarm_New[pRow_Alert->PK_Alert_get()]) )
	{
		if( !pRow_Alert )
			m_bExitDelay_New=false;
		else
			m_mapAlarm_New[pRow_Alert->PK_Alert_get()]=false;
		m_pAlarmManager->AddRelativeAlarm(COUNTDOWN_INTERVAL_1,this,id,pRow_Alert);
	}
	else if( !bRapidCountdown )
		m_pAlarmManager->AddRelativeAlarm(COUNTDOWN_INTERVAL_2,this,id,pRow_Alert);
	else	
		m_pAlarmManager->AddRelativeAlarm(COUNTDOWN_INTERVAL_3,this,id,pRow_Alert);
}

void Security_Plugin::SayToDevices(string sText,DeviceData_Router *pDeviceData_Router)
{
	string sPhones = "";
	bool bBypass_Event = 1; // We don't want to trigger the normal "watching/listening to media" events when doing countdowns or notification
	bool bDont_Setup_AV = 0;
	string sVoice = ""; //TODO: Once we get some decent voices, set this to one of them!

	DCE::CMD_Send_Audio_To_Device CMD_Send_Audio_To_Device(m_dwPK_Device,m_PK_Device_TextToSpeach,sText,sPhones,DATA_Get_PK_Device(),bBypass_Event, bDont_Setup_AV, sVoice);
	SendCommand(CMD_Send_Audio_To_Device);
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

bool Security_Plugin::OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	bool bRegistered = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST]=="1";
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Security_Plugin::OrbiterRegistered orbiter %d registered %d",
		pMessage->m_dwPK_Device_From,(int) bRegistered);
#endif
	if( bRegistered )
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
		if( pOH_Orbiter )
		{
			SetHouseModeBoundIcon(-1,pOH_Orbiter);
			SetMonitorModeBoundIcon(pOH_Orbiter);
		}
	}
	return false;
}
//<-dceag-createinst-b->!

void Security_Plugin::AnnounceAlert(DeviceData_Router *pDevice)
{
	SayToDevices(pDevice->m_sDescription + (pDevice->m_pRoom ? " in " + pDevice->m_pRoom->m_sDescription : ""),NULL);
}

void Security_Plugin::SnapPhoto(Row_Alert_Device *pRow_Alert_Device,DeviceData_Router *pDevice)
{
	for(map<int,DeviceRelation *>::iterator it=pDevice->m_mapDeviceRelation.begin();it!=pDevice->m_mapDeviceRelation.end();++it)
	{
		DeviceRelation *pDeviceRelation = (*it).second;
		DeviceData_Router *pDevice_Camera = pDeviceRelation->m_pDevice;
		if( 
			!pDevice_Camera->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) &&
			!pDevice_Camera->WithinCategory(DEVICECATEGORY_IP_Cameras_CONST) 
		)
			continue;

		char *pData=NULL;
		int iData_Size=0;
		string sFormat="jpg";
		DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame(m_dwPK_Device,pDevice_Camera->m_dwPK_Device,"",0,0,0,&pData,&iData_Size,&sFormat);
		if( SendCommand(CMD_Get_Video_Frame) && iData_Size )
		{
			if(sFormat.empty())
				sFormat = "jpg";

			string sFile = DATA_Get_Path() + "/alert_" + StringUtils::itos(pRow_Alert_Device->PK_Alert_Device_get()) + "_cam" + StringUtils::itos(pDevice_Camera->m_dwPK_Device) + "." + sFormat;
			FILE *pFile = fopen( sFile.c_str(),"wb");
			if( pFile )
			{
				fwrite(pData,iData_Size,1,pFile);
				fclose(pFile);

				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Security plugin write photo to %s",sFile.c_str());
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to write photo to %s",sFile.c_str());
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to get pic from %d %s",
				pDevice_Camera->m_dwPK_Device,pDevice_Camera->m_sDescription.c_str());
	}
}

Row_Alert *Security_Plugin::LogAlert(Row_AlertType *pRow_AlertType,DeviceData_Router *pDevice,bool bAnnouncementOnly,bool bNotify)
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	Row_Alert *pRow_Alert=NULL;
	bool bNewAlert=false;
	// Let's see if there are any pending alerts to pool this with
	for(size_t s=0;s<m_vectPendingAlerts.size();++s)
	{
		Row_Alert *p = m_vectPendingAlerts[s];
		if( p->FK_AlertType_get()==pRow_AlertType->PK_AlertType_get() )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found a matching alert PK: %d detect: %d pool: %d  now: %d",
p->PK_Alert_get(),(int) StringUtils::SQLDateTime(p->DetectionTime_get()),
pRow_AlertType->PoolAlerts_get(),(int) time(NULL));
			// It's the same type of alert.  See if we should be pooling it
			if( StringUtils::SQLDateTime(p->DetectionTime_get()) + pRow_AlertType->PoolAlerts_get() > time(NULL) )
			{
				pRow_Alert=p;
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Pooling alert PK: %d",pRow_Alert->PK_Alert_get());
				break;
			}
		}
	}
		
	if( !pRow_Alert )
	{
		bNewAlert=true;
		pRow_Alert = m_pDatabase_pluto_security->Alert_get()->AddRow();
		pRow_Alert->FK_AlertType_set(pRow_AlertType->PK_AlertType_get());
		pRow_Alert->Notification_set(bNotify);
		pRow_Alert->AnnouncementOnly_set(bAnnouncementOnly);
		pRow_Alert->EK_Device_set(pDevice->m_dwPK_Device);
		pRow_Alert->DetectionTime_set(StringUtils::SQLDateTime(time(NULL)));
		pRow_Alert->ExpirationTime_set(StringUtils::SQLDateTime(time(NULL) + (pRow_AlertType ? pRow_AlertType->ExitDelay_get() : 0) ));
		pRow_Alert->Table_Alert_get()->Commit();
		m_vectPendingAlerts.push_back(pRow_Alert);
		m_mapAlarm_New[pRow_Alert->PK_Alert_get()]=true;
LoggerWrapper::GetInstance()->Write(LV_STATUS,"new alert PK: %d",pRow_Alert->PK_Alert_get());
	}

	Row_Alert_Device *pRow_Alert_Device = m_pDatabase_pluto_security->Alert_Device_get()->AddRow();
	pRow_Alert_Device->FK_Alert_set(pRow_Alert->PK_Alert_get());
	pRow_Alert_Device->EK_Device_set(pDevice->m_dwPK_Device);
	pRow_Alert_Device->DetectionTime_set(StringUtils::SQLDateTime(time(NULL)));
	pRow_Alert_Device->Table_Alert_Device_get()->Commit();

	SnapPhoto(pRow_Alert_Device,pDevice);

	return bNewAlert ? pRow_Alert : NULL;
}


void Security_Plugin::SetMonitorModeBoundIcon(OH_Orbiter *pOH_Orbiter_Compare)
{
	for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = (*it).second;
		if( pOH_Orbiter_Compare==NULL || pOH_Orbiter==pOH_Orbiter_Compare )
		{
			DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,StringUtils::itos((int) m_bMonitorMode),"","monitormode");
			SendCommand(CMD_Set_Bound_Icon);
		}
	}
}

void Security_Plugin::SetHouseModeBoundIcon(int PK_DeviceGroup,OH_Orbiter *pOH_Orbiter_Compare)
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Security_Plugin::SetHouseModeBoundIcon group %d orbiter %d",
		PK_DeviceGroup,(pOH_Orbiter_Compare ? pOH_Orbiter_Compare->m_pDeviceData_Router->m_dwPK_Device : 0));
#endif
	for(map<int,int>::iterator itHM=m_mapPK_HouseMode.begin();itHM!=m_mapPK_HouseMode.end();++itHM)
	{
		if( PK_DeviceGroup==-1 || PK_DeviceGroup==itHM->first )
		{
			int PK_HouseMode=itHM->second;
			for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
			{
				OH_Orbiter *pOH_Orbiter = (*it).second;
				if( pOH_Orbiter_Compare==NULL || pOH_Orbiter==pOH_Orbiter_Compare )
				{
					DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,StringUtils::itos(PK_HouseMode),"","housemode" + StringUtils::itos(itHM->first));
					SendCommand(CMD_Set_Bound_Icon);
					
					DCE::CMD_Set_House_Mode CMD_Set_House_Mode(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,StringUtils::itos(PK_HouseMode),0,"",0,"");
					SendCommand(CMD_Set_House_Mode);
				}
#ifdef DEBUG
				else
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Security_Plugin::SetHouseModeBoundIcon skipping orbiter group %d orbiter %d",
						PK_DeviceGroup,(pOH_Orbiter_Compare ? pOH_Orbiter_Compare->m_pDeviceData_Router->m_dwPK_Device : 0));
#endif
			}
		}
	}
}

void Security_Plugin::GetHouseModes()
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_dwPK_Device,DEVICEDATA_Configuration_CONST);
	if( !pRow_Device_DeviceData )
		return;

	m_mapPK_HouseMode.clear();
	string sData=pRow_Device_DeviceData->IK_DeviceData_get();
	string::size_type pos=0;
	while( pos<sData.size() && pos!=string::npos )
	{
		int PK_DeviceGroup = atoi( StringUtils::Tokenize(sData,",",pos).c_str() );
		m_mapPK_HouseMode[PK_DeviceGroup]=atoi( StringUtils::Tokenize(sData,",",pos).c_str() );
	}
}

void Security_Plugin::SaveHouseModes()
{
	PLUTO_SAFETY_LOCK(sm,m_SecurityMutex);
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(m_dwPK_Device,DEVICEDATA_Configuration_CONST);
	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(m_dwPK_Device);
		pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_Configuration_CONST);
	}

	string sData;
	for(map<int,int>::iterator it=m_mapPK_HouseMode.begin();it!=m_mapPK_HouseMode.end();++it)
		sData += StringUtils::itos(it->first) + "," + StringUtils::itos(it->second) + ",";

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Security_Plugin::SaveHouseModes(): SetHouseMode sData: %s", sData.c_str());
	pRow_Device_DeviceData->IK_DeviceData_set(sData);
	pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
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
	// The password can either be the password or teh PIN code, and either plain text or md5.  iPK_Users is optional
	ostringstream sql;
	sql << "SELECT PK_Users FROM Users WHERE PK_Users=" << iPK_Users
		<< " AND (Password='" << sPassword << "' OR Password='" << m_pRouter->md5(sPassword) 
		<< "' OR PINCode='" << sPassword << "' OR PINCode='" << m_pRouter->md5(sPassword) << "')";

	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( ( result_set.r=m_pRouter->db_wrapper_query_result( sql.str( ) ) )==0 || ( row = db_wrapper_fetch_row( result_set.r ) )==NULL )
        *bIsSuccessful=false;
	else
        *bIsSuccessful=true;
}

void Security_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
	switch(iFloorplanObjectType)
	{
	case FLOORPLANOBJECTTYPE_SECURITY_DOOR_CONST:
	case FLOORPLANOBJECTTYPE_SECURITY_WINDOW_CONST:
	case FLOORPLANOBJECTTYPE_SECURITY_MOTION_DETECTOR_CONST:
	case FLOORPLANOBJECTTYPE_SECURITY_SMOKE_CONST:
	case FLOORPLANOBJECTTYPE_SECURITY_AIRQUALITY_CONST:
		PK_DesignObj_Toolbar=0;
		break;
	case FLOORPLANOBJECTTYPE_SECURITY_CAMERA_CONST:
		PK_DesignObj_Toolbar=DESIGNOBJ_grpCameraControls_CONST;  // Camera toolbar (view)
		break;
	case FLOORPLANOBJECTTYPE_SECURITY_INTERCOM_CONST:
		PK_DesignObj_Toolbar=DESIGNOBJ_grpIntercomControls_CONST;  // Intercom - call, open door
		break;
	case FLOORPLANOBJECTTYPE_SECURITY_SIREN_CONST:
	case FLOORPLANOBJECTTYPE_SECURITY_CHIME_CONST:
		PK_DesignObj_Toolbar=DESIGNOBJ_grpSirenControls_CONST;  // Sound
		break;
	};

	OSD=pDeviceData_Router->m_sState_get();
	string::size_type pos=0;
	string Mode = StringUtils::Tokenize(OSD,",",pos);
	int PK_HouseMode = GetModeID(Mode);
	string Bypass = StringUtils::Tokenize(OSD,",",pos);
	bool bTripped = pDeviceData_Router->m_sStatus_get()=="TRIPPED";
	int PK_AlertType = 0;
	if( bTripped )
		PK_AlertType = GetAlertType(PK_HouseMode,pDeviceData_Router);

	if( Bypass=="BYPASS" || Bypass=="PERMBYPASS" )
		iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_BYPASSED_CONST;
	else if( bTripped )
		iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_TRIGGERED_CONST;
	else if( PK_AlertType>0 )
		iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_ARMED_CONST;
	else
		iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_CLOSED_CONST;
}
