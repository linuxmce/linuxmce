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
#include "YouTube_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "YouTubeMediaStream.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Table_Users.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
YouTube_Plugin::YouTube_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: YouTube_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_YouTubeMediaMutex("youtube media mutex")
{
  m_YouTubeMediaMutex.Init(NULL);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
YouTube_Plugin::~YouTube_Plugin()
//<-dceag-dest-e->
{
  PLUTO_SAFETY_LOCK(mm,m_YouTubeMediaMutex);
  pthread_mutex_destroy(&m_YouTubeMediaMutex.mutex);
}

//<-dceag-getconfig-b->
bool YouTube_Plugin::GetConfig()
{
	if( !YouTube_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool YouTube_Plugin::Register()
//<-dceag-reg-e->
{
	m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to youtube plugin");
		return false;
	}

	vector<int> vectPK_DeviceTemplate;
	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_YouTube_Player_CONST);
	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXX YOUTUBE PLAYER REGISTERED XXXXXXX");

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
void YouTube_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void YouTube_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

class MediaStream *YouTube_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
  YouTubeMediaStream *pYouTubeMediaStream;
  MediaDevice *pMediaDevice_PassedIn;

  PLUTO_SAFETY_LOCK(xm, m_YouTubeMediaMutex);

  if (m_bQuit_get())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"YouTube_Plugin::CreateMediaStream called with m_bQuit set. Returning NULL.");
      return NULL;
    }
  
  PLUTO_SAFETY_LOCK(mm, m_pMedia_Plugin->m_MediaMutex);
  
  pMediaDevice_PassedIn=NULL;
  
  if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
      return NULL;
    }
  
  if ( pMediaDevice != NULL  && // test the media device only if it set
       pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_YouTube_Player_CONST )
    {
      pMediaDevice_PassedIn = pMediaDevice;
      pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_YouTube_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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
  
  pYouTubeMediaStream = new YouTubeMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
						pMediaDevice,
						iPK_Users, st_RemovableMedia, StreamID );
  
  m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;

  return pYouTubeMediaStream;

}

bool YouTube_Plugin::StartMedia( class MediaStream *pMediaStream,string &sError )
{

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"YouTube_Plugin::StartMedia Called");
  
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  YouTubeMediaStream *pYouTubeMediaStream = NULL;
  if ( (pYouTubeMediaStream = ConvertToYouTubeMediaStream(pMediaStream, "YouTube_Plugin::StartMedia(): ")) == NULL )
    return false;
  
  string sFileToPlay;
  MediaFile *pMediaFile = NULL;
  
  sFileToPlay = pYouTubeMediaStream->GetFilenameToPlay("Empty file name");
  
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "YouTube_PlugIn::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());
  
  string mediaURL;
  string Response;
  
  mediaURL = sFileToPlay;
  
  // send the CMD straight through.
  
  pYouTubeMediaStream->m_sMediaDescription = "YouTube";
  
  DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
				     pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				     pYouTubeMediaStream->m_iPK_MediaType,
				     pYouTubeMediaStream->m_iStreamID_get( ),
				     "00:00:00",mediaURL);
  SendCommand(CMD_Play_Media);
  
  /** We're going to send a message to all the orbiters in this area so they know what the remote is,
      and we will send all bound remotes to the new screen */
  for( MapEntertainArea::iterator itEA = pYouTubeMediaStream->m_mapEntertainArea.begin( );itEA != pYouTubeMediaStream->m_mapEntertainArea.end( );++itEA )
    {
      EntertainArea *pEntertainArea = ( *itEA ).second;
      LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
      for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
	  OH_Orbiter *pOH_Orbiter = (*it).second;
	  if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
	    continue;
	  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
	  bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
	  pYouTubeMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
	}
    }
  
  return MediaHandlerBase::StartMedia(pMediaStream,sError);
  
}

bool YouTube_Plugin::StopMedia( class MediaStream *pMediaStream )
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StopMedia Called");
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
  if( it!=m_mapDevicesToStreams.end() )
    m_mapDevicesToStreams.erase(it);
  
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "YouTube_Plugin::StopMedia() Stopping Media Stream Playback... Pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
  
  YouTubeMediaStream *pYouTubeMediaStream = NULL;
  
  if ((pYouTubeMediaStream = ConvertToYouTubeMediaStream(pMediaStream,"YouTube_Plugin::StopMedia():")) == NULL )
    return false;
  
  string savedPosition;
  
  DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,
				     pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				     pYouTubeMediaStream->m_iStreamID_get(),
				     &savedPosition);
  
  SendCommand(CMD_Stop_Media);
  return true;  
}

MediaDevice *YouTube_Plugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  MediaDevice *pMediaDevice;
  pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_YouTube_Player_CONST);
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
  if ( pMediaDevice == NULL )
    {
      return NULL;
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
  
  return pMediaDevice;
}

YouTubeMediaStream *YouTube_Plugin::ConvertToYouTubeMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
		return NULL;
	}
 
	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_YOUTUBE )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a YouTubeMediaStream!").c_str());
		return NULL;
	}
 
	return static_cast<YouTubeMediaStream*>(pMediaStream);
}

/* Grabbed almost verbatim from Xine Player */
bool YouTube_Plugin::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  /** Confirm this is from one of ours */
  if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_YouTube_Player_CONST )
    return false; 
  
  int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
  bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";
  
  /** Find the stream */
  YouTubeMediaStream *pYouTubeMediaStream = NULL;
  MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID,pMessage->m_dwPK_Device_From );
  if( !pMediaStream || (pYouTubeMediaStream = ConvertToYouTubeMediaStream(pMediaStream, "YouTube_Plugin::MenuOnScreen(): ")) == NULL )
    return false;
  
  pYouTubeMediaStream->m_bUseAltScreens=bOnOff;
  
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pYouTubeMediaStream, pYouTubeMediaStream->m_iStreamID_get( ), pYouTubeMediaStream->m_iPK_MediaType );
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pYouTubeMediaStream->m_mapEntertainArea.size( ) );
  
  
  /** We're going to send a message to all the orbiters in this area so they know what the remote is,
      and we will send all bound remotes to the new screen */
  for( MapEntertainArea::iterator itEA = pYouTubeMediaStream->m_mapEntertainArea.begin( );itEA != pYouTubeMediaStream->m_mapEntertainArea.end( );++itEA )
    {
      EntertainArea *pEntertainArea = ( *itEA ).second;
      LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
      for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
	  OH_Orbiter *pOH_Orbiter = (*it).second;
	  if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
	    continue;
	  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
	  bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
	  pYouTubeMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
	}
    }
  
  return false;
}

