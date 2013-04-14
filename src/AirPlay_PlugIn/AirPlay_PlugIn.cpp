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
#include "AirPlay_PlugIn.h"
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
#include "pluto_main/Define_DeviceTemplate.h"

/*#include "Orbiter_Plugin/Orbiter_Plugin.h"
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
#include "../pluto_main/Table_Users.h"*/



//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
AirPlay_PlugIn::AirPlay_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: AirPlay_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_AirPlayMediaMutex("airplay media mutex")
{
  	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::Starting up...");
  	m_AirPlayMediaMutex.Init(NULL);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
AirPlay_PlugIn::~AirPlay_PlugIn()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool AirPlay_PlugIn::GetConfig()
{
	if( !AirPlay_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool AirPlay_PlugIn::Register()
//<-dceag-reg-e->
{
  	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::Register()");
	m_pMedia_Plugin = ( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
  	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
  	//if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
    if( !m_pMedia_Plugin)
    	{
      		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::Cannot find sister plugins");
      		return false;
    	}

  	vector<int> vectPK_DeviceTemplate;
  	vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_AirPlay_Streamer_CONST);
  	m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

  	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::Registered device media player 2246");

  	//RegisterMsgInterceptor(( MessageInterceptorFn )( &AirPlay_PlugIn::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );

	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
AirPlay_PlugIn_Command *Create_AirPlay_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new AirPlay_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void AirPlay_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void AirPlay_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

class MediaStream *AirPlay_PlugIn::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
  	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_PlugIn::CreateMediaStream Called");
  
  	AirPlayMediaStream *pAirPlayMediaStream;
  	MediaDevice *pMediaDevice_PassedIn;
  	PLUTO_SAFETY_LOCK( xm, m_AirPlayMediaMutex );
  
 
  	if(m_bQuit_get())
    {
      	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::CreateMediaStream with m_bQuit");
      	return NULL;
    }
  
  	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
  
  	pMediaDevice_PassedIn = NULL;
  	if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
    {
      	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::I can't create a media stream without an entertainment area or a media device");
      	return NULL;
    }
  
  	if ( pMediaDevice != NULL  && // test the media device only if it set
       pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_AirPlay_Streamer_CONST )
    {	
      	pMediaDevice_PassedIn = pMediaDevice;
      	pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_AirPlay_Streamer_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
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
	  		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::I didn't find a device in the target entertainment area !");
	  		return NULL;
		}
    }
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::Selected device (%d: %s) as playback device!",
				      pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				      pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
  
  pAirPlayMediaStream = new AirPlayMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
					  pMediaDevice,
					  iPK_Users, st_RemovableMedia, StreamID );
  
  m_mapDevicesToStreams[pMediaDevice->m_pDeviceData_Router->m_dwPK_Device] = StreamID;

  return pAirPlayMediaStream;
  
}

MediaDevice *AirPlay_PlugIn::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
		PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
		MediaDevice *pMediaDevice;
		pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_AirPlay_Streamer_CONST);
 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::Looking for a proper device in the ent area %d - %s"
			, pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
		if ( pMediaDevice == NULL ) return NULL;

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::Returning device %d - %s"
			, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());
 
		return pMediaDevice;
}

AirPlayMediaStream *AirPlay_PlugIn::ConvertToAirPlayMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	if ( pMediaStream == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "AirPlay_PlugIn::Stream is a NULL stream!").c_str());
		return NULL;
	}
 
	if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_AIRPLAY_AUDIO )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "AirPlay_PlugIn::Stream is not an AirPlayMediaStream!").c_str());
		return NULL;
	}
 
	return static_cast<AirPlayMediaStream*>(pMediaStream);
}

bool AirPlay_PlugIn::StartMedia( MediaStream *pMediaStream,string &sError )
{
	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "AirPlay_PlugIn::StartMedia() Starting media stream playback.");

	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex ); 
 
	AirPlayMediaStream *pAirPlayMediaStream = NULL;
	if ( (pAirPlayMediaStream = ConvertToAirPlayMediaStream(pMediaStream, "AirPlay_PlugIn::StartMedia(): ")) == NULL )
		return false;
 
	MediaFile *pMediaFile = NULL;
	string sFileToPlay;
	string sLocation, sPosition;

	LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "AirPlay_PlugIn::StartMedia() Media type %d", pMediaStream->m_iPK_MediaType);
 
	sFileToPlay = pAirPlayMediaStream->GetFilenameToPlay("empty file name");

	// If there is a | at the end, then parse it as an explicit seek location. 
	if (sFileToPlay.find("}") != string::npos)
	  {
	    string::size_type pos=0;
	    sLocation = StringUtils::Tokenize(sFileToPlay, "}",pos);
	    sPosition = StringUtils::Tokenize(sFileToPlay, "}",pos);
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sLocation is %s",sLocation.c_str());
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sPosition is %s",sPosition.c_str());
	  }
	else
	  {
	    sLocation = sFileToPlay;
	    sPosition = "";
	  }

	// send the CMD straight through.
 
	pAirPlayMediaStream->m_sMediaDescription = "AirPlay audio";

	DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
					   pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
					   pAirPlayMediaStream->m_iPK_MediaType,
					   pAirPlayMediaStream->m_iStreamID_get(),
					   sPosition,sLocation);
	SendCommand(CMD_Play_Media);
 
	/* We're going to send a message to all the orbiters in this area so they know what the remote is,
	and we will send all bound remotes to the new screen */
	
	
	
	for( MapEntertainArea::iterator itEA = pAirPlayMediaStream->m_mapEntertainArea.begin( );itEA != pAirPlayMediaStream->m_mapEntertainArea.end( );++itEA )
	{
		EntertainArea *pEntertainArea = ( *itEA ).second;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "AirPlay_PlugIn::Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
        {
            OH_Orbiter *pOH_Orbiter = (*it).second;
			if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
				continue;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_PlugIn::Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
			bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
			pAirPlayMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
		}
	}

	return MediaHandlerBase::StartMedia(pMediaStream,sError);
 
}

bool AirPlay_PlugIn::StopMedia( class MediaStream *pMediaStream )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"StopMedia Called");
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
 
	map<int, int>::iterator it = m_mapDevicesToStreams.find(pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    if( it!=m_mapDevicesToStreams.end() )
        m_mapDevicesToStreams.erase(it);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "AirPlay::PlugIn::StopMedia() Stopping Media Stream Playback... Pos: %d", pMediaStream->m_iDequeMediaFile_Pos );
 
	AirPlayMediaStream *pAirPlayMediaStream = NULL;
 
	if ((pAirPlayMediaStream = ConvertToAirPlayMediaStream(pMediaStream,"AirPlay::PlugIn::StopMedia():")) == NULL )
		return false;
 
	if (!pAirPlayMediaStream->m_pMediaDevice_Source)
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Stopping media in AirPlay::PlugIn but MediaDevice_Source is NULL");
	    return false;
	  }

	int PK_Device = pAirPlayMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
	int iStreamID = pAirPlayMediaStream->m_iStreamID_get();
	string savedPosition;
 
	DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,
						pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
						pAirPlayMediaStream->m_iStreamID_get(),
						&savedPosition);
 
	if (!SendCommand(CMD_Stop_Media))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"The Target device %d didn't respond to stop media command!", PK_Device);
	  }
	else
	  {
	    // no need to handle bookmarks, I don't think!
	  }
 
	return MediaHandlerBase::StopMedia(pMediaStream);

}
