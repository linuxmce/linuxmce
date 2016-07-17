/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//<-dceag-d-b->
#include "VDRPlugin.h"
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
#include "VDRMediaStream.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "../Datagrid_Plugin/Datagrid_Plugin.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_media/Table_Bookmark.h"
#include "../DCE/DataGrid.h"

#define ALARM_UPDATE_EPG	1

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VDRPlugin::VDRPlugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VDRPlugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_VDRMutex("vdr"), m_ConMutex( "VDR Connection" ), m_EPGMutex("epg")
{
	pthread_cond_init( &m_VDRCond, NULL );
	m_VDRMutex.Init(NULL,&m_VDRCond);
	pthread_cond_init( &m_ConCond, NULL );
	m_ConMutex.Init(NULL,&m_ConCond);
	pthread_cond_init( &m_EPGCond, NULL );
	m_EPGMutex.Init(NULL,&m_EPGCond);
	m_sVDRIp="127.0.0.1";
	m_timeUpdateInterval = 7200; // intervals between updating EPG from VDR in seconds (2 hour)
	m_VDRConnection.SetAddress(m_sVDRIp, "2001");

	m_pAlarmManager=NULL;
	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(1);
	m_iEPGUpdateNo = 0;
}
//<-dceag-getconfig-b->
bool VDRPlugin::GetConfig()
{
	if( !VDRPlugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VDRPlugin::~VDRPlugin()
//<-dceag-dest-e->
{
	m_VDRConnection.Close();
}

void VDRPlugin::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
}




//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VDRPlugin::Register()
//<-dceag-reg-e->
{	
	m_iPriority=DATA_Get_Priority();
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin || !m_pDatagrid_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_VDR_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	// Use the player, not the plugin, as the template for datagrids so that <%=NPDT%> in orbiter works.  Myth plugin does the same grids
	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::CurrentShows))
		,DATAGRID_EPG_Current_Shows_CONST,DEVICETEMPLATE_VDR_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::AllShows))
		,DATAGRID_EPG_All_Shows_CONST,DEVICETEMPLATE_VDR_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::EPGGrid))
		,DATAGRID_EPG_Grid_CONST,DEVICETEMPLATE_VDR_CONST);
	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::FavoriteChannels))
		,DATAGRID_Favorite_Channels_CONST,DEVICETEMPLATE_VDR_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::FavoriteShows))
		,DATAGRID_Favorite_Shows_CONST,DEVICETEMPLATE_VDR_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator( new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&VDRPlugin::OtherShowtimes))
		,DATAGRID_Other_Showtimes_CONST,DEVICETEMPLATE_VDR_CONST);

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &VDRPlugin::NewBookmarks ), 0, 0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Save_Bookmark_CONST );

	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_VDR_CONST);
	if( pListDeviceData_Router )
	{
		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
		{
			DeviceData_Router *pDevice_VDRPlayer = *it;
			RegisterMsgInterceptor( ( MessageInterceptorFn )( &Media_Plugin::PlaybackStarted ), 0, pDevice_VDRPlayer->m_dwPK_Device, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Started_CONST );
			RegisterMsgInterceptor( ( MessageInterceptorFn )( &VDRPlugin::TuneToChannel ), 0, pDevice_VDRPlayer->m_dwPK_Device, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Tune_to_channel_CONST );
			RegisterMsgInterceptor( ( MessageInterceptorFn )( &VDRPlugin::PlaybackStarted ), pDevice_VDRPlayer->m_dwPK_Device, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Started_CONST );
		}
	}

	BuildChannelList();
	m_bBookmarksNeedRefreshing=false;
	RefreshBookmarks();
	UpdateTimers();

	return Connect(PK_DeviceTemplate_get()); 
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
void VDRPlugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VDRPlugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

class MediaStream *VDRPlugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}

	if ( pMediaDevice != NULL  && // test the media device only if it set
		pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_VDR_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_VDR_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
	}

	if ( !pMediaDevice )
	{
		for(size_t s=0;s<vectEntertainArea.size();++s)
		{
			EntertainArea *pEntertainArea = vectEntertainArea[0];
			pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea);
			if( pMediaDevice )
				break;
		}
		if( !pMediaDevice )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
			return NULL;
		}
	}

	VDRMediaStream *pVDRMediaStream = new VDRMediaStream( this, pMediaHandlerInfo, iPK_MediaProvider,
		pMediaDevice,
		iPK_Users, st_RemovableMedia, StreamID );

		//2008-04-01 burgi
		pVDRMediaStream->m_sMediaDescription = "Not available";
    pVDRMediaStream->m_sSectionDescription = "Not available";
    pVDRMediaStream->m_sMediaSynopsis = "Not available";

	return pVDRMediaStream;
}

bool VDRPlugin::StartMedia( class MediaStream *pMediaStream,string &sError )
{
	VDRMediaStream *pVDRMediaStream = (VDRMediaStream *) pMediaStream;
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "VDRPlugin::StartMedia() Starting media stream playback. pos: %d", pVDRMediaStream->m_iDequeMediaFile_Pos );
	int PK_Device = pVDRMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pVDRMediaStream->m_iStreamID_get( );

	for(map<int,bool>::iterator it=pVDRMediaStream->m_mapOrbiter_HasInitialPosition.begin();it!=pVDRMediaStream->m_mapOrbiter_HasInitialPosition.begin();++it)
		it->second = false;

	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	if(!m_ListVDRChannel.empty())
	{
		VDRChannel *pVDRChannel = *(m_ListVDRChannel.begin());
		pVDRMediaStream->m_pVDRChannel = pVDRChannel;

		if (pVDRMediaStream->StreamingRequired())
		  {
		    DCE::CMD_Start_Streaming CMD_Start_Streaming(m_dwPK_Device,
								 pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
								 pVDRMediaStream->m_iPK_MediaType,
								 pVDRMediaStream->m_iStreamID_get(),
								 " CHAN:" + StringUtils::itos(pVDRMediaStream->m_pVDRChannel->m_dwChanNum),
								 "",
								 pVDRMediaStream->GetTargets(DEVICETEMPLATE_VDR_CONST));
		    SendCommand(CMD_Start_Streaming);
		  }
		else
		  {
		    
		    DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device, PK_Device,
						       pVDRMediaStream->m_iPK_MediaType,
						       pVDRMediaStream->m_iStreamID_get(), 
						       " CHAN:" + StringUtils::itos(pVDRMediaStream->m_pVDRChannel->m_dwChanNum),"");
		    SendCommand(CMD_Play_Media);
		  }
	}	
	
	return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool VDRPlugin::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in VDR_PlugIn!");

	string SavedPosition;
	int PK_Device = pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pMediaStream->m_iStreamID_get( );
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,PK_Device,pMediaStream->m_iStreamID_get(),&SavedPosition);


	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( CMD_Stop_Media ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped",
			pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	}

	return MediaHandlerBase::StopMedia(pMediaStream);

}

MediaDevice *VDRPlugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_VDR_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
}



/*

COMMANDS TO IMPLEMENT

*/



//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void VDRPlugin::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	/*
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) &&
		pEPG->m_mapEvent.size() && sValue_To_Assign.size() )
	{
		VDREPG::Event *pEvent=NULL;
		if( sValue_To_Assign[0]=='-' || sValue_To_Assign[0]=='+' )
		{
			pEvent = pEPG->m_mapEvent_Find(pVDRMediaStream->m_EventID);
			if( !pEvent )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist cannot find current event!");
				pEvent = pEPG->m_mapEvent.begin()->second;
			}
			VDREPG::Channel *pChannel = pEvent->m_pChannel;
			if( sValue_To_Assign[0]=='-' )
				pEvent = pEvent->m_pChannel->m_pChannel_Prior->GetCurrentEvent();
			else
				pEvent = pEvent->m_pChannel->m_pChannel_Next->GetCurrentEvent();
		}
		else if( sValue_To_Assign[0]=='E' )
		{
			pEvent = pEPG->m_mapEvent_Find(atoi(sValue_To_Assign.substr(1).c_str()));
			if( !pEvent )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist event invalid!");
				pEvent = pEPG->m_mapEvent.begin()->second;
			}
			pEvent = pEvent->ConfirmCurrentProgram(); // Confirm it's the current one
		}
		else  // Must be a channel
		{
			VDREPG::Channel *pChannel = pEPG->m_mapChannelNumber_Find( atoi(sValue_To_Assign.c_str()) );
			if( !pChannel )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist cannot find channel %s",sValue_To_Assign.c_str());
				pChannel = *pEPG->m_listChannel.begin();
			}
			pEvent = pChannel->GetCurrentEvent();
		}
		if( !pEvent || !pEvent->m_pChannel )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist trying to tune to unknown event %s",sValue_To_Assign.c_str());
		else
		{
			pVDRMediaStream->m_EventID = pEvent->m_EventID;
			DCE::CMD_Tune_to_channel CMD_Tune_to_channel(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				"",StringUtils::itos(pEvent->m_pChannel->m_ChannelID));
			SendCommand(CMD_Tune_to_channel);
		}
		vm.Release();

		m_pMedia_Plugin->MediaInfoChanged(pVDRMediaStream);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Jump_Position_In_Playlist confused");
		*/
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

void VDRPlugin::CMD_Schedule_Recording(string sType,string sOptions,string sProgramID,string *sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c185-e->
{
	string::size_type pos=0;
	string sChannelID = StringUtils::Tokenize(sProgramID,",",pos);
	time_t tStartTime = atoi(StringUtils::Tokenize(sProgramID,",",pos).c_str());
	time_t tStopTime = atoi(StringUtils::Tokenize(sProgramID,",",pos).c_str());

	struct tm tmStart,tmStop;
	localtime_r(&tStartTime,&tmStart);
	localtime_r(&tStopTime,&tmStop);

    if( sChannelID.size() && sChannelID[0]=='i' )
        sChannelID = sChannelID.substr(1);

	VDRChannel *pVDRChannel = m_mapVDRChannel_Find(sChannelID);
	if( !pVDRChannel )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Schedule_Recording invalid channel %s",sProgramID.c_str());
		return;
	}
	if (pVDRChannel->NeedsEPGUpdate(m_timeUpdateInterval))
        UpdateEPGFromVDR(sChannelID, false);

	PLUTO_SAFETY_LOCK(em,m_EPGMutex);
	VDRProgramInstance *pVDRProgramInstance = pVDRChannel->m_pVDRProgramInstance_First;
	while(pVDRProgramInstance && pVDRProgramInstance->m_tStartTime!=tStartTime)
		pVDRProgramInstance = pVDRProgramInstance->m_pVDRProgramInstance_Next;
	if( !pVDRProgramInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Schedule_Recording invalid program %s %d",sProgramID.c_str(),tStartTime);
		return;
	}

    // types:
    // O - record once
    // C - record all on channel
    //
	if( sType=="O" )
	{
		char sDate[20];
		//fixed the formatting of date
		//start stop time, wrong parameters for VDR  
		//todo adding prerecording time of 5 mins and post of 15 mins burgiman 2007-11-07
		sprintf(sDate,"%d-%02d-%02d", tmStart.tm_year+1900, tmStart.tm_mon+1, tmStart.tm_mday);

		string sVDRResponse;
		PLUTO_SAFETY_LOCK(vc, m_ConMutex);
		bool bResult = m_VDRConnection.SendVDRCommand("NEWT 1:" + StringUtils::itos(pVDRChannel->m_dwChanNum) + 
			":"  + 
			sDate + ":" + 
			(tmStart.tm_hour<10 ? "0" : "")+StringUtils::itos(tmStart.tm_hour) + (tmStart.tm_min<10 ? "0" : "") + StringUtils::itos(tmStart.tm_min) + ":" + 
			(tmStop.tm_hour<10 ? "0" : "")+StringUtils::itos(tmStop.tm_hour) + (tmStop.tm_min<10 ? "0" : "") + StringUtils::itos(tmStop.tm_min) + ":" + 
			"50:99:" + pVDRProgramInstance->GetTitle() + ":",sVDRResponse);

		m_VDRConnection.Close();
		int iTimer = atoi(sVDRResponse.c_str());
		if( !bResult || iTimer<1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Schedule_Recording bad response %s %d %s",sProgramID.c_str(),tStartTime,sVDRResponse.c_str());
			return;
		}
		pVDRProgramInstance->m_bRecording = true;
		pVDRProgramInstance->m_iRecordingID = iTimer;
		m_mapVDRRecording[iTimer] = pVDRProgramInstance;
		*sID = StringUtils::itos(iTimer);
	}
	else if( sType=="C" )
	{
		string sCommand;
		if( pVDRProgramInstance->m_pVDREpisode->m_sID!=pVDRProgramInstance->m_pVDREpisode->m_sDescription )
			// U.S. style with a unique episode ID
			sCommand = "NEWS 9:3:12:29=0:" + pVDRProgramInstance->m_pVDREpisode->m_sID;
		else
			// Ues the description
			sCommand = "NEWS 9:3:12:29=0:" + pVDRProgramInstance->GetTitle();
		string sVDRResponse;
		PLUTO_SAFETY_LOCK(vc, m_ConMutex);
		bool bResult = m_VDRConnection.SendVDRCommand(sCommand,sVDRResponse);
		m_VDRConnection.Close();
		int iTimer = atoi(sVDRResponse.c_str());
		if( !bResult || iTimer<1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Schedule_Recording bad recurring response %s %d %s",sProgramID.c_str(),tStartTime,sVDRResponse.c_str());
			return;
		}
		*sID = StringUtils::itos(iTimer);
	}
}

// Shows on current channel
class DataGridTable *VDRPlugin::CurrentShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
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

	VDRChannel *pVDRChannel = m_mapVDRChannel_Find(sChanId);
	if( pVDRChannel==NULL )
		return pDataGridTable;

	time_t tNow = time(NULL);
    struct tm t;
	localtime_r(&tNow,&t);
	int Month = t.tm_mon;
	int Day = t.tm_mday;

	DataGridCell *pCell;
	int iRow=0;

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDR_PlugIn::CurrentShows A datagrid for all the shows on channel %s was requested %s params %s", sChanId.c_str(), GridID.c_str(), Parms.c_str());
  
	// Refresh channel EPG if last time was more than X ago
	if (pVDRChannel->NeedsEPGUpdate(m_timeUpdateInterval))
        UpdateEPGFromVDR(sChanId, false);

	PLUTO_SAFETY_LOCK(em,m_EPGMutex);
	VDRRecording vdrRecording;
	vdrRecording.data.time.channel_id = atoi(sChanId.c_str());

	VDRProgramInstance *pVDRProgramInstance = pVDRChannel->GetCurrentProgramInstance(tNow);
	while(pVDRProgramInstance)
	{
		pCell = new DataGridCell(pVDRProgramInstance->GetTitle());
		pCell->m_mapAttributes["chanid"]=sChanId;
		pCell->m_mapAttributes["programid"]=pVDRProgramInstance->GetProgramId();
		pCell->m_mapAttributes["seriesid"]=pVDRProgramInstance->GetSeriesId();
		pCell->m_mapAttributes["starttime"]=StringUtils::itos(pVDRProgramInstance->m_tStartTime);
		pCell->m_mapAttributes["endtime"]=StringUtils::itos(pVDRProgramInstance->m_tStopTime);

/*
		vdrRecording.data.time.StartTime = tStart;
		if( (it_mapScheduledRecordings=m_mapScheduledRecordings.find(vdrRecording.data.int64))!=m_mapScheduledRecordings.end() )
		{
			szRecording[0] = it_mapScheduledRecordings->second.first;
			pCell->m_mapAttributes["recording"] = szRecording;
			pCell->m_mapAttributes["recordid"] = StringUtils::itos(it_mapScheduledRecordings->second.second);
		}
*/
		struct tm t;
		time_t tStartTime = pVDRProgramInstance->m_tStartTime;
		localtime_r(&tStartTime,&t);
		string sDate;
		if( t.tm_mon==Month && t.tm_mday==Day )
			sDate = "Today";
		else
			sDate = StringUtils::itos(t.tm_mon+1) + "/" + StringUtils::itos(t.tm_mday);

		string sTime = StringUtils::HourMinute(tStartTime) + "-" + StringUtils::HourMinute(pVDRProgramInstance->m_tStopTime);
		/*
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
*/
		MapBookmark *pMapBookmark_Series_Or_Program;
		MapBookmark::iterator it;
		if( (pMapBookmark_Series_Or_Program=m_mapSeriesBookmarks_Find(pVDRProgramInstance->GetSeriesId())) &&
			(
				(it=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
				(it=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
			))
		{
			// It's a user's favorited series
			pCell->m_mapAttributes["PK_Bookmark"] = it->second;
		}
		else if( (pMapBookmark_Series_Or_Program=m_mapProgramBookmarks_Find(pVDRProgramInstance->GetProgramId())) &&
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
		pCell->m_mapAttributes["Synopsis"] = pVDRProgramInstance->GetSynopsis();

		pDataGridTable->SetData(0,iRow++,pCell);
		pVDRProgramInstance = pVDRProgramInstance->m_pVDRProgramInstance_Next;
	}
 
	return pDataGridTable;
}

// Shows on at this time on all channels
class DataGridTable *VDRPlugin::AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	if( m_bBookmarksNeedRefreshing )
		RefreshBookmarks();

	DataGridTable *pDataGridTable = new DataGridTable();

	string::size_type pos=0;
	int iPK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iPK_EntertainArea = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::AllShows A datagrid for all the shows was requested %s params %s", GridID.c_str(), Parms.c_str());
    
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( iPK_EntertainArea );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::AllShows cannot find a stream %p",pEntertainArea);
		return pDataGridTable;
	}

	time_t tNow = time(NULL);

	string sProvider;
	map<int,bool> mapVideoSourcesToUse;
	// The source is 0, means all VDR channels and video sources.  Otherwise, just those for the given device (like a cable box)
	int PK_Device_Source = pEntertainArea->m_pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_LiveTV_CONST ? 0 : pEntertainArea->m_pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	list_int *p_list_int = NULL;
//	if( PK_Device_Source && m_mapDevicesToSources.find(PK_Device_Source)!=m_mapDevicesToSources.end() )
//		p_list_int = &(m_mapDevicesToSources[PK_Device_Source]);

	// Go through all the channels, and set the cell to NULL if we're not supposed to include it
	// or to a new value if we are.  We'll update the description with the show down below
	for(ListVDRChannel::iterator it=m_ListVDRChannel.begin();it!=m_ListVDRChannel.end();++it)
	{
		VDRChannel *pVDRChannel = *it;
		if( p_list_int )
		{
			bool bOk=false;
			for(list_int::iterator it=p_list_int->begin();it!=p_list_int->end();++it)
			{
				if( *it==pVDRChannel->m_pVDRSource->m_dwID )
				{
					bOk=true;
					break;
				}
			}
			if( !bOk )
			{
				pVDRChannel->m_pCell=NULL;
				continue;
			}
		}
		pVDRChannel->m_pCell = GetChannelCell(pVDRChannel);
	}

	int requireEPGUpdate = -1;
	int chanNo = 0;
	int iRow=0;
	for(ListVDRChannel::iterator it=m_ListVDRChannel.begin();it!=m_ListVDRChannel.end();++it)
	{
		VDRChannel *pVDRChannel = *it;
		if( !pVDRChannel || !pVDRChannel->m_pCell )
			continue; // Shouldn't happen that pVDRChannel is NULL, if cell is, we're not including it

//		if( bAllSource==false && mapVideoSourcesToUse[ pVDRChannel->m_pVDRSource->m_dwID ]==false )  // Not a source for this list
//			continue;

		// Refresh channel EPG if last time was more than X ago
		if (pVDRChannel->NeedsEPGUpdate(m_timeUpdateInterval) && requireEPGUpdate == -1)
			requireEPGUpdate = chanNo;

		PLUTO_SAFETY_LOCK(em,m_EPGMutex);
		VDRProgramInstance *pVDRProgramInstance = pVDRChannel->GetCurrentProgramInstance(tNow);

		if( pVDRChannel->m_pPic )
		{
			char *pPic = new char[ pVDRChannel->m_Pic_size ];
			memcpy(pPic,pVDRChannel->m_pPic,pVDRChannel->m_Pic_size);
			pVDRChannel->m_pCell->SetImage( pPic, pVDRChannel->m_Pic_size, GR_JPG );
		}

		string sTime,sNumber,sInfo;
		if( pVDRProgramInstance )
		{
			string sTime = StringUtils::HourMinute(pVDRProgramInstance->m_tStartTime) + " - " + StringUtils::HourMinute(pVDRProgramInstance->m_tStopTime);
			string sNumber = StringUtils::itos(pVDRChannel->m_dwChanNum);
			string sInfo = pVDRProgramInstance->GetSynopsis();
			pVDRChannel->m_pCell->m_mapAttributes["Series"] = pVDRProgramInstance->GetSeriesId();
			pVDRChannel->m_pCell->m_mapAttributes["Program"] = pVDRProgramInstance->GetProgramId();
			pVDRChannel->m_pCell->m_mapAttributes["Info"] = pVDRProgramInstance->GetTitle();
		}

		pVDRChannel->m_pCell->m_mapAttributes["Number"] = sNumber;
		pVDRChannel->m_pCell->m_mapAttributes["Time"] = sTime;
		MapBookmark *pMapBookmark_Series_Or_Program;
		MapBookmark::iterator itB;
		if( (itB=pVDRChannel->m_mapBookmark.find(0))!=pVDRChannel->m_mapBookmark.end() ||
			(itB=pVDRChannel->m_mapBookmark.find(iPK_Users))!=pVDRChannel->m_mapBookmark.end() )
		{
			// It's a user's favorited channel
			pVDRChannel->m_pCell->m_mapAttributes["PK_Bookmark"] = itB->second;
			pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
		}
		else if( pVDRProgramInstance &&
			(pMapBookmark_Series_Or_Program=m_mapSeriesBookmarks_Find(pVDRProgramInstance->GetSeriesId())) &&
			(
				(itB=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
				(itB=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
			))
		{
			// It's a user's favorited series
			pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
		}
		else if( pVDRProgramInstance &&
			(pMapBookmark_Series_Or_Program=m_mapProgramBookmarks_Find(pVDRProgramInstance->GetProgramId())) &&
			(
				(itB=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
				(itB=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
			))
		{
			// It's a user's favorited series.
			pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
		}
		chanNo++;
	}

	for(ListVDRChannel::iterator it=m_ListVDRChannel.begin();it!=m_ListVDRChannel.end();++it)
	{
		VDRChannel *pVDRChannel = *it;
		if( pVDRChannel->m_pCell )
			pDataGridTable->SetData(0,iRow++,pVDRChannel->m_pCell);
	}

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::AllShows cells: %d source %d plist %p map %d",
//		(int) pDataGridTable->m_MemoryDataTable.size(), PK_Device_Source, p_list_int, (int) m_mapDevicesToSources.size() );

	if (requireEPGUpdate >= 0)
		ScheduleEPGUpdate(1, requireEPGUpdate);
	return pDataGridTable;
}

DataGridCell *VDRPlugin::GetChannelCell(VDRChannel *pVDRChannel)
{
	string sChannelName = StringUtils::itos(pVDRChannel->m_dwChanNum) + " " + pVDRChannel->m_sShortName;
	DataGridCell *pCell = new DataGridCell(sChannelName,"i" + pVDRChannel->m_sID);
	pCell->m_mapAttributes["Name"] = sChannelName;
	pCell->m_mapAttributes["Source"] = pVDRChannel->m_pVDRSource->m_sDescription;
	return pCell;
}

class DataGridTable *VDRPlugin::EPGGrid(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGridTable = new DataGridTable();

	string::size_type pos=0;
	int iPK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iPK_EntertainArea = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	string st = StringUtils::Tokenize(Parms,",",pos);
	time_t startTime = time(NULL);
	if (!st.empty()) 
	{
		istringstream ss(st);
		ss >> startTime;
	}
	string et = StringUtils::Tokenize(Parms,",",pos);
	time_t endTime = time(NULL);
	if (!et.empty()) 
	{
		istringstream ss(et);
		ss >> endTime;
	}
	bool bIncludeChannels = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
    string source = StringUtils::Tokenize(Parms,",",pos); // Source if there is no active stream

	PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::EPGrid An EPG datagrid was requested %s params %s", GridID.c_str(), Parms.c_str());
    
/*	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find( iPK_EntertainArea );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream || !pEntertainArea->m_pMediaStream->m_pMediaDevice_Source )
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::EPGGrid cannot find a stream %p",pEntertainArea);
		return pDataGridTable;
	}
*/
	int colNo = 0, rowNo = 0;
	for(ListVDRChannel::iterator it=m_ListVDRChannel.begin();it!=m_ListVDRChannel.end();++it)
	{
		rowNo = 0;
		VDRChannel *pVDRChannel = *it;
		if( !pVDRChannel )
			continue; // Shouldn't happen that pVDRChannel is NULL, we're not including it

        if (source.empty() || (pVDRChannel->m_pVDRSource != NULL &&
                               pVDRChannel->m_pVDRSource->m_sDescription == source)) {
            if (bIncludeChannels)
            {
                pDataGridTable->SetData(colNo, rowNo, GetChannelCell(pVDRChannel));
                rowNo++;
            }

            // Refresh channel EPG if last time was more than X ago
            // This will only update once for each call as the check for channel 2 will return false as it just was updated
            if (pVDRChannel->NeedsEPGUpdate(m_timeUpdateInterval))
                UpdateEPGFromVDR(pVDRChannel->m_sID, true);

	    PLUTO_SAFETY_LOCK(em,m_EPGMutex);
            VDRProgramInstance *pVDRProgramInstance = pVDRChannel->GetCurrentProgramInstance(startTime);

            while ( pVDRProgramInstance && pVDRProgramInstance->m_tStartTime < endTime)
            {
                string sStartTime = StringUtils::itos(pVDRProgramInstance->m_tStartTime);
                string sEndTime = StringUtils::itos(pVDRProgramInstance->m_tStopTime);
                string sNumber = StringUtils::itos(pVDRChannel->m_dwChanNum);
                string sInfo = pVDRProgramInstance->GetSynopsis();
                string id = pVDRProgramInstance->GetTitle();
                DataGridCell* pCell = new DataGridCell(pVDRProgramInstance->GetProgramId(), id);
                pCell->m_mapAttributes["Series"] = pVDRProgramInstance->GetSeriesId();
                pCell->m_mapAttributes["Program"] = pVDRProgramInstance->GetProgramId();
                pCell->m_mapAttributes["Info"] = pVDRProgramInstance->GetTitle();
                pCell->m_mapAttributes["Synopsis"] = sInfo;
                pCell->m_mapAttributes["Number"] = sNumber;
                pCell->m_mapAttributes["StartTime"] = sStartTime;
                pCell->m_mapAttributes["EndTime"] = sEndTime;

                pCell->m_mapAttributes["recording"] = pVDRProgramInstance->m_bRecording ? "O" : "";  // type of recording
                pCell->m_mapAttributes["recordid"] = StringUtils::itos(pVDRProgramInstance->m_iRecordingID);  // recording id

                pDataGridTable->SetData(colNo, rowNo, pCell);
                pVDRProgramInstance = pVDRProgramInstance->m_pVDRProgramInstance_Next;
                rowNo++;
            }

/*		MapBookmark *pMapBookmark_Series_Or_Program;
		MapBookmark::iterator itB;
		if( (itB=pVDRChannel->m_mapBookmark.find(0))!=pVDRChannel->m_mapBookmark.end() ||
			(itB=pVDRChannel->m_mapBookmark.find(iPK_Users))!=pVDRChannel->m_mapBookmark.end() )
		{
			// It's a user's favorited channel
			pVDRChannel->m_pCell->m_mapAttributes["PK_Bookmark"] = itB->second;
			pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
		}
		else if( pVDRProgramInstance &&
			(pMapBookmark_Series_Or_Program=m_mapSeriesBookmarks_Find(pVDRProgramInstance->GetSeriesId())) &&
			(
				(itB=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
				(itB=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
			))
		{
			// It's a user's favorited series
			pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
		}
		else if( pVDRProgramInstance &&
			(pMapBookmark_Series_Or_Program=m_mapProgramBookmarks_Find(pVDRProgramInstance->GetProgramId())) &&
			(
				(itB=pMapBookmark_Series_Or_Program->find(0))!=pMapBookmark_Series_Or_Program->end() ||
				(itB=pMapBookmark_Series_Or_Program->find(iPK_Users))!=pMapBookmark_Series_Or_Program->end()
			))
		{
			// It's a user's favorited series.
			pDataGridTable->SetData(0,iRow++,new DataGridCell(pVDRChannel->m_pCell)); // A copy since we'll add this one later too
			}*/
            colNo++;
        }
	}

	return pDataGridTable;
}

class DataGridTable *VDRPlugin::OtherShowtimes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	/*
	if( Parms.size()==0 )
		return pDataGrid;  // Shouldn't happen

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( !GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CurrentShows No EPG");
		return pDataGrid;
	}

	int iRow=0;
	VDREPG::Event *pEvent = pEPG->m_mapEvent_Find(atoi(Parms.substr(1).c_str()));
	pEvent->m_pProgram->m_listEvent.sort(EventsByShowtime);
	for(list<VDREPG::Event *>::iterator it=pEvent->m_pProgram->m_listEvent.begin();it!=pEvent->m_pProgram->m_listEvent.end();++it)
	{
		VDREPG::Event *pEvent_Other = *it;
		if( pEvent_Other->AlreadyOver() )
			continue;

		string sDesc = pEvent_Other->m_pChannel->m_sChannelName + " / " + pEvent_Other->GetShortShowtime();
		if( pEvent_Other->m_sDescription_Short.size() )
			sDesc += "\n" + pEvent_Other->m_sDescription_Short;
		else if( pEvent_Other->m_sDescription_Long.size() )
			sDesc += "\n" + pEvent_Other->m_sDescription_Long;

		DataGridCell *pDataGridCell = new DataGridCell(sDesc, "E" + StringUtils::itos(pEvent_Other->m_EventID));
		pDataGrid->SetData(0,iRow++,pDataGridCell);
	}
*/
	return pDataGrid;
}

class DataGridTable *VDRPlugin::FavoriteChannels(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	/*
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::FavoriteChannels Called to populate: %s", Parms.c_str());

	int PK_Users = atoi(Parms.c_str());
	vector<Row_Bookmark *> vectRow_Bookmark;
	Database_pluto_media *pDatabase_pluto_media = m_pMedia_Plugin->GetMediaDatabaseConnect();
	pDatabase_pluto_media->Bookmark_get()->GetRows("EK_MediaType=1 AND Position LIKE ' CHAN:%' AND (EK_Users IS NULL OR EK_Users=" + StringUtils::itos(PK_Users) + ")",&vectRow_Bookmark);

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];
		string sDescription = pRow_Bookmark->Description_get().size() ? pRow_Bookmark->Description_get() : pRow_Bookmark->Position_get();
		DataGridCell *pDataGridCell = new DataGridCell(sDescription, "B" + StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		pDataGrid->SetData(0,s,pDataGridCell);
	}
*/
	return pDataGrid;
}

class DataGridTable *VDRPlugin::FavoriteShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
/*
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRPlugin::FavoriteShows Called to populate: %s", Parms.c_str());

	int PK_Users = atoi(Parms.c_str());
	vector<Row_Bookmark *> vectRow_Bookmark;
	Database_pluto_media *pDatabase_pluto_media = m_pMedia_Plugin->GetMediaDatabaseConnect();
	pDatabase_pluto_media->Bookmark_get()->GetRows("EK_MediaType=1 AND Position LIKE ' PROG:%' AND (EK_Users IS NULL OR EK_Users=" + StringUtils::itos(PK_Users) + ")",&vectRow_Bookmark);

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];
		string sDescription = pRow_Bookmark->Description_get().size() ? pRow_Bookmark->Description_get() : pRow_Bookmark->Position_get();
		DataGridCell *pDataGridCell = new DataGridCell(sDescription, "B" + StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		pDataGrid->SetData(0,s,pDataGridCell);
	}
*/
	return pDataGrid;
}
	/*

bool VDRPlugin::GetVdrAndEpgFromOrbiter(int PK_Device,MediaDevice *&pMediaDevice_VDR,VDREPG::EPG *&pEPG,VDRMediaStream *&pVDRMediaStream)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 1");
#ifdef DEBUG
	if( m_VDRMutex.m_NumLocks==0 || m_pMedia_Plugin->m_MediaMutex.m_NumLocks==0 )
	{
		// EPG is volatile and the mutex must be blocked whenever we get a pointer to it, also media plugin to protect teh stream
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter you're not holding the vdr mutex");
		return false;
	}
#endif

	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(PK_Device);
	if( pOH_Orbiter && pOH_Orbiter->m_pEntertainArea )
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 2");
		ListMediaDevice *pListMediaDevice = pOH_Orbiter->m_pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_VDR_CONST);
		if( pListMediaDevice && pListMediaDevice->size() )
			pMediaDevice_VDR = *pListMediaDevice->begin();
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter No VDR device");
			return false;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter not ent area");
		return false;
	}

	if( pMediaDevice_VDR->m_mapEntertainArea.size()==1 )
	{
		EntertainArea *pEntertainArea = pMediaDevice_VDR->m_mapEntertainArea.begin()->second;
		if( pEntertainArea->m_pMediaStream->GetType()!=MEDIASTREAM_TYPE_VDR )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter media stream isn't for VDR");
			return false;
		}
		pVDRMediaStream = (VDRMediaStream *) pEntertainArea->m_pMediaStream;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter VDR isn't in a single entertainment area");
		return false;
	}

LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 3");
	pEPG = m_mapEPG_Find(pMediaDevice_VDR->m_pDeviceData_Router->m_dwPK_Device);
	if( !pEPG )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::GetVdrAndEpgFromOrbiter No EPG");
		return false;
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetVdrAndEpgFromOrbiter 4");
	return true;
}
*/	
//<-dceag-c698-b->

	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

void VDRPlugin::CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c698-e->
{
	/*
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	PLUTO_SAFETY_LOCK(vm,m_VDRMutex);
	MediaDevice *pMediaDevice; VDREPG::EPG *pEPG; VDRMediaStream *pVDRMediaStream;
	if( !GetVdrAndEpgFromOrbiter(pMessage->m_dwPK_Device_From,pMediaDevice,pEPG,pVDRMediaStream) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Get_Extended_Media_Data No EPG");
		return;
	}

	VDREPG::Event *pEvent = NULL;
	if( sProgramID.size()==0 )
		pEvent = pEPG->m_mapEvent_Find(pVDRMediaStream->m_EventID);
	else
		pEvent = pEPG->m_mapEvent_Find(atoi(sProgramID.substr(1).c_str()));
	if( !pEvent || !pEvent->m_pChannel )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::CMD_Get_Extended_Media_Data trying to tune to unknown event %s",sProgramID.c_str());
	else
	{
		struct tm *tmptr = localtime(&pEvent->m_tStartTime);
		struct tm tm_start = *tmptr;
		tmptr = localtime(&pEvent->m_tStopTime);
		DCE::CMD_Set_Variable CMD_Set_Variable1(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_1_CONST,pEvent->m_pChannel->m_sChannelName + " / " +
			StringUtils::itos(tm_start.tm_hour) + ":" + (tm_start.tm_min<10 ? "0" : "") + StringUtils::itos(tm_start.tm_min) + " - " +
			StringUtils::itos(tmptr->tm_hour) + ":" + (tmptr->tm_min<10 ? "0" : "") + StringUtils::itos(tmptr->tm_min));
		DCE::CMD_Set_Variable CMD_Set_Variable2(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_2_CONST,pEvent->m_pProgram->m_sTitle);
		DCE::CMD_Set_Variable CMD_Set_Variable3(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_3_CONST,pEvent->m_sDescription_Short);
		DCE::CMD_Set_Variable CMD_Set_Variable4(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Misc_Data_4_CONST,pEvent->m_sDescription_Long);
		DCE::CMD_Set_Variable CMD_Set_VariableEvent(m_dwPK_Device,pMessage->m_dwPK_Device_From,
			VARIABLE_Datagrid_Input_CONST,"E" + StringUtils::itos(pEvent->m_EventID));
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable2.m_pMessage );
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable3.m_pMessage );
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable4.m_pMessage );
		CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_VariableEvent.m_pMessage );
		if( sPK_DesignObj.size() )
		{
			DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pMessage->m_dwPK_Device_From,
				0,sPK_DesignObj,"","",false,false);
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Goto_DesignObj.m_pMessage );
		}
		else
		{
			DCE::CMD_Refresh CMD_Refresh(m_dwPK_Device,pMessage->m_dwPK_Device_From,"");
			CMD_Set_Variable1.m_pMessage->m_vectExtraMessages.push_back( CMD_Refresh.m_pMessage );
		}
		SendCommand(CMD_Set_Variable1);
	}
	*/
}

	/*
VDREPG::Event *VDRPlugin::GetStartingEvent(VDREPG::EPG *pEPG,int PK_Users)
{
	VDREPG::Event *pEvent = NULL;  // This will be the one with the highest priority
	int iPriorityLevel = 5;  // The priority of that event.  1=best match (current show) private, 2=current show public, 3=channel private, 4=channel public, 5=no match
	if( !pEPG )
		return pEvent; // Can't do anything without EPG

	vector<Row_Bookmark *> vectRow_Bookmark;
	Database_pluto_media *pDatabase_pluto_media = m_pMedia_Plugin->GetMediaDatabaseConnect();
	pDatabase_pluto_media->Bookmark_get()->GetRows("EK_MediaType=1 AND (EK_Users IS NULL OR EK_Users=" + StringUtils::itos(PK_Users)+")",&vectRow_Bookmark);
	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		int iPriority_Bookmark;
		VDREPG::Event *pEvent_Bookmark = GetEventForBookmark(pEPG,vectRow_Bookmark[s],iPriority_Bookmark);
		if( pEvent_Bookmark && iPriorityLevel>iPriority_Bookmark )
		{
			iPriorityLevel=iPriority_Bookmark;
			pEvent = pEvent_Bookmark;
		}
	}

	if( !pEvent )  // Just find the first show
	{
		time_t tNow = time(NULL);
		for(size_t s=0;s<pEPG->m_vectChannel.size();++s)
		{
			pEvent = pEPG->m_vectChannel[s]->GetCurrentEvent();
			if( pEvent && pEvent->m_tStartTime<tNow && pEvent->m_tStopTime>tNow)
				return pEvent;
		}
	}
	return pEvent;
	return NULL;
}
	*/

	/*
VDREPG::Event *VDRPlugin::GetEventForBookmark(VDREPG::EPG *pEPG,Row_Bookmark *pRow_Bookmark,int &iPriority_Bookmark)
{
	time_t tNow = time(NULL);
	string::size_type pos;
	bool bIsShow = (pos=pRow_Bookmark->Position_get().find(" PROG:"))!=string::npos && pRow_Bookmark->Position_get().size()-pos>6;
	if( bIsShow )
	{
		VDREPG::Program *pProgram = pEPG->m_mapProgram_Find(pRow_Bookmark->Position_get().substr(pos+6));
		if( !pProgram )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetEventForBookmark Cannot find any programs of %s",pRow_Bookmark->Position_get().c_str());
			return NULL;
		}

		VDREPG::Event *pEvent = pProgram->GetNextEvent();
		if( !pEvent )
			return NULL; // Should never happen

		if( pEvent->m_tStartTime > tNow + 600 )
			return NULL; // Ignore this it's not starting soon

		if( pRow_Bookmark->EK_Users_get() )
			iPriority_Bookmark = 1;
		else
			iPriority_Bookmark = 2;
		return pEvent;
	}

	VDREPG::Channel *pChannel;
	VDREPG::Event *pEvent;
	pos=pRow_Bookmark->Position_get().find(" CHAN:");
	if( pos==string::npos ||  pRow_Bookmark->Position_get().size()-pos<=6 || 
		(pChannel=pEPG->m_mapChannelName_Find(pRow_Bookmark->Position_get().substr(pos+6)))==NULL ||
		(pEvent=pChannel->GetCurrentEvent())==NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::GetEventForBookmark Cannot find any channels of %s",pRow_Bookmark->Position_get().c_str());
		return NULL;
	}

	if( pRow_Bookmark->EK_Users_get() )
		iPriority_Bookmark = 3;
	else
		iPriority_Bookmark = 4;
	return pEvent;
}
	*/

//<-dceag-c764-b->

	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

void VDRPlugin::CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c764-e->
{
}
//<-dceag-c824-b->

	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */
		/** @param #2 PK_Device */
			/** If specified, this is the capture card that triggered this change to call checktvproviders for */
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

void VDRPlugin::CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage)
//<-dceag-c824-e->
{
}

void VDRPlugin::PurgeChannelList()
{
	for(map<string,VDRChannel *>::iterator it=m_mapVDRChannel.begin();it!=m_mapVDRChannel.end();++it)
		delete it->second;
	m_mapVDRChannel.clear();
	m_ListVDRChannel.clear();

	for(map<string,VDRSource *>::iterator it=m_mapVDRSource.begin();it!=m_mapVDRSource.end();++it)
		delete it->second;
	m_mapVDRSource.clear();
}

VDRSource *VDRPlugin::GetNewSource(string sSource)
{
	VDRSource *pVDRSource = m_mapVDRSource_Find(sSource);
	if( pVDRSource==NULL )
	{
		pVDRSource = new VDRSource(1,sSource);
		m_mapVDRSource[sSource]=pVDRSource;
	}
	return pVDRSource;
}

VDRSeries *VDRPlugin::GetNewSeries(string sID)
{
	VDRSeries *pVDRSeries = m_mapVDRSeries_Find(sID);
	if( pVDRSeries==NULL )
	{
		pVDRSeries = new VDRSeries(sID);
		m_mapVDRSeries[sID]=pVDRSeries;
	}
	return pVDRSeries;
}

void VDRPlugin::BuildChannelList()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	PurgeChannelList();

	string sVDRResponse;
	{
		PLUTO_SAFETY_LOCK(vc, m_ConMutex);
		bool result = m_VDRConnection.SendVDRCommand("LSTC",sVDRResponse);
		m_VDRConnection.Close();
		if( !result )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::BuildChannelList cannot get channel list");
			return;
		}
 	}
	LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA,"VDRPlugin::BuildChannelList received channel list");
	        
	string::size_type pos_line=0;
	string sLine;
	int i = 0;
	while( true )
	{
		sLine = StringUtils::Tokenize(sVDRResponse,"\n",pos_line);
		if( sLine.empty()==true )
			break;

		string::size_type pos_delimit = 0;
		string sChannel = StringUtils::Tokenize(sLine,":",pos_delimit);
		StringUtils::Replace(sChannel,"|",":");
		string sC1 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC2 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sSource = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC4 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC5 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC6 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC7 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC8 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sSID = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sNID = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sTID = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC12 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC13 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC14 = StringUtils::Tokenize(sLine,":",pos_delimit);
		string sC15 = StringUtils::Tokenize(sLine,":",pos_delimit);

		string::size_type pos_semicolon = sChannel.find(';',2);
		string sChannelNameLong,sChannelNameShort;
		if( pos_semicolon!=string::npos )
		{
			sChannelNameShort = sChannel.substr(2,pos_semicolon-2);
			sChannelNameLong = sChannel.substr(pos_semicolon+1);
		}
		else
			sChannelNameShort = sChannelNameLong = sChannel.substr(2);

		VDRSource *pVDRSource = GetNewSource(sSource);
		int iChannel = atoi(sChannel.c_str());
		string sChannelID = sSource + "-" + sNID + "-" + sTID + "-" + sSID;
		if (StringUtils::TrimSpaces(sC12) != "0")
			sChannelID += "-" + sC12;
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDRPlugin::BuildChannelList, adding channel = %s", sChannelID.c_str());
		VDRChannel *pVDRChannel = new VDRChannel(sChannelID,iChannel,pVDRSource,sChannelNameShort,sChannelNameLong,NULL,0);
		m_mapVDRChannel[sChannelID]=pVDRChannel;
		m_ListVDRChannel.push_back(pVDRChannel);

		i++;
	}
	ScheduleEPGUpdate(30, 0);
}

/**
 * If non-empty the parameters will be used in the fetching of data
 */
void VDRPlugin::UpdateEPGFromVDR(string channelId, bool file)
{
	string sVDRResponse="";
	bool bCacheFailed=false;
	
    if (file) {
        if (!FileUtils::FileExists("/var/cache/var/epg.data"))
        {
            bCacheFailed=true;
            LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDRPlugin::UpdateEPGFromVDR(%s,%b): file /var/cache/var/epg.data does not exist. Attempting a fetch from VDR.",channelId.c_str(),file);
        }
        else if (FileUtils::FileSize("/var/cache/var/epg.data")<1)
        {
            bCacheFailed=true;
            LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDRPlugin::UpdateEPGFromVDR(%s,%b): file /var/cache/var/epg.data has a zero length. Attempting a fetch from VDR.",channelId.c_str(),file);
        }
        
        if (!bCacheFailed)
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDRPlugin::UpdateEPGFromVDRD(%s,%b): cache file /var/cache/var/epg.data exists. Using it.",channelId.c_str(),file);
            FileUtils::ReadTextFile("/var/cache/vdr/epg.data", sVDRResponse);
        }
        else
        {
            // it failed above, let's ask VDR for the channel data.
            UpdateEPGFromVDR(channelId,false);
        }
    } else {
        string cmd = "LSTE ";
        if (!channelId.empty())
            cmd += channelId + " ";

        PLUTO_SAFETY_LOCK(vc, m_ConMutex);
        bool result = m_VDRConnection.SendVDRCommand(cmd,sVDRResponse);
        m_VDRConnection.Close();
        if( !result )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::UpdateEPGFromVDR cannot get epg, cmd = %s", cmd.c_str());
            return;
        }
        LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA,"VDRPlugin::UpdateEPGFromVDR received epg");
    }

	clock_t startClock = clock();
	VDRChannel *pVDRChannel = NULL;
	VDRProgramInstance *pVDRProgramInstance = NULL, *pVDRProgramInstance_Last = NULL;
	string sSeriesDescription="",sSeriesID="",sEpisodeDescription="",sEpisodeID="",sDescription="";
	string::size_type pos=0;
	PLUTO_SAFETY_LOCK(em,m_EPGMutex);
	while( true )
	{
		string sLine = StringUtils::Tokenize(sVDRResponse,"\n",pos);
		if( sLine.empty()==true )
			break;

		if( sLine[0]=='C' )
		{
			pVDRProgramInstance=pVDRProgramInstance_Last=NULL;
			string::size_type pos_space = sLine.find(' ',2);
			if( pos_space!=string::npos )
			{
				string sChannelID = sLine.substr(2,pos_space-2);
				pVDRChannel = m_mapVDRChannel_Find(sChannelID);
				if( !pVDRChannel )
				{	
				        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDRPlugin::UpdateEPGFromVDR cannot find channel %s (%s)",sChannelID.c_str(), sLine.c_str());
				}
                                else
                                {
                                        LoggerWrapper::GetInstance()->Write(LV_DEBUG,"VDRPlugin::UpdateEPGFromVDR found channel %s",sLine.c_str());
					pVDRChannel->m_timeEPGUpdated = time(NULL);
                                }

			}
		}

		if( pVDRChannel )
		{
			if( sLine[0]=='E' )
			{
				pVDRProgramInstance = new VDRProgramInstance();
				if( pVDRChannel->m_pVDRProgramInstance_First==NULL )
					pVDRChannel->m_pVDRProgramInstance_First = pVDRProgramInstance;
				else if( pVDRProgramInstance_Last )
					pVDRProgramInstance_Last->m_pVDRProgramInstance_Next = pVDRProgramInstance;

				pVDRProgramInstance_Last = pVDRProgramInstance;
				
				string::size_type pos=2;
				string sID = StringUtils::Tokenize(sLine," ",pos);
				string sStartTime = StringUtils::Tokenize(sLine," ",pos);
				string sDuration = StringUtils::Tokenize(sLine," ",pos);
				string s4 = StringUtils::Tokenize(sLine," ",pos);
				string s5 = StringUtils::Tokenize(sLine," ",pos);

				pVDRProgramInstance->m_iID = atoi(sID.c_str());
				pVDRProgramInstance->m_tStartTime = atoi(sStartTime.c_str());
				pVDRProgramInstance->m_tStopTime = pVDRProgramInstance->m_tStartTime + atoi(sDuration.c_str());
			
			}
			else if( pVDRProgramInstance )
			{
				if( sLine[0]=='T' )
					sSeriesDescription = sLine.substr(2);
				else if( sLine[0]=='S' )
					sEpisodeDescription = sLine.substr(2);
				else if( sLine[0]=='D' )
					sDescription = sLine.substr(2);
				else if( sLine[0]=='e' )
				{
					string::size_type pos_extraInfo = sDescription.find("|||");
					if( pos_extraInfo!=string::npos )
					{
						string::size_type pos_EpisodeID = sDescription.find("EpisodeID:");
						if( pos_EpisodeID!=string::npos )
						{
							pos_EpisodeID+=11;
							string::size_type pos_Termination = sDescription.find('|',pos_EpisodeID);
							string sEpisode = pos_Termination==string::npos ? sDescription.substr(pos_EpisodeID) : sDescription.substr(pos_EpisodeID,pos_Termination-pos_EpisodeID);
							string::size_type pos_Dot = sEpisode.find('.');
							if( pos_Dot!=string::npos )
							{
								sSeriesID = sEpisode.substr(0,pos_Dot);
								sEpisodeID = sEpisode.substr(pos_Dot+1);
							}
							else
								sSeriesID = sEpisode;
						}
					}

					if( sSeriesID.empty() )
						sSeriesID = sSeriesDescription;
					if( sEpisodeID.empty() )
						sEpisodeID = sEpisodeDescription;

					VDRSeries *pVDRSeries = GetNewSeries(sSeriesID);
					pVDRSeries->m_sDescription = sSeriesDescription;
					VDREpisode *pVDREpisode = pVDRSeries->GetNewEpisode(sEpisodeID);
					pVDREpisode->m_sDescription = sEpisodeDescription;
					pVDREpisode->m_sSynopsis = pos_extraInfo==string::npos ? sDescription : sDescription.substr(0,pos_extraInfo);
					pVDRProgramInstance->m_pVDREpisode = pVDREpisode;

					sSeriesDescription="";
					sSeriesID="";
					sEpisodeDescription="";
					sEpisodeID="";
					sDescription="";
				}
			}
		}
	}	
int k=2;	
	/*
	
	VDRSource *pVDRSource = m_mapVDRSource_Find( atoi(row[6]) );
			if( !pVDRSource )
			{
				string sDescription = row[6];

				sSQL = "select name from videosource where sourceid=" + string(row[6]);
				PlutoSqlResult result2;
				DB_ROW row2;
				if( (result2.r=m_pDBHelper_VDR->db_wrapper_query_result(sSQL))!=NULL && (row2 = db_wrapper_fetch_row(result2.r))!=NULL )
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
				pVDRSource = new VDRSource(atoi(row[6]),sDescription);
				m_mapVDRSource[ pVDRSource->m_dwID ] =pVDRSource;
			}

			string sFilename;
			if( row[5] )
				sFilename = "/home/mediapics/" + string(row[5]) + "_tn.jpg";
			else if( row[4] )
				sFilename = row[4];

			size_t size=0;
			char *pData = sFilename.empty() ? NULL : FileUtils::ReadFileIntoBuffer(sFilename,size);
			VDRChannel *pVDRChannel = new VDRChannel( atoi(row[0]), atoi(row[1]), pVDRSource, row[2] ? row[2] : "", row[3] ? row[3] : "", pData, size );
			m_mapVDRChannel[pVDRChannel->m_dwID] = pVDRChannel;
			m_ListVDRChannel.push_back(pVDRChannel);
		}
	}
	m_ListVDRChannel.sort(ChannelComparer);
*/
/*
	VDRSource *pVDRSource = new VDRSource(1,"source 1");
	time_t tNow = time(NULL);
	size_t size;
	char *pPic = FileUtils::ReadFileIntoBuffer("C:\\temp\\lmce.jpg",size);
	for(int i=0;i<100;++i)
	{
		VDRChannel *pVDRChannel = new VDRChannel(i,i,pVDRSource,"ch" + StringUtils::itos(i),"channel " + StringUtils::itos(i),pPic,size);
		m_ListVDRChannel.push_back(pVDRChannel);

		VDRProgramInstance *pVDRProgramInstance_Prior = NULL;
		for(int t=0;t<10;++t)
		{
			VDRProgramInstance *pVDRProgramInstance = new VDRProgramInstance();
			pVDRProgramInstance->m_tStartTime = tNow + (t*30*60);
			pVDRProgramInstance->m_tStopTime = tNow + ((t+1)*30*60);
			if( pVDRProgramInstance_Prior )
				pVDRProgramInstance_Prior->m_pVDRProgramInstance_Next = pVDRProgramInstance;
			else 
				pVDRChannel->m_pVDRProgramInstance_First=pVDRProgramInstance;
			pVDRProgramInstance_Prior=pVDRProgramInstance;
		}
	}
*/
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateEPGFromVDR end, time taken = %f (excluding ReceiveData time)", ((float)(clock() - startClock))/CLOCKS_PER_SEC);

}

bool VDRPlugin::TuneToChannel( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( pMessage->m_dwPK_Device_To!=m_pMedia_Plugin->m_dwPK_Device &&  // Let media plugin convert this to the destination device and add the stream id
		pMessage->m_mapParameters.find(COMMANDPARAMETER_ProgramID_CONST)!=pMessage->m_mapParameters.end() )
	{
		MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str()),0);

		string sProgramID = pMessage->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST];
		if( sProgramID.size()>1 && sProgramID[0]=='i' )
		{
			VDRChannel *pVDRChannel = m_mapVDRChannel_Find( sProgramID.substr(1) );
			if( pVDRChannel )
				pMessage->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST] = StringUtils::itos(pVDRChannel->m_dwChanNum);
		}
		////
		string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
		string sSection = pMessage->m_mapParameters[EVENTPARAMETER_SectionDescription_CONST];
		string sAudio = pMessage->m_mapParameters[EVENTPARAMETER_Audio_CONST];
		string sVideo = pMessage->m_mapParameters[EVENTPARAMETER_Video_CONST];		
		VDRMediaStream *pVDRMediaStream = (VDRMediaStream *) pMediaStream;
    if ( pVDRMediaStream == NULL  )
    {
        return false;
    }
		pVDRMediaStream->m_sMediaDescription = sMRL;
		pVDRMediaStream->m_sSectionDescription = sSection;
		pVDRMediaStream->m_sMediaSynopsis = sAudio + "/" + sVideo;

		m_pMedia_Plugin->MediaInfoChanged(pVDRMediaStream,true);		
	
		////
	}
	return false;
}

void VDRPlugin::RefreshBookmarks()
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
	m_bBookmarksNeedRefreshing=false;

	for(map<string,MapBookmark *>::iterator it=m_mapSeriesBookmarks.begin();it!=m_mapSeriesBookmarks.end();++it)
		delete it->second;
	m_mapSeriesBookmarks.clear();
	for(map<string,MapBookmark *>::iterator it=m_mapProgramBookmarks.begin();it!=m_mapProgramBookmarks.end();++it)
		delete it->second;
	m_mapProgramBookmarks.clear();
	for(map<string,VDRChannel *>::iterator it=m_mapVDRChannel.begin();it!=m_mapVDRChannel.end();++it)
		it->second->m_mapBookmark.clear();

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
				const char *ChanId = ( (const char *) (row[2][6]=='i' ? &row[2][7] : &row[2][6]) );  // Skip any leading 'i'
				VDRChannel *pVDRChannel = m_mapVDRChannel_Find(ChanId);
				if( pVDRChannel )
					pVDRChannel->m_mapBookmark[ row[1] ? atoi(row[1]) : 0 ] = atoi(row[0]);
			}
		}
	}
}

bool VDRPlugin::NewBookmarks( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	m_bBookmarksNeedRefreshing=true;
	return false;
}
//<-dceag-c846-b->

	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

void VDRPlugin::CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c846-e->
{
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

void VDRPlugin::CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage)
//<-dceag-c910-e->
{
}

//<-dceag-c911-b->

	/** @brief COMMAND: #911 - Remove Scheduled Recording */
	/** Remove a scheduled recording */
		/** @param #10 ID */
			/** The ID of the recording rule to remove.  This will remove all recordings with this ID, and ProgramID is ignored if this is specified. */
		/** @param #68 ProgramID */
			/** The ID of the program to remove.  If ID is empty, remove just this program. */

void VDRPlugin::CMD_Remove_Scheduled_Recording(string sID,string sProgramID,string &sCMD_Result,Message *pMessage)
//<-dceag-c911-e->
{
    string sVDRResponse;
    PLUTO_SAFETY_LOCK(vc, m_ConMutex);
    if( !m_VDRConnection.SendVDRCommand("DELT " + sID,sVDRResponse) )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "VDRTV_PlugIn::CMD_Remove_Scheduled_Recording(): Failed to remove recording with id %s, message from VDR: %s", sID.c_str(), sVDRResponse.c_str());
    } else {
        // remove recording marker on program instance
        int iTimerID = atoi(sID.c_str());
        VDRProgramInstance* pInstance = m_mapVDRRecording_Find(iTimerID);
        if (pInstance)
        {
            pInstance->m_bRecording = false;
            pInstance->m_iRecordingID = 0;
            m_mapVDRRecording.erase(iTimerID);
        }
    }
    m_VDRConnection.Close();
}

void VDRPlugin::UpdateTimers()
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	string sVDRResponse;
	PLUTO_SAFETY_LOCK(vc, m_ConMutex);
	bool result = m_VDRConnection.SendVDRCommand("LSTT",sVDRResponse);
	m_VDRConnection.Close();

	if( result )
	{
		string::size_type pos=0;
		while(true)
		{
			string sLine = StringUtils::Tokenize(sVDRResponse,"\n",pos);
			if( sLine.empty() )
				break;
			string::size_type pos_colon=0;
            //  1 9:2:2015-09-13:1939:2009:50:99:Landeplage: Lys og varme:
            int id = atoi(StringUtils::Tokenize(sLine," ",pos_colon).c_str());
            string s1 = StringUtils::Tokenize(sLine,":",pos_colon);
            int channelNum = atoi(StringUtils::Tokenize(sLine,":",pos_colon).c_str());
            string date = StringUtils::Tokenize(sLine,":",pos_colon);
            string stime = StringUtils::Tokenize(sLine,":",pos_colon);
            string etime = StringUtils::Tokenize(sLine,":",pos_colon);
            string s3 = StringUtils::Tokenize(sLine,":",pos_colon);
            string s4 = StringUtils::Tokenize(sLine,":",pos_colon);
            string title = StringUtils::Tokenize(sLine,"",pos_colon);

            // Find actual program that is being recorded and mark it
            // TODO: support timed recording? (no program, just start and stop time?)
            size_t c = 0;
            VDRChannel *pVDRChannel = NULL;
            ListVDRChannel::iterator it=m_ListVDRChannel.begin();
            while (pVDRChannel == NULL && it!=m_ListVDRChannel.end())
            {
                if ((*it)->m_dwChanNum == channelNum)
                    pVDRChannel = *it;
                it++;
            }
            if (pVDRChannel)
            {
                string::size_type pos_t = 0;
                int y = atoi(StringUtils::Tokenize(date,"-",pos_t).c_str());
                int m = atoi(StringUtils::Tokenize(date,"-",pos_t).c_str());
                int d = atoi(StringUtils::Tokenize(date,"-",pos_t).c_str());
                int h = atoi(stime.substr(0, 2).c_str());
                int min = atoi(stime.substr(2).c_str());
                time_t tStartTime = StringUtils::MakeTime(y, m, d, h, min);

		PLUTO_SAFETY_LOCK(em,m_EPGMutex);
                VDRProgramInstance *pVDRProgramInstance = pVDRChannel->m_pVDRProgramInstance_First;
                while(pVDRProgramInstance && pVDRProgramInstance->m_tStartTime!=tStartTime)
                    pVDRProgramInstance = pVDRProgramInstance->m_pVDRProgramInstance_Next;
                if( !pVDRProgramInstance )
                {
                    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDRPlugin::UpdateTimers() invalid program - unable to map timer to program");
                    break;
                } else {
                    pVDRProgramInstance->m_bRecording = true;
                    pVDRProgramInstance->m_iRecordingID = id;
                }

            } else {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "VDRTV_PlugIn::UpdateTimers() no such channel - unable to map timer to channel+program");
                break;
            }

		}
	}
}


bool VDRPlugin::PlaybackStarted( class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	PLUTO_SAFETY_LOCK(mm,m_pMedia_Plugin->m_MediaMutex);
  int iStreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
	string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
	string sSection = pMessage->m_mapParameters[EVENTPARAMETER_SectionDescription_CONST];
	string sAudio = pMessage->m_mapParameters[EVENTPARAMETER_Audio_CONST];
	string sVideo = pMessage->m_mapParameters[EVENTPARAMETER_Video_CONST];

  MediaStream * pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( iStreamID, pMessage->m_dwPK_Device_From );
	VDRMediaStream *pVDRMediaStream = (VDRMediaStream *) pMediaStream;

    if ( pVDRMediaStream == NULL  )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "VDRPlugin::PlaybackStarted Stream ID %d is not mapped to a media stream object", iStreamID);
        return false;
    }

	pVDRMediaStream->m_sMediaDescription = sMRL;
	pVDRMediaStream->m_sSectionDescription = sSection;
	pVDRMediaStream->m_sMediaSynopsis = sAudio + "/" + sVideo;

	m_pMedia_Plugin->MediaInfoChanged(pVDRMediaStream,true);

	return false;

}
//<-dceag-c986-b->

	/** @brief COMMAND: #986 - Sync Storage Groups */
	/** Synchronize Storage Groups. Sent in response to a storage device being added. */

void VDRPlugin::CMD_Sync_Storage_Groups(string &sCMD_Result,Message *pMessage)
//<-dceag-c986-e->
{

}

void VDRPlugin::ScheduleEPGUpdate(int delay, int chanNo)
{
	PLUTO_SAFETY_LOCK(em,m_EPGMutex);
	m_iEPGUpdateNo = chanNo;
	m_pAlarmManager->AddRelativeAlarm(delay,this,ALARM_UPDATE_EPG, NULL);
}

void VDRPlugin::AlarmCallback(int id, void* param)
{
	if( id == ALARM_UPDATE_EPG )
	{
		PLUTO_SAFETY_LOCK(em,m_EPGMutex);
		if(!m_ListVDRChannel.empty())
		{
			VDRChannel *pVDRChannel = m_ListVDRChannel[m_iEPGUpdateNo];
			em.Release();
			if (pVDRChannel)
			{
				if (pVDRChannel->NeedsEPGUpdate(m_timeUpdateInterval)) // Might have been updated elsewhere
				{
                    UpdateEPGFromVDR("", true);

				}
			}
			PLUTO_SAFETY_LOCK(em,m_EPGMutex);
            // Comment code for now, will only ever do a full update by reading epg.data file
            // when there is a scheduled update
/*			m_iEPGUpdateNo++;
			if (m_iEPGUpdateNo >= m_ListVDRChannel.size())
			{
				// Start over after the update interval has passed
                m_iEPGUpdateNo = 0;*/
				m_pAlarmManager->AddRelativeAlarm(m_timeUpdateInterval,this,ALARM_UPDATE_EPG, NULL);
/*			} else {
				m_pAlarmManager->AddRelativeAlarm(4,this,ALARM_UPDATE_EPG, NULL); // 2 second between to allow some time off
            }*/
		}
	}
}
