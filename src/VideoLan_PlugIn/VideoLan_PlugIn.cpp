/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "VideoLan_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VideoLan_PlugIn::VideoLan_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VideoLan_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-getconfig-b->
bool VideoLan_PlugIn::GetConfig()
{
	if( !VideoLan_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	return true;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
VideoLan_PlugIn::VideoLan_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: VideoLan_PlugIn_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
VideoLan_PlugIn::~VideoLan_PlugIn()
//<-dceag-dest-e->
{
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VideoLan_PlugIn::Register()
//<-dceag-reg-e->
{
	m_iPriority=DATA_Get_Priority();
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_VideoLan_Client_CONST);
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
VideoLan_PlugIn_Command *Create_VideoLan_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new VideoLan_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void VideoLan_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VideoLan_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
/*

	COMMANDS TO IMPLEMENT

*/

class MediaStream *VideoLan_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	VideoLanMediaStream *pVideoLanMediaStream;
	MediaDevice *pMediaDevice_PassedIn;

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	pMediaDevice_PassedIn = NULL;
	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}

	if ( pMediaDevice != NULL  && // test the media device only if it set
	     pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_VideoLan_Client_CONST )
	{
		pMediaDevice_PassedIn = pMediaDevice;
		pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_VideoLan_Client_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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

	pVideoLanMediaStream = new VideoLanMediaStream( this, pMediaHandlerInfo, iPK_MediaProvider,
							pMediaDevice, 
							iPK_Users, st_RemovableMedia, StreamID );

	return pVideoLanMediaStream;
}

VideoLanMediaStream *VideoLan_PlugIn::ConvertToVideoLanMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}

	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_VIDEOLAN )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a VideoLanMediaStream!").c_str());
		return NULL;
	}

	return static_cast<VideoLanMediaStream *>(pMediaStream);
}

bool VideoLan_PlugIn::StartMedia( class MediaStream *pMediaStream,string &sError )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "VideoLan_PlugIn::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );

	VideoLanMediaStream *pVideoLanMediaStream = NULL;
	if ( (pVideoLanMediaStream = ConvertToVideoLanMediaStream(pMediaStream, "VideoLan_PlugIn::StartMedia(): ")) == NULL )
		return false;

	if ( !pVideoLanMediaStream->m_pMediaDevice_Source || pVideoLanMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory != DEVICECATEGORY_Media_Streamers_CONST )
	{
		pVideoLanMediaStream->m_pMediaDevice_Source = FindStreamerDevice();
	}

	if( pVideoLanMediaStream->m_pMediaDevice_Source && pVideoLanMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_DeviceCategory == DEVICECATEGORY_Media_Streamers_CONST )
	{
		// Find the destinations for each ent area
		map<int, EntertainArea *>::iterator itEntertainmentAreas;
		for ( itEntertainmentAreas = pVideoLanMediaStream->m_mapEntertainArea.begin(); itEntertainmentAreas != pVideoLanMediaStream->m_mapEntertainArea.end(); itEntertainmentAreas++ )
		{
			EntertainArea *pEntertainArea = ( *itEntertainmentAreas ).second;
			pEntertainArea->m_pMediaDevice_ActiveDest = FindMediaDeviceForEntertainArea(pEntertainArea);
		}

		pVideoLanMediaStream->setIsStreaming();
		StartStreaming(pVideoLanMediaStream);
	}

	string sFileToPlay = pVideoLanMediaStream->GetFilenameToPlay("Empty file name");

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "VideoLan_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

	string mediaURL;
	string Response;

	mediaURL = sFileToPlay;


	LoggerWrapper::GetInstance()->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
// 	if( !SendCommand( cmd, &Response ) )
// 		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The player %d (%s) didn't respond to play media command!",
// 					pVideoLanMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
// 					pVideoLanMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_sDescription.c_str());
// 	else
// 		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The sources device responded to play media command!" );

	// If there are more than 1 song in the queue, we likely added to an existing queue, so we want
	// to refresh=true so any orbiters will re-render the play list
	return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool VideoLan_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in VideoLan_PlugIn!");

	VideoLanMediaStream *pVideoLanMediaStream = NULL;

	if ( (pVideoLanMediaStream = ConvertToVideoLanMediaStream(pMediaStream, "VideoLan_PlugIn::StopMedia() ")) == NULL )
		return false;

	if( pVideoLanMediaStream->isStreaming() )
		StopStreaming(pVideoLanMediaStream,NULL);  // NULL=stop all
	pVideoLanMediaStream->setIsStreaming(false);

	if( !pVideoLanMediaStream->m_pMediaDevice_Source )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Stopping media in VideoLan_PlugIn but mediadevice_source is null");
		return false;
	}
	int PK_Device = pVideoLanMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int StreamID = pVideoLanMediaStream->m_iStreamID_get( );
	string SavedPosition;
	DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
							PK_Device,  		// Send to the device that is actually playing
							StreamID,      		// Send the stream ID that we want to actually stop
							&SavedPosition);


	// TODO: Remove the device from the list of players also.
	string Response;
	if( !SendCommand( cmd ) ) // hack - todo see above, &Response ) )
	{
		// TODO: handle failure when sending the command. This is ignored now.
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
	}
	else
	{
		if( pVideoLanMediaStream->m_iDequeMediaFile_Pos>=0 && pVideoLanMediaStream->m_iDequeMediaFile_Pos<pVideoLanMediaStream->m_dequeMediaFile.size() )
		{
			pVideoLanMediaStream->m_dequeMediaFile[pVideoLanMediaStream->m_iDequeMediaFile_Pos]->m_sStartPosition = SavedPosition;
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media stopped at %s",SavedPosition.c_str());
		}

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "The target device %d responded to stop media command! Stopped",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	}

	return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *VideoLan_PlugIn::FindStreamerDevice()
{
	int iStreamerDeviceId = 0;

	if ( (iStreamerDeviceId = m_pRouter->FindClosestRelative(DEVICETEMPLATE_VideoLan_Server_CONST, m_dwPK_Device)) <= 0 )
		return NULL;
	else
		return m_pMedia_Plugin->m_mapMediaDevice_Find(iStreamerDeviceId);
}

bool VideoLan_PlugIn::StopStreaming(VideoLanMediaStream *pVideoLanMediaStream, vector<MediaDevice*> *stopStreamingTargets)
{
	if ( ! pVideoLanMediaStream->isStreaming() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "VideoLan_PlugIn::StopStreaming() Function was called for a MediaStream that is not streaming currently!");
		return false;
	}

	vector<MediaDevice*>::const_iterator itDevices;
	map<int, EntertainArea *>::iterator itEntertainmentAreas;

	if( stopStreamingTargets )
		itDevices = stopStreamingTargets->begin();
	else
		itEntertainmentAreas = pVideoLanMediaStream->m_mapEntertainArea.begin();

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
		else if( !stopStreamingTargets && ++itEntertainmentAreas == pVideoLanMediaStream->m_mapEntertainArea.end() )
			break;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "VideoLan_PlugIn::StopStreaming() Send stop streaming command to the Streamer with targets: %s!", strSqueezesToStopList.c_str());
	if ( strSqueezesToStopList.size() != 0 )
	{
		// send the StopStreaming command to the VideoLanStreamer
		DCE::CMD_Stop_Streaming stopStreamingCommands(
				m_dwPK_Device,
				pVideoLanMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				pVideoLanMediaStream->m_iStreamID_get(),
				strSqueezesToStopList);

		SendCommand(stopStreamingCommands);
	}

	return true;
}

bool VideoLan_PlugIn::StartStreaming(VideoLanMediaStream *pMediaStream)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	map<int, MediaDevice *>::const_iterator itPlaybackDevices;
	map<int, MediaDevice *> mapPlaybackDevices;
	string strTargetDevices = "";

	if ( ! pMediaStream->isStreaming() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Called VideoLan_PlugIn::StartStreaming but with a MediaStream (streamid: %d) that is non streamable.", pMediaStream->m_iStreamID_get());
		return false;
	}

	if ( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "VideoLan_PlugIn::StartStreaming() was passed a stream with a NULL source device! Ignoring request!.");
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

	string resultingURL;
	DCE::CMD_Start_Streaming startStreamingCommand(
					m_dwPK_Device,
					pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					0,
					pMediaStream->m_iStreamID_get(),
					"",
					resultingURL,
					strTargetDevices);

	if( !SendCommand(startStreamingCommand) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Send StartStreaming Command failed");
		return false;
	}

		string mediaURL = pMediaStream->GetFilenameToPlay("Empty file name");
		MediaFile *pMediaFile = NULL;
		// HACK: -- todo: get real informations.
		if( pMediaStream->m_dequeMediaFile.size()>pMediaStream->m_iDequeMediaFile_Pos )
		{
			pMediaFile = pMediaStream->m_dequeMediaFile[pMediaStream->m_iDequeMediaFile_Pos];
			if( pMediaFile && pMediaFile->m_sDescription.size() )
				pMediaStream->m_sMediaDescription = pMediaFile->m_sDescription;
			else
				pMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(mediaURL);
		}
		else
			pMediaStream->m_sMediaDescription = FileUtils::FilenameWithoutPath(mediaURL);

		if ( pMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST )
		{
			if( mediaURL[0]=='/' )
				mediaURL = "dvdsimple:" + mediaURL;
			else
				mediaURL = "dvdsimple:/" + mediaURL;
		}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"About to call CMD_Play_Media sole master to %d play media within start streaming",pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);

	DCE::CMD_Play_Media cmd(m_dwPK_Device,
							pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
							pMediaStream->m_iPK_MediaType,
							pMediaStream->m_iStreamID_get( ),
							pMediaFile ? pMediaFile->m_sStartPosition : "",
							mediaURL);

	// No handling of errors (it will in some cases deadlock the router.)
	SendCommand(cmd);

	if( pMediaFile )
		pMediaFile->m_sStartPosition=""; // Be sure to reset the start position so next time we start at the beginning of the file if this is in a queue

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Established streaming configuration: %d -> [%s]!",
											pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
											strTargetDevices.c_str());
	return true;
}

MediaDevice *VideoLan_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
	MediaDevice *pMediaDevice;
	pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_VideoLan_Client_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

	return pMediaDevice;
}
