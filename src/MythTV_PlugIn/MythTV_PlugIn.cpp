/**
 *
 * @file MythTV_PlugIn.cpp
 * @brief source file for the MythTvStream, MythTV_PlugIn classes
 *
 */

//<-dceag-d-b->
#include "MythTV_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "MythBackEnd_Socket.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Variable.h"
#include "../pluto_main/Table_EventParameter.h"
#include "../pluto_main/Table_Users.h"
#include "../pluto_media/Table_LongAttribute.h"
#include "../pluto_media/Table_MediaProvider.h"
#include "../pluto_media/Table_ProviderSource.h"
#include "../pluto_media/Table_Picture.h"
#include "../pluto_media/Define_FileFormat.h"
#include "../pluto_media/Define_MediaSubType.h"
#include "../pluto_media/Define_AttributeType.h"

#include "../Gen_Devices/AllScreens.h"

#include "DataGrid.h"
#include "EPGGrid.h"
#include "MythTvMediaStream.h"
#ifndef WIN32
//#include "MythTvWrapper.h"
//#include <libmythtv/frame.h>
#endif

#include "../Orbiter_Plugin/OH_Orbiter.h"

#include <sstream>
#include <math.h>
#include <time.h>

static const string SCRIPT_DIR =             "/usr/pluto/bin";
static const string SCRIPT_SYNC_TO_DB =      SCRIPT_DIR + "/MythTvSyncDB.sh";
static const string SCRIPT_TUNE =            SCRIPT_DIR + "/TuneToChannel.sh";
static const string SCRIPT_RESTART_BACKEND = SCRIPT_DIR + "/Restart_MythBackend.sh";
static const string SCRIPT_RESTART_ALL_BACKENDS = SCRIPT_DIR + "/Restart_Backend_With_SchemaLock.sh";
static const string SCRIPT_INITIAL_FILLDB =  SCRIPT_DIR + "/MythTvInitialFillDB.sh";
static const string SCRIPT_FORCE_KILL =      SCRIPT_DIR + "/ForciblyKillProcess.sh";
static const string SCRIPT_MYTHTV_SETUP =    SCRIPT_DIR + "/mythtv_setup.pl";

// For sorting channels
static bool ChannelComparer(MythChannel *x, MythChannel *y)
{
	return x->m_dwChanNum<y->m_dwChanNum;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MythTV_PlugIn::MythTV_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MythTV_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pEPGGrid = NULL;
	m_bFillDbRunning = m_bNeedToRunFillDb = false;
//	m_bPreProcessSpeedControl=false;  // We do some ridiculous hacks in Myth player to convert speed control commands to keystrokes
//	m_pMythWrapper = NULL;
	m_pDBHelper_Myth = NULL;
	m_pAlarmManager=NULL;
	m_pMythBackEnd_Socket=NULL;
	m_dwPK_File_LastCheckedForNewRecordings=0;

	m_bAskBeforeReload=true;
	m_bImplementsPendingTasks=true;
	m_iRetriesStartingMyth=0;
}

//<-dceag-getconfig-b->
bool MythTV_PlugIn::GetConfig()
{
	if( !MythTV_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	m_sMyth_Token = MYTH_TOKEN;
	m_sMyth_Protocol = MYTH_PROTOCOL;

	if (DATA_Get_Token() != "" && DATA_Get_Protocol() != "" )
	{
       	    m_sMyth_Token = DATA_Get_Token();
            m_sMyth_Protocol = DATA_Get_Protocol();
    	}
	m_pDBHelper_Myth = new DBHelper(m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ),"mythconverg");

	// This will produce a harmless error after the first run.  By default myth doesn't index the start time, making 
	// the AllShows grid take a very, very long time to execute the sql select
	m_pDBHelper_Myth->threaded_db_wrapper_query("alter table `mythconverg`.`program` add index `starttime` ( `starttime` )",true);

	// Get timezone offset to correct time values displayed on orbiters, mythtv passes everything in UTC since 0.26
	string sSQL = "SELECT TIME_FORMAT( NOW() - UTC_TIMESTAMP(), '%H:%i' ) AS tz_offset";
	PlutoSqlResult result;
	DB_ROW row;
	if ((result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL)
	{
		row=db_wrapper_fetch_row(result.r);
		m_sTzOffset = row[0];
	}
	else
	{
		m_sTzOffset = "+00:00";
	}

	m_pEPGGrid = new EPGGrid(m_pDBHelper_Myth);

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(2);      //4 = number of worker threads
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
MythTV_PlugIn::~MythTV_PlugIn()
//<-dceag-dest-e->
{
#ifndef WIN32
//	delete m_pMythWrapper;
#endif

	delete m_pEPGGrid;
	m_pEPGGrid = NULL;

	delete m_pDBHelper_Myth;
	m_pDBHelper_Myth = NULL;

	PurgeChannelList();
}

void MythTV_PlugIn::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
	delete m_pMythBackEnd_Socket;
	m_pMythBackEnd_Socket=NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MythTV_PlugIn::Register()
//<-dceag-reg-e->
{
	m_iPriority=DATA_Get_Priority();

	/** Get a pointer to the media plugin */
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pGeneral_Info_Plugin=( General_Info_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pMedia_Plugin || !m_pGeneral_Info_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to myth plugin");
		return false;
	}

	m_pMedia_Plugin->RegisterMediaPlugin(this, this, DEVICETEMPLATE_MythTV_Player_CONST, true);

	// Use the player, not the plugin, as the template for datagrids so that <%=NPDT%> in orbiter works
    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::CurrentShows))
                                                ,DATAGRID_EPG_Current_Shows_CONST,DEVICETEMPLATE_MythTV_Player_CONST);

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::AllShows))
                                                ,DATAGRID_EPG_All_Shows_CONST,DEVICETEMPLATE_MythTV_Player_CONST);

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::PVREPGGrid))
                                                ,DATAGRID_EPG_Grid_CONST,DEVICETEMPLATE_MythTV_Player_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&MythTV_PlugIn::TvProviders))
												,DATAGRID_TV_Providers_CONST,DEVICETEMPLATE_MythTV_Player_CONST);

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::FavoriteChannels ))
										        , DATAGRID_Favorite_Channels_CONST,DEVICETEMPLATE_MythTV_Player_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::FavoriteShows ))
										        , DATAGRID_Favorite_Shows_CONST,DEVICETEMPLATE_MythTV_Player_CONST );

    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::ThumbnailableAttributes ))
										        , DATAGRID_Thumbnailable_Attributes_CONST,DEVICETEMPLATE_MythTV_Player_CONST );
    m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &MythTV_PlugIn::ThumbnailableAttributes ))
										        , DATAGRID_Thumbnailable_Attributes_CONST,DEVICETEMPLATE_MythTV_Player_CONST );

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::MediaInfoChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_MythTV_Channel_Changed_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::NewRecording), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_MythTV_Show_Recorded_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::ScanningProgress ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Channel_Scan_Progress_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::NewBookmarks ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Save_Bookmark_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::TuneToChannel ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Tune_to_channel_CONST );

	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_MythTV_Player_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDevice_MythPlayer = *it;
			RegisterMsgInterceptor( ( MessageInterceptorFn )( &MythTV_PlugIn::PlaybackStarted ), pDevice_MythPlayer->m_dwPK_Device, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Started_CONST );
		}
	}

	//Setup Mythtv
	//This includes purging the settings table of any deleted MD devices
	//as well as generating and keeping the storagegroups up-to-date
	system(SCRIPT_MYTHTV_SETUP.c_str());

	/*
	bool bPathsChanged = SetPaths();
	if( bPathsChanged )
        { // Paths were changed so we need to restart the backend[s].
	
	DeviceData_Base *pDevice_App_Server = (DeviceData_Router *)
	m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
	DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
	SCRIPT_RESTART_ALL_BACKENDS, "restart_all_backends_in_register", "",
	"", "", false, false, false, false);
	SendCommand(CMD_Spawn_Application);
        }*/
	
	// Don't actually build the bookmark/channel list because Sync_Cards_Providers will fill in m_mapDevicesToSources,
	// but it's not run until about 20 seconds after everything starts so if it needs to send a message to the orbiters
	// or use AppServer, those devices will be available
	m_bBookmarksNeedRefreshing=true;

	BuildAttachedInfraredTargetsMap();

	BuildChannelList();

	m_pAlarmManager->AddRelativeAlarm(30,this,CHECK_FOR_NEW_RECORDINGS,NULL);
	m_pAlarmManager->AddRelativeAlarm(5,this,RUN_BACKEND_STARTER,NULL);
	m_pAlarmManager->AddRelativeAlarm(15,this,CHECK_FOR_SCHEDULED_RECORDINGS,NULL);
	m_pAlarmManager->AddRelativeAlarm(60,this,CONFIRM_MASTER_BACKEND_OK,NULL);
	m_pAlarmManager->AddRelativeAlarm(45,this,SYNC_PROVIDERS_AND_CARDS,NULL);  // Do this after pnp has had a chance to update all incoming devices

	m_pMythBackEnd_Socket = new MythBackEnd_Socket(this,m_pRouter->sDBHost_get( ));  // The master backend is on the same server as mysql
	m_pMythBackEnd_Socket->Connect();

	BuildCategoryColors();

    return Connect(PK_DeviceTemplate_get());
}

void MythTV_PlugIn::BuildCategoryColors()
{
	// for now, this is hardcoded until I do some databsae changes to pluto_myth -tschak
	m_mapCategoryColors["Unknown"] = 3165768;
	m_mapCategoryColors["Special"] = 1463685;
	m_mapCategoryColors["Children"] = 2299525;
	m_mapCategoryColors["House/garden"] = 4228374;
	m_mapCategoryColors["Travel"] = 1475961;
	m_mapCategoryColors["Educational"] = 1475922;
	m_mapCategoryColors["Anthology"] = 6980886;
	m_mapCategoryColors["News"] = 8721942;
	m_mapCategoryColors["Talk"] = 8722044;
	m_mapCategoryColors["Collectibles"] = 8722007;
	m_mapCategoryColors["Documentary"] = 8732694;
	m_mapCategoryColors["Home improvement"] = 4948062;
	m_mapCategoryColors["Music"] = 7241803;
	m_mapCategoryColors["Public affairs"] = 8416331;
	m_mapCategoryColors["Cooking"] = 8142080;
	m_mapCategoryColors["How-to"] = 31840;
	m_mapCategoryColors["Interview"] = 1507452;
	m_mapCategoryColors["Sitcom"] = 5374076;
	m_mapCategoryColors["Drama"] = 7864444;
	m_mapCategoryColors["Music special"] = 21116;
	m_mapCategoryColors["Science"] = 31867;
	m_mapCategoryColors["History"] = 8092672;
	m_mapCategoryColors["Sports non-event"] = 40979;
	m_mapCategoryColors["Shopping"] = 10519552;
	m_mapCategoryColors["Religious"] = 10526880;
	m_mapCategoryColors["Consumer"] = 12015104;
	m_mapCategoryColors["Fantasy"] = 39066;
	m_mapCategoryColors["Suspense"] = 5570560;
	m_mapCategoryColors["Game show"] = 5746785;
	m_mapCategoryColors["Crime Drama"] = 16648;
	m_mapCategoryColors["Western"] = 8540160;
	m_mapCategoryColors["Community"] = 33399;
	m_mapCategoryColors["Entertainment"] = 3202;
	m_mapCategoryColors["Bus./financial"] = 567552;
	m_mapCategoryColors["Sports event"] = 423680;
	m_mapCategoryColors["Newsmagazine"] = 8055;
	m_mapCategoryColors["Sports talk"] = 7730;
	m_mapCategoryColors["Reality"] = 7798784;
	m_mapCategoryColors["Soap"] = 7798893;
	m_mapCategoryColors["Health"] = 3897088;
	m_mapCategoryColors["Action"] = 8131090;
	m_mapCategoryColors["Science fiction"] = 4274446;
	m_mapCategoryColors["Comedy"] = 3820938;
	m_mapCategoryColors["Weather"] = 3836298;
	m_mapCategoryColors["Romance-comedy"] = 9058953;
	m_mapCategoryColors["Crime"] = 1109;
	m_mapCategoryColors["Comedy-drama"] = 9324685;
	m_mapCategoryColors["Musical comedy"] = 4755062;
	m_mapCategoryColors["Animals"] = 149369984;
	m_mapCategoryColors["Baseball"] = 3300920;
	m_mapCategoryColors["Animated"] = 3294302;
	m_mapCategoryColors["Holiday"] = 6173234;
	m_mapCategoryColors["Mystery"] = 3092299;
	m_mapCategoryColors["Musical"] = 4013960;
	m_mapCategoryColors["Paranormal"] = 7032398;
	m_mapCategoryColors["Nature"] = 5270350;
	m_mapCategoryColors["Holiday Special"] = 7794327;
	m_mapCategoryColors["Technology"] = 6384462;
	m_mapCategoryColors["Adventure"] = 3822346;
	m_mapCategoryColors["Exercise"] = 676666;
	m_mapCategoryColors["Fundraiser"] = 669011;
	m_mapCategoryColors["Biography"] = 676630;
	m_mapCategoryColors["Variety"] = 4204112;
	m_mapCategoryColors["War"] = 23398; 
	m_mapCategoryColors["Horror"] = 10158080;
	m_mapCategoryColors["Fashion"] = 7667867;
	m_mapCategoryColors["Historical drama"] = 39804;
	m_mapCategoryColors["Pro wrestling"] = 39702;
	m_mapCategoryColors["Auction"] = 9870080;
	m_mapCategoryColors["Auto"] = 23451;
	m_mapCategoryColors["Motorcycle"] = 36507;
	m_mapCategoryColors["Action sports"] = 4102912;
	m_mapCategoryColors["Medical"] = 10158080;
	m_mapCategoryColors["Politics"] = 4795436;
	m_mapCategoryColors["Docudrama"] = 1382973;
}

void MythTV_PlugIn::BuildAttachedInfraredTargetsMap()
{
	string SQL = 	"SELECT FK_Device, IK_DeviceData "
					"FROM Device_DeviceData "
						"JOIN Device ON PK_Device=FK_Device "
					"WHERE FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " AND "
						"FK_DeviceData=" + StringUtils::itos(DEVICEDATA_MythTV_PVR_Input_CONST);

	PlutoSqlResult result;
	DB_ROW row;

	if( ( result.r=m_pMedia_Plugin->GetMainDatabaseConnection()->db_wrapper_query_result( SQL ) ) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			if( !row[1] || !row[0])
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::BuildAttachedInfraredTargetsMap() found a database row with null entries. Ignoring!");
				continue;
			}

			int PK_Device = atoi(row[0]);
			int mythConnectionID = atoi(row[1]);

			if ( PK_Device == 0 || mythConnectionID == 0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,
							"MythTV_PlugIn::BuildAttachedInfraredTargetsMap() one of the device ID or the mythconnection specification is 0: (device: %d, mythconnID: %d)",
							PK_Device, mythConnectionID);
				continue;
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Mapping MythTV tuner input with ID %d to device ID %d", mythConnectionID, PK_Device);
			m_mapMythInputsToDevices[mythConnectionID] = PK_Device;
		}
	}
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
void MythTV_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void MythTV_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

bool MythTV_PlugIn::StartMedia(class MediaStream *pMediaStream,string &sError)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	MythTvMediaStream *pMythTvMediaStream = NULL;

	if( (pMythTvMediaStream = ConvertToMythMediaStream(pMediaStream, "MythTV_PlugIn::StartMedia() ")) == NULL)
        return false;

    string Response;
	if( !ConfirmSourceIsADestination(pMythTvMediaStream) )
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::StartMedia don't know how media will get to destination.  Unless there's some output zones in the mix results won't be right");
	m_dwTargetDevice = pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;

	vector<DeviceData_Router *> vectDeviceData_Router_PVR;
	DeviceData_Router *pDevice_Myth = m_pRouter->m_mapDeviceData_Router_Find(pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	pDevice_Myth->FindSibblingsWithinCategory(m_pRouter->m_mapDeviceCategory_Find(DEVICECATEGORY_Capture_Cards_CONST),vectDeviceData_Router_PVR);
	for(size_t s=0;s<vectDeviceData_Router_PVR.size();++s)
	{
		DeviceData_Router *pDevice_CaptureCard = vectDeviceData_Router_PVR[s];
		for(size_t s2=0;s2<pDevice_CaptureCard->m_vectDevices_SendingPipes.size();++s2)
		{
			DeviceData_Router *pDevice_Tuner = pDevice_CaptureCard->m_vectDevices_SendingPipes[s2];
			DCE::CMD_On(m_dwPK_Device,pDevice_Tuner->m_dwPK_Device,0,"");
		}
	}

	string sMediaURL = pMythTvMediaStream->GetFilenameToPlay("");

	DCE::CMD_Play_Media cmd(m_dwPK_Device, pMythTvMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,pMythTvMediaStream->m_iPK_MediaType,pMythTvMediaStream->m_iStreamID_get( ),pMediaStream->m_sStartPosition, sMediaURL);
	SendCommand(cmd);

	m_pAlarmManager->CancelAlarmByType(CONFIRM_MASTER_BACKEND_OK);  // Do this immediately 
	m_pAlarmManager->AddRelativeAlarm(1,this,CONFIRM_MASTER_BACKEND_OK,(void*)pMythTvMediaStream->m_iStreamID_get());
	return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool MythTV_PlugIn::StopMedia(class MediaStream *pMediaStream)
{
    PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media stream playback--sending command, waiting for response");

	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);


	vector<DeviceData_Router *> vectDeviceData_Router_PVR;
	DeviceData_Router *pDevice_Myth = m_pRouter->m_mapDeviceData_Router_Find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	pDevice_Myth->FindSibblingsWithinCategory(m_pRouter->m_mapDeviceCategory_Find(DEVICECATEGORY_Capture_Cards_CONST),vectDeviceData_Router_PVR);
	for(size_t s=0;s<vectDeviceData_Router_PVR.size();++s)
	{
		DeviceData_Router *pDevice_CaptureCard = vectDeviceData_Router_PVR[s];
		for(size_t s2=0;s2<pDevice_CaptureCard->m_vectDevices_SendingPipes.size();++s2)
		{
			DeviceData_Router *pDevice_Tuner = pDevice_CaptureCard->m_vectDevices_SendingPipes[s2];
			DCE::CMD_Off(m_dwPK_Device,pDevice_Tuner->m_dwPK_Device,0);
		}
	}

	string sLastPosition;
    DCE::CMD_Stop_Media cmd(m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,0,&sLastPosition);
	// this is useless here because all the message processing in the Media Plugin is done with the m_MediaMutex taken and
	// since this mutex is recursive the release here is useless and the same apply for all the media plugin processing functions.
	mm.Release();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Sending command to stop media to the player: %d", pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	SendCommand(cmd);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::StopMedia(): Returning from stop media command to the player: %d last pos %s", 
		pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, sLastPosition.c_str());
	pMediaStream->m_sLastPosition=sLastPosition;

	return MediaHandlerBase::StopMedia(pMediaStream);
}


class MediaStream *MythTV_PlugIn::CreateMediaStream(class MediaHandlerInfo *pMediaHandlerInfo,int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice,int iPK_Users, deque<MediaFile *> *dequeFilenames,int StreamID)
{
    if ( m_pMedia_Plugin == NULL )
        return NULL;

	EntertainArea *pEntertainArea=NULL;
	if( vectEntertainArea.size() )
		pEntertainArea=vectEntertainArea[0];

    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    if ( pMediaHandlerInfo == NULL )
        return NULL;

    if( (!pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_MythTV_Player_CONST) && pEntertainArea )
    {
        ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_MythTV_Player_CONST);

		if( pListMediaDevice && pListMediaDevice->size())
		{
			if ( pListMediaDevice->size() > 1 )
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "There are multiple Myth Player's (%d devices) in the ent area %d. This is an error picking only the first one!", pListMediaDevice->size(), pEntertainArea->m_iPK_EntertainArea);

		    pMediaDevice = pListMediaDevice->front();
		}
    }

    if( ! pMediaDevice || pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_MythTV_Player_CONST )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Myth plugin being told to play in an entertainment area without a MythTV Player");
        return NULL;
    }

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found playback device to be: %d %s (at IP address: %s)!",
				pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str(),
				pMediaDevice->m_pDeviceData_Router->GetIPAddress().c_str());

    MythTvMediaStream *pMediaStream = new MythTvMediaStream(pMediaHandlerInfo, iPK_MediaProvider, pMediaDevice, 0, st_RemovableMedia,StreamID);

	pMediaStream->m_sMediaDescription = "Not available";
    pMediaStream->m_sSectionDescription = "Not available";
    pMediaStream->m_sMediaSynopsis = "Not available";
    m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;

    return pMediaStream;
}


MythTvMediaStream* MythTV_PlugIn::ConvertToMythMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_MYTHTV )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a MythTvMediaStream!").c_str());
		return NULL;
	}

	return static_cast<MythTvMediaStream*>(pMediaStream);
}

/**
 * GetPicturePath - sister function to grab picture for a given MythTV recording.
 */
bool MythTV_PlugIn::GetPicturePath(string sBaseName, string sHostName, string sStorageGroup, string &sPicturePath)
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Plugin::GetPicturePath(%s,%s,%s)",sBaseName.c_str(),sHostName.c_str(),sStorageGroup.c_str());
  string sSQL = "SELECT dirname FROM storagegroup WHERE hostname = '"+sHostName+"' AND groupname = '"+sStorageGroup+"'";
  PlutoSqlResult result;
  DB_ROW row;
  if ((result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Plugin::GetPicturePath rows %d",result.r->row_count);
      while((row=db_wrapper_fetch_row(result.r)))
	{
	  string sDirName = row[0];
	  string sFilePath = sDirName + "/" + sBaseName + ".png";
	  string sConvertedPath = sDirName + "/" + sBaseName + ".jpg";
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::GetPicturePath - Trying path %s",sFilePath.c_str());
	  if (FileUtils::FileExists(sFilePath))
	    {
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Plugin::GetPicturePath found picture at path %s",sFilePath.c_str());
	      sPicturePath = sConvertedPath;
	      // Convert the image using imagemagick to jpeg so Media PlugIn is happy, if needed.
	      if (!FileUtils::FileExists(sConvertedPath))
		{
		  string sCmd = "convert "+sFilePath+" "+sConvertedPath;
		  system(sCmd.c_str());
		}
	      return true;
	    }
	}
    }
  return false;
}

/** 
 * GetRecordingPicture - find out if a recording already has a PK_Picture, and if so
 * return it.
 */
bool MythTV_PlugIn::GetRecordingPicture(string sPath,int &iPK_Picture)
{
  vector<Row_Picture *> v_Rows;
  iPK_Picture=0;
  if (m_pMedia_Plugin->m_pDatabase_pluto_media->Picture_get()->GetRows("URL='"+sPath+"'",&v_Rows))
    {
      if (!v_Rows.empty())
	{
	  iPK_Picture=v_Rows[0]->PK_Picture_get();
	  return true;
	}
    }

  return false;

}

string MythTV_PlugIn::GetRecordingURL(string sBaseName, string sHostName, string sStorageGroup)
{
  int iPK_Picture;
  string sRet="";
  string sFullPath="";
  string sFilePath="";
  string sSQL = "SELECT dirname FROM storagegroup WHERE hostname = '"+sHostName+"' AND groupname = '"+sStorageGroup+"'";
  PlutoSqlResult result;
  DB_ROW row;
  if ((result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Plugin::GetPicturePath rows %d",result.r->row_count);
      while((row=db_wrapper_fetch_row(result.r)))
	{
	  string sDirName = row[0];
	  string sCurrPath = sDirName + "/" + sBaseName + ".jpg";
	  sFilePath=sDirName+"/"+sBaseName;
	  if (FileUtils::FileExists(sCurrPath))
	    {
	      sFullPath=sCurrPath;
	      break;
	    }
	}
      
      // If path found, Grab the matching PK_Picture 

      if (!sFullPath.empty())
	{
	  vector<Row_Picture *> v_Rows;
	  iPK_Picture=0;
	  if (m_pMedia_Plugin->m_pDatabase_pluto_media->Picture_get()->GetRows("URL='"+sFullPath+"'",&v_Rows))
	    {
	      if (!v_Rows.empty())
		{
		  iPK_Picture = v_Rows[0]->PK_Picture_get();
		}
	    }
	}
    }
  sRet = sFilePath + "|" + StringUtils::itos(iPK_Picture) + ".pvrtv"; // hackorama for media plugin
  return sRet;
}

/**
 * PopulateDataGrid - populate the datagrid for Recorded TV Display
 */
void MythTV_PlugIn::PopulateDataGrid(string sToken,MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign )
{

  string sSQL, sDescription, sMediaURL, sDateTime, sPictureFilename;

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"XXX POPULATEDATAGRID XXX PK_AttributeType_Sort %d sPK_Attribute is %s",PK_AttributeType_Sort, sPK_Attribute.c_str());

  if (PK_AttributeType_Sort == ATTRIBUTETYPE_Title_CONST)
    {
      if (!sPK_Attribute.empty())
	{
	  // If this is not empty, we are drilling down into a show.

	  // parse the attribute which is Series ID|Program ID
	  string::size_type tokenPos = 0;
	  string sSeriesId = StringUtils::Tokenize(sPK_Attribute,"|",tokenPos);
	  string sProgramId = StringUtils::Tokenize(sPK_Attribute,"|",tokenPos);

	  sSQL = "SELECT title, seriesid, programid, basename, subtitle, description, CONVERT_TZ(lastmodified,'+00:00','"+m_sTzOffset+"'), hostname, storagegroup from recorded WHERE seriesid = '"+sSeriesId+"' OR programid = '"+sProgramId+"'";
 	  PlutoSqlResult result;
	  DB_ROW row;
	  if ((result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL)
	    {
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::PopulateDataGrid title rows %d",result.r->row_count);
	      while((row=db_wrapper_fetch_row(result.r)))
		{
		  string sTitle = row[0];
		  string sSeriesId = row[1];
		  string sProgramId = row[2];
		  string sBaseName = row[3];
		  string sSubtitle = row[4];
		  string sDesc = row[5];
		  string sDateTime = row[6];
		  string sHostName = row[7];
		  string sStorageGroup = row[8];
		  string sPicturePath;

		  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Plugin::PopulateDataGrid - drilldown sTitle /%s/ sSubtitle /%s/ sDesc /%s/ ",sTitle.c_str(),sSubtitle.c_str(),sDesc.c_str());


		  // Build a visible description from title, subtitle, description.
		  // Title is always used. If subtitle is available then it is used.
		  // If subtitle is empty, then description is used IF it is below
		  // characters.

		  if (sSubtitle.empty())
		    {
		      if ((sDesc.size() > 0) && (sDesc.size() < 25))
			{
			  sDescription = sTitle + "\n\"" + sDesc + "\"";
			}
		      else
			{
			  sDescription = sTitle;
			}
		    }
		  else
		    {
		      sDescription = sTitle = "\n\"" + sSubtitle + "\"";
		    }

		  sMediaURL = sBaseName;
		  FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(sDescription,"!E,"+sToken+","+GetRecordingURL(sMediaURL,sHostName,sStorageGroup),0,StringUtils::SQLDateTime(sDateTime));
		  if (GetPicturePath(sBaseName,sHostName,sStorageGroup,sPicturePath))
		    {
		      int iPK_Picture=0;
		      if (!GetRecordingPicture(sPicturePath,iPK_Picture))
			{
			  // No recording picture currently exists for this recording, try to add one.
			  Row_Picture *pRecordingPicture = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(sPicturePath);
			  if (pRecordingPicture)
			    {
			      pFileBrowserInfo->m_PK_Picture = pRecordingPicture->PK_Picture_get();
			    }
			  delete pRecordingPicture;
			}
		      else
			{
			  // A picture already exists, grab the existing record.
			  pFileBrowserInfo->m_PK_Picture = iPK_Picture;
			}
		    }
		  pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
	    } // end
	}
      else
	{
	  // Title, Top level, show one of each.
	  sSQL = "SELECT title, COUNT(title) as numitems, seriesid, programid, basename, subtitle, description, CONVERT_TZ(lastmodified,'+00:00','"+m_sTzOffset+"'), hostname, storagegroup from recorded GROUP by title;";
 	  PlutoSqlResult result;
	  DB_ROW row;
	  if ((result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL)
	    {
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::PopulateDataGrid title rows %d",result.r->row_count);
	      while((row=db_wrapper_fetch_row(result.r)))
		{
		  string sTitle = row[0];
		  int iNumItems = atoi(row[1]);
		  string sSeriesId = row[2];
		  string sProgramId = row[3];
		  string sBaseName = row[4];
		  string sSubtitle = row[5];
		  string sDesc = row[6];
		  string sDateTime = row[7];
		  string sHostName = row[8];
		  string sStorageGroup = row[9];
		  string sPicturePath;

		  // Build a visible description from title, subtitle, description.
		  // Title is always used. If subtitle is available then it is used.
		  // If subtitle is empty, then description is used IF it is below
		  // characters.

		  if (sSubtitle.empty())
		    {
		      if (sDesc.size() < 25)
			{
			  sDescription = sTitle + "\n\"" + sDesc + "\"";
			}
		      else
			{
			  sDescription = sTitle;
			}
		    }
		  else
		    {
		      sDescription = sTitle + "\n\"" + sSubtitle + "\"";
		    }

		  // If there are multiple items, then make a drilldown, otherwise
		  // make a direct play link to send off to the MythTV_Player
		  
		  if (iNumItems>1)
		    {
		      sDescription = sTitle + " (" + StringUtils::itos(iNumItems) + " items)";
		      sMediaURL = sSeriesId+"|"+sProgramId;
		      FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(sDescription,"!e,"+sToken+","+sMediaURL,0,StringUtils::SQLDateTime(sDateTime));
		      if (GetPicturePath(sBaseName,sHostName,sStorageGroup,sPicturePath))
			{
			  int iPK_Picture;
			  if (!GetRecordingPicture(sPicturePath,iPK_Picture))
			    {
			      // No recording picture currently exists for this recording, try to add one.
			      Row_Picture *pRecordingPicture = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(sPicturePath);
			      if (pRecordingPicture)
				{
				  pFileBrowserInfo->m_PK_Picture = pRecordingPicture->PK_Picture_get();
				}
			      delete pRecordingPicture;
			    }
			  else
			    {
			      // A picture already exists, grab the existing record.
			      pFileBrowserInfo->m_PK_Picture = iPK_Picture;
			    }
			}
		      pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		    }
		  else
		    {
		      sMediaURL = sBaseName;
		      FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(sDescription,"!E,"+sToken+","+GetRecordingURL(sMediaURL,sHostName,sStorageGroup),0,StringUtils::SQLDateTime(sDateTime));
		      // COME BACK HERE
		      if (GetPicturePath(sBaseName,sHostName,sStorageGroup,sPicturePath))
			{
			  int iPK_Picture;
			  if (!GetRecordingPicture(sPicturePath,iPK_Picture))
			    {
			      // No recording picture currently exists for this recording, try to add one.
			      Row_Picture *pRecordingPicture = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(sPicturePath);
			      if (pRecordingPicture)
				{
				  pFileBrowserInfo->m_PK_Picture = pRecordingPicture->PK_Picture_get();
				}
			      delete pRecordingPicture;
			    }
			  else
			    {
			      // A picture already exists, grab the existing record.
			      pFileBrowserInfo->m_PK_Picture = iPK_Picture;
			    }
			}
		      pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo); 
		    }
		}
	    } // end	  
	}
    }
  else if (PK_AttributeType_Sort == ATTRIBUTETYPE_Genre_CONST)
    {
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::PopulateDataGrid() Somehow we fell through.");
    }

  return;
}

void MythTV_PlugIn::GetExtendedAttributes(string sType, string sPK_MediaSource, string sURL, string *sValue_To_Assign)
{

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_Plugin::GetExtendedAttributes sType is %s",sType.c_str());

  size_t pos=0;

  string sFinalDescription, sSynopsis;

  string sMediaURL = StringUtils::Tokenize(sURL,"|",pos);
  string sPK_Picture = StringUtils::Tokenize(sURL,"|",pos);
  string sFileName = FileUtils::FilenameWithoutPath(sMediaURL);
  string sPicture = "/home/mediapics/"+sPK_Picture+".jpg";
 
  // Get title and synopsis from sMediaURL

  string sSQL = "SELECT title, subtitle, description FROM recorded WHERE basename = '"+FileUtils::FilenameWithoutPath(sMediaURL)+"'";
  PlutoSqlResult result;
  DB_ROW row;
  if ((result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL)
    {
      while((row=db_wrapper_fetch_row(result.r)))
	{
	  string sTitle = row[0];
	  string sSubtitle = row[1];
	  string sDesc = row[2];
	  string sDescription;
	
	  sSynopsis=sDesc;

	  if (sSubtitle.empty())
	    {
	      if (sDesc.size() < 25)
		{
		  sDescription = sTitle + "\n\"" + sDesc + "\"";
		}
	      else
		{
		  sDescription = sTitle;
		}
	    }
	  else
	    {
	      sDescription = sTitle + "\n\"" + sSubtitle + "\"";
	    }
	}
    }
  
  *sValue_To_Assign = 
    "FILE\t" + sFileName + "\t" + 
    "TITLE\t" + sFinalDescription + "\t" +
    "SYNOPSIS\t" + sSynopsis + "\t" +
    "PICTURE\t" + sPicture;

  return;

}

// Parms = Users,PK_EntertainmentArea
// All channels
class DataGridTable *MythTV_PlugIn::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign,
	Message *pMessage)
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();

	DataGridTable *pDataGridTable = new DataGridTable();

	string::size_type pos=0;
	int iPK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iPK_EntertainArea = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::AllShows A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());
    
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( iPK_EntertainArea );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::AllShows cannot find a stream %p",pEntertainArea);
		return pDataGridTable;
	}

	string sProvider;
	map<int,bool> mapVideoSourcesToUse;
	// The source is 0, means all myth channels and video sources.  Otherwise, just those for the given device (like a cable box)
	int PK_Device_Source = pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_LiveTV_CONST ? 0 : pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	list_int *p_list_int = NULL;
	if( PK_Device_Source && m_mapDevicesToSources.find(PK_Device_Source)!=m_mapDevicesToSources.end() )
		p_list_int = &(m_mapDevicesToSources[PK_Device_Source]);

	// Go through all the channels, and set the cell to NULL if we're not supposed to include it
	// or to a new value if we are.  We'll update the description with the show down below
	for(ListMythChannel::iterator it=m_ListMythChannel.begin();it!=m_ListMythChannel.end();++it)
	{
		MythChannel *pMythChannel = *it;
		if( p_list_int )
		{
			bool bOk=false;
			for(list_int::iterator it=p_list_int->begin();it!=p_list_int->end();++it)
			{
				if( *it==pMythChannel->m_pMythSource->m_dwID )
				{
					bOk=true;
					break;
				}
			}
			if( !bOk )
			{
				pMythChannel->m_pCell=NULL;
				continue;
			}
		}
		string sChannelName = StringUtils::itos(pMythChannel->m_dwChanNum) + " " + pMythChannel->m_sShortName;
		pMythChannel->m_pCell = new DataGridCell(sChannelName,"i" + StringUtils::itos(pMythChannel->m_dwID));
		pMythChannel->m_pCell->m_mapAttributes["Name"] = sChannelName;
		pMythChannel->m_pCell->m_mapAttributes["Source"] = pMythChannel->m_pMythSource->m_sDescription;
	}

	// When tune to channel gets an 'i' in front, it's assumed that it's a channel id
	string sSQL = "SELECT chanid, title, CONVERT_TZ(starttime,'+00:00','"+m_sTzOffset+"') as tz_starttime, CONVERT_TZ(endtime,'+00:00','"+m_sTzOffset+"') as tz_endtime, seriesid, programid "
		"FROM program "
		"WHERE starttime < UTC_TIMESTAMP() AND endtime > UTC_TIMESTAMP() " + sProvider;

	bool bAllSource = mapVideoSourcesToUse.empty();
	int iRow=0;
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::AllShows rows %d map %d allsource %d sources to use %d",
			result.r->row_count, (int) m_mapMythChannel.size(), (int) bAllSource, (int) mapVideoSourcesToUse.size() );

		while((row = db_wrapper_fetch_row(result.r)))
		{
			MythChannel *pMythChannel = m_mapMythChannel_Find( atoi(row[0]) );
			if( !pMythChannel || !pMythChannel->m_pCell )
				continue; // Shouldn't happen that pMythChannel is NULL, if cell is, we're not including it

			if( bAllSource==false && mapVideoSourcesToUse[ pMythChannel->m_pMythSource->m_dwID ]==false )  // Not a source for this list
				continue;

			pMythChannel->m_pCell->SetText( string(pMythChannel->m_pCell->GetText()) + row[1] );
			if( pMythChannel->m_pPic )
			{
				char *pPic = new char[ pMythChannel->m_Pic_size ];
				memcpy(pPic,pMythChannel->m_pPic,pMythChannel->m_Pic_size);
				pMythChannel->m_pCell->SetImage( pPic, pMythChannel->m_Pic_size, GR_JPG );
			}

			time_t tStart = StringUtils::SQLDateTime( row[2] );
			time_t tStop = StringUtils::SQLDateTime( row[3] );
			string sTime = StringUtils::HourMinute(tStart) + " - " + StringUtils::HourMinute(tStop);
			string sNumber = NULL != row[0] ? row[0] : "";
			string sInfo = NULL != row[4] ? row[1]: "";

			pMythChannel->m_pCell->m_mapAttributes["Number"] = sNumber;
			pMythChannel->m_pCell->m_mapAttributes["Time"] = sTime;
			pMythChannel->m_pCell->m_mapAttributes["Info"] = sInfo;
			if( row[4] )
				pMythChannel->m_pCell->m_mapAttributes["Series"] = row[4];
			if( row[5] )
				pMythChannel->m_pCell->m_mapAttributes["Program"] = row[5];
			MapBookmark *pMapBookmark_Series_Or_Program;
			MapBookmark::iterator it;
			if( (it=pMythChannel->m_mapBookmark.find(0))!=pMythChannel->m_mapBookmark.end() ||
				(it=pMythChannel->m_mapBookmark.find(iPK_Users))!=pMythChannel->m_mapBookmark.end() )
			{
				// It's a user's favorited channel
				pMythChannel->m_pCell->m_mapAttributes["PK_Bookmark"] = it->second;
				pDataGridTable->SetData(0,iRow++,new DataGridCell(pMythChannel->m_pCell)); // A copy since we'll add this one later too
			}
			else if( row[4] &&
				(pMapBookmark_Series_Or_Program=m_mapSeriesBookmarks_Find(row[4])) &&
				(
					(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
					(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
				))
			{
				// It's a user's favorited series
				pDataGridTable->SetData(0,iRow++,new DataGridCell(pMythChannel->m_pCell)); // A copy since we'll add this one later too
			}
			else if( row[5] &&
				(pMapBookmark_Series_Or_Program=m_mapProgramBookmarks_Find(row[5])) &&
				(
					(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
					(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
				))
			{
				// It's a user's favorited series.
				pDataGridTable->SetData(0,iRow++,new DataGridCell(pMythChannel->m_pCell)); // A copy since we'll add this one later too
			}
		}
	}

	for(ListMythChannel::iterator it=m_ListMythChannel.begin();it!=m_ListMythChannel.end();++it)
	{
		MythChannel *pMythChannel = *it;
		if( pMythChannel->m_pCell )
			pDataGridTable->SetData(0,iRow++,pMythChannel->m_pCell);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::AllShows cells: %d source %d plist %p map %d",
		(int) pDataGridTable->m_MemoryDataTable.size(), PK_Device_Source, p_list_int, (int) m_mapDevicesToSources.size() );

	return pDataGridTable;
}

// A Traditional EPG Grid
class DataGridTable *MythTV_PlugIn::PVREPGGrid(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
  DataGridTable *pDataGridTable = new DataGridTable();

  if (m_bBookmarksNeedRefreshing)
    RefreshBookmarks();

  string::size_type tokenPos=0;
  int iPK_Users = atoi(StringUtils::Tokenize(Parms,",",tokenPos).c_str());
  int iPK_EntertainArea = atoi(StringUtils::Tokenize(Parms,",",tokenPos).c_str());

  PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::EPGGrid() a datagrid for the EPG was requested %s params %s", GridID.c_str(), Parms.c_str());

  EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(iPK_EntertainArea);

  if (!pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source)
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::EPGGrid can't find a stream %p",pEntertainArea);
      // Come back here and do a data grid if a stream isn't available. For now, just be blank.
      return pDataGridTable;
    }
  
  // Row 0 column 0 is a blank 'shim' column.
  DataGridCell *pShimColumn = new DataGridCell("2010\nJun 17","");
  pDataGridTable->SetData(0,0,pShimColumn);

  // Do the time columns.
  // -------------------------------------------------------
  // There are 12 cells, in an hour block (5 min increments)
  // 6 cells in a 30 minute block
  // 288 cells in all.

  int iTimeColumn = 1; // Time column counter . The First column is blank (Channel)

  // Do the AM slots.
  for (int i=0;i<12;i++)
    {
      int iVisualTimeIndex = i;
      string sVisualTimeHour, sActualTimeHour, sVisualTimeHalfHour, sActualTimeHalfHour;
      ostringstream ssVisualTimeHour, ssActualTimeHour, ssVisualTimeHalfHour, ssActualTimeHalfHour;
      
      // Show midnight as 12:00 AM
      if (i == 0)
	iVisualTimeIndex = 12;

      // Construct the streams
      ssVisualTimeHour << iVisualTimeIndex << ":" << "00 AM";
      ssActualTimeHour << i << ":" << "00";
      ssVisualTimeHalfHour << iVisualTimeIndex << ":" << "30 AM";
      ssActualTimeHalfHour << i << ":" << "30";

      // Build the strings.
      sVisualTimeHour = ssVisualTimeHour.str();
      sActualTimeHour = ssActualTimeHour.str();
      sVisualTimeHalfHour = ssVisualTimeHalfHour.str();
      sActualTimeHalfHour = ssVisualTimeHalfHour.str();

      DataGridCell *pCellHour = new DataGridCell(sVisualTimeHour,sActualTimeHour);
      pCellHour->m_Colspan = 6;
      DataGridCell *pCellHalfHour = new DataGridCell(sVisualTimeHalfHour,sActualTimeHalfHour);
      pCellHalfHour->m_Colspan = 6;

      pDataGridTable->SetData(iTimeColumn, 0, pCellHour);
      iTimeColumn += 6;
      pDataGridTable->SetData(iTimeColumn, 0, pCellHalfHour);
      iTimeColumn += 6;

    }

  // Do the PM slots.
  for (int i=0;i<12;i++)
    {
      int iVisualTimeIndex = i;
      string sVisualTimeHour, sActualTimeHour, sVisualTimeHalfHour, sActualTimeHalfHour;
      ostringstream ssVisualTimeHour, ssActualTimeHour, ssVisualTimeHalfHour, ssActualTimeHalfHour;
      
      // Show midnight as 12:00 AM
      if (i == 0)
	iVisualTimeIndex = 12;

      // Construct the streams
      ssVisualTimeHour << iVisualTimeIndex << ":" << "00 PM";
      ssActualTimeHour << i+12 << ":" << "00";
      ssVisualTimeHalfHour << iVisualTimeIndex << ":" << "30 PM";
      ssActualTimeHalfHour << i+12 << ":" << "30";

      // Build the strings.
      sVisualTimeHour = ssVisualTimeHour.str();
      sActualTimeHour = ssActualTimeHour.str();
      sVisualTimeHalfHour = ssVisualTimeHalfHour.str();
      sActualTimeHalfHour = ssVisualTimeHalfHour.str();

      DataGridCell *pCellHour = new DataGridCell(sVisualTimeHour,sActualTimeHour);
      pCellHour->m_Colspan = 6;
      pCellHour->m_mapAttributes["timeslot"] = sVisualTimeHour;
      DataGridCell *pCellHalfHour = new DataGridCell(sVisualTimeHalfHour,sActualTimeHalfHour);
      pCellHalfHour->m_mapAttributes["timeslot"] = sVisualTimeHour;
      pCellHalfHour->m_Colspan = 6;

      pDataGridTable->SetData(iTimeColumn, 0, pCellHour);
      iTimeColumn += 6;
      pDataGridTable->SetData(iTimeColumn, 0, pCellHalfHour);
      iTimeColumn += 6;

    }

  // Do the channels.
  for (ListMythChannel::iterator it=m_ListMythChannel.begin();it!=m_ListMythChannel.end();++it)
    {
      MythChannel *pMythChannel = *it;
      string sChannelName = StringUtils::itos(pMythChannel->m_dwChanNum) + " "+pMythChannel->m_sShortName;
      pMythChannel->m_pCell = new DataGridCell(sChannelName,"i"+StringUtils::itos(pMythChannel->m_dwID));
      pMythChannel->m_pCell->m_mapAttributes["Name"] = sChannelName;
    }

  // Grab the current channels and programs for current time.
  string sCurrentSQL = "SELECT chanid, title, CONVERT_TZ(starttime,'+00:00','"+m_sTzOffset+"') as tz_starttime, CONVERT_TZ(endtime,'+00:00','"+m_sTzOffset+"') as tz_endtime, seriesid, programid "
    "FROM program "
    "WHERE starttime < UTC_TIMESTAMP() AND endtime > UTC_TIMESTAMP()";

  PlutoSqlResult result;
  DB_ROW row;

  if ( ( result.r=m_pDBHelper_Myth->db_wrapper_query_result(sCurrentSQL) ) != NULL  )
    {
      // Rows were returned, process them
      while ((row = db_wrapper_fetch_row(result.r)))
	{
	  MythChannel *pMythChannel = m_mapMythChannel_Find(atoi(row[0])); // row[0] is chanid
	  if ( !pMythChannel || !pMythChannel->m_pCell )
	    continue;  // Technically, shouldn't happen.

	  // Append the current show to the end of the channel description. 
	  pMythChannel->m_pCell->SetText( string(pMythChannel->m_pCell->GetText()) + row[1] );

	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Row: %s %s",row[0],row[1]);

	  // If Channel picture is attached, use it.
	  if ( pMythChannel->m_pPic )
	    {
	      char *pPic = new char[ pMythChannel->m_Pic_size ];
	      memcpy(pPic, pMythChannel->m_pPic, pMythChannel->m_Pic_size);
	      pMythChannel->m_pCell->SetImage( pPic, pMythChannel->m_Pic_size, GR_JPG );
	    }

	  // Set Channel and Show attributes in this data grid cell.
	  string sChannelID = (NULL != row[0] ? row[0] : "");
	  string sShowTitle = (NULL != row[1] ? row[1] : "");

	  pMythChannel->m_pCell->m_mapAttributes["ChannelID"] = sChannelID;
	  pMythChannel->m_pCell->m_mapAttributes["ShowTitle"] = sShowTitle;

	  // FIXME: Come back here and write code for Channel bookmarks!
	  
	}
     
       m_vectRowToChannels.push_back(0); // First row is empty.
    
      int iRow = 1;

      // And finally, slap all the channels into the cells in our pDataGridTable
      for (ListMythChannel::iterator it=m_ListMythChannel.begin();it!=m_ListMythChannel.end();++it)
	{
	  MythChannel *pMythChannel = *it;
	  if ( pMythChannel->m_pCell )
	    { 
	      pDataGridTable->SetData(0, iRow++, pMythChannel->m_pCell);
	      m_vectRowToChannels.push_back(pMythChannel->m_dwID);
	    } 
	}

    }

  // Now, fill in the guide data in the middle. For now, this query is fixed to today
  // so that I can work on the column span logic.
  
  string sGuideSQL = "SELECT * from program WHERE ( CONVERT_TZ(endtime,'+00:00','"+m_sTzOffset+"') > '2010-06-20' AND CONVERT_TZ(endtime,'+00:00','"+m_sTzOffset+"') < '2010-06-21')";

  if ( ( result.r=m_pDBHelper_Myth->db_wrapper_query_result(sGuideSQL) ) != NULL  )
    {
      // Rows were returned, process them
      int iRow = 1;
      while ((row = db_wrapper_fetch_row(result.r)))
	{
	  // row[0] = channel ID
	  // row[1] = starttime
	  // row[2] = endtime
	  // row[3] = title
	  // row[4] = subtitle
	  // row[5] = description	
	  // row[6] = category (Comedy)
	  // row[7] = category_type (series or blank)
	  // row[8] = airdate
	  // row[9] = stars
	  // row[10] = previouslyshown (boolean)
	  // row[11] = title_pronounce (???)
	  // row[12] = stereo (boolean)
	  // row[13] = subtitled (boolean)
	  // row[14] = hdtv (boolean)
	  // row[15] = closedcaptioned (boolean)
	  // row[16] = partnumber (x of y)
	  // row[17] = parttotal
	  // row[18] = seriesID
	  // row[19] = original Airdate
	  // row[20] = showtype (Series)
	  // row[21] = colorcode (???)
	  // row[22] = syndicated episode number
	  // row[23] = program ID
	  // row[24] = manual ID
	  // row[25] = generic (boolean)
	  // row[26] = listing source (mythtv-setup)
	  // row[27] = First
	  // row[28] = Last occurance
	  // row[29] = subtitletypes
	  // row[30] = videoprop

	  string sChannelID = row[0];
	  string sStartTime = row[1];
	  string sEndTime = row[2];
	  string sShowTitle = row[3]; // FIXME: come back and handle show subtitles.
	  string sCategory = row[6];
	  string sShowID = sChannelID + "," + sStartTime + "," + sEndTime;
	  int iTargetRow = FindTargetRowForChanID(sChannelID);
	  int iTargetColumn = FindTargetColForStartTime(sStartTime, sEndTime);
	  int iColSpan = FindColSpanForEndTime(sStartTime, sEndTime);
//	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sChannelID %s sStartTime %s sEndTime %s sShowTitle %s iTargetRow %i iTargetColumn %i iColSpan %i",sChannelID.c_str(), sStartTime.c_str(), sEndTime.c_str(), sShowTitle.c_str(), iTargetRow, iTargetColumn, iColSpan);
	  DataGridCell *pCell = new DataGridCell(sShowTitle, sShowID);
	  pCell->m_Colspan = iColSpan;
	  pCell->m_AltColor = mapCategoryColor_Find(sCategory);
	  pDataGridTable->SetData(iTargetColumn,iTargetRow,pCell);
	  iRow++;
	}
      
    }

  return pDataGridTable;

}

int MythTV_PlugIn::FindTargetRowForChanID(string sChannelID)
{
	MythChannel *pMythChannel = m_mapMythChannel_Find(atoi(sChannelID.c_str()));
	int iChan = pMythChannel->m_dwID;
	
	vector<int>::iterator it;

	int iRow = 0;
	for (it = m_vectRowToChannels.begin(); it != m_vectRowToChannels.end(); ++it)
	{
		if (*it == iChan)
			return iRow;

		iRow++; 
	} 
}

int MythTV_PlugIn::FindTargetColForStartTime(string sStartTime,string sEndTime)
{
	// Convert sql time into a time struct we can deal with more easily.
	time_t tvStartTime = StringUtils::SQLDateTime(sStartTime);
	time_t tvEndTime = StringUtils::SQLDateTime(sEndTime);
	struct tm tStartTime;
	struct tm tEndTime;
	localtime_r(&tvStartTime,&tStartTime);
	localtime_r(&tvEndTime,&tEndTime);

	// Figure out where our first column starts.
	int hourColumn;
	if (tStartTime.tm_mday != tEndTime.tm_mday)
	{
		hourColumn = 0;	// This starts sometime the day before, set to 0
	} else
	{
		hourColumn = tStartTime.tm_hour * 12;
	}
	int minuteColumn = ceil(tStartTime.tm_min/5);
	int columnStartTime = hourColumn+minuteColumn + 1;   // + 1 is the Channel Column
	return columnStartTime;
}

int MythTV_PlugIn::FindColSpanForEndTime(string sStartTime, string sEndTime)
{
	time_t tvStartTime = StringUtils::SQLDateTime(sStartTime);
	time_t tvEndTime = StringUtils::SQLDateTime(sEndTime);
	struct tm tEndTime;
	struct tm tStartTime;
	localtime_r(&tvEndTime,&tEndTime);
	localtime_r(&tvStartTime,&tStartTime);

	// If the show actually winds up spilling into the net day
	// then we need to make it so that the column span = the very last column...

	int startColumn = FindTargetColForStartTime(sStartTime, sEndTime); 
	int endHourColumn = tEndTime.tm_hour * 12;
	int endMinuteColumn = ceil(tEndTime.tm_min/5);
	int endColumn;
	if (tStartTime.tm_mday < tEndTime.tm_mday)
	{
		endColumn = 288; // (12 * 12) * 2 = last possible cell in 24 hr period.
	}
	else
	{
		endColumn = endHourColumn + endMinuteColumn;
	}
	int colspan = endColumn - startColumn + 1;	// + 1 is the Channel column
	return colspan;
}

// The shows on a given channel
class DataGridTable *MythTV_PlugIn::CurrentShows(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,Message *pMessage)
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();
    int nHeight = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Height_CONST].c_str());
	DataGridTable *pDataGridTable = new DataGridTable();
	
	string::size_type pos = 0;
	string sChanId = StringUtils::Tokenize( Parms, ",", pos);
	if( sChanId.size() && sChanId[0]=='i' )
		sChanId = sChanId.substr(1);
	int iPK_Users = atoi(StringUtils::Tokenize( Parms, ",", pos).c_str());
	bool bOnePageOnly = pos < Parms.size(); // If there's trailing data, we're supposed to only cache one page
	
	if( atoi(sChanId.c_str())==0 )
		return pDataGridTable;

	time_t tNow = time(NULL);
	struct tm t;
	localtime_r(&tNow,&t);
	int Month = t.tm_mon;
	int Day = t.tm_mday;

	DataGridCell *pCell;

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::CurrentShows A datagrid for all the shows was requested %s params %s record m_mapScheduledRecordings size: %d",
		GridID.c_str(), Parms.c_str(), (int) m_mapScheduledRecordings.size() );

	MythRecording mythRecording;
	mythRecording.channel_id = atoi(sChanId.c_str());

	string sProvider;
	// When tune to channel gets an 'i' in front, it's assumed that it's a channel id
	string sSQL =
		"SELECT program.chanid, program.programid, program.seriesid, title, CONVERT_TZ(starttime,'+00:00','"+m_sTzOffset+"') as tz_starttime, CONVERT_TZ(endtime,'+00:00','"+m_sTzOffset+"') as tz_endtime, description, Picture_series.EK_Picture AS Picture_series, Picture_program.EK_Picture AS Picture_program "
		"FROM program "
		"LEFT JOIN `pluto_myth`.`Picture` AS Picture_series ON Picture_series.seriesid=program.seriesid "
		"LEFT JOIN `pluto_myth`.`Picture` AS Picture_program ON Picture_program.programid=program.programid "
		"WHERE program.chanid=" + sChanId + " AND endtime > UTC_TIMESTAMP() ORDER BY endtime";

	if( bOnePageOnly )
		sSQL += " LIMIT " + StringUtils::itos(nHeight);

	PlutoSqlResult result;
	DB_ROW row;
	int iRow=0;
	char szRecording[2];
	szRecording[1]=0; // This will be a 1 character null terminated string
	map< u_int64_t, pair<char,int> >::iterator it_mapScheduledRecordings;
	MapBookmark *pMapBookmark_Series_Or_Program;
	MapBookmark::iterator it;

for(map< u_int64_t, pair<char,int> >::iterator itmr=m_mapScheduledRecordings.begin();itmr!=m_mapScheduledRecordings.end();++itmr)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"contains " UINT64_PRINTF,itmr->first);
}

	if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL )
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			pCell = new DataGridCell(row[3] ? row[3] : "","i" + Parms);
			pCell->m_mapAttributes["chanid"]=row[0];
			pCell->m_mapAttributes["programid"]=row[1];
			pCell->m_mapAttributes["seriesid"]=row[2];
			time_t tStart = row[4] ? StringUtils::SQLDateTime( row[4] ) : 0;
			time_t tStop = row[5] ? StringUtils::SQLDateTime( row[5] ) : 0;
			pCell->m_mapAttributes["starttime"]=StringUtils::itos(tStart);
			pCell->m_mapAttributes["endtime"]=StringUtils::itos(tStop);
			mythRecording.scheduled_start_time = tStart;
			it_mapScheduledRecordings = m_mapScheduledRecordings.find(mythRecording.key());
			if (it_mapScheduledRecordings != m_mapScheduledRecordings.end())
			{
				szRecording[0] = it_mapScheduledRecordings->second.first;
				pCell->m_mapAttributes["recording"] = szRecording;
				pCell->m_mapAttributes["recordid"] = StringUtils::itos(it_mapScheduledRecordings->second.second);
			}

			struct tm t;
			localtime_r(&tStart,&t);
			string sDate;
			if( t.tm_mon==Month && t.tm_mday==Day )
				sDate = "Today";
			else
				sDate = StringUtils::itos(t.tm_mon+1) + "/" + StringUtils::itos(t.tm_mday);

			string sTime = StringUtils::HourMinute(tStart) + "-" + StringUtils::HourMinute(tStop);
			
			string sPK_Picture;
			if( row[8] && row[8][0] )
				sPK_Picture = row[8];
			else if( row[7] && row[7][0] )
				sPK_Picture = row[7];

			if( sPK_Picture.empty()==false )
			{
				size_t size;
				char *pPic = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + sPK_Picture + "_tn.jpg",size);
				if( pPic )
					pCell->SetImage( pPic, size, GR_JPG );
			}

			if( row[2] &&
				(pMapBookmark_Series_Or_Program=m_mapSeriesBookmarks_Find(row[2])) &&
				(
					(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
					(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
				))
			{
				// It's a user's favorited series
				pCell->m_mapAttributes["PK_Bookmark"] = it->second;
			}
			else if( row[1] &&
				(pMapBookmark_Series_Or_Program=m_mapProgramBookmarks_Find(row[1])) &&
				(
					(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
					(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
				))
			{
				// It's a user's favorited series.
				pCell->m_mapAttributes["PK_Bookmark"] = it->second;
			}

			pCell->m_mapAttributes["Date"] = sDate;
			pCell->m_mapAttributes["Time"] = sTime;
			if( row[6] )
				pCell->m_mapAttributes["Synopsis"] = row[6];

			pDataGridTable->SetData(0,iRow++,pCell);
		}
	}
 
	return pDataGridTable;
}

bool MythTV_PlugIn::MediaInfoChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
#ifndef WIN32
    MythTvMediaStream *pMythTvStream;

    if ( pDeviceFrom->m_dwPK_DeviceTemplate == DEVICETEMPLATE_MythTV_Backend_Proxy_CONST )
    {
		int playbackDevice = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
		int tunedChannel = atoi(pMessage->m_mapParameters[EVENTPARAMETER_MythTV_ChannelID_CONST].c_str());
		if ( playbackDevice == 0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_PlugIn::MediaInfoChanged() called for an event which didn't provided a proper device ID");
			return false;
		}

		if ( (pMythTvStream = ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[playbackDevice],pMessage->m_dwPK_Device_From), "MythTV_PlugIn::MediaInfoChanged() ")) == NULL)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not detect a valid MythTV media stream based on the device %d", pDeviceFrom->m_dwPK_Device);
			return false;
		}
/*
		if ( m_pMythWrapper->GetCurrentChannelProgram(tunedChannel,
															pMythTvStream->m_sMediaDescription,
															pMythTvStream->m_sSectionDescription,
															pMythTvStream->m_sMediaSynopsis) == false )
		{
			pMythTvStream->m_sMediaDescription = "Channel: " + StringUtils::itos(tunedChannel);
			pMythTvStream->m_sSectionDescription = "Show info not available";
			pMythTvStream->m_sMediaSynopsis = "Other info not available";
		}
		*/

		pMythTvStream->m_iNextProgramChannelID = pMythTvStream->m_iCurrentProgramChannelID = tunedChannel;
		m_pMedia_Plugin->MediaInfoChanged(pMythTvStream);
    }
#endif
    return false;
}
/*

COMMANDS TO IMPLEMENT

*/

//<-dceag-sample-b->!
//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MythTV_PlugIn::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Jump to position called %s", sValue_To_Assign.c_str());

     MythTvMediaStream *pMediaStream =
         (MythTvMediaStream *) m_pMedia_Plugin->DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From);

	 if( !pMediaStream )
         return;  

	int m_dwTargetDevice = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;

	DCE::CMD_Jump_Position_In_Playlist tuneCommand(m_dwPK_Device, m_dwTargetDevice, sValue_To_Assign,pMediaStream->m_iStreamID_get());
	SendCommand(tuneCommand);
}

//<-dceag-c185-b->

	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #10 ID */
			/** The timer id assigned to the new schedule. */
		/** @param #14 Type */
			/** The type of recording: O=Once, C=Channel */
		/** @param #39 Options */
			/** Options for this recording, tbd later */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

void MythTV_PlugIn::CMD_Schedule_Recording(string sType,string sOptions,string sProgramID,string *sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c185-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	string::size_type pos=0;
	string sChanId = StringUtils::Tokenize(sProgramID,",",pos);
	time_t tStart = atoi(StringUtils::Tokenize(sProgramID,",",pos).c_str());
	time_t tStop = atoi(StringUtils::Tokenize(sProgramID,",",pos).c_str());
	string sStart = StringUtils::SQLDateTime(tStart);
	string sStop = StringUtils::SQLDateTime(tStop);
	string::size_type pos_start = sStart.find(' ');
	string::size_type pos_stop = sStop.find(' ');
	if( pos_start==string::npos || pos_stop==string::npos )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Schedule_Recording sProgramID %s is malformed", sProgramID.c_str());
		return;
	}

	// This bit of code is a bit odd. It has to do with legacy values for 
	// setting a scheduling option. Initially, two values, "O" and "C" were 
	// used for "Record Once" and "Record all on channel." I have to deal
	// with this, as well as with the new values which map directly to 
	// the constants in MythTV. -tschak
	if ( sType=="O" ) 
	{
		sType="1";
	} 
	else if (sType=="C")
	{
		sType="3";
	}
	else if (atoi(sType.c_str()) == 0) // Handle 0 or any random letters.
	{
		sType="0";	// Set to Do not record.
	}
	else 
	{
		// sType is a number, and is already set at this point.
	}

	string sSQL = "INSERT INTO record(type,chanid,startdate,starttime,enddate,endtime,title,subtitle,description,category,"
		"station,seriesid,programid,autocommflag,autoexpire,autouserjob1) "
		"SELECT " + sType + ",program.chanid,"
		"DATE(CONVERT_TZ('" + sStart + "','" + m_sTzOffset + "','+00:00')),TIME(CONVERT_TZ('" + sStart + "','" + m_sTzOffset + "','+00:00'))," + // Start Time
		"DATE(CONVERT_TZ('" + sStop + "','" + m_sTzOffset + "','+00:00')),TIME(CONVERT_TZ('" + sStop + "','" + m_sTzOffset + "','+00:00'))," + // Stop Time
		"title,subtitle,description,category,callsign,seriesid,programid,0,1,1 "
		"FROM program join channel on program.chanid=channel.chanid "
		"WHERE program.chanid=" + sChanId + " AND starttime=CONVERT_TZ('" + sStart + "','" + m_sTzOffset + "','+00:00')";

	int iID = m_pDBHelper_Myth->threaded_db_wrapper_query_withID(sSQL);

	// Add it to our local cache so the user sees the change instantly
	MythRecording mythRecording;
	mythRecording.channel_id = atoi(sChanId.c_str());
	mythRecording.scheduled_start_time = tStart;
	if( sType=="1" )
	{
		m_mapScheduledRecordings[mythRecording.key()] =
			make_pair<char,int> ('O',iID);
	}
	else if( sType=="3")
	{
		m_mapScheduledRecordings[mythRecording.key()] =
			make_pair<char,int> ('C',iID);
	}
	else
	{
		m_mapScheduledRecordings[mythRecording.key()] = 
			make_pair<char,int>(sType[0],iID);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::CMD_Schedule_Recording key " UINT64_PRINTF " %d=%s m_mapScheduledRecordings size %d", 
		(u_int64_t) mythRecording.key(), iID, sSQL.c_str(), (int) m_mapScheduledRecordings.size());
	if( m_pMythBackEnd_Socket )
		m_pMythBackEnd_Socket->SendMythString("RESCHEDULE_RECORDINGS " + StringUtils::itos(iID));
}

//<-dceag-createinst-b->!

class DataGridTable *MythTV_PlugIn::TvProviders(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
/*
	DataGridCell *pCell;
	if( !m_pDBHelper_Myth || !m_pDBHelper_Myth->m_bConnected )
		return pDataGrid;

	string::size_type pos=0;
	string sPK_PostalCode = StringUtils::Tokenize(Parms,",",pos);
	bool bInternalTuner = StringUtils::Tokenize(Parms,",",pos)=="1";
	string sSQL = "SELECT PK_Provider,Provider.Description,ProviderType.Description FROM PostalCode_Provider"
		" JOIN Provider ON FK_Provider=PK_Provider"
		" JOIN ProviderType ON FK_ProviderType=PK_ProviderType"
		" WHERE EK_PostalCode=" + sPK_PostalCode +
		" AND FK_ProviderType IN " +
		(bInternalTuner ? "(1,4)" : "(1,2,3,4)");

	PlutoSqlResult result_set;
    DB_ROW row;
	pCell = new DataGridCell("None","0");
	pDataGrid->SetData(0,0,pCell);
	int iRow=1;
	if( (result_set.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			pCell = new DataGridCell(string(row[2]) + ": " + row[1], row[0]);
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}*/

	return pDataGrid;
}
string MythTV_PlugIn::GetRecordingStatusFor(string sProgramID)
{
/*
	// sProgramID = chanid,startdatetime,enddatetime
        string::size_type pos=0;
        string sChanId = StringUtils::Tokenize(sProgramID,",",pos);
        time_t tStart = atoi(StringUtils::Tokenize(sProgramID,",",pos).c_str());
        time_t tStop = atoi(StringUtils::Tokenize(sProgramID,",",pos).c_str());
        string sStart = StringUtils::SQLDateTime(tStart);
        string sStop = StringUtils::SQLDateTime(tStop);
        string::size_type pos_start = sStart.find(' ');
        string::size_type pos_stop = sStop.find(' ');
        if( pos_start==string::npos || pos_stop==string::npos )
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::GetRecordingStatusFor sProgramID %s is malformed", sProgramID.c_str());
                return "There was an Error Getting Recording Status";
        }
	


	DB_ROW row;
	PlutoSqlResult result_set;

	string sSQL = "SELECT type from record WHERE chanid = '"+sChanID+"' " +
			"AND startdate = '"+sStartDate+"' AND starttime = '"+sStartTime+"' " +
			"AND enddate = '"+sEndDate+"' AND endtime = '"+sEndTime+"' ";

	if ( (result_set.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			
		}
	}

*/
	return "None";
}

//<-dceag-c698-b->

	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

void MythTV_PlugIn::CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c698-e->
{
	// This is intended to be called from Orbiter devices.
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	int dwPK_Orbiter = pMessage->m_dwPK_Device_From;

	// Parse the parameters: 
	string::size_type tokenPos = 0;
	string sChanID = StringUtils::Tokenize(sProgramID,",",tokenPos);
	string sStartTime = StringUtils::Tokenize(sProgramID,",",tokenPos);
	string sEndTime = StringUtils::Tokenize(sProgramID,",",tokenPos);

	PlutoSqlResult result;
	DB_ROW row;

	string sSQL = "SELECT * from program WHERE chanid = '"+sChanID+"' AND starttime = CONVERT_TZ('"+sStartTime+"','"+m_sTzOffset+"','+00:00') AND endtime = CONVERT_TZ('"+sEndTime+"','"+m_sTzOffset+"','+00:00')";

	if ( (result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL ) 
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			string sTitle = row[3];
			string sSubTitle = row[4];
			string sDescription = row[5];
			string sProgramFullName;
			string sProgramFullTime = sStartTime + "\n" + sEndTime;

			// Construct the title string with subtitle in Quotes, if present.
			if (!sSubTitle.empty())
			{
				sProgramFullName = sTitle + "\n \"" + sSubTitle  + "\"";
			}
			else
			{
				sProgramFullName = sTitle;
			}

			// FIXME: grab test graphic
			size_t length;
			char *data = FileUtils::ReadURL("http://www.thetvdb.com/banners/posters/79334-3.jpg",
							length, true);

			// Create messages to set orbiter variables...

			DCE::CMD_Set_Variable CMD_Set_Variable1(m_dwPK_Device, dwPK_Orbiter, 
							VARIABLE_Misc_Data_1_CONST,
							sProgramFullName);

			DCE::CMD_Set_Variable CMD_Set_Variable2(m_dwPK_Device, dwPK_Orbiter,
							VARIABLE_Misc_Data_2_CONST,
							sProgramFullTime);

			DCE::CMD_Set_Variable CMD_Set_Variable3(m_dwPK_Device, dwPK_Orbiter,
							VARIABLE_Misc_Data_3_CONST,
							sDescription);

			DCE::CMD_Set_Variable CMD_Set_Variable4(m_dwPK_Device, dwPK_Orbiter, 
							VARIABLE_Misc_Data_4_CONST,
							GetRecordingStatusFor(sProgramID));

			DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, dwPK_Orbiter, 
							StringUtils::itos(DESIGNOBJ_objCDCover_CONST),
							"1",data,(int) length,"0");

			DCE::CMD_Refresh CMD_Refresh(m_dwPK_Device, dwPK_Orbiter, "");

			// Shove the 2 and 3 messages into extramessages for 1, so that
			// only one message gets sent. More efficient.

			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable2.m_pMessage );
                	CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable3.m_pMessage );
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable4.m_pMessage );
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Refresh.m_pMessage );

			// And finally, send the message.
			SendCommand(CMD_Set_Variable1);
			SendCommand(CMD_Update_Object_Image);

		}	
	}

}

//<-dceag-c764-b->

	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

void MythTV_PlugIn::CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c764-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::CMD_Set_Active_Menu %s", sText.c_str());

    MythTvMediaStream *pMythTvMediaStream =
		ConvertToMythMediaStream(m_pMedia_Plugin->m_mapMediaStream_Find(m_mapDevicesToStreams[pMessage->m_dwPK_Device_From],pMessage->m_dwPK_Device_From));

    if( !pMythTvMediaStream )
	{
	    LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_PlugIn::CMD_Set_Active_Menu stream is NULL");
        return;  /** Can't do anything */
	}

	int PK_Screen_Remote,PK_Screen_OSD;
	if( sText=="live" )
	{
		PK_Screen_Remote=SCREEN_PVR_Remote_CONST;
		PK_Screen_OSD=SCREEN_PVR_Full_Screen_CONST;
	}
	else if( sText=="nonlive" )
	{
		PK_Screen_Remote=SCREEN_PVR_Recordings_CONST;
		PK_Screen_OSD=SCREEN_PVR_Recordings_Full_Screen_CONST;
	}
	else
	{
		PK_Screen_Remote=SCREEN_PVR_OSD_CONST;
		PK_Screen_OSD=SCREEN_PVR_OSD_Full_Screen_CONST;
	}

	/** We're going to send a message to all the orbiters that are bound to remotes in any of the entertainment areas */
	for( MapEntertainArea::iterator itEA = pMythTvMediaStream->m_mapEntertainArea.begin( );itEA != pMythTvMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
		for( MapBoundRemote::iterator itBR=pEntertainArea->m_mapBoundRemote.begin( );itBR!=pEntertainArea->m_mapBoundRemote.end( );++itBR )
		{
			BoundRemote *pBoundRemote = ( *itBR ).second;
			if (pBoundRemote)
			{
				RemoteControlSet *pRemoteControlSet =
					pMythTvMediaStream->m_mapRemoteControlSet[pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device];
				if (pRemoteControlSet)
				{
					pRemoteControlSet->m_iPK_Screen_Remote = PK_Screen_Remote;
					pRemoteControlSet->m_iPK_Screen_OSD = PK_Screen_OSD;

					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing bound remote: for orbiter: %d", pBoundRemote->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
					pMythTvMediaStream->SetNowPlaying( pBoundRemote->m_pOH_Orbiter,false,true );
				}
			}
		}
	}
}

//<-dceag-c824-b->

	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */
		/** @param #2 PK_Device */
			/** If specified, this is the capture card that triggered this change to call checktvproviders for */
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

void MythTV_PlugIn::CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c824-e->
{
	DeviceData_Router *pDevice_Core = m_pRouter->m_mapDeviceData_Router_Find( m_pRouter->iPK_Device_get() );
	if( pDevice_Core )
		pDevice_Core = pDevice_Core->GetTopMostDevice();

 	string sNoConfig = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,m_dwPK_Device,DEVICEDATA_Dont_Auto_Configure_CONST);
	if( atoi(sNoConfig.c_str()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- skipping because Don't Auto Configure is set");
		return;
	}

	string cmd = SCRIPT_SYNC_TO_DB;
	string args = "master " + ((pDevice_Core && !pDevice_Core->m_sIPAddress.empty()) ?
				   pDevice_Core->m_sIPAddress : m_pRouter->sDBHost_get());

	DeviceData_Base *pDevice_App_Server = (DeviceData_Router *)
	    m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
	if (pDevice_App_Server)
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(
			m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			cmd, "mythtv_sync_db_master", args, "", "", false, false, false, false);
		SendCommand(CMD_Spawn_Application);
	}

	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_MythTV_Player_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDevice_MythPlayer = *it;

			string sHostname = "dcerouter";
			int PK_Device_PC = DatabaseUtils::GetTopMostDevice(m_pMedia_Plugin->m_pDatabase_pluto_main,pDevice_MythPlayer->m_dwPK_Device);
			Row_Device *pRow_Device_PC = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device_PC);
			if( pRow_Device_PC )
			{
				Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_PC->FK_DeviceTemplate_getrow();
				if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Core_CONST )
					sHostname = "moon" + StringUtils::itos(pRow_Device_PC->PK_Device_get());

				if (pDevice_App_Server)
				{
					args = "slave " + pRow_Device_PC->IPaddress_get() + " " + sHostname;
					DCE::CMD_Spawn_Application CMD_Spawn_Application(
						m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
						cmd, "mythtv_sync_db_slave", args, "", "", false, false, false, false);
					SendCommand(CMD_Spawn_Application);
				}
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cannot find topmost device to setup %d", pDevice_MythPlayer->m_dwPK_Device);
				continue;
			}
		}
	}

	//bool bPathsChanged = SetPaths();
	bool bAddedProviders=false;

	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	m_bBookmarksNeedRefreshing=true;
	m_mapDevicesToSources.clear();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders");
    DB_ROW row,row2;

	string sSQL = "SELECT data FROM settings WHERE value='DBSchemaVer'";
	PlutoSqlResult result_set_check;
	if( (result_set_check.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))==NULL || (row=db_wrapper_fetch_row(result_set_check.r))==NULL || atoi(row[0])<MINIMUM_MYTH_SCHEMA )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards skipping now because I need at least schema %d insted of %s",
			MINIMUM_MYTH_SCHEMA, (NULL != result_set_check.r && row && row[0]) ? row[0] : "*none*");
		return;
	}

	// Find either inputs with a provider specified, or certain types of devices which should be added, but still need a provider
	sSQL = "SELECT PK_Device,IK_DeviceData FROM Device LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" TOSTRING(DEVICEDATA_EK_MediaProvider_CONST) " AND IK_DeviceData IS NOT NULL AND IK_DeviceData<>'' AND IK_DeviceData<>'NONE' "
		"WHERE IK_DeviceData IS NOT NULL OR (IK_DeviceData IS NULL AND FK_DeviceTemplate IN (" TOSTRING(DEVICETEMPLATE_Antenna_Port_CONST) "))";

	bool bModifiedRows=false; // Keep track of whether or not we changed anything
	PlutoSqlResult result_set;
	if( (result_set.r=m_pMedia_Plugin->m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders sql has %d rows", result_set.r->row_count);
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			Row_Device *pRow_Device = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow( atoi(row[0]) );
			Row_MediaProvider *pRow_MediaProvider = row[1] ? m_pMedia_Plugin->m_pDatabase_pluto_media->MediaProvider_get()->GetRow( atoi(row[1]) ) : NULL;

			if( !pRow_Device )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::SyncCardsAndProviders cannot find device %s provider %s",row[0],row[1]);
				continue;
			}

			pRow_Device->Reload();

			string sHostname = "dcerouter";
			int PK_Device_PC = DatabaseUtils::GetTopMostDevice(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
			Row_Device *pRow_Device_PC = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device_PC);
			if( pRow_Device_PC )
			{
				Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_PC->FK_DeviceTemplate_getrow();
				if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Core_CONST )
					sHostname = "moon" + StringUtils::itos(pRow_Device_PC->PK_Device_get());
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards topmost for %d is %d host %s",
					pRow_Device->PK_Device_get(),pRow_Device_PC->PK_Device_get(),sHostname.c_str());
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cannot find topmost device for %d", pRow_Device->PK_Device_get());

			Row_Device_DeviceData *pRow_Device_DeviceData_UseInMyth = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Dont_Consolidate_Media_CONST);
			bool bUseInMyth = !pRow_Device_DeviceData_UseInMyth || pRow_Device_DeviceData_UseInMyth->IK_DeviceData_get()!="1";
			Row_Device *pRow_Device_Source = pRow_Device;

			string::size_type pos=0;
			string sMediaProviderID = pRow_MediaProvider ? pRow_MediaProvider->ID_get() : "";
			string sUsername = StringUtils::Tokenize(sMediaProviderID,"\t",pos);
			string sPassword = StringUtils::Tokenize(sMediaProviderID,"\t",pos);
			int PK_DeviceTemplate_MediaType = atoi(StringUtils::Tokenize(sMediaProviderID,"\t",pos).c_str());
			int PK_ProviderSource = atoi(StringUtils::Tokenize(sMediaProviderID,"\t",pos).c_str());
                        (void) PK_DeviceTemplate_MediaType;
                        (void) PK_ProviderSource;
			string sLineup = StringUtils::Tokenize(sMediaProviderID,"\t",pos);

			// We only care about capture cards
			Row_Device *pRow_Device_External = NULL;
			Row_Device_DeviceData *pRow_Device_DeviceData = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_FK_Device_Capture_Card_Port_CONST);
			if( pRow_Device_DeviceData )
			{
				pRow_Device_External=pRow_Device;
				pRow_Device = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );
				if( !pRow_Device )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cannot find capture card for %d", pRow_Device_Source->PK_Device_get());
					continue;
				}
			}
			Row_Device *pRow_Device_CaptureCard = pRow_Device;
			if( !DatabaseUtils::DeviceIsWithinCategory(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICECATEGORY_Capture_Cards_CONST) )
			{
				pRow_Device_CaptureCard = pRow_Device_CaptureCard->FK_Device_ControlledVia_getrow();
				if( !pRow_Device_CaptureCard || !DatabaseUtils::DeviceIsWithinCategory(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICECATEGORY_Capture_Cards_CONST) )
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders not a capture card device %s provider %s",row[0],row[1]);
#endif
					continue;
				}
			}

			/* Don't work as expected, I think the data is not set properly in the screenhandler
			   (see changeset 22677)

			// Skip card if the Use Automatically devdata isn't set
			sNoConfig = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Use_Automatically_CONST);
			if( !atoi(sNoConfig.c_str()) )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- skipping because Use Automatically DeviceData is not set");
				continue;
			}
		 	*/
			bool bTunersAsSeparateDevices = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Children_as_Separate_Tuners_CONST)=="1";

			// We need to add configure scripts for each model of card, or the sql statement to insert for each card. 

			// We have a capture card.  See if it's in the database already.  We use DEVICEDATA_Port_CONST for the port
			int cardid = atoi(DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pRow_Device->PK_Device_get() : pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST).c_str());
			if( cardid && (pRow_Device->Disabled_get()==1 || pRow_Device_CaptureCard->Disabled_get()==1) )
			{
				// Don't delete the card, because then the whole source and everything are deleted when the card is removed.
				// Just delete the inputs so Myth won't use them, because we can add back inputs easily when the card is enabled again
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards deleting disabled cardid %d from device %d",cardid,pRow_Device->PK_Device_get());
				sSQL = "UPDATE `capturecard` SET hostname=NULL WHERE cardid=" + StringUtils::itos(cardid);
				m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
				continue;
			}
			else if( pRow_Device->Disabled_get()==1 || pRow_Device_CaptureCard->Disabled_get()==1 )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders skipping disabled device %d", pRow_Device->PK_Device_get());
				continue;
			}

			PlutoSqlResult result_confirm_cardid_is_valid;
			sSQL = "SELECT cardid FROM capturecard WHERE cardid=" + StringUtils::itos(cardid);
			if( ( result_confirm_cardid_is_valid.r=m_pDBHelper_Myth->db_wrapper_query_result( sSQL ) )==NULL 
				|| result_confirm_cardid_is_valid.r->row_count<1 )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards cardid %d is invalid",cardid);
				cardid=0;
				DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pRow_Device->PK_Device_get() : pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST,"");
			}


			string sPortName = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Name_CONST);
			string sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
			if( sBlockDevice.empty() )
				sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Block_Device_CONST);

			string sScanningScript = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Scanning_Script_CONST);
			if( sScanningScript.empty() )
				sScanningScript = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Scanning_Script_CONST);

			bool bNewCard=false;
				
			string sPortNumber = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_PortChannel_Number_CONST);
			if( sPortName.empty() )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::SyncCardsAndProviders no port name for device %s provider %s",row[0],row[1]);
				continue;
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders device %d card id %d block %s port %s", 
				pRow_Device->PK_Device_get(),cardid,sBlockDevice.c_str(),sPortName.c_str());

			if( !cardid )
			{
				bNewCard=true;
				bModifiedRows=true;
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true !cardid %d",pRow_Device->PK_Device_get());

				sSQL = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Configuration_CONST);
				if( sSQL.empty() )
					sSQL = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device->FK_Device_ControlledVia_get(),DEVICEDATA_Configuration_CONST);

				if( sSQL.empty() )
				{
					sSQL = "INSERT INTO `capturecard`(cardtype,hostname,defaultinput) VALUES ('MPEG','" + sHostname + "','" + sPortName + "');";
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders adding card with default query: %s", sSQL.c_str());
				}
				else
				{
					StringUtils::Replace(&sSQL,"<%=HOST%>",sHostname);
					StringUtils::Replace(&sSQL,"<%=PORT%>",sPortName);
					StringUtils::Replace(&sSQL,"<%=PORTNUM%>",sPortNumber);
					string sIPAddress = pRow_Device_CaptureCard->IPaddress_get();
					StringUtils::Replace(&sSQL,"<%=IP%>",sIPAddress);
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders adding card with custom query: %s", sSQL.c_str());
				}

				cardid = m_pDBHelper_Myth->threaded_db_wrapper_query_withID(sSQL);
				DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pRow_Device->PK_Device_get() : pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST,StringUtils::itos(cardid));
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders added card %d", cardid);
			}

			sSQL = "UPDATE `capturecard` SET hostname='" + sHostname + "' WHERE cardid=" + StringUtils::itos(cardid);
			m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);

			if( sBlockDevice.empty()==false )
			{
				sSQL = "UPDATE `capturecard` SET videodevice='" + sBlockDevice + "' WHERE cardid=" + StringUtils::itos(cardid);
				if( m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL)>0 )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true %s",sSQL.c_str());
					bModifiedRows=true;
				}
			}
			else
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- no block device for %d", cardid);

			if( (bNewCard || pRow_Device_CaptureCard->NeedConfigure_get()==1 || pRow_Device->NeedConfigure_get()==1) && sScanningScript.empty()==false )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders device %d scan script %s",
					pRow_Device->PK_Device_get(),sScanningScript.c_str());
				StartScanningScript(pRow_Device,pRow_Device_CaptureCard,iPK_Orbiter,sScanningScript);
				continue;
			}

			// When we're first doing a channel scan, we'll add a source, but not a card input, and give the source the name UNKNOWN_[cardid]
			// See if there is such a source, and if it has any channels
			string s = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_External ? pRow_Device_External->PK_Device_get() : pRow_Device->PK_Device_get(),DEVICEDATA_Source_CONST);
			int sourceid=atoi(s.c_str());
			if( sourceid )
			{
				sSQL = "SELECT sourceid FROM videosource WHERE sourceid=" + StringUtils::itos(sourceid);
				PlutoSqlResult result_set_has_source;
				if( (result_set_has_source.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))==NULL || 
					result_set_has_source.r->row_count==0 )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards card id %d has invalid source %d", cardid, sourceid);
					sourceid=0;
				}
			}

			bool bHasChannels = false, bRenameUnknown=false;
			PlutoSqlResult result_set_us;
			if( sourceid==0 )
			{
				sSQL = "SELECT sourceid FROM videosource WHERE name='UNKNOWN_" + StringUtils::itos(cardid) + "'";
				if( (result_set_us.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && (row2=db_wrapper_fetch_row(result_set_us.r)) && atoi(row2[0]) )
				{
					sourceid = atoi(row2[0]);
					bRenameUnknown=true;
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards card id %d has unknown source %d", cardid, sourceid);
				}
			}

			// This it the query to see if we have channels
			if( sourceid )
			{
				sSQL = "SELECT sourceid FROM channel WHERE sourceid=" + StringUtils::itos(sourceid) + " LIMIT 1";
				PlutoSqlResult result_set_has_channels;
				if( (result_set_has_channels.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && result_set_has_channels.r->row_count>0 )
					bHasChannels = true;
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders device %d/%d source %d prov %p channels %d",
				pRow_Device->PK_Device_get(),pRow_Device_External ? pRow_Device_External->PK_Device_get() : 0,sourceid,pRow_MediaProvider,(int) bHasChannels);

			if( pRow_MediaProvider!=NULL || bHasChannels )  // If we have channels, we must already have a source id
			{
				Row_ProviderSource *pRow_ProviderSource = pRow_MediaProvider ? pRow_MediaProvider->FK_ProviderSource_getrow() : NULL;
				string sGrabber;
				if( pRow_ProviderSource )
					sGrabber = pRow_ProviderSource->FillCommandLine_get();
				string sProviderName = pRow_MediaProvider ? "Provider " + StringUtils::itos(pRow_MediaProvider->PK_MediaProvider_get()) : "";

				if( !sourceid && pRow_MediaProvider )  // No source, but we do have a provider
				{
					// See if the provider is in the database already
					sSQL = "SELECT sourceid FROM `videosource` WHERE name='" + sProviderName + "'";
					PlutoSqlResult result_set2;
					if( (result_set2.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && (row2=db_wrapper_fetch_row(result_set2.r)) && atoi(row2[0]) )
						sourceid = atoi(row2[0]);
					else
					{
						PlutoSqlResult result_set2;
						if( (result_set2.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && (row2=db_wrapper_fetch_row(result_set2.r)) && atoi(row2[0]) )
							sourceid = atoi(row2[0]);
						else
						{
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true %s",sSQL.c_str());
							bModifiedRows=true;
							// The data direct shouldn't be hardcoded
							sSQL = "INSERT INTO `videosource`(name) VALUES ('" + sProviderName + "')";
							sourceid = m_pDBHelper_Myth->threaded_db_wrapper_query_withID(sSQL);
							bAddedProviders=true;
						}
					}
				}

				if( sourceid && sProviderName.empty()==false )
				{
					int iAppendValue=0;

					if( bRenameUnknown )
					{
						// The same provider can be in here more than once, but name must be unique, so append a number to it if it is
						// This can happen for a device that does it's own channel scan, adds it's source and channels as UNKNOWN_
						// and then later the user assigns it to a provider that already exists.  Since channels are stored by source, not card,
						// we want to leave the provider in here more than once
						while(true)  
						{
							string s = sProviderName + (iAppendValue ? "_" + StringUtils::itos(iAppendValue) : "");
							sSQL = "SELECT name FROM `videosource` WHERE name='" + s + "' AND sourceid<>" + StringUtils::itos(sourceid);
							PlutoSqlResult result_set2;
							if( (result_set2.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && result_set2.r->row_count>0 )
							{
								iAppendValue++;  // This name is already used.  Add a suffix to it and try again
								continue;
							}
							else
								break;
						}
					}

					string sRealProvider = sProviderName + (iAppendValue ? "_" + StringUtils::itos(iAppendValue) : "");
					// Only set the name if we're renaming an unknown id
					sSQL = "UPDATE `videosource` SET " + (bRenameUnknown ? "name='" + sRealProvider + "'," : "") + "xmltvgrabber='" + sGrabber + "',userid='" + sUsername + "', password='" + sPassword + "', lineupid='" + sLineup + "' WHERE sourceid=" + StringUtils::itos(sourceid);
					if( m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL)>0 )
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true %s",sSQL.c_str());
						bModifiedRows=true;
						bAddedProviders=true;
					}
				}

				m_mapDevicesToSources[pRow_Device_Source->PK_Device_get()].push_back(sourceid);
				if( bUseInMyth )
					m_mapDevicesToSources[0].push_back(sourceid);

				int cardinputid=0;
				sSQL = "SELECT cardinputid FROM `cardinput` WHERE cardid='" + StringUtils::itos(cardid) + "' AND inputname='" + sPortName + "'";
				PlutoSqlResult result_set3;
				if( (result_set3.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && (row2=db_wrapper_fetch_row(result_set3.r)) && atoi(row2[0]) )
					cardinputid = atoi(row2[0]);

				if( cardinputid && !bUseInMyth )
				{
					m_pDBHelper_Myth->threaded_db_wrapper_query("DELETE FROM `cardinput` WHERE cardinputid=" + StringUtils::itos(cardinputid));
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards deleting cardinput %d", cardinputid);
				}
				else if( !cardinputid && bUseInMyth )
				{
					bModifiedRows=true;
					sSQL = "INSERT INTO `cardinput`(cardid,sourceid,inputname) VALUES (" + StringUtils::itos(cardid) + "," + StringUtils::itos(sourceid) + ",'" + sPortName + "')";
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true cardinputid %d bUseInMyth %d sql: %s",(int) cardinputid, (int) bUseInMyth, sSQL.c_str());
					cardinputid = m_pDBHelper_Myth->threaded_db_wrapper_query_withID(sSQL);
				}
				else if( cardinputid )
				{
					sSQL = "UPDATE cardinput SET sourceid='" + StringUtils::itos(sourceid) + "' WHERE cardid='" + StringUtils::itos(cardid) + "' AND inputname='" + sPortName + "'";
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards updating cardinput %s", sSQL.c_str());
					m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
				}

				if( pRow_Device_External )
				{
					sSQL = "UPDATE cardinput SET externalcommand='" + SCRIPT_TUNE + " " +
						StringUtils::itos(pRow_Device_External->PK_Device_get()) + " " +
						StringUtils::itos(sourceid) + "' WHERE cardinputid=" + StringUtils::itos(cardinputid);
					if( m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL)>0 )
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true %s",sSQL.c_str());
						bModifiedRows=true;
					}
				}
			}

			DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pRow_Device_External ? pRow_Device_External->PK_Device_get() : pRow_Device->PK_Device_get(),DEVICEDATA_Source_CONST,StringUtils::itos(sourceid));
		}
	}

	// MythTV crashes sometimes when you have an entry in capturecard with no corresponding entries in cardinput.
	// We will create a "dummy" videosource to use as a default when no videosource has been assigned in cardinput.
	sSQL = "SELECT sourceid FROM videosource WHERE name = 'LMCE-Default'";
	PlutoSqlResult result_videosource;
	if( (result_videosource.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		row=db_wrapper_fetch_row( result_videosource.r );
		if(result_videosource.r->row_count==0) {
			//LMCE-Default does not exist, create it!
			sSQL = "INSERT INTO videosource (name,xmltvgrabber,freqtable) VALUES ('LMCE-Default','/bin/true','us-bcast')";
			m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		}
	}

	// Myth crashes when you have an entry in capture card with no corresponding entires in cardinput for some types of capture cards
	// So if there are no inputs, we will point to the LMCE-Default videosource. To do this, we need to manage INSERTING, UPDATING and DELETING the appropriate entries.
	string sDefaultSourceID;
	
	sSQL = "SELECT sourceid FROM videosource WHERE name = 'LMCE-Default'";
	PlutoSqlResult result_defaultVideoSource;
	if( (result_defaultVideoSource.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		row=db_wrapper_fetch_row( result_defaultVideoSource.r );
		sDefaultSourceID = row[0];
	}
	
	sSQL="SELECT cardid FROM capturecard";
	PlutoSqlResult result_captureCards;
	if( (result_captureCards.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		string sThisCard;
		while( ( row=db_wrapper_fetch_row( result_captureCards.r ) ) )
		{
			sThisCard=row[0];
			sSQL="SELECT sourceid FROM cardinput WHERE cardid="+sThisCard;
			PlutoSqlResult result_sourceid;
			if( (result_sourceid.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
			{
				if (result_sourceid.r->row_count==0) {
					// There is NO ENTRY, create one to LMCE-Default!
					sSQL="INSERT INTO cardinput (cardid,sourceid,inputname) VALUES ('"+sThisCard+"','"+sDefaultSourceID+"','MPEG2')";
					m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
				} else if (result_sourceid.r->row_count > 1) {
					// More than one videosources exist, so we can safely remove the LMCE-Default videosource.

					// There is more than one sourceid associated with this capturecard.
					// If one of them is LMCE-Default, we can safely remove it now!
					sSQL="DELETE FROM cardinput WHERE sourceid='"+sDefaultSourceID+"'";
					m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);						
				} else {
					// There is nothing to do. There is exactly 1 sourceid. This means that either
					// the sourceid is real, or the sourceid the LMCE-Default one.
				}				
			}
		}
	}

	// Use LMCE-Default for any videosource that does not exist
	sSQL = "UPDATE cardinput LEFT JOIN videosource ON cardinput.sourceid=videosource.sourceid SET cardinput.sourceid='"+sDefaultSourceID+"' WHERE videosource.sourceid IS NULL";
	m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);	
	
	// If for some reason there is a problem and cardinput table is incorrect, delete the hostname to keep some cards from crashing MythTV out (this should never happen, as we make sure its populated above)
	sSQL = "UPDATE capturecard LEFT JOIN cardinput ON capturecard.cardid=cardinput.cardid SET hostname=NULL WHERE cardinput.cardid IS NULL";
	m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		
	// Delete any stray rows for cards that no longer exist
	sSQL = "DELETE cardinput FROM cardinput LEFT JOIN capturecard ON capturecard.cardid=cardinput.cardid WHERE capturecard.cardid IS NULL";
	if( m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL)>0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true %s",sSQL.c_str());
		bModifiedRows=true;
	}

	// We may have videosources with no inputs if the user unplugged a pvr capture device temporarily and is going to reconnect it.
	// So don't delete the source

	// Since we're sometimes removing and re-adding devices as the user unplugs/plugs them in, be sure we cleanup the database
	// and don't leave any stray channel or programs around
	sSQL = "DELETE channel FROM channel left JOIN videosource ON channel.sourceid=videosource.sourceid WHERE videosource.sourceid IS NULL";
	m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
	sSQL = "DELETE program FROM program left JOIN channel ON program.chanid=channel.chanid WHERE channel.chanid IS NULL";
	m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);

	if( iPK_Device )
		CheckForTvFormatAndProvider( iPK_Device ); 		// See if we've found a tv format, or can otherwise determine the provider

	if( m_mapPendingScans.empty()==false )
		return;

	// Find cards with invalid starting channels
	sSQL = "SELECT cardinputid,cardinput.sourceid FROM cardinput LEFT JOIN channel ON startchan=channum AND cardinput.sourceid=channel.sourceid WHERE channum IS NULL";
	PlutoSqlResult result_set2;
	if( (result_set2.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		while( ( row=db_wrapper_fetch_row( result_set2.r ) ) )
		{
			if( row[0] && row[1] )
			{
				string cardinputid=row[0];
				string sourceid=row[1];
				string sChanNum="1";

				sSQL = "SELECT min(channum) FROM channel WHERE sourceid=" + sourceid;
				PlutoSqlResult result_set3;
				if( (result_set3.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set3.r)) && row[0] )
					sChanNum = row[0];

				sSQL = "UPDATE cardinput SET startchan='" + sChanNum + "' WHERE cardinputid=" + cardinputid;
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards %s",sSQL.c_str());
				if( m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL)>0 )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Sync_Providers_and_Cards bModifiedRows=true %s",sSQL.c_str());
					bModifiedRows=true;
				}
			}
		}
	}

	// Be sure we have some valid capture cards and sources before we do a fill
	sSQL = "SELECT videosource.sourceid FROM videosource JOIN cardinput ON videosource.sourceid=cardinput.sourceid JOIN capturecard ON capturecard.cardid=cardinput.cardid WHERE hostname IS NOT NULL limit 1";
	PlutoSqlResult result_set_sources;
	bool bContainsVideoSources = (result_set_sources.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL && result_set_sources.r->row_count>0;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- bModifiedRows %d m_bFillDbRunning %d m_bNeedToRunFillDb %d bContainsVideoSources %d",
		(int) bModifiedRows,(int) m_bFillDbRunning,(int) m_bNeedToRunFillDb, (int) bContainsVideoSources);
	if( (bModifiedRows && bContainsVideoSources) )  // We've changed stuff.  Need to run the fill process
	{
		if( m_bFillDbRunning ) {
			m_bNeedToRunFillDb = true;  // A fill is already running.  Need to re-run it when we're done
		} else {
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SyncCardsAndProviders -- setting m_bFillDbRunning=true and starting fill");
			m_bFillDbRunning=true;
			StartFillDatabase(bAddedProviders);
		}
	} else {
		system(SCRIPT_MYTHTV_SETUP.c_str());
	}
	/*else if( bPathsChanged )
        { // Paths were changed, so we don't need to call mythfilldatabase, but we do need to restart the backend[s].
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			SCRIPT_RESTART_ALL_BACKENDS, "restart_all_backends_in_sync", "",
			"", "", false, false, false, false);
		SendCommand(CMD_Spawn_Application);
        }*/
}

bool MythTV_PlugIn::UpdateMythSetting(string value,string data,string hostname,bool bOnlyIfNotExisting)
{
	if( hostname=="*" )
	{
		bool bChanged = false;
		string sSQL = "SELECT hostname FROM settings WHERE hostname IS NOT NULL GROUP BY hostname";
		PlutoSqlResult result;
		DB_ROW row;
		if( (result.r = m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
		{
			while( ( row=db_wrapper_fetch_row( result.r ) ) )
			{
				if( row[0] )
				{
					bChanged |= UpdateMythSetting(value,data,row[0],bOnlyIfNotExisting);
				}
			}
		}
		return bChanged;
	}

	string sSQL = "SELECT data FROM settings WHERE value='" + StringUtils::SQLEscape(value) + "' AND " 
		" hostname " + (hostname.empty() ? "IS NULL" : "='" + StringUtils::SQLEscape(hostname) + "'");
	PlutoSqlResult result;
	if( (result.r = m_pDBHelper_Myth->db_wrapper_query_result(sSQL))==NULL || result.r->row_count==0 )
	{
		sSQL = "INSERT INTO settings(value,hostname,data) VALUES('" + StringUtils::SQLEscape(value) + "',"
			+ (hostname.empty() ? "NULL" : "'" + StringUtils::SQLEscape(hostname) + "',")
			+ "'" + StringUtils::SQLEscape(data) + "')";
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		return true;
	}
	else if( bOnlyIfNotExisting )
		return false;

	DB_ROW row;
	row = db_wrapper_fetch_row( result.r ) ;
	if( row[0] && (data==row[0]) )
		return false;

	sSQL = "UPDATE settings SET data='" + StringUtils::SQLEscape(data) + "' WHERE value='" + StringUtils::SQLEscape(value) + "' "
		" AND hostname " + (hostname.empty() ? "IS NULL" : "='" + StringUtils::SQLEscape(hostname) + "'");

	m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
	return true;
}

void MythTV_PlugIn::CheckForNewRecordings()
{
	/* I haven't been able to find a clean way to get myth back end to notify us
	when it has finished making new recordings.  So we'll do it the brute force way
	and search for new files that have 'pvr' in the path but don't yet have any
	attributes */

	string sSQL = "SELECT max(PK_File) FROM File";
	PlutoSqlResult result;
    	DB_ROW row;
    
	int PK_File_Max=0;
	if( ( result.r=m_pMedia_Plugin->m_pDatabase_pluto_media->db_wrapper_query_result( sSQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) && row[0] ) {
		PK_File_Max = atoi(row[0]);
	}

	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex); // protect m_sNewRecordings
	sSQL = "LEFT JOIN File_Attribute ON FK_File=PK_File WHERE (Missing=0 AND Path like '%pvr%' AND FK_File IS NULL AND PK_File>" + StringUtils::itos(m_dwPK_File_LastCheckedForNewRecordings) + ")";
	if( m_sNewRecordings.empty()==false )
	{
		sSQL += " OR Filename in (" + m_sNewRecordings + ")";
		m_sNewRecordings=""; // Reset it
	}
	vector<Row_File *> vectRow_File;
	m_pMedia_Plugin->m_pDatabase_pluto_media->File_get()->GetRows( sSQL, &vectRow_File );

	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"MythTV_PlugIn::CheckForNewRecordings PK_File_Max %d m_dwPK_File_LastCheckedForNewRecordings %d files: %d %s",
		PK_File_Max, m_dwPK_File_LastCheckedForNewRecordings, (int) vectRow_File.size(), sSQL.c_str());

	for(vector<Row_File *>::iterator it=vectRow_File.begin();it!=vectRow_File.end();++it)
	{
		Row_File *pRow_File = *it;
		// Find the file isn mythconverg.recorded and import the attributes

		sSQL = 
			"SELECT recorded.chanid, CONVERT_TZ(recorded.starttime,'+00:00','"+m_sTzOffset+"') as tz_starttime, recorded.title, recorded.subtitle, recorded.stars, recorded.category, recorded.description,"
			"recordedprogram.hdtv, recordedprogram.category_type, channel.name, recordedrating.rating, recgroup, recordedprogram.seriesid, recordedprogram.programid, channel.icon, CONCAT(storagegroup.dirname,'/',recorded.basename,'.png') AS screenshot FROM recorded "
			"LEFT JOIN recordedprogram ON recorded.chanid=recordedprogram.chanid and recorded.starttime=recordedprogram.starttime "
			"LEFT JOIN channel ON recorded.chanid=channel.chanid "
			"LEFT JOIN recordedrating ON recorded.chanid=recordedrating.chanid and recorded.starttime=recordedrating.starttime "
			"LEFT JOIN storagegroup ON recorded.storagegroup = storagegroup.groupname "
			"WHERE basename='" + pRow_File->Filename_get() + "'";

		PlutoSqlResult result;
		if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result( sSQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			string title = (row[2]) ? row[2] : "";
			// If it's just a live tv buffer that hasn't been purged yet, add "LiveTV: " to the title.
			if( row[11] && strcmp(row[11],"LiveTV")==0 )
			{
				pRow_File->Ignore_set(1);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"MythTV_PlugIn::CheckForNewRecordings file: %s is live tv title %s",pRow_File->Filename_get().c_str(), title.c_str());
	
				if( title!="" ) {
					title = "LiveTV: " + title;
				} else {
					continue;
				}
			}

			int PK_Attribute_CreationDate,PK_Attribute_Title=0,PK_Attribute_Episode=0,PK_Attribute_Channel=0,PK_Attribute_Misc;

			LoggerWrapper::GetInstance()->Write(
				LV_DEBUG,
				"RESULT ROW: file %s chanid %s start_time %s title %s subtitle %s stars %s category %s " // 6
				"description %s hdtv %s category_type %s chan_name %s rating %s recgroup %s " // 6
				"seriesid %s programid %s icon %s screenshot %s", // 3
				pRow_File->Filename_get().c_str(),
				row[0], row[1], row[2], row[3], row[4], row[5],
				row[6], row[7], row[8], row[9], row[10], row[11],
				row[12], row[13], row[14], row[15]);

			if( row[1] && row[1][0] != 0 /*starttime*/)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching starttime attribute -- 35start (%s)", row[1]);
				m_pMedia_Plugin->CMD_Add_Media_Attribute(
					/*val*/row[1],/*streamid*/0,/*tracks*/"",
					/*attr type*/ATTRIBUTETYPE_Creation_Date_CONST,
					/*section*/"",
					/*file*/pRow_File->PK_File_get(),
					/*int attr key*/&PK_Attribute_CreationDate);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching starttime attribute -- end\n");
			}
			if( title != "" )
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching title attribute -- start (%s)", title.c_str());
				m_pMedia_Plugin->CMD_Add_Media_Attribute(title,0,"",ATTRIBUTETYPE_Title_CONST,"",
									 pRow_File->PK_File_get(),&PK_Attribute_Title);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching title attribute -- end\n");
			}
			if( row[3] && row[3][0] != 0 /*subtitle*/)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching subtitle attribute -- start (%s)", row[3]);
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[3],0,"",ATTRIBUTETYPE_Episode_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Episode);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching subtitle attribute -- end\n");
			}
			if( row[4] && row[4][0] != 0 /*stars*/)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching stars attribute -- start (%s)", row[4]);
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[4],0,"",ATTRIBUTETYPE_Rating_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching stars attribute -- end\n");
			}
			if( row[5] && row[5][0] != 0 /*category*/)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching category attribute -- start (%s)", row[5]);
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[5],0,"",ATTRIBUTETYPE_Genre_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching category attribute -- end\n");
			}
			if( row[6] && row[6][0] != 0 /*description*/)
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching description attribute -- start (%s)", row[6]);
				Row_LongAttribute *pRow_LongAttribute = m_pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->AddRow();
				pRow_LongAttribute->FK_File_set( pRow_File->PK_File_get() );
				pRow_LongAttribute->FK_AttributeType_set( ATTRIBUTETYPE_Synopsis_CONST );
				pRow_LongAttribute->Text_set( row[6] );
				m_pMedia_Plugin->m_pDatabase_pluto_media->LongAttribute_get()->Commit();
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching description attribute -- end\n");
			}
			
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching HDTV attribute -- start (%s)", row[7]);
			if( row[7] && row[7][0] != 0 && atoi(row[7]) /*hdtv*/) {
				pRow_File->FK_FileFormat_set(FILEFORMAT_HD_1080_CONST);
			} else {
				pRow_File->FK_FileFormat_set(FILEFORMAT_Standard_Def_CONST);
			}
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching description attribute -- end\n");
			
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching Category Type attribute -- start (%s)", row[8]);
			if( row[8] && row[8][0] != 0 /*category_type*/)
			{
				string sType = row[8];
				if( StringUtils::StartsWith(sType,"Sports",true) ) {
					pRow_File->FK_MediaSubType_set(MEDIASUBTYPE_Sports_Events_CONST);
				} else {
					pRow_File->FK_MediaSubType_set(MEDIASUBTYPE_TV_Shows_CONST);
				}
			} else {
				pRow_File->FK_MediaSubType_set(MEDIASUBTYPE_TV_Shows_CONST);
			}
			LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching Category Type attribute -- end\n");


			if( row[9] && row[9][0] != 0 /*channel name*/) {
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching Channel Name attribute -- start (%s)", row[9]);
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[9],0,"",ATTRIBUTETYPE_Channel_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Channel);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching Channel Name attribute -- end\n");
			}

			if( row[10] && row[10][0] != 0 /*rating*/) {
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching rating attribute -- start (%s)", row[10]);
				m_pMedia_Plugin->CMD_Add_Media_Attribute(row[10],0,"",ATTRIBUTETYPE_Rated_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Fetching rating attribute -- end\n");
			}

			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"MythTV_PlugIn::CheckForNewRecordings file: %s %d %d",pRow_File->Filename_get().c_str(),PK_Attribute_Title,PK_Attribute_Episode);

			int PK_Picture = 0;
			if( PK_Attribute_Title || PK_Attribute_Episode )
			{
				if( row[12] ) {
					sSQL = string("seriesid='") + row[12] + "'";
				} else {
					sSQL = "";
				}

				if( row[13] )
				{
					if( sSQL.empty()==false ) {
						sSQL += " OR ";
					}
					sSQL += string("programid='") + row[13] + "'";
				}
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Title/Episode %s",sSQL.c_str());

				PlutoSqlResult result;
			    	DB_ROW row_pic;
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Look For Picture...");				
				if( (sSQL!="") && (result.r=m_pDBHelper_Myth->db_wrapper_query_result( "SELECT EK_Picture FROM `pluto_myth`.Picture WHERE " + sSQL ) ) && ( row_pic=db_wrapper_fetch_row( result.r ) ) && row_pic[0] && atoi(row_pic[0]) )
				{
					LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Custom MythTV picture found - %s",sSQL.c_str());
					PK_Picture = atoi(row_pic[0]);
					if( PK_Attribute_Title ) {
						m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Title,atoi(row_pic[0]));
					}
					if( PK_Attribute_Episode ) {
						m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Episode,atoi(row_pic[0]));
					}				
				}
				else  // Look for a channel logo
				{
					PlutoSqlResult result;
					if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result( "SELECT EK_Picture FROM `pluto_myth`.Picture WHERE chanid=" + string(row[0]) ) ) && ( row_pic=db_wrapper_fetch_row( result.r ) ) && row_pic[0] && atoi(row_pic[0]) )
					{
						LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Channel Picture Found. - %s",sSQL.c_str());
						PK_Picture = atoi(row_pic[0]);
						if( PK_Attribute_Title ) {
							m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Title,atoi(row_pic[0]));
						}						
						if( PK_Attribute_Episode ) {
							m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Episode,atoi(row_pic[0]));
						}					
					}
					else if( row[14] )  // Use the icon from myth channel
					{
						Row_Picture *pRow_Picture = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(row[15]);
						if( !pRow_Picture ) {
							LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CheckForNewRecordings: Failed to add picture %s",row[15]);
						} else {
							LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Adding Picture %d / %s", pRow_Picture->PK_Picture_get(), row[15]);
							PK_Picture = pRow_Picture->PK_Picture_get();
							if( PK_Attribute_Title ) {
								m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Title,pRow_Picture->PK_Picture_get());
							}							
							if( PK_Attribute_Episode ) {
								m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToAttribute(PK_Attribute_Episode,pRow_Picture->PK_Picture_get());
							}						
						}
					}
				}
			}

			if( PK_Picture ) {
				m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPictureToFile(pRow_File->PK_File_get(),PK_Picture);
			}


			LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Looking for Personnel attributes...");
			/*sSQL = 
				"SELECT role,name FROM recordedcredits JOIN people on recordedcredits.person=people.person "
				"WHERE chanid='" + string(row[0]) + "' AND starttime='" + row[1] + "'";

			PlutoSqlResult result_person;
			if( (result_person.r=m_pDBHelper_Myth->db_wrapper_query_result( sSQL ) ) )
			{
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: Personell results fetched - lets add them in!");
				while( row=db_wrapper_fetch_row( result_person.r ) )
				{
					string sRole = row[0];
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Adding Petsonell attribute -- (%s)%s", sRole,row[1]);
					if( sRole=="director" ) {
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Director_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
					} else if( sRole=="actor" ) {
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Performer_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
					} else if( sRole=="producer" ) {
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Producer_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
					} else if( sRole=="executive_producer" ) {
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_Executive_Producer_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
					} else if( sRole=="writer" ) {
						m_pMedia_Plugin->CMD_Add_Media_Attribute(row[1],0,"",ATTRIBUTETYPE_ComposerWriter_CONST,"",pRow_File->PK_File_get(),&PK_Attribute_Misc);
					}
				}
			} else {
				LoggerWrapper::GetInstance()->Write(LV_DEBUG,"CheckForNewRecordings: No personell results!");
			}
			*/
		} else {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CheckForNewRecordings couldn't locate file %s", pRow_File->Filename_get().c_str());
		}
	}
	
	m_pMedia_Plugin->m_pDatabase_pluto_media->File_get()->Commit();
	m_pAlarmManager->CancelAlarmByType(CHECK_FOR_NEW_RECORDINGS);  // Don't get multiple entries in here since this is called by both a timer and new recordings
	m_pAlarmManager->AddRelativeAlarm(3600,this,CHECK_FOR_NEW_RECORDINGS,NULL);  /* check again in an hour */

	m_dwPK_File_LastCheckedForNewRecordings = PK_File_Max;
	LoggerWrapper::GetInstance()->Write(LV_DEBUG, "MythTV_PlugIn::CheckForNewRecordings run complete.");
}

void MythTV_PlugIn::AlarmCallback(int id, void* param)
{
	if( id==CHECK_FOR_NEW_RECORDINGS )
		CheckForNewRecordings();
	else if( id==START_SCAN_JOB )
	{
		ScanJob *pScanJob = (ScanJob *) param;
		StartScanJob(pScanJob);
	}
	else if( id==RUN_BACKEND_STARTER )
		RunBackendStarter();
	else if( id==CHECK_FOR_SCHEDULED_RECORDINGS )
		UpdateUpcomingRecordings();
	else if( id==CONFIRM_MASTER_BACKEND_OK )
		ConfirmMasterBackendOk((int)(long) param);
	else if( id==SYNC_PROVIDERS_AND_CARDS )
		CMD_Sync_Providers_and_Cards(0,0);
}

bool MythTV_PlugIn::NewBookmarks( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	m_bBookmarksNeedRefreshing=true;
	return false;
}

bool MythTV_PlugIn::TuneToChannel( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( pMessage->m_dwPK_Device_To!=m_pMedia_Plugin->m_dwPK_Device &&  // Let media plugin convert this to the destination device and add the stream id
		pMessage->m_mapParameters.find(COMMANDPARAMETER_ProgramID_CONST)!=pMessage->m_mapParameters.end() )
	{
		MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str()),0);
		if( pMediaStream && pMediaStream->GetType()==MEDIASTREAM_TYPE_MYTHTV )
			return false;  // it's for a myth player, no processing needed

		string sProgramID = pMessage->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST];
		if( sProgramID.size()>1 && sProgramID[0]=='i' )
		{
			// It's a channel ID, not a channel number.  Convert it
			string sSQL = "SELECT channum FROM channel WHERE chanid=" + sProgramID.substr(1);
			PlutoSqlResult result_set_check;
			DB_ROW row;
			if( (result_set_check.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL && (row=db_wrapper_fetch_row(result_set_check.r))!=NULL && row && row[0] )
				pMessage->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST] = row[0];
		}
	}
	return false;
}

void MythTV_PlugIn::RefreshBookmarks()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	m_bBookmarksNeedRefreshing=false;

	for(map<string,MapBookmark *>::iterator it=m_mapSeriesBookmarks.begin();it!=m_mapSeriesBookmarks.end();++it)
		delete it->second;
	m_mapSeriesBookmarks.clear();
	for(map<string,MapBookmark *>::iterator it=m_mapProgramBookmarks.begin();it!=m_mapProgramBookmarks.end();++it)
		delete it->second;
	m_mapProgramBookmarks.clear();
	for(map<int,MythChannel *>::iterator it=m_mapMythChannel.begin();it!=m_mapMythChannel.end();++it)
		it->second->m_mapBookmark.clear();

	BuildChannelList();

	string sSQL = "SELECT PK_Bookmark,EK_Users,Position FROM Bookmark WHERE EK_MediaType=" TOSTRING(MEDIATYPE_pluto_LiveTV_CONST);
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pMedia_Plugin->m_pDatabase_pluto_media->db_wrapper_query_result(sSQL)) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			char *pPos = NULL;
			if( !row[2] )
				continue;
			if( (pPos=strstr(row[2]," PROG:"))!=NULL )
			{
				string sProgram = (const char *) &row[2][6];
				MapBookmark *pMapBookmark;
				map<string,MapBookmark *>::iterator it=m_mapProgramBookmarks.find(sProgram);
				if( it==m_mapProgramBookmarks.end() )
				{
					pMapBookmark = new MapBookmark;
					m_mapProgramBookmarks[sProgram]=pMapBookmark;
				}
				else
					pMapBookmark = it->second;
				(*pMapBookmark)[ row[1] ? atoi(row[1]) : 0 ] = atoi(row[0]);
			}
			else if( (pPos=strstr(row[2]," SERIES:"))!=NULL )
			{
				string sProgram = (const char *) &row[2][8];
				MapBookmark *pMapBookmark;
				map<string,MapBookmark *>::iterator it=m_mapSeriesBookmarks.find(sProgram);
				if( it==m_mapSeriesBookmarks.end() )
				{
					pMapBookmark = new MapBookmark;
					m_mapSeriesBookmarks[sProgram]=pMapBookmark;
				}
				else
					pMapBookmark = it->second;
				(*pMapBookmark)[ row[1] ? atoi(row[1]) : 0 ] = atoi(row[0]);
			}
			else if( (pPos=strstr(row[2]," CHAN:"))!=NULL )
			{
				int ChanId = atoi( (const char *) (row[2][6]=='i' ? &row[2][7] : &row[2][6]) );  // Skip any leading 'i'
				MythChannel *pMythChannel = m_mapMythChannel_Find(ChanId);
				if( pMythChannel )
					pMythChannel->m_mapBookmark[ row[1] ? atoi(row[1]) : 0 ] = atoi(row[0]);
			}
		}
	}
}

void MythTV_PlugIn::PurgeChannelList()
{
	for(map<int,MythChannel *>::iterator it=m_mapMythChannel.begin();it!=m_mapMythChannel.end();++it)
		delete it->second;
	m_mapMythChannel.clear();
	m_ListMythChannel.clear();

	for(map<int,MythSource *>::iterator it=m_mapMythSource.begin();it!=m_mapMythSource.end();++it)
		delete it->second;
	m_mapMythSource.clear();
}

void MythTV_PlugIn::BuildChannelList()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	PurgeChannelList();

	string sSQL = "SELECT channel.chanid, channum, callsign, name, icon, pic.EK_Picture, sourceid "
		"FROM channel "
		"LEFT JOIN `pluto_myth`.`Picture` AS pic ON channel.chanid=pic.chanid";
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL )
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			MythSource *pMythSource = m_mapMythSource_Find( atoi(row[6]) );
			if( !pMythSource )
			{
				string sDescription = row[6];

				sSQL = "SELECT name FROM videosource WHERE sourceid=" + string(row[6]);
				PlutoSqlResult result2;
				DB_ROW row2;
				if( (result2.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL && (row2 = db_wrapper_fetch_row(result2.r))!=NULL )
				{
					string s = row2[0];
					// This is probably in the format Provider X.  Try to get that better description
					if( s.size()>9 )
					{
						int Provider = atoi(s.substr(8).c_str());
						Row_MediaProvider *pRow_MediaProvider = m_pMedia_Plugin->m_pDatabase_pluto_media->MediaProvider_get()->GetRow(Provider);
						if( pRow_MediaProvider && pRow_MediaProvider->Description_get().empty()==false )
							sDescription += " " + pRow_MediaProvider->Description_get();
					}
					else
						sDescription += " " + s;
				}
				pMythSource = new MythSource(atoi(row[6]),sDescription);
				m_mapMythSource[ pMythSource->m_dwID ] =pMythSource;
			}

			string sFilename;
			if( row[5] )
				sFilename = "/home/mediapics/" + string(row[5]) + "_tn.jpg";
			else if( row[4] )
				sFilename = row[4];

			size_t size=0;
			char *pData = sFilename.empty() ? NULL : FileUtils::ReadFileIntoBuffer(sFilename,size);
			MythChannel *pMythChannel = new MythChannel( atoi(row[0]), atoi(row[1]), pMythSource, row[2] ? row[2] : "", row[3] ? row[3] : "", pData, size );
			m_mapMythChannel[pMythChannel->m_dwID] = pMythChannel;
			m_ListMythChannel.push_back(pMythChannel);
		}
	}

	m_ListMythChannel.sort(ChannelComparer);
}

class DataGridTable *MythTV_PlugIn::FavoriteChannels( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();

	string sPK_Users = Parms.empty() ? " AND EK_Users IS NULL " : " AND (EK_Users IS NULL OR EK_Users=" + Parms + ")";
	string sSQL = "EK_MediaType=" TOSTRING(MEDIATYPE_pluto_LiveTV_CONST) + sPK_Users + " AND Position like '%CHAN:%' ORDER BY Description";
	vector<Row_Bookmark *> vectRow_Bookmark;
	 m_pMedia_Plugin->m_pDatabase_pluto_media->Bookmark_get()->GetRows(sSQL,&vectRow_Bookmark);

	int iRow=0;
	DataGridTable *pDataGrid = new DataGridTable();
	for(vector<Row_Bookmark *>::iterator it=vectRow_Bookmark.begin();it!=vectRow_Bookmark.end();++it)
	{
		Row_Bookmark *pRow_Bookmark = *it;
		DataGridCell *pCell = new DataGridCell( pRow_Bookmark->Description_get(), StringUtils::itos( pRow_Bookmark->PK_Bookmark_get() ) );
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *MythTV_PlugIn::FavoriteShows( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();

	string sPK_Users = Parms.empty() ? " AND EK_Users IS NULL " : " AND (EK_Users IS NULL OR EK_Users=" + Parms + ")";
	string sSQL = "EK_MediaType=" TOSTRING(MEDIATYPE_pluto_LiveTV_CONST) + sPK_Users + " AND (Position like '%PROG:%' OR Position like '%SERIES:%') ORDER BY Description";
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pMedia_Plugin->m_pDatabase_pluto_media->Bookmark_get()->GetRows(sSQL,&vectRow_Bookmark);

	int iRow=0;
	DataGridTable *pDataGrid = new DataGridTable();
	for(vector<Row_Bookmark *>::iterator it=vectRow_Bookmark.begin();it!=vectRow_Bookmark.end();++it)
	{
		Row_Bookmark *pRow_Bookmark = *it;
		DataGridCell *pCell = new DataGridCell( pRow_Bookmark->Description_get(), StringUtils::itos( pRow_Bookmark->PK_Bookmark_get() ) );
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *MythTV_PlugIn::ThumbnailableAttributes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

	MythTvMediaStream *pMythTvMediaStream = NULL;
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || 
		(pMythTvMediaStream = ConvertToMythMediaStream(pEntertainArea->m_pMediaStream, "MythTV_PlugIn::ThumbnailableAttributes() ")) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ThumbnailableAttributes can't get stream or ea");
		return pDataGrid;
	}

	MythChannel *pMythChannel = m_mapMythChannel_Find( pMythTvMediaStream->m_iCurrentProgramChannelID );
	if( !pMythChannel )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ThumbnailableAttributes can't get channel %d", pMythTvMediaStream->m_iCurrentProgramChannelID );
		return pDataGrid;
	}

	int iRow=0;
	DataGridCell *pCell = new DataGridCell( "Channel: " + StringUtils::itos(pMythChannel->m_dwChanNum) + " " + pMythChannel->m_sShortName, 
		"CHAN:" + StringUtils::itos( pMythChannel->m_dwID ) );
	pDataGrid->SetData(0,iRow++,pCell);

	// Try to find what series/program is on now
	string sSQL = "SELECT seriesid, programid, title "
		"FROM program "
		"WHERE starttime < UTC_TIMESTAMP() AND endtime > UTC_TIMESTAMP() "
		" AND chanid=" + StringUtils::itos(pMythTvMediaStream->m_iCurrentProgramChannelID);

	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))!=NULL && (row = db_wrapper_fetch_row(result.r)) )
	{
		if( row[0] && row[0][0] )
		{
			pCell = new DataGridCell( row[2], "SERIES:" + string(row[0]) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
		else if( row[1] && row[1][0] )
		{
			pCell = new DataGridCell( row[2], "PROG:" + string(row[1]) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	return pDataGrid;
}

//<-dceag-c846-b->

	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

void MythTV_PlugIn::CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c846-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	if( !pData || iData_Size==0 || sFilename.size()<3 )
		return;

	Row_Picture *pRow_Picture = m_pMedia_Plugin->m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddPicture(pData,iData_Size,"JPG","");
	if( !pRow_Picture )
		return;

	string sWhere,sInsert;
	string::size_type pos;
	if( (pos=sFilename.find("CHAN:"))!=string::npos )
	{
		string sSQL = "DELETE FROM `pluto_myth`.`Picture` WHERE chanid=" + sFilename.substr(pos+5);
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		sSQL = "INSERT INTO `pluto_myth`.`Picture`(EK_Picture,chanid) VALUES(" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "," + sFilename.substr(pos+5) + ")";
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		BuildChannelList();
	}
	else if( (pos=sFilename.find("SERIES:"))!=string::npos )
	{
		string sSQL = "DELETE FROM `pluto_myth`.`Picture` WHERE seriesid='" + sFilename.substr(pos+7) + "'";
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		sSQL = "INSERT INTO `pluto_myth`.`Picture`(EK_Picture,seriesid) VALUES(" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + ",'" + sFilename.substr(pos+7) + "')";
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
	}
	else if( (pos=sFilename.find("PROG:"))!=string::npos )
	{
		string sSQL = "DELETE FROM `pluto_myth`.`Picture` WHERE programid='" + sFilename.substr(pos+5) + "'";
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
		sSQL = "INSERT INTO `pluto_myth`.`Picture`(EK_Picture,programid) VALUES(" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + ",'" + sFilename.substr(pos+5) + "')";
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
	}
}

void MythTV_PlugIn::StartScanningScript(Row_Device *pRow_Device_Tuner,Row_Device *pRow_Device_CaptureCard,int iPK_Orbiter,string sScanningScript)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	map< int, ScanJob * >::iterator it=m_mapPendingScans.find( pRow_Device_Tuner->PK_Device_get() );
	if( it!=m_mapPendingScans.end() )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanningScript %d/%d already scanning", pRow_Device_Tuner->PK_Device_get(), pRow_Device_CaptureCard->PK_Device_get() );
		return;
	}

	ScanJob *pScanJob = new ScanJob( pRow_Device_CaptureCard, pRow_Device_Tuner, iPK_Orbiter, sScanningScript );
	m_mapPendingScans[ pScanJob->m_pRow_Device_Tuner->PK_Device_get() ] = pScanJob;
	StartScanJob(pScanJob);
}

void MythTV_PlugIn::StartScanJob(ScanJob *pScanJob)
{
	if( pScanJob->m_bActive )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanJob %d/%d active", pScanJob->m_pRow_Device_Tuner->PK_Device_get(),pScanJob->m_pRow_Device_CaptureCard->PK_Device_get());
		return;
	}

	// If we're still busy downloading packages we don't want to do this yet since the files may not be installed yet
	if( m_pGeneral_Info_Plugin->PendingConfigs() )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanJob %d/%d waiting for packages", pScanJob->m_pRow_Device_Tuner->PK_Device_get(),pScanJob->m_pRow_Device_CaptureCard->PK_Device_get() );
		m_pAlarmManager->AddRelativeAlarm(30,this,START_SCAN_JOB,(void *) pScanJob);  /* check again in 30 seconds */
		return;
	}

	if( pScanJob->m_pRow_Device_CaptureCard )
		pScanJob->m_pRow_Device_CaptureCard->Reload();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanJob card %d %s",
		(pScanJob->m_pRow_Device_CaptureCard ? pScanJob->m_pRow_Device_CaptureCard->PK_Device_get() : -1),
		(pScanJob->m_pRow_Device_CaptureCard ? pScanJob->m_pRow_Device_CaptureCard->Status_get().c_str() : "none") );

	if( pScanJob->m_pRow_Device_CaptureCard && pScanJob->m_pRow_Device_CaptureCard->Status_get()=="**RUN_CONFIG**" )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MythTV_PlugIn::StartScanJob %d/%d waiting for configure", pScanJob->m_pRow_Device_Tuner->PK_Device_get(),
			pScanJob->m_pRow_Device_CaptureCard->PK_Device_get() );
		m_pAlarmManager->AddRelativeAlarm(30,this,START_SCAN_JOB,(void *) pScanJob);  /* check again in 30 seconds */
		return;
	}

	// Before we start be sure the block device is in the database
	bool bTunersAsSeparateDevices = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pScanJob->m_pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Children_as_Separate_Tuners_CONST)=="1";
	int cardid = atoi(DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,bTunersAsSeparateDevices ? pScanJob->m_pRow_Device_Tuner->PK_Device_get() : pScanJob->m_pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Port_CONST).c_str());
	string sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pScanJob->m_pRow_Device_Tuner->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
	if( sBlockDevice.empty() )
		sBlockDevice = DatabaseUtils::GetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,pScanJob->m_pRow_Device_CaptureCard->PK_Device_get(),DEVICEDATA_Block_Device_CONST);
	if( sBlockDevice.empty()==false )
	{
		string sSQL = "UPDATE `capturecard` SET videodevice='" + sBlockDevice + "' WHERE cardid=" + StringUtils::itos(cardid);
		m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);
	}

	bool bNeedConfigure=true;
	DeviceData_Base *pDevice_App_Server = (DeviceData_Router *) m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
	if( pDevice_App_Server )
	{
		string sScanFailed = "0 -1000 2 " TOSTRING(EVENT_Channel_Scan_Progress_CONST) " " 
			TOSTRING(EVENTPARAMETER_PK_Device_CONST) " " + StringUtils::itos( pScanJob->m_pRow_Device_Tuner->PK_Device_get() ) + " "
			TOSTRING(EVENTPARAMETER_Result_CONST) " -1 "
			TOSTRING(EVENTPARAMETER_Text_CONST) " \"Failed to start\" ";

		string sArguments = StringUtils::itos(pScanJob->m_pRow_Device_Tuner->PK_Device_get()) + "\t" + StringUtils::itos(pScanJob->m_pRow_Device_CaptureCard->PK_Device_get());
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			SCRIPT_DIR + pScanJob->m_sScanJob,"scanchannels",sArguments,sScanFailed,"",false,false,false,true);
		string sResponse;
		if( !SendCommand(CMD_Spawn_Application,&sResponse) || sResponse!="OK" )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::StartScanJob -- app server didn't respond");
			m_pAlarmManager->AddRelativeAlarm(30,this,START_SCAN_JOB,(void *) pScanJob);  /* check again in 30 seconds */
		}
		else
		{
			pScanJob->m_bActive=true;
			bNeedConfigure=false;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::StartScanJob -- no app server");
		m_mapPendingScans.erase( pScanJob->m_pRow_Device_Tuner->PK_Device_get() );
		delete pScanJob;
		return;
	}

	string sDevices = StringUtils::itos(pScanJob->m_pRow_Device_CaptureCard->PK_Device_get()) + "," 
			+ StringUtils::itos(pScanJob->m_pRow_Device_Tuner->PK_Device_get());
	string sSQL;

	if( bNeedConfigure )
		sSQL = "UPDATE Device SET NeedConfigure=1,Status='**RUN_CONFIG**' WHERE PK_Device IN(" + sDevices + ")";
	else
		sSQL = "UPDATE Device SET NeedConfigure=0 WHERE PK_Device IN(" + sDevices + ")";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::StartScanJob myth plugin check %s",sSQL.c_str());
	
	m_pMedia_Plugin->m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
	pScanJob->m_pRow_Device_CaptureCard->Reload();
	pScanJob->m_pRow_Device_Tuner->Reload();
}

bool MythTV_PlugIn::ReportPendingTasks(PendingTaskList *pPendingTaskList)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "checking scanning jobs %d %d",m_dwPK_Device, (int) m_mapPendingScans.size());

	if( m_mapPendingScans.size() )
	{
		if( pPendingTaskList )
		{
			for( map< int, ScanJob * >::iterator it=m_mapPendingScans.begin();it!=m_mapPendingScans.end();++it )
			{
				ScanJob *pScanJob = it->second;
				string sDesc = "Scanning for channels: " + pScanJob->m_pRow_Device_CaptureCard->Description_get() + "/" + pScanJob->m_pRow_Device_Tuner->Description_get() + " ";
				if( pScanJob->m_bActive )
					sDesc += StringUtils::itos(pScanJob->m_iPercentCompletion) + "% ";
				else
					sDesc += "waiting...";

				pPendingTaskList->m_listPendingTask.push_back(new PendingTask(pScanJob->m_pRow_Device_Tuner->PK_Device_get(),
					m_dwPK_Device,m_dwPK_Device,
					"channel_scan","Scanning Channels: " + sDesc + " " + pScanJob->m_sStatus,
					-1,0,true));
			}
		}
		return true;
	}
	return false;
}

bool MythTV_PlugIn::ScanningProgress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	int		iPK_Device = atoi( pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str( ) );
	int		iResult  = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str( ) );
	int		iValue  = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST].c_str( ) );
	string      sText = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];

	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	map< int, ScanJob * >::iterator it=m_mapPendingScans.find(iPK_Device);
	if( it==m_mapPendingScans.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ScanningProgress cannot find job for %d", iPK_Device);
		return false;
	}

	ScanJob *pScanJob = it->second;
	if( iResult!=0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::ScanningProgress done for job %d", iPK_Device);

		string sSQL = "SELECT DISTINCT cardid,modulation,tvformat FROM cardinput "
			"JOIN channel ON cardinput.sourceid=channel.sourceid "
			"JOIN dtv_multiplex ON dtv_multiplex.mplexid=channel.mplexid ";
		PlutoSqlResult result;
		DB_ROW row=NULL;
		
		if( ( result.r=m_pDBHelper_Myth->db_wrapper_query_result( sSQL ) ) )
		{
			while( ( row=db_wrapper_fetch_row( result.r ) ) )
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::ScanningProgress sql %s %s %s", row[0], row[1], row[2]);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::ScanningProgress sql nothing %p %p", result.r, row);

		DCE::CMD_Sync_Providers_and_Cards CMD_Sync_Providers_and_Cards(m_dwPK_Device,m_dwPK_Device,iPK_Device,pScanJob->m_iPK_Orbiter);
		QueueMessageToRouter(CMD_Sync_Providers_and_Cards.m_pMessage);  // Do this asynchronously
		m_mapPendingScans.erase(it);
		delete pScanJob;
		return false;
	}

	pScanJob->m_sStatus = sText;
	pScanJob->m_iPercentCompletion = iValue;

	return false;
}

void MythTV_PlugIn::CheckForTvFormatAndProvider( int iPK_Device )
{
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pMedia_Plugin->m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(iPK_Device,DEVICEDATA_Port_CONST);
	if( !pRow_Device_DeviceData || pRow_Device_DeviceData->IK_DeviceData_get().empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CheckForTvFormatAndProvider cannot determine anything without a port");
		return;
	}

	string sSQL = "SELECT DISTINCT modulation,tvformat FROM cardinput "
		"JOIN channel ON cardinput.sourceid=channel.sourceid "
		"JOIN dtv_multiplex ON dtv_multiplex.mplexid=channel.mplexid "
		"WHERE cardinput.cardid=" + pRow_Device_DeviceData->IK_DeviceData_get();

	PlutoSqlResult result;
	DB_ROW row=NULL;
	if( (result.r = m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && ( row=db_wrapper_fetch_row( result.r ) ) && row[0] && row[1] )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CheckForTvFormatAndProvider row count %s / %s", row[0], row[1]);
		if( result.r->row_count>1 )
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MythTV_PlugIn::CheckForTvFormatAndProvider found more than 1 tv format for %d",iPK_Device);
		DatabaseUtils::SetDeviceData(m_pMedia_Plugin->m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_Type_CONST,row[0]);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CheckForTvFormatAndProvider nothing got for %s %p/%p", sSQL.c_str(), result.r, row);
}
//<-dceag-c882-b->

	/** @brief COMMAND: #882 - Abort Task */
	/** Abort a pending task */
		/** @param #248 Parameter ID */
			/** The ID of the task to abort */

void MythTV_PlugIn::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{
}

bool MythTV_PlugIn::SetPaths()
{
	
	bool bChanged = false;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SetPaths");

	// Get all the backend ip's so we can match the hostname myth uses to our own device id
	map<string,int> mapIpToDevice;
	vector<Row_Device *> vectRow_Device;
	// Get the top level devices
	m_pMedia_Plugin->m_pDatabase_pluto_main->Device_get()->GetRows("FK_Device_ControlledVia IS NULL",&vectRow_Device);
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SetPaths ip %s=%d",(*it)->IPaddress_get().c_str(),(*it)->PK_Device_get());
		mapIpToDevice[ (*it)->IPaddress_get() ] = (*it)->PK_Device_get();
	}

	int PK_Device_Storage = atoi(DATA_Get_PK_Device().c_str());
	string sFilename = PK_Device_Storage ? "/mnt/device/" + StringUtils::itos(PK_Device_Storage) + "/public/data/videos/tv_shows_" : "/home/public/data/videos/tv_shows_";

	string sSQL = "SELECT data,hostname FROM settings WHERE value='BackendServerIP'";
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r = m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			if( !row[1] || !row[1][0] )
				continue;
			int PK_Device = row[0] ? mapIpToDevice[row[0]] : 0;

			if(PK_Device == 0)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_PlugIn::SetPaths: won't create tv_shows for device 0. We are probably out of sync!");
				continue;
			}

			string sDirectory = sFilename + StringUtils::itos(PK_Device);

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SetPaths row %s/%s device %d directory %s", row[1], row[0] ? row[0] : "x", PK_Device, sDirectory.c_str());

			//TODO:
			//This would be a good place to update the "Default" mythtv storage group to the device with the most space...

			/*
			Note:This section is no longer needed, as this is now handled in PlutoStorageDevices
			const string CmdList[] =
			{
				"mkdir -p \"" + sDirectory + "\"",
				"chmod 2775 \"" + sDirectory +"\"",
				"chown root.public \"" + sDirectory +"\"",
				""
			};

			for (int i = 0; CmdList[i] != ""; i++)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::Register %s", CmdList[i].c_str());
				system(CmdList[i].c_str());
			}*/

			bChanged |= UpdateMythSetting("LiveBufferDir",sDirectory,row[1]);
			bChanged |= UpdateMythSetting("RecordFilePrefix",sDirectory,row[1]);
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SetPaths : change noted? %s", ((bChanged) ? "yes" : "no"));
	return bChanged;
} 

void MythTV_PlugIn::RunBackendStarter()
{
	DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST,this);
	if( pDevice_App_Server )
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			"/usr/bin/screen", "restart_myth", "-d\t-m\t-S\tRestart_Myth_Backend\t" + SCRIPT_RESTART_BACKEND,
			"", "", false, false, false, false);
		CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Retry;
		SendCommand(CMD_Spawn_Application);

		string sStatus = GetStatus();
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::RunBackendStarter %s", sStatus.c_str());
		if( sStatus=="FILLDATABASE" )
			StartFillDatabase();
	}
}

void MythTV_PlugIn::StartFillDatabase(bool bNotifyUser)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::StartFillDatabase starting fill");

	DeviceData_Router *pDevice_App_Server=NULL,*pDevice_Us = m_pRouter->m_mapDeviceData_Router_Find(m_dwPK_Device);
	if( pDevice_Us )
		pDevice_App_Server = (DeviceData_Router *) pDevice_Us->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);

	if( pDevice_App_Server )
	{
		string sResponse = "fill done";
		DCE::CMD_Spawn_Application CMD_Spawn_Application_fill(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
			SCRIPT_INITIAL_FILLDB, "filldb", "", "", "", false, false, true, false);
		CMD_Spawn_Application_fill.m_pMessage->m_eRetry=MR_Persist;
		if( !SendCommand(CMD_Spawn_Application_fill) )
		{
			m_bFillDbRunning=false;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::StartFillDatabase failed m_bFillDbRunning=false");
		}

		SetStatus("FILLDATABASE");  // A marker so we know if we do a reload before this finishes

		if( bNotifyUser )
		{
			DCE::SCREEN_PopupMessage SCREEN_PopupMessage(
				m_dwPK_Device, DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
				"It will take about few minutes to retrieve your channel listings. "
				"Please wait until you see the message \"MythTV is ready\" before "
				"you start using TV features.", // Main message
				"", // Command Line
				"generic message", // Description
				"0", // sPromptToResetRouter
				"300", // sTimeout
				"1"); // sCannotGoBack
			SendCommand(SCREEN_PopupMessage);
		}
	}
}

//<-dceag-c910-b->

	/** @brief COMMAND: #910 - Reporting EPG Status */
	/** Reporting the status of an EPG update */
		/** @param #9 Text */
			/** Any messages about this */
		/** @param #40 IsSuccessful */
			/** true if the process succeeded */
		/** @param #257 Task */
			/** The type of EPG task: channel (retrieving channels), guide (retrieving guide) */

void MythTV_PlugIn::CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage)
//<-dceag-c910-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Report_EPG Success %d m_bNeedToRunFillDb %d m_bFillDbRunning %d",
		bIsSuccessful,(int) m_bNeedToRunFillDb,(int) m_bFillDbRunning);
	if( m_bNeedToRunFillDb )  // A flag to fill the database again when the first one finished
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Reporting_EPG_Status restarting m_bFillDbRunning=true");
		m_bNeedToRunFillDb=false;
		m_bFillDbRunning=true;  // Reset the flag
		StartFillDatabase(true);  // notify the user because the only way to have multiple fills queued is if he added a provider
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::CMD_Reporting_EPG_Status done m_bFillDbRunning=false");
		m_bFillDbRunning=false;
		SetStatus("");  // A marker so we know if we do a reload before this finishes
	}
}

bool MythTV_PlugIn::SafeToReload(string &sReason)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::SafeToReload m_bFillDbRunning %d m_bNeedToRunFillDb %d",
		(int) m_bFillDbRunning,(int) m_bNeedToRunFillDb);
	if( m_bFillDbRunning )
	{
		sReason = "MythTV_PlugIn loading EPG data";
		return false;
	}
	if( m_mapPendingScans.size() )
	{
		sReason = "MythTV_PlugIn scanning channels";
		return false;
	}

	return true;
}

void MythTV_PlugIn::UpdateUpcomingRecordings()
{
	if( !m_pMythBackEnd_Socket )
		return;

	string sResponse;
	m_pMythBackEnd_Socket->SendMythString("QUERY_GETALLPENDING",&sResponse);
	if( sResponse.empty() )  // Invalid data, try again in 1 minute
	{
		m_pAlarmManager->AddRelativeAlarm(60,this,CHECK_FOR_SCHEDULED_RECORDINGS,NULL);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::UpdateUpcomingRecordings no recordings");
		return;
	}

	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	string::size_type pos=0;
	const char *pToken = "[]:[]"; // What kind of token is this??
	MythRecording mythRecording;

	string sHasConflicts = "", sListSize = "";
	int iListSize = 0;

	if (pos < sResponse.size())
		sHasConflicts = StringUtils::Tokenize(sResponse,pToken,pos,true);

	if (pos < sResponse.size())
	{
		sListSize = StringUtils::Tokenize(sResponse,pToken,pos,true);
		iListSize = atoi(sListSize.c_str());
	}

	LoggerWrapper::GetInstance()->Write(
		LV_STATUS,"MythTV_PlugIn::UpdateUpcomingRecordings got listsize(%d) entries(%d) m_mapScheduledRecordings size was: %d",
		(int) sResponse.size(), iListSize, (int) m_mapScheduledRecordings.size());
	m_mapScheduledRecordings.clear();

	while (pos<sResponse.size())
	{
		string sTitle        = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sSubtitle     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sDescription  = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sCategory     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sChanId       = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sChannel      = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sCallSign     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sChannelName  = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sPathName     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sFileSizeHW   = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sFileSizeLW   = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sStartTime    = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sStopTime     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sDuplicate    = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sSharable     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sFindID       = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sHostname     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sSourceID     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sCardID       = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sInputID      = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecPriority  = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecStatus    = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecordID     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecType      = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sDupIn        = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sDupMethod    = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecStartTime = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecStopTime  = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRepeat       = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sProgramFlags = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecGroup     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sChanCommFree = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sChanOutputFilters=StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sSeriesID     = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sProgramID    = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sLastModified = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sStars        = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sOrigAirDate  = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sHasAirDate   = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sPlaybackGroup= StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sRecPriority2 = StringUtils::Tokenize(sResponse,pToken,pos,true);
		string sParentID     = StringUtils::Tokenize(sResponse,pToken,pos,true);

		mythRecording.channel_id           = atoi(sChanId.c_str());
		mythRecording.scheduled_start_time = atoi(sStartTime.c_str());

		if (sRecType == "1")
                {
			m_mapScheduledRecordings[mythRecording.key()] = 
				make_pair<char,int> ('O',atoi(sRecordID.c_str()));
		}
                else if (sRecType == "3")
                {
			m_mapScheduledRecordings[mythRecording.key()] = 
				make_pair<char,int> ('C',atoi(sRecordID.c_str()));;
		}

		LoggerWrapper::GetInstance()->Write(
			LV_STATUS,"MythTV_PlugIn::UpdateUpcomingRecordings "
			"adding record id " UINT64_PRINTF " / %2s type %s chan %5d start %s m_mapScheduledRecordings size %d",
			(u_int64_t) mythRecording.key(), sRecordID.c_str(), sRecType.c_str(), mythRecording.channel_id,
			ctime(&mythRecording.scheduled_start_time),(int) m_mapScheduledRecordings.size());
	}
	LoggerWrapper::GetInstance()->Write(
		LV_STATUS,"MythTV_PlugIn::UpdateUpcomingRecordings done m_mapScheduledRecordings size %d",
		(int) m_mapScheduledRecordings.size());
	m_pAlarmManager->CancelAlarmByType(CHECK_FOR_SCHEDULED_RECORDINGS);
	m_pAlarmManager->AddRelativeAlarm(60 * 60,this,CHECK_FOR_SCHEDULED_RECORDINGS,NULL);  // Update once an hour just in case
}

//<-dceag-c911-b->

	/** @brief COMMAND: #911 - Remove Scheduled Recording */
	/** Remove a scheduled recording */
		/** @param #10 ID */
			/** The ID of the recording rule to remove.  This will remove all recordings with this ID, and ProgramID is ignored if this is specified. */
		/** @param #68 ProgramID */
			/** The ID of the program to remove.  If ID is empty, remove just this program. */

void MythTV_PlugIn::CMD_Remove_Scheduled_Recording(string sID,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c911-e->
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	string sSQL = "DELETE FROM record WHERE recordid=" + sID;
	m_pDBHelper_Myth->threaded_db_wrapper_query(sSQL);

	if( m_pMythBackEnd_Socket )
		m_pMythBackEnd_Socket->SendMythStringGetOk("RESCHEDULE_RECORDINGS -1");
}

void MythTV_PlugIn::ConfirmMasterBackendOk(int iMediaStreamID)
{
	if( !m_pMythBackEnd_Socket )
		return;
	// If Myth isn't reachable, forcibly kill it and notify the user
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	MediaStream *pMediaStream = iMediaStreamID ? m_pMedia_Plugin->m_mapMediaStream_Find(iMediaStreamID,0) : NULL;

	while(m_pMythBackEnd_Socket->ProcessSocket());  // Keep processing as long as there's incoming strings

	string sResponse;
	m_pMythBackEnd_Socket->SendMythString("QUERY_UPTIME",&sResponse);
	if( sResponse.empty() )
	{
		if( m_iRetriesStartingMyth++>50 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ConfirmMasterBackendOk -- restarted myth 50 times already.  Aborting");
			return;
		}
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ConfirmMasterBackendOk -- lost communication with it.  Forcing it to die");
		DeviceData_Base *pDevice_App_Server = m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
		if( pDevice_App_Server )
		{
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_App_Server->m_dwPK_Device,
				SCRIPT_FORCE_KILL,"killmyth","mythbackend","","",false,false,false,true);
			SendCommand(CMD_Spawn_Application);
		}

		string sMessage = "MythTV seems to have died.  I'm trying to reset it now.  If you do not see tv, try stopping the tv and restarting it.  If you still have problems, you may need to reboot.";
		if( pMediaStream )
		{
			int PK_Device = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device_ControlledVia;
			if( pMediaStream->m_pOH_Orbiter_StartedMedia )
				PK_Device = pMediaStream->m_pOH_Orbiter_StartedMedia->m_pDeviceData_Router->m_dwPK_Device;
			DCE::SCREEN_DialogGenericError SCREEN_DialogGenericError(m_dwPK_Device,PK_Device,
				sMessage,"0","20000","1");
			SendCommand(SCREEN_DialogGenericError);
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::ConfirmMasterBackendOk got %s", sResponse.c_str());

		// The protocol is really lame since messages can come in asyncronously and there's way to know what incoming
		// message is in response to which outgoing message.  Chances are we received the response to uptime, but it's 
		// possible that it's some other back end message, so process it anyway
		m_pMythBackEnd_Socket->ProcessIncomingString(sResponse);
	}

	m_pAlarmManager->CancelAlarmByType(CONFIRM_MASTER_BACKEND_OK);  // Just be sure we're not in here more than once
	m_pAlarmManager->AddRelativeAlarm(30,this,CONFIRM_MASTER_BACKEND_OK,NULL);
}

bool MythTV_PlugIn::PlaybackStarted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
    int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
	/*
	string sSectionDescription = pMessage->m_mapParameters[EVENTPARAMETER_SectionDescription_CONST];
	string sAudio = pMessage->m_mapParameters[EVENTPARAMETER_Audio_CONST];
	string sVideo = pMessage->m_mapParameters[EVENTPARAMETER_Video_CONST];
	*/
    MediaStream * pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( iStreamID, pMessage->m_dwPK_Device_From );
    MythTvMediaStream * pMythTvMediaStream;

    if ( pMediaStream == NULL || (pMythTvMediaStream = ConvertToMythMediaStream(pMediaStream, "MythTV_PlugIn::PlaybackStarted() "))==NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "MythTV_PlugIn::PlaybackStarted Stream ID %d is not mapped to a media stream object", iStreamID);
        return false;
    }


	MythChannel *pMythChannel = m_mapMythChannel_Find( atoi(sMRL.c_str()) );
	if( pMythChannel==NULL )
	{
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::PlaybackStarted Stream ID %d no channel for %s in %d", iStreamID,sMRL.c_str(), (int) m_mapMythChannel.size());
        return false;
	}

	pMythTvMediaStream->m_sMediaDescription = pMythChannel->m_sLongName;
	pMythTvMediaStream->m_iCurrentProgramChannelID = pMythTvMediaStream->m_iTrackOrSectionOrChannel = pMythChannel->m_dwID;

	string sSQL = "SELECT title,subtitle,description FROM program WHERE chanid=" + sMRL + " AND "
		" starttime <= UTC_TIMESTAMP() AND endtime >= UTC_TIMESTAMP()";
	PlutoSqlResult result;
	DB_ROW row;
	if( (result.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL))==NULL || (row=db_wrapper_fetch_row(result.r))==NULL )
	{
		pMythTvMediaStream->m_sSectionDescription = "";
		pMythTvMediaStream->m_sMediaSynopsis = "";
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MythTV_PlugIn::PlaybackStarted Stream ID %d no guide data for %s", iStreamID,sMRL.c_str());
	}
	else
	{
		pMythTvMediaStream->m_sSectionDescription = row[0] ? row[0] : "";
		pMythTvMediaStream->m_sMediaSynopsis = row[2] ? row[2] : "";
	}

	m_pMedia_Plugin->MediaInfoChanged(pMythTvMediaStream,true);

	return false;
}

bool MythTV_PlugIn::NewRecording( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	string sFile = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];
	
	if( m_sNewRecordings.empty()==false )
		m_sNewRecordings += ",";
	m_sNewRecordings += "'" + FileUtils::FilenameWithoutPath(sFile) + "'";
	m_pAlarmManager->AddRelativeAlarm(60,this,CHECK_FOR_NEW_RECORDINGS,NULL);  /* check in 1 minute*/
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn::NewRecording now %s", m_sNewRecordings.c_str());
	return false;
}

bool MythTV_PlugIn::ConfirmSourceIsADestination(MythTvMediaStream *pMythTvMediaStream)
{
	MediaDevice *pMediaDevice_Myth = NULL; // Any xine that's in the destination
	for( MapEntertainArea::iterator itEA = pMythTvMediaStream->m_mapEntertainArea.begin( );itEA != pMythTvMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_MythTV_Player_CONST);
		if( !pListMediaDevice )
			continue;
		for(ListMediaDevice::iterator it=pListMediaDevice->begin();it!=pListMediaDevice->end();++it)
		{
			MediaDevice *pMediaDevice = *it;
			if( pMythTvMediaStream->m_pMediaDevice_Source->IsInEntertainArea( pEntertainArea ) )
				return true; // We're good.  The source is one of the destinations
			if( !pMediaDevice_Myth )
				pMediaDevice_Myth = pMediaDevice;
		}
	}
	
	// If we reached here, then the source is not in one of the destination areas
	if( !pMediaDevice_Myth )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MythTV_PlugIn::ConfirmSourceIsADestination no myth player can handle this out of %d ea's",pMythTvMediaStream->m_mapEntertainArea.size());
		return false;
	}
	pMythTvMediaStream->m_pMediaDevice_Source = pMediaDevice_Myth;
	return true;
}
//<-dceag-c986-b->

	/** @brief COMMAND: #986 - Sync Storage Groups */
	/** Synchronize Storage Groups. Sent in response to a storage device being added. */

void MythTV_PlugIn::CMD_Sync_Storage_Groups(string &sCMD_Result,Message *pMessage)
//<-dceag-c986-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MythTV_PlugIn - Synchronizing Storage Groups");
	system(SCRIPT_MYTHTV_SETUP.c_str());
}
