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
#include "AirPlay_Streamer.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_MediaType.h"
#include "AirPlay_Protocol_AirPlay.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
AirPlay_Streamer::AirPlay_Streamer(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: AirPlay_Streamer_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
  m_pAirPlay_Service = new AirPlay_Service(this);
  m_bIsPaused=false;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
AirPlay_Streamer::AirPlay_Streamer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: AirPlay_Streamer_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
AirPlay_Streamer::~AirPlay_Streamer()
//<-dceag-dest-e->
{
  delete m_pAirPlay_Service;
  m_pAirPlay_Service = NULL;
  m_bIsPaused=false;
}

//<-dceag-getconfig-b->
bool AirPlay_Streamer::GetConfig()
{
	if( !AirPlay_Streamer_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GETCONFIG! ");

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	m_pDevice_Media_PlugIn = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_Media_Plugin_CONST);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool AirPlay_Streamer::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
AirPlay_Streamer_Command *Create_AirPlay_Streamer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new AirPlay_Streamer(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void AirPlay_Streamer::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void AirPlay_Streamer::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

void AirPlay_Streamer::PostConnect()
{
  Command_Impl::PostConnect();
  EVENT_Playback_Completed ("", 0, false); 
}

void AirPlay_Streamer::OnQuit()
{
  Command_Impl::OnQuit();
}

string AirPlay_Streamer::DeviceNameFromMacAddress(string sMacAddress)
{
  string::size_type pos=0;
  if (sMacAddress.size() == 17)
    {
      string sMac1 = StringUtils::Tokenize(sMacAddress,":",pos);
      string sMac2 = StringUtils::Tokenize(sMacAddress,":",pos);
      string sMac3 = StringUtils::Tokenize(sMacAddress,":",pos);
      string sMac4 = StringUtils::Tokenize(sMacAddress,":",pos);
      string sMac5 = StringUtils::Tokenize(sMacAddress,":",pos);
      string sMac6 = StringUtils::Tokenize(sMacAddress,":",pos);
      return sMac1+sMac2+sMac3+sMac4+sMac5+sMac6;
    }
  else
    {
      // Generic Mac Address foobage.
      return "001122334455";
    }
}

void AirPlay_Streamer::CreateChildren()
{
  Command_Impl::CreateChildren();

  m_pDeviceMD = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_pData->m_dwPK_Device_MD);
  char cName[100];
  sprintf(cName,"%s@%s",DeviceNameFromMacAddress(m_pDeviceMD->GetMacAddress()).c_str(),m_pDeviceMD->m_sDescription.c_str());
  // sprintf(cName,"%s",m_pDeviceMD->m_sDescription.c_str());
  m_pAirPlay_Service->Name_set(cName);
  m_pAirPlay_Service->Name_AirPlay_set(m_pDeviceMD->m_sDescription.c_str());
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Service name is %s",cName);
  m_sCurrentMacAddress = m_pDeviceMD->GetMacAddress();
  if (m_sCurrentMacAddress.empty())
  {
    // This is possibly the core, get the core's MAC address.
    m_pDeviceCore=m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_pData->m_dwPK_Device_Core);
    if (m_pDeviceCore)
      {
	m_sCurrentMacAddress=m_pDeviceCore->GetMacAddress();
      }
  }
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Address is %s",m_sCurrentMacAddress.c_str());
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Service is %s",m_pDeviceMD->m_sDescription.c_str());
  if (!m_pAirPlay_Service->init())
    return;
  
  m_pAirPlay_Service->start();

}

void AirPlay_Streamer::StartAirTunesPlayback()
{
  // This is to avoid the weird protocol inconistency on iOS devices where a dummy AirTunes
  // stream is started, and ultimately not used, when AirPlay is requested. wtf?
  if (!m_bIsPlaying) // seriously, wtf?
    {
      CMD_MH_Play_Media CMD_MH_Play_Media(m_pData->m_dwPK_Device_ControlledVia,
					  m_pDevice_Media_PlugIn->m_dwPK_Device,
					  0,
					  "",
					  MEDIATYPE_lmce_Airplay_audio_CONST,
					  DEVICETEMPLATE_AirPlay_Streamer_CONST,
					  "",
					  false,
					  0,
					  0,
					  0,
					  0);
      SendCommand(CMD_MH_Play_Media);
    }
}

void AirPlay_Streamer::StartAirPlayPhotos(string sLocation)
{
  CMD_MH_Play_Media CMD_MH_Play_Media(m_pData->m_dwPK_Device_ControlledVia,
				      m_pDevice_Media_PlugIn->m_dwPK_Device,
				      0,
				      sLocation,
				      MEDIATYPE_lmce_AirPlay_video_CONST,
				      DEVICETEMPLATE_AirPlay_Streamer_CONST,
				      "",
				      false,
				      0,
				      0,
				      0,
				      0);
  SendCommand(CMD_MH_Play_Media);
  
}


void AirPlay_Streamer::StopAirTunesPlayback()
{
  if (!m_bIsPlaying)
    {
      CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat (m_dwPK_Device,
						   DEVICECATEGORY_Media_Plugins_CONST,
						   false, BL_SameHouse,
						   m_dwPK_Device, 0, 0, "",
						   false);
      SendCommand(CMD_MH_Stop_Media_Cat);
    }
}

void AirPlay_Streamer::StopAirPlayPlayback()
{
  CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat (m_dwPK_Device,
					       DEVICECATEGORY_Media_Plugins_CONST,
					       false, BL_SameHouse,
					       m_dwPK_Device, 0, 0, "",
					       false);
  SendCommand(CMD_MH_Stop_Media_Cat);
  m_bIsPlaying=false;
}

void AirPlay_Streamer::StopAirPlayPhotos()
{
  CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat (m_dwPK_Device,
					       DEVICECATEGORY_Media_Plugins_CONST,
					       false, BL_SameHouse,
					       m_dwPK_Device, 0, 0, "",
					       false);
  SendCommand(CMD_MH_Stop_Media_Cat);

  if (FileUtils::FileExists("/tmp/airplay_photo.jpg"))
    {
      if (!FileUtils::DelFile("/tmp/airplay_photo.jpg"))
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not delete /tmp/airplay_photo.jpg");
	}
    }

  if (FileUtils::FileExists("/tmp/airplay_photo.png"))
    {
      if (!FileUtils::DelFile("/tmp/airplay_photo.png"))
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not delete /tmp/airplay_photo.png");
	}
    }

}



void AirPlay_Streamer::StartAirPlayPlayback(string sLocation, float fPosition)
{
  // TODO: Implement
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Streamer::StartAirPlayPlayback - got a playback request for URL: %s",sLocation.c_str());

  char cPosition[32];
  string sPosition;
  sprintf(cPosition,"%f",fPosition);
  sPosition=cPosition;

  sLocation += "}" + sPosition;

  CMD_MH_Play_Media CMD_MH_Play_Media(m_pData->m_dwPK_Device_ControlledVia,
				      m_pDevice_Media_PlugIn->m_dwPK_Device,
				      0,
				      sLocation,
				      MEDIATYPE_lmce_AirPlay_video_CONST,
				      DEVICETEMPLATE_AirPlay_Streamer_CONST,
				      "",
				      false,
				      0,
				      0,
				      0,
				      0);
  SendCommand(CMD_MH_Play_Media);
  m_bIsPlaying=true;
}

/**
 * Accessor, is paused.
 */
bool AirPlay_Streamer::IsPaused()
{
  return m_bIsPaused;
}

void AirPlay_Streamer::SetPause(bool bIsPaused)
{
  m_bIsPaused=bIsPaused;
}

/**
 * Accessor, is playing.
 */
bool AirPlay_Streamer::IsPlaying()
{
  return m_bIsPlaying;
}

void AirPlay_Streamer::SetPlaying(bool bIsPlaying)
{
  m_bIsPlaying=bIsPlaying;
}

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void AirPlay_Streamer::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	cout << "Need to implement command #28 - Simulate Keypress" << endl;
	cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c32-b->

	/** @brief COMMAND: #32 - Update Object Image */
	/** Display an image on the media player */
		/** @param #3 PK_DesignObj */
			/** The object in which to put the bitmap */
		/** @param #14 Type */
			/** 1=bmp, 2=jpg, 3=png */
		/** @param #19 Data */
			/** The contents of the bitmap, like reading from the file into a memory buffer */
		/** @param #23 Disable Aspect Lock */
			/** If 1, the image will be stretched to fit the object */

void AirPlay_Streamer::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
	cout << "Need to implement command #32 - Update Object Image" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #14 - Type=" << sType << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
}

//<-dceag-c37-b->

	/** @brief COMMAND: #37 - Play Media */
	/** This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** The media that we need to play. */
		/** @param #42 MediaPosition */
			/** The position at which we need to start playing. */
		/** @param #59 MediaURL */
			/** The file to play, or other media id.  The format is specific on the media type and the media player. */

void AirPlay_Streamer::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

	m_iPK_MediaType = iPK_MediaType;

	if (iPK_MediaType == 60)
	  {
	    // Ignore
	    return;
	  }

	AirPlay_Protocol_AirPlay* pAirPlay_Protocol_AirPlay = (AirPlay_Protocol_AirPlay *)m_pAirPlay_Service->m_pAirPlay_Protocol_Factory->getAirPlayProtocolForMediaType(iPK_MediaType);

	if (!pAirPlay_Protocol_AirPlay)
	  {
	    return;
	  }

	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->PlayURL(sMediaURL);
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->SetPosition(atof(sMediaPosition.c_str()));

}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void AirPlay_Streamer::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

	if (m_iPK_MediaType == 60)
	  {
	    return; // ignore
	  }

	AirPlay_Protocol_AirPlay* pAirPlay_Protocol_AirPlay = (AirPlay_Protocol_AirPlay *)m_pAirPlay_Service->m_pAirPlay_Protocol_Factory->getAirPlayProtocolForMediaType(m_iPK_MediaType);

	if (!pAirPlay_Protocol_AirPlay)
	  {
	    return;
	  }
	
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->Stop();

	
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void AirPlay_Streamer::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	cout << "Need to implement command #39 - Pause Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	
	if (m_iPK_MediaType == 60)
	  {
	    return; // ignore
	  }
	
	AirPlay_Protocol_AirPlay* pAirPlay_Protocol_AirPlay = (AirPlay_Protocol_AirPlay *)m_pAirPlay_Service->m_pAirPlay_Protocol_Factory->getAirPlayProtocolForMediaType(m_iPK_MediaType);
	
	if (!pAirPlay_Protocol_AirPlay)
	  {
	    return;
	  }
	SetPause(true);
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->Pause();
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void AirPlay_Streamer::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	cout << "Need to implement command #40 - Restart Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	if (m_iPK_MediaType == 60)
	  {
	    return; // ignore
	  }
	
	AirPlay_Protocol_AirPlay* pAirPlay_Protocol_AirPlay = (AirPlay_Protocol_AirPlay *)m_pAirPlay_Service->m_pAirPlay_Protocol_Factory->getAirPlayProtocolForMediaType(m_iPK_MediaType);
	
	if (!pAirPlay_Protocol_AirPlay)
	  {
	    return;
	  }
	SetPause(false);
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->Restart();

}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
		/** @param #220 Report */
			/** If true, report this speed to the user on the OSD */

void AirPlay_Streamer::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
	cout << "Parm #220 - Report=" << bReport << endl;

	float fMediaPlayBackSpeed = iMediaPlaybackSpeed / 1000;

	if (m_iPK_MediaType == 60)
	  {
	    return; // ignore
	  }
	
	AirPlay_Protocol_AirPlay* pAirPlay_Protocol_AirPlay = (AirPlay_Protocol_AirPlay *)m_pAirPlay_Service->m_pAirPlay_Protocol_Factory->getAirPlayProtocolForMediaType(m_iPK_MediaType);
	
	if (!pAirPlay_Protocol_AirPlay)
	  {
	    return;
	  }
	SetPause(false);
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->Restart();
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->SetRate(fMediaPlayBackSpeed);

}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void AirPlay_Streamer::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
	cout << "Need to implement command #42 - Jump to Position in Stream" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void AirPlay_Streamer::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	
	if (m_iPK_MediaType == 60)
	  {
	    return; // ignore
	  }
	
	AirPlay_Protocol_AirPlay* pAirPlay_Protocol_AirPlay = (AirPlay_Protocol_AirPlay *)m_pAirPlay_Service->m_pAirPlay_Protocol_Factory->getAirPlayProtocolForMediaType(m_iPK_MediaType);
	
	if (!pAirPlay_Protocol_AirPlay)
	  {
	    return;
	  }
	SetPause(false);
	pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->Restart();
	if (sValue_To_Assign[0] == '+')
	  {
	    string sMult = sValue_To_Assign.substr(1,string::npos);
	    int iMult = atoi(sMult.c_str());
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Jumping Forward %d*30 secs.",iMult);
	    pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->JumpFwd(iMult);
	  }
	else if (sValue_To_Assign[0] == '-')
	  {
	    string sMult = sValue_To_Assign.substr(1,string::npos);
	    int iMult = atoi(sMult.c_str());
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Jumping backward %d*30 secs.",iMult);
	    pAirPlay_Protocol_AirPlay->m_pAirPlay_Videolan->JumpBack(iMult);
	  }
	else 
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Absolute Jump to Position in Stream not supported for AirPlay Streams.");
	  }

}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void AirPlay_Streamer::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c249-b->

	/** @brief COMMAND: #249 - Start Streaming */
	/** Like play media, but it means the destination device is not the same as the source */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #41 StreamID */
			/** Identifier for this streaming session. */
		/** @param #42 MediaPosition */
			/** Where to start playing from */
		/** @param #59 MediaURL */
			/** The url to use to play this stream. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void AirPlay_Streamer::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	cout << "Need to implement command #249 - Start Streaming" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;
}

//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

void AirPlay_Streamer::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
}

//<-dceag-c262-b->

	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void AirPlay_Streamer::CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c262-e->
{
	cout << "Need to implement command #262 - Stop Streaming" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void AirPlay_Streamer::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	cout << "Need to implement command #412 - Set Media Position" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void AirPlay_Streamer::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
	cout << "Need to implement command #548 - Menu" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void AirPlay_Streamer::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
	cout << "Need to implement command #916 - Set Aspect Ratio" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #260 - Aspect_Ratio=" << sAspect_Ratio << endl;
}

//<-dceag-c917-b->

	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

void AirPlay_Streamer::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
	cout << "Need to implement command #917 - Set Zoom" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #261 - Zoom_Level=" << sZoom_Level << endl;
}

//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void AirPlay_Streamer::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	cout << "Need to implement command #920 - Set Media ID" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}


