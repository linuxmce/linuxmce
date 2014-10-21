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
  vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_Xine_Player_CONST);
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
  return NULL;
}

VLCMediaStream *VLC_Plugin::ConvertToVLCMediaStream(MediaStream *pMediaStream, string callerIdMessage)
{
  return NULL;
}

bool VLC_Plugin::StartMedia( MediaStream *pMediaStream,string &sError )
{
  return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool VLC_Plugin::StopMedia( class MediaStream *pMediaStream )
{
  return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *VLC_Plugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
  return NULL;
}

bool VLC_Plugin::MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
  return false; // Let something else intercept it if needed.
}

bool VLC_Plugin::ConfirmSourceIsADestination(string &sMRL,VLCMediaStream *pVLCMediaStream,int PK_Device_Drive)
{
  return true;
}

/*

	COMMANDS TO IMPLEMENT

*/


