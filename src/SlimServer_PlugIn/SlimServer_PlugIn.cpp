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
#include "SlimServer_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
SlimServer_PlugIn::SlimServer_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: SlimServer_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-getconfig-b->
bool SlimServer_PlugIn::GetConfig()
{
	if( !SlimServer_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
SlimServer_PlugIn::SlimServer_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: SlimServer_PlugIn_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
SlimServer_PlugIn::~SlimServer_PlugIn()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SlimServer_PlugIn::Register()
//<-dceag-reg-e->
{
	m_iPriority=DATA_Get_Priority();

	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"SlimServer Plugin is at %i priority for playing media.",m_iPriority);
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_SqueezeBox_Player_CONST);
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Sound_Bridge_CONST);
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Squeezeslave_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

	// In our device data we will give ourselves a lower priority and set multipledestinations=true 
	// so we yield priority for single destinations to a non-streaming type of media
	for(size_t s=0;s<m_vectMediaHandlerInfo.size();++s)
		m_vectMediaHandlerInfo[s]->m_bMultipleDestinations=true;

	return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
SlimServer_PlugIn_Command *Create_SlimServer_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new SlimServer_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void SlimServer_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void SlimServer_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

class MediaStream *SlimServer_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	SlimServerMediaStream *pSlimServerMediaStream;
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}

	if ( pMediaDevice != NULL  && // test the media device only if it set
	     pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_SqueezeBox_Player_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_SqueezeBox_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Selected device (%d: %s) as playback device!",
			pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
			pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	pSlimServerMediaStream = new SlimServerMediaStream( this, pMediaHandlerInfo,
							iPK_MediaProvider, pMediaDevice, 
							iPK_Users, st_RemovableMedia, StreamID );

	return pSlimServerMediaStream;
}

SlimServerMediaStream *SlimServer_PlugIn::ConvertToSlimServerMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_SLIMSERVER )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a SlimServerMediaStream!").c_str());
		return NULL;
	}

	return static_cast<SlimServerMediaStream *>(pMediaStream);
}

bool SlimServer_PlugIn::StartMedia( class MediaStream *pMediaStream,string &sError )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "SlimServer_PlugIn::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );

	SlimServerMediaStream *pSlimServerMediaStream = NULL;
	if ( (pSlimServerMediaStream = ConvertToSlimServerMediaStream(pMediaStream, "SlimServer_PlugIn::StartMedia(): ")) == NULL )
		return false;

	if ( !pSlimServerMediaStream->m_pMediaDevice_Source || pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Media_Streamers_CONST )
	{
		pSlimServerMediaStream->m_pMediaDevice_Source = FindStreamerDevice();
	}

	if( pSlimServerMediaStream->m_pMediaDevice_Source && pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory == DEVICECATEGORY_Media_Streamers_CONST )
	{
		// Find the destinations for each ent area
		map<int, EntertainArea *>::iterator itEntertainmentAreas;
		for ( itEntertainmentAreas = pSlimServerMediaStream->m_mapEntertainArea.begin(); itEntertainmentAreas != pSlimServerMediaStream->m_mapEntertainArea.end(); itEntertainmentAreas++ )
		{
			EntertainArea *pEntertainArea = ( *itEntertainmentAreas ).second;
			pEntertainArea->m_pMediaDevice_ActiveDest = FindMediaDeviceForEntertainArea(pEntertainArea);
		}

		pSlimServerMediaStream->setIsStreaming();
		StartStreaming(pSlimServerMediaStream);
	}

	string sFileToPlay = pSlimServerMediaStream->GetFilenameToPlay("Empty file name");

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "SlimServer_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

	string mediaURL;
	string Response;

	mediaURL = sFileToPlay;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Is streaming ?: %d", pSlimServerMediaStream->isStreaming() );

	// Establish streaming configuration first if this applies here
	// This is probably needed only if we are doing a direct playback to the target ent area.
	//	if ( pSlimServerMediaStream->isStreaming() )
 	//		StartStreaming(pSlimServerMediaStream);

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
// 	if( !SendCommand( cmd, &Response ) )
// 		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The player %d (%s) didn't respond to play media command!",
// 					pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
// 					pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_sDescription.c_str());
// 	else
// 		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The sources device responded to play media command!" );

	// If there are more than 1 song in the queue, we likely added to an existing queue, so we want
	// to refresh=true so any orbiters will re-render the play list
	return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool SlimServer_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in SlimServer_PlugIn!");

	SlimServerMediaStream *pSlimServerMediaStream = NULL;

	if ( (pSlimServerMediaStream = ConvertToSlimServerMediaStream(pMediaStream, "SlimServer_PlugIn::StopMedia() ")) == NULL )
		return false;

	if( !pSlimServerMediaStream->m_pMediaDevice_Source )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Stopping media in SlimServer_PlugIn but mediadevice_source is null");
		return false;
	}

	int PK_Device = pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pSlimServerMediaStream->m_iStreamID_get( );
	string text;
	string SavedPosition;
	if( pSlimServerMediaStream->isStreaming() )
	{
	        // Get media position before stopping streaming, if we dont, the media might have stopped and its too late
		DCE::CMD_Report_Playback_Position reportPositionCommand(
				m_dwPK_Device,
				PK_Device,
				StreamID,
				&text,
				&SavedPosition);

		SendCommand(reportPositionCommand);
	  
		// send the StopStreaming command to the SlimServerStreamer
		StopStreaming(pSlimServerMediaStream,NULL);  // NULL=stop all
	}
	pSlimServerMediaStream->setIsStreaming(false);

	string SavedPosition2;
	DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
							PK_Device,  		// Send to the device that is actually playing
							StreamID,      		// Send the stream ID that we want to actually stop
							&SavedPosition2);


	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( cmd ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
	        if (SavedPosition2.length() > 0) {
		        // If Stop_Media replied with a media position, use that, as it might be more accurate
		        SavedPosition = SavedPosition2;
	        }

		if( pSlimServerMediaStream->m_iDequeMediaFile_Pos>=0 && pSlimServerMediaStream->m_iDequeMediaFile_Pos<pSlimServerMediaStream->m_dequeMediaFile.size() )
		{
			pSlimServerMediaStream->m_dequeMediaFile[pSlimServerMediaStream->m_iDequeMediaFile_Pos]->m_sStartPosition = SavedPosition;
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media stopped at %s",SavedPosition.c_str());
		}

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped at position: %d",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	}

	return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *SlimServer_PlugIn::FindStreamerDevice()
{
	int iStreamerDeviceId = 0;

	if ( (iStreamerDeviceId = m_pRouter->FindClosestRelative(DEVICETEMPLATE_Slim_Server_Streamer_CONST, m_dwPK_Device)) <= 0 )
		return NULL;
	else
		return m_pMedia_Plugin->m_mapMediaDevice_Find(iStreamerDeviceId);
}

bool SlimServer_PlugIn::StopStreaming(SlimServerMediaStream *pSlimServerMediaStream, vector<MediaDevice*> *stopStreamingTargets)
{
	if ( ! pSlimServerMediaStream->isStreaming() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SlimServer_PlugIn::StopStreaming() Function was called for a MediaStream that is not streaming currently!");
		return false;
	}

	vector<MediaDevice*>::const_iterator itDevices;
	map<int, EntertainArea *>::iterator itEntertainmentAreas;

	if( stopStreamingTargets )
		itDevices = stopStreamingTargets->begin();
	else
		itEntertainmentAreas = pSlimServerMediaStream->m_mapEntertainArea.begin();

	string strSqueezesToStopList = "";
	while ( true )
	{
		MediaDevice *pDevice;
		if( stopStreamingTargets )
			pDevice = (*itDevices);
		else
			pDevice = (*itEntertainmentAreas).second->m_pMediaDevice_ActiveDest;

		if( pDevice )
			strSqueezesToStopList += StringUtils::itos(pDevice->m_pDeviceData_Router->m_dwPK_Device) + ",";

		if( stopStreamingTargets && ++itDevices == stopStreamingTargets->end() )
			break;
		else if( !stopStreamingTargets && ++itEntertainmentAreas == pSlimServerMediaStream->m_mapEntertainArea.end() )
			break;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "SlimServer_PlugIn::StopStreaming() Send stop streaming command to the Streamer with targets: %s!", strSqueezesToStopList.c_str());
	if ( strSqueezesToStopList.size() != 0 )
	{
		// send the StopStreaming command to the SlimServerStreamer
		DCE::CMD_Stop_Streaming stopStreamingCommands(
				m_dwPK_Device,
				pSlimServerMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				pSlimServerMediaStream->m_iStreamID_get(),
				strSqueezesToStopList);

		SendCommand(stopStreamingCommands);
	}

	return true;
}

bool SlimServer_PlugIn::StartStreaming(SlimServerMediaStream *pMediaStream)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	map<int, MediaDevice *>::const_iterator itPlaybackDevices;
	map<int, MediaDevice *> mapPlaybackDevices;
	string strTargetDevices = "";

	if ( ! pMediaStream->isStreaming() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Called SlimServer_PlugIn::StartStreaming but with a MediaStream (streamid: %d) that is non streamable.", pMediaStream->m_iStreamID_get());
		return false;
	}

	if ( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SlimServer_PlugIn::StartStreaming() was passed a stream with a NULL source device! Ignoring request!.");
		return false;
	}

	for( MapEntertainArea::iterator itEA = pMediaStream->m_mapEntertainArea.begin( );itEA != pMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		GetRenderDevices(pEntertainArea, &mapPlaybackDevices);
	}

	// First get the list of playback devices
	for ( itPlaybackDevices = mapPlaybackDevices.begin(); itPlaybackDevices != mapPlaybackDevices.end(); itPlaybackDevices++ )
		strTargetDevices += StringUtils::itos((*itPlaybackDevices).second->m_pDeviceData_Router->m_dwPK_Device) + ",";

	// virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,Message *pMessage);
	itPlaybackDevices = mapPlaybackDevices.begin();

	DCE::CMD_Start_Streaming startStreamingCommand(
					m_dwPK_Device,
					pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					0,
					pMediaStream->m_iStreamID_get(),
					"",
					"",
					strTargetDevices);

	if( !SendCommand(startStreamingCommand) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Send StartStreaming Command failed");
		return false;
	}

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"About to call CMD_Play_Media sole master to %d play media within start streaming",pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	MediaFile *pMediaFile = NULL;
	// HACK: -- todo: get real informations.
	if( pMediaStream->m_dequeMediaFile.size()>pMediaStream->m_iDequeMediaFile_Pos )
		pMediaFile = pMediaStream->m_dequeMediaFile[pMediaStream->m_iDequeMediaFile_Pos];

	string sFilename = pMediaStream->GetFilenameToPlay("Empty file name");
	DCE::CMD_Play_Media cmd(m_dwPK_Device,
							pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
							pMediaStream->m_iPK_MediaType,
							pMediaStream->m_iStreamID_get( ),
							pMediaFile ? pMediaFile->m_sStartPosition : "",
							sFilename);

	// No handling of errors (it will in some cases deadlock the router.)
	SendCommand(cmd);

	if( pMediaFile )
		pMediaFile->m_sStartPosition=""; // Be sure to reset the start position so next time we start at the beginning of the file if this is in a queue

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Established streaming configuration: %d -> [%s]!",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
											strTargetDevices.c_str());
	return true;
}

MediaDevice *SlimServer_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_SqueezeBox_Player_CONST);
	if( !pMediaDevice )
		pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Sound_Bridge_CONST);
	if( !pMediaDevice )
		pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_Squeezeslave_CONST);

	if( !pMediaDevice )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SlimServer_PlugIn::FindMediaDeviceForEntertainArea no device in ea %d",pEntertainArea->m_iPK_EntertainArea);
		return NULL;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
}
