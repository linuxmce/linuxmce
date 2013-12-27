/*
 Orbiter_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

//<-dceag-d-b->
#include "Orbiter_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#define EXTERN_REVISION_DATE
#include "version.h"

#include <sstream>

#include "PlutoUtils/ProcessUtils.h"
#include "Orbiter/Floorplan.h"
#include "Orbiter/OrbiterFileBrowser.h"

#include "pluto_security/Database_pluto_security.h"
#include "pluto_security/Table_AlertType.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceCategory_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_Size.h"
#include "pluto_main/Define_UI.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Define_PnpProtocol.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_FloorplanObjectType.h"
#include "pluto_main/Table_FloorplanObjectType_Color.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Manufacturer.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Size.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_media/Define_AttributeType.h"
#include "DCERouter.h"
#include "CreateDevice/CreateDevice.h"
#include "BD/PhoneDevice.h"
#include "Gen_Devices/AllScreens.h"
#include "pluto_main/Define_Screen.h"
#include "SerializeClass/ShapesColors.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "DataGrid.h"
#include "PopulateListsInVMC.h"

#include <cctype>
#include <algorithm>

string g_sLatestMobilePhoneVersion="2006.11.09";

#include "../Media_Plugin/EntertainArea.h"
#include "../Media_Plugin/Media_Plugin.h"
#include "../General_Info_Plugin/General_Info_Plugin.h"
#include "../Plug_And_Play_Plugin/Plug_And_Play_Plugin.h"

#define EXPIRATION_INTERVAL 30

// Alarm Types
#define PROCESS_SCREEN_SAVER_PHOTOS		1


//<-dceag-const-b->!
Orbiter_Plugin::Orbiter_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Orbiter_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter), m_UnknownDevicesMutex("Unknown devices varibles"),
    m_AllowedConnectionsMutex("allow connections")
//<-dceag-const-e->
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_UnknownDevicesMutex.Init(&m_MutexAttr);
    m_AllowedConnectionsMutex.Init(NULL);
	m_pDatabase_pluto_main=NULL;
	m_pDatabase_pluto_media=NULL;
	m_pDatabase_pluto_security=NULL;
	m_pRegenMonitor=NULL;
	m_bFloorPlansArePrepared = false;
	m_pAlarmManager=NULL;
	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;
}

//<-dceag-getconfig-b->
bool Orbiter_Plugin::GetConfig()
{
	if( !Orbiter_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
    if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
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

    m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_media->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), "pluto_media", m_pRouter->iDBPort_get( ) ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit_set(true);
        return false;
    }

	m_pRegenMonitor=new RegenMonitor(m_pDatabase_pluto_main,m_pDatabase_pluto_media);
	m_sRegenAllDevicesRooms = m_pRegenMonitor->AllDevicesRooms();
//#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::GetConfig Starting up with m_sRegenAllDevicesRooms=%s",m_sRegenAllDevicesRooms.c_str());
//#endif

	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
		m_mapOH_User[ vectRow_Users[s]->PK_Users_get() ] = new OH_User(vectRow_Users[s]->PK_Users_get());

    // Check for all orbiters
    for(map<int,class DeviceData_Router *>::const_iterator it=m_pRouter->m_mapDeviceData_Router_get()->begin();it!=m_pRouter->m_mapDeviceData_Router_get()->end();++it)
    {
        DeviceData_Router *pDeviceData_Router=(*it).second;
        if( pDeviceData_Router->WithinCategory(DEVICECATEGORY_Orbiter_CONST) )
        {
            OH_Orbiter *pOH_Orbiter = new OH_Orbiter(pDeviceData_Router);
			Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(pDeviceData_Router->m_dwPK_Device);
			if( pRow_Orbiter && pRow_Orbiter->FirstRegen_get() )
			{
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter: %d FirstRegen", pDeviceData_Router->m_dwPK_Device);
				pOH_Orbiter->m_bFirstRegen=true;
			}
            m_mapOH_Orbiter[pDeviceData_Router->m_dwPK_Device] = pOH_Orbiter;
            m_sPK_Device_AllOrbiters += StringUtils::itos(pDeviceData_Router->m_dwPK_Device) + ",";
            if( pDeviceData_Router->m_sMacAddress.size()==0 )
            {
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter: %d %s doesn't have a mac address.",
                    pDeviceData_Router->m_dwPK_Device,pDeviceData_Router->m_sDescription.c_str());
            }
            else
                m_mapOH_Orbiter_Mac[StringUtils::ToUpper(pDeviceData_Router->m_sMacAddress)] = pOH_Orbiter;

			// In special cases where the media director is something other than a standard pluto media director
			// and the on-screen orbiter is a proxy running on another device, we'll allow the media director
			// to be specified in DEVICEDATA_PK_Device_CONST, and treat it like a normal m/d
			string sPK_Device = pOH_Orbiter->m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PK_Device_CONST);
			DeviceData_Router *pDevice_MD = NULL;
			if( sPK_Device.empty()==false )
			{
				pDevice_MD = m_pRouter->m_mapDeviceData_Router_Find( atoi(sPK_Device.c_str()) );
				if( pDevice_MD->m_pDevice_MD )
					pDevice_MD = (DeviceData_Router *) pDevice_MD->m_pDevice_MD;
			}
			if( pDevice_MD==NULL &&
				pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia &&
                pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia->m_dwPK_DeviceCategory==DEVICECATEGORY_Media_Director_CONST )
					pDevice_MD = (DeviceData_Router *) pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia;

			if( pDevice_MD )
            {
				pOH_Orbiter->m_pDeviceData_Router->m_pDevice_MD=pDevice_MD;
                RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::OSD_OnOff) ,0,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,0,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST);
                RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::OSD_OnOff) ,0,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,0,0,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST);

                RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::OSD_OnOff) ,0,pDevice_MD->m_dwPK_Device,0,0,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST);
                RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::OSD_OnOff) ,0,pDevice_MD->m_dwPK_Device,0,0,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST);
                m_mapMD_2_Orbiter[pDevice_MD->m_dwPK_Device] = pOH_Orbiter;
            }
        }
    }

	m_sPK_Device_AllOrbiters_AllowingPopups = m_sPK_Device_AllOrbiters;
	m_iThreshHold = DATA_Get_ThreshHold();
	m_bIgnoreAllBluetoothDevices = DATA_Get_Ignore();

	string sStatus = GetStatus();
	if( sStatus.size() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Starting to regen CMD_Regen from GetStatus %s",sStatus.c_str());
		if( sStatus=="*" )
			CMD_Regen_Orbiter(0,"","");
		else
		{
			string::size_type pos=0;
			while(pos<sStatus.size())
			{
				int PK_Device=atoi(StringUtils::Tokenize(sStatus,",",pos).c_str());
				if( PK_Device )
					CMD_Regen_Orbiter(PK_Device,"","");
			}
		}
		SetStatus("");
	}

	string sMapping = DATA_Get_Mapping();
	if( sMapping.size() )
	{
		vector<string> vectMapping;
		StringUtils::Tokenize(sMapping,"\r\n",vectMapping);
		for(vector<string>::iterator it=vectMapping.begin();it!=vectMapping.end();++it)
		{
			string::size_type pos = it->find('=');
			if( pos!=string::npos )
			{
				int PK_User = atoi(it->c_str());
				string sID = it->substr( pos+1 );
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin user %d has id %s",PK_User,sID.c_str());
				m_mapUsersID[ sID ] = PK_User;
			}
		}
	}

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Orbiter_Plugin::PresenceDetected ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Presence_Detected_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Orbiter_Plugin::PresenceLost ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Presence_Lost_CONST );

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      //4 = number of worker threads
	m_pAlarmManager->AddRelativeAlarm(60,this,PROCESS_SCREEN_SAVER_PHOTOS,NULL);

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Orbiter_Plugin::~Orbiter_Plugin()
//<-dceag-dest-e->
{
    m_mapUnknownDevices.clear();
	delete m_pDatabase_pluto_main;
	delete m_pDatabase_pluto_media;
    delete m_pDatabase_pluto_security;
	delete m_pRegenMonitor;

	for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
		delete (*it).second;

	for(map<int,OH_User *>::iterator it=m_mapOH_User.begin();it!=m_mapOH_User.end();++it)
		delete (*it).second;

    PLUTO_SAFETY_LOCK(ac, m_AllowedConnectionsMutex);
    for(map<string, class AllowedConnections *>::iterator it=m_mapAllowedConnections.begin();it!=m_mapAllowedConnections.end();++it)
        delete (*it).second;
    ac.Release();
}

void Orbiter_Plugin::PrepareToDelete()
{
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}

/* Kind of a hack -- The goal was to allow the lighting, telecom, media, climate and security plug-ins to be
replaced with another model.  This plug-in needs to call them for handling floorplans and follow-me.
So rather than including including pointers directly to the plug-ins, I made generic 'follow-me plugin' and 'floorplan plugin'
classes which our lcmts plug-ins are derived from.  However I can't just cast the pointer to those plug-in which I
receive as pointers to Command_Impl, because they are not the first base class (ie multiple derived).  So, here the
lcmts plug-ins are included, the command_impl is cast to that class, then it's recast to a follow-me or floorplan plugin.
Don't know the best solution to allow for this type of abstraction. */

#include "../Lighting_Plugin/Lighting_Plugin.h"
#include "../Climate_Plugin/Climate_Plugin.h"
#include "../Media_Plugin/Media_Plugin.h"
#include "../Telecom_Plugin/Telecom_Plugin.h"
#include "../Security_Plugin/Security_Plugin.h"

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Orbiter_Plugin::Register()
//<-dceag-reg-e->
{
	Lighting_Plugin *pLighting_Plugin=( Lighting_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Lighting_Plugin_CONST);
	m_pLighting_Floorplan=( FloorplanInfoProvider * ) pLighting_Plugin;

	Climate_Plugin *pClimate_Plugin=( Climate_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Climate_Plugin_CONST);
	m_pClimate_Floorplan=( FloorplanInfoProvider * ) pClimate_Plugin;

	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pMedia_Floorplan=( FloorplanInfoProvider * ) m_pMedia_Plugin;

	Security_Plugin *pSecurity_Plugin=( Security_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Security_Plugin_CONST);
	m_pSecurity_Floorplan=( FloorplanInfoProvider * ) pSecurity_Plugin;

	Telecom_Plugin *pTelecom_Plugin=( Telecom_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Telecom_Plugin_CONST);
	m_pTelecom_Floorplan=( FloorplanInfoProvider * ) pTelecom_Plugin;

	m_pGeneral_Info_Plugin=( General_Info_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	m_pPlug_And_Play_Plugin=( Plug_And_Play_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Plug_And_Play_Plugin_CONST);
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);

	if( !m_pDatagrid_Plugin || !m_pLighting_Floorplan || !m_pClimate_Floorplan || !m_pMedia_Floorplan || !m_pSecurity_Floorplan || !m_pTelecom_Floorplan || !m_pGeneral_Info_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to orbiter plugin");
		return false;
	}

	// Check for remote controls
    for(map<int,class DeviceData_Router *>::const_iterator it=m_pRouter->m_mapDeviceData_Router_get()->begin();it!=m_pRouter->m_mapDeviceData_Router_get()->end();++it)
    {
        DeviceData_Router *pDeviceData_Router=(*it).second;
        if( pDeviceData_Router->WithinCategory(DEVICECATEGORY_Remote_Controls_CONST) || pDeviceData_Router->WithinCategory(DEVICECATEGORY_Infrared_Receivers_CONST) )
        {
			vector<DeviceData_Router *> vectDeviceData_Router;
			// The remote should have a sibbling that is an on screen orbiter
			if( pDeviceData_Router->m_pDevice_ControlledVia )
				((DeviceData_Router *) pDeviceData_Router->m_pDevice_ControlledVia)->FindChildrenWithinCategory(DEVICECATEGORY_Standard_Orbiter_CONST,vectDeviceData_Router);

			OH_Orbiter *pOH_Orbiter=NULL;
			if( vectDeviceData_Router.size() )
				pOH_Orbiter = m_mapOH_Orbiter_Find(vectDeviceData_Router[0]->m_dwPK_Device);

			if( !pOH_Orbiter )
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Remote Control %d has no orbiter",pDeviceData_Router->m_dwPK_Device);
			else
				m_mapRemote_2_Orbiter[pDeviceData_Router->m_dwPK_Device] = pOH_Orbiter;
        }
    }

    GenerateVMCFiles();
    GeneratePlutoMOConfig();

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&Orbiter_Plugin::FloorplanDevices)), 
		DATAGRID_Floorplan_Devices_CONST,PK_DeviceTemplate_get());

    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterDetected) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_detected_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLinked) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_linked_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLost),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_lost_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::ReloadAborted),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Reload_Aborted_CONST);
	RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,0,0,0,0);
	RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::DeviceConfigured),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Device_Configured_CONST);

    return Connect(PK_DeviceTemplate_get());

}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Orbiter_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Orbiter_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

// Our message interceptor
bool Orbiter_Plugin::RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	if( pMessage->m_dwPK_Device_To==DEVICETEMPLATE_VirtDev_All_Orbiters_CONST )
	{
        pMessage->m_dwPK_Device_To = DEVICEID_LIST;
        pMessage->m_sPK_Device_List_To = m_sPK_Device_AllOrbiters;
		return false;
	}

	if (!pDeviceFrom)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"GotRouteToOrbitersInRoom, but pDeviceFrom is NULL");
        return false;
    }

    // If the sender was an orbiter, get the current room.  Otherwise the permanent room
    OH_Orbiter *pOH_Orbiter_From = m_mapOH_Orbiter_Find(pDeviceFrom->m_dwPK_Device);
    int iPK_Room = pOH_Orbiter_From ? pOH_Orbiter_From->m_dwPK_Room : pDeviceFrom->m_dwPK_Room;

    string sDeviceList="";
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        if( pOH_Orbiter->m_dwPK_Room != iPK_Room || pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device==pDeviceFrom->m_dwPK_Device )
            continue; // Don't send to the sender
        if( (pMessage->m_dwPK_Device_To==DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST && pOH_Orbiter->m_pDeviceData_Router->m_dwPK_DeviceCategory!=DEVICECATEGORY_Mobile_Orbiter_CONST) ||
                (pMessage->m_dwPK_Device_To==DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST && pOH_Orbiter->m_pDeviceData_Router->m_dwPK_DeviceCategory!=DEVICECATEGORY_Standard_Orbiter_CONST) )
            continue;
        if( sDeviceList.length() )
            sDeviceList += ",";
        sDeviceList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
    }
    if( sDeviceList.length() )
    {
        pMessage->m_dwPK_Device_To = DEVICEID_LIST;
        pMessage->m_sPK_Device_List_To = sDeviceList;
    }
    return false;  // Continue to process it
}

bool Orbiter_Plugin::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	if( m_listRegenCommands.size()==0 )
		return false;

	string sOrbiters;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Cannot reboot m_listRegenCommands %d pending %p",(int) m_listRegenCommands.size(),pPendingTaskList);

	for(list<int>::iterator it=m_listRegenCommands.begin();it!=m_listRegenCommands.end();++it)
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::PendingTasks m_listRegenCommands %d",*it);

	if( pPendingTaskList )
	{
		for(list<int>::iterator it=m_listRegenCommands.begin();it!=m_listRegenCommands.end();++it)
		{
			char cPercent = 0;
			int nSecondsLeft = 0;

			string sDescription = "Regen Orbiter " + StringUtils::itos(*it);
			OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(*it);

			int Minutes = pOH_Orbiter ? (int)(time(NULL) - pOH_Orbiter->m_tRegenTime) / 60 : 0;
			int Seconds = pOH_Orbiter ? (int)(time(NULL) - pOH_Orbiter->m_tRegenTime) % 60 : 0;

			Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(*it);
			if( pRow_Orbiter )
			{
				pRow_Orbiter->Reload();

				Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pRow_Orbiter->PK_Orbiter_get());
				Row_Room *pRow_Room = pRow_Device ? pRow_Device->FK_Room_getrow() : NULL;
				string sRoom = pRow_Room ? pRow_Room->Description_get() : "";

				sDescription += 
					" (" + (pRow_Device ? pRow_Device->Description_get() : "") + 
					(sRoom != "" ? " / " + sRoom : "") + ")";

				nSecondsLeft = Minutes * 60 + Seconds;

                if(pRow_Orbiter->RegenPercent_get() == 100)
                {
                    //OrbiterGen didn't start to regen this orbiter
                    //if OrbiterGen finished with this Orbiter, we shouldn't see it as a pending task anymore
                    sDescription += " - Scheduled";
                }
                else
                {
                    sDescription += " - " + pRow_Orbiter->RegenStatus_get();
					cPercent = (char)(pRow_Orbiter->RegenPercent_get() % 100);
                }
			}

			pPendingTaskList->m_listPendingTask.push_back(
				new PendingTask(
					*it,				//dwID
					m_dwPK_Device,		//dwPK_Device_Processing
					m_dwPK_Device,		//dwPK_Device_Abort
					"orbitergen",		//sType
					sDescription,		//sDescription
					cPercent,			//cPercentComplete
					nSecondsLeft,		//dwSecondsLeft
					false				//bCanAbort
				)
			);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::ReportPendingTasks "
				"id %d, device processing %d, device abort %d, "
				"type %s, description '%s', percent %c, seconds left %d, can abort %d",
				*it, m_dwPK_Device, m_dwPK_Device,
				"orbitergen", sDescription.c_str(), cPercent, nSecondsLeft, false
			);
		}
	}

	return true;
}

void Orbiter_Plugin::ProcessUnknownDevice()
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"in process");

    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);

    string sMacAddress;
    UnknownDeviceInfos *pUnknownDeviceInfos = NULL;

    map<string,UnknownDeviceInfos *>::iterator itUnknownDevice;
    for(itUnknownDevice = m_mapUnknownDevices.begin(); itUnknownDevice != m_mapUnknownDevices.end(); ++itUnknownDevice)
    {
		UnknownDeviceInfos *pUnknownDeviceInfos_tmp = (*itUnknownDevice).second;
        if(false == pUnknownDeviceInfos_tmp->m_bProcessed) //unprocessed unknown device
        {
            pUnknownDeviceInfos_tmp->m_bProcessed = true; //mark as processed

            sMacAddress = (*itUnknownDevice).first;
            pUnknownDeviceInfos = pUnknownDeviceInfos_tmp;

            break;
        }
    }

    if(NULL == pUnknownDeviceInfos)
        return; //no new unknown device to process

    string sDeviceCategory;
    int iPK_DeviceTemplate;
    string sManufacturer;

    if(!IdentifyDevice(sMacAddress, sDeviceCategory, iPK_DeviceTemplate, sManufacturer))
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "skipping detection of %s.  it's just a dongle",sMacAddress.c_str());
        DCE::CMD_Ignore_MAC_Address CMD_Ignore_MAC_Address(m_dwPK_Device,pUnknownDeviceInfos->m_iDeviceIDFrom,sMacAddress);
        SendCommand(CMD_Ignore_MAC_Address);
        return;
    }

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Detected unknown bluetooth device %s", sMacAddress.c_str());
    pUnknownDeviceInfos->m_iPK_DeviceTemplate = iPK_DeviceTemplate;

    string Description;
	Description += "  Manufacturer: " + sManufacturer;
	Description += "  Category: " + sDeviceCategory;
	Description += "  Bluetooth ID: " + pUnknownDeviceInfos->m_sID;

	EVENT_Device_Detected(sMacAddress,Description,"",0,"",COMMMETHOD_Bluetooth_CONST,PNPPROTOCOL_Proprietary_CONST,"","","mobile_phone","");
}

bool Orbiter_Plugin::IdentifyDevice(const string& sMacAddress, string &sDeviceCategoryDesc, int &iPK_DeviceTemplate, string &sManufacturerDesc)
{
	if( sMacAddress.empty() )
		return false;
	PhoneDevice pd("", sMacAddress, 0);
    vector<Row_DHCPDevice *> vectRow_DHCPDevice;
    ostringstream sql;
    sql << "Mac_Range_Low<=" << pd.m_iMacAddress << " AND Mac_Range_High>=" << pd.m_iMacAddress;
    m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sql.str(),&vectRow_DHCPDevice);
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Search %s returned %d rows", sql.str().c_str(), (int) vectRow_DHCPDevice.size() );
	Row_DHCPDevice *pRow_DHCPDevice = vectRow_DHCPDevice.size() ? vectRow_DHCPDevice[0] : NULL;

    if(NULL == pRow_DHCPDevice)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unknown type of bluetooth device detected. Is this a phone? Mac: %s", sMacAddress.c_str());

        sDeviceCategoryDesc = "Unknown";
        iPK_DeviceTemplate = 0;
        sManufacturerDesc = "Unknown";
        return true;
    }

    if(pRow_DHCPDevice->FK_DeviceCategory_get() == DEVICECATEGORY_Bluetooth_Dongles_CONST)
        return false;

	Row_DeviceCategory *pRow_DeviceCategory = pRow_DHCPDevice->FK_DeviceCategory_getrow();
	if( pRow_DeviceCategory )
	    sDeviceCategoryDesc = pRow_DeviceCategory->Description_get();
    iPK_DeviceTemplate = pRow_DHCPDevice->FK_DeviceTemplate_get();
	Row_Manufacturer *pRow_Manufacturer = pRow_DHCPDevice->FK_Manufacturer_getrow();
	if( pRow_Manufacturer )
	    sManufacturerDesc = pRow_Manufacturer->Description_get();
    
    return true;
}

bool Orbiter_Plugin::MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	if( m_pPlug_And_Play_Plugin && m_pPlug_And_Play_Plugin->m_bSuspendProcessing_get()==true )
	{
		// Just ignore this.  Another detected event will be fired soon.
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::MobileOrbiterDetected processing is now suspended.");
		return false;
	}

    if (NULL == pDeviceFrom)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Got orbiter detected, but pDeviceFrom is NULL. "
			"We are assuming that it's a bluetooth_dongle and the router wasn't reloaded");
    }

    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    string sID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];
    sMacAddress=StringUtils::ToUpper(sMacAddress);

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

    if(!pOH_Orbiter)
    {
        // We don't know what this is.  Let's see if it's a known phone, or anything else we recognize
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( DEVICE_MACADDRESS_FIELD + string("='") + sMacAddress + "' ", &vectRow_Device );

        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found: %d rows in devices for %s",(int) vectRow_Device.size(),sMacAddress.c_str());

        // If we have any records, then it's something that is already in our database, and it's not a phone, since it's
        // not in the OH_Orbiter map.  Just ignore it.
        if( !vectRow_Device.size() )
        {
            // Let's see if we've identified it before in the Unknown Devices table
            vector<Row_UnknownDevices *> vectRow_UnknownDevices;
            m_pDatabase_pluto_main->UnknownDevices_get()->GetRows( UNKNOWNDEVICES_MACADDRESS_FIELD + string("='") + sMacAddress + "' ", &vectRow_UnknownDevices );

            // If we found it here, then we've seen it before and the user already added it to the unknown table.  If not
            // we need to ask the user if it's a phone that he wants to use on the system.
            if( vectRow_UnknownDevices.size()==0 && m_bIgnoreAllBluetoothDevices==false )
            {
                PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
                if(NULL == m_mapUnknownDevices_Find(sMacAddress))
                    m_mapUnknownDevices[sMacAddress] = new UnknownDeviceInfos(pMessage->m_dwPK_Device_From, sID);  // We need to remember who detected this device
                mm.Release();
                ProcessUnknownDevice();
            }
			else
            {
                //this is a known 'unknown device' :)
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Skipping detection of %s. It is an 'unknown device'",sMacAddress.c_str());
                DCE::CMD_Ignore_MAC_Address CMD_Ignore_MAC_Address(m_dwPK_Device, pMessage->m_dwPK_Device_From, sMacAddress);
                SendCommand(CMD_Ignore_MAC_Address);
            }
				
        }
    }
    else
    {
	    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
        if(
			NULL != pOH_Orbiter->m_pDevice_CurrentDetected && 
			pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device == pMessage->m_dwPK_Device_From
		)
        {
            int SignalStrength = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Signal_Strength_CONST].c_str());
            pOH_Orbiter->m_iLastSignalStrength = SignalStrength;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Redetecting the same device with strength: %d", pOH_Orbiter->m_iLastSignalStrength);
        }
        else
        {
            int iCurrentSignalStrength = 0;
            int iOldSignalStrength = pOH_Orbiter->m_iLastSignalStrength;

            if( pOH_Orbiter->m_pDevice_CurrentDetected )
            {
				// It's possible the other dongle has died, or the m/d is turned off, so we'll need
				// to remove the m_pDevice_CurrentDetected if we fail to communicate with it twice, otherwise
				// the 'link' command will never get sent to this one.
				bool bFailedToCommunicateWithDongle=false;
                DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength1(
                    m_dwPK_Device,
					pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,
                    sMacAddress,
                    &iCurrentSignalStrength);

                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Getting the signal strength ... (1)");
                if( SendCommand(CMD_Get_Signal_Strength1) )
			    {
                    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Mobile Orbiter %s dongle %d reported strength of %d (1)",sMacAddress.c_str(),pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,iCurrentSignalStrength);
					if(iCurrentSignalStrength)
                    {
						pOH_Orbiter->m_iLastSignalStrength = iCurrentSignalStrength;
                        iOldSignalStrength = iCurrentSignalStrength;
                    }
                }
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Dongle %d failed to respond once",pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device);
					bFailedToCommunicateWithDongle=true;
				}

                DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength2(
                    m_dwPK_Device,
                    pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,
                    sMacAddress,
                    &iCurrentSignalStrength);

                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Getting the signal strength ... (2)");
                if( SendCommand(CMD_Get_Signal_Strength2) )
                {
                    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Mobile Orbiter %s dongle %d reported strength of %d (2)",sMacAddress.c_str(),pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,iCurrentSignalStrength);
                    if(iCurrentSignalStrength)
                        pOH_Orbiter->m_iLastSignalStrength = iCurrentSignalStrength;
                }
				else if( bFailedToCommunicateWithDongle )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Dongle %d failed to respond twice",pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device);
					pOH_Orbiter->m_pDevice_CurrentDetected=NULL;
				}
            }

			if( pOH_Orbiter->m_pDevice_CurrentDetected &&
                (iCurrentSignalStrength >= m_iThreshHold || iOldSignalStrength >= m_iThreshHold)
			)
            {
                //the current signal strength or the last signal strength are higher then the thresh hold
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Mobile Orbiter %s already has a strong association with %d (%d/%d/%d)",
                    sMacAddress.c_str(),
                    pOH_Orbiter->m_pDevice_CurrentDetected->m_sDescription.c_str(),
                    iOldSignalStrength, iCurrentSignalStrength, m_iThreshHold);
            }
            else
            {
                //the current signal strength and the last signal strength are lower then the thresh hold
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Mobile Orbiter %s told to link with %d (%d,%d,%d)", sMacAddress.c_str(),
                    pMessage->m_dwPK_Device_From, iOldSignalStrength, iCurrentSignalStrength, m_iThreshHold);

				if( pOH_Orbiter->NeedApp())
					SendAppToPhone( pOH_Orbiter, pMessage->m_dwPK_Device_From, sMacAddress );

                string sVmcFileToSend = "";
                if(pOH_Orbiter->NeedVMC())
                {
                    //we'll reset the status when we'll be sure that the connection is established
                    sVmcFileToSend = pOH_Orbiter->m_sUpdateVMCFile;
                }

				if(NULL != pOH_Orbiter->m_pDevice_CurrentDetected)
				{
                    //allow only 'pMessage->m_dwPK_Device_From' device to connect to PlutoMO in EXPIRATION_INTERVAL sec.
                    //this will prevent other dongle to connect to PlutoMO while it is disconnected and
                    //it is waiting for the 'pMessage->m_dwPK_Device_From' to connect
                    //if that device will not connect to PlutoMO in EXPIRATION_INTERVAL seconds, any other device 
                    //will be allowed to connect to PlutoMO.
                    PLUTO_SAFETY_LOCK(ac, m_AllowedConnectionsMutex);
                    m_mapAllowedConnections[sMacAddress] = new AllowedConnections(time(NULL) + EXPIRATION_INTERVAL, pMessage->m_dwPK_Device_From);
                    ac.Release();

                    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Only %d dongle will be allow to connect to %s phone in %d seconds", pMessage->m_dwPK_Device_From,
                        sMacAddress.c_str(), EXPIRATION_INTERVAL);

                    //this dongle will send a link with mobile orbiter when it has finished disconnecting
					DCE::CMD_Disconnect_From_Mobile_Orbiter cmd_Disconnect_From_Mobile_Orbiter(
						m_dwPK_Device, pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,
						sMacAddress,sVmcFileToSend,pMessage->m_dwPK_Device_From,pOH_Orbiter->m_sConfigFile); 
					SendCommand(cmd_Disconnect_From_Mobile_Orbiter);
				}
				else 
				{
                    if(!ConnectionAllowed(pMessage->m_dwPK_Device_From, sMacAddress))
                        return false;

                     //Only do this if there's no other dongle
					DCE::CMD_Link_with_mobile_orbiter CMD_Link_with_mobile_orbiter(
						m_dwPK_Device, pMessage->m_dwPK_Device_From, 
						sMacAddress, sVmcFileToSend, pOH_Orbiter->m_sConfigFile);
					SendCommand(CMD_Link_with_mobile_orbiter);
				}
            }
        }
    }

    return false;
}

bool Orbiter_Plugin::ConnectionAllowed(int iDevice, string sMacAddress)
{
    PLUTO_SAFETY_LOCK(ac, m_AllowedConnectionsMutex);
    AllowedConnections *pAllowedConnections = m_mapAllowedConnections_Find(sMacAddress);
    if(NULL != pAllowedConnections)
    {
        if(pAllowedConnections->m_iDeviceIDAllowed == iDevice)
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "This device (%d) is allowed to connect to PlutoMO %s", iDevice, sMacAddress.c_str());
            m_mapAllowedConnections[sMacAddress] = NULL;
            delete pAllowedConnections;
        }
        else //other device
            if(pAllowedConnections->m_tExpirationTime < time(NULL)) 
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "The connection interval expired. Device %d is now allowed to connect to PlutoMO %s", iDevice,
                    sMacAddress.c_str());
                m_mapAllowedConnections[sMacAddress] = NULL;
                delete pAllowedConnections;
            }
            else
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device %d is not allowed to connect to PlutoMO %s. Waiting for %d device to link", iDevice,
                    sMacAddress.c_str(), pAllowedConnections->m_iDeviceIDAllowed);
                return false;
            }
    }
    ac.Release();
    return true;
}

bool Orbiter_Plugin::MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

	if (!pDeviceFrom || !pOH_Orbiter)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL or unknown dev %s",pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
		return false;
    }

    if(!ConnectionAllowed(pDeviceFrom->m_dwPK_Device, sMacAddress))
        return false;

    pOH_Orbiter->m_sVersion = pMessage->m_mapParameters[EVENTPARAMETER_Version_CONST];
LoggerWrapper::GetInstance()->Write(LV_STATUS,"mobile orbiter linked: %p with version: %s",pOH_Orbiter,pOH_Orbiter->m_sVersion.c_str());

    Row_Device *pRow_Device = pOH_Orbiter->m_pDeviceData_Router->m_pRow_Device;
	if( pOH_Orbiter->NeedApp() || pOH_Orbiter->m_sVersion != g_sLatestMobilePhoneVersion )
	{
		// It's possible that multiple detectors picked up the phone around the same time and we already
		// are sending a new version to one, while another is detecting the phone.  Give the user 15 minutes
		// to acknowledge and install before we try again
		if( pOH_Orbiter->m_tSendAppTime && time(NULL)-pOH_Orbiter->m_tSendAppTime<900 )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"We already tried to send app to phone %s within the last 15 minutes",sMacAddress.c_str());
		else
			SendAppToPhone(pOH_Orbiter,pDeviceFrom->m_dwPK_Device, sMacAddress);
	}

    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	DeviceData_Router *pDevice_PriorDetected = pOH_Orbiter->m_pDevice_CurrentDetected;

    // Associated with a new media director.  Show the corresponding menu
    pOH_Orbiter->m_pDevice_CurrentDetected = (DeviceData_Router *) pDeviceFrom;

    DCE::CMD_Create_Mobile_Orbiter CMD_Create_Mobile_Orbiter(m_dwPK_Device,pDeviceFrom->m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
		StringUtils::itos(pOH_Orbiter->m_pEntertainArea_Locked ? pOH_Orbiter->m_pEntertainArea_Locked->m_iPK_EntertainArea : 0),sMacAddress,
		pOH_Orbiter->m_dwPK_Room_Locked ? pOH_Orbiter->m_dwPK_Room_Locked : 0);
    SendCommand(CMD_Create_Mobile_Orbiter);

    //it's ok now to reset the status
    if(pOH_Orbiter->NeedVMC())
        pOH_Orbiter->NeedVMC(false);

		// See if there's an ent group involved

    //if( pOH_Orbiter->m_pEntGroupAudioZone_LockedOn )
    //{
    //  pOH_Orbiter->m_pController->m_pEntGroup = pMobileOrbiter->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
    //}

    return false;
}

bool Orbiter_Plugin::ReloadAborted(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	int PK_Device = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
	int PK_Orbiter = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Orbiter_CONST].c_str());

    OH_Orbiter *pOH_Orbiter_From = m_mapOH_Orbiter_Find(PK_Orbiter);

	if( !pOH_Orbiter_From )
		return false;

	DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
	SCREEN_Cannot_Reload_Router SCREEN_Cannot_Reload_Router(m_dwPK_Device, PK_Orbiter, pDeviceData_Router->m_sDescription + "\n" + pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST]);
	SendCommand(SCREEN_Cannot_Reload_Router);

	return false;
}

bool Orbiter_Plugin::MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    if (!pDeviceFrom)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
        return false;
    }

    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    //bool bConnectionFailed = "0" != pMessage->m_mapParameters[EVENTPARAMETER_ConnectionFailed_CONST];

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

    if(!pOH_Orbiter)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
    }
    else
    {
	    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
        if(pOH_Orbiter->m_pDevice_CurrentDetected == pDeviceFrom)
        {
            pOH_Orbiter->m_pDevice_CurrentDetected=NULL;
			pOH_Orbiter->m_iLastSignalStrength = 0;
			mm.Release();

			FireFollowMe("LCTS",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
				0,pOH_Orbiter->m_dwPK_Room);
			FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
				0,pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0);
			pOH_Orbiter->m_dwPK_Room=0;
			pOH_Orbiter->m_pEntertainArea=NULL;

        }

    }

    return false;
}


/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c58-b->

	/** @brief COMMAND: #58 - Set Current User */
	/** Set what user is currently using the orbiter.  The 'From' device is assumed to be the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

void Orbiter_Plugin::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
{
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
        return;
    }

    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	DeviceData_Router *pDevice_CurrentDetected = pOH_Orbiter->m_pDevice_CurrentDetected;
	mm.Release();

	int iPK_Users_Last = pOH_Orbiter->PK_Users_get();
	if( pOH_Orbiter->PK_Users_get() && pOH_Orbiter->PK_Users_get()!=iPK_Users && NULL != pDevice_CurrentDetected)
		FireFollowMe("LCMST",pDevice_CurrentDetected->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),0,0);
    pOH_Orbiter->m_pOH_User = m_mapOH_User_Find(iPK_Users);
	if( pOH_Orbiter->PK_Users_get() && pOH_Orbiter->PK_Users_get()!=iPK_Users_Last && pOH_Orbiter->m_dwPK_Room )
		FireFollowMe("LCST",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_dwPK_Room,0);
	if( pOH_Orbiter->PK_Users_get() && pOH_Orbiter->PK_Users_get()!=iPK_Users_Last && pOH_Orbiter->m_pEntertainArea )
		FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea,0);
}

//<-dceag-c59-b->

	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** Set what entertainment area the orbiter (the 'from' in the message) is in. */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

void Orbiter_Plugin::CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(atoi(sPK_EntertainArea.c_str()));
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
        return;
    }
	EntertainArea *pEntertainArea_Last = pOH_Orbiter->m_pEntertainArea;
    pOH_Orbiter->m_pEntertainArea=pEntertainArea;

// Somehow it happened that an orbiter was set to an ent area, but when it did a play media, it says it wasn't
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter %p %d now in ea %p %d",pOH_Orbiter,
					  pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pEntertainArea,
					  (pEntertainArea ? pEntertainArea->m_iPK_EntertainArea : 0));
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	if( pEntertainArea && pEntertainArea->m_pMediaStream )
		pEntertainArea->m_pMediaStream->SetNowPlaying( pOH_Orbiter, false );
	else
		m_pMedia_Plugin->SetNothingNowPlaying( pOH_Orbiter, false );
	mm.Release();

	FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
		pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0,
		pEntertainArea_Last ? pEntertainArea_Last->m_iPK_EntertainArea : 0);

	DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconl(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
		StringUtils::itos((int)(pEntertainArea && pEntertainArea->m_bMediaIsPrivate)),"","media_private");
	SendCommand(CMD_Set_Bound_Iconl);

}

//<-dceag-sample-b->! no sample

//<-dceag-c77-b->

	/** @brief COMMAND: #77 - Set Current Room */
	/** Set what room the orbiter is in. */
		/** @param #57 PK_Room */
			/** The room */

void Orbiter_Plugin::CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c77-e->
{
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
        return;
    }

	int iPK_Room_Prior = pOH_Orbiter->m_dwPK_Room;
	pOH_Orbiter->m_dwPK_Room=iPK_Room;
	FireFollowMe("LCTS",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
		iPK_Room,iPK_Room_Prior);
}
//<-dceag-c78-b->

	/** @brief COMMAND: #78 - New Orbiter */
	/** Tells orbiter plugin to add a new orbiter, or update the parameters on an existing one. */
		/** @param #2 PK_Device */
			/** If 0 is passed in, the new orbiter device is returned.  Otherwise, update this orbiter. */
		/** @param #14 Type */
			/** CE, Windows, Linux, Symbian60, MsSmartphone */
		/** @param #17 PK_Users */
			/** The primary user of the phone. */
		/** @param #44 PK_DeviceTemplate */
			/** What type of orbiter it is. */
		/** @param #47 Mac address */
			/** The MAC Address of the phone. */
		/** @param #57 PK_Room */
			/** The default room */
		/** @param #60 Width */
			/** Screen Width */
		/** @param #61 Height */
			/** Screen Height */
		/** @param #141 PK_Skin */
			/** The skin, 0=use default */
		/** @param #142 PK_Language */
			/** The language, 0=use default */
		/** @param #143 PK_Size */
			/** The size, 0=use default */

void Orbiter_Plugin::CMD_New_Orbiter(string sType,int iPK_Users,int iPK_DeviceTemplate,string sMac_address,int iPK_Room,int iWidth,int iHeight,int iPK_Skin,int iPK_Language,int iPK_Size,int *iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c78-e->
{
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
    UnknownDeviceInfos *pUnknownDeviceInfos = m_mapUnknownDevices_Find(sMac_address);
    if(pUnknownDeviceInfos && !iPK_DeviceTemplate)
        iPK_DeviceTemplate = pUnknownDeviceInfos->m_iPK_DeviceTemplate;
    mm.Release();

	int PK_UI=0;

	if( !iPK_DeviceTemplate )
	{
		string sDeviceCategory;
		string sManufacturer;
		IdentifyDevice(sMac_address, sDeviceCategory, iPK_DeviceTemplate, sManufacturer);
		if( pUnknownDeviceInfos )
		{
			// We know this is a mobile mobile, since it was detected by bluetooth, but we can't identify the make
			// So we must ask the user, on whatever orbiter he made this selection with
			DCE::SCREEN_WhatModelMobileOrbiter  SCREEN_WhatModelMobileOrbiter(m_dwPK_Device, pMessage->m_dwPK_Device_From, iPK_Users, sMac_address);
			SendCommand(SCREEN_WhatModelMobileOrbiter);

			return;
		}
	}

    if(!iPK_DeviceTemplate)
    {
        if( (iWidth==240 && iHeight==320) || iPK_Size==SIZE_240x320_PDA_CONST )
		{
			if( sType=="CE" )
				iPK_DeviceTemplate=DEVICETEMPLATE_Windows_CE_PDA_Vert_Display_CONST;
			else
				iPK_DeviceTemplate=DEVICETEMPLATE_Windows_XP_PDA_Vert_Device_CONST;
			PK_UI=UI_PDA_4_3_vertical_style_CONST;
			iWidth=240;
			iHeight=320;
			iPK_Size=SIZE_240x320_PDA_CONST;
		}
		else if( (iWidth==320 && iHeight==240) || iPK_Size==SIZE_Horizontal_PDA_CONST )
		{
			if( sType=="CE" )
				iPK_DeviceTemplate=DEVICETEMPLATE_Windows_CE_PCtablet_Horiz_CONST;
			else
				iPK_DeviceTemplate=DEVICETEMPLATE_Windows_XP_PCtablet_Horiz_CONST;
			PK_UI=UI_Normal_Horizontal_3_4_CONST;
			iWidth=320;
			iHeight=240;
			iPK_Size=SIZE_Horizontal_PDA_CONST;
		}
		else if( sType=="Symbian60" )
		{
			iPK_DeviceTemplate=DEVICETEMPLATE_Symbian_S60_v1v2_CONST;
			// iPK_DeviceTemplate=DEVICETEMPLATE_Symbian_Series_60_mobile_CONST;
			PK_UI=UI_Symbian_Series_60_Phone_CONST;
			iWidth=176;
			iHeight=208;
			iPK_Size=SIZE_Symbian_60_scale_was_220_158_CONST;
		}
		else if( sType=="MsSmartphone" )
		{
			iPK_DeviceTemplate=DEVICETEMPLATE_Windows_Mobile_Smartphone_CONST;
			PK_UI=UI_Symbian_Series_60_Phone_CONST;
			iWidth=176;
			iHeight=220;
			iPK_Size=SIZE_176x220_Windows_Mobile_CONST;
		}
		else
		{
			if( sType=="CE" )
				iPK_DeviceTemplate=DEVICETEMPLATE_Windows_CE_PCtablet_Horiz_CONST;
			else
				iPK_DeviceTemplate=DEVICETEMPLATE_Windows_XP_PCtablet_Horiz_CONST;
			PK_UI=UI_Normal_Horizontal_3_4_CONST;
			if( /*(!iWidth || !iHeight) &&*/ iPK_Size )
			{
				Row_Size *pRow_Size = m_pDatabase_pluto_main->Size_get()->GetRow(iPK_Size);
				if( pRow_Size && pRow_Size->Width_get() && pRow_Size->Height_get() )
				{
					iWidth=pRow_Size->Width_get();
					iHeight=pRow_Size->Height_get();
				}
				else
				{
					switch( iPK_Size )
					{
					case SIZE_800x600_CONST:
						iWidth=800;
						iHeight=600;
						break;
					case SIZE_640x480_Standard_TV_CONST:
						iWidth=640;
						iHeight=480;
						break;
					case SIZE_1024x768_CONST:
						iWidth=1024;
						iHeight=768;
						break;
					}
				}
			}
			else if( iWidth && iHeight && !iPK_Size )
			{
				if( iWidth==800 && iHeight==600 )
					iPK_Size=SIZE_800x600_CONST;
				else if( iWidth==640 && iHeight==480 )
					iPK_Size=SIZE_640x480_Standard_TV_CONST;
				else if( iWidth==1024 && iHeight==768 )
					iPK_Size=SIZE_1024x768_CONST;
			}
			else if( !iWidth || !iHeight || !iPK_Size )
			{
				iWidth=640;
				iHeight=480;
				iPK_Size=SIZE_640x480_Standard_TV_CONST;
			}
 		}
    }

    int iFK_Room = 1;

	if(NULL != pUnknownDeviceInfos)
		iFK_Room = DatabaseUtils::GetRoomForDevice(m_pDatabase_pluto_main, pUnknownDeviceInfos->m_iDeviceIDFrom);

	CreateDevice createDevice(m_pRouter->iPK_Installation_get(),m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get());
	int PK_Device = createDevice.DoIt(0,iPK_DeviceTemplate,"","",sMac_address,0,"",0,iPK_Room);
	(*iPK_Device) = PK_Device;

	LoggerWrapper::GetInstance()->Write(
        LV_STATUS,
		"New mobile orbiter, setting: %d to mac: %s user: %d",
        PK_Device,
        sMac_address.c_str(), iPK_Users
    );

	// Set everything
	if( iPK_Users )
		m_pRouter->SetDeviceDataInDB(PK_Device,DEVICEDATA_PK_Users_CONST,StringUtils::itos(iPK_Users),false);
	if( iPK_Skin )
		m_pRouter->SetDeviceDataInDB(PK_Device,DEVICEDATA_PK_Skin_CONST,StringUtils::itos(iPK_Skin),false);
	if( iPK_Language )
		m_pRouter->SetDeviceDataInDB(PK_Device,DEVICEDATA_PK_Language_CONST,StringUtils::itos(iPK_Language),false);
	if( iPK_Size )
		m_pRouter->SetDeviceDataInDB(PK_Device,DEVICEDATA_PK_Size_CONST,StringUtils::itos(iPK_Size),false);

    Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
    pRow_Device->Reload(); 

    if( iPK_Room > 0 )
        pRow_Device->FK_Room_set(iPK_Room);
        
    pRow_Device->Table_Device_get()->Commit();

	mm.Relock();
	if( !IsRegenerating(PK_Device) )
	{
		// construct strings outside of array initializer so they don't destroy after the array is initialized, leaving us with invalid pointers
		string sThingsToRegen = StringUtils::itos(PK_Device);
		string sOrbiter = StringUtils::itos(m_dwPK_Device);
		char * args[] = { "/usr/pluto/bin/RegenOrbiterOnTheFly.sh", (char *)(sThingsToRegen.c_str()), (char *)(sOrbiter.c_str()), NULL };
		ProcessUtils::SpawnDaemon(args[0], args);

		m_listRegenCommands.push_back(PK_Device);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_New_Orbiter Executing regen PK_Device: %d now m_listRegenCommands is: %d",PK_Device,(int) m_listRegenCommands.size());
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Already generating %d",iPK_Device);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_New_Orbiter now pending jobs size is m_listRegenCommands: %d",(int) m_listRegenCommands.size());
	for(list<int>::iterator it = m_listRegenCommands.begin(); it != m_listRegenCommands.end(); ++it)
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Still generating m_listRegenCommands %d",*it);

	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);

	bool bDontSendInstructions = false;

    if(!pUnknownDeviceInfos || !pUnknownDeviceInfos->m_iDeviceIDFrom )
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Got New Mobile Orbiter but can't find device!");
    else if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Mobile_Orbiter_CONST )
    {
        Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
		pRow_Device->Reload(); // Just in case it's been changed
		pRow_Device->State_set(NEED_VMC_TOKEN); //first time the app is connected, the vmc file is sent
		if( iPK_Users )
		{
			Row_Users *pRow_Users = m_pDatabase_pluto_main->Users_get()->GetRow(iPK_Users);
			if( pRow_Users )
			{
				if( pRow_Users->Nickname_get().size() )
					pRow_Device->Description_set( pRow_Users->Nickname_get() + " " + pRow_Device->Description_get() );
				else if( pRow_Users->FirstName_get().size() )
					pRow_Device->Description_set( pRow_Users->FirstName_get() + " " + pRow_Device->Description_get() );
				else if( pRow_Users->UserName_get().size() )
					pRow_Device->Description_set( pRow_Users->UserName_get() + " " + pRow_Device->Description_get() );
			}
		}
		pRow_Device->Table_Device_get()->Commit();
		
        const string csMacToken = "<mac>";
		Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);
        string sPlutoMOInstallCmdLine = pRow_DeviceTemplate->Comments_get();
		sPlutoMOInstallCmdLine = StringUtils::Replace(sPlutoMOInstallCmdLine, csMacToken, sMac_address);

		long dwPK_AppServer = GetAppServerAssociatedWithDevice(pUnknownDeviceInfos->m_iDeviceIDFrom);
		if(dwPK_AppServer > 0)
			CMD_Send_File_To_Phone(sMac_address, sPlutoMOInstallCmdLine, dwPK_AppServer);

		bDontSendInstructions = true;
    }

LoggerWrapper::GetInstance()->Write(LV_STATUS,"setting process flag to false");

	if(!bDontSendInstructions)
	{
		SCREEN_DialogPhoneInstructions SCREEN_DialogPhoneInstructions(m_dwPK_Device, pMessage->m_dwPK_Device_From,
			"<%=T" + StringUtils::itos(TEXT_instructions_CONST) + "%>", pUnknownDeviceInfos ? pUnknownDeviceInfos->m_sID : "N/A");
		SendCommand(SCREEN_DialogPhoneInstructions);
	}

    //ProcessUnknownDevice();
}
//<-dceag-c79-b->

	/** @brief COMMAND: #79 - Add Unknown Device */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that the device is a phone and it should be added */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

void Orbiter_Plugin::CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c79-e->
{
    Row_UnknownDevices *pRow_Device = m_pDatabase_pluto_main->UnknownDevices_get()->AddRow();
    pRow_Device->MacAddress_set(sMac_address);
    pRow_Device->Description_set(sID);
    m_pDatabase_pluto_main->UnknownDevices_get()->Commit();

    LoggerWrapper::GetInstance()->Write(
        LV_STATUS,
        "Added phone to unknown devices table, setting: %d to mac: %s",
        pRow_Device->PK_UnknownDevices_get(),
        pRow_Device->MacAddress_get().c_str()
    );

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_sPK_Device_AllOrbiters, "", SCREEN_NewPhoneDetected_CONST);
    SendCommand(CMD_Remove_Screen_From_History_DL);

    ProcessUnknownDevice();
}


//<-dceag-c183-b->

	/** @brief COMMAND: #183 - Get Floorplan Layout */
	/** Gets the layout of all floorplans for the orbiter. */
		/** @param #5 Value To Assign */
			/** A | delimited list in the format, where {} indicate a repeating value: #pages,{#Types,{#Objects,{DeviceDescription, ObjectDescription, FillX Point, FillY Point, PK_DesignObj, Page, PK_Device, Type}}} */

void Orbiter_Plugin::CMD_Get_Floorplan_Layout(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c183-e->
{
    if ( ! m_bFloorPlansArePrepared )
        PrepareFloorplanInfo();

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find orbiter for floorplan layout: %d",pMessage->m_dwPK_Device_From);
        return;
    }
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
    (*sValue_To_Assign) = StringUtils::itos((int) pOH_Orbiter->m_mapFloorplanObjectVector.size()) + "|";

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Set value of floorplan layout to: %s", (*sValue_To_Assign).c_str());

    map<int,FloorplanObjectVectorMap *>::iterator itFloorplanObjectVectorMap;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Size of the map is: %d", pOH_Orbiter->m_mapFloorplanObjectVector.size() );
    for(itFloorplanObjectVectorMap = pOH_Orbiter->m_mapFloorplanObjectVector.begin();itFloorplanObjectVectorMap!=pOH_Orbiter->m_mapFloorplanObjectVector.end();itFloorplanObjectVectorMap++)
    {
        int Page = (*itFloorplanObjectVectorMap).first;
        FloorplanObjectVectorMap *pfpObjVectorMap = (*itFloorplanObjectVectorMap).second;

        if( !pfpObjVectorMap )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There's a null in m_mapFloorplanObjectVector with Page: %d controller: %d",Page,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
            (*sValue_To_Assign) += StringUtils::itos(Page) + "|0|";
            continue;
        }

        (*sValue_To_Assign) += StringUtils::itos(Page) + "|" + StringUtils::itos((int) pfpObjVectorMap->size()) + "|";

        map<int,FloorplanObjectVector *>::iterator itFloorplanObjectVector;
        for(itFloorplanObjectVector = pfpObjVectorMap->begin();itFloorplanObjectVector!=pfpObjVectorMap->end();itFloorplanObjectVector++)
        {
            int Type = (*itFloorplanObjectVector).first;
            FloorplanObjectVector *pfpObjVector = (*itFloorplanObjectVector).second;

            if( !pfpObjVector )
            {
                // 2004 I don't fully understand why this null gets in there, but it's harmless--it just
                // means there are no floorplan objects of this type on the given page
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"There's a null in m_FloorplanObjectVector with Page: %d Type: %d Controller: %d",Page,Type,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
                (*sValue_To_Assign) += StringUtils::itos(Type) + "|0|";
                continue;
            }

            (*sValue_To_Assign) += StringUtils::itos(Type) + "|" +
                StringUtils::itos((int) pfpObjVector->size()) + "|";

            for(int i=0;i<(int) pfpObjVector->size();++i)
            {
                FloorplanObject *fpObj = (*pfpObjVector)[i];

                (*sValue_To_Assign) += fpObj->ObjectTypeDescription + "|" + StringUtils::itos(fpObj->FillX) + "|" + StringUtils::itos(fpObj->FillY) + "|" +
                    fpObj->ObjectID + "|" + StringUtils::itos(fpObj->Page) + "|" + StringUtils::itos(fpObj->PK_Device) + "|" +
					(fpObj->m_pDeviceData_Router ? fpObj->m_pDeviceData_Router->m_sDescription : fpObj->m_pEntertainArea->m_sDescription) + "|" +
                    StringUtils::itos(fpObj->Type) + "|";
            }
        }
    }
}

//<-dceag-c186-b->

	/** @brief COMMAND: #186 - Get Current Floorplan */
	/** Gets the current Floorplan status (ie what items are on/off, etc.) for the specified Floorplan type. */
		/** @param #5 Value To Assign */
			/** The status of all the devices within the floorplan. */
		/** @param #10 ID */
			/** The page number (ie the floorplan ID based). */
		/** @param #46 PK_FloorplanType */
			/** The type of floorplan (lights, climate, etc.) */

void Orbiter_Plugin::CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c186-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
	if( !pOH_Orbiter )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find controller for floorplan: %d",pMessage->m_dwPK_Device_From);
		return;
	}
	int Page = atoi(sID.c_str());

	FloorplanInfoProvider *pFloorplanInfoProvider=NULL;
	switch( iPK_FloorplanType )
	{
	case FLOORPLANTYPE_Lighting_Zone_CONST:
		pFloorplanInfoProvider=m_pLighting_Floorplan;
		break;
	case FLOORPLANTYPE_Entertainment_Zone_CONST:
		pFloorplanInfoProvider=m_pMedia_Floorplan;
		break;
	case FLOORPLANTYPE_Climate_Zone_CONST:
		pFloorplanInfoProvider=m_pClimate_Floorplan;
		break;
	case FLOORPLANTYPE_Phones_CONST:
		pFloorplanInfoProvider=m_pTelecom_Floorplan;
		break;
	case FLOORPLANTYPE_Security_Zone_CONST:
		pFloorplanInfoProvider=m_pSecurity_Floorplan;
		break;
	};

	if( !pFloorplanInfoProvider )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Dont have a provider for floorplan: %d",pMessage->m_dwPK_Device_From);
		return;
	}

	FloorplanObjectVectorMap *pFloorplanObjectVectorMap = pOH_Orbiter->m_mapFloorplanObjectVector_Find(Page);
	FloorplanObjectVector *fpObjVector=NULL;
	if( pFloorplanObjectVectorMap )
		fpObjVector = (*pFloorplanObjectVectorMap)[iPK_FloorplanType];

LoggerWrapper::GetInstance()->Write(LV_STATUS, "get floorplan for page %d type %d map %p objs %d", Page, iPK_FloorplanType, pFloorplanObjectVectorMap,(fpObjVector ? (int) fpObjVector->size() : 0));

	if( fpObjVector )
	{
		for(int i=0;i<(int) fpObjVector->size();++i)
		{
			string OSD="";

			// Color is the color to fill the icon with, Description is the status which
			// appears at the bottom of the floorplan when the item is selected, OSD is
			// the text will be put into any text object within the icon (like the temperature
			// next to a thermastat, and PK_DesignObj_Toolbar is the toolbar to activate
			// when the object is selected.
			int iPK_FloorplanObjectType_Color=0,iColor=0,PK_DesignObj_Toolbar=0;
			string sDescription;
			Row_FloorplanObjectType_Color *pRow_FloorplanObjectType_Color = NULL;

			FloorplanObject *fpObj = (*fpObjVector)[i];
			DeviceData_Router *pDeviceData_Router = fpObj->m_pDeviceData_Router;

			pFloorplanInfoProvider->GetFloorplanDeviceInfo(pDeviceData_Router,fpObj->m_pEntertainArea,fpObj->Type,iPK_FloorplanObjectType_Color,iColor,sDescription,OSD,PK_DesignObj_Toolbar);
			if( iPK_FloorplanObjectType_Color )
				pRow_FloorplanObjectType_Color = m_pDatabase_pluto_main->FloorplanObjectType_Color_get()->GetRow(iPK_FloorplanObjectType_Color);

			if( !iColor && pRow_FloorplanObjectType_Color )
				iColor = pRow_FloorplanObjectType_Color->Color_get();
			if( sDescription.length()==0 && pRow_FloorplanObjectType_Color )
				sDescription = pRow_FloorplanObjectType_Color->Description_get();

			if(fpObj->PK_Device < 0) //entertain area
			{
				vector<Row_Device *> vectRow_Device;
				m_pDatabase_pluto_main->Device_get()->GetRows(
					"JOIN DeviceTemplate ON Device.FK_DeviceTemplate = DeviceTemplate.PK_DeviceTemplate "
					"WHERE FK_Room = " + StringUtils::ltos(fpObj->m_pEntertainArea->m_pRoom->m_dwPK_Room) + " "
					"AND FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Media_Director_CONST),
					&vectRow_Device);

				if(vectRow_Device.size() > 0)
				{
					Row_Device* pRow_Device = *vectRow_Device.begin();
					pRow_Device->Reload();
					if(pRow_Device->Status_get() != "MD_ON")
					{
						iColor = PlutoColor::Gray().m_Value;
						sDescription = "offline";
					}
				}
			}

			(*sValue_To_Assign) += StringUtils::itos(iColor) + "|" + sDescription + "|" + OSD + (PK_DesignObj_Toolbar ? "|" + StringUtils::itos(PK_DesignObj_Toolbar) : "|") + "|";
		}
	}
	if( (*sValue_To_Assign).length()==0 )
		(*sValue_To_Assign)="0";
}

void Orbiter_Plugin::PrepareFloorplanInfo()
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Preparing floorplan");
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
        PrepareFloorplanInfo(it->second);
    m_bFloorPlansArePrepared = true;
}

void Orbiter_Plugin::PrepareFloorplanInfo(OH_Orbiter *pOH_Orbiter)
{
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);

	for(map<int,FloorplanObjectVectorMap *>::iterator it1=pOH_Orbiter->m_mapFloorplanObjectVector.begin();it1!=pOH_Orbiter->m_mapFloorplanObjectVector.end();++it1)
	{
		FloorplanObjectVectorMap *pFloorplanObjectVectorMap = it1->second;
		for(map<int,FloorplanObjectVector *>::iterator it2=pFloorplanObjectVectorMap->begin();it2!=pFloorplanObjectVectorMap->end();++it2)
		{
			FloorplanObjectVector *pFloorplanObjectVector = it2->second;
			if( !pFloorplanObjectVector )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter_Plugin::PrepareFloorplanInfo pFloorplanObjectVector is NULL %d",it2->first);
				continue;
			}
			for(size_t s=0;s<pFloorplanObjectVector->size();++s)
				delete (*pFloorplanObjectVector)[s];
			delete pFloorplanObjectVector;
		}
		delete pFloorplanObjectVectorMap;
	}
	pOH_Orbiter->m_mapFloorplanObjectVector.clear();
	
	Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
    if( !pRow_Orbiter )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Cannot find Row_Orbiter for: %d",
            pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
        return;
    }

	pRow_Orbiter->Reload();  // Just in case this has since changed
    string s = pRow_Orbiter->FloorplanInfo_get();
    string::size_type pos=0;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "This is a valid orbiter: %d fpinfo = %s", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, s.c_str());
    int NumDevices = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Preparing floorplan %d devices",NumDevices);
    for(int iDevice=0;iDevice<NumDevices;++iDevice)
    {
		// The device can be either a device, or an EntertainArea if this is the media floorplan
        int PK_Device = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "DeviceID: %d", PK_Device);
        DeviceData_Router *pDeviceData_Router = NULL;
		EntertainArea *pEntertainArea = NULL;
		if( PK_Device>0 )
			pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
		else
        {
			pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(PK_Device * -1);
			if( !pEntertainArea )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Device referred by the floorplan %d for orbiter %d does not exist", PK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
				// clean the bogus data
				StringUtils::Tokenize(s, "\t", pos);
				StringUtils::Tokenize(s, "\t", pos);
				StringUtils::Tokenize(s, "\t", pos);
				StringUtils::Tokenize(s, "\t", pos);
				continue;
			}
        }

		int FloorplanObjectType = 0;
		if( pDeviceData_Router )
			FloorplanObjectType = atoi(pDeviceData_Router->mapParameters_Find(DEVICEDATA_PK_FloorplanObjectType_CONST).c_str());
		else if( pEntertainArea )
		{
			Row_EntertainArea *pRow_EntertainArea = m_pDatabase_pluto_main->EntertainArea_get()->GetRow(pEntertainArea->m_iPK_EntertainArea);
			if( pRow_EntertainArea  )
				FloorplanObjectType = pRow_EntertainArea->FK_FloorplanObjectType_get();
		}
        Row_FloorplanObjectType *pRow_FloorplanObjectType = m_pDatabase_pluto_main->FloorplanObjectType_get()->GetRow(FloorplanObjectType);

        int Page = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
        string ObjectID = StringUtils::Tokenize(s, "\t", pos);
        int FillX = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
        int FillY = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got this data for the floorplan %d %d %d, %s", Page, FillX, FillY, ObjectID.c_str());
        if ( pRow_FloorplanObjectType == NULL )
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Invalid Floorplan object type: %d Device %d", FloorplanObjectType,PK_Device);
            return;
        }

        string Description = pRow_FloorplanObjectType->Description_get();
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got description %s", Description.c_str());
        int FloorplanType = pRow_FloorplanObjectType->FK_FloorplanType_get();
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got type %d page %d", FloorplanType,Page);

        FloorplanObjectVectorMap *pFpObjMap = pOH_Orbiter->m_mapFloorplanObjectVector_Find(Page);
        if( !pFpObjMap )
        {
            pFpObjMap = new FloorplanObjectVectorMap();
            pOH_Orbiter->m_mapFloorplanObjectVector[Page] = pFpObjMap;
        }

        FloorplanObjectVector *fpVect = (*pFpObjMap)[FloorplanType];
        if( !fpVect )
        {
            fpVect = new FloorplanObjectVector();
            (*pFpObjMap)[FloorplanType] = fpVect;
        }
        FloorplanObject *fp = new FloorplanObject();
        fp->PK_Device = PK_Device;
        fp->m_pDeviceData_Router = pDeviceData_Router;
		fp->m_pEntertainArea = pEntertainArea;
        fp->Type = FloorplanObjectType;
        fp->Page = Page;
        fp->FillX = FillX;
        fp->FillY = FillY;
        fp->ObjectID = ObjectID;;
        fp->ObjectTypeDescription = Description;
        fpVect->push_back(fp);
    }
}

//<-dceag-c255-b->

	/** @brief COMMAND: #255 - Orbiter Registered */
	/** Indicates the orbiter has registered, or unregistered */
		/** @param #8 On/Off */
			/** 1 means it is registering, 0 means it is closing */
		/** @param #17 PK_Users */
			/** If registering, the current user */
		/** @param #19 Data */
			/** a serialized map containing the media types,file list screens, so the orbiter can display the file lists without asking media plugin each time */
		/** @param #45 PK_EntertainArea */
			/** If registering, the current EA */
		/** @param #57 PK_Room */
			/** If registering, the current room */

void Orbiter_Plugin::CMD_Orbiter_Registered(string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c255-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );
	if( !pOH_Orbiter )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Received registration from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
		return;
	}
	else if( sOnOff=="1" )
		pOH_Orbiter->m_tRegistered = time(NULL);
	else
		pOH_Orbiter->m_tRegistered = 0;

	CMD_Send_Orbiter_Popups(true,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);

	if( pOH_Orbiter->m_tRegistered )
	{
		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconl(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting ? "1" : "0","","follow_light");

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconm(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Media ? "1" : "0","","follow_media");
		CMD_Set_Bound_Iconl.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Bound_Iconm.m_pMessage);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconc(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate ? "1" : "0","","follow_climate");
		CMD_Set_Bound_Iconl.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Bound_Iconc.m_pMessage);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icons(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Security ? "1" : "0","","follow_security");
		CMD_Set_Bound_Iconl.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Bound_Icons.m_pMessage);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icont(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom ? "1" : "0","","follow_telecom");
		CMD_Set_Bound_Iconl.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Bound_Icont.m_pMessage);

		pOH_Orbiter->m_dwPK_Room=iPK_Room;

		PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
		EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(atoi(sPK_EntertainArea.c_str()));
	    pOH_Orbiter->m_pEntertainArea=pEntertainArea;
		if( pEntertainArea && pEntertainArea->m_pMediaStream )
			pEntertainArea->m_pMediaStream->SetNowPlaying( pOH_Orbiter, false, false, CMD_Set_Bound_Iconl.m_pMessage);
		mm.Release();
		
		OrbiterFileBrowser_Collection *pOrbiterFileBrowser_Collection = m_pMedia_Plugin->CreateOrbiterFileList(pOH_Orbiter);
		ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_MCR_Remote_CONST);

		if(NULL != pListDeviceData_Router)
		{
			for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
			{
				DeviceData_Router *pDeviceData_Router = *it;
				string sSerialNumber = pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_Serial_Number_CONST);
				if( sSerialNumber.empty() )
					continue;

				int iRemoteID = atoi(pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
				pOrbiterFileBrowser_Collection->m_mapRemoteControls[ sSerialNumber ] = make_pair<int,int> (pDeviceData_Router->m_dwPK_Device,iRemoteID);
			}
		}

		vector<Row_Users *> vectRow_Users;
		m_pDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users);
		for(vector<Row_Users *>::iterator itUsers=vectRow_Users.begin();itUsers!=vectRow_Users.end();++itUsers)
			pOrbiterFileBrowser_Collection->m_mapUsernames[ (*itUsers)->PK_Users_get() ] = (*itUsers)->UserName_get();

		pOrbiterFileBrowser_Collection->SerializeWrite();
		*iData_Size = pOrbiterFileBrowser_Collection->m_dwAllocatedSize;
		*pData = pOrbiterFileBrowser_Collection->m_pcDataBlock;

		SendCommand(CMD_Set_Bound_Iconl);

		delete pOrbiterFileBrowser_Collection;

		// It's an OSD -- see if there are any unconfigured devices
		if( pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia && pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Computers_CONST) )
			CheckForNewWizardDevices( (DeviceData_Router *) pOH_Orbiter->m_pDeviceData_Router->m_pDevice_ControlledVia);

		string sRegenAllDevicesRooms = m_pRegenMonitor->AllDevicesRooms();
//#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Orbiter_Registered %d sRegenAllDevicesRooms \n%s\n%s\n",pMessage->m_dwPK_Device_From,m_sRegenAllDevicesRooms.c_str(),sRegenAllDevicesRooms.c_str());
//#endif

		// Don't do this on the first start
		if( m_sRegenAllDevicesRooms!=sRegenAllDevicesRooms && pOH_Orbiter->m_bFirstRegen==false )
		{
//#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Orbiter_Registered sRegenAllDevicesRooms %d not equal %s=%s",pMessage->m_dwPK_Device_From,m_sRegenAllDevicesRooms.c_str(),sRegenAllDevicesRooms.c_str());
//#endif
			DCE::SCREEN_Need_Reload_Router SCREEN_Need_Reload_Router(m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			SendCommand(SCREEN_Need_Reload_Router);
		}
		else
		{
			string sScenariosFloorplans = m_pRegenMonitor->AllScenariosFloorplans();
			Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			if( pRow_Orbiter )
			{
				pRow_Orbiter->Reload();
//#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Orbiter_Registered %d sScenariosFloorplans %s=%s",pMessage->m_dwPK_Device_From,pRow_Orbiter->ScenariosFloorplans_get().c_str(),sScenariosFloorplans.c_str());
//#endif
				if( pRow_Orbiter->ScenariosFloorplans_get()!=sScenariosFloorplans )
				{
//#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Orbiter_Registered not equal sScenariosFloorplans \n%s\n%s\n",pRow_Orbiter->ScenariosFloorplans_get().c_str(),sScenariosFloorplans.c_str());
//#endif
					DCE::SCREEN_Need_Regen_Orbiter SCREEN_Need_Regen_Orbiter(m_dwPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
					SendCommand(SCREEN_Need_Regen_Orbiter);
				}
			}
		}
	}
	else
	{
		*pData=NULL;
		*iData_Size=0;
	}
}

//<-dceag-c261-b->

	/** @brief COMMAND: #261 - Set Follow-Me */
	/** Specifies whether the orbiter has follow-me on or off for the given user. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #9 Text */
			/** Can be 'L', 'M', 'C', 'S', 'T' for Lighting, Media, Climate, Security, Telecom followed by 0 or 1 for off/on. */
		/** @param #17 PK_Users */
			/** The User */

void Orbiter_Plugin::CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c261-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find( iPK_Device );
	if( !pOH_Orbiter || sText.length()<2 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid set follow me: %d %s",iPK_Device,sText.c_str());
		return;
	}

	bool bOnOff = sText[1]=='0';

LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter %d set follow me to %s for user %d",iPK_Device,sText.c_str(),iPK_Users);

	switch( sText[0] )
	{
	case 'L':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting : false,"follow_light");
			FireFollowMe("L",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	case 'M':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Media = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Media : false,"follow_media");
			FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff && pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0,0);
		}
		break;
	case 'C':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate : false,"follow_climate");
			FireFollowMe("C",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	case 'T':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom : false,"follow_telecom");
			FireFollowMe("T",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	case 'S':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Security = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Security : false,"follow_security");
			FireFollowMe("S",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	}
}
//<-dceag-c266-b->

	/** @brief COMMAND: #266 - Regen Orbiter */
	/** Regenerates an Orbiter.  When regeneration is complete, the "Regen Orbiter Finished" command will be sent */
		/** @param #2 PK_Device */
			/** The Orbiter to regenerate */
		/** @param #21 Force */
			/** Can be -r to force a full regen, or -a for a quick one */
		/** @param #24 Reset */
			/** 'Y' or '1' means reset the router before doing the regen */

void Orbiter_Plugin::CMD_Regen_Orbiter(int iPK_Device,string sForce,string sReset,string &sCMD_Result,Message *pMessage)
//<-dceag-c266-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter Starting regen orbiter for %d reset: %s with m_listRegenCommands %d size",iPK_Device,sReset.c_str(),(int) m_listRegenCommands.size());
	if( sReset.size() && (sReset[0]=='Y' || sReset[0]=='1') )
	{
		if( !m_pRouter->RequestReload(pMessage->m_dwPK_Device_From) )
		{
			sCMD_Result = "CANNOT RELOAD NOW";
			return;
		}

		SetStatus( iPK_Device ? StringUtils::itos(iPK_Device) : string("*") );
		m_pRouter->Reload();
		return;
	}

	string sOrbiterList;
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	list<OH_Orbiter *> temp_listRegenCommands;

    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter checking orb %d/%d of %d device %d regen %d",
			it->first,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,(int) m_mapOH_Orbiter.size(), iPK_Device,(int) pOH_Orbiter->m_tRegenTime); 

		if( iPK_Device==0 || pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device==iPK_Device )
		{
			if( IsRegenerating(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device) || pOH_Orbiter->m_tRegenTime )
			{
				int Minutes = (int)(time(NULL) - pOH_Orbiter->m_tRegenTime) /60;
				string sMessage = "We already started regenerating the orbiter " + pOH_Orbiter->m_pDeviceData_Router->m_sDescription + " " + StringUtils::itos(Minutes) +
					" minutes ago.  When it is finished, it will return to the main menu automatically.  If you think it is stuck, you may want to reset the LinuxMCE system";
				SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device, pMessage->m_dwPK_Device_From,
					sMessage, // Main message
					"", // Command Line
					"already_regen", // Description
					"0", // sPromptToResetRouter
					"0", // sTimeout
					"1"); // sCannotGoBack
				SendCommand(SCREEN_PopupMessage);
				return;
			}
			else
			{
				if( sOrbiterList.size() )
					sOrbiterList += ",";
				sOrbiterList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
				temp_listRegenCommands.push_back(pOH_Orbiter);
			}
		}
	}

	for(list<OH_Orbiter *>::iterator it=temp_listRegenCommands.begin();it!=temp_listRegenCommands.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = *it;
		pOH_Orbiter->m_tRegenTime = time(NULL);
		m_listRegenCommands.push_back(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter iPK_Device==%d Added %d to m_listRegenCommands %d size",
			iPK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,(int) m_listRegenCommands.size());
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter iPK_Device==%d ready for  m_listRegenCommands %d size",
		iPK_Device, (int) m_listRegenCommands.size());
	if( temp_listRegenCommands.size() )
	{
		// construct strings outside of array initializer so they don't destroy after the array is initialized, leaving us with invalid pointers
		string sThingsToRegen = iPK_Device ? (char *)(StringUtils::itos(iPK_Device).c_str()) : (char *) sOrbiterList.c_str();
		string sOrbiter = StringUtils::itos(m_dwPK_Device).c_str();
		char * args[] = { "/usr/pluto/bin/RegenOrbiterOnTheFly.sh", (char*)(sThingsToRegen.c_str()), (char *)(sOrbiter.c_str()), (char *)(sForce.c_str()), NULL };
		ProcessUtils::SpawnDaemon(args[0], args);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter Execution %s returned for %d",sThingsToRegen.c_str(),iPK_Device);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter temp_listRegenCommands is empty or already generating %d",iPK_Device);
}

//<-dceag-c267-b->

	/** @brief COMMAND: #267 - Regen Orbiter Finished */
	/** Regeneration of the indicated Orbiter has been finished */
		/** @param #2 PK_Device */
			/** The Orbiter */

void Orbiter_Plugin::CMD_Regen_Orbiter_Finished(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c267-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got a CMD_Regen_Orbiter_Finished");
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Regen_Orbiter_Finished Regen finished for: %d m_listRegenCommands size is: %d",iPK_Device,(int) m_listRegenCommands.size());
	for(list<int>::iterator it = m_listRegenCommands.begin(); it != m_listRegenCommands.end(); ++it)
	{
		if(*it == iPK_Device)
		{
			m_listRegenCommands.erase(it);
			break;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"after Regen finished for: %d m_listRegenCommands size is: %d",iPK_Device,(int) m_listRegenCommands.size());
	for(list<int>::iterator it = m_listRegenCommands.begin(); it != m_listRegenCommands.end(); ++it)
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Still generating %d",*it);

	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Device);
	if( pOH_Orbiter )  // It may be a new orbiter
	{
		pOH_Orbiter->m_tRegenTime = 0;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ready to reload the orbiter with id %d", iPK_Device);
		mm.Release();
		PrepareFloorplanInfo(pOH_Orbiter);
	}

	Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow( iPK_Device );  // If we marked this as 'first regen', we're now regening, so don't treat it as first or else we'll supress pnp messages
	if( pRow_Orbiter && pOH_Orbiter )
	{
		pRow_Orbiter->Reload();
		pOH_Orbiter->m_bFirstRegen=(pRow_Orbiter->FirstRegen_get()==1);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter: %d FirstRegen setting to %d/%d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,(int) pRow_Orbiter->FirstRegen_get(),(int) pOH_Orbiter->m_bFirstRegen);
	}

}

//<-dceag-createinst-b->!



bool Orbiter_Plugin::OSD_OnOff( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	bool bDestIsMD=false; // Will be true if the destination was an MD, false if an OSD
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_To);
	if( !pOH_Orbiter )
	{
		pOH_Orbiter = m_mapMD_2_Orbiter_Find(pMessage->m_dwPK_Device_To);
		bDestIsMD=true;
	}

	if( !pOH_Orbiter || !pDeviceTo->m_pDevice_MD )
		return false;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::OSD_OnOff orbiter %d md %d DestIsMd %d pipes %d",
		pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pDeviceTo->m_pDevice_MD->m_dwPK_Device, (int) bDestIsMD,
		(int) ((DeviceData_Router *) pDeviceTo)->m_mapPipe_Available.size());

	// If this is coming from an OSD Orbiter, the Force parameter can be specified and set to 1, meaning even if there
	// is no active pipe, still force an 'off' on all available pipes
	if( bDestIsMD && pDeviceTo && 
		pMessage->m_mapParameters.find(COMMANDPARAMETER_Force_CONST)!=pMessage->m_mapParameters.end() && pMessage->m_mapParameters[COMMANDPARAMETER_Force_CONST]=="1" &&
		((DeviceData_Router *) pDeviceTo)->m_mapPipe_Active.size()==0 && ((DeviceData_Router *) pDeviceTo)->m_mapPipe_Available.size()>0 )
	{
		DeviceData_Router *pDeviceData_Router = (DeviceData_Router *) pDeviceTo;
		int PK_Pipe=0;
        if( pMessage->m_mapParameters.find(COMMANDPARAMETER_PK_Pipe_CONST)!=pMessage->m_mapParameters.end() )
			PK_Pipe = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST].c_str());

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::OSD_OnOff device %d available pipes %d",
			pDeviceData_Router->m_dwPK_Device,(int) pDeviceData_Router->m_mapPipe_Available.size());
	
		for(map<int,Pipe *>::iterator it=pDeviceData_Router->m_mapPipe_Available.begin();it!=pDeviceData_Router->m_mapPipe_Available.end();++it)
        {
            Pipe *pPipe = (*it).second;
		
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::OSD_OnOff pipe %p PK_Pipe %d dontsend %d",
				pPipe, PK_Pipe, (int) pPipe->m_bDontSendOff);

			if( (PK_Pipe && PK_Pipe!=pPipe->m_PK_Pipe) || pPipe->m_bDontSendOff )
				continue;

			Message *pMessage_New = new Message( pMessage->m_dwPK_Device_From, pPipe->m_pDevice_To->m_dwPK_Device,
                PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_Off_CONST,0);
			if( PK_Pipe )
				pMessage_New->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST] = pMessage->m_mapParameters[COMMANDPARAMETER_PK_Pipe_CONST];
	        if( pMessage->m_mapParameters.find(COMMANDPARAMETER_PK_Device_Pipes_CONST)!=pMessage->m_mapParameters.end() )
				pMessage_New->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST] = pMessage->m_mapParameters[COMMANDPARAMETER_PK_Device_Pipes_CONST];
            QueueMessageToRouter(pMessage_New);
        }
	}

	if( pMessage->m_mapParameters.find(COMMANDPARAMETER_Already_processed_CONST)!=pMessage->m_mapParameters.end() )
		return false; // This message originated with us

	pOH_Orbiter->m_bDisplayOn = pMessage->m_dwID==COMMAND_Generic_On_CONST;
	if( pOH_Orbiter->m_pDeviceData_Router->m_mapParameters.find(DEVICEDATA_Leave_Monitor_on_for_OSD_CONST)!=
		pOH_Orbiter->m_pDeviceData_Router->m_mapParameters.end() && 
		pOH_Orbiter->m_pDeviceData_Router->m_mapParameters[DEVICEDATA_Leave_Monitor_on_for_OSD_CONST]=="1" )
	{
		OverrideAVPipe(pOH_Orbiter->m_pDeviceData_Router,pOH_Orbiter->m_bDisplayOn);
	}

	if( bDestIsMD )
	{
		if( pMessage->m_dwID==COMMAND_Generic_Off_CONST )
		{
			DeviceData_Router *pDevice_MD = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_To);
			bool bMD_Uses_External_Video=false;  // See if there's a video path, like going to a TV

			for( map<int,Pipe *>::iterator it=pDevice_MD->m_mapPipe_Available.begin();it!=pDevice_MD->m_mapPipe_Available.end();++it)
			{
				Pipe *pPipe = it->second;
				if( pPipe->m_PK_Pipe==2 || pPipe->m_PK_Pipe==4 )
				{
					bMD_Uses_External_Video=true;
					break;
				}
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::OSD_OnOff off uses external %d size %d", (int) bMD_Uses_External_Video, (int) pDevice_MD->m_mapPipe_Available.size());

			if( bMD_Uses_External_Video )
			{
				// We just turned off the TV that our Orbiter's OSD is using.  So set display off flag to 'off', so when
				// the user touches the screen, it will turn the tv back on.
				DCE::CMD_Display_OnOff CMD_Display_OnOff(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,"0",true);
				SendCommand(CMD_Display_OnOff);
			}
		}
	}
	else
	{
		MediaDevice *pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(pDeviceTo->m_pDevice_MD->m_dwPK_Device);
		if( pMediaDevice && pMediaDevice->m_dwPK_Command_LastPower==pMessage->m_dwID && time(NULL)-pMediaDevice->m_tLastPowerCommand < DONT_RESEND_POWER_WITHIN_X_SECONDS )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::OSD_OnOff Not resending power command");
			return false;
		}
		else if( pMediaDevice )
		{
			pMediaDevice->m_tLastPowerCommand=time(NULL);
			pMediaDevice->m_dwPK_Command_LastPower=pMessage->m_dwID;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::OSD_OnOff off forwarding to non md");

		Message *pMessage_New = new Message(pMessage);
		pMessage_New->m_dwPK_Device_To = pDeviceTo->m_pDevice_MD->m_dwPK_Device;
		pMessage_New->m_mapParameters[COMMANDPARAMETER_Already_processed_CONST] = "1"; // So we know we already processed it and don't create an infinite loop
		QueueMessageToRouter(pMessage_New);
	}

	return false; // Let others handle it too
}

void Orbiter_Plugin::FireFollowMe(string sMask,int iPK_Orbiter,int iPK_Users,int iPK_RoomOrEntArea,int iPK_RoomOrEntArea_Left)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter_Plugin::FireFollowMe mask %s orb %d user %d r/e %d r/e left %d",
					  sMask.c_str(),iPK_Orbiter,iPK_Users,iPK_RoomOrEntArea,iPK_RoomOrEntArea_Left);

	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Orbiter);
	if( !pOH_Orbiter )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot fire follow me for unknown orbiter: %d",iPK_Orbiter);
		return;
	}

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting && sMask.find('L')!=string::npos )
		EVENT_Follow_Me_Lighting(iPK_Orbiter, 0, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate && sMask.find('C')!=string::npos )
		EVENT_Follow_Me_Climate(iPK_Orbiter, 0, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Security && sMask.find('S')!=string::npos )
		EVENT_Follow_Me_Security(iPK_Orbiter, 0, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom && sMask.find('T')!=string::npos )
		EVENT_Follow_Me_Telecom(iPK_Orbiter, 0, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Media && sMask.find('M')!=string::npos )
		EVENT_Follow_Me_Media(iPK_Orbiter, 0, iPK_Users, iPK_RoomOrEntArea, iPK_RoomOrEntArea_Left);
}

void Orbiter_Plugin::SetBoundIcons(int iPK_Users,bool bOnOff,string sType)
{
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        if( pOH_Orbiter->PK_Users_get() != iPK_Users )
			continue; // Don't change

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,bOnOff ? "1" : "0","",sType);
		SendCommand(CMD_Set_Bound_Icon);
	}
}

void Orbiter_Plugin::SendAppToPhone(OH_Orbiter *pOH_Orbiter, int nDeviceID, string sMacAddress)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Phone needs file - NeedApp: %d, NeedVMC: %d,  version: / %s / %s",
		(int) pOH_Orbiter->NeedApp(),(int) pOH_Orbiter->NeedVMC(),
        g_sLatestMobilePhoneVersion.c_str(),pOH_Orbiter->m_sVersion.c_str());

	pOH_Orbiter->m_sVersion = g_sLatestMobilePhoneVersion;
	pOH_Orbiter->NeedApp(false);
	pOH_Orbiter->m_tSendAppTime = time(NULL);

    static const string csMacToken = "<mac>";
    Row_DeviceTemplate *pRow_DeviceTemplate = pOH_Orbiter->m_pDeviceData_Router->m_pRow_Device->FK_DeviceTemplate_getrow();
    string sPlutoMOInstallCmdLine = pRow_DeviceTemplate->Comments_get();
    sPlutoMOInstallCmdLine = StringUtils::Replace(sPlutoMOInstallCmdLine, csMacToken, sMacAddress);

	long dwPK_AppServer = GetAppServerAssociatedWithDevice(nDeviceID);
	if(dwPK_AppServer > 0)
		CMD_Send_File_To_Phone(sMacAddress, sPlutoMOInstallCmdLine, dwPK_AppServer);
}

long Orbiter_Plugin::GetAppServerAssociatedWithDevice(int nDeviceID)
{
	int nDeviceMD_ID = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main, nDeviceID);
	DeviceData_Base *pDevice_MD = this->m_pRouter->m_mapDeviceData_Router_Find(nDeviceMD_ID);

	if(NULL != pDevice_MD)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Searching for a App_Server associated to device %d", pDevice_MD->m_dwPK_Device);
		DeviceData_Base *pDevice_AppServer = ((DeviceData_Impl *)pDevice_MD)->FindSelfOrChildWithinCategory(DEVICECATEGORY_App_Server_CONST);

		if(NULL == pDevice_AppServer)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Searching for a App_Server associated to its parent %d (is this a hybrid?)", pDevice_MD->m_dwPK_Device);

			//this might be a hybrid, which doesn't have an App_Server child anymore
			//let's search for generic pc device and then let's grab its App_Server child
			DeviceData_Base *pGeneric_PC_As_Core = pDevice_MD->FindSelfOrParentWithinCategory(DEVICETEMPLATE_Generic_PC_as_Core_CONST);
			if(NULL != pGeneric_PC_As_Core)
			{
				pDevice_AppServer = ((DeviceData_Impl *)pGeneric_PC_As_Core)->FindSelfOrChildWithinCategory(DEVICECATEGORY_App_Server_CONST);
			}
		}

		if(NULL != pDevice_AppServer)
			return pDevice_AppServer->m_dwPK_Device;
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got the MD %d, but couldn't find the App_Server on it.", pDevice_MD->m_dwPK_Device);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Couldn't find the App_Server for %d's MD/HY", nDeviceID);

	return 0;
}

void Orbiter_Plugin::GenerateVMCFiles()
{
    //create vmc files for each mobile device
    for(map<string, OH_Orbiter *>::iterator iter = m_mapOH_Orbiter_Mac.begin(); iter != m_mapOH_Orbiter_Mac.end(); ++iter)
    {
        OH_Orbiter *pOH_Orbiter = (*iter).second;

        long dwPKDevice = pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device;
        Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(dwPKDevice);
        long dwDeviceTemplate = pRow_Device->FK_DeviceTemplate_get();
        Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(dwDeviceTemplate);
        string sDescription = pRow_DeviceTemplate->Description_get();
        string sSourceVMCFileName = sDescription + ".vmc";

        StringUtils::Replace(&sSourceVMCFileName, "/", "-");
        StringUtils::Replace(&sSourceVMCFileName, ":", "-");
        StringUtils::Replace(&sSourceVMCFileName, "\\", "-");
        StringUtils::Replace(&sSourceVMCFileName, "*", "-");
        StringUtils::Replace(&sSourceVMCFileName, "?", "-");
        sSourceVMCFileName = "/usr/pluto/bin/" + sSourceVMCFileName;

        string sDestFileName = "/usr/pluto/bin/dev_" + StringUtils::ltos(dwPKDevice) + ".vmc";
        string sOldChecksum = FileUtils::FileChecksum(sDestFileName);

        PopulateListsInVMC PopulateListsInVMC_(sSourceVMCFileName, sDestFileName, dwPKDevice, m_pDatabase_pluto_main, m_pRouter->iPK_Installation_get());
        PopulateListsInVMC_.DoIt();

        pOH_Orbiter->m_sUpdateVMCFile = sDestFileName;
        if(sOldChecksum != FileUtils::FileChecksum(sDestFileName))
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to send %s to PlutoMO, the checksum is changed", sDestFileName.c_str());
            pOH_Orbiter->NeedVMC(true);
        }
    }
}

void Orbiter_Plugin::GeneratePlutoMOConfig()
{
    const string csWapConfFile("/etc/wap.conf");
	const string csCallerIdConfFile("/etc/pluto-callerid.conf");
    const string csPlutoMOConf("/usr/pluto/bin/PlutoMO");

	string sCustomCallerID;
	if(FileUtils::FileExists(csCallerIdConfFile))
	{
		FileUtils::ReadTextFile(csCallerIdConfFile, sCustomCallerID);
		sCustomCallerID = StringUtils::Replace(sCustomCallerID, "\n", "");
		sCustomCallerID = StringUtils::TrimSpaces(sCustomCallerID);
	}

    if(FileUtils::FileExists(csWapConfFile))
    {
		string sPlutoMOConfig;
		FileUtils::ReadTextFile(csWapConfFile, sPlutoMOConfig);
		sPlutoMOConfig += "?\n";

        for(map<string, OH_Orbiter *>::iterator iter = m_mapOH_Orbiter_Mac.begin(); iter != m_mapOH_Orbiter_Mac.end(); ++iter)
        {
            OH_Orbiter *pOH_Orbiter = (*iter).second;

            long dwPKDevice = pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device;
            string sPKDevice = StringUtils::ltos(dwPKDevice);
            string sPlutoMOConfFile = csPlutoMOConf + "_" + sPKDevice + ".cfg";

            LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to generate '%s' file", sPlutoMOConfFile.c_str());

	        //generating the list with alert types
            vector<Row_AlertType *> vectRow_AlertType;
            m_pDatabase_pluto_security->AlertType_get()->GetRows("1 = 1", &vectRow_AlertType);
            sPlutoMOConfig += StringUtils::ltos(long(vectRow_AlertType.size())) + "\n";

            size_t i;
            for(i = 0; i < vectRow_AlertType.size(); i++)
            {
                Row_AlertType *pRow_AlertType = vectRow_AlertType[i];
                sPlutoMOConfig += pRow_AlertType->Description_get() + "\n";
            }

            //generate the list with known caller ids
            sPlutoMOConfig += StringUtils::ltos(long(vectRow_AlertType.size())) + "\n";
            for(i = 0; i < vectRow_AlertType.size(); i++)
            {
                Row_AlertType *pRow_AlertType = vectRow_AlertType[i];

                sPlutoMOConfig += StringUtils::ltos(pRow_AlertType->PK_AlertType_get()) + "\n";
                
				string sCallerID;
				if(sCustomCallerID == "")
					sCallerID = GenerateCallerID(pRow_AlertType->PK_AlertType_get());
				else
					sCallerID = sCustomCallerID;

                sPlutoMOConfig += sCallerID + "\n";
                sPlutoMOConfig += pRow_AlertType->Description_get() + "\n";

                //TODO: need a 'Hang up true/false' field in AlertType table
                sPlutoMOConfig += "0\n";
            }
            vectRow_AlertType.clear();

            //user name and pin
            vector<Row_Device_DeviceData *> vectRow_Device_DeviceData;
            m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows(
                " FK_Device = " + sPKDevice + " AND " + 
                " FK_DeviceData = " + StringUtils::ltos(DEVICEDATA_PK_Users_CONST),
                &vectRow_Device_DeviceData);

            if(vectRow_Device_DeviceData.size() != 1)
            {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No user for %s mobile or too many: %d", sPKDevice.c_str(),
                    vectRow_Device_DeviceData.size());
                vectRow_Device_DeviceData.clear();
                continue;
            }

            string sPK_User = vectRow_Device_DeviceData[0]->IK_DeviceData_get();
            vectRow_Device_DeviceData.clear();

            Row_Users *pRow_Users = m_pDatabase_pluto_main->Users_get()->GetRow(atoi(sPK_User.c_str()));

            if(!pRow_Users)
            {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No record found in the database for user %s, rows %d", sPK_User.c_str(),
                    vectRow_Device_DeviceData.size());
                vectRow_Device_DeviceData.clear();
                continue;
            }

            string sUserName = pRow_Users->UserName_get();
            string sPin = pRow_Users->PINCode_get();
            string sPhoneNumber;

            m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows(
                " FK_Device = " + sPKDevice + " AND " + 
                " FK_DeviceData = " + StringUtils::ltos(DEVICEDATA_Mobile_Orbiter_Phone_CONST),
                &vectRow_Device_DeviceData);

            if(vectRow_Device_DeviceData.size() != 1)
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "No phone number for %s mobile or too many: %d", sPKDevice.c_str(),
                    vectRow_Device_DeviceData.size());
                vectRow_Device_DeviceData.clear();
            }
            else
                sPhoneNumber = vectRow_Device_DeviceData[0]->IK_DeviceData_get();

            vectRow_Device_DeviceData.clear();

            sPlutoMOConfig += sUserName + "\n" + sPin + "\n" + sPhoneNumber + "\n";
            pOH_Orbiter->m_sConfigFile = sPlutoMOConfFile;

            FileUtils::WriteBufferIntoFile(sPlutoMOConfFile, const_cast<char *>(sPlutoMOConfig.c_str()), 
                sPlutoMOConfig.length());
        }
    }
    else
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "'%s' file not found. No PlutoMO.cfg will be generated", csWapConfFile.c_str());
    }
}

void Orbiter_Plugin::OverrideAVPipe(DeviceData_Router *pDevice_OSD,bool bOverride)
{
	// This will only be called with OSD's.
	// Find the a/v equipment we are connected to, and disable the off for those pipes 
	// So other a/v equipment won't turn them off.  It will be our parent, the media director
	if( !pDevice_OSD->m_pDevice_MD )
		return;

	DeviceData_Router *pDevice_MD = (DeviceData_Router *) pDevice_OSD->m_pDevice_MD;
	MediaDevice *pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(pDevice_MD->m_dwPK_Device);
	if( pMediaDevice )
		pMediaDevice->m_bDontSendOffIfOSD_ON=bOverride;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::OverrideAVPipe OSD %d pDevice_MD %d pMediaDevice %p Override %d",
		pDevice_OSD->m_dwPK_Device, pDevice_MD->m_dwPK_Device, pMediaDevice, (int) bOverride);

	SetPipesEnable(pDevice_MD,bOverride);
}

void Orbiter_Plugin::SetPipesEnable(DeviceData_Router *pDevice,bool bOverride)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::SetPipesEnable device %d pipes %d override %d",
		pDevice->m_dwPK_Device, (int) pDevice->m_mapPipe_Available.size(), (int) bOverride);
		
	for(map<int, class Pipe *>::iterator it=pDevice->m_mapPipe_Available.begin();
		it!=pDevice->m_mapPipe_Available.end();++it)
	{
		class Pipe *pPipe = it->second;
		DeviceData_Router *pDevice_Dest = m_pRouter->m_mapDeviceData_Router_Find(pPipe->m_pDevice_To->m_dwPK_Device);
		if( !pDevice_Dest )
			continue;
		SetPipesEnable(pDevice_Dest,bOverride);
		MediaDevice *pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(pDevice_Dest->m_dwPK_Device);
		if( !pMediaDevice )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Problem overriding a/v pipe to device %d which isn't categorized as media",pPipe->m_pDevice_To->m_dwPK_Device);
			continue; // shouldn't happen -- it's not a/v equipment
		}
		pMediaDevice->m_bDontSendOffIfOSD_ON=bOverride;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::SetPipesEnable device %d m_bDontSendOffIfOSD_ON %d # of sending pipes %d",
			pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, (int) pMediaDevice->m_bDontSendOffIfOSD_ON, (int) pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes.size());

		for(size_t s=0;s<pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes.size();++s)
		{
			// Find the pipes used to send to our output devices and disable them
			DeviceData_Router *pAVDevice = pMediaDevice->m_pDeviceData_Router->m_vectDevices_SendingPipes[s];
			for(map<int, class Pipe *>::iterator it2=pAVDevice->m_mapPipe_Available.begin();
				it2!=pAVDevice->m_mapPipe_Available.end();++it2)
			{
				class Pipe *pPipe2 = it2->second;

				if( pPipe2->m_pDevice_To->m_dwPK_Device==pMediaDevice->m_pDeviceData_Router->m_dwPK_Device )
				{
					pPipe2->m_bDontSendOff=bOverride;
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::SetPipesEnable pipe %p to %d dont sendoff %d",pPipe2, pPipe2->m_pDevice_To->m_dwPK_Device, (int) pPipe2->m_bDontSendOff);
				}
			}
		}
	}
}
//<-dceag-c404-b->

	/** @brief COMMAND: #404 - Set Auto Switch to Remote */
	/** Specifies whether the given orbiter will automatically switch to the remote control when media starts. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #119 True/False */
			/** If true it will switch automatically */

void Orbiter_Plugin::CMD_Set_Auto_Switch_to_Remote(int iPK_Device,bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c404-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Device);
	if( pOH_Orbiter )
		pOH_Orbiter->m_bDontAutoShowRemote=!bTrueFalse;
}
//<-dceag-c406-b->

	/** @brief COMMAND: #406 - Display Message */
	/** Displays a message on orbiters, or all orbiters. */
		/** @param #9 Text */
			/** The message to display */
		/** @param #14 Type */
			/** For devices implementing VFD_LCD_Base, this is the message type defined in the header */
		/** @param #50 Name */
			/** you can give the message a name, such as "status", "error", etc */
		/** @param #102 Time */
			/** Number of seconds to display the message for */
		/** @param #103 List PK Device */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

void Orbiter_Plugin::CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c406-e->
{
	//TODO: remove me

	int iTime = sTime.size() ? atoi(sTime.c_str()) : 0;
	DisplayMessageOnOrbiter(sList_PK_Device,sText,false,iTime,true);
}

//<-dceag-c686-b->

	/** @brief COMMAND: #686 - Display Dialog Box On Orbiter */
	/** Display a dialog box on orbiters, or all orbiter, with 0-4 custom buttons. */
		/** @param #9 Text */
			/** The message to display */
		/** @param #39 Options */
			/** A pipe delimited list with options and messages like this: option1|message1|options2|message2 */
		/** @param #103 List PK Device */
			/** A comma delimited list of orbiters, or all orbiters if empty */

void Orbiter_Plugin::CMD_Display_Dialog_Box_On_Orbiter(string sText,string sOptions,string sList_PK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c686-e->
{
	//TODO: removed me

    //allows to user to use MessageSend tool, with ' instead of "
    sOptions = StringUtils::Replace(sOptions, "'", "\""); 

    vector<string> vectOptions;
    StringUtils::Tokenize(sOptions, "|", vectOptions);

    //fill the rest of parameters with weren't specified
    while(vectOptions.size() < 8)
        vectOptions.push_back("");

    DisplayMessageOnOrbiter(sList_PK_Device, sText, false, 0, true,
        vectOptions[0], vectOptions[1], vectOptions[2], vectOptions[3], 
        vectOptions[4], vectOptions[5], vectOptions[6], vectOptions[7]);
}

//<-dceag-c693-b->

	/** @brief COMMAND: #693 - Send File To Phone */
	/** Send a file to the phone. */
		/** @param #47 Mac address */
			/** Phone's mac address */
		/** @param #137 Command Line */
			/** Command line to be sent */
		/** @param #140 App_Server_Device_ID */
			/** App_Server which will spawn the application */

void Orbiter_Plugin::CMD_Send_File_To_Phone(string sMac_address,string sCommand_Line,int iApp_Server_Device_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c693-e->
{
	if( sCommand_Line.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot send file to %s with empty command line",sMac_address.c_str());
		return;
	}
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to send file to phone. Command line: '%s', device %s.", sCommand_Line.c_str(), sMac_address.c_str());

    //get phone's name
    string sDisplayedInfo = sMac_address;
    //already protected
    UnknownDeviceInfos *pUnknownDeviceInfos = m_mapUnknownDevices_Find(sMac_address);
    if(pUnknownDeviceInfos)
        sDisplayedInfo = pUnknownDeviceInfos->m_sID;

    string sArguments;
    string sCommand_LineClone(sCommand_Line);
    vector<string> vectArgs;
    StringUtils::Tokenize(sCommand_LineClone, " ", vectArgs);

    sCommand_Line = vectArgs[0];
    for(size_t i = 1; i < vectArgs.size(); i++)
    {
        sArguments += vectArgs[i];
        sArguments += i == vectArgs.size() - 1 ? "" : "\t";
    }

    string sParameters, sCommOnFailure, sCommOnSuccess;
    string sName("Phone Install");

	sCommOnFailure = 
		"-targetType category " + 
		StringUtils::itos(m_dwPK_Device) + " " + 
		StringUtils::itos(DEVICECATEGORY_Standard_Orbiter_CONST) + " " + 	
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + 
		StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::itos(SCREEN_DialogSendFileToPhoneFailed_CONST) + " " + 
		StringUtils::itos(COMMANDPARAMETER_PhoneName_CONST) + " " + " \"" + sDisplayedInfo + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_Mac_address_CONST) + " " + " \"" + sMac_address + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_Command_Line_CONST) + " " + " \"" + sCommand_LineClone + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Device_From_CONST) + " " + StringUtils::ltos(m_dwPK_Device) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Device_AppServer_CONST) + " " + StringUtils::itos(iApp_Server_Device_ID);

	sCommOnSuccess =
		"-targetType category " + 
		StringUtils::itos(m_dwPK_Device) + " " + 
		StringUtils::itos(DEVICECATEGORY_Standard_Orbiter_CONST) + " " + 	
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + 
		StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::itos(SCREEN_DialogPhoneInstructions_CONST) + " " + 
		StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " " + "\"<%=T" + StringUtils::itos(TEXT_instructions_CONST) + "%>\"" + " " + 
		StringUtils::itos(COMMANDPARAMETER_PhoneName_CONST) + " " + " \"" + sDisplayedInfo + "\"";

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Launching send to phone job: \"%s\"", sName.c_str());

    DCE::CMD_Spawn_Application cmd_Spawn_Application(m_dwPK_Device, iApp_Server_Device_ID,
        sCommand_Line, sName, sArguments, sCommOnFailure, sCommOnSuccess, false,false,false,true);
	cmd_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
    SendCommand(cmd_Spawn_Application);
}

//<-dceag-c694-b->

	/** @brief COMMAND: #694 - Get Orbiter Status */
	/** Reports the status of the given orbiter */
		/** @param #2 PK_Device */
			/** The orbiter */
		/** @param #5 Value To Assign */
			/** The status: O=OK to load, N=New, skin generated, need router reset, n=new, no skins at all, R=Regenerating skin now, r=Regenerating skin for new orbiter, U=Unknown, D=Device is not an orbiter */
		/** @param #9 Text */
			/** If a regen is in progress, this is a status. */
		/** @param #48 Value */
			/** If a regen is in progress, this is a percentage */

void Orbiter_Plugin::CMD_Get_Orbiter_Status(int iPK_Device,string *sValue_To_Assign,string *sText,int *iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c694-e->
{
	Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(iPK_Device);
	if( pRow_Orbiter )
		pRow_Orbiter->Reload();

	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Device);
	if( pOH_Orbiter )
	{
		if( IsRegenerating(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device) )
		{
			*sValue_To_Assign = "R";
			if( pRow_Orbiter )
			{
				*sText = pRow_Orbiter->RegenStatus_get();
				*iValue = pRow_Orbiter->RegenPercent_get();
			}
			else
				*iValue = 0;  // Shouldn't happen
		}
		else
			*sValue_To_Assign = "O";
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Get_Orbiter_Status for %d returning %s",iPK_Device,sValue_To_Assign->c_str());
		return;
	}

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	if( !pRow_Device )  // We know nothing about this
	{
		*sValue_To_Assign = "U";
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Get_Orbiter_Status for %d returning %s",iPK_Device,sValue_To_Assign->c_str());
		return;
	}

	bool bIsOrbiter=false;
	Row_DeviceCategory *pRow_DeviceCategory = pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow();
	while( pRow_DeviceCategory )
	{
		if( pRow_DeviceCategory->PK_DeviceCategory_get()==DEVICECATEGORY_Orbiter_CONST )
		{
			bIsOrbiter=true;
			break;
		}
		pRow_DeviceCategory = pRow_DeviceCategory->FK_DeviceCategory_Parent_getrow();
	}

	if( !bIsOrbiter )
	{
		*sValue_To_Assign = "D";  // Not an orbiter
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Get_Orbiter_Status for %d returning %s",iPK_Device,sValue_To_Assign->c_str());
		return;
	}

	if( IsRegenerating(iPK_Device) )
	{
		*sValue_To_Assign = "r";  // Regenerating
		if( pRow_Orbiter )
		{
			*sText = pRow_Orbiter->RegenStatus_get();
			*iValue = pRow_Orbiter->RegenPercent_get();
		}
		else
			*iValue = 0;  // Shouldn't happen
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Get_Orbiter_Status for %d returning %s",iPK_Device,sValue_To_Assign->c_str());
		return;
	}

	if( FileUtils::FileExists("/usr/pluto/orbiter/C" + StringUtils::itos(iPK_Device) + "/C" + StringUtils::itos(iPK_Device) + ".info") )
		*sValue_To_Assign = "N";
	else
		*sValue_To_Assign = "n";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Get_Orbiter_Status for %d returning %s",iPK_Device,sValue_To_Assign->c_str());
}

bool Orbiter_Plugin::IsRegenerating(int iPK_Device)
{
	PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	for(list<int>::iterator it=m_listRegenCommands.begin();it!=m_listRegenCommands.end();++it)
		if( *it == iPK_Device )
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS, "RowOrbiter for %d", iPK_Device);			
			Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(iPK_Device);
			if(pRow_Orbiter)
			{
LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to reload row for %d", iPK_Device);				
				pRow_Orbiter->Reload();
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reloaded row for %d", iPK_Device);					
				if(pRow_Orbiter->RegenPercent_get() == 100)
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"IsRegenerating %d = false (scheduled)",iPK_Device);
					return false;
				}
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"IsRegenerating %d = true",iPK_Device);
			return true;
		}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IsRegenerating %d = false",iPK_Device);
	return false;
	
}
//<-dceag-c695-b->

	/** @brief COMMAND: #695 - Get Orbiter Options */
	/** Get various options a user may want to setup his orbiter, such as the skins, users, etc. */
		/** @param #5 Value To Assign */
			/** The response in:
ID\tDescription\n
format */
		/** @param #9 Text */
			/** One of the following: Users, Room, Skin, Language, Size, MenuMenu */

void Orbiter_Plugin::CMD_Get_Orbiter_Options(string sText,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c695-e->
{
	string sSQL = "SELECT PK_" + sText + "," + (sText=="Users" ? "UserName" : "Description");
	if( sText=="Size" )
		sSQL += ",Width,Height";
	sSQL += " FROM " + sText;

    if(sText == "Room")
        sSQL += " WHERE FK_Installation = " + StringUtils::itos(m_pRouter->iPK_Installation_get());

    if(sText == "MainMenu")
        sSQL = "SELECT PK_DesignObj, Description FROM DesignObj INNER JOIN DeviceTemplate_DesignObj ON DesignObj.PK_DesignObj = DeviceTemplate_DesignObj.FK_DesignObj";

	PlutoSqlResult result_set;
    DB_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
	{
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			(*sValue_To_Assign) += row[0] + string("\t") + row[1];
			if( sText=="Size" )
				(*sValue_To_Assign) += string("\t") + row[2] + "\t" + row[3];
			(*sValue_To_Assign) += "\n";
		}
	}
}

bool Orbiter_Plugin::CheckForNewWizardDevices(DeviceData_Router *pDevice_MD)
{
	return false;  // Don't think we're going to need to use this
	// this was never implemented
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("IsNewDevice=1",&vectRow_Device);
	if( vectRow_Device.size()==0 )
		return false;
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
		if( pRow_DeviceTemplate && pRow_DeviceTemplate->WizardURL_get().size() )
		{
			// Send this to all m/d's or just the one past in
			for(map<int,class DeviceData_Router *>::const_iterator it=m_pRouter->m_mapDeviceData_Router_get()->begin();it!=m_pRouter->m_mapDeviceData_Router_get()->end();++it)
			{
				DeviceData_Router *pDeviceData_Router_MD=(*it).second;
				if( (pDevice_MD && pDevice_MD==pDeviceData_Router_MD) || (!pDevice_MD && pDeviceData_Router_MD->WithinCategory(DEVICECATEGORY_Media_Director_CONST)) )
				{
//					DeviceData_Router *pDevice_AppServer,*pDevice_Orbiter_OSD;
//					m_pGeneral_Info_Plugin->GetAppServerAndOsdForMD(pDeviceData_Router_MD,&pDevice_AppServer,&pDevice_Orbiter_OSD);
//					if( !pDevice_AppServer || !pDevice_Orbiter_OSD )
//						return false; // Should never happen

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CheckForNewWizardDevices device %d needs config",pRow_Device->PK_Device_get());
					/*
					Message *pMessage = m_pGeneral_Info_Plugin->BuildMessageToSpawnApp(NULL,pDeviceData_Router_MD,
						pDevice_AppServer,pDevice_Orbiter_OSD,
						"/usr/pluto/bin/Mozilla.sh","0\thttp://www.cnn.com","New Device Wizard",1);
					QueueMessageToRouter(pMessage);
					*/
				}
			}
			return true;
		}
		else
			pRow_Device->IsNewDevice_set(0);
	}
	m_pDatabase_pluto_main->Device_get()->Commit();
	return false;
}
//<-dceag-c758-b->

	/** @brief COMMAND: #758 - Send Orbiter Popups */
	/** Indicates if Orbiter should receive popup messages */
		/** @param #119 True/False */
			/** True = yes, I want popups */
		/** @param #198 PK_Orbiter */
			/** The orbiter */

void Orbiter_Plugin::CMD_Send_Orbiter_Popups(bool bTrueFalse,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c758-e->
{
	OH_Orbiter *pOH_Orbiter=m_mapOH_Orbiter_Find(iPK_Orbiter);
	if( pOH_Orbiter )
		pOH_Orbiter->m_bSendPopups = bTrueFalse;

	m_sPK_Device_AllOrbiters_AllowingPopups="";
	for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
		if( it->second->m_bSendPopups )
			m_sPK_Device_AllOrbiters_AllowingPopups += StringUtils::itos(it->first) + ",";

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Send_Orbiter_Popups list is now %s",
		m_sPK_Device_AllOrbiters_AllowingPopups.c_str());
}

bool Orbiter_Plugin::DeviceConfigured(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	string sName = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %s configured! Processing next unknown device...", sName.c_str());
    ProcessUnknownDevice();
	return false;
}
//<-dceag-c818-b->

	/** @brief COMMAND: #818 - Get Screen Saver Files */
	/** Given an Orbiter, returns the files that Orbiter should use for it's screen saver */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #13 Filename */
			/** A \n delimited list of the files to use */

void Orbiter_Plugin::CMD_Get_Screen_Saver_Files(int iPK_Device,string *sFilename,string &sCMD_Result,Message *pMessage)
//<-dceag-c818-e->
{
	string sSQL = "SELECT DISTINCT PK_File,Path,Filename "
		"FROM File "
		"JOIN File_Attribute ON FK_File=PK_File "
		"JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Screen_Saver_For_MD_CONST) " "
		"WHERE (Name='' OR Name='*' OR Name='" + StringUtils::itos(iPK_Device) + "') AND Missing=0 AND Filename like '%jpg' "
		" AND PK_File NOT IN "
		"( "
		"SELECT FK_File FROM File_Attribute "
		"INNER JOIN Attribute ON FK_Attribute = PK_Attribute "
		"WHERE FK_AttributeType = " TOSTRING(ATTRIBUTETYPE_Screen_saver_disabled_CONST)
		")";

	PlutoSqlResult result_set1,result_set2;

	// The above are the files to use for the screen saver for this orbiter
	result_set1.r=m_pDatabase_pluto_media->db_wrapper_query_result(sSQL);
	PlutoSqlResult *p_result_set = &result_set1;

	if( !result_set1.r || result_set1.r->row_count==0 )
		return;

	// See if this orbiter is supposed to filter based on tags
	DeviceData_Router *pDevice_Orbiter = m_pRouter->m_mapDeviceData_Router_Find( iPK_Device );
	string sKeywords;
	if( pDevice_Orbiter && (sKeywords=pDevice_Orbiter->m_mapParameters_Find(DEVICEDATA_Keywords_CONST)).empty()==false )
	{
		// Keywords needs to be ' separated, like 'Dogs','Cats'.  Incoming formate is Dogs,Cats
		sKeywords = "'" + StringUtils::Replace(sKeywords,",","','") + "'";
		StringUtils::Replace(&sKeywords," ",""); // Strip spaces
	    DB_ROW row;
		string sPK_File;

		while ((row = db_wrapper_fetch_row(result_set1.r)))
			sPK_File += (sPK_File.empty() ? "" : ",") + string(row[0]);

		sSQL = "SELECT DISTINCT PK_File,Path,Filename "
			"FROM File "
			"JOIN File_Attribute ON FK_File=PK_File "
			"JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Keyword_CONST) " "
			"WHERE Name IN (" + sKeywords + ") AND Filename like '%jpg' AND "
			"PK_File IN (" + sPK_File + ");";

		// Create a new result set based on the tags
		result_set2.r=m_pDatabase_pluto_media->db_wrapper_query_result(sSQL);
		if( !result_set2.r || result_set2.r->row_count==0 )
			return;

		p_result_set = &result_set2;  // Assign it to the  pointer we will use below
	}

    DB_ROW row;
	while ((row = db_wrapper_fetch_row(p_result_set->r)))
		*sFilename += row[1] + string("/") + row[2] + "\n";
}

void Orbiter_Plugin::StartRetrievingScreenSaverFiles()
{
	// If all orbiters have filters on tags we will only retrieve files that use those tags
	// Otherwise we should just get all files and CMD_Get_Screen_Saver_Files will do the 
	// filtering.
	string sOnlyRetrieveTags;

	for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = it->second;
		string sTags = pOH_Orbiter->m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_Keywords_CONST);
		if( sTags.empty() )
		{
			sOnlyRetrieveTags="";  // Retrieve all tags
			break;
		}
		if( sOnlyRetrieveTags.empty()==false )
			sOnlyRetrieveTags += ",";
		sOnlyRetrieveTags += sTags;  // Add these to the tags
	}

	string s = DATA_Get_File_Name_and_Path();
	vector<string> vectApps;
	StringUtils::Tokenize(s,"\r\n",vectApps);

	DeviceData_Router *pDevice_App_Server=NULL,*pDevice_Us = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);
	if( pDevice_Us )
		pDevice_App_Server = (DeviceData_Router *) pDevice_Us->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter_Plugin::StartRetrievingScreenSaverFiles apps %d/%s", vectApps.size(), s.c_str());

	if( pDevice_App_Server )
	{
		for(vector<string>::iterator it=vectApps.begin();it!=vectApps.end();++it)
		{
			string sFilename = FileUtils::FilenameWithoutPath(*it);
			string sArguments = StringUtils::itos(DATA_Get_Quantity()) + "\t" +
				StringUtils::itos(DATA_Get_Width()) + "\t" +
				StringUtils::itos(DATA_Get_Height()) + "\t" +
				sOnlyRetrieveTags;
		
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
				*it,"screen_saver_" + sFilename,sArguments,"","",false,false,false,false);
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
			SendCommand(CMD_Spawn_Application);
		}

		// Also update our list of software while we're at it
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/pluto/bin/getxmls","getxmls","","","",false,false,false,false);
		CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
		SendCommand(CMD_Spawn_Application);
	}
	m_pAlarmManager->AddRelativeAlarm(60 * 60 * 24 /* do again in 24 hours */,this,PROCESS_SCREEN_SAVER_PHOTOS,NULL);
}

void Orbiter_Plugin::AlarmCallback(int id, void* param)
{
	if( id==PROCESS_SCREEN_SAVER_PHOTOS )
		StartRetrievingScreenSaverFiles();
}

bool Orbiter_Plugin::PresenceDetected( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];
	map<string,int>::iterator it=m_mapUsersID.find( sID );
	if( it!=m_mapUsersID.end() )
	{
		MediaDevice *pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(pMessage->m_dwPK_Device_From);
		if( !pMediaDevice || pMediaDevice->m_mapEntertainArea.size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::PresenceDetected -- sender is not a media device or has no ent area %p",pMediaDevice);
			return false;
		}

		EVENT_Follow_Me_Media(0, 0, it->second, pMediaDevice->m_mapEntertainArea.begin()->first, 0);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::PresenceDetected no user associated with %s",sID.c_str());
	return false;
}

bool Orbiter_Plugin::PresenceLost( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	return false;
}

//<-dceag-c828-b->

	/** @brief COMMAND: #828 - Get List of Remotes */
	/** Get the list of follow-me enabled remote controls */
		/** @param #9 Text */
			/** The list of remotes in the format:
]PK_Device],[PK_User],[RemoteID] */

void Orbiter_Plugin::CMD_Get_List_of_Remotes(string *sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c828-e->
{
}

//<-dceag-c829-b->

	/** @brief COMMAND: #829 - Get Remote ID */
	/** For the special follow-me controls, this takes in a serial number and returns a unique 8-bit id for the remote, and the PK_Device */
		/** @param #2 PK_Device */
			/** The device id */
		/** @param #48 Value */
			/** The 8-bit unique id for this remote */
		/** @param #206 UID */
			/** The serial number */

void Orbiter_Plugin::CMD_Get_Remote_ID(string sUID,int *iPK_Device,int *iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c829-e->
{
	string sSQL = "SELECT PK_Device FROM Device_DeviceData JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceData=" TOSTRING(DEVICEDATA_Serial_Number_CONST) " AND IK_DeviceData='" + sUID + "' AND FK_DeviceTemplate=" TOSTRING(DEVICETEMPLATE_MCR_Remote_CONST);
	PlutoSqlResult result_set;
    DB_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Get_Remote_ID %s is %d",sUID.c_str(),*iPK_Device);
#endif
		*iPK_Device = atoi(row[0]);
		string sValue = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,*iPK_Device,DEVICEDATA_PortChannel_Number_CONST);
		*iValue = atoi(sValue.c_str());
		return;
	}

	DCE::CMD_Create_Device CMD_Create_Device(m_dwPK_Device,m_pGeneral_Info_Plugin->m_dwPK_Device,DEVICETEMPLATE_MCR_Remote_CONST,
		"",0,"","",0,0,"",pMessage->m_dwPK_Device_From,0,iPK_Device);
	if( !SendCommand(CMD_Create_Device) || *iPK_Device==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter_Plugin::CMD_Get_Remote_ID error creating remote");
		return;
	}

	*iValue=1;
	sSQL = "SELECT max(IK_DeviceData) FROM Device_DeviceData JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceData=" 
		TOSTRING(DEVICEDATA_PortChannel_Number_CONST) " AND FK_DeviceTemplate=" TOSTRING(DEVICETEMPLATE_MCR_Remote_CONST);
	PlutoSqlResult result_set2;
	if( (result_set2.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set2.r)) && atoi(row[0])>0 )
		*iValue = atoi(row[0]) + 1;
	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,*iPK_Device,DEVICEDATA_PortChannel_Number_CONST,StringUtils::itos(*iValue));
	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,*iPK_Device,DEVICEDATA_Serial_Number_CONST,sUID);
}

//<-dceag-c830-b->

	/** @brief COMMAND: #830 - Set Active Remote */
	/** Specified which remote control is controling a particular device. */
		/** @param #2 PK_Device */
			/** The device that is controlling it */
		/** @param #197 Fire Event */
			/** If true, a follow me will get fired for this remote */
		/** @param #198 PK_Orbiter */
			/** The orbiter being controlled */

void Orbiter_Plugin::CMD_Set_Active_Remote(int iPK_Device,bool bFire_Event,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c830-e->
{
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	int PK_Orbiter_Prior = m_mapRemote_Orbiter_Find(iPK_Device); // See what Orbiter this was previously bound to
	OH_Orbiter *pOH_Orbiter_Prior = PK_Orbiter_Prior ? m_mapOH_Orbiter_Find(PK_Orbiter_Prior) : NULL;

	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Orbiter);
	if( pOH_Orbiter )
	{
		pOH_Orbiter->m_dwPK_Device_CurrentRemote=iPK_Device;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter_Plugin::CMD_Set_Active_Remote Orbiter %d has remote %d previously orbiter %d",
			iPK_Orbiter,iPK_Device,PK_Orbiter_Prior);
		if( bFire_Event )
		{
			if( pOH_Orbiter->m_pEntertainArea )
				EVENT_Follow_Me_Media(iPK_Orbiter, iPK_Device,
					pOH_Orbiter_Prior && pOH_Orbiter_Prior!=pOH_Orbiter && pOH_Orbiter_Prior->m_pOH_User ? pOH_Orbiter_Prior->m_pOH_User->m_iPK_Users : NULL,
					pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea,
					pOH_Orbiter_Prior && pOH_Orbiter_Prior->m_pEntertainArea ? pOH_Orbiter_Prior->m_pEntertainArea->m_iPK_EntertainArea : 0);

			EVENT_Follow_Me_Telecom(iPK_Orbiter, iPK_Device,
				pOH_Orbiter->m_dwPK_Room,
				pOH_Orbiter_Prior && pOH_Orbiter_Prior!=pOH_Orbiter && pOH_Orbiter_Prior->m_pOH_User ? pOH_Orbiter_Prior->m_pOH_User->m_iPK_Users : NULL,
				pOH_Orbiter_Prior ? pOH_Orbiter_Prior->m_dwPK_Room : 0);
		}
		m_mapRemote_Orbiter[iPK_Device]=iPK_Orbiter;
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter_Plugin::CMD_Set_Active_Remote No orbiter %d",iPK_Orbiter);
}

class DataGridTable *Orbiter_Plugin::FloorplanDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"General_Info_Plugin::FloorplanDevices");
#endif

	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	int iRow=0;

	if( Parms=="M" )
	{
        vector<Row_EntertainArea *> vectRow_EntertainArea;
		string SQL = "JOIN Room ON FK_Room=PK_Room WHERE FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get());
		m_pDatabase_pluto_main->EntertainArea_get()->GetRows(SQL,&vectRow_EntertainArea);
		for(vector<Row_EntertainArea *>::iterator it=vectRow_EntertainArea.begin();it!=vectRow_EntertainArea.end();++it)
		{
			Row_EntertainArea *pRow_EntertainArea = *it;
			EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(pRow_EntertainArea->PK_EntertainArea_get());
			if( !pEntertainArea )
				continue; // Shouldn't happen
			
			// Store the EA as a negative value to differentiate from a device
			pCell = new DataGridCell(pRow_EntertainArea->Description_get(),"-" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get()));
			pDataGrid->SetData(0,iRow++,pCell);

			int iPK_FloorplanObjectType_Color=0,iColor=0,PK_DesignObj_Toolbar=0;
			string sDescription;
			string OSD;
			Row_FloorplanObjectType_Color *pRow_FloorplanObjectType_Color = NULL;

			m_pMedia_Floorplan->GetFloorplanDeviceInfo(NULL,pEntertainArea,
				pRow_EntertainArea->FK_FloorplanObjectType_get(),
				iPK_FloorplanObjectType_Color,iColor,sDescription,OSD,PK_DesignObj_Toolbar);

			if( iPK_FloorplanObjectType_Color )
				pRow_FloorplanObjectType_Color = m_pDatabase_pluto_main->FloorplanObjectType_Color_get()->GetRow(iPK_FloorplanObjectType_Color);

			if( !iColor && pRow_FloorplanObjectType_Color )
				iColor = pRow_FloorplanObjectType_Color->Color_get();
			if( sDescription.length()==0 && pRow_FloorplanObjectType_Color )
				sDescription = pRow_FloorplanObjectType_Color->Description_get();

			pCell->m_AltColor = iColor;
			pCell->m_mapAttributes["Description"] = sDescription;
			pCell->m_mapAttributes["OSD"] = OSD;
			pCell->m_mapAttributes["PK_DesignObj_Toolbar"] = StringUtils::itos(PK_DesignObj_Toolbar);
		}
	}
	else if( Parms.empty()==false )
	{
		int PK_DeviceCategory=0;
		FloorplanInfoProvider *pFloorplanInfoProvider=NULL;

		switch(Parms[0])
		{
		case 'L':
			PK_DeviceCategory=DEVICECATEGORY_Lighting_Device_CONST;
			pFloorplanInfoProvider=m_pLighting_Floorplan;
			break;
		case 'S':
			PK_DeviceCategory=DEVICECATEGORY_Security_Device_CONST;
			pFloorplanInfoProvider=m_pSecurity_Floorplan;
			break;
		case 'T':
			PK_DeviceCategory=DEVICECATEGORY_Phones_CONST;
			pFloorplanInfoProvider=m_pTelecom_Floorplan;
			break;
		case 'C':
			PK_DeviceCategory=DEVICECATEGORY_Climate_Device_CONST;
			pFloorplanInfoProvider=m_pClimate_Floorplan;
			break;
		}

		if( !pFloorplanInfoProvider )
			return NULL; // Shouldn't happen

		string sSQL = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory LEFT JOIN Room ON PK_Room=FK_Room WHERE FK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) + " OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) + " ORDER BY Room.Description";
		vector<Row_Device *> vectRow_Device;
		m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device);
		for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
		{
			Row_Device *pRow_Device = *it;
			DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(pRow_Device->PK_Device_get());
			if( !pDevice )
				continue;

			string sDescription = pDevice->m_sDescription;
			if( pDevice->m_pRoom )
				sDescription = pDevice->m_pRoom->m_sDescription + "\n" + sDescription;

			int iPK_FloorplanObjectType_Color=0,iColor=0,PK_DesignObj_Toolbar=0;
			string OSD;
			Row_FloorplanObjectType_Color *pRow_FloorplanObjectType_Color = NULL;

			string sD2;
			pFloorplanInfoProvider->GetFloorplanDeviceInfo(pDevice,NULL,
				atoi(pDevice->m_mapParameters_Find(DEVICEDATA_PK_FloorplanObjectType_CONST).c_str()),
				iPK_FloorplanObjectType_Color,iColor,sD2,OSD,PK_DesignObj_Toolbar);

			if( sD2.empty()==false || OSD.empty() == false )
			{
				sDescription += "\n";
				if( sD2.empty()==false )
					sDescription += sD2 + " / ";
				sDescription += OSD;
			}

			pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
			pDataGrid->SetData(0,iRow++,pCell);

			if( iPK_FloorplanObjectType_Color )
				pRow_FloorplanObjectType_Color = m_pDatabase_pluto_main->FloorplanObjectType_Color_get()->GetRow(iPK_FloorplanObjectType_Color);

			if( !iColor && pRow_FloorplanObjectType_Color )
				iColor = pRow_FloorplanObjectType_Color->Color_get();
			if( sDescription.length()==0 && pRow_FloorplanObjectType_Color )
				sDescription = pRow_FloorplanObjectType_Color->Description_get();

			pCell->m_AltColor = iColor;
			pCell->m_mapAttributes["Description"] = sDescription;
			pCell->m_mapAttributes["OSD"] = OSD;
			pCell->m_mapAttributes["PK_DesignObj_Toolbar"] = StringUtils::itos(PK_DesignObj_Toolbar);
		}
	}

	return pDataGrid;
}
//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void Orbiter_Plugin::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
}

bool Orbiter_Plugin::SafeToReload(string &sReason)
{
    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);
	if( m_listRegenCommands.size()==0 )
		return true;

	sReason = "See Pending Tasks.  Orbiter Plugin Generating Orbiters: ";

	bool bFirst=true;
	for(list<int>::iterator it=m_listRegenCommands.begin();it!=m_listRegenCommands.end();++it)
	{
		if( bFirst )
			bFirst=false;
		else
			sReason += ",";
		sReason += StringUtils::itos(*it);
	}
	return false;
}

//<-dceag-c918-b->

	/** @brief COMMAND: #918 - Start AV Wizard */
	/** Start AV Wizard on the given device. */
		/** @param #2 PK_Device */
			/** The device to do this for. */

void Orbiter_Plugin::CMD_Start_AV_Wizard(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c918-e->
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if(NULL != pDevice)
	{
		DeviceData_Router *pDevice_App_Server = (DeviceData_Router *)pDevice->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);

		if(NULL != pDevice_App_Server)
		{
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
				"/usr/pluto/bin/RebootWithAVWizard.sh","RebootWithAVWizard","","","",false,false,false,false);
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
			SendCommand(CMD_Spawn_Application);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Couldn't find an app server for device %d", iPK_Device);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Invalid device %d. The device was registered?", iPK_Device);
}

