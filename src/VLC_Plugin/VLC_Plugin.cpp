/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "VLC_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VLC_Plugin::VLC_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VLC_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_VLCMediaMutex("vlc media mutex")
{
  m_VLCMediaMutex.Init(NULL);
}

//<-dceag-dest-b->
VLC_Plugin::~VLC_Plugin()
//<-dceag-dest-e->
{
  PLUTO_SAFETY_LOCK(mm,m_VLCMediaMutex);
  pthread_mutex_destroy(&m_VLCMediaMutex.mutex);
}

//<-dceag-getconfig-b->
bool VLC_Plugin::GetConfig()
{
	if( !VLC_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VLC_Plugin::Register()
//<-dceag-reg-e->
{
  m_iPriority=DATA_Get_Priority();
  
  m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
  m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
  if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to vlc plugin");
      return false;
    }
  
  vector<int> vectPK_DeviceTemplate;
  vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_VLC_Player_CONST);
  m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );
  
  RegisterMsgInterceptor(( MessageInterceptorFn )( &VLC_Plugin::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );
  
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
void VLC_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void VLC_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNHANDLED";
}

//<-dceag-sample-b->!

/**
 * MediaHandlerBase implementations, below.
 */
class MediaStream *VLC_Plugin::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
  VLCMediaStream *pVLCMediaStream;
  MediaDevice *pMediaDevice_PassedIn;
  
  PLUTO_SAFETY_LOCK( vm, m_VLCMediaMutex );
  
  if(m_bQuit_get())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Plugin::CreateMediaStream with m_bQuit");
      return NULL;
    }

  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  pMediaDevice_PassedIn = NULL;
  if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
      return NULL;
    }

  if (pMediaDevice != NULL &&
      pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_VLC_Player_CONST)
    {
      pMediaDevice_PassedIn = pMediaDevice;
      pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_VLC_Player_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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
  
  pVLCMediaStream = new VLCMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
					pMediaDevice,
					iPK_Users, st_RemovableMedia, StreamID );

  // if the source device is a disk drive then we can't move this media stream around.
  if ( pMediaDevice_PassedIn && pMediaDevice_PassedIn->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Disk_Drive_CONST )
    pVLCMediaStream->setIsMovable(false);

  return pVLCMediaStream;
}

VLCMediaStream *VLC_Plugin::ConvertToVLCMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  if ( pMediaStream == NULL )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
      return NULL;
    }
  
  if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_VIDEOLAN )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a VLCMediaStream!").c_str());
      return NULL;
    }
  
  return static_cast<VLCMediaStream*>(pMediaStream);
}

bool VLC_Plugin::StartMedia( MediaStream *pMediaStream,string &sError )
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "VLC_Plugin::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
  
  VLCMediaStream *pVLCMediaStream = NULL;
  if ( (pVLCMediaStream = ConvertToVLCMediaStream(pMediaStream, "VLC_Plugin::StartMedia(): ")) == NULL )
    return false;

  string sFileToPlay;
  
  MediaFile *pMediaFile = NULL;
  // HACK: -- todo: get real informations.
  if( pVLCMediaStream->m_dequeMediaFile.size()>pVLCMediaStream->m_iDequeMediaFile_Pos )
    pMediaFile = pVLCMediaStream->m_dequeMediaFile[pVLCMediaStream->m_iDequeMediaFile_Pos];
  
  if( pMediaFile && (pMediaFile->m_dwPK_Device_Disk_Drive || pMediaFile->m_dwPK_Disk)  )
    {
      sFileToPlay = m_pMedia_Plugin->GetMRLFromDiscID(pMediaFile->m_dwPK_Disk,pMediaFile->m_dwPK_Device_Disk_Drive);
      
      //don't reset media stream's PK_Disc 
      if(pMediaFile->m_dwPK_Disk)
	pVLCMediaStream->m_dwPK_Disc = pMediaFile->m_dwPK_Disk;
    }
  else 
    sFileToPlay = pVLCMediaStream->GetFilenameToPlay("Empty file name");
  
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "VLC_Plugin::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());
  
  string mediaURL;
  string Response;
  
  mediaURL = sFileToPlay;
  
  // If there's a \t(xxx)\t embedded in the file name, than this is a disc and xxx is the drive
  string::size_type size = mediaURL.size();
  string::size_type posDrive;
  if( size>3 && mediaURL[size-1]=='\t' && mediaURL[size-2]==')' && (posDrive=mediaURL.find("\t("))!=string::npos )
    {
      int PK_Device_Drive = atoi( mediaURL.substr( posDrive+2 ).c_str() );
      DeviceData_Router *pDevice_Drive = m_pMedia_Plugin->m_pRouter->m_mapDeviceData_Router_Find(PK_Device_Drive);
      mediaURL = mediaURL.substr(0,posDrive);  // It didn't work with this and the above as 1 command
      
      if( pDevice_Drive && pDevice_Drive->GetTopMostDevice() != pVLCMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->GetTopMostDevice() )
	{
	  // We don't have the correct source xine.  Find one that's on the same pc as the disk drive, and it will be the source
	  DeviceData_Router *pDevice_VLC = (DeviceData_Router *) pDevice_Drive->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_VLC_Player_CONST);
	  MediaDevice *pDevice_Source = NULL;
	  if( pDevice_VLC==NULL || (pDevice_Source=m_pMedia_Plugin->m_mapMediaDevice_Find(pDevice_VLC->m_dwPK_Device))==NULL )
	    // This probably isn't want they want, but go ahead and let it play the local disc anyway
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Plugin::StartMedia -- need to play a disc on a pc with no vlc player!");
	  else
	    pVLCMediaStream->m_pMediaDevice_Source = pDevice_Source;
	}
    }

  if ( pVLCMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST || FileUtils::FindExtension(mediaURL)=="dvd" || FileUtils::FindExtension(mediaURL)=="iso" ) 
    {
      mediaURL = "dvd://" + mediaURL;
      // If the source is one ea and the destination in another, it could be remotely playing a disc, so 
      // let ConfirmSourceIsADestination process it
      // and see if it can still make it non-streaming by using network block
      // If it changes the source to use an nbd device, then the subsequent StreamingRequired will return true
      // Only for DVD, Audio CDs cannot use network block(nbd), so we must always stream
      if( pVLCMediaStream->StreamingRequired() )
	{
	  // For now we're not able to have a xine that renders to a NULL window and can do dvd's.  They require 
	  // a live window with events.  So for the moment this function will confirm that if we're playing a dvd disc remotely that we make the 
	  // source be one of the destinations, and change the mrl to reference the source disk
	  if( !ConfirmSourceIsADestination(mediaURL,pVLCMediaStream,pMediaFile ? pMediaFile->m_dwPK_Device_Disk_Drive : 0) )
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Plugin::StartMedia don't know how media will get to destination.  Unless there's some output zones in the mix results won't be right");
	}
      
    }
  else if( mediaURL.size()>5 && mediaURL.substr(0,5)=="/dev/" && pVLCMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_CD_CONST )
    {
      mediaURL = "cdda://" + mediaURL;
      if( pMediaFile && pMediaFile->m_iTrack )
	mediaURL += "/" + StringUtils::itos(pMediaFile->m_iTrack);
    }
  else if( mediaURL.size()>5 && mediaURL.substr(0,5)=="/dev/" && pVLCMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_BD_CONST )
    {
      mediaURL = "bluray://" + mediaURL;
    }
  
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d", 
					m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					pMediaStream->m_iDequeMediaFile_Pos);
    if (pVLCMediaStream->StreamingRequired())
      {
	DCE::CMD_Start_Streaming scmd(m_dwPK_Device,
				      pVLCMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				      pVLCMediaStream->m_iPK_MediaType,
				      pVLCMediaStream->m_iStreamID_get(),
				      pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pVLCMediaStream->m_sStartPosition,
				      mediaURL,
				      pVLCMediaStream->GetTargets(DEVICETEMPLATE_VLC_Player_CONST));
	SendCommand(scmd);
      }
    else
      {
	DCE::CMD_Play_Media cmd(m_dwPK_Device,
				pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
				pVLCMediaStream->m_iPK_MediaType,
				pVLCMediaStream->m_iStreamID_get( ),
				pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pVLCMediaStream->m_sStartPosition,
				mediaURL);
	
	// No handling of errors (it will in some cases deadlock the router.)
	SendCommand(cmd);
      }

    // if the disk ID not known, trying to see if we have a file with this name
    int iPK_File=0;
    if (pMediaFile)
      iPK_File = pMediaFile->m_dwPK_File;
    // if file or disk ID is already known, sending it
    if ( iPK_File || pMediaStream->m_dwPK_Disc )
      {
	string sMediaID = (iPK_File?"F":"D");
	sMediaID += StringUtils::itos(iPK_File?iPK_File:pMediaStream->m_dwPK_Disc);
	DCE::CMD_Set_Media_ID cmd(m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, sMediaID, pVLCMediaStream->m_iStreamID_get());
	SendCommand(cmd);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "set media id command sent with id %s", sMediaID.c_str());
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "set media id command not sent");
      }
    
    if( pMediaFile && pVLCMediaStream->m_iPK_Playlist==0 )  // If this is part of a playlist, rather than just a normal bookmark, the user will likely want it to keep resuming at the set position
      pMediaFile->m_sStartPosition=""; // Be sure to reset the start position so next time we start at the beginning of the file if this is in a queue
    
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    
    return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool VLC_Plugin::StopMedia( class MediaStream *pMediaStream )
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in VLC_Plugin!");
  
  VLCMediaStream *pVLCMediaStream = NULL;
  
  if ( (pVLCMediaStream = ConvertToVLCMediaStream(pMediaStream, "VLC_Plugin::StopMedia() ")) == NULL )
    return false;
  
  if( !pVLCMediaStream->m_pMediaDevice_Source )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Stopping media in VLC_Plugin but mediadevice_source is null");
      return false;
    }
  int PK_Device = pVLCMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
  int StreamID = pVLCMediaStream->m_iStreamID_get( );
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
      pVLCMediaStream->m_sLastPosition=SavedPosition;
      if( pVLCMediaStream->m_iDequeMediaFile_Pos>=0 && pVLCMediaStream->m_iDequeMediaFile_Pos<pVLCMediaStream->m_dequeMediaFile.size() )
	{
	  pVLCMediaStream->m_dequeMediaFile[pVLCMediaStream->m_iDequeMediaFile_Pos]->m_sStartPosition = SavedPosition;
	  LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media stopped at %s",SavedPosition.c_str());
	}
      
      LoggerWrapper::GetInstance()->Write( LV_STATUS, "The target device %d responded to stop media command",
					   pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    }
  
  return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *VLC_Plugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  MediaDevice *pMediaDevice;
  pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_VLC_Player_CONST);
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
  if ( pMediaDevice == NULL )
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find a VLC Player device (with device template id: %d) in the entertainment area: %d. Looking for a squeeze box.",
					  DEVICETEMPLATE_VLC_Player_CONST,
					  pEntertainArea->m_iPK_EntertainArea);
      
      return NULL;
    }
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
  
  return pMediaDevice;
}

bool VLC_Plugin::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
  PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  /** Confirm this is from one of ours */
  if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_VLC_Player_CONST )
    return false; /** Some other media player. We only know xine's menu handling */
  
  int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
  bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";
  
  /** Find the stream */
  VLCMediaStream *pVLCMediaStream = NULL;
  MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID,pMessage->m_dwPK_Device_From );
  if( !pMediaStream || (pVLCMediaStream = ConvertToVLCMediaStream(pMediaStream, "VLC_Plugin::MenuOnScreen(): ")) == NULL )
    return false;
  
  pVLCMediaStream->m_bUseAltScreens=bOnOff;
  
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pVLCMediaStream, pVLCMediaStream->m_iStreamID_get( ), pVLCMediaStream->m_iPK_MediaType );
  LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pVLCMediaStream->m_mapEntertainArea.size( ) );
  
  
  /** We're going to send a message to all the orbiters in this area so they know what the remote is,
      and we will send all bound remotes to the new screen */
  for( MapEntertainArea::iterator itEA = pVLCMediaStream->m_mapEntertainArea.begin( );itEA != pVLCMediaStream->m_mapEntertainArea.end( );++itEA )
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
	  pVLCMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
	}
    }
  
  return false; // Let something else intercept it if needed.
}

bool VLC_Plugin::ConfirmSourceIsADestination(string &sMRL,VLCMediaStream *pVLCMediaStream,int PK_Device_Drive)
{
  MediaDevice *pMediaDevice_VLC = NULL; // Any xine that's in the destination
  for( MapEntertainArea::iterator itEA = pVLCMediaStream->m_mapEntertainArea.begin( );itEA != pVLCMediaStream->m_mapEntertainArea.end( );++itEA )
    {
      EntertainArea *pEntertainArea = ( *itEA ).second;
      ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_VLC_Player_CONST);
      if( !pListMediaDevice )
	continue;
      for(ListMediaDevice::iterator it=pListMediaDevice->begin();it!=pListMediaDevice->end();++it)
	{
	  MediaDevice *pMediaDevice = *it;
	  if( pVLCMediaStream->m_pMediaDevice_Source->IsInEntertainArea( pEntertainArea ) )
	    return true; // We're good.  The source is one of the destinations
	  if( !pMediaDevice_VLC )
	    pMediaDevice_VLC = pMediaDevice;
	}
    }
  
  // If we reached here, then the source is not in one of the destination areas
  if( !pMediaDevice_VLC )
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Plugin::ConfirmSourceIsADestination no xine player can handle this out of %d ea's",pVLCMediaStream->m_mapEntertainArea.size());
      return false;
    }
  
  // See if the source was a disk drive
  if( sMRL.find("/dev/")!=string::npos )
    {
      // TODO -- what if the directory is /dev/??
      DeviceData_Router *pDevice_Disk_Drive = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_Drive);
      if( !pDevice_Disk_Drive )
	pDevice_Disk_Drive = (DeviceData_Router *) pVLCMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Disc_Drives_CONST);
      if( !pDevice_Disk_Drive )
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Plugin::ConfirmSourceIsADestination can't find the disk drive %s",sMRL.c_str());
	  return false;
	}
      string sDrive = pDevice_Disk_Drive->m_mapParameters_Find(DEVICEDATA_Drive_CONST);
      string::size_type pos = sMRL.find( sDrive );  // Find the /dev/cdrom in ther
      if( pos==string::npos )
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Plugin::ConfirmSourceIsADestination 1 can't find drive's device %s -- %s",sMRL.c_str(),sDrive.c_str());
	  sDrive = "/dev/cdrom";
	  pos = sMRL.find( sDrive );  // Find the /dev/cdrom in ther
	}
      if( pos==string::npos )
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Plugin::ConfirmSourceIsADestination can't find drive's device %s -- %s",sMRL.c_str(),sDrive.c_str());
	  return false;
	}
      
      int iComputerID = -1;
      
      DeviceData_Base *pParent = pDevice_Disk_Drive->GetTopMostDevice();
      
      if (pParent)
	iComputerID = pParent->m_dwPK_Device;
      
      if (iComputerID == -1)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Plugin::ConfirmSourceIsADestination can't find the computer on which disk drive is: %i", pDevice_Disk_Drive->m_dwPK_Device);
	  return false;
	}
      
      string sDrive_New = "/mnt/optical/" + StringUtils::itos(iComputerID) + "_" + StringUtils::Replace(sDrive, "/dev/", "");
      StringUtils::Replace(&sMRL,sDrive,sDrive_New);
    }
  else
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Plugin::ConfirmSourceIsADestination %s isn't a disk device",sMRL.c_str());  // Shouldn't happen
  
  pVLCMediaStream->m_pMediaDevice_Source = pMediaDevice_VLC;
  return true;
}
 
/*

	COMMANDS TO IMPLEMENT

*/


