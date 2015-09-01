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
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Room.h"
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
	m_pDatabase_pluto_main = NULL;
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
	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		m_bQuit_set(true);
		return false;
	}

	// The first 2 command groups for each room are considered the 'on' and 'off'
	string sWhere = "JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup WHERE FK_Array=" + StringUtils::itos(ARRAY_Lighting_Scenarios_CONST) + " ORDER BY FK_Room,Sort";
	vector<Row_CommandGroup_Room *> vectRow_CommandGroup_Room;
	m_pDatabase_pluto_main->CommandGroup_Room_get()->GetRows(sWhere,&vectRow_CommandGroup_Room);

	int PK_CommandGroup_On,PK_CommandGroup_Off,PK_Room=0;
	for(size_t s=0;s<vectRow_CommandGroup_Room.size();++s)
	{
		Row_CommandGroup_Room *pRow_CommandGroup_Room = vectRow_CommandGroup_Room[s];
		if( !PK_Room || pRow_CommandGroup_Room->FK_Room_get()!=PK_Room )
		{
			// This is a new room
			PK_Room = pRow_CommandGroup_Room->FK_Room_get();
			PK_CommandGroup_On = pRow_CommandGroup_Room->FK_CommandGroup_get();
			PK_CommandGroup_Off = 0;
		}
		else if( !PK_CommandGroup_Off )
		{
			PK_CommandGroup_Off = pRow_CommandGroup_Room->FK_CommandGroup_get();
			m_mapRoom_CommandGroup[PK_Room] = longPair(PK_CommandGroup_On,PK_CommandGroup_Off);
		}
	}
	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(1);      // number of worker threads
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Lighting_Plugin::~Lighting_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
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
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to lighting plugin");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator( 
		new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Lighting_Plugin::LightingScenariosGrid ) )
		, DATAGRID_Lighting_Scenarios_CONST,PK_DeviceTemplate_get() );

//    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::LightingCommand ), 0, 0, 0, DEVICECATEGORY_Lighting_Device_CONST, 0, 0 );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::LightingCommand ), 0, 0, 0, DEVICECATEGORY_Lighting_Device_CONST, MESSAGETYPE_COMMAND, 0 );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::LightingFollowMe ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Follow_Me_Lighting_CONST );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::DeviceState ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_OnOff_CONST );
    RegisterMsgInterceptor(( MessageInterceptorFn )( &Lighting_Plugin::DeviceState ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Brightness_Changed_CONST );
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

class DataGridTable *Lighting_Plugin::LightingScenariosGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	vector<Row_CommandGroup *> vectRowCommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get( )->GetRows( COMMANDGROUP_FK_ARRAY_FIELD + string( "=" ) + StringUtils::itos( ARRAY_Lighting_Scenarios_CONST ) + " AND " 
			+ COMMANDGROUP_FK_INSTALLATION_FIELD + "=" + StringUtils::itos( m_pRouter->iPK_Installation_get( ) ), &vectRowCommandGroup );
	for( size_t s=0;s<vectRowCommandGroup.size( );++s )
	{
		Row_CommandGroup *pRow_CommandGroup = vectRowCommandGroup[s];
		pCell = new DataGridCell( pRow_CommandGroup->Description_get( ), StringUtils::itos( pRow_CommandGroup->PK_CommandGroup_get( ) ) );
		pDataGrid->SetData( 0, ( int ) s, pCell );
	}

	return pDataGrid;
}

bool Lighting_Plugin::DeviceState( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
    // This only runs as a plug-in so we can safely cast it
    class DeviceData_Router *pDevice_RouterFrom = (class DeviceData_Router *) pDeviceFrom;
	string sLevel = "";    

	// If this device is currently turned on because of a related camera, we ignore one event from it
	PLUTO_SAFETY_LOCK(lm,m_LightingMutex);
	map<int, bool>::iterator it=m_mapLightsToRestoreEvents.find(pMessage->m_dwPK_Device_From);
	if( it!=m_mapLightsToRestoreEvents.end() )
	{
		// first event received, remove from map and return to ignore it
		m_mapLightsToRestoreEvents.erase(it);
		return false;
	}
	lm.Release();

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
	} else	if( pMessage->m_dwID == EVENT_Brightness_Changed_CONST ) {
		LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Lighting_Plugin: EVENT_Brightness_Changed_CONST !");
		// Replace the current brightness
		string sLevel = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
		int iLevel = atoi(sLevel.c_str());
		SetLightState( pMessage->m_dwPK_Device_From, true, iLevel, false );
	}
	
	return false;
}

bool Lighting_Plugin::LightingFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	return HandleFollowMe(pMessage);
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
		{
			m_mapLightsToRestore[ pDevice_Light->m_dwPK_Device ] = make_pair<time_t,string> ( time(NULL)+m_iCameraTimeout, sState );
			m_mapLightsToRestoreEvents[ pDevice_Light->m_dwPK_Device ] = true;
		}
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

void Lighting_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
	switch(iFloorplanObjectType)
	{
	case FLOORPLANOBJECTTYPE_LIGHT_CEILING_LIGHT_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_TABLE_LAMP_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_WALL_SCONCE_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_FLOOR_LAMP_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_CHANDALIER_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_PICTURE_LIGHT_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_ACCENT_LIGHT_CONST:
	case FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST: 
	case FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST:
		OSD = pDeviceData_Router->m_sState_get();
		if(OSD.find("OFF") != string::npos)
			iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_LIGHT_CEILING_LIGHT_OFF_CONST;
		else
			iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_LIGHT_CEILING_LIGHT_ON_CONST;

		PK_DesignObj_Toolbar=DESIGNOBJ_grpLightControls_CONST;
		break;
	default:
		OSD = pDeviceData_Router->m_sState_get();
		if(OSD.find("OFF") != string::npos)
			iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_LIGHT_CEILING_LIGHT_OFF_CONST;
		else
			iPK_FloorplanObjectType_Color = FLOORPLANOBJECTTYPE_COLOR_LIGHT_CEILING_LIGHT_ON_CONST;

		PK_DesignObj_Toolbar=DESIGNOBJ_grpLightControlsOnOff_CONST;
		break;
	};
}

//<-dceag-sample-b->!

/**

	@brief COMMANDS TO IMPLEMENT

*/


void Lighting_Plugin::FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
LoggerWrapper::GetInstance()->Write(LV_WARNING,"Lighting entered room, exec %d",m_mapRoom_CommandGroup[iPK_RoomOrEntArea].first);
	ExecCommandGroup(m_mapRoom_CommandGroup[iPK_RoomOrEntArea].first);
}

void Lighting_Plugin::FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
LoggerWrapper::GetInstance()->Write(LV_WARNING,"Lighting left room, exec %d",m_mapRoom_CommandGroup[iPK_RoomOrEntArea_Left].second);
	ExecCommandGroup(m_mapRoom_CommandGroup[iPK_RoomOrEntArea_Left].second);
}
//<-dceag-createinst-b->!
//<-dceag-c184-b->

	/** @brief COMMAND: #184 - Set Level */
	/** Went sent by an orbiter, the plugin will adjust all lights in the Orbiter's room */
		/** @param #76 Level */
			/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

void Lighting_Plugin::CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c184-e->
{
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
	if( !pOH_Orbiter || !m_pListDeviceData_Router_Lights )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device %d sent a set lighting_level but it's not an orbiter or we have no lights %p",
			pMessage->m_dwPK_Device_From,m_pListDeviceData_Router_Lights);
		return;
	}

	for(ListDeviceData_Router::iterator it=m_pListDeviceData_Router_Lights->begin();it!=m_pListDeviceData_Router_Lights->end();++it)
	{
		DeviceData_Router *pDeviceData_Router = *it;
		if( pDeviceData_Router->m_dwPK_Room==pOH_Orbiter->m_dwPK_Room )
		{
			DCE::CMD_Set_Level CMD_Set_Level(m_dwPK_Device,pDeviceData_Router->m_dwPK_Device,sLevel);
			SendCommand(CMD_Set_Level);
		}
	}
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

			if( iLevel==0 )
				pMessage->m_dwID=COMMAND_Generic_Off_CONST;
			else
			{
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

	switch(pDevice->m_dwPK_DeviceTemplate)
	{
		case DEVICETEMPLATE_Brightness_sensor_CONST:
		case DEVICETEMPLATE_Multilevel_Sensor_CONST:
			pDevice->m_sState_set(StringUtils::itos(Level));
			break; 
		default:
			pDevice->m_sState_set( (bIsOn ? "ON" : "OFF") + string("/") + StringUtils::itos(Level));
	}
	
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Level to store in state %i",Level);


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
		map<int, bool >::iterator it2=m_mapLightsToRestoreEvents.find(PK_Device);
		if( it2!=m_mapLightsToRestoreEvents.end() )
		{
			m_mapLightsToRestoreEvents.erase(it2);
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
			map<int, bool>::iterator it2=m_mapLightsToRestoreEvents.find(it->first);
			if( it2!=m_mapLightsToRestoreEvents.end() )
			{
				m_mapLightsToRestoreEvents.erase(it2);
			}
			m_mapLightsToRestore.erase(it++);
		}
		else
			++it;
	}

	lm.Release();
	SetLightingAlarm();
}
