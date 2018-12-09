/*
 General_Info_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 

 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include <sstream>
//<-dceag-d-b->
#include "General_Info_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/DatabaseUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "../PlutoUtils/md5.h"
#include "BD/PhoneDevice.h"
#include "CreateDevice/CreateDevice.h"
#include "UpdateEntArea/UpdateEntArea.h"

#include "DCERouter.h"
#include "DCE/DeviceData_Router.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Table_Device_QuickStart.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Table_QuickStartTemplate.h"
#include "pluto_main/Table_Device_MRU.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Country.h"
#include "pluto_main/Table_Device_Device_Related.h"
#include "pluto_main/Table_Distro.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"
#include "pluto_main/Table_Software.h"
#include "pluto_main/Table_Software_Device.h"
#include "pluto_main/Table_FloorplanObjectType_Color.h"
#include "pluto_main/Define_RoomType.h"

#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_DiscLocation.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Define_AttributeType.h"

#include "DataGrid.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "Event_Plugin/Event_Plugin.h"
#include "Gen_Devices/AllScreens.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"

#include "SerializeClass/ShapesColors.h"

#include "Web_DHCP_Query.h"
using namespace nsWeb_DHCP_Query;
using namespace DefaultScenarios;

#ifndef WEB_QUERY_DEBUG
static const string sURL_Base = "http://plutohome.com/getRegisteredDevices.php";
#else
static const string sURL_Base = "http://10.0.0.175/plutohome-com/getRegisteredDevices.php";
#endif

#include "DCEConfig.h"
DCEConfig dceConfig; // Needed by CreateDevice

#define UPDATE_ENT_AREA				1
#define PROCESS_CHILD_DEVICES		2

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
General_Info_Plugin::General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: General_Info_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_GipMutex("GeneralInfo")
{
	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_GipMutex.Init( &m_MutexAttr );

	m_pPostCreateOptions=NULL;
	m_bRerunConfigWhenDone=false;
	m_pDatabase_pluto_main=NULL;
	m_pDatabase_pluto_media=NULL;
	m_dwPK_Device_Prompting_For_A_Room=0;
	m_bNewInstall=false;
	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;
	m_pAlarmManager=NULL;
}

//<-dceag-getconfig-b->
bool General_Info_Plugin::GetConfig()
{
	if( !General_Info_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit_set(true);
		return false;
	}

	m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_media->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(), "pluto_media" ,m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit_set(true);
		return false;
	}

	if( !DatabaseUtils::AlreadyHasRooms(m_pDatabase_pluto_main,m_pRouter->iPK_Installation_get()) || !DatabaseUtils::AlreadyHasUsers(m_pDatabase_pluto_main,m_pRouter->iPK_Installation_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::GetConfig System has no rooms or users yet.");
		m_bNewInstall=true;
	}

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);  // number of worker threads

	return true;
}

void General_Info_Plugin::PrepareToDelete()
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	Command_Impl::PrepareToDelete();
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
General_Info_Plugin::~General_Info_Plugin()
//<-dceag-dest-e->
{
	delete m_pPostCreateOptions;
	delete m_pDatabase_pluto_main;
	delete m_pDatabase_pluto_media;	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool General_Info_Plugin::Register()
//<-dceag-reg-e->
{
	// Get the datagrid plugin
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pEvent_Plugin=( Event_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Event_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin || !m_pEvent_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to general info plugin");
		return false;
	}

	m_pPostCreateOptions = new PostCreateOptions(m_pDatabase_pluto_main,m_pRouter,this);
	
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
			new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::PendingTasksGrid ) )
			, DATAGRID_Pending_Tasks_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
			new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::QuickStartApps ) )
			, DATAGRID_Quick_Start_Apps_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
			new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::MRUDocuments ) )
			, DATAGRID_MRU_Documents_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
			new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::Rooms ) )
			, DATAGRID_Rooms_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::RoomTypes ) )
			, DATAGRID_Room_Types_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::ChildrenInfo)), 
		DATAGRID_Devices_Children_Of_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AvailableSerialPorts)), 
		DATAGRID_Available_Serial_Ports_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::LightsTypes)), 
		DATAGRID_LightType_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::InstalledAVDevices)), 
		DATAGRID_Installed_AV_Devices_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::SensorType)), 
		DATAGRID_Sensor_Type_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::BookmarkList)), 
		DATAGRID_Mozilla_Bookmarks_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::TypesOfPhones)), 
		DATAGRID_Types_Of_Mobile_Phones_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::UsersGrid)), 
		DATAGRID_Users_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::CountriesGrid)), 
		DATAGRID_Countries_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::CitiesGrid)), 
		DATAGRID_Cities_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::DevicesOfCategory)), 
		DATAGRID_Devices_Of_Category_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::StorageDevices)), 
		DATAGRID_Storage_Devices_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::DeviceTemplatesOfCategory)), 
		DATAGRID_Device_Templates_By_Categ_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AddSoftware)),
		DATAGRID_Installable_Apps_CONST, PK_DeviceTemplate_get());

	//AV Wizard - Template settings
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVWhatDelay)), 
		DATAGRID_TVWhatDelays_CONST,PK_DeviceTemplate_get());
/*
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVDiscret)), 
		DATAGRID_Confirm_On_Off_Codes_CONST,PK_DeviceTemplate_get());
		*/
	//AV Wizard - Input 
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVInputNotListed)), 
		DATAGRID_Select_Available_Inputs_CONST,PK_DeviceTemplate_get());	
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVMediaType)), 
		DATAGRID_Media_Type_CONST,PK_DeviceTemplate_get());
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVMediaConnector)), 
		DATAGRID_Media_Connector_Type_CONST,PK_DeviceTemplate_get());
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVInputsAvaible)), 
		DATAGRID_Confirm_Inputs_Order_CONST,PK_DeviceTemplate_get());

	//AV Wizard - DSP Mode
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVDSPMode)), 
		DATAGRID_Select_Available_DSPModes_CONST,PK_DeviceTemplate_get());
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVDSPModeOrder)), 
		DATAGRID_Confirm_DSPModes_Order_CONST,PK_DeviceTemplate_get());

	//AV Wizard - IR Codes
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AVIRCodesSets)), 
		DATAGRID_IR_Codes_Sets_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::IRCommands)), 
		DATAGRID_IR_Commands_CONST,PK_DeviceTemplate_get());
	
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::Discs)), 
		DATAGRID_Discs_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::JukeBoxes)), 
		DATAGRID_JukeBoxes_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::HardDiscs)), 
		DATAGRID_HardDiscs_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::CompactFlashes)), 
		DATAGRID_CompactFlashes_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::NetworkStorage)), 
		DATAGRID_NetworkStorage_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::JukeboxDrives)), 
		DATAGRID_Jukebox_Drives_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::JukeboxSlots)), 
		DATAGRID_Jukebox_Slots_CONST,PK_DeviceTemplate_get());

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::ReportingChildDevices ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Reporting_Child_Devices_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::LowSystemDiskSpace ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Low_System_Disk_Space_CONST );

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
void General_Info_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void General_Info_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c68-b->

	/** @brief COMMAND: #68 - Get Device Data */
	/** Return the device data for a device. */
		/** @param #2 PK_Device */
			/** The device which you want data from */
		/** @param #5 Value To Assign */
			/** The value. */
		/** @param #52 PK_DeviceData */
			/** What parameter to get. */
		/** @param #53 UseDefault */
			/** Report the default value, rather than requesting a live value. */

void General_Info_Plugin::CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c68-e->
{
	if( bUseDefault )
	{
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
		if( !pDeviceData_Router )
			*sValue_To_Assign="BAD DEVICE";
		else
		{
			if( pDeviceData_Router->m_mapParameters.find(iPK_DeviceData)==pDeviceData_Router->m_mapParameters.end() )
				*sValue_To_Assign="BAD PARAMETER";
			else
				*sValue_To_Assign=pDeviceData_Router->m_mapParameters[iPK_DeviceData];
		}
		return;
	}
	else
	{
		// TODO -- Send device request
	}
}

//<-dceag-c71-b->

	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

void General_Info_Plugin::CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c71-e->
{
	LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File: file %s", sFilename.c_str());

	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

	if( c && Length )
		LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "sending file: %s size: %d", sFilename.c_str(),(int) Length);
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "requested missing file: %s", sFilename.c_str());

	*iData_Size = (int) Length;
	*pData = c;

	LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File: ended for file %s", sFilename.c_str());
}

//<-dceag-c239-b->

	/** @brief COMMAND: #239 - Request File And Checksum */
	/** Get the contents of a file from the server and the checksum of the file */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */
		/** @param #91 Checksum */
			/** The checksum of the file */
		/** @param #92 Checksum Only */
			/** If this is true, this command will return only the checksum of the file, Data will be null. */

void General_Info_Plugin::CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage)
//<-dceag-c239-e->
{
	cout << "Command #239 - Request File And Checksum" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #91 - Checksum=" << sChecksum << endl;
	cout << "Parm #92 - Checksum_Only=" << bChecksum_Only << endl; 

	LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "request missing  file: %s", sFilename.c_str());
	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

	if(c==NULL) //file not found
	{
		LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "The requested file '%s' was not found", sFilename.c_str());
		return;
	}

	*iData_Size = (int) Length;
	*pData = c;
	*sChecksum = FileUtils::FileChecksum(*pData, *iData_Size);

	if(*bChecksum_Only)
	{
		*iData_Size = 0;

		if(NULL != *pData)
		{
			delete *pData;
			*pData = NULL;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File_And_Checksum: file %s", sFilename.c_str());
}
//<-dceag-c246-b->

	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

void General_Info_Plugin::CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage)
//<-dceag-c246-e->
{
	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,iPK_DeviceData,sValue_To_Assign);
}

//<-dceag-c247-b->

	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */
		/** @param #2 PK_Device */
			/** The device id which you need information for. */
		/** @param #5 Value To Assign */
			/** The state data for the device. */

void General_Info_Plugin::CMD_Get_Device_State(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c247-e->
{
	DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( !pDeviceData_Router )
	{
		sCMD_Result = "BAD DEVICE";
		*sValue_To_Assign = "";
	}
	else
	{
		sCMD_Result = "OK";
		*sValue_To_Assign = pDeviceData_Router->m_sState_get();
	}
}

//<-dceag-c248-b->

	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */
		/** @param #2 PK_Device */
			/** The device id which you need information for. */
		/** @param #5 Value To Assign */
			/** The status data for the device. */

void General_Info_Plugin::CMD_Get_Device_Status(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c248-e->
{
	DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( !pDeviceData_Router )
	{
		sCMD_Result = "BAD DEVICE";
		*sValue_To_Assign = "";
	}
	else
	{
		sCMD_Result = "OK";
		*sValue_To_Assign = pDeviceData_Router->m_sStatus_get();
	}
}
//<-dceag-createinst-b->!
//<-dceag-c272-b->

	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */
		/** @param #21 Force */

void General_Info_Plugin::CMD_Restart_DCERouter(string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c272-e->
{
// temp debugging since this wasn't going through
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Forwarding reload to router");
	Message *pMessageOut = new Message(pMessage->m_dwPK_Device_From,DEVICEID_DCEROUTER,
		PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,
		(sForce == "Y" || sForce == "y") ? SYSCOMMAND_RELOAD_FORCED : SYSCOMMAND_RELOAD, 0);
	SendMessageToRouter(pMessageOut);
}
//<-dceag-c322-b->

	/** @brief COMMAND: #322 - Wake Device */
	/** Sends a Wake on LAN to the specified device. */
		/** @param #2 PK_Device */
			/** The device to wake up */

void General_Info_Plugin::CMD_Wake_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c322-e->
{
	string sPK_Device = StringUtils::itos(iPK_Device);
	char * cmd[] = { (char *)("/usr/pluto/bin/WakeMD.sh"), (char *)("--dev"), (char *) sPK_Device.c_str(), NULL };
	ProcessUtils::SpawnDaemon(cmd[0], cmd);
}

//<-dceag-c323-b->

	/** @brief COMMAND: #323 - Halt Device */
	/** Halts, or suspends, the given device. */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */
		/** @param #47 Mac address */
			/** If PK_Device is not specified (is 0), we'll use the mac address to determine the device id */

void General_Info_Plugin::CMD_Halt_Device(int iPK_Device,string sForce,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c323-e->
{	
	if ( 0 == iPK_Device ) // iPK_Device=0 => find device by MAC Address
	{
		if( sMac_address.empty() )
		{
			iPK_Device = pMessage->m_dwPK_Device_From;
		}
		else
		{
			PlutoSqlResult result;
			DB_ROW row;
			string sSQL = 
				"SELECT PK_Device "
				"FROM Device "
				"WHERE MACaddress = '" + sMac_address +"'";

			enum FieldNames
			{
				fnDeviceID			
			};

			int iDeviceID = 0;
			// Execute query
			if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL.c_str())) )
			{
				while((row = db_wrapper_fetch_row(result.r)))	
				{
					if(NULL != row[fnDeviceID]) // device found
					{
						iDeviceID = atoi(row[fnDeviceID]);
						break;
					}
				}
				if ( 0 == iDeviceID ){
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot halt device %s - device not found in database", sMac_address.c_str() );
					return;
				}
				else iPK_Device = iDeviceID;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot halt device %s - error executing SQL", sMac_address.c_str());
				return;
			}
		}
	}


	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( !pDevice )
		pDevice = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);

	if( !pDevice )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot halt unknown device %d",iPK_Device);
		return;
	}

	string sPK_Orbiters = m_pOrbiter_Plugin->PK_Device_Orbiters_In_Room_get( pDevice->m_dwPK_Room, false );
	if( !sPK_Orbiters.empty() )
	{
		DCE::SCREEN_PopupMessage_DL SCREEN_PopupMessage_DL(m_dwPK_Device,sPK_Orbiters,"Shutting down","","","","0","1");
		SendCommand(SCREEN_PopupMessage_DL);
	}

	DeviceData_Router *pDevice_AppServer=NULL;
	if( pDevice->m_pDevice_Core )
		pDevice_AppServer = (DeviceData_Router *) ((DeviceData_Impl *) (pDevice->m_pDevice_Core))->FindSelfOrChildWithinCategory( DEVICECATEGORY_App_Server_CONST );

	if( !pDevice_AppServer && pDevice->m_pDevice_MD )
		pDevice_AppServer = (DeviceData_Router *) ((DeviceData_Impl *) (pDevice->m_pDevice_MD))->FindSelfOrChildWithinCategory( DEVICECATEGORY_App_Server_CONST );

	if( !pDevice_AppServer )
		pDevice_AppServer = (DeviceData_Router *) pDevice->FindSelfOrChildWithinCategory( DEVICECATEGORY_App_Server_CONST );

	if( !pDevice_AppServer )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot halt unknown device %d without an app server",iPK_Device);
		return;
	}

	if( sForce=="V" || sForce=="N" )
	{
		SetNetBoot(pDevice,sForce=="N");
		DCE::CMD_Halt_Device CMD_Halt_Device(m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,pDevice->m_dwPK_Device,sForce.c_str(), "");
		SendCommand(CMD_Halt_Device);
	}
	else
	{
		Message *pMessage_Out = new Message(pMessage);
		pMessage_Out->m_dwPK_Device_To = pDevice_AppServer->m_dwPK_Device;
		SendMessageToRouter(pMessage_Out);
	}
}

void General_Info_Plugin::SetNetBoot(DeviceData_Router *pDevice,bool bNetBoot)
{
	if( pDevice->m_sMacAddress.length()<17 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot set boot on device %d with no Mac",pDevice->m_dwPK_Device);
		return;
	}

	string sFile = "/tftpboot/pxelinux.cfg/01-" + StringUtils::ToLower(StringUtils::Replace(pDevice->m_sMacAddress,":","-"));

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting net boot for file %s to %d",sFile.c_str(),(int) bNetBoot);

	if( bNetBoot )
		StringUtils::Replace(sFile,sFile,"LOCALBOOT 0 #ERNEL ","KERNEL ");
	else
		StringUtils::Replace(sFile,sFile,"KERNEL ","LOCALBOOT 0 #ERNEL ");
}

//<-dceag-c365-b->

	/** @brief COMMAND: #365 - Get Room Description */
	/** Given a room or device ID, returns the description.  If device ID, also returns the room ID. */
		/** @param #2 PK_Device */
			/** The ID of the device.  Specify this or the room ID. */
		/** @param #9 Text */
			/** The description of the room */
		/** @param #57 PK_Room */
			/** The ID of the room.  Specify this or the device ID. */

void General_Info_Plugin::CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c365-e->
{
	if( !(*iPK_Room ) )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
		if( !pDevice )
		{
			*sText = "Bad Device/Room";
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Bad Device/room");
			return;
		}
		iPK_Device = pDevice->m_dwPK_Device;
		*iPK_Room = pDevice->m_dwPK_Room;
	}

	Room *pRoom = m_pRouter->m_mapRoom_Find(*iPK_Room);
	if( !pRoom )
	{
		*sText = "Bad Room";
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Bad room");
		return;
	}

	*sText = pRoom->m_sDescription;
}
//<-dceag-c371-b->

	/** @brief COMMAND: #371 - Is Daytime */
	/** Returns true or false to indicate if it is daytime (ie between sunrise and sunset) */
		/** @param #119 True/False */
			/** Returns true if it is daytime. */

void General_Info_Plugin::CMD_Is_Daytime(bool *bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c371-e->
{
	(*bTrueFalse) = m_pEvent_Plugin->IsDaytime();
}


bool General_Info_Plugin::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "General_Info_Plugin::ReportPendingTasks m_mapMediaDirectors_PendingConfig %d %d",m_dwPK_Device, (int) m_mapMediaDirectors_PendingConfig.size());
	bool bPendingTasks=false;
	for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
	{
		if( it->second )
		{
			Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(it->first);
			if( pPendingTaskList )
				pPendingTaskList->m_listPendingTask.push_back(new PendingTask(it->first,m_dwPK_Device,m_dwPK_Device,
					"software_update","Updating software on: " + (pRow_Device ? pRow_Device->Description_get() : StringUtils::itos(it->first)),-1,0,false));

			LoggerWrapper::GetInstance()->Write( LV_STATUS, "General_Info_Plugin::ReportPendingTasks m_mapMediaDirectors_PendingConfig md %d is busy",it->first);
			bPendingTasks=true;
		}
	}
	return bPendingTasks;
}

class DataGridTable *General_Info_Plugin::PendingTasksGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	PLUTO_SAFETY_LOCK(lm,m_pRouter->m_ListenerMutex);
	list<int> DevicesWithPendingTasks;
	ServerSocketMap::iterator iDC;
	for(iDC = m_pRouter->m_mapServerSocket.begin(); iDC!=m_pRouter->m_mapServerSocket.end(); ++iDC)
	{
		ServerSocket *pServerSocket = (*iDC).second;
		if( pServerSocket->m_bImplementsPendingTasks )
			DevicesWithPendingTasks.push_back((*iDC).first);
	}
	lm.Release();

	PendingTaskList pendingTaskList;
	for(list<int>::iterator it=DevicesWithPendingTasks.begin();it!=DevicesWithPendingTasks.end();++it)
		ReportPendingTasksFromDevice(m_pcRequestSocket->m_pClientSocket,m_dwPK_Device,*it,&pendingTaskList);

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	int RowCount=0;
	for(list<PendingTask *>::iterator it=pendingTaskList.m_listPendingTask.begin();it!=pendingTaskList.m_listPendingTask.end();++it)
	{
		PendingTask *pPendingTask = *it;
		pCell = new DataGridCell( pPendingTask->m_sDescription, StringUtils::itos(pPendingTask->m_dwID) );
		pCell->m_mapAttributes["Title"] = pPendingTask->m_sDescription;
		string sStatus;
		if( pPendingTask->m_cPercentComplete>0 )
			sStatus += StringUtils::itos((int) pPendingTask->m_cPercentComplete) + "% ";
		if( pPendingTask->m_dwSecondsLeft )
			sStatus += StringUtils::itos(pPendingTask->m_dwSecondsLeft) + "sec ";

		pCell->m_mapAttributes["Status"] = sStatus;
		if( pPendingTask->m_bCanAbort )
			pCell->m_mapAttributes["Abort"] = "1";
		pCell->m_mapAttributes["PK_Device"] = StringUtils::itos(pPendingTask->m_dwPK_Device_Abort);
	pDataGrid->SetData( 0, RowCount++, pCell );
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::QuickStartApps( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCellIcon,*pCellText;
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);

	int PK_Device_MD=atoi(Parms.c_str());
	DeviceData_Router *pDevice_MD = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_MD);
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( PK_Device_MD );

	if( !pRow_Device || !pDevice_MD )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"QuickStartApps - invalid MD %s",Parms.c_str());
		return pDataGrid;
	}

	/*

	DeviceData_Router *pDevice_AppServer,*pDevice_Orbiter_OSD;
	GetAppServerAndOsdForMD(pDevice_MD,&pDevice_AppServer,&pDevice_Orbiter_OSD);

	if( !pDevice_AppServer || !pDevice_Orbiter_OSD )
	{
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"There are no App Servers/OSD on this media director");
		SCREEN_DialogGenericError SCREEN_DialogGenericError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
			"There are no App Servers/OSD on this media director", "0", "0", "0");
		SendCommand(SCREEN_DialogGenericError);
		return pDataGrid;
	}
*/
	vector<Row_Device_QuickStart *> vectRow_Device_QuickStart;
	list<pair<string, string> > *p_Bookmarks=NULL;
	size_t s=0;
	list<pair<string, string> >::iterator it;
	if( ExtraData )
	{
		p_Bookmarks = (list<pair<string, string> > *) ExtraData;
		it=p_Bookmarks->begin();
		if( it == p_Bookmarks->end() )
			return pDataGrid;
	}
	else
		pRow_Device->Device_QuickStart_FK_Device_getrows(&vectRow_Device_QuickStart);
/*
	LastApplication *pLastApplication = m_mapLastApplication_Find(pDevice_MD->m_dwPK_Device);
	if( pLastApplication && pLastApplication->m_sName.size() )
	{
		int PK_DesignObj_OSD=DESIGNOBJ_generic_app_full_screen_CONST;
		int PK_DesignObj_Remote=DESIGNOBJ_mnuGenericAppController_CONST;
		if( pLastApplication->m_iPK_QuickStartTemplate )
		{
			Row_QuickStartTemplate *pRow_QuickStartTemplate = m_pDatabase_pluto_main->QuickStartTemplate_get()->GetRow(pLastApplication->m_iPK_QuickStartTemplate);
			if( pRow_QuickStartTemplate )
			{
				PK_DesignObj_OSD=pRow_QuickStartTemplate->FK_DesignObj_OSD_get();
				PK_DesignObj_Remote=pRow_QuickStartTemplate->FK_DesignObj_get();;
			}
		}

		string sMessage;

		if( pDevice_Orbiter_OSD->m_dwPK_Device==pMessage->m_dwPK_Device_From )  // We chose this from an OSD -- just goto the app screen
		{
			sMessage = 
				//TODO: replace this with GotoScreen's
				StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pMessage->m_dwPK_Device_From) +
				" 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " " + 
				StringUtils::itos(PK_DesignObj_OSD);
		}
		else  // We chose this from a remote--the remote goes to a remote control screen, and the osd to the app
		{
			sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pMessage->m_dwPK_Device_From) +
				" 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " + 
					StringUtils::ltos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::ltos(SCREEN_GenericAppController_CONST) + 
				+ " & " +
				StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pDevice_Orbiter_OSD->m_dwPK_Device) +
				" 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " " + 
				StringUtils::itos(PK_DesignObj_OSD);
		}
				
		DCE::CMD_Show_Object CMD_Show_Object(m_dwPK_Device,pMessage->m_dwPK_Device_From,StringUtils::itos(DESIGNOBJ_butResumeControl_CONST),
			0,"","","1");
		DCE::CMD_Show_Object CMD_Show_Object2(m_dwPK_Device,pMessage->m_dwPK_Device_From,StringUtils::itos(DESIGNOBJ_objExitAppOnOSD_CONST),
			0,"","","1");
		DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device,pMessage->m_dwPK_Device_From,"",pLastApplication->m_sName,TEXT_STATUS_CONST);
		DCE::CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device,pMessage->m_dwPK_Device_From,VARIABLE_Misc_Data_1_CONST,sMessage);

		CMD_Show_Object.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object2.m_pMessage);
		CMD_Show_Object.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text.m_pMessage);
		CMD_Show_Object.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable.m_pMessage);
		SendCommand(CMD_Show_Object);
	}
*/
	// This will be the template we use for string applications
	Row_QuickStartTemplate *pRow_QuickStartTemplate = p_Bookmarks ? m_pDatabase_pluto_main->QuickStartTemplate_get()->GetRow(7) : NULL; // HACKOMATIC!

	int iRow=0;
	while(true)
	{
		string sDescription,sBinary,sArguments;
		size_t iSize;
		char *pBuffer=NULL;

		if( p_Bookmarks )
		{
			sDescription=it->second;
			sBinary=pRow_QuickStartTemplate ? pRow_QuickStartTemplate->Binary_get() : "/usr/pluto/bin/Mozilla.sh";
			sArguments=StringUtils::itos(pOH_Orbiter && pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_iPK_Users : 0) + "\t" + it->first;
		}
		else
		{
		if(!vectRow_Device_QuickStart.size())
		return pDataGrid;

			Row_Device_QuickStart *pRow_Device_QuickStart = vectRow_Device_QuickStart[s];
			pRow_QuickStartTemplate=pRow_Device_QuickStart->FK_QuickStartTemplate_getrow();
			sDescription = pRow_Device_QuickStart->Description_get();
			sBinary = pRow_QuickStartTemplate ? pRow_QuickStartTemplate->Binary_get() : "";
			sArguments = pRow_QuickStartTemplate ? pRow_QuickStartTemplate->Arguments_get() : "";
		if( pRow_Device_QuickStart->EK_Picture_get() )
		pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Device_QuickStart->EK_Picture_get()) + "_tn.jpg",iSize);
		}

		if( pRow_QuickStartTemplate )
		{
			if( !pBuffer )
		pBuffer = FileUtils::ReadFileIntoBuffer("/home/quick_start_icons/template_" + StringUtils::itos(pRow_QuickStartTemplate->PK_QuickStartTemplate_get()) + ".png", iSize);
		}

		if( sBinary.size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"QuickStart device with no binary to run");

			if((p_Bookmarks && ++it == p_Bookmarks->end()) || (!p_Bookmarks && ++s>=vectRow_Device_QuickStart.size()))
				break;

			continue;
		}

		if( sDescription.size()==0 )
			sDescription = sBinary;

		string sCellValue;
		if( pRow_QuickStartTemplate )
			sCellValue = pRow_QuickStartTemplate->WindowClass_get() + "\t" + StringUtils::itos(pRow_QuickStartTemplate->FK_DesignObj_get()) + "\t" + StringUtils::itos(pRow_QuickStartTemplate->FK_DesignObj_OSD_get()) + "\t";
		else
			sCellValue = "\t\t\t";

		sCellValue += sDescription + "\t" + sBinary + "\t" + sArguments;

		pCellIcon = new DataGridCell( "", sCellValue );
		pCellText = new DataGridCell( sDescription, sCellValue );
		pCellText->m_Colspan=3;
		pDataGrid->SetData( 0, iRow, pCellIcon );
		pDataGrid->SetData( 1, iRow, pCellText );
	if( pBuffer )
	{
		pCellIcon->m_pGraphicData = pBuffer;
		pCellIcon->m_GraphicLength = iSize;
	}
/*
		pCellIcon->m_pMessage = BuildMessageToSpawnApp(m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From),
			pDevice_MD,pDevice_AppServer,pDevice_Orbiter_OSD,
			sBinary,sArguments,sDescription,(pRow_QuickStartTemplate ? pRow_QuickStartTemplate->PK_QuickStartTemplate_get() : 0));
		pCellText->m_pMessage = new Message(pCellIcon->m_pMessage);
*/
		if( (p_Bookmarks && ++it == p_Bookmarks->end()) || 
			(!p_Bookmarks && ++s>=vectRow_Device_QuickStart.size()) )
				break;
		iRow++;
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::MRUDocuments( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
//	DataGridCell *pCell;


	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::TypesOfPhones( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	vector<Row_DeviceTemplate *> vectRow_DeviceTemplate;
	m_pDatabase_pluto_main->DeviceTemplate_get()->GetRows("FK_DeviceCategory="+StringUtils::itos(DEVICECATEGORY_Mobile_Orbiter_CONST),&vectRow_DeviceTemplate);
	for(size_t s=0;s<vectRow_DeviceTemplate.size();++s)
	{
		Row_DeviceTemplate *pRow_DeviceTemplate = vectRow_DeviceTemplate[s];
		pCell = new DataGridCell(pRow_DeviceTemplate->Description_get(),StringUtils::itos(pRow_DeviceTemplate->PK_DeviceTemplate_get()));
		pDataGrid->SetData(0,s,pCell);
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::UsersGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 4;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation="
		+ StringUtils::itos(m_pRouter->iPK_Installation_get()) + " ORDER BY UserName",&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
	{
		Row_Users *pRow_Users = vectRow_Users[s];
		pCell = new DataGridCell(pRow_Users->UserName_get(),StringUtils::itos(pRow_Users->PK_Users_get()));
			pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::DevicesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory IN (" + 
		Parms + ") ORDER BY Device.Description",&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		Row_Room *pRow_Room = pRow_Device->FK_Room_getrow();

		string sDescription = pRow_Device->Description_get();
		if( pRow_Room )
			sDescription += "(" + pRow_Room->Description_get() + ")";

		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
		pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::StorageDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	int iRow=0,iCol=0;

	PlutoSqlResult result;
	DB_ROW row;
	string sSQL = 
		"SELECT PK_Device, Device.Description, Device_DeviceData.IK_DeviceData, FK_DeviceCategory, DirectoryStructure.IK_DeviceData "
		"FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN Device_DeviceData ON FK_Device = PK_Device "
		"LEFT JOIN Device_DeviceData AS DirectoryStructure ON DirectoryStructure.FK_Device = PK_Device AND DirectoryStructure.FK_DeviceData=" TOSTRING(DEVICEDATA_PK_Users_CONST) " "
		"WHERE FK_DeviceCategory IN (" + 
			StringUtils::ltos(DEVICECATEGORY_Core_CONST) + ", " + 
			StringUtils::ltos(DEVICECATEGORY_Hard_Drives_CONST) + ", " + 
			StringUtils::ltos(DEVICECATEGORY_Storage_Devices_CONST) + ", " + 
			StringUtils::ltos(DEVICECATEGORY_Network_Storage_CONST) + ", " + 
			StringUtils::ltos(DEVICECATEGORY_Raid_Devices_CONST) + ", " + 
			StringUtils::ltos(DEVICECATEGORY_Raid_Generic_CONST) +
			+ ") AND Device_DeviceData.FK_DeviceData = " + 
			StringUtils::ltos(DEVICEDATA_Free_Disk_Space_in_MBytes_CONST) + " " +
		"ORDER BY CAST(Device_DeviceData.IK_DeviceData AS UNSIGNED) DESC";

	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL.c_str())) )
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			if(NULL != row[0] && NULL != row[1] && NULL != row[3])
			{
				string sFreeSpace;
				if(0 == row[2] || string(row[2]).empty())
					sFreeSpace = "0";
				else
					sFreeSpace = row[2];

				string sText = string(row[1]) + " (#" + row[0] + ") " + sFreeSpace + "MB";
				string sValue;

				if(atoi(row[3]) == DEVICECATEGORY_Core_CONST)
					sValue = string(row[0]) + "\t" + "/home/";
				else
					sValue = string(row[0]) + "\t" + string("/mnt/device/") + row[0] + "/";

				if( row[4]==NULL || atoi(row[4])==-1 )
					sValue += "\t1";  // Means use the directory structure

				pCell = new DataGridCell(sText, sValue);
				pDataGrid->SetData( iCol++, iRow, pCell );

				if( iCol>=iWidth )
				{
					iCol=0;
					iRow++;
				}
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::DeviceTemplatesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	vector<Row_DeviceTemplate *> vectRow_DeviceTemplate;
	m_pDatabase_pluto_main->DeviceTemplate_get()->GetRows(
		"FK_DeviceCategory = " +  Parms + " ORDER BY Description",
		&vectRow_DeviceTemplate);

	for(size_t s=0;s<vectRow_DeviceTemplate.size();++s)
	{
		Row_DeviceTemplate *pRow_DeviceTemplate = vectRow_DeviceTemplate[s];

		string sDescription = pRow_DeviceTemplate->Description_get();
		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_DeviceTemplate->PK_DeviceTemplate_get()));
		pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::CitiesGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	string::size_type pos=0;
	int PK_Country=atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	Parms = Parms.substr(pos); // Now it's the city name
	int iRow=0,iCol=0;

	if( !PK_Country || Parms.size()==0 )
		return pDataGrid;

	for(int i=0;i<2;++i)
	{
		PlutoSqlResult result;
		DB_ROW row;
		string sSQL;
		// Fields are 0=PK_City,1=PK_PostalCode,2=City,3=Region,4=PostalCode,5=Lat,6=Long,7=time zone
		if( i==0 )
			sSQL = "SELECT PK_City,PK_PostalCode,City.City,Region.Region,PostalCode.PostalCode,City.Latitude,City.Longitude,ZoneName FROM PostalCode "
				"LEFT JOIN City ON FK_City=PK_City "
				"LEFT JOIN Region on FK_Region=PK_Region "
				"LEFT JOIN TimeZone ON PK_TimeZone=FK_TimeZone "
				"WHERE PostalCode.FK_Country=" + StringUtils::itos(PK_Country) + " AND PostalCode.PostalCode LIKE '%" + Parms + "%' "
				"ORDER BY City.City like '" + Parms + "%' desc,City.City,Region.Region";  // Put the cities that start with this string first
		else
			sSQL = "SELECT PK_City,0,City.City,Region.Region,'',City.Latitude,City.Longitude,ZoneName FROM City "
				"LEFT JOIN Region on FK_Region=PK_Region "
				"LEFT JOIN TimeZone ON PK_TimeZone=FK_TimeZone "
				"WHERE City.City LIKE '%" + Parms + "%' "
				"AND City.FK_Country=" + StringUtils::itos(PK_Country) + " " 
				"ORDER BY City.City like '" + Parms + "%' desc,City.City,Region.Region";  // Put the cities that start with this string first

		if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL.c_str())) )
		{
			while( ( row=db_wrapper_fetch_row( result.r ) ) )
			{
				string sName = row[2];
				if( row[3] )
					sName += " (" + (i==0 ? string(row[4]) + ", " : "") + string(row[3]) + ")";
				pCell = new DataGridCell( sName, row[0] + string("\t") + row[1] + "\t" + row[2] + "\t" + (row[3] ? row[3] : "") + "\t" + (row[4] ? row[4] : "") + "\t" + (row[5] ? row[5] : "") + "\t" + (row[6] ? row[6] : "") + "\t" + (row[7] ? row[7] : "") );
				pDataGrid->SetData( iCol++, iRow, pCell );
				if( iCol>=iWidth )
				{
					iCol=0;
					iRow++;
				}
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::CountriesGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int PK_Country=0;
	int iRow=0,iCol=0;
	vector<Row_Country *> vectRow_Country;
	m_pDatabase_pluto_main->Country_get()->GetRows("1=1 ORDER BY Description",&vectRow_Country);
	for(size_t s=0;s<vectRow_Country.size();++s)
	{
		Row_Country *pRow_Country = vectRow_Country[s];
		if( !PK_Country && Parms.size() && StringUtils::StartsWith(pRow_Country->Description_get(),Parms,true) )
			PK_Country = pRow_Country->PK_Country_get();

		pCell = new DataGridCell(pRow_Country->Description_get(),StringUtils::itos(pRow_Country->PK_Country_get()));
			pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	if( PK_Country )
		*sValue_To_Assign = StringUtils::itos(PK_Country);

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::Rooms( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 4;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	string sql;
	map<int,string> mapRoomsWithMDs;
	
	if( Parms.empty() || (Parms[0]!='1' && Parms[0]!='2') )
		sql = "SELECT PK_Room,Description FROM Room WHERE FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " ORDER BY Description";
	else if( Parms[0]=='1' )
		sql = "SELECT DISTINCT PK_Room,Room.Description FROM Room JOIN Device ON FK_Room=PK_Room JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Media_Director_CONST) " AND Room.FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " ORDER BY Description";
	else
	{
		// Filter out rooms with media directors
		string sRoomsWithMDs;
		string sSQL2 = "SELECT FK_Room,PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Media_Director_CONST);
		PlutoSqlResult result;
		DB_ROW row;
		if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL2.c_str())) )
		{
			while( ( row=db_wrapper_fetch_row( result.r ) ) )
			{
				if( row[0] )
					mapRoomsWithMDs[ atoi(row[0]) ]=row[1];
			}
		}
		sql = "SELECT PK_Room,Description FROM Room WHERE FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " ORDER BY Description";
	}

	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sql.c_str())) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell( row[1], row[0] );
			if( mapRoomsWithMDs.find( atoi( row[0] ) )!=mapRoomsWithMDs.end() )
			{
				pCell->m_mapAttributes["PK_Device"] = mapRoomsWithMDs[ atoi( row[0] ) ];
				pCell->m_AltColor = PlutoColor::Gray().AsARGB();
			}

			pDataGrid->SetData( iCol++, iRow, pCell );
			if( iCol>=iWidth )
			{
				iCol=0;
				iRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::RoomTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0, iCol=0;
	string sql = 
		"SELECT PK_RoomType, RoomType.Description, Sum(IF(NOT IFNULL(FK_Installation, 0), 0, 1)) As Count "
		"FROM RoomType LEFT JOIN Room ON FK_RoomType = PK_RoomType "
		"WHERE FK_Installation = " + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " OR FK_Installation IS NULL " 
		"GROUP BY PK_RoomType "
		"ORDER BY Description";
	PlutoSqlResult result;
	DB_ROW row;
	if((result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sql.c_str())))
	{
		while((row=db_wrapper_fetch_row(result.r)))
		{
			string sPK_RoomType = row[0];
			string sDescription = row[1];
			string sNumberOfRoomTypes = row[2];

			pCell = new DataGridCell(sDescription, sPK_RoomType);
			pDataGrid->SetData(0, iRow, pCell);

			pCell = new DataGridCell(sNumberOfRoomTypes, sPK_RoomType);
			pDataGrid->SetData(1, iRow++, pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::ChildrenInfo(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	string::size_type pos=0;
	string sPK_Device_Parent = StringUtils::Tokenize(Parms,",",pos);

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0;
	string sql = 
		"SELECT PK_Device, Device.Description AS Description, Room.Description As Room, DeviceTemplate.Description As ChildType "
		"FROM Device "
		"LEFT JOIN Room ON FK_Room = PK_Room "
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE FK_Device_ControlledVia = " + sPK_Device_Parent;
	PlutoSqlResult result;
	DB_ROW row;
	if((result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sql.c_str())))
	{
		while((row=db_wrapper_fetch_row(result.r)))
		{
			string PK_ChildDevice = row[0];
			string sChildName = row[1];
			string sRoomName = row[2] ? row[2] : "";
			string sChildType = row[3];

			pCell = new DataGridCell(sChildName, PK_ChildDevice);
			pDataGrid->SetData(0, iRow, pCell);

			pCell = new DataGridCell(sRoomName, PK_ChildDevice);
			pDataGrid->SetData(1, iRow, pCell);

			pCell = new DataGridCell(sChildType, PK_ChildDevice);
			pDataGrid->SetData(2, iRow++, pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AvailableSerialPorts(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 3;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	vector< pair<int,string> > vectAllPorts;
	DatabaseUtils::GetUnusedPortsOnAllPCs(m_pDatabase_pluto_main,vectAllPorts,m_pRouter->iPK_Installation_get());

	int iRow=0, iCol=0;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::AvailableSerialPorts - found %d ports",(int) vectAllPorts.size());
	for(size_t s=0;s<vectAllPorts.size();++s)
	{
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(vectAllPorts[s].first);
		if( !pRow_Device )
			continue;

		pCell = new DataGridCell(pRow_Device->Description_get() + " " + DatabaseUtils::HumanReadablePort(m_pDatabase_pluto_main,vectAllPorts[s].first,vectAllPorts[s].second)
			, StringUtils::itos(vectAllPorts[s].first) + "," + vectAllPorts[s].second);
		pDataGrid->SetData(iCol++, iRow, pCell);

		if(iCol >= iWidth)
		{
			iCol = 0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::LightsTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 3;

	string::size_type pos=0;
	string sFloorPlanType = StringUtils::Tokenize(Parms,",",pos);

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	string sql = 
		"SELECT PK_FloorplanObjectType, Description "
		"FROM FloorplanObjectType "
		"WHERE FK_FloorplanType = " + sFloorPlanType + " "
		"ORDER BY Description";

	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sql.c_str())) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell(row[1], row[0]);
			pDataGrid->SetData( iCol++, iRow, pCell );
			if( iCol>=iWidth )
			{
				iCol=0;
				iRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::InstalledAVDevices(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 2;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	string sPK_DeviceCategory_Parent = StringUtils::ltos(DEVICECATEGORY_AV_CONST);
	int iRow=0,iCol=0;
	string sql = 
		"SELECT DISTINCT D.PK_Device, D.Description FROM DeviceTemplate DT "
		"JOIN DeviceCategory DC1 ON DT.FK_DeviceCategory = DC1.PK_DeviceCategory "
		"LEFT JOIN DeviceCategory DC2 ON DC1.PK_DeviceCategory = DC2.FK_DeviceCategory_Parent "
		"JOIN Device D ON D.FK_DeviceTemplate = DT.PK_DeviceTemplate "
		"WHERE "
			"(DT.FK_DeviceCategory = " + sPK_DeviceCategory_Parent + " OR "
			"DC1.FK_DeviceCategory_Parent = " + sPK_DeviceCategory_Parent + " OR "
			"DC2.FK_DeviceCategory_Parent = " + sPK_DeviceCategory_Parent + ") "
		"AND FK_Device_RouteTo is NULL "  // No embedded devices
		"AND D.FK_Installation = " + StringUtils::itos(m_pRouter->iPK_Installation_get());
	if( Parms.size() )
	{
		string::size_type pos=0;
		string sCategories = StringUtils::Tokenize(Parms,"|",pos);
		string sDevices = StringUtils::Tokenize(Parms,"|",pos);

		if( sDevices.size() )
			sql += " AND D.PK_Device<>" + sDevices;

		if( sCategories.size() )
		{
			string sExcludeCategories = " NOT IN (" + sCategories + ")";
			sql += " AND (DT.FK_DeviceCategory IS NULL OR DT.FK_DeviceCategory " + sExcludeCategories + ") " + 
			"AND (DC1.FK_DeviceCategory_Parent IS NULL OR DC1.FK_DeviceCategory_Parent " + sExcludeCategories + ") " + 
			"AND (DC2.FK_DeviceCategory_Parent IS NULL OR DC2.FK_DeviceCategory_Parent " + sExcludeCategories + ") ";
		}
	}

	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sql.c_str()) ))
	{
		while((row = db_wrapper_fetch_row( result.r )))
		{
			pCell = new DataGridCell(row[1], row[0]);
			pDataGrid->SetData(iCol++, iRow, pCell );

			if(iCol >= iWidth)
			{
				iCol = 0;
				iRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::SensorType(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	string::size_type pos=0;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0;
	string sql = 
		"SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory = " + 
		StringUtils::ltos(DEVICECATEGORY_Security_Device_CONST);

	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sql.c_str())) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell(row[1], row[0]);
			pDataGrid->SetData(0, iRow++, pCell );
		}
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AddSoftware( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(atoi(Parms.c_str()));
	if( pDevice )
		pDevice = pDevice->GetTopMostDevice();
	if(!pDevice)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::AddSoftware can't find device %s",Parms.c_str());
		return NULL;
	}

	vector<class Row_Device_DeviceData*> vpRows;
	if ( !m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows(" FK_Device="+StringUtils::itos(pDevice->m_dwPK_Device) + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Model_CONST), 
		  &vpRows) || vpRows.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::AddSoftware No Model is set for device %d", pDevice->m_dwPK_Device);
		return NULL;
	}

	string sModel = vpRows[0]->IK_DeviceData_get().c_str();

	if ( StringUtils::StartsWith(sModel, "LMCE_CORE_") )
	{
		sModel = StringUtils::Replace(sModel, "LMCE_CORE_", "");
	}
	else if ( StringUtils::StartsWith(sModel, "LMCE_MD_") )
	{
		sModel = StringUtils::Replace(sModel, "LMCE_MD_", "");
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::AddSoftware unknown Model is set for device %d: %s", 
			pDevice->m_dwPK_Device, sModel.c_str());
		return NULL;
	}


	DataGridTable *pDataGrid = new DataGridTable();

	string sPlutoVersion = dceConfig.m_mapParameters_Find("PlutoVersion");
	
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"General_Info_Plugin::AddSoftware Starting install list for device %d with model %s Version %s", pDevice->m_dwPK_Device, sModel.c_str(),sPlutoVersion.c_str());
#endif

	DataGridCell *pCell;

	// Valid Installation status are: [I]nstalled, [i]nstalling, [R]emoved, [r]emoving

	int iRow=0;
	string sSQL =
		"SELECT PK_Software,PackageName,Title,IconStr,Category,Rating,Description,Status FROM Software_Source "
		"JOIN Software ON Software_Source.FK_Software=PK_Software "
		"LEFT JOIN Software_Device ON Software_Device.FK_Software=PK_Software AND FK_Device=" + StringUtils::itos(pDevice->m_dwPK_Device) + " "
		"WHERE Distro='" + sModel + "' AND Required_Version_Min<='" + sPlutoVersion + "' AND Required_Version_Max>='" + sPlutoVersion + "' "
		"ORDER BY PackageName,Version DESC";

	int PK_Software_Last = 0;
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pRouter->db_wrapper_query_result(sSQL))  )
	{
		while( (row = db_wrapper_fetch_row( result.r ))!=NULL )
		{
			if( !row[0] || atoi(row[0])==PK_Software_Last )
				continue;
			PK_Software_Last = atoi(row[0]);
			pCell = new DataGridCell("",row[0]);
			if( row[3] )
			{
				unsigned long *lengths = db_wrapper_fetch_lengths(result.r);
				char *Data=new char[lengths[3]];
				memcpy(Data,row[3],lengths[3]);
				pCell->SetImage(Data, lengths[3], GR_PNG);
			}
			if( row[2] )
				pCell->m_mapAttributes["Title"] = row[2];
			else if( row[1] )
				pCell->m_mapAttributes["Title"] = row[1];
			if( row[4] )
				pCell->m_mapAttributes["Category"] = row[4];
			if( row[5] )
				pCell->m_mapAttributes["Rating"] = row[5];
			if( row[7] )
				pCell->m_mapAttributes["Installation_status"] = row[7];
			if( row[6] )
				pCell->m_mapAttributes["Description"] = row[6];

			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	return pDataGrid;
}

//AV Wizard - Template settings
class DataGridTable * General_Info_Plugin::AVWhatDelay( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	string sManufacturerId,sTemplateId;
	sTemplateId = Parms;

	string sql = "SELECT IR_PowerDelay,IR_ModeDelay,DigitDelay FROM DeviceTemplate_AV WHERE FK_DeviceTemplate='" + 
		sTemplateId + "'";
	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV WizardAVWhatDelay sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , Parms.c_str() );

	PlutoSqlResult result;
	DB_ROW row;
	string header[]={ 
		"Number of seconds to wait for the device to warm up after sending a code to turn the power on",
		"Number of seconds to wait before sending other codes after changing inputs or modes on this device ",
		"Number of seconds* between commands (up to 3 decimal places) when sending a series of codes, such as a sequence of digits to tune to a channel"};
	int nRow;

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		if( (row = db_wrapper_fetch_row( result.r )) )
		{
			for(nRow=0;nRow<3;nRow++)
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS , "Read AVWhatDelay grid" );
				LoggerWrapper::GetInstance()->Write( LV_STATUS , row[nRow] );

				pCell = new DataGridCell(row[nRow], StringUtils::itos(nRow));
				pDataGrid->SetData(1, nRow, pCell );
			}

			for(nRow=0;nRow<3;nRow++)
			{
				pCell = new DataGridCell(header[nRow], StringUtils::itos(nRow));
				pDataGrid->SetData(0, nRow, pCell );
			}
		}
		else
			LoggerWrapper::GetInstance()->Write( LV_STATUS , "Couldn't read the AVWhatDelay grid " );
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS , "Couldn't read the AVWhatDelay grid " );

	return pDataGrid;
}
/*
class DataGridTable *General_Info_Plugin::AVDiscret( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	//input parameters
	string sManufacturerId,sTemplateId,sDeviceCategory,sCommands;	  
	string sInfraredGrupIds; 
	string sql,index;
	string::size_type pos=0;

	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0,nPos = 0 ;

	sManufacturerId = StringUtils::Tokenize( Parms, ",", pos );
	sTemplateId = StringUtils::Tokenize( Parms, ",", pos );
	sDeviceCategory = StringUtils::Tokenize( Parms, ",", pos );
	sCommands = StringUtils::Tokenize( Parms, ",", pos );
	sDeviceCategory = "77"; //temporary

	// construct Infrared Group
	sql = "SELECT PK_InfraredGroup FROM InfraredGroup WHERE FK_Manufacturer=";
	sql += sManufacturerId + " " + "AND FK_DeviceCategory='" + sDeviceCategory + "'";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVDiscret sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			sInfraredGrupIds += string(row[0]) + ",";
		}
	}

	if( !sInfraredGrupIds.empty() ) sInfraredGrupIds[sInfraredGrupIds.size()-1] = ' ';

	sql = "SELECT FK_InfraredGroup, InfraredGroup.Description AS IRG_Name,Command.Description,IRData\
		FROM InfraredGroup_Command INNER JOIN Command ON FK_Command=PK_Command\
		INNER JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup ";
	sql += string( "WHERE (FK_DeviceTemplate IS NULL OR FK_DeviceTemplate=") + sTemplateId + ")" + " ";
	sql += string("AND FK_InfraredGroup IN (") +  sInfraredGrupIds + ") AND FK_Command IN (192, 193)" + " ";
	sql += "AND FK_CommMethod=1 ORDER BY FK_InfraredGroup ASC, FK_Command ASC";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVDiscret sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			if( row[0] )
				index = string(row[0]) + "," + StringUtils::ltos(nPos++);
			else
				index = string("0,") + StringUtils::ltos(nPos++);
			for(int i=0;i<3;i++)
			{
				pCell = new DataGridCell( row[i+1], index);
				pDataGrid->SetData(i, nRow, pCell );
			}
			nRow++;
		}
	}

	return pDataGrid;
}
*/
//AV Wizard - Input type
class DataGridTable *General_Info_Plugin::AVInputNotListed(string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sPKTemplate = Parms;
	string sql;

	sql = "SELECT PK_Command,Description FROM Command where FK_CommandCategory=22";
	// add a filter
	if( !Parms.empty() )
		sql += string( " AND PK_Command IN (" ) + Parms + ")";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVInputNotListed sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );

	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0,nCol = 0,nMaxCol;
	nMaxCol = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());;
	if( nMaxCol <= 0 )
		nMaxCol = 1;;

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
				pCell = new DataGridCell( row[1], row[0]);
				pDataGrid->SetData(nCol++, nRow, pCell );
			if( nCol >= nMaxCol )
			{
				nCol = 0;
				nRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AVMediaType( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sPKTemplate = Parms;
	string sql;

	sql = "SELECT PK_MediaType,Description FROM MediaType WHERE DCEAware=0";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVMediaType sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0,nCol = 0,nMaxCol;
	nMaxCol = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());;
	if( nMaxCol <= 0 )
		nMaxCol = 1;

	pCell = new DataGridCell( "Unknown", "0" );	
	pDataGrid->SetData(0, 0, pCell );
	if( nMaxCol == 1 )
		nRow++;
	else
		nCol++;
	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
				pCell = new DataGridCell( row[1], row[0]);
				pDataGrid->SetData(nCol++, nRow, pCell );

				if( nCol >= nMaxCol )
				{
					nCol = 0;
					nRow++;
				}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AVMediaConnector( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sPKTemplate = Parms;
	string sql;

	sql = "SELECT PK_ConnectorType,Description FROM ConnectorType";
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVMediaConnector sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0,nCol = 0,nMaxCol;
	nMaxCol = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());;
	if( nMaxCol <= 0 )
		nMaxCol = 1;

	pCell = new DataGridCell( "Unknown", "0" );	
	pDataGrid->SetData(0, 0, pCell );
	if( nMaxCol == 1 )
		nRow++;
	else
		nCol++;
	
	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			pCell = new DataGridCell( row[1], row[0]);
			pDataGrid->SetData(nCol++, nRow, pCell );
			if( nCol >= nMaxCol )
			{
				nCol = 0;
				nRow++;
			}
		}
	}			

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AVInputsAvaible( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sTemplateId = Parms;
	string sql,index;

	sql = "SELECT PK_Command,OrderNo,Description FROM Command,DeviceTemplate_Input WHERE PK_Command=FK_Command\
		  AND FK_CommandCategory=22 AND FK_Devicetemplate=";
	sql += sTemplateId + " " + "ORDER BY OrderNo";
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVInputsAvaible sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0;
		
	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			if( row[1] )
				index = string(row[0]) + "," + row[1];
			else
				index = string(row[0]) + ",0";
			if( row[2] )
				pCell = new DataGridCell( row[2], index );
			else
				pCell = new DataGridCell( "NULL", index );
			pDataGrid->SetData(0, nRow++, pCell );
		}
	}			

	return pDataGrid;
}

//AV Wizard - DSP Mode
class DataGridTable *General_Info_Plugin::AVDSPMode( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sPKTemplate = Parms;
	string sql,index;

	sql = "SELECT PK_Command,DeviceTemplate_DSPMode.FK_Command,Command.Description as DSPMode_Desc\
	  FROM Command\
	  LEFT JOIN DeviceTemplate_DSPMode ON PK_Command = FK_Command AND FK_DeviceTemplate='";
	sql += Parms + "' " + "WHERE FK_CommandCategory=21 ORDER BY DSPMode_Desc ASC";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVDSPMode sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0,nCol = 0,nMaxCol;
	nMaxCol = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());;
	if( nMaxCol <= 0 )
		nMaxCol = 1;

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
				if( row[1] )
					index = string(row[0]) + "," + row[1];
				else
					index = string(row[0]) + ",0";

				if( row[2] )
					pCell = new DataGridCell( row[2], index );
				else
					pCell = new DataGridCell( "NULL", index );
				pDataGrid->SetData(nCol++, nRow, pCell );
			if( nCol >= nMaxCol )
			{
				nCol = 0;
				nRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AVDSPModeOrder( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sTemplateId = Parms;
	string sql,index;
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0;

	sql = string( "SELECT PK_Command,Command.Description,DeviceTemplate_DSPMode.OrderNo as DSPMode_Desc \
		FROM Command JOIN DeviceTemplate_DSPMode ON PK_Command = FK_Command AND FK_DeviceTemplate='" );
	sql +=	sTemplateId + "' " + "WHERE FK_CommandCategory=21 ORDER BY DSPMode_Desc ASC";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVDSPModeOrder sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			index = string(row[0]) + "," + StringUtils::ltos(nRow);
			if( row[1] )
				pCell = new DataGridCell( row[1], index );
			else
				pCell = new DataGridCell( "NULL", index );
			pDataGrid->SetData(0, nRow++, pCell );
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AVIRCodesSets( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sManufacturerId,sDeviceCategory;
	string sql,index;
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0;

	sManufacturerId = StringUtils::Tokenize(Parms, ",", pos);
	sDeviceCategory = StringUtils::Tokenize(Parms, ",", pos);
	if( sManufacturerId.empty() || sDeviceCategory.empty() )
	{
		pCell = new DataGridCell( "Couldn't read parameters", "0" );
		pDataGrid->SetData(0, 0, pCell );
		return pDataGrid;
	}

	sql = "SELECT PK_InfraredGroup,Description FROM InfraredGroup WHERE ";
	sql += "FK_Manufacturer=" + sManufacturerId + " " + "AND FK_DeviceCategory=" + sDeviceCategory;

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVIRCodesSets sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			index = string(row[0]) + "," + StringUtils::ltos(nRow);
			if( row[1] && row[1] )
				pCell = new DataGridCell( row[1], row[0] );
			else
				pCell = new DataGridCell( "NULL", "0" );
			pDataGrid->SetData(0, nRow++, pCell );
		}
	}

	return pDataGrid;
}

// get the IRCommands for an IR Groups
class DataGridTable *General_Info_Plugin::IRCommands( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos = 0;
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string sIRGroupId;
	string sql,index;
	PlutoSqlResult result;
	DB_ROW row;
	int nRow = 0;

	sIRGroupId = StringUtils::Tokenize(Parms, ",", pos);
	if( sIRGroupId.empty() )
	{
		pCell = new DataGridCell( "Couldn't read parameters", "0" );
		pDataGrid->SetData(0, 0, pCell );
		return pDataGrid;
	}

	sql = "SELECT PK_Command,Description,FK_InfraredGroup,IRData FROM Command,InfraredGroup_Command\
		  WHERE PK_Command=FK_Command AND FK_InfraredGroup='";
	sql += sIRGroupId + "'";

	LoggerWrapper::GetInstance()->Write( LV_STATUS , "AV Wizard AVIRCodesSets sql" );
	LoggerWrapper::GetInstance()->Write( LV_STATUS , sql.c_str() );

	if( (result.r = m_pRouter->db_wrapper_query_result(sql))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			index = string(row[0]) + "," + StringUtils::ltos(nRow);
			if( row[2] && row[1] )
				pCell = new DataGridCell( row[2], row[0] );
			else
				pCell = new DataGridCell( "NULL", "0" );
			pDataGrid->SetData(0, nRow++, pCell );
		}
	}


	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::Discs( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );

	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Disc_Drives_CONST);
	if( !pListDeviceData_Router )
		return pDataGrid;

	DataGridCell *pCell;
	int iRow=0;
	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();
		it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;

		if( pDevice->m_pDevice_ControlledVia && 
			pDevice->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_CDDVD_Jukeboxes_CONST) )
				continue; // Ignore disks in jukeboxes, since they're handled separately

//		if( !m_pRouter->DeviceIsRegistered(pDevice->m_dwPK_Device) )
//			continue; // skip devices that aren't online
		pCell = new DataGridCell(pDevice->m_sDescription,StringUtils::itos(pDevice->m_dwPK_Device));
		if( pDevice->m_pRoom )
			pCell->m_mapAttributes["Room"]=pDevice->m_pRoom->m_sDescription;

		vector<Row_DiscLocation *> vectRow_DiscLocation;
		m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(pDevice->m_dwPK_Device),&vectRow_DiscLocation);
		if( vectRow_DiscLocation.size() )
		{
			Row_DiscLocation *pRow_DiscLocation = vectRow_DiscLocation[0];
			pRow_DiscLocation->Reload();
			string sSQL = "JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_Disc=" + StringUtils::itos(pRow_DiscLocation->FK_Disc_get())
				+ " AND FK_AttributeType IN (" TOSTRING(ATTRIBUTETYPE_Album_CONST) "," TOSTRING(ATTRIBUTETYPE_Title_CONST) ")";

			if( pRow_DiscLocation->EK_Device_Ripping_isNull()==false )
			{
				pCell->m_mapAttributes["EK_Device_Ripping"] = StringUtils::itos(pRow_DiscLocation->EK_Device_Ripping_get());
				pCell->m_mapAttributes["RipJob"] = StringUtils::itos(pRow_DiscLocation->RipJob_get());
			}
			pCell->m_mapAttributes["PK_Disc"]=StringUtils::itos(pRow_DiscLocation->FK_Disc_get());
			if( pRow_DiscLocation->Type_get()=="c" )
				pCell->m_mapAttributes["PK_MediaType"] = TOSTRING(MEDIATYPE_pluto_CD_CONST);
			else if( pRow_DiscLocation->Type_get()=="d" )
				pCell->m_mapAttributes["PK_MediaType"] = TOSTRING(MEDIATYPE_pluto_DVD_CONST);

			Row_Disc *pRow_Disc = pRow_DiscLocation->FK_Disc_getrow();
			if( pRow_Disc )
			{
				pRow_Disc->Reload();
				if( pRow_Disc->FK_File_isNull()==false )
					pCell->m_mapAttributes["PK_File"]=StringUtils::itos(pRow_Disc->FK_File_get());
			}

			vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
			m_pDatabase_pluto_media->Disc_Attribute_get()->GetRows(sSQL,&vectRow_Disc_Attribute);
			if( vectRow_Disc_Attribute.size() )
			{
				Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[0];
				Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
				if( pRow_Attribute )
					pCell->m_mapAttributes["Disc"]=pRow_Attribute->Name_get();
			}
		}
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::JukeBoxes( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );

	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_CDDVD_Jukeboxes_CONST);
	if( !pListDeviceData_Router )
		return pDataGrid;

	DataGridCell *pCell;
	int iRow=0;
	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();
		it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
		pCell = new DataGridCell(pDevice->m_sDescription,StringUtils::itos(pDevice->m_dwPK_Device));
		pCell->m_mapAttributes["Room"]=pDevice->m_pRoom ? pDevice->m_pRoom->m_sDescription : "";
		pCell->m_mapAttributes["Name"]=pDevice->m_sDescription;
		pDataGrid->SetData(0,iRow++,pCell);
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::HardDiscs( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	int iRow=0;

	// Both internal and network drives
	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Hard_Drives_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();
			it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDevice = *it;
			pCell = new DataGridCell(pDevice->m_sDescription,StringUtils::itos(pDevice->m_dwPK_Device));
			pCell->m_mapAttributes["Name"]=pDevice->m_sDescription;
			pCell->m_mapAttributes["Capacity"]=pDevice->m_mapParameters_Find(DEVICEDATA_Capacity_CONST);
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Network_Storage_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();
			it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDevice = *it;
			pCell = new DataGridCell(pDevice->m_sDescription,StringUtils::itos(pDevice->m_dwPK_Device));
			pCell->m_mapAttributes["Name"]=pDevice->m_sDescription;
			pCell->m_mapAttributes["Capacity"]=pDevice->m_mapParameters_Find(DEVICEDATA_Capacity_CONST);
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::CompactFlashes( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Flash_Memory_CONST);
	if( !pListDeviceData_Router )
		return pDataGrid;

	DataGridCell *pCell;
	int iRow=0;
	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();
		it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
		pCell = new DataGridCell(pDevice->m_sDescription,StringUtils::itos(pDevice->m_dwPK_Device));
		pCell->m_mapAttributes["Name"]=pDevice->m_sDescription;
		pCell->m_mapAttributes["Capacity"]=pDevice->m_mapParameters_Find(DEVICEDATA_Capacity_CONST);
		pDataGrid->SetData(0,iRow++,pCell);
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::NetworkStorage( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Network_Storage_CONST);
	if( !pListDeviceData_Router )
		return pDataGrid;

	DataGridCell *pCell;
	int iRow=0;
	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();
		it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
		pCell = new DataGridCell(pDevice->m_sDescription,StringUtils::itos(pDevice->m_dwPK_Device));
		pDataGrid->SetData(0,iRow++,pCell);
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::JukeboxDrives( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );

	// The Jukebox is the parms, the drives are the children
	DeviceData_Router *pDevice_Jukebox = m_pRouter->m_mapDeviceData_Router_Find( atoi(Parms.c_str()) );
	if( !pDevice_Jukebox )
		return pDataGrid;  // Shouldn't happen

	DataGridCell *pCell;
	int iRow=0;
	for(VectDeviceData_Impl::iterator it=pDevice_Jukebox->m_vectDeviceData_Impl_Children.begin();
		it!=pDevice_Jukebox->m_vectDeviceData_Impl_Children.end();++it)
	{
		DeviceData_Router *pDevice_Drive = (DeviceData_Router *) *it;
		string sPort = pDevice_Drive->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
		vector<Row_DiscLocation *> vectRow_DiscLocation;
		m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(pDevice_Drive->m_dwPK_Device),&vectRow_DiscLocation);
		if( vectRow_DiscLocation.size() )
		{
			Row_DiscLocation *pRow_DiscLocation = vectRow_DiscLocation[0];
			pRow_DiscLocation->Reload();

			Row_Disc *pRow_Disc = pRow_DiscLocation->FK_Disc_getrow();

			pCell = new DataGridCell("# " + StringUtils::itos(pDevice_Drive->m_dwPK_Device),sPort);

			pCell->m_mapAttributes["PK_Device"] = StringUtils::itos(pDevice_Drive->m_dwPK_Device);
			pCell->m_mapAttributes["DeviceDescription"] = pDevice_Drive->m_sDescription;
			pCell->m_mapAttributes["ID"] = sPort;

			if( pRow_DiscLocation->EK_Device_Ripping_isNull()==false )
			{
				pCell->m_mapAttributes["EK_Device_Ripping"] = StringUtils::itos(pRow_DiscLocation->EK_Device_Ripping_get());
				pCell->m_mapAttributes["RipJob"] = StringUtils::itos(pRow_DiscLocation->RipJob_get());
			}

			if( pRow_Disc )
			{
				pRow_Disc->Reload();
				pCell->m_mapAttributes["PK_Disc"] = StringUtils::itos(pRow_Disc->PK_Disc_get());

				if( pRow_Disc->FK_File_isNull()==false )
					pCell->m_mapAttributes["PK_File"]=StringUtils::itos(pRow_Disc->FK_File_get());

				string sSQL = "JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_Disc=" + StringUtils::itos(pRow_Disc->PK_Disc_get())
					+ " AND FK_AttributeType IN (" TOSTRING(ATTRIBUTETYPE_Album_CONST) "," TOSTRING(ATTRIBUTETYPE_Title_CONST) ")";

				vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
				m_pDatabase_pluto_media->Disc_Attribute_get()->GetRows(sSQL,&vectRow_Disc_Attribute);
				if( vectRow_Disc_Attribute.size() )
				{
					Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[0];
					Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
					if( pRow_Attribute )
						pCell->m_mapAttributes["Disc"]=pRow_Attribute->Name_get();
				}
			}
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::JukeboxSlots( string GridID, string Parms, void *ExtraData, 
	int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );

	// The Jukebox is the parms, the drives are the children
	DeviceData_Router *pDevice_Jukebox = m_pRouter->m_mapDeviceData_Router_Find( atoi(Parms.c_str()) );
	if( !pDevice_Jukebox )
		return pDataGrid;  // Shouldn't happen

	DataGridCell *pCell;
	int iRow=0;

	vector<Row_DiscLocation *> vectRow_DiscLocation;
	m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(pDevice_Jukebox->m_dwPK_Device),&vectRow_DiscLocation);
	for(vector<Row_DiscLocation *>::iterator it=vectRow_DiscLocation.begin();it!=vectRow_DiscLocation.end();++it)
	{
		Row_DiscLocation *pRow_DiscLocation = *it;
		pRow_DiscLocation->Reload();
		Row_Disc *pRow_Disc = pRow_DiscLocation->FK_Disc_getrow();
		pCell = new DataGridCell(StringUtils::itos(pRow_DiscLocation->Slot_get()),pRow_Disc ? StringUtils::itos(pRow_Disc->PK_Disc_get()) : "");
		pCell->m_mapAttributes["PK_Device"] = StringUtils::itos(pRow_DiscLocation->EK_Device_get());
		pCell->m_mapAttributes["Slot"] = StringUtils::itos(pRow_DiscLocation->Slot_get()) + " " + pRow_DiscLocation->Type_get();

		if( pRow_DiscLocation->EK_Device_Ripping_isNull()==false )
		{
			pCell->m_mapAttributes["EK_Device_Ripping"] = StringUtils::itos(pRow_DiscLocation->EK_Device_Ripping_get());
			pCell->m_mapAttributes["RipJob"] = StringUtils::itos(pRow_DiscLocation->RipJob_get());
		}
		if( pRow_DiscLocation->Type_get()=="c" )
			pCell->m_mapAttributes["PK_MediaType"] = TOSTRING(MEDIATYPE_pluto_CD_CONST);
		else if( pRow_DiscLocation->Type_get()=="d" )
			pCell->m_mapAttributes["PK_MediaType"] = TOSTRING(MEDIATYPE_pluto_DVD_CONST);

		if( pRow_Disc )
		{
			pRow_Disc->Reload();
			pCell->m_mapAttributes["PK_Disc"] = StringUtils::itos(pRow_Disc->PK_Disc_get());

			if( pRow_Disc->FK_File_isNull()==false )
				pCell->m_mapAttributes["PK_File"]=StringUtils::itos(pRow_Disc->FK_File_get());

			string sSQL = "JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_Disc=" + StringUtils::itos(pRow_Disc->PK_Disc_get())
				+ " AND FK_AttributeType IN (" TOSTRING(ATTRIBUTETYPE_Album_CONST) "," TOSTRING(ATTRIBUTETYPE_Title_CONST) ")";

			vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
			m_pDatabase_pluto_media->Disc_Attribute_get()->GetRows(sSQL,&vectRow_Disc_Attribute);
			if( vectRow_Disc_Attribute.size() )
			{
				Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[0];
				Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
				if( pRow_Attribute )
					pCell->m_mapAttributes["Disc"]=pRow_Attribute->Name_get();
			}
		}
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

//<-dceag-c395-b->

	/** @brief COMMAND: #395 - Check for updates */
	/** Check all PC's in the system to see if there are available updates on any of them by having all AppServer's run /usr/pluto/bin/Config_Device_Changes.sh */

void General_Info_Plugin::CMD_Check_for_updates(string &sCMD_Result,Message *pMessage)
//<-dceag-c395-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates");
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);

	if( PendingConfigs() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates Schedule a m_bRerunConfigWhenDone");
		m_bRerunConfigWhenDone=true;
		return;
	}

	ListDeviceData_Router *pListDeviceData_Router = 
		m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_App_Server_CONST);

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"General_Info_Plugin::CMD_Check_for_updates launching now General plugin checking for updates %p",pListDeviceData_Router);

	if( !pListDeviceData_Router )
		return;

	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
	
		if(NULL != pDevice->m_pDevice_ControlledVia && !m_mapMediaDirectors_PendingConfig[pDevice->m_pDevice_ControlledVia->m_dwPK_Device] )
		{
			string sResponseCommand = StringUtils::itos(pDevice->m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " +
				StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Check_for_updates_done_CONST);
			string sFailureCommand = sResponseCommand + " " + StringUtils::itos(COMMANDPARAMETER_Failed_CONST) + " 1";
			string sSuccessCommand = sResponseCommand + " " + StringUtils::itos(COMMANDPARAMETER_Failed_CONST) + " 0";

			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice->m_dwPK_Device,"/usr/pluto/bin/Config_Device_Changes.sh","cdc",
				"F\tStartLocalDevice\tAlert",
				sFailureCommand,
				sSuccessCommand,false,false,false,true);
			string sResponse;
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
			if( !SendCommand(CMD_Spawn_Application,&sResponse) || sResponse!="OK" )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"General_Info_Plugin::CMD_Check_for_updates m_mapMediaDirectors_PendingConfig Failed to send spawn application to %d",pDevice->m_dwPK_Device);
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates m_mapMediaDirectors_PendingConfig %d=true",pDevice->m_pDevice_ControlledVia->m_dwPK_Device);
				m_mapMediaDirectors_PendingConfig[pDevice->m_pDevice_ControlledVia->m_dwPK_Device]=true;
			}
		}
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates m_mapMediaDirectors_PendingConfig %d pending configs",(int) m_mapMediaDirectors_PendingConfig.size());
	for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates m_mapMediaDirectors_PendingConfig md %d=%d",it->first,(int) it->second);
#endif
}

//<-dceag-c396-b->

	/** @brief COMMAND: #396 - Check for updates done */
	/** An App Server finished running /usr/pluto/bin/Config_Device_Changes.sh and notifies the g.i. plugin. */
		/** @param #230 Failed */
			/** This is true if the script used to check for updates fails. */

void General_Info_Plugin::CMD_Check_for_updates_done(bool bFailed,string &sCMD_Result,Message *pMessage)
//<-dceag-c396-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates_done m_mapMediaDirectors_PendingConfig from %d",pMessage->m_dwPK_Device_From);
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);

	if(bFailed)
	{
		//TODO : add more stuff here ?
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "General_Info_Plugin::CMD_Check_for_updates_done Config_Device_Changes.sh failed!");
	}

	DeviceData_Router *pDevice_AppServer = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From);
	if( !pDevice_AppServer || !pDevice_AppServer->m_pDevice_ControlledVia )
		return; // Shouldn't happen

	m_mapMediaDirectors_PendingConfig[pDevice_AppServer->m_pDevice_ControlledVia->m_dwPK_Device]=false;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates_done m_mapMediaDirectors_PendingConfig from %d, %d pending configs",pMessage->m_dwPK_Device_From,(int) m_mapMediaDirectors_PendingConfig.size());
	for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates_done m_mapMediaDirectors_PendingConfig md %d=%d",it->first,(int) it->second);
#endif

	if( PendingConfigs() )
		return; // Others are still running

	if( m_bRerunConfigWhenDone )
	{
		m_bRerunConfigWhenDone=false;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"New requests came in the meantime.  Rerun again. m_mapMediaDirectors_PendingConfig");
		CMD_Check_for_updates();  // Do it again
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Done updating config. m_mapMediaDirectors_PendingConfig");
		DoneCheckingForUpdates();
	}
}

class DataGridTable * General_Info_Plugin::BookmarkList(string GridID, string Parms, void * ExtraData,int *iPK_Variable,string *sValue_To_Assign, class Message *pMessage)
{
	string::size_type pos=0;
	StringUtils::Tokenize(Parms,",",pos);  // Skip the MD
	list<pair<string, string> > Bookmarks = GetUserBookmarks(StringUtils::Tokenize(Parms,",",pos));
	return QuickStartApps( GridID, Parms, (void *) &Bookmarks, iPK_Variable, sValue_To_Assign, pMessage );
}

pair<string, string> strpair(string x, string y)
{
	return pair<string, string>(x, y);
}

list<pair<string, string> > General_Info_Plugin::GetUserBookmarks(string sPK_User)
{
	list<pair<string, string> > Bookmarks;
	// the following code reads the Mozilla bookmarks

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Reading bookmarks from %s", 
	("/home/user_" + sPK_User + "/bookmarks.html").c_str());

	size_t Size;
	char *Buffer = FileUtils::ReadFileIntoBuffer("/home/user_" + sPK_User + "/bookmarks.html", Size);
	if( Buffer )
	{
	char * BufferTop = Buffer;
	char *BraceA;
	char *PosInBuffer=Buffer;
	while( (BraceA=strstr(Buffer,"<A")) )
	{
		char *HRef = strstr(BraceA,"HREF");
		if( !HRef )
		{
		Buffer++;  // Skip past this and continue
		continue;
		}

		char *FirstQuote = strchr(HRef,'"');
		if( !FirstQuote )
		{
		Buffer++;  // Skip past this and continue
		continue;
		}

		char *SecondQuote = strchr(FirstQuote+1,'"');
		if( !SecondQuote )
		{
		Buffer++;  // Skip past this and continue
		continue;
		}
		*SecondQuote=0;

		string Link(FirstQuote+1);
		char *LastBrace = strchr(SecondQuote+1,'>');
		if( !LastBrace )
		{
		Buffer++;  // Skip past this and continue
		continue;
		}

		char * EndBraceA = strstr(LastBrace+1, "</A>");
		*EndBraceA = 0;
		string LinkText(LastBrace+1);

		Buffer = EndBraceA+1;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"add bookmarks %s / %s",Link.c_str(), LinkText.c_str());
		// Do not touch place: URLs.
		if (Link.find("place:") == string::npos)
			Bookmarks.push_back(pair<string, string>(Link, LinkText));

	}

	delete[] BufferTop;
	}

	if( Bookmarks.size()==0 )
	{
	Bookmarks.push_back(make_pair ("http://dcerouter/lmce-admin","LinuxMCE Admin"));
	//Bookmarks.push_back(make_pair<string,string> ("http://dcerouter/support","Pluto Support"));
	}

	return Bookmarks;
}
//<-dceag-c697-b->

	/** @brief COMMAND: #697 - Set Active Application */
	/** Tell General Info Plugin what computing application is running on an MD */
		/** @param #2 PK_Device */
			/** The media director */
		/** @param #50 Name */
			/** The name of the application */
		/** @param #146 PK_QuickStartTemplate */
			/** The quick start template */
/*
void General_Info_Plugin::CMD_Set_Active_Application(int iPK_Device,string sName,int iPK_QuickStartTemplate,string &sCMD_Result,Message *pMessage)
//<-dceag-c697-e->

{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	LastApplication *pLastApplication = m_mapLastApplication_Find(iPK_Device);
	if( !pLastApplication )
	{
		pLastApplication = new LastApplication();
		m_mapLastApplication[iPK_Device]=pLastApplication;
	}
	pLastApplication->m_sName=sName;
	pLastApplication->m_iPK_QuickStartTemplate=iPK_QuickStartTemplate;
}

void General_Info_Plugin::GetAppServerAndOsdForMD(DeviceData_Router *pDevice_MD,DeviceData_Router **pDevice_AppServer,DeviceData_Router **pDevice_Orbiter_OSD)
{
	pDevice_AppServer = pDevice_MD->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
	pDevice_Orbiter_OSD = pDevice_MD->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Standard_Orbiter_CONST);
}

Message *General_Info_Plugin::BuildMessageToSpawnApp(DeviceData_Router *pDevice_OrbiterRequesting,DeviceData_Router *pDevice_MD,
	DeviceData_Router *pDevice_AppServer,DeviceData_Router *pDevice_Orbiter_OSD,
	string sBinary,string sArguments,string sDescription,int PK_QuickStartTemplate)
{
	int PK_DesignObj_OSD=DESIGNOBJ_generic_app_full_screen_CONST;
	int PK_DesignObj_Remote=DESIGNOBJ_mnuGenericAppController_CONST;
	if( PK_QuickStartTemplate )
	{
		Row_QuickStartTemplate *pRow_QuickStartTemplate = m_pDatabase_pluto_main->QuickStartTemplate_get()->GetRow(PK_QuickStartTemplate);
		if( pRow_QuickStartTemplate )
		{
			if( pRow_QuickStartTemplate->FK_DesignObj_get() )
				PK_DesignObj_Remote=pRow_QuickStartTemplate->FK_DesignObj_get();
			if( pRow_QuickStartTemplate->FK_DesignObj_OSD_get() )
				PK_DesignObj_OSD=pRow_QuickStartTemplate->FK_DesignObj_OSD_get();
			if( sBinary.size()==0 )
				sBinary=pRow_QuickStartTemplate->Binary_get();
			if( sArguments.size()==0 )
				sArguments=pRow_QuickStartTemplate->Arguments_get();
			if( sDescription.size()==0 )
				sDescription=pRow_QuickStartTemplate->Description_get();
		}
	}

	string sMessage = "0 " + StringUtils::itos(pDevice_Orbiter_OSD->m_dwPK_Device) + 
		" 1 4 16 " + StringUtils::itos(PK_DesignObj_OSD) + 
		" & 0 " + StringUtils::itos(m_dwPK_Device) + " 1 " + StringUtils::itos(COMMAND_Set_Active_Application_CONST) +
		" " + StringUtils::itos(COMMANDPARAMETER_Name_CONST) + " \"\" " + StringUtils::itos(COMMANDPARAMETER_PK_Device_CONST) + " " +
		StringUtils::itos(pDevice_MD->m_dwPK_Device);

	if( pDevice_OrbiterRequesting && pDevice_Orbiter_OSD->m_dwPK_Device!=pDevice_OrbiterRequesting->m_dwPK_Device )
		sMessage += " & 0 " + StringUtils::itos(pDevice_OrbiterRequesting->m_dwPK_Device ) + 
		" 1 4 16 " + StringUtils::itos(PK_DesignObj_Remote);

	DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
		sBinary,"generic_app",sArguments,
		sMessage,sMessage,true,false,true);

	if( pDevice_OrbiterRequesting )
	{
		DCE::CMD_Set_Active_Application CMD_Set_Active_Application(pDevice_OrbiterRequesting->m_dwPK_Device,m_dwPK_Device,
			pDevice_MD->m_dwPK_Device,sDescription,PK_QuickStartTemplate);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Active_Application.m_pMessage);
	}

	// If this is the same on screen orbiter on which the app will run, we will send the user 
	// to a screen that retains a small strip at the bottom to terminate the app and return to the orbiter.
	// Otherwise, we will send the OSD to the full screen app, and the orbiter will become a remote control
	if( pDevice_OrbiterRequesting && pDevice_Orbiter_OSD->m_dwPK_Device==pDevice_OrbiterRequesting->m_dwPK_Device )
	{
		// This is the OSD orbiter
		//TODO: replace "mozilla" with custom class name from database
		DCE::CMD_Activate_Window CMD_Activate_Window_(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device, "Firefox-bin");
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Activate_Window_.m_pMessage);

		DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device,0,
			StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj.m_pMessage);

		DCE::CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device,
			VARIABLE_Array_Desc_CONST,sDescription);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable.m_pMessage);
	}
	else
	{
		// Do this on the OSD orbiter
		DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device,0,
			StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj.m_pMessage);

		// Do this on the controlling orbiter
		if( pDevice_OrbiterRequesting )
		{
			//DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj2(m_dwPK_Device,pDevice_OrbiterRequesting->m_dwPK_Device,0,
			//	StringUtils::itos(PK_DesignObj_Remote),"","",false,false);
			SCREEN_GenericAppController SCREEN_GenericAppController(m_dwPK_Device,pDevice_OrbiterRequesting->m_dwPK_Device);
			//CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj2.m_pMessage);
			CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(SCREEN_GenericAppController.m_pMessage);
		}
	}

	// Turn the MD on
	if( pDevice_MD )
	{
		DCE::CMD_On CMD_On(m_dwPK_Device,pDevice_MD->m_dwPK_Device,0,"");
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_On.m_pMessage);
	}
	return CMD_Spawn_Application.m_pMessage;
}
*/

struct ChildDeviceProcessing
{
	string m_sChildren;
	Row_Device *m_pRow_Device;

	ChildDeviceProcessing(Row_Device *pRow_Device,string sChildren) { m_sChildren=sChildren; m_pRow_Device=pRow_Device; }
};

bool General_Info_Plugin::ReportingChildDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sError = pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
	if( sError.size() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ReportingChildDevices Device %d failed to report its children: %s",
			pMessage->m_dwPK_Device_From,sError.c_str());
		return false;
	}

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pMessage->m_dwPK_Device_From);
	if( !pRow_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ReportingChildDevices Device %d is invalid",
			pMessage->m_dwPK_Device_From);
		return false;
	}

	string sChildren = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
	ChildDeviceProcessing *pChildDeviceProcessing = new ChildDeviceProcessing(pRow_Device,sChildren);  // Do this in a separate thread since create_device is slow and we don't want to block teh socket
	m_pAlarmManager->AddRelativeAlarm(1,this,PROCESS_CHILD_DEVICES,(void *) pChildDeviceProcessing);
	return false;
}

void General_Info_Plugin::ReportingChildDevices_Offline( void *pVoid )
{
	ChildDeviceProcessing *pChildDeviceProcessing = (ChildDeviceProcessing *) pVoid;

	map<int,bool> mapCurrentChildren;
	vector<string> vectLines;
	StringUtils::Tokenize(pChildDeviceProcessing->m_sChildren,"\n",vectLines);
	for(size_t s=0;s<vectLines.size();++s)
	{
		// This will add the child device if it doesn't exist
		Row_Device *pRow_Device_Child = ProcessChildDevice(pChildDeviceProcessing->m_pRow_Device,vectLines[s]);
		if( pRow_Device_Child )
			mapCurrentChildren[pRow_Device_Child->PK_Device_get()]=true;
	}

	// See if any child devices have since disappeared
	string sSQL = "FK_Device_ControlledVia=" + StringUtils::itos(pChildDeviceProcessing->m_pRow_Device->PK_Device_get());
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		if( pRow_Device->FK_Device_RouteTo_isNull()==false )
		{
			// Skip embedded children, iterate through their children
			sSQL = "FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get());
			vector<Row_Device *> vectRow_Device_Children;
			m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device_Children);
			for(size_t s=0;s<vectRow_Device_Children.size();++s)
			{
				Row_Device *pRow_Device_Child = vectRow_Device_Children[s];
				if( mapCurrentChildren[pRow_Device_Child->PK_Device_get()]==false )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ReportingChildDevices removing dead embedded device %d %s",
						pRow_Device_Child->PK_Device_get(),pRow_Device_Child->Description_get().c_str());
					CMD_Delete_Device(pRow_Device_Child->PK_Device_get());
				}
			}
			continue; // Don't delete the embedded device
		}

		if( mapCurrentChildren[pRow_Device->PK_Device_get()]==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ReportingChildDevices removing dead device %d %s",
				pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str());
			CMD_Delete_Device(pRow_Device->PK_Device_get());
		}
	}
	m_pDatabase_pluto_main->Device_get()->Commit();
	m_pDatabase_pluto_main->Device_DeviceData_get()->Commit();

	delete pChildDeviceProcessing;
}

bool General_Info_Plugin::LowSystemDiskSpace ( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{  
	SCREEN_PopupMessage_DL SCREEN_PopupMessage_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(),
		"Your system is running low on disk space", // Main message
		"", // Command Line
		"low_disk_space", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack

	SendCommand(SCREEN_PopupMessage_DL);
	return false;
}

// It will be like this:
// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \t [PK_DeviceData] \t [Value] ... \n

Row_Device *General_Info_Plugin::ProcessChildDevice(Row_Device *pRow_Device,string sLine)
{
	string::size_type pos=0;
	string sInternalID = StringUtils::Tokenize(sLine,"\t",pos);
	string sDescription = StringUtils::Tokenize(sLine,"\t",pos);
	string sRoomName = StringUtils::Tokenize(sLine,"\t",pos);
	int PK_DeviceTemplate = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
	string sPK_FloorplanObjectType = StringUtils::Tokenize(sLine,"\t",pos);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ProcessChildDevice: pos=%d, %s", pos, sLine.c_str());
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(PK_DeviceTemplate);
	if( !pRow_DeviceTemplate )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice Line %s malformed",sLine.c_str());
		return NULL;
	}

	// Find the child device with this internal id
	vector<Row_Device *> vectRow_Device_Child;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN Device_DeviceData ON FK_Device=Device.PK_Device "
		" LEFT JOIN Device AS Device_Parent ON Device.FK_Device_ControlledVia = Device_Parent.PK_Device "
		" WHERE (Device.FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get()) + " OR Device_Parent.FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get()) + ") "
		" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) +
		" AND IK_DeviceData='" + StringUtils::SQLEscape(sInternalID) + "'",&vectRow_Device_Child);
	
	bool bCreatedNew=false;
	Row_Device *pRow_Device_Child;
	if( vectRow_Device_Child.size() )
	{
		pRow_Device_Child = vectRow_Device_Child[0];
		pRow_Device_Child->Reload();   // Don't overwrite the room or other data that may already be there
		return pRow_Device_Child; // For the time being, don't do anything because it's resetting the device's psc_mod causing orbiter to report the router needs a reload
	}
	else
	{
		// Create it since it doesn't exist
		int iPK_Device;
		bCreatedNew=true;
		CMD_Create_Device(PK_DeviceTemplate,"",0,"",
			StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID,
			0,pRow_Device->PK_Device_get(),"",0,0,&iPK_Device);
		pRow_Device_Child = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
		if( !pRow_Device_Child )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice failed to create child %d",iPK_Device);
			return NULL;
		}
	}

	// Don't reset the description if it's already there, the user may have overridden the default name
	if( pRow_Device_Child->Description_get().size()==0 )
	{
		if( sDescription.size() )
			pRow_Device_Child->Description_set(sDescription);
		else
			pRow_Device_Child->Description_set(sInternalID);
	}
	else if( bCreatedNew )
	{
		string sOldDescription = pRow_Device_Child->Description_get();
		if( sDescription.size() )
			pRow_Device_Child->Description_set(sOldDescription + " " + sDescription);
		else
			pRow_Device_Child->Description_set(sOldDescription + " " + sInternalID);
	}
	vector<Row_Room *> vectRow_Room;
	m_pDatabase_pluto_main->Room_get()->GetRows("Description like '" + StringUtils::SQLEscape(sRoomName) + "'",&vectRow_Room);
	if( vectRow_Room.size() )
		pRow_Device_Child->FK_Room_set( vectRow_Room[0]->PK_Room_get() );
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d %s in unknown room %s",
			pRow_Device_Child->PK_Device_get(),sDescription.c_str(),sRoomName.c_str());

	pRow_Device_Child->FK_DeviceTemplate_set( PK_DeviceTemplate );

	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device_Child->PK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(pRow_Device_Child->PK_Device_get());
		pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_PK_FloorplanObjectType_CONST);
	}

	pRow_Device_DeviceData->IK_DeviceData_set( sPK_FloorplanObjectType );

	while(pos<sLine.size())
	{
		int PK_DeviceData = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
		string sValue = StringUtils::Tokenize(sLine,"\t",pos);
		if( PK_DeviceData )
			DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device_Child->PK_Device_get(),PK_DeviceData,sValue);
	}

	return pRow_Device_Child;
}

//<-dceag-c718-b->

	/** @brief COMMAND: #718 - Create Device */
	/** Creates a new device of the given template */
		/** @param #2 PK_Device */
			/** The new device number */
		/** @param #44 PK_DeviceTemplate */
			/** The template */
		/** @param #47 Mac address */
			/** The mac address */
		/** @param #57 PK_Room */
			/** The room for the device.  0=no room, -1=ask user */
		/** @param #58 IP Address */
			/** The IP of the device */
		/** @param #109 Data String */
			/** Extra device data to be assigned when creating the device. It should look like this:
devicedata_id1|devicedata_value1|devicedata_id2|devicedata_value2| etc. */
		/** @param #150 PK_DHCPDevice */
			/** Only needed if this is a dhcp pnp device */
		/** @param #156 PK_Device_ControlledVia */
			/** The controlled via */
		/** @param #163 Description */
			/** The name for the device */
		/** @param #198 PK_Orbiter */
			/** The orbiter which should be used to prompt the user for any extra information.  Zero means all orbiters */
		/** @param #201 PK_Device_Related */
			/** Will make the new device relate to this one if possible */

void General_Info_Plugin::CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,string sDescription,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c718-e->
{
	Row_DHCPDevice *pRow_DHCPDevice = NULL;
	if( iPK_DHCPDevice )
	{
		pRow_DHCPDevice = m_pDatabase_pluto_main->DHCPDevice_get()->GetRow(iPK_DHCPDevice);
		if( !iPK_DeviceTemplate )
			iPK_DeviceTemplate = pRow_DHCPDevice->FK_DeviceTemplate_get();
	}

	if( !iPK_DeviceTemplate )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::CMD_Create_Device Device Template not specified");
		return;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device template %d mac %s room %d ip %d data %s",
		iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str());
#endif
	OH_Orbiter *pOH_Orbiter = NULL;
	if( iPK_Orbiter )
		pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(iPK_Orbiter);

	CreateDevice createDevice(m_pRouter->iPK_Installation_get(),m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get());
	createDevice.m_bInstallPackagesInBackground = true;
	*iPK_Device = createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,sDescription,sIP_Address,sMac_address,iPK_Device_ControlledVia,sData_String,iPK_Device_Related,iPK_Room > 0 ? iPK_Room : 0);

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device created %d template %d mac %s room %d ip %d data %s",
			*iPK_Device,iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str());
#endif
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(*iPK_Device);
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(*iPK_Device);;
	if( pRow_Device ) // Should always be there
	{
		if( pDevice && pDevice->m_bDisabled==false )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device %d was already there",pDevice->m_dwPK_Device);
		else
		{
			m_listRow_Device_NewAdditions.push_back(pRow_Device);
			if( iPK_Room==-1 )  // Means prompt, or just auto-assign.  As opposed to 0 which means put in no room
			{
				Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_Device->FK_DeviceTemplate_get(),DEVICEDATA_Autoassign_to_parents_room_CONST);
				if( pRow_DeviceTemplate_DeviceData && atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str()) )
				{
					Row_Device *pRow_Device_ControlledVia = pRow_Device->FK_Device_ControlledVia_getrow();
					if( pRow_Device_ControlledVia )
						pRow_Device->FK_Room_set( iPK_Room=pRow_Device_ControlledVia->FK_Room_get() );
				}
			}
			m_pPostCreateOptions->PostCreateDevice(pRow_Device,pOH_Orbiter);

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created device %d %s",*iPK_Device,pRow_Device->Description_get().c_str());
			CMD_Check_for_updates();

			m_pAlarmManager->AddRelativeAlarm(1,this,UPDATE_ENT_AREA,NULL);

			if( iPK_Room==-1 )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device adding %d to m_listNewPnpDevicesWaitingForARoom size: %d",
					*iPK_Device,(int) m_mapNewPnpDevicesWaitingForARoom.size());
				m_mapNewPnpDevicesWaitingForARoom[*iPK_Device]=iPK_Orbiter;
				ServiceRoomPromptRequests();
			}
			else 
			{
				if( !iPK_Room )
				{
					/*
						AB - 7/20/06  -- This shouldn't be here anymore.  If the child should be automatically set to the parent's room we'll do that with the device data.  Lights, for example, should not
					Row_Device *pRow_Device_ControlledVia = pRow_Device->FK_Device_ControlledVia_getrow();
					if( pRow_Device_ControlledVia )
						iPK_Room = pRow_Device_ControlledVia->FK_Room_get();
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Temp - CreateDevice, room was empty, now it's %d %p %d",
						iPK_Room,pRow_Device_ControlledVia,pRow_Device->FK_Device_ControlledVia_get());
					*/
				}
				Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(iPK_Room);
				if( pRow_Room )
					SetRoomForDevice(pRow_Device,pRow_Room);
			}
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Database reports row as ip %s mac %s",
				pRow_Device->IPaddress_get().c_str(),pRow_Device->MACaddress_get().c_str());

			Message *pMessage_Event = new Message(m_dwPK_Device,DEVICEID_EVENTMANAGER,PRIORITY_NORMAL,MESSAGETYPE_EVENT,EVENT_New_Device_Created_CONST,
				1,EVENTPARAMETER_PK_Device_CONST,StringUtils::itos(*iPK_Device).c_str());
			QueueMessageToRouter(pMessage_Event);
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find %d in database",*iPK_Device);


}

void General_Info_Plugin::ServiceRoomPromptRequests()
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ServiceRoomPromptRequests m_dwPK_Device_Prompting_For_A_Room %d m_tTimePromptedForRoom %d m_mapNewPnpDevicesWaitingForARoom %d",
		m_dwPK_Device_Prompting_For_A_Room,(int) m_tTimePromptedForRoom,(int) m_mapNewPnpDevicesWaitingForARoom.size());
	if( m_dwPK_Device_Prompting_For_A_Room && time(NULL)-m_tTimePromptedForRoom<60 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ServiceRoomPromptRequests keep waiting for %d",m_dwPK_Device_Prompting_For_A_Room);
		return;
	}

	int PK_Device_To_Prompt_For_Room=0;  // The device we want to ask for a room for
	if( m_dwPK_Device_Prompting_For_A_Room )
	{
		PK_Device_To_Prompt_For_Room = m_dwPK_Device_Prompting_For_A_Room;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ServiceRoomPromptRequests user didn't specify room for %d.  Ask again.",m_dwPK_Device_Prompting_For_A_Room);
	}
	else
	{
		if( m_mapNewPnpDevicesWaitingForARoom.size() )
		{
			PK_Device_To_Prompt_For_Room = m_dwPK_Device_Prompting_For_A_Room = m_mapNewPnpDevicesWaitingForARoom.begin()->first;
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ServiceRoomPromptRequests service new request by device %d.  Ask again.",m_dwPK_Device_Prompting_For_A_Room);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::ServiceRoomPromptRequests all done");
			return;
		}
	}

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device_To_Prompt_For_Room);
	if( !pRow_Device )
		return; // Can't happen

	int iPK_Orbiter = m_mapNewPnpDevicesWaitingForARoom[PK_Device_To_Prompt_For_Room];
	m_tTimePromptedForRoom = time(NULL);
	if( iPK_Orbiter )
	{
		DCE::SCREEN_Pick_Room_For_Device SCREEN_Pick_Room_For_Device(m_dwPK_Device,iPK_Orbiter,PK_Device_To_Prompt_For_Room,
			pRow_Device->Description_get(),pRow_Device->FK_DeviceTemplate_getrow()->Comments_get());
		SendCommand(SCREEN_Pick_Room_For_Device);
	}
	else
	{
		DCE::SCREEN_Pick_Room_For_Device_DL SCREEN_Pick_Room_For_Device_DL(m_dwPK_Device,m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,PK_Device_To_Prompt_For_Room,
			pRow_Device->Description_get(),pRow_Device->FK_DeviceTemplate_getrow()->Comments_get());
		SendCommand(SCREEN_Pick_Room_For_Device_DL);
	}
}

//<-dceag-c719-b->

	/** @brief COMMAND: #719 - Delete Device */
	/** Deletes a device */
		/** @param #2 PK_Device */
			/** The device to delete */

void General_Info_Plugin::CMD_Delete_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c719-e->
{
	// First delete any embedded devices
	string sSQL = "SELECT PK_Device FROM Device where FK_Device_RouteTo=" + StringUtils::itos(iPK_Device);
	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=m_pRouter->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
			if( row[0] && atoi(row[0]) )
				CMD_Delete_Device( atoi(row[0]) );
	
	m_pDatabase_pluto_main->threaded_db_wrapper_query("UPDATE Device SET FK_Device_ControlledVia=NULL WHERE FK_Device_ControlledVia=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device WHERE PK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM CommandGroup_Command WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_Command WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_CommandGroup WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_DeviceData WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_DeviceGroup WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_Device_Related WHERE FK_Device=" + StringUtils::itos(iPK_Device) + " OR FK_Device_Related=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_EntertainArea WHERE FK_Device=" + StringUtils::itos(iPK_Device));

	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_HouseMode WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_Orbiter WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_StartupScript WHERE FK_Device=" + StringUtils::itos(iPK_Device));

	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_Users WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Package_Device WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM PaidLicense WHERE FK_Device=" + StringUtils::itos(iPK_Device));

	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM Device_Device_Pipe WHERE FK_Device_From=" + StringUtils::itos(iPK_Device) + " OR FK_Device_To=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_db_wrapper_query("DELETE FROM PaidLicense WHERE FK_Device=" + StringUtils::itos(iPK_Device));
}

//<-dceag-c274-b->

	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device, or if negative, this is a PK_PnpQueue and needs to be forwarded to plug and play plugin */
		/** @param #50 Name */
			/** If PK_Room is empty, a new room with this name will be created */
		/** @param #57 PK_Room */
			/** The room */

void General_Info_Plugin::CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c274-e->
{
	PLUTO_SAFETY_LOCK(mm, m_GipMutex);
	size_t sBefore=m_mapNewPnpDevicesWaitingForARoom.size();

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	Row_Room *pRow_Room=NULL;
	if( iPK_Room )
		pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(iPK_Room);
	if( !pRow_Room )
	{
		if( sName.size()==0 )
		{
			DCE::SCREEN_Pick_Room_For_Device SCREEN_Pick_Room_For_Device(m_dwPK_Device,pMessage->m_dwPK_Device_From,iPK_Device,
				pRow_Device ? pRow_Device->Description_get() : "",
				"That room was invalid.  Please specify the room, or choose 'New Room' and type in the name of the new room");
			SendCommand(SCREEN_Pick_Room_For_Device);
			return;
		}

		vector<Row_Room *> vectRow_Room;
		m_pDatabase_pluto_main->Room_get()->GetRows("Description = '" + StringUtils::SQLEscape(sName) + "'",&vectRow_Room);
		if( vectRow_Room.size() )
		{
			pRow_Room = vectRow_Room[0];
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Set_Room_For_Device found existing room %d",pRow_Room->PK_Room_get());
		}
		else
		{
			pRow_Room = m_pDatabase_pluto_main->Room_get()->AddRow();
			pRow_Room->Description_set(sName);
			pRow_Room->FK_RoomType_set(ROOMTYPE_Miscellaneous_CONST);
			pRow_Room->FK_Installation_set(m_pRouter->iPK_Installation_get());
			m_pDatabase_pluto_main->Room_get()->Commit();
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Set_Room_For_Device added room %p %d %s",pRow_Room,pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
		}
	}

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get(), 
		StringUtils::itos(iPK_Device), SCREEN_Pick_Room_For_Device_CONST);
	SendCommand(CMD_Remove_Screen_From_History_DL);

	if( iPK_Device<0 && pRow_Room )
	{
		// This is for plug and play plugin
		DCE::CMD_Choose_Pnp_Device_Template_DT CMD_Choose_Pnp_Device_Template_DT(m_dwPK_Device,DEVICETEMPLATE_Plug_And_Play_Plugin_CONST,BL_SameHouse,
			pRow_Room->PK_Room_get(),0,iPK_Device*-1);
		SendCommand(CMD_Choose_Pnp_Device_Template_DT);
		return;
	}
	if( !pRow_Device || !pRow_Room )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot set device %d to room %d",iPK_Device,iPK_Room);
		return;
	}
	else
		SetRoomForDevice(pRow_Device,pRow_Room);

	// Fix up any auto-assign child devices
	string sSQL = "UPDATE Device "
		"JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate AND FK_DeviceData=" TOSTRING(DEVICEDATA_Autoassign_to_parents_room_CONST) " "
		"LEFT JOIN Device As Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia "
		"SET Device.FK_Room=Parent.FK_Room "
		"WHERE IK_DeviceData=1";
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);

	map<int,int>::iterator it=m_mapNewPnpDevicesWaitingForARoom.find(pRow_Device->PK_Device_get());
	if( it!=m_mapNewPnpDevicesWaitingForARoom.end() )
		m_mapNewPnpDevicesWaitingForARoom.erase(it);

	if( m_dwPK_Device_Prompting_For_A_Room==iPK_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Set_Room_For_Device -- Device %d == device",iPK_Device);
		m_dwPK_Device_Prompting_For_A_Room=0;
	}

	m_pAlarmManager->AddRelativeAlarm(1,this,UPDATE_ENT_AREA,NULL);

bool bStillRunningConfig = PendingConfigs();
LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Set_Room_For_Device: before %d after %d pending %d",
(int) sBefore,(int) m_mapNewPnpDevicesWaitingForARoom.size(),(int) bStillRunningConfig);
	// If there pnp devices waiting for the room, and we finished specifying the last one, and we're
	// not still getting the software, let the user know his device is done
	if( sBefore && m_mapNewPnpDevicesWaitingForARoom.size()==0 && !bStillRunningConfig )
	{
		if( !m_pOrbiter_Plugin->CheckForNewWizardDevices(NULL) )  // Don't display the 'device is done' if there are still some config settings we need
			PromptUserToReloadAfterNewDevices();
	}
	else if( m_mapNewPnpDevicesWaitingForARoom.size()>0 )
		ServiceRoomPromptRequests();
}

void General_Info_Plugin::DoneCheckingForUpdates()
{
	PLUTO_SAFETY_LOCK(mm, m_GipMutex);

	bool bStillRunningConfig = PendingConfigs();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"DoneCheckingForUpdates: %d pending %d",
		(int) m_mapNewPnpDevicesWaitingForARoom.size(),(int) bStillRunningConfig);

	// We must have started the check for updates because we added a new device.  However we finished
	// getting room info from the user, so he's ready to go
	if( m_mapNewPnpDevicesWaitingForARoom.size()==0 && !m_pOrbiter_Plugin->CheckForNewWizardDevices(NULL) )
		PromptUserToReloadAfterNewDevices();
	else if( m_mapNewPnpDevicesWaitingForARoom.size()>0 )
		ServiceRoomPromptRequests();
}


//<-dceag-c752-b->

	/** @brief COMMAND: #752 - Check Mounts */
	/** Re-mount all network storage devices */

void General_Info_Plugin::CMD_Check_Mounts(string &sCMD_Result,Message *pMessage)
//<-dceag-c752-e->
{
	ListDeviceData_Router *pListDeviceData_Router = 
		m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_App_Server_CONST);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"General_Info_Plugin::CMD_Check_Mounts %p",pListDeviceData_Router);
#endif

	if( !pListDeviceData_Router )
		return;

	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
		string sResponse;
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice->m_dwPK_Device,"/usr/pluto/bin/DoAllMounts.sh","dm",
			"","","",false,false,false,true);
		CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
		SendCommand(CMD_Spawn_Application);
	}
}

void General_Info_Plugin::SetRoomForDevice(Row_Device *pRow_Device,Row_Room *pRow_Room)
{
	pRow_Device->Reload();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting device %d to with ip %s mac %s to room %d",
		pRow_Device->PK_Device_get(),pRow_Device->IPaddress_get().c_str(),pRow_Device->MACaddress_get().c_str(),pRow_Room->PK_Room_get());
	pRow_Device->FK_Room_set( pRow_Room->PK_Room_get() );
	pRow_Device->Table_Device_get()->Commit();

	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device_Child = vectRow_Device[s];
		SetRoomForDevice(pRow_Device_Child,pRow_Room);
	}
}
//<-dceag-c765-b->

	/** @brief COMMAND: #765 - Set Device Relations */
	/** Set which devices are related to which */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #103 List PK Device */
			/** The devices it relates to */
		/** @param #205 Reverse */
			/** If true, the device list are the source devices and the PK_Device is the relation */

void General_Info_Plugin::CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse,string &sCMD_Result,Message *pMessage)
//<-dceag-c765-e->
{
	string::size_type pos=0;
	while(pos<sList_PK_Device.size())
	{
		int PK_Device,PK_Device_Related;
		if( bReverse )
		{
			PK_Device = atoi(StringUtils::Tokenize(sList_PK_Device,"|",pos).c_str());
			PK_Device_Related = iPK_Device;
		}
		else
		{
			PK_Device_Related = atoi(StringUtils::Tokenize(sList_PK_Device,"|",pos).c_str());
			PK_Device = iPK_Device;
		}
		if( !PK_Device || !PK_Device_Related )
			continue;

		Row_Device_Device_Related *pRow_Device_Device_Related = m_pDatabase_pluto_main->Device_Device_Related_get()->GetRow(PK_Device,PK_Device_Related);
		if( pRow_Device_Device_Related )
			continue;  // It's already there

		pRow_Device_Device_Related = m_pDatabase_pluto_main->Device_Device_Related_get()->AddRow();
		pRow_Device_Device_Related->FK_Device_set(PK_Device);
		pRow_Device_Device_Related->FK_Device_Related_set(PK_Device_Related);
	}
	m_pDatabase_pluto_main->Device_Device_Related_get()->Commit();
}

//<-dceag-c789-b->

	/** @brief COMMAND: #789 - Force Update Packages */
	/** For the system to upgrade */

void General_Info_Plugin::CMD_Force_Update_Packages(string &sCMD_Result,Message *pMessage)
//<-dceag-c789-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Forcing package update");
	char * args[] = { (char *)("/usr/pluto/bin/ForceUpdates.sh"), NULL };
	ProcessUtils::SpawnDaemon(args[0], args);
}

//<-dceag-c791-b->

	/** @brief COMMAND: #791 - Set Enable Status */
	/** This command will enable or disable a device. A reload router will be needed in order to take effect. */
		/** @param #2 PK_Device */
			/** The id of the device to be enabled/disabled */
		/** @param #208 Enable */
			/** If this is true ( = 1), the device will be enabled. Otherwise, the device will be disabled. */

void General_Info_Plugin::CMD_Set_Enable_Status(int iPK_Device,bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c791-e->
{
	//get the device row from 'Device' table
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);

	if(NULL != pRow_Device)
	{
		//the device exists; setting 'Disabled' flag
		pRow_Device->Reload();
		pRow_Device->Disabled_set(bEnable ? 0 : 1);
		pRow_Device->Table_Device_get()->Commit();

		// This has been commented out on the grounds that I think it is RETARDED. -tschak
//		// If it's a serial device, remove the com port
//		string sSQL = "UPDATE Device_DeviceData SET IK_DeviceData=NULL WHERE FK_Device=" + StringUtils::itos(iPK_Device) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_COM_Port_on_PC_CONST);
//		m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL,true);
	}
	else
	{
		//no row, no valid device id; are you missing something ? 
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to set enable status for device %d: the device doesn't exists",
			iPK_Device);
	}
}

//<-dceag-c800-b->

	/** @brief COMMAND: #800 - InitAVDeviceTemplateSettings */
	/** Save default values in database for a deviceTemplate */
		/** @param #44 PK_DeviceTemplate */
			/** Template id */

void General_Info_Plugin::CMD_InitAVDeviceTemplateSettings(int iPK_DeviceTemplate,string &sCMD_Result,Message *pMessage)
//<-dceag-c800-e->
{
}
//<-dceag-c802-b->

	/** @brief COMMAND: #802 - Get Available Storage Device */
	/** The the available storage devices */
		/** @param #2 PK_Device */
			/** The device id for the NAS or internal drive being used.  If it will be stored on the Core's internal home directory, this will be the device id for the Core.  0 is no device can save the file. */
		/** @param #163 Description */
			/** The descripition for the device being used for the storage (ie Core, or a device name). */
		/** @param #219 Path */
			/** Returns the fully qualified path to prepend to the filename.  If this is empty, that means there is no device which can save this file */
		/** @param #222 Size */
			/** The expected size of the file in MB.  If specified, only a device with enough space and which can handle it (ie some NAS don't do big files).  If not specified, returns device with the most free space. */

void General_Info_Plugin::CMD_Get_Available_Storage_Device(int iSize,int *iPK_Device,string *sDescription,string *sPath,string &sCMD_Result,Message *pMessage)
//<-dceag-c802-e->
{
	PlutoSqlResult result_set;
	string sSQL;
	DB_ROW row;
	
	// Consider that we don't have any drive to store the file 
	*iPK_Device = 0;
	
	// Selecting first internal drive (1790) that has space to store the file
	sSQL = " " 
		"SELECT "
			"Device.PK_Device, "
			"Device.Description, "
			"DDSize.IK_DeviceData Size, "
			"DDType.IK_DeviceData Type, "
			"DDMaxFileSize.IK_DeviceData MaxFileSize "
		"FROM "
			"Device "
			"JOIN Device_DeviceData DDSize ON DDSize.FK_Device = Device.PK_Device AND DDSize.FK_DeviceData = 160 "
			"JOIN Device_DeviceData DDType ON DDType.FK_Device = Device.PK_Device AND DDType.FK_DeviceData = 159 "
			"LEFT JOIN Device_DeviceData DDMaxFileSize ON DDMaxFileSize.FK_Device = Device.PK_Device AND DDType.FK_DeviceData = 166 "
		"WHERE "
			"DDSize.IK_DeviceData > " + StringUtils::itos(iSize) + " "
			"AND  ( "
				"DDMaxFileSize.IK_DeviceData IS NULL "
				"OR "
				"DDMaxFileSize.IK_DeviceData > " + StringUtils::itos(iSize) + " "
				" ) AND Device.FK_DeviceTemplate = 1790 "
		"LIMIT 1 ";
	result_set.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL);
	
	// If no internal drive, try our luck with the network storage devices (cat 158)
	if (result_set.r == NULL) {
		sSQL = " \
			SELECT 	Device.PK_Device, \
				Device.Description, \
				DDSize.IK_DeviceData Size, \
				DDType.IK_DeviceData Type, \
				DDMaxFileSize.IK_DeviceData MaxFileSize, \
				DeviceTemplate.FK_DeviceCategory Category \
			FROM \
				Device \
				JOIN Device_DeviceData DDSize ON DDSize.FK_Device = Device.PK_Device AND DDSize.FK_DeviceData = 160 \
				JOIN Device_DeviceData DDType ON DDType.FK_Device = Device.PK_Device AND DDType.FK_DeviceData = 159 \
				LEFT JOIN Device_DeviceData DDMaxFileSize ON DDMaxFileSize.FK_Device = Device.PK_Device AND DDType.FK_DeviceData = 166 \
				JOIN DeviceTemplate ON DeviceTemplate.PK_DeviceTemplate = Device.FK_DeviceTemplate \
			WHERE \
				DDSize.IK_DeviceData > " + StringUtils::itos(iSize) + " \
				AND ( \
					DDMaxFileSize.IK_DeviceData IS NULL \
					OR \
					DDMaxFileSize.IK_DeviceData > " + StringUtils::itos(iSize) + " \
				) \
				AND \
				DeviceTemplate.FK_Category = 158 \
			LIMIT 1 \
		";
		result_set.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL);
	}
	
	// If we found a storage device(nas/internal drive) that can store the file
	if (result_set.r != NULL) 
	{
		row = NULL;
		if ( (row = db_wrapper_fetch_row(result_set.r)) != NULL ) 
		{
			*iPK_Device = atoi(row[0]);
			*sDescription = row[1];
			   	*sPath = "/mnt/device/";
			*sPath += row[0];

		}	
	}

	
	// If we didn't find any storage device that could store the file, look on the core's /home partition
	if (*iPK_Device == 0 ) {
		sSQL = " \
			SELECT	\
				Device.PK_Device \
			FROM \
				Device \
				JOIN Device_DeviceData DDSize ON DDSize.FK_Device = Device.PK_Device AND DDSize.FK_DeviceData = 160 \
			WHERE  \
				IK_DeviceData > " + StringUtils::itos(iSize) + " \
				AND \
				Device.FK_DeviceTemplate = 7 \
			LIMIT 1 \
		";
		result_set.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL);
		
		if (result_set.r != NULL) {
			row = NULL;
			if ( (row = db_wrapper_fetch_row(result_set.r)) != NULL ) {
				*iPK_Device = atoi(row[0]);
				*sDescription = "Core /home directory";
				*sPath = "/home/";
			}
		}
	}	
	
}
//<-dceag-c803-b->

	/** @brief COMMAND: #803 - Blacklist Internal Disk Drive */
	/** Adds an internal disk drive device to the list of blacklisted drives. */
		/** @param #156 PK_Device_ControlledVia */
			/** The 'computer' device that contains the blacklisted drive. */
		/** @param #223 Block Device */
			/** Linux block device representing the Internal Disk Drive. */

void General_Info_Plugin::CMD_Blacklist_Internal_Disk_Drive(int iPK_Device_ControlledVia,string sBlock_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c803-e->
{
	/* OBSOLITE 
	DeviceData_Base *pDevice_MD = this->m_pRouter->m_mapDeviceData_Router_Find(iPK_Device_ControlledVia);
	if(pDevice_MD) 
	{
		DeviceData_Base *pDevice_AppServer = ((DeviceData_Impl *)pDevice_MD)->FindSelfOrChildWithinCategory(DEVICECATEGORY_App_Server_CONST);
		
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device, pDevice_AppServer->m_dwPK_Device , "/usr/pluto/bin/StorageDevices_Radar.sh", "blacklist", 
				
				"--blacklist\t" + sBlock_Device, "", "", false, false, false, true);
	
		SendCommand (CMD_Spawn_Application);
	}
	*/
}

void General_Info_Plugin::PromptUserToReloadAfterNewDevices()
{
	bool bDevicesNeedingReload=false;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::PromptUserToReloadAfterNewDevices has %d devices",(int) m_listRow_Device_NewAdditions.size());
	for(list<Row_Device *>::iterator it=m_listRow_Device_NewAdditions.begin();it!=m_listRow_Device_NewAdditions.end();++it)
	{
		Row_Device *pRow_Device = *it;
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get()->GetRow(pRow_Device->FK_DeviceTemplate_get(),DEVICEDATA_Immediate_Reload_Isnt_Necessar_CONST);
		if( !pRow_DeviceTemplate_DeviceData || atoi(pRow_DeviceTemplate_DeviceData->IK_DeviceData_get().c_str())!=1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::PromptUserToReloadAfterNewDevices device %d needs reload",pRow_Device->PK_Device_get());
			bDevicesNeedingReload=true;
			break;
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::PromptUserToReloadAfterNewDevices device %d doesn't need reload",pRow_Device->PK_Device_get());
	}

	if( bDevicesNeedingReload && !m_bNewInstall )  // Don't ask this on a new install since the user will need to reload again anyway
	{
		SCREEN_PopupMessage_DL SCREEN_PopupMessage_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(),
			"<%=T" + StringUtils::itos(TEXT_New_Devices_Configured_CONST) + "%>", // Main message
			"", // Command Line
			"new_device_reload", // Description
			"1", // sPromptToResetRouter
			"0", // sTimeout
			"1"); // sCannotGoBack
		SendCommand(SCREEN_PopupMessage_DL);
	}
}

//<-dceag-c808-b->

	/** @brief COMMAND: #808 - Get Unused Serial Ports */
	/** Get the unused serial ports */
		/** @param #2 PK_Device */
			/** The computer to get the ports for */
		/** @param #5 Value To Assign */
			/** A comma delimited list of ports */

void General_Info_Plugin::CMD_Get_Unused_Serial_Ports(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c808-e->
{
	int iPK_Device_PC = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,iPK_Device);
	string sPorts = DatabaseUtils::GetDeviceData(m_pDatabase_pluto_main,iPK_Device_PC,DEVICEDATA_Available_Serial_Ports_CONST);
	string sAvailablePorts;
	string::size_type pos=0;
	while(pos<sPorts.size())
	{
		string sPort = StringUtils::Tokenize(sPorts,",",pos);
		string sSQL = "select FK_DeviceData FROM Device_DeviceData "
			"JOIN Device AS D1 ON FK_Device=D1.PK_Device "
			"LEFT JOIN Device AS D2 ON D1.FK_Device_ControlledVia=D2.PK_Device "
			"LEFT JOIN Device AS D3 ON D2.FK_Device_ControlledVia=D3.PK_Device "
			"LEFT JOIN Device AS D4 ON D3.FK_Device_ControlledVia=D4.PK_Device "
			"WHERE FK_DeviceData=" TOSTRING(DEVICEDATA_COM_Port_on_PC_CONST) " "
			"AND IK_DeviceData='" + StringUtils::SQLEscape(sPort) + "' "
			"AND (D1.PK_Device=" + StringUtils::itos(iPK_Device_PC) + " OR D2.PK_Device=" + StringUtils::itos(iPK_Device_PC) + " OR D3.PK_Device=" + StringUtils::itos(iPK_Device_PC) + " OR D4.PK_Device=" + StringUtils::itos(iPK_Device_PC) + " OR D4.FK_Device_ControlledVia=" + StringUtils::itos(iPK_Device_PC) + ")";
		
		PlutoSqlResult result;
		if( (result.r = m_pRouter->db_wrapper_query_result(sSQL)) && result.r->row_count>0 )
			continue;
		if( sAvailablePorts.size() )
			sAvailablePorts += ",";
		sAvailablePorts += sPort;
	}
	*sValue_To_Assign = sAvailablePorts;
}

//<-dceag-c813-b->

	/** @brief COMMAND: #813 - Add Software */
	/** Install Software on media director */
		/** @param #2 PK_Device */
			/** The MD to install it on */
		/** @param #119 True/False */
			/** True = add the package, false=remove it */
		/** @param #229 PK_Software */
			/** ID of installing package */

void General_Info_Plugin::CMD_Add_Software(int iPK_Device,bool bTrueFalse,int iPK_Software,string &sCMD_Result,Message *pMessage)
//<-dceag-c813-e->
{
	if( iPK_Device<1 )
		iPK_Device = pMessage->m_dwPK_Device_From;

	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if(pDevice)
		pDevice = pDevice->GetTopMostDevice();

	Row_Software *pRow_Software = m_pDatabase_pluto_main->Software_get()->GetRow(iPK_Software);

	if(!pDevice || !pRow_Software)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::CMD_Add_Software can't find device %d or software %d",iPK_Device,iPK_Software);
		return;
	}

	Row_Software_Device *pRow_Software_Device = m_pDatabase_pluto_main->Software_Device_get()->GetRow(pRow_Software->PK_Software_get(),pDevice->m_dwPK_Device);
	if( pRow_Software_Device==NULL )
	{
		pRow_Software_Device = m_pDatabase_pluto_main->Software_Device_get()->AddRow();
		pRow_Software_Device->FK_Software_set(pRow_Software->PK_Software_get());
		pRow_Software_Device->FK_Device_set(pDevice->m_dwPK_Device);
	}

	string sCommand="/usr/pluto/bin/AddSoftwareHelper.sh";
	string sArguments=( bTrueFalse ? "install" : "remove" ) + string("\t") + StringUtils::itos(pDevice->m_dwPK_Device) + "\t" + StringUtils::itos(pRow_Software->PK_Software_get()) + "\t" + StringUtils::itos(pMessage->m_dwPK_Device_From);

	DeviceData_Base *pDevice_AppServer = m_pData->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST, this, 0 ); 
	if( pDevice_AppServer )
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
			sCommand,"install software",sArguments,"","",false,false,false,true);
		SendCommand(CMD_Spawn_Application);
		
		pRow_Software_Device->Status_set(bTrueFalse ? "i" : "r");
		pRow_Software_Device->Table_Software_Device_get()->Commit();
	}
	else
	{
		DCE::SCREEN_DialogGenericError SCREEN_DialogGenericError(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			"Cannot install software right now.  Please try again","0","10","1");
		SendCommand(SCREEN_DialogGenericError);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"General_Info_Plugin::CMD_Add_Software -- no app server");
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"General_Info_Plugin::CMD_Add_Software Starting Add software device %d software %d true %d cmd %s %s",
		pDevice->m_dwPK_Device,iPK_Software,(int) bTrueFalse,sCommand.c_str(),sArguments.c_str());
}
//<-dceag-c833-b->

	/** @brief COMMAND: #833 - Get User Name */
	/** Given a PK_User, get the users name */
		/** @param #5 Value To Assign */
			/** The user's name */
		/** @param #17 PK_Users */
			/** The user id */

void General_Info_Plugin::CMD_Get_User_Name(int iPK_Users,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c833-e->
{
	Row_Users *pRow_Users = m_pDatabase_pluto_main->Users_get()->GetRow(iPK_Users);
	if( pRow_Users )
		*sValue_To_Assign = pRow_Users->UserName_get();
}
//<-dceag-c835-b->

	/** @brief COMMAND: #835 - Get Network Devices Shares */
	/** Get the list with shares for all network devices from this installation. */
		/** @param #244 Custom Response */
			/** Contains the following info:

device id, device ip, share name, user name, password, free space.

Delimiter: '\n' */

void General_Info_Plugin::CMD_Get_Network_Devices_Shares(char **pCustom_Response,int *iCustom_Response_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c835-e->
{

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Starting Get_Network_Devices_Shares");

	map<int, pair<string, pair<string, string> > > mapIP;
	multimap<int, pair<int, string> > mapInfo;

	PlutoSqlResult result;
	DB_ROW row;
	string sSQL = 
		"SELECT DeviceChild.PK_Device, DeviceParent.IPaddress, DeviceParent.Description, DeviceChild.Description, FK_DeviceData, IK_DeviceData "
		"FROM Device AS DeviceParent "
		"INNER JOIN DeviceTemplate ON DeviceParent.FK_DeviceTemplate = PK_DeviceTemplate "
		"INNER JOIN Device AS DeviceChild ON DeviceChild.FK_Device_ControlledVia = DeviceParent.PK_Device "
		"INNER JOIN Device_DeviceData ON DeviceChild.PK_Device = Device_DeviceData.FK_Device "
		"AND FK_DeviceData IN (" + StringUtils::ltos(DEVICEDATA_Share_Name_CONST) + ", " + 
			StringUtils::ltos(DEVICEDATA_Username_CONST) + ", " + 
			StringUtils::ltos(DEVICEDATA_Password_CONST) + ", " +
			StringUtils::ltos(DEVICEDATA_Free_Disk_Space_in_MBytes_CONST ) + ", " +
			StringUtils::ltos(DEVICEDATA_Readonly_CONST ) +
			" ) ";
		//"WHERE FK_DeviceCategory = " + StringUtils::ltos(DEVICECATEGORY_FileMedia_Server_CONST);

	enum FieldNames
	{
		fnDeviceID,
		fnIP,
		fnParentDescription,
		fnChildDescription,
		fnDeviceDataType,
		fnDeviceDataValue
	};

	if( (result.r = m_pDatabase_pluto_main->db_wrapper_query_result(sSQL.c_str())) )
	{
		while((row = db_wrapper_fetch_row(result.r)))	
		{
			if(NULL != row[fnDeviceID] && NULL != row[fnIP])
			{
				int DeviceID = atoi(row[fnDeviceID]);

				//TODO: optimize with a find
				mapIP[DeviceID] = make_pair( row[fnIP], 
					make_pair( string(row[fnParentDescription]), string(row[fnChildDescription]) )
					);

				int nDeviceDataType = NULL != row[fnDeviceDataType] ? atoi(row[fnDeviceDataType]) : 0;
				string sDeviceDataValue = NULL != row[fnDeviceDataValue] ? row[fnDeviceDataValue] : string();
				mapInfo.insert(make_pair(DeviceID, make_pair(nDeviceDataType, sDeviceDataValue)));
			}
		}
	}

	// read core share user/pass
	string sFilename = "/usr/pluto/var/sambaCredentials.secret";
	
	//string sFilename = "c:\\work\\sambaCredentials.secret";
	
	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);	
	string sCoreUser="", sCorePass="";
	if( c && Length ){
		string sFile(c);
		string sLine = "";
		int iPos = 0;
		string sValue = "username=";
		iPos = sFile.find( sValue, 0 );
		if ( iPos>=0 ) {
			sLine = sFile.substr( iPos+sValue.size(), sFile.size()-iPos+1 );
			iPos = sLine.find( "\n", 0 );
			if ( iPos>=0 )
				sCoreUser = sLine.substr( 0, iPos );
		}
		sValue = "password=";
		iPos = sFile.find( sValue, 0 );
		if ( iPos>=0 ) {
			sLine = sFile.substr( iPos+sValue.size(), sFile.size()-iPos+1 );
			iPos = sLine.find( "\n", 0 );
			if ( iPos>=0 )
				sCorePass = sLine.substr( 0, iPos );
		}

		delete[] c;
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "sambaCredentials file missing");

	//got the info, let's serialize them

	stringstream strout;
	// writeln mapIP.size
	strout<<mapIP.size()+1<<"\n";
	// serializing core
	// writeln DeviceID
	strout<<0<<"\n";
	// writeln IP
	strout<<"0.0.0.0"<<"\n";
	// Parent Description
	strout<<"core"<<"\n";
	// Child description
	strout<<"backup share"<<"\n";
	// writeln iCount
	strout<<5<<"\n";
	// writeln sParamType+"="+infoiter->second->second				
	strout<<"ShareName"<<"="<<"\\home\\public\\data"<<"\n";
	strout<<"UserName"<<"="<<sCoreUser<<"\n";
	strout<<"Password"<<"="<<sCorePass<<"\n";
	strout<<"FreeSpace"<<"="<<0<<"\n";
	strout<<"ReadOnly"<<"="<<0<<"\n";	


	for( map<int, pair<string, pair<string, string> > >::iterator iter = mapIP.begin(); iter!= mapIP.end(); ++iter )
	{
		// writeln DeviceID
		strout<<iter->first<<"\n";
		// writeln IP = mapIP->second		
		strout<<iter->second.first<<"\n";
		strout<<iter->second.second.first<<"\n";
		strout<<iter->second.second.second<<"\n";
		int iCount = mapInfo.count( iter->first );
		// writeln iCount
		strout<<iCount<<"\n";
		if ( iCount>0 )
		{			
			pair<multimap<int, pair<int, string> >::iterator, multimap<int, pair<int, string> >::iterator > iterBounds;
			iterBounds = mapInfo.equal_range( iter->first );
			// writeln
			for( multimap<int, pair<int,string> >::iterator infoiter = iterBounds.first; infoiter != iterBounds.second; ++infoiter )
			{
				string sParamType = "Unknown";
				switch( infoiter->second.first )
				{
				case DEVICEDATA_Share_Name_CONST:					
					sParamType = "ShareName";
					break;
				case DEVICEDATA_Username_CONST:
					sParamType = "UserName";
					break;
				case DEVICEDATA_Password_CONST:
					sParamType = "Password";
					break;
				case DEVICEDATA_Free_Disk_Space_in_MBytes_CONST:
					sParamType = "FreeSpace";
					break;
				case DEVICEDATA_Readonly_CONST:
					sParamType = "ReadOnly";
					break;
				}
				// writeln sParamType+"="+infoiter->second->second				
				strout<<sParamType<<"="<<infoiter->second.second<<"\n";
			}
		}
	}

	strout.seekg( 0, ios::beg );
	strout.seekg( 0, ios::end );	
	*iCustom_Response_Size = strout.tellg();		
	*pCustom_Response = new char[*iCustom_Response_Size];
	strout.seekg( 0, ios::beg );
	strout.read( *pCustom_Response, *iCustom_Response_Size );
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Finishing Get_Network_Devices_Shares");
}

//<-dceag-c843-b->

	/** @brief COMMAND: #843 - RemoteAssistance_SetStatus */
	/** Change the state of Remote Assistance */
		/** @param #208 Enable */
			/** Desired state of Remote Assistance */

void General_Info_Plugin::CMD_RemoteAssistance_SetStatus(bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c843-e->
{
	string sParm = bEnable ? "--enable" : "--disable";
	char * args[] = { (char *)("/usr/pluto/bin/RA-handler.sh"), (char *) sParm.c_str() };
	ProcessUtils::SpawnDaemon(args[0], args);
}
//<-dceag-c844-b->

	/** @brief COMMAND: #844 - RemoteAssistance_GetStatus */
	/** Get the state of Remote Assistance */
		/** @param #208 Enable */
			/** The state of Remote Assistance */

void General_Info_Plugin::CMD_RemoteAssistance_GetStatus(bool *bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c844-e->
{
	FILE * f;
	char buffer[1024];
	* bEnable = false;

	f = fopen("/etc/pluto.conf", "r");
	while (fgets(buffer, 1024, f) != NULL)
	{
		if (strncmp(buffer, "remote", 5) == 0)
		{
			* bEnable = true;
			break;
		}
	}
	fclose(f);
}
//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void General_Info_Plugin::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
}

bool General_Info_Plugin::SafeToReload(string &sReason)
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	if( PendingConfigs()==false )
		return true;

	sReason = "See Pending Tasks.  General Info Plugin Download Software on: ";

	bool bFirst=true;
	for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
	{
		if( it->second==false )
			continue;

		if( bFirst )
			bFirst=false;
		else
			sReason += ",";
		sReason += StringUtils::itos(it->first);
	}
	return false;
}

//<-dceag-c915-b->

	/** @brief COMMAND: #915 - Enable Device */
	/** Called when instead of creating a device, we re-enabled one that had been disabled and need to call the postcreateoptions again */
		/** @param #2 PK_Device */
			/** The device that was enabled */
		/** @param #198 PK_Orbiter */
			/** The Orbiter that did it, if any */

void General_Info_Plugin::CMD_Enable_Device(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c915-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "General_Info_Plugin::CMD_Enable_Device %d %d",iPK_Device,iPK_Orbiter);
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	if( pRow_Device )
	{
		OH_Orbiter *pOH_Orbiter = NULL;
		if( iPK_Orbiter )
			pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(iPK_Orbiter);
		m_pPostCreateOptions->PostCreateDevice(pRow_Device,pOH_Orbiter);
	}
}

void General_Info_Plugin::AlarmCallback(int id, void* param)
{
	if( id==UPDATE_ENT_AREA )
		UpdateEntAreas();
	else if( id==PROCESS_CHILD_DEVICES )
		ReportingChildDevices_Offline(param);
}

void General_Info_Plugin::UpdateEntAreas()
{
	system("/usr/pluto/bin/UpdateEntArea");
}
//<-dceag-c934-b->

	/** @brief COMMAND: #934 - Get Home Symlink */
	/** Get home symlink */
		/** @param #219 Path */
			/** The physical path like /mnt/device/<deviceid> */
		/** @param #275 Symlink */
			/** The symlink like /home/public/data ... */

void General_Info_Plugin::CMD_Get_Home_Symlink(string sPath,string *sSymlink,string &sCMD_Result,Message *pMessage)
//<-dceag-c934-e->
{
	string sResult;

	//tokenize the path
	vector<string> vectSubdirs;
	StringUtils::Tokenize(sPath, "/", vectSubdirs);

	size_t nTokenNumber = 3;

	//has to start with /mnt/device/<deviceid>
	if(vectSubdirs.size() < nTokenNumber)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: The path is too short: %s", sPath.c_str());
		return;
	}

	if(vectSubdirs[0] == "home")
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Get_Home_Symlink: Nothing to change: %s", sPath.c_str());
		*sSymlink = sPath;
		return;		
	}

	if(vectSubdirs[0] != "mnt" && vectSubdirs[1] != "device")
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: The path doesn't have the correct format: %s", sPath.c_str());
		return;
	}

	int nDeviceID = atoi(vectSubdirs[2].c_str());
	if(nDeviceID <= 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: Invalid device: %s", vectSubdirs[2].c_str());
		return;
	}

	//is this a valid device?
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(nDeviceID);
	if(NULL == pRow_Device)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: The device doesn't exist: %d", nDeviceID);
		return;
	}

	string sDescription = pRow_Device->Description_get();
	sDescription = StringUtils::Replace(sDescription, "/", "-");
 
	//get PK_Users device data
	vector<Row_Device_DeviceData *> vectDevice_DeviceData;
	m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows(
		"WHERE FK_Device = " + StringUtils::ltos(nDeviceID) + 
		" AND FK_DeviceData = " TOSTRING(DEVICEDATA_PK_Users_CONST),
		&vectDevice_DeviceData);

	if(vectDevice_DeviceData.empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: Can't find device data 'PK_Users' for device %d", nDeviceID);
		return;
	}

	int nPK_Users = atoi(vectDevice_DeviceData[0]->IK_DeviceData_get().c_str());

	if(sDescription[sDescription.size() - 1] != ' ')
		sDescription += " ";

	string sFullDescription = sDescription + "[" + StringUtils::ltos(nDeviceID) + "]";

	if(nPK_Users == -1)
	{
		nTokenNumber = 6;

		if(vectSubdirs.size() < nTokenNumber)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: The path is too short %s", sPath.c_str());
			return;
		}

		if(vectSubdirs[4] != "data")
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Get_Home_Symlink: Invalid path %s", sPath.c_str());
			return;
		}

		string sUserTypeSubdir = vectSubdirs[3]; //public or user_x
		string sMediaSubdir = vectSubdirs[5]; //videos, audio, etc
		sResult = "/home/" + sUserTypeSubdir + "/data/" + sMediaSubdir + "/" + sFullDescription;
	}
	else if(nPK_Users == 0)
	{
		sResult = "/home/public/data/other/" + sFullDescription;
	}
	else
	{
		sResult = "/home/user_" + StringUtils::ltos(nPK_Users) + "/data/other/" + sFullDescription;
	}

	for(size_t i = nTokenNumber; i < vectSubdirs.size(); ++i)
		sResult += "/" + vectSubdirs[i];

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "CMD_Get_Home_Symlink: Translated from %s to %s", 
		sPath.c_str(), sResult.c_str());

	*sSymlink = sResult;
}
//<-dceag-c956-b->

	/** @brief COMMAND: #956 - Get Devices To Start */
	/** Get the list with devices to start. */
		/** @param #2 PK_Device */
			/** The parent device */
		/** @param #5 Value To Assign */
			/** A pipe delimited list like this: DeviceID1|TemplateName1|CommandLine1\nDeviceID2|DeviceTemplateName2|CommandLine2 etc */

void General_Info_Plugin::CMD_Get_Devices_To_Start(int iPK_Device,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c956-e->
{
}
//<-dceag-c957-b->

	/** @brief COMMAND: #957 - Update Device */
	/** Will update the description, ip, mac, device data */
		/** @param #2 PK_Device */
			/** Device ID */
		/** @param #47 Mac address */
			/** Mac address */
		/** @param #57 PK_Room */
			/** Room ID */
		/** @param #58 IP Address */
			/** IP address */
		/** @param #109 Data String */
			/** pipe delimited list with device data */
		/** @param #163 Description */
			/** Device description */

void General_Info_Plugin::CMD_Update_Device(int iPK_Device,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,string sDescription,string &sCMD_Result,Message *pMessage)
//<-dceag-c957-e->
{
	// Tokenize the device data
	vector<string> vectDeviceDataBuff;
	StringUtils::Tokenize(sData_String,"|",vectDeviceDataBuff);
	
	// Iterate over the device data tokens in pairs
	for (unsigned int i=0; i<vectDeviceDataBuff.size(); i=i+2) {
		// Get the device data and convert it to an int
		string sPK_DeviceData = vectDeviceDataBuff[i];
		int iPK_DeviceData = atoi(sPK_DeviceData.c_str());

		// Get the value of the device data
		string sDeviceDataValue = vectDeviceDataBuff[i+1];

		// LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DeviceData: %s Value: %s", sPK_DeviceData.c_str(), sDeviceDataValue.c_str());

		// Set the device data
		m_pRouter->SetDeviceDataInDB(iPK_Device,iPK_DeviceData,sDeviceDataValue,false);
	}

	// Get ready to update the room
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	pRow_Device->Reload();

	// Update the room
	if(iPK_Room > 0) {
		pRow_Device->FK_Room_set(iPK_Room);
	}

	// Update the description
	if(sDescription.size()) {
		pRow_Device->Description_set(sDescription);
		m_pDatabase_pluto_main->Device_get()->Commit();
	}
}
//<-dceag-c1075-b->

	/** @brief COMMAND: #1075 - Send Email */
	/** Send an email in response to events. */
		/** @param #282 To */
			/** Who does the message go to? */
		/** @param #283 Subject */
			/** Synopsis of what happened. */
		/** @param #284 MessageBody */
			/** Details about the event. */
		/** @param #288 From */
			/** What is the sender's email address? */

void General_Info_Plugin::CMD_Send_Email(string sTo,string sSubject,string sMessageBody,string sFrom,string &sCMD_Result,Message *pMessage)
//<-dceag-c1075-e->
{
	cout << "Need to implement command #1075 - Send Email" << endl;
	cout << "Parm #282 - To=" << sTo << endl; 
	cout << "Parm #283 - Subject=" << sSubject << endl;
	cout << "Parm #284 - MessageBody=" << sMessageBody << endl;
	cout << "Parm #285 - From=" << sFrom;

	// Write our messagebody to a file
	ofstream myfile ("/tmp/MessageBody.tmp");
	if (myfile.is_open()){
	myfile << sMessageBody.c_str();
	myfile.close(); }

	// Build our command 
	string sCommand = "mailx -r \""+ sFrom + "\" -s \"" + sSubject + "\" \"" + sTo + "\" < /tmp/MessageBody.tmp";

	// Send our command
	system(sCommand.c_str());

	// Clean up
	sCommand = "rm /tmp/MessageBody.tmp";

	system(sCommand.c_str());
}
//<-dceag-c1106-b->

	/** @brief COMMAND: #1106 - Get Users */
	/** Get users */
		/** @param #5 Value To Assign */
			/** Returns the users in this system as a Pk\tNickName\nPk\tNickName\n etc. */

void General_Info_Plugin::CMD_Get_Users(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c1106-e->
{
	string result = "";
	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("1=1 ORDER BY Nickname", &vectRow_Users);
	for(size_t s=0; s<vectRow_Users.size(); s++)
	{
		result += StringUtils::itos(vectRow_Users[s]->PK_Users_get()) + "\t" + vectRow_Users[s]->Nickname_get() + "\n";
	}
	(*sValue_To_Assign) = result;
}
//<-dceag-c1107-b->

	/** @brief COMMAND: #1107 - Get Rooms */
	/** Get the rooms */
		/** @param #5 Value To Assign */
			/** Returns the rooms in this installation as Pk\tDescription\nPk\tDescription\n etc. */

void General_Info_Plugin::CMD_Get_Rooms(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c1107-e->
{
	string result = "";
	vector<Row_Room *> vectRow_Room;
	m_pDatabase_pluto_main->Room_get()->GetRows("1=1 ORDER BY Description", &vectRow_Room);
	for(size_t s=0; s<vectRow_Room.size(); s++)
	{
		result += StringUtils::itos(vectRow_Room[s]->PK_Room_get()) + "\t" + vectRow_Room[s]->Description_get() + "\n";
	}
	(*sValue_To_Assign) = result;
}
//<-dceag-c1108-b->

	/** @brief COMMAND: #1108 - Get Scenarios */
	/** Get scenarios */
		/** @param #5 Value To Assign */
			/** Returns the scenarios for the specified parameters. */
		/** @param #45 PK_EntertainArea */
			/** Entertainment area to get scenarios for. Only valid for media scenarios. */
		/** @param #48 Value */
			/** Scenario to get. 1 = Lightning, 2= Climate, 3 = Security, 4 = Telecom, 5 = Media */
		/** @param #57 PK_Room */
			/** The Room to get the scenarios for. Required! */

void General_Info_Plugin::CMD_Get_Scenarios(string sPK_EntertainArea,int iValue,int iPK_Room,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c1108-e->
{
	string result = "";
	switch(iValue)
	{
	case ARRAY_Lighting_Scenarios_CONST:
	case ARRAY_Climate_Scenarios_CONST:
	case ARRAY_Security_Scenarios_CONST:
	case ARRAY_Communication_Scenarios_CONST:
	case ARRAY_Misc_Scenarios_CONST:
	case ARRAY_Sleeping_Scenarios_CONST:
	{
		vector<class Row_CommandGroup_Room *> vectAGs;
		string sql = string(COMMANDGROUP_ROOM_FK_ROOM_FIELD) + "=" +
			StringUtils::itos(iPK_Room) + " ORDER BY " + COMMANDGROUP_ROOM_SORT_FIELD;
		m_pDatabase_pluto_main->CommandGroup_Room_get()->GetRows(sql,&vectAGs);

		for(size_t s=0;s<vectAGs.size();++s)
		{
			Row_CommandGroup_Room *drAG_R = vectAGs[s];
			Row_CommandGroup *pRow_CommandGroup=drAG_R->FK_CommandGroup_getrow();
			if( pRow_CommandGroup && pRow_CommandGroup->FK_Array_get()==iValue && !pRow_CommandGroup->Disabled_get() )
			{
				result +=  StringUtils::itos(drAG_R->FK_CommandGroup_get()) + "\t" +
					// PK_Text
					(drAG_R->FK_CommandGroup_getrow()->FK_Text_isNull() ? "" : StringUtils::itos(drAG_R->FK_CommandGroup_getrow()->FK_Text_get())) + "\t" +
					// description
					drAG_R->FK_CommandGroup_getrow()->Description_get() + "\n";
			}
		}
		break;
	}
	case ARRAY_Media_Scenarios_CONST:
	{
		int PK_EntArea = atoi(sPK_EntertainArea.c_str());
		if (PK_EntArea > 0)
		{
			vector<class Row_CommandGroup_EntertainArea *> vectEGs;
			string sql = string(COMMANDGROUP_ENTERTAINAREA_FK_ENTERTAINAREA_FIELD) + "=" + \
				StringUtils::itos(PK_EntArea) + " ORDER BY " + COMMANDGROUP_ENTERTAINAREA_SORT_FIELD;
			m_pDatabase_pluto_main->CommandGroup_EntertainArea_get()->GetRows(sql,&vectEGs);

			for(size_t s=0;s<vectEGs.size();++s)
			{
				Row_CommandGroup_EntertainArea *drAG_E = vectEGs[s];
				Row_CommandGroup *pRow_CommandGroup=drAG_E->FK_CommandGroup_getrow();
				if( pRow_CommandGroup && pRow_CommandGroup->FK_Array_get()==iValue && !pRow_CommandGroup->Disabled_get() )
				{
					result +=  StringUtils::itos(drAG_E->FK_CommandGroup_get()) + "\t" +
						// PK_Text
						(drAG_E->FK_CommandGroup_getrow()->FK_Text_isNull() ? "" : StringUtils::itos(drAG_E->FK_CommandGroup_getrow()->FK_Text_get())) + "\t" +
						// description
						drAG_E->FK_CommandGroup_getrow()->Description_get() + "\n";
				}
			}
		}

		break;
	}
	}

	(*sValue_To_Assign) = result;
}
//<-dceag-c1109-b->

	/** @brief COMMAND: #1109 - Get Entertainment Areas */
	/** Get the entertainment areas */
		/** @param #5 Value To Assign */
			/** Returns the entertainment areas for this installation as Pk_EntArea\tFK_Room\tDescription\n etc. */

void General_Info_Plugin::CMD_Get_Entertainment_Areas(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c1109-e->
{
	string result = "";
	vector<Row_EntertainArea *> vectRow_EntertainArea;
	m_pDatabase_pluto_main->EntertainArea_get()->GetRows("1=1 ORDER BY Description", &vectRow_EntertainArea);
	for(size_t s=0; s<vectRow_EntertainArea.size(); s++)
	{
		result += StringUtils::itos(vectRow_EntertainArea[s]->PK_EntertainArea_get()) + "\t"+ StringUtils::itos(vectRow_EntertainArea[s]->FK_Room_get()) + "\t" + vectRow_EntertainArea[s]->Description_get() + "\n";
	}
	(*sValue_To_Assign) = result;
}
