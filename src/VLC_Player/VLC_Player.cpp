/*
  Copyright (C) 2014 LinuxMCE
  
  www.linuxmce.org
  
  
  This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  See the GNU General Public License for more details.
  
*/
//<-dceag-d-b->
#include "VLC_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_Button.h"
#include "../Xine_Player/XineMediaInfo.h"
#include "Position.h"

void * SpawnTimecodeReportingThread(void * Arg)
{
  DCE::VLC_Player *pVLC_Player = (DCE::VLC_Player *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pVLC_Player->TimecodeReportingLoop();
  return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VLC_Player::VLC_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
  : VLC_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
    //<-dceag-const-e->
  ,m_VLCMutex("vlc_player")
{
  m_config=new VLC::Config();
  m_pVLC=NULL;
  m_bTimecodeReporting=false;
  m_pAlarmManager=NULL;
  m_VLCMutex.Init(NULL);
  m_pNotificationSocket = new XineNotification_SocketListener(string("m_pNotificationSocket"));
  m_pNotificationSocket->m_bSendOnlySocket = true; // one second

}

//<-dceag-dest-b->
VLC_Player::~VLC_Player()
//<-dceag-dest-e->
{
  EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not
  
  m_iMediaPlaybackSpeed=0;

  UnmountRemoteDVD();
  UnmountLocalBD();

  if (m_config)
    {
      delete m_config;
      m_config=NULL;
    }
  
  if (m_pVLC)
    {
      delete m_pVLC;
      m_pVLC=NULL;
    }

  if (m_pAlarmManager)
    {
      m_pAlarmManager->Stop();
      delete m_pAlarmManager;
      m_pAlarmManager=NULL;
    }
  
}

bool VLC_Player::Connect(int iPK_DeviceTemplate)
{
  if (!Command_Impl::Connect(iPK_DeviceTemplate))
    return false;

  DeviceData_Base *pDevice = m_pData->GetTopMostDevice();
  m_sIPofMD = pDevice->m_sIPAddress;
  int iPort = DATA_Get_Port();
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Configured port for time/speed notification is: %i, IP is %s", iPort, m_sIPofMD.c_str());
  m_pNotificationSocket->StartListening (iPort);
  EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not

  return true;
}

string VLC_Player::MD_DeviceData_get(int iFK_DeviceData)
{
  return m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_MD,iFK_DeviceData);
}

string VLC_Player::Soundcard_get()
{
  return MD_DeviceData_get(DEVICEDATA_Sound_Card_CONST);
}

string VLC_Player::Audiosettings_get()
{
  return MD_DeviceData_get(DEVICEDATA_Audio_settings_CONST);
}

string VLC_Player::Videosettings_get()
{
  return MD_DeviceData_get(DEVICEDATA_Video_settings_CONST);
}

//<-dceag-getconfig-b->
bool VLC_Player::GetConfig()
{
  if( !VLC_Player_Command::GetConfig() )
    return false;
  //<-dceag-getconfig-e->
  
  if (!m_config)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"m_config = NULL while inside GetConfig(). Bailing.");
      return false;
    }
  else
    {
      m_config->m_sSoundCard=Soundcard_get();
      m_config->m_sAudioSettings=Audiosettings_get();
      m_config->m_sHardwareAcceleration=DATA_Get_Hardware_acceleration();
      m_config->m_sDeinterlacingMode=DATA_Get_Deinterlacing_Mode();
      m_config->m_sVideoSettings=Videosettings_get();

    }
  
  m_pVLC=new VLC(m_config,this);
  
  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not instantiate VLC with configuration data. Bailing.");
      return false;
    }
  
  if (!m_pVLC->init())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not initialize VLC. Bailing.");
      return false;
    }

  m_pAlarmManager=new AlarmManager();
  m_pAlarmManager->Start(1); // Change this to more workers?

  // At this point, VLC is running.
  
  return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VLC_Player::Register()
//<-dceag-reg-e->
{
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
void VLC_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VLC_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
  sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

void VLC_Player::AlarmCallback(int id, void* param)
{
  PLUTO_SAFETY_LOCK(vm,m_VLCMutex);

  switch (id)
    {
    case 1:
      // temporary hack.
      if (m_pVLC->IsPlaying())
      {
	m_pAlarmManager->CancelAlarmByType(1);
	m_pVLC->UpdateStatus();
	m_pAlarmManager->AddRelativeAlarm(1,this,1,NULL);
      }
      else
	{
	  m_pAlarmManager->CancelAlarmByType(1);
	}
      break;
    case 2:
      // Transport Control altered.
      if (m_pVLC->IsPlaying())
	{
	  DoTransportControls();
	}
      else
	{
	  m_pAlarmManager->CancelAlarmByType(2);
	}
    }
}

void VLC_Player::ReportTimecodeViaIP(int iStreamID, int Speed)
{  
  // filling media info structure
  XineMediaInfo mediaInfo;
  
  mediaInfo.m_iSpeed = m_iMediaPlaybackSpeed;
  mediaInfo.m_iPositionInMilliseconds = (int)m_pVLC->GetTime();
  mediaInfo.m_iTotalLengthInMilliseconds = (int)m_pVLC->GetCurrentDuration();
  
  mediaInfo.m_iStreamID = m_pVLC->GetStreamID();
  mediaInfo.m_iTitle = m_pVLC->GetCurrentTitle();
  mediaInfo.m_iChapter = m_pVLC->GetCurrentChapter();
  mediaInfo.m_sFileName = m_pVLC->GetMediaURL();
  mediaInfo.m_sMediaType = m_pVLC->GetMediaType();
  mediaInfo.m_iMediaID = m_pVLC->GetMediaID();
  mediaInfo.m_iAngle = m_pVLC->GetCurrentAngle();
  mediaInfo.m_iAudio = m_pVLC->GetAudioTrack();
  mediaInfo.m_iSubtitle = m_pVLC->GetSubtitle();
  string sIPTimeCodeInfo = mediaInfo.ToString();
  
  // LoggerWrapper::GetInstance()->Write(LV_STATUS,"reporting timecode stream %d speed %d %s", iStreamID, Speed, sIPTimeCodeInfo.c_str() );
  // EVENT_Media_Position_Changed(atoi(mediaInfo.m_sMediaType.c_str()), mediaInfo.m_sFileName, StringUtils::itos(mediaInfo.m_iMediaID), iStreamID, mediaInfo.FormatTotalTime(), mediaInfo.FormatCurrentTime(), Speed);
  
  m_pNotificationSocket->SendStringToAll( sIPTimeCodeInfo );
}

void VLC_Player::StartTimecodeReporting()
{
  if (m_bTimecodeReporting)
    return; // Already running. There is no need to start another one.

  if (pthread_create(&m_timecodeThread, NULL, SpawnTimecodeReportingThread, (void *)this))
    {
      // Failed.
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_PlayeR::StartTimecodeReporting - Failed to spawn timecode thread.");
      return;
    }
  m_bTimecodeReporting=true;
  usleep(10000);
  return;
}

void VLC_Player::StopTimecodeReporting()
{
  if (m_bTimecodeReporting && m_timecodeThread)
    {
      m_bTimecodeReporting=false;
      pthread_join(m_timecodeThread,NULL);
    }
}

void VLC_Player::TimecodeReportingLoop()
{
  while (m_bTimecodeReporting)
    {
      ReportTimecodeViaIP(m_pVLC->GetStreamID(),m_iMediaPlaybackSpeed);
      m_pVLC->UpdateTracks();
      Sleep(DATA_Get_Time_Code_Report_Frequency()*1000);
    }
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

void VLC_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
  cout << "Need to implement command #28 - Simulate Keypress" << endl;
  cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #50 - Name=" << sName << endl;
  
  int PK_Button = atoi(sPK_Button.c_str());
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Player::CMD_Simulate_Keypress %d",PK_Button);
  switch(PK_Button)
    {
    case BUTTON_Up_Arrow_CONST:
      CMD_Move_Up(iStreamID);
      break;
    case BUTTON_Down_Arrow_CONST:
      CMD_Move_Down(iStreamID);
      break;
    case BUTTON_Left_Arrow_CONST:
      CMD_Move_Left(iStreamID);
      break;
    case BUTTON_Right_Arrow_CONST:
      CMD_Move_Right(iStreamID);
      break;
    case BUTTON_Enter_CONST:
      CMD_EnterGo(iStreamID);
      break;
    default:
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Xine_Player::CMD_Simulate_Keypress -- Can't handle %d",PK_Button);
      break;
    }	
}

//<-dceag-c29-b->

/** @brief COMMAND: #29 - Simulate Mouse Click */
/** Simlate a mouse click at a certain position on the screen */
/** @param #11 Position X */
/** position X */
/** @param #12 Position Y */
/** position Y */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
  cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
  cout << "Parm #11 - Position_X=" << iPosition_X << endl;
  cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
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

void VLC_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
  cout << "Need to implement command #32 - Update Object Image" << endl;
  cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
  cout << "Parm #14 - Type=" << sType << endl;
  cout << "Parm #19 - Data  (data value)" << endl;
  cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
}

/**
 * Did this URL get passed from CMD_Start_Streaming? 
 */
bool VLC_Player::MediaURLIsStreaming(string sMediaURL)
{
  return sMediaURL.find("#sync") != string::npos;
}

/**
 * Strip the #sync tag from the URL, because we've detected it, and no longer need it.
 */
string VLC_Player::StripSyncTagFromURL(string sMediaURL)
{
  return sMediaURL.substr(0,sMediaURL.size()-5); // #sync
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

void VLC_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
  cout << "Need to implement command #37 - Play Media" << endl;
  cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
  cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

  string sMediaInfo;

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::CMD_Play_Media() called with iPK_MediaType %d, iStreamID %d, sMediaPosition %s, sMediaURL %s",
				      iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str());
  
  // Error out if VLC isn't initialized for some reason...
  if (!m_pVLC)
    {
      sCMD_Result="ERROR";
    }

  if (IsRemoteDVD(sMediaURL))
    {
      MountRemoteDVD(sMediaURL);
    }
  else if (IsLocalBD(sMediaURL))
    {
      MountLocalBD(sMediaURL);
    }

  m_iPK_MediaType = iPK_MediaType;

  if (MediaURLIsStreaming(sMediaURL))
    {
      sMediaURL = StripSyncTagFromURL(sMediaURL);
    }

  if (m_pVLC->PlayURL(sMediaURL,iStreamID,sMediaPosition,sMediaInfo))
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::EVENT_Playback_Started(streamID=%i)",iStreamID);
      EVENT_Playback_Started(sMediaURL,iStreamID,sMediaInfo,m_pVLC->m_sAudioInfo,m_pVLC->m_sVideoInfo);
      m_iMediaPlaybackSpeed=1000;
    }
  else
    {
      UnmountRemoteDVD();
      UnmountLocalBD();
      sCMD_Result="ERROR";
      EVENT_Playback_Completed(sMediaURL,iStreamID,true);
    }

}

//<-dceag-c38-b->

/** @brief COMMAND: #38 - Stop Media */
/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
/** @param #41 StreamID */
/** The media needing to be stopped. */
/** @param #42 MediaPosition */
/** The position at which this stream was last played. */

void VLC_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
  cout << "Need to implement command #38 - Stop Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
  
  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Stop_Media(iStreamID=%i) - m_pVLC == NULL",iStreamID);
      sCMD_Result="ERROR";
      return;
    }
  
  Position pPosition = Position(m_pVLC->GetCurrentTitle(),
				m_pVLC->GetCurrentChapter(),
				m_pVLC->GetTime(),
				m_pVLC->GetSubtitle(),
				m_pVLC->GetAudioTrack(),
				m_pVLC->GetCurrentDuration());
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Stop Position::toString(%s)",pPosition.toString().c_str());

  *sMediaPosition=pPosition.toString();

  // Preliminary, needs to be amended to close multiple streams
  m_pVLC->Stop();
  m_iMediaPlaybackSpeed=0;
  if (!m_pVLC->IsPlaying())
    m_pAlarmManager->CancelAlarmByType(1);

  StopTimecodeReporting();

  UnmountRemoteDVD();
  UnmountLocalBD();
  m_pVLC->SetPlaying(false); // This must be done after position query, otherwise we get incorrect data.

  // Tell other targets to stop.
  if (!m_sStreamingTargets.empty())
    {
      string::size_type pos=0;
      string curTarget = StringUtils::Tokenize(m_sStreamingTargets,string(","),pos);
      while (curTarget!="")
	{
	  int targetDevice = atoi(curTarget.c_str());
	  DCE::CMD_Stop_Media cmd(m_dwPK_Device, targetDevice, iStreamID,sMediaPosition);
	  SendCommandNoResponse(cmd);
	  curTarget = StringUtils::Tokenize(m_sStreamingTargets,string(","),pos);
	}
      
    }

  m_sStreamingTargets.clear();

}

//<-dceag-c39-b->

/** @brief COMMAND: #39 - Pause Media */
/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
/** @param #41 StreamID */
/** The media stream for which we need to pause playback. */

void VLC_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
  cout << "Need to implement command #39 - Pause Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Pause_Media(iStreamId=%d) no VLC object! Bailing!",
					  iStreamID);
      sCMD_Result="ERROR";
      return;
    }

  if (!m_pVLC->IsPlaying())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Pause_Media(iStreamId=%d) nothing playing.",
					  iStreamID);
      sCMD_Result="ERROR";
      return;      
    }

  m_pVLC->SetRate(0.0);
  m_pVLC->Pause();
  m_iMediaPlaybackSpeed=0;

}

//<-dceag-c40-b->

/** @brief COMMAND: #40 - Restart Media */
/** This will restart a media was paused with the above command */
/** @param #41 StreamID */
/** The media stream that we need to restart playback for. */

void VLC_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
  cout << "Need to implement command #40 - Restart Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Restart_Media(iStreamId=%d) no VLC object! Bailing!",
					  iStreamID);
      sCMD_Result="ERROR";
      return;
    }

  if (!m_pVLC->IsPlaying())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Restart_Media(iStreamId=%d) nothing playing.",
					  iStreamID);
      sCMD_Result="ERROR";
      return;      
    }

  m_pVLC->SetRate(1.0);
  m_pVLC->Restart();
  m_iMediaPlaybackSpeed=1000;
}

/**
 * DoTransportControls - Transport control in VLC_Player is represented by a series of libvlc_media_player_set_time() calls, which
 * call this function from an alarm. The frequency of the alarm is determined by the requested media speed, e.g. 2x means two jumps by 2000ms
 * every second, 4x means four jumps by 4000ms every second, and so on. 
 */
void VLC_Player::DoTransportControls()
{

  int64_t iSleepValue=0, iNewTimeVal=0;
  string sOSDStatus, sSpeed, sDirection;
  m_pAlarmManager->CancelAlarmByType(2);

  if (!m_pVLC->IsPlaying())
    {
      // Media is no longer playing, most likely we fell off the end. Just return.
      return;
    }
  
  if ((m_iMediaPlaybackSpeed == 1000))
    {
      m_pVLC->SetRate(1.0);
      m_pVLC->Restart();
      return;
    }
  
  /* Any positive speed greater than 0; less than 8000, use a set rate */
  if ((m_iMediaPlaybackSpeed < 8000) && (m_iMediaPlaybackSpeed > 0))
    {
      m_pVLC->Restart();
      m_pVLC->SetRate((float)m_iMediaPlaybackSpeed/1000);
      return;
    }

  switch (abs(m_iMediaPlaybackSpeed))
    {
    case 250:
    case 500:
    case 1000:
    case 2000:
    case 3000:
    case 4000:
    case 6000:
    case 8000:
    case 10000:
    case 15000:
    case 16000:
    case 20000:
    case 30000:
    case 32000:
    case 50000:
    case 64000:
    case 100000:
    case 200000:
    case 400000:
    default:
      iSleepValue=125;
      break;
    }
  

  // for (i=0;i<iNumSleeps;++i)
  //   {
  //     m_pVLC->Pause();
  //     if (m_iMediaPlaybackSpeed < 0)
  //     	{
  // 	  iNewTimeVal = (m_pVLC->GetTime())-abs(m_iMediaPlaybackSpeed);
  // 	  if (iNewTimeVal <= 0)
  // 	    iNewTimeVal = 0;
  //     	  m_pVLC->SetTime(iNewTimeVal);
  //     	}
  //     else
  //     	{
  // 	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Going forward by %d - new time is %d",m_iMediaPlaybackSpeed,m_pVLC->GetTime()+m_iMediaPlaybackSpeed);
  // 	  iNewTimeVal = (m_pVLC->GetTime())+abs(m_iMediaPlaybackSpeed);
  // 	  if (iNewTimeVal >= m_pVLC->GetCurrentDuration())
  // 	    iNewTimeVal = m_pVLC->GetDuration();
  //     	  m_pVLC->SetTime(iNewTimeVal);	
  //     	}
  //     Sleep(iSleepValue);
  //   }

  m_pVLC->Pause();
  if (m_iMediaPlaybackSpeed < 0)
    {
      iNewTimeVal = (m_pVLC->GetTime())-abs(m_iMediaPlaybackSpeed);
      if (iNewTimeVal <= 0)
	iNewTimeVal = 0;
      m_pVLC->SetTime(iNewTimeVal);
    }
  else
    {
      iNewTimeVal = (m_pVLC->GetTime())+abs(m_iMediaPlaybackSpeed);
      if (iNewTimeVal >= m_pVLC->GetCurrentDuration())
	iNewTimeVal = m_pVLC->GetDuration();
      m_pVLC->SetTime(iNewTimeVal);
    }

  // Generate OSD String
  if (m_iMediaPlaybackSpeed == 0)
    {
      sDirection = "||";
    }
  else if (m_iMediaPlaybackSpeed == 1000)
    {
      sDirection = "|>";
    }
  else if (m_iMediaPlaybackSpeed < 0)
    {
      sDirection = "<<";
    }
  else if (m_iMediaPlaybackSpeed > 0)
    {
      sDirection = ">>";
    }

  sSpeed = StringUtils::itos(m_iMediaPlaybackSpeed / 1000) + "x";

  sOSDStatus = sDirection + "    " + sSpeed + "   " + m_pVLC->OSD_Time(iNewTimeVal) + " / " + m_pVLC->OSD_Time(m_pVLC->GetCurrentDuration());

  // Update OSD
  m_pVLC->OSD_Status(sOSDStatus);



  Sleep(iSleepValue);

  m_pAlarmManager->AddRelativeAlarm(0,this,2,NULL);

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

void VLC_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
  
  cout << "Need to implement command #41 - Change Playback Speed" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
  cout << "Parm #220 - Report=" << bReport << endl;

  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Change_Playback_Speed(iStreamId=%d,iMediaPlaybackSpeed=%d,bReport=%d) no VLC object! Bailing!",
					  iStreamID,iMediaPlaybackSpeed,bReport);
      sCMD_Result="ERROR";
      return;
    }

  m_iMediaPlaybackSpeed=iMediaPlaybackSpeed;

  if (iMediaPlaybackSpeed==0)
    {
      m_pVLC->SetRate(0.0);
      m_pVLC->Pause();
    }
  else if (iMediaPlaybackSpeed!=0)
    {
      m_pVLC->Restart();
      DoTransportControls();
    }

}

//<-dceag-c42-b->

/** @brief COMMAND: #42 - Jump to Position in Stream */
/** Jump to a position in the stream, specified in seconds. */
/** @param #5 Value To Assign */
/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
/** @param #41 StreamID */
/** The stream */

void VLC_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
  cout << "Need to implement command #42 - Jump to Position in Stream" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  char firstChar=sValue_To_Assign[0];
  int64_t newTime=0;
  
  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Jump_to_Position_in_Stream(sValue_To_Assign=%s,iStreamID=%i) - m_pVLC == NULL",sValue_To_Assign.c_str(),iStreamID);
      sCMD_Result="ERROR";
      return;
    }
  
  if (firstChar=='+')
    {
      int64_t currentTime = m_pVLC->GetTime();
      newTime=atoi(sValue_To_Assign.substr(1).c_str())*1000;
      m_pVLC->SetTime(currentTime+newTime);
    }
  else if (firstChar=='-')
    {
      int64_t currentTime = m_pVLC->GetTime();
      newTime=atoi(sValue_To_Assign.c_str());
      m_pVLC->SetTime((currentTime+newTime)*1000);
    }
  else if (firstChar=='T')
    {
      string sNewTime = sValue_To_Assign.substr(1);
      m_pVLC->SetTime(atoi(sNewTime.c_str()));
    }
  else
    {
      m_pVLC->SetTime(atoi(sValue_To_Assign.c_str())*1000);
    }
  
}

//<-dceag-c63-b->

/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
  cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID=%i) - m_pVLC == NULL",iStreamID);
      sCMD_Result="ERROR";
      return;
    }

  m_pVLC->NextChapter();

}

//<-dceag-c64-b->

/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
  cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Skip_Back_ChannelTrack_Lower(iStreamID=%i) - m_pVLC == NULL",iStreamID);
      sCMD_Result="ERROR";
      return;
    }

  m_pVLC->PreviousChapter();

}

//<-dceag-c65-b->

/** @brief COMMAND: #65 - Jump Position In Playlist */
/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
/** @param #5 Value To Assign */
/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
  cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  if (sValue_To_Assign.size()==0)
    return; // do nothing.
  
  if (!m_pVLC)
    return; // do nothing.
  
  // Resume playback at normal speed, if needed.
  if (m_iMediaPlaybackSpeed!=1000)
    {
      m_iMediaPlaybackSpeed=1000; // Play at normal speed.
      DoTransportControls();
    }
  
  if (m_pVLC->hasChapters())
    {
      // Skip forward/backward chapters.

      int iCurrentChapter = m_pVLC->GetCurrentChapter();
      int iNewChapter=0;
      int iChapterOffset=0;

      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXX - Current chapter is %d",iCurrentChapter);

      if (sValue_To_Assign.find("+") != string::npos)
	{
	  // Skip forward X chapters
	  iChapterOffset=atoi(sValue_To_Assign.substr(1).c_str());
	  iNewChapter = iCurrentChapter + iChapterOffset;
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Jump_Position_in_Playlist() - Plus value %d %d.",iChapterOffset,iNewChapter);
	}
      else if (sValue_To_Assign.find("-") != string::npos)
	{
	  // Skip backward X chapters
	  iChapterOffset=atoi(sValue_To_Assign.substr(1).c_str());
	  iNewChapter = iCurrentChapter - iChapterOffset;
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Jump_Position_in_Playlist() - Minus value %d %d.",iChapterOffset,iNewChapter);
	}
      else 
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Jump_Position_in_Playlist() - Absolute value.");
	  iNewChapter = atoi(sValue_To_Assign.c_str());
	}

      if (iNewChapter!=0 && iNewChapter <= m_pVLC->numChapters())
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::CMD_Jump_Position_in_Playlist(%d) - setting new chapter.",iNewChapter);
	  m_pVLC->SetChapter(iNewChapter);
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::CMD_Jump_Position_in_Playlist(%d) - invalid chapter number or offset, ignoring.",iNewChapter);
	}

    }
  else
    {
      // Non-chapter behavior. Jump forward/backward in increments of 30sec.
      if (sValue_To_Assign[0] == '-')
	{
	  m_pVLC->JumpBack(atoi(sValue_To_Assign.substr(1).c_str()));
	}
      else if (sValue_To_Assign[0] == '+')
	{
	  m_pVLC->JumpFwd(atoi(sValue_To_Assign.substr(1).c_str()));
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::CMD_Jump_Position_in_Playlist(%d) - trying to jump chapters in media with no chapters. Ignoring.");
	}
    }

}
//<-dceag-c81-b->

/** @brief COMMAND: #81 - Navigate Next */
/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
/** @param #41 StreamID */
/** The stream on which to do the navigation. */

void VLC_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
  cout << "Need to implement command #81 - Navigate Next" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c82-b->

/** @brief COMMAND: #82 - Navigate Prev */
/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
/** @param #41 StreamID */
/** The stream on which to do the navigation. */

void VLC_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
  cout << "Need to implement command #82 - Navigate Prev" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c84-b->

/** @brief COMMAND: #84 - Get Video Frame */
/** Get's the current video frame from the media player. */
/** @param #19 Data */
/** The video frame */
/** @param #20 Format */
/** Format of the frame */
/** @param #23 Disable Aspect Lock */
/** Disable Aspect Ratio */
/** @param #41 StreamID */
/** The ID of the stream */
/** @param #60 Width */
/** Frame width */
/** @param #61 Height */
/** Frame height */

void VLC_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
  cout << "Need to implement command #84 - Get Video Frame" << endl;
  cout << "Parm #19 - Data  (data value)" << endl;
  cout << "Parm #20 - Format=" << sFormat << endl;
  cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #60 - Width=" << iWidth << endl;
  cout << "Parm #61 - Height=" << iHeight << endl;

  size_t size=0;

  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::CMD_Get_Video_Frame(sDisable_Aspect_Lock=%s,iStreamID=%d,iWidth=%d,iHeight=%d,iData_Size=%d) - m_pVLC == NULL",sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,iData_Size);
      sCMD_Result="ERROR";
      return;
    }
  
  if (!m_pVLC->GetScreenShot(iWidth,iHeight,sCMD_Result))
    {
      return;
    }

  *pData = FileUtils::ReadFileIntoBuffer("/tmp/shot.png",size);
  *iData_Size = size;
  FileUtils::DelFile("/tmp/shot.png");
  return;

}

//<-dceag-c87-b->

/** @brief COMMAND: #87 - Goto Media Menu */
/** Goto to the current media Root Menu. */
/** @param #41 StreamID */
/** The stream ID */
/** @param #64 MenuType */
/** The type of menu that the user want to jump to.
    (For DVD handlers usually this applies)
    0 - Root menu 
    1 - Title menu
    2 - Media menu */

void VLC_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
  cout << "Need to implement command #87 - Goto Media Menu" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #64 - MenuType=" << iMenuType << endl;

  if (!m_pVLC)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC_Player::Goto_Media_Menu(iStreamID=%i,iMenuType=%i) - no VLC object! Bailing!");
      sCMD_Result="ERROR";
      return;
    }

  m_pVLC->GotoMediaMenu(iMenuType);

}

//<-dceag-c92-b->

/** @brief COMMAND: #92 - Pause */
/** Pause the media */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
  cout << "Need to implement command #92 - Pause" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  CMD_Pause_Media(0);
}

//<-dceag-c95-b->

/** @brief COMMAND: #95 - Stop */
/** Stop the media */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #203 Eject */
/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void VLC_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
  cout << "Need to implement command #95 - Stop" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #203 - Eject=" << bEject << endl;
  string sMediaPosition;
  CMD_Stop_Media(0,&sMediaPosition);
}

//<-dceag-c126-b->

/** @brief COMMAND: #126 - Guide */
/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void VLC_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
  cout << "Need to implement command #126 - Guide" << endl;
  CMD_Goto_Media_Menu(0,0);
}

//<-dceag-c139-b->

/** @brief COMMAND: #139 - Play */
/** Play the media */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
  cout << "Need to implement command #139 - Play" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  CMD_Play_Media(0,1,"","");
}

//<-dceag-c140-b->

/** @brief COMMAND: #140 - Audio Track */
/** Go to an audio track */
/** @param #5 Value To Assign */
/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c140-e->
{
  cout << "Need to implement command #140 - Audio Track" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  if (sValue_To_Assign=="")
    {
      m_pVLC->SetAudioTrack(m_pVLC->GetAudioTrack()+1);
    }
  else
    {
      m_pVLC->SetAudioTrack(atoi(sValue_To_Assign.c_str()));
    }

  m_pVLC->SetPreviousAudioTrack(m_pVLC->GetVLCAudioTrack());

}

//<-dceag-c141-b->

/** @brief COMMAND: #141 - Subtitle */
/** Go to a subtitle */
/** @param #5 Value To Assign */
/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c141-e->
{
  cout << "Need to implement command #141 - Subtitle" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  if (sValue_To_Assign=="")
    {
      m_pVLC->SetSubtitle(m_pVLC->GetSubtitle()+1);
    }
  else
    {
      m_pVLC->SetSubtitle(atoi(sValue_To_Assign.c_str()));
    }  

}

//<-dceag-c142-b->

/** @brief COMMAND: #142 - Angle */
/** Go to an angle */
/** @param #5 Value To Assign */
/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c142-e->
{
  cout << "Need to implement command #142 - Angle" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c190-b->

/** @brief COMMAND: #190 - Enter/Go */
/** Select the currently highlighted menu item */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
  cout << "Need to implement command #190 - Enter/Go" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pVLC->EnterGo();
}

//<-dceag-c200-b->

/** @brief COMMAND: #200 - Move Up */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
  cout << "Need to implement command #200 - Move Up" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pVLC->MoveUp();
}

//<-dceag-c201-b->

/** @brief COMMAND: #201 - Move Down */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
  cout << "Need to implement command #201 - Move Down" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pVLC->MoveDown();
}

//<-dceag-c202-b->

/** @brief COMMAND: #202 - Move Left */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
  cout << "Need to implement command #202 - Move Left" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pVLC->MoveLeft();
}

//<-dceag-c203-b->

/** @brief COMMAND: #203 - Move Right */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
  cout << "Need to implement command #203 - Move Right" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pVLC->MoveRight();
}

//<-dceag-c204-b->

/** @brief COMMAND: #204 - 0 */
/** 0 */

void VLC_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
  cout << "Need to implement command #204 - 0" << endl;
}

//<-dceag-c205-b->

/** @brief COMMAND: #205 - 1 */
/** 1 */

void VLC_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
  cout << "Need to implement command #205 - 1" << endl;
}

//<-dceag-c206-b->

/** @brief COMMAND: #206 - 2 */
/** 2 */

void VLC_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
  cout << "Need to implement command #206 - 2" << endl;
}

//<-dceag-c207-b->

/** @brief COMMAND: #207 - 3 */
/** 3 */

void VLC_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
  cout << "Need to implement command #207 - 3" << endl;
}

//<-dceag-c208-b->

/** @brief COMMAND: #208 - 4 */
/** 4 */

void VLC_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
  cout << "Need to implement command #208 - 4" << endl;
}

//<-dceag-c209-b->

/** @brief COMMAND: #209 - 5 */
/** 5 */

void VLC_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
  cout << "Need to implement command #209 - 5" << endl;
}

//<-dceag-c210-b->

/** @brief COMMAND: #210 - 6 */
/** 6 */

void VLC_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
  cout << "Need to implement command #210 - 6" << endl;
}

//<-dceag-c211-b->

/** @brief COMMAND: #211 - 7 */
/** 7 */

void VLC_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
  cout << "Need to implement command #211 - 7" << endl;
}

//<-dceag-c212-b->

/** @brief COMMAND: #212 - 8 */
/** 8 */

void VLC_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
  cout << "Need to implement command #212 - 8" << endl;
}

//<-dceag-c213-b->

/** @brief COMMAND: #213 - 9 */
/** 9 */

void VLC_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
  cout << "Need to implement command #213 - 9" << endl;
}

//<-dceag-c240-b->

/** @brief COMMAND: #240 - Back / Prior Menu */
/** Navigate back .. ( Escape ) */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
  cout << "Need to implement command #240 - Back / Prior Menu" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
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

void VLC_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
  cout << "Need to implement command #249 - Start Streaming" << endl;
  cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
  cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
  cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;

  m_sStreamingTargets=sStreamingTargets;

  string::size_type pos=0;
  string curTarget = StringUtils::Tokenize(sStreamingTargets,string(","),pos);
  while (curTarget!="")
    {
      int targetDevice = atoi(curTarget.c_str());
      DCE::CMD_Play_Media cmd(m_dwPK_Device, targetDevice, iPK_MediaType, iStreamID, sMediaPosition, sMediaURL + "#sync");
      SendCommandNoResponse(cmd);
      curTarget = StringUtils::Tokenize(sStreamingTargets,string(","),pos);
    }
  
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

void VLC_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
  cout << "Need to implement command #259 - Report Playback Position" << endl;
  cout << "Parm #9 - Text=" << sText << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

  if (!m_pVLC)
    return;

  Position pPosition = Position(m_pVLC->GetCurrentTitle(),
				m_pVLC->GetCurrentChapter(),
				m_pVLC->GetTime(),
				m_pVLC->GetSubtitle(),
				m_pVLC->GetAudioTrack(),
				m_pVLC->GetCurrentDuration());
  
  *sMediaPosition = pPosition.toString();

}

//<-dceag-c412-b->

/** @brief COMMAND: #412 - Set Media Position */
/** Jump to a certain media position */
/** @param #41 StreamID */
/** The stream to set */
/** @param #42 MediaPosition */
/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void VLC_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
  cout << "Need to implement command #412 - Set Media Position" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

  if (sMediaPosition.empty())
    return;

  if (!m_pVLC)
    return;

  Position position = Position(sMediaPosition);

  if (position.hasOnlyPos())
    {
      // We only got a POS: indicator, just jump to position.
      m_pVLC->SetTime(position.getPosition());
    }
  else
    {
      // We are changing more than position, re-issue a play.
      // If we are streaming, we should re-do the CMD_Start_Streaming here (FIXME)
      CMD_Play_Media(0,m_pVLC->GetStreamID(),position.toString(),m_pVLC->GetMediaURL(),sCMD_Result,pMessage);
    }
}

//<-dceag-c548-b->

/** @brief COMMAND: #548 - Menu */
/** Show a menu associated with this media */
/** @param #9 Text */
/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
/** @param #41 StreamID */
/** ID of stream to apply */

void VLC_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
  cout << "Need to implement command #548 - Menu" << endl;
  cout << "Parm #9 - Text=" << sText << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  CMD_Goto_Media_Menu(0,0);
}

//<-dceag-c916-b->

/** @brief COMMAND: #916 - Set Aspect Ratio */
/** Force aspect ratio */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #260 Aspect Ratio */
/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void VLC_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
  cout << "Need to implement command #916 - Set Aspect Ratio" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #260 - Aspect_Ratio=" << sAspect_Ratio << endl;

  m_pVLC->SetAspectRatio(sAspect_Ratio);

}

//<-dceag-c917-b->

/** @brief COMMAND: #917 - Set Zoom */
/** Sets zoom level, relative, absolute or 'auto' */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #261 Zoom Level */
/** Zoom level to set */

void VLC_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
  cout << "Need to implement command #917 - Set Zoom" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #261 - Zoom_Level=" << sZoom_Level << endl;

  m_pVLC->SetZoomFactor(sZoom_Level);

}

//<-dceag-c920-b->

/** @brief COMMAND: #920 - Set Media ID */
/** Set Media ID - information about media stream */
/** @param #10 ID */
/** Media ID (special format) */
/** @param #41 StreamID */
/** ID of stream to set media information for */

void VLC_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
  cout << "Need to implement command #920 - Set Media ID" << endl;
  cout << "Parm #10 - ID=" << sID << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  m_pVLC->SetMediaType("N",-1);

  if (!sID.empty())
    {
      string sMediaType = sID.substr(0,1);
      sID.erase(0,1);
      int iMediaID = atoi(sID.c_str());
      m_pVLC->SetMediaType(sMediaType, iMediaID);
    }
  
}

/**
 * Blu-Ray mounting functions.
 */
bool VLC_Player::MountLocalBD(string sURL) {
	// for simplicity (for now) we'll assume a single BD drive is possible
	string sDrive = sURL.substr(9);
	string sMount = FileUtils::ExcludeTrailingSlash("/tmp" + sDrive);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Player::MountLocalBD(1) - Drive: %s, Mount: %s (%s)", sDrive.c_str(), sMount.c_str(), sURL.c_str() );
	if (FileUtils::DirExists(sMount)) {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Player::MountLocalBD(2) - Drive: %s, Mount: %s (%s)", sDrive.c_str(), sMount.c_str(), sURL.c_str() );
		// unmount
		string sUmountCommand = "umount " + sMount;
		system(sUmountCommand.c_str());
		sUmountCommand = "umount " + sDrive;
		system(sUmountCommand.c_str());
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Player::MountLocalBD(3) - Drive: %s, Mount: %s (%s)", sDrive.c_str(), sMount.c_str(), sURL.c_str() );
	FileUtils::MakeDir(sMount);
	// mount
	string sMountCommand = "mount " + sDrive + " " + sMount;
	system(sMountCommand.c_str());
	mountedLocalBluRays.push_back(sURL);
	return true;
}

bool VLC_Player::UnmountLocalBD(string sURL) {
	string sDrive = sURL.substr(9);
	string sMount = FileUtils::ExcludeTrailingSlash("/tmp" + sDrive);

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Player::UnmountLocalBD(1) - Drive: %s, Mount: %s (%s)", sDrive.c_str(), sMount.c_str(), sURL.c_str() );
	if (FileUtils::DirExists(sMount)) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Player::UnmountLocalBD(2) - Drive: %s, Mount: %s (%s)", sDrive.c_str(), sMount.c_str(), sURL.c_str() );
		// unmount
		string sUmountCommand = "umount " + sMount;
		system(sUmountCommand.c_str());
		sUmountCommand = "umount " + sDrive;
		system(sUmountCommand.c_str());
	}
	return true;
}

bool VLC_Player::UnmountLocalBD() {
  if (mountedLocalBluRays.size() != 0)
    {
      string sPath = mountedLocalBluRays.back();
      mountedLocalBluRays.pop_back();
      bool bResult = UnmountLocalBD(sPath);
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::UnmountLocalBD() unmount %s", (bResult?"succeeded":"failed"));
      return bResult;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::UnmountLocalBD() no local BD mounted.");
      return false;
    }
  return true;
}

bool VLC_Player::IsLocalBD(string sURL) {
	// TODO: Add .BD extension or whatever extension lmce things BD disks are in file format
	bool bResult = StringUtils::StartsWith(sURL,"bluray:",true);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC_Player::IsLocalBD() - %s (%s)", bResult ? "YES" : "NO", sURL.c_str() );
	return bResult;
}

/**
 * Remote DVD mount/umount functions.
 */

bool VLC_Player::IsRemoteDVD(string sURL)
{
	bool bResult = false;
	ExtractComputerAndDeviceFromRemoteDVD(sURL, bResult);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"IsRemoteDVD - %s = %d",sURL.c_str(),bResult);
	return bResult;
}

bool VLC_Player::MountRemoteDVD(string sURL) {
  // TODO release previously mounted DVD?
  
  bool bResult = false;
  pair<int, string> remoteDVD = ExtractComputerAndDeviceFromRemoteDVD(sURL, bResult);
  
  if ( !bResult ) {
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::MountRemoteDVD %s is not a remote DVD",sURL.c_str());
    return false;
  }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::MountRemoteDVD trying to mount %s",sURL.c_str());
    }
  
  return MountRemoteDVD(remoteDVD.first, remoteDVD.second);
}

bool VLC_Player::UnmountRemoteDVD(string sURL) {
  return UnmountRemoteDVD();
}

bool VLC_Player::MountRemoteDVD(int iComputerID, string sDevice) {
  int iResult = InvokeRemoteDVDHelper(iComputerID, sDevice, "start");
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::MountRemoteDVD %s", ( (iResult==0)?"mounted OK":"failed to mount") );
  
  if (iResult == 0)
    mountedRemoteDVDs.push_back(make_pair(iComputerID, sDevice));
  
  return (iResult == 0) ;
}

bool VLC_Player::UnmountRemoteDVD(int iComputerID, string sDevice) {
  int iResult = InvokeRemoteDVDHelper(iComputerID, sDevice, "stop");
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::UnmountRemoteDVD %s", ( (iResult==0)?"unmounted OK":"failed to unmount") );
  
  return (iResult == 0) ;
}

int VLC_Player::InvokeRemoteDVDHelper(int iComputerID, string sDevice, string sCommand) {
  string sHelperCommand = "/usr/pluto/bin/StorageDevices_DVD.sh " + StringUtils::itos(iComputerID) + " " + sDevice + " " + sCommand;
  int iResult = system( sHelperCommand.c_str() );
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC_Player::InvokeRemoteDVDHelper invoked [%s], return code %i", sHelperCommand.c_str(), iResult);
  
  return iResult;
}

// extracts computerID and device name from URL, sets bResult to true if succeeds, false otherwise
pair<int, string> VLC_Player::ExtractComputerAndDeviceFromRemoteDVD(string sURL, bool &bResult) {
  const char prefix[] = "dvd:///mnt/optical/";
  bResult = false;
  pair<int, string> pResult = make_pair(-1, "");
  
  if ( !StringUtils::StartsWith(sURL, prefix) )
    return pResult;
  
  sURL.erase(0, sizeof(prefix)-1);
  
  vector<string> vInfo = StringUtils::Split(sURL, "_");
  if ( vInfo.size() < 2 )
    return pResult;
  
  int iComputerID = atoi( vInfo[0].c_str() );
  if ( iComputerID == 0 )
    return pResult;
  
  string sDevice = vInfo[1];
  
  pResult.first = iComputerID;
  pResult.second = sDevice;
  
  bResult = true;
  return pResult;
}

bool VLC_Player::UnmountRemoteDVD() {
  if (mountedRemoteDVDs.size() != 0)
    {
      pair<int, string> rDVD = mountedRemoteDVDs.back();
      mountedRemoteDVDs.pop_back();
      bool bResult = UnmountRemoteDVD(rDVD.first, rDVD.second);
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::UnmountRemoteDVD() unmount %s", (bResult?"succeeded":"failed") );
      return bResult;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC_Player::UnmountRemoteDVD() no remote DVD mounted" );
      return false;
    }
}

