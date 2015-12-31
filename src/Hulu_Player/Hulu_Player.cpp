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
#include "Hulu_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

// Additional Includes ///////////////////////
#include "PlutoUtils/ProcessUtils.h"
#include "WindowUtils/WindowUtils.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Button.h"
#include "Gen_Devices/AllScreens.h"

#include <sstream>
#include <pthread.h>

#include "LIRCD_Thread.h"

// ------------------- ///////////////////////

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Hulu_Player::Hulu_Player (int DeviceID, string ServerAddress,
			  bool bConnectEventHandler, bool bLocalMode,
			  class Router * pRouter):
Hulu_Player_Command (DeviceID, ServerAddress, bConnectEventHandler,
		     bLocalMode, pRouter)
//<-dceag-const-e->
,
m_HuluMutex ("hulu_player")
{
  m_HuluMutex.Init (NULL);
  m_pDevice_App_Server = NULL;
  m_LIRCD_Thread = (pthread_t) NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Hulu_Player::~Hulu_Player ()
//<-dceag-dest-e->
{
  // Exit the LIRCD Thread
  LIRCD_bQuit = true;
  if (m_LIRCD_Thread != (pthread_t) NULL)
    pthread_join (m_LIRCD_Thread, NULL);
}

void
Hulu_Player::PrepareToDelete ()
{
  Command_Impl::PrepareToDelete ();
  m_pDevice_App_Server = NULL;
}

void
Hulu_Player::CreateChildren ()
{
  if (pthread_create (&m_LIRCD_Thread, NULL, LIRCD_Thread, (void *) this))
    {
      m_LIRCD_Thread = (pthread_t) NULL;
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "Failed to create LIRCD Thread. Exiting with error code 1.");
      m_bQuit_set (true);
      exit (1);
    }
}


//<-dceag-getconfig-b->
bool
Hulu_Player::GetConfig ()
{
  if (!Hulu_Player_Command::GetConfig ())
    return false;
//<-dceag-getconfig-e->


  // Put your code here to initialize the data in this class
  // The configuration parameters DATA_ are now populated

  m_pDevice_Hulu_Plugin =
    m_pData->m_AllDevices.
    m_mapDeviceData_Base_FindFirstOfTemplate
    (DEVICETEMPLATE_Hulu_PlugIn_CONST);
  if (!m_pDevice_Hulu_Plugin)
    {
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "I need a Hulu plugin to function.");
      return false;
    }

  m_pDevice_App_Server =
    m_pData->
    FindFirstRelatedDeviceOfCategory (DEVICECATEGORY_App_Server_CONST, this);
  if (!m_pDevice_App_Server)
    {
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "I need an App Server to function.");
      return false;
    }

  return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool
Hulu_Player::Register ()
//<-dceag-reg-e->
{
  return Connect (PK_DeviceTemplate_get ());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
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
void
Hulu_Player::ReceivedCommandForChild (DeviceData_Impl * pDeviceData_Impl,
				      string & sCMD_Result,
				      Message * pMessage)
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
void
Hulu_Player::ReceivedUnknownCommand (string & sCMD_Result, Message * pMessage)
//<-dceag-cmduk-e->
{
  sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

// Private Methods ////////////////////////////////////////////////////////////

bool
Hulu_Player::UpdateHuluConfig ()
{

  const string s_OutputFile = "/root/.huludesktop";
  string s_ConfigFile =
    "[display]\n"
    "fullscreen = FALSE\n"
    "width = 640\n"
    "height = 480\n"
    "pos_x = 0\n"
    "pos_y = 0\n"
    "\n"
    "[remote]\n"
    "lirc_device = /tmp/hulu_lircd\n"
    "lirc_remote_identifier = mceusb\n"
    "lirc_release_suffix = _UP\n"
    "lirc_repeat_threshold = 10\n"
    "button_name_up = Up\n"
    "button_name_down = Down\n"
    "button_name_left = Left\n"
    "button_name_right = Right\n"
    "button_name_select = OK\n"
    "button_name_menu = Home\n"
    "\n"
    "[flash]\n"
    "flash_location = /usr/pluto/bin/libflashplayer-hulu.so\n"
    "\n"
    "[screensaver]\n"
    "suspend_script = (null)\n"
    "resume_script = (null)\n"
    "\n" "[version]\n" "latest = (null)\n" "eula_version = 1\n";


  if (!FileUtils::WriteTextFile (s_OutputFile, s_ConfigFile))
    {
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "Couldn't write %s",
					    s_OutputFile.c_str ());
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance ()->Write (LV_STATUS, "Wrote %s",
					    s_OutputFile.c_str ());
      return true;
    }

}

bool
Hulu_Player::LaunchHulu ()
{
  DeviceData_Base *pDevice_App_Server =
    m_pData->
    FindFirstRelatedDeviceOfCategory (DEVICECATEGORY_App_Server_CONST, this);
  if (pDevice_App_Server)
    {
      string sMessage =
	StringUtils::itos (m_dwPK_Device) + " " +
	StringUtils::itos (m_dwPK_Device) +
	" 1 " TOSTRING (COMMAND_Application_Exited_CONST) " "
	TOSTRING (COMMANDPARAMETER_Exit_Code_CONST) " ";

      if (!UpdateHuluConfig ())
	{
	  return false;
	}

      DCE::CMD_Spawn_Application CMD_Spawn_Application (m_dwPK_Device,
							pDevice_App_Server->
							m_dwPK_Device,
							"huludesktop",
							"huludesktop", "",
							sMessage + "1",
							sMessage + "0", false,
							false, true, false);
      if (SendCommand (CMD_Spawn_Application))
	{
	  m_bHuluIsRunning = true;

	  return true;
	}
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "Hulu_Player::LaunchHulu - failed to launch");
    }
  else
    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					  "Hulu_Player::LaunchHulu - no app server");
  return false;
}

bool
Hulu_Player::StopHulu ()
{
  DeviceData_Base *pDevice_App_Server = NULL;
  string sResponse;
  if (!m_bRouterReloading)
    {
      pDevice_App_Server =
	m_pData->
	FindFirstRelatedDeviceOfCategory (DEVICECATEGORY_App_Server_CONST,
					  this);
      if (pDevice_App_Server)
	{
	  DCE::CMD_Kill_Application CMD_Kill_Application (m_dwPK_Device,
							  pDevice_App_Server->
							  m_dwPK_Device,
							  "huludesktop",
							  false);

	  LIRCD_bConnectionActive = false;  // Drop the connection.

	  return SendCommand (CMD_Kill_Application, &sResponse);	// Get return confirmation so we know it's gone before we continue
	}
    }

  LoggerWrapper::GetInstance ()->Write (LV_STATUS,
					"Hulu_Player::StopHulu %p %s",
					pDevice_App_Server,
					sResponse.c_str ());
  return false;

}

void
Hulu_Player::SendLIRCDCommand (string sCommand)
{
  // Send a formatted command to the LIRCD socket. Currently calls the LIRCd socket
  // since the lircd thread deals with raw commands, This is a convienience method
  // that fakes the necessary formatted command bits.
  //
  LIRCD_SendCommand ("00000000 0 " + sCommand + "_UP mceusb\n");
}

// End Private Methods ////////////////////////////////////////////////////////
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

void
Hulu_Player::CMD_Simulate_Keypress (string sPK_Button, int iStreamID,
				    string sName, string & sCMD_Result,
				    Message * pMessage)
//<-dceag-c28-e->
{
  cout << "Need to implement command #28 - Simulate Keypress" << endl;
  cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #50 - Name=" << sName << endl;

  switch (atoi (sPK_Button.c_str ()))
    {
    case BUTTON_Up_Arrow_CONST:
      SendLIRCDCommand ("Up");
      break;
    case BUTTON_Down_Arrow_CONST:
      SendLIRCDCommand ("Down");
      break;
    case BUTTON_Left_Arrow_CONST:
      SendLIRCDCommand ("Left");
      break;
    case BUTTON_Right_Arrow_CONST:
      SendLIRCDCommand ("Right");
      break;
    case BUTTON_Enter_CONST:
      SendLIRCDCommand ("OK");
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

void
Hulu_Player::CMD_Simulate_Mouse_Click (int iPosition_X, int iPosition_Y,
				       int iStreamID, string & sCMD_Result,
				       Message * pMessage)
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

void
Hulu_Player::CMD_Update_Object_Image (string sPK_DesignObj, string sType,
				      char *pData, int iData_Size,
				      string sDisable_Aspect_Lock,
				      string & sCMD_Result,
				      Message * pMessage)
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

void
Hulu_Player::CMD_Play_Media (int iPK_MediaType, int iStreamID,
			     string sMediaPosition, string sMediaURL,
			     string & sCMD_Result, Message * pMessage)
//<-dceag-c37-e->
{
  cout << "Need to implement command #37 - Play Media" << endl;
  cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
  cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

  LaunchHulu ();

}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void
Hulu_Player::CMD_Stop_Media (int iStreamID, string * sMediaPosition,
			     string & sCMD_Result, Message * pMessage)
//<-dceag-c38-e->
{
  cout << "Need to implement command #38 - Stop Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

  StopHulu ();

}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void
Hulu_Player::CMD_Pause_Media (int iStreamID, string & sCMD_Result,
			      Message * pMessage)
//<-dceag-c39-e->
{
  cout << "Need to implement command #39 - Pause Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void
Hulu_Player::CMD_Restart_Media (int iStreamID, string & sCMD_Result,
				Message * pMessage)
//<-dceag-c40-e->
{
  cout << "Need to implement command #40 - Restart Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
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

void
Hulu_Player::CMD_Change_Playback_Speed (int iStreamID,
					int iMediaPlaybackSpeed, bool bReport,
					string & sCMD_Result,
					Message * pMessage)
//<-dceag-c41-e->
{
  cout << "Need to implement command #41 - Change Playback Speed" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
  cout << "Parm #220 - Report=" << bReport << endl;
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void
Hulu_Player::CMD_Jump_to_Position_in_Stream (string sValue_To_Assign,
					     int iStreamID,
					     string & sCMD_Result,
					     Message * pMessage)
//<-dceag-c42-e->
{
  cout << "Need to implement command #42 - Jump to Position in Stream" <<
    endl;
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

void
Hulu_Player::CMD_Jump_Position_In_Playlist (string sValue_To_Assign,
					    int iStreamID,
					    string & sCMD_Result,
					    Message * pMessage)
//<-dceag-c65-e->
{
  cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void
Hulu_Player::CMD_Navigate_Next (int iStreamID, string & sCMD_Result,
				Message * pMessage)
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

void
Hulu_Player::CMD_Navigate_Prev (int iStreamID, string & sCMD_Result,
				Message * pMessage)
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

void
Hulu_Player::CMD_Get_Video_Frame (string sDisable_Aspect_Lock, int iStreamID,
				  int iWidth, int iHeight, char **pData,
				  int *iData_Size, string * sFormat,
				  string & sCMD_Result, Message * pMessage)
//<-dceag-c84-e->
{
  cout << "Need to implement command #84 - Get Video Frame" << endl;
  cout << "Parm #19 - Data  (data value)" << endl;
  cout << "Parm #20 - Format=" << sFormat << endl;
  cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #60 - Width=" << iWidth << endl;
  cout << "Parm #61 - Height=" << iHeight << endl;

	/** 
         * Temporarily commented out until i can find a sane way to do this nicely.
         *
	PLUTO_SAFETY_LOCK(mm,m_HuluMutex);

	FileUtils::DelFile("/tmp/HuluOSD.png");
	FileUtils::DelFile("/tmp/HuluOSD.jpg");

	string cmd = "import -window " + m_sHuluWindowId + " /tmp/HuluOSD.png";
	system(cmd.c_str());

	string cmd2 = "convert -sample 800x800 /tmp/HuluOSD.png /tmp/HuluOSD.jpg";
	system(cmd2.c_str());

	size_t size;
	*pData = FileUtils::ReadFileIntoBuffer("/tmp/HuluOSD.jpg",size);
	*iData_Size = size;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Hulu_Player::CMD_Get_Video_Frame got %d",size);
 
	*
	**/

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

void
Hulu_Player::CMD_Goto_Media_Menu (int iStreamID, int iMenuType,
				  string & sCMD_Result, Message * pMessage)
//<-dceag-c87-e->
{
  cout << "Need to implement command #87 - Goto Media Menu" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #64 - MenuType=" << iMenuType << endl;

  switch (iMenuType)
    {
    case SHOW_OSD:
      EVENT_Menu_Onscreen (iStreamID, true);
      break;
    case SHOW_REMOTE:
    default:
      EVENT_Menu_Onscreen (iStreamID, false);
    }

}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void
Hulu_Player::CMD_Guide (string & sCMD_Result, Message * pMessage)
//<-dceag-c126-e->
{
  cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_EnterGo (int iStreamID, string & sCMD_Result,
			  Message * pMessage)
//<-dceag-c190-e->
{
  cout << "Need to implement command #190 - Enter/Go" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  SendLIRCDCommand ("OK");

}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Move_Up (int iStreamID, string & sCMD_Result,
			  Message * pMessage)
//<-dceag-c200-e->
{
  cout << "Need to implement command #200 - Move Up" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  SendLIRCDCommand ("Up");

}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Move_Down (int iStreamID, string & sCMD_Result,
			    Message * pMessage)
//<-dceag-c201-e->
{
  cout << "Need to implement command #201 - Move Down" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  SendLIRCDCommand ("Down");

}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Move_Left (int iStreamID, string & sCMD_Result,
			    Message * pMessage)
//<-dceag-c202-e->
{
  cout << "Need to implement command #202 - Move Left" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  SendLIRCDCommand ("Left");

}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Move_Right (int iStreamID, string & sCMD_Result,
			     Message * pMessage)
//<-dceag-c203-e->
{
  cout << "Need to implement command #203 - Move Right" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  SendLIRCDCommand ("Right");

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

void
Hulu_Player::CMD_Start_Streaming (int iPK_MediaType, int iStreamID,
				  string sMediaPosition, string sMediaURL,
				  string sStreamingTargets,
				  string & sCMD_Result, Message * pMessage)
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

void
Hulu_Player::CMD_Report_Playback_Position (int iStreamID, string * sText,
					   string * sMediaPosition,
					   string & sCMD_Result,
					   Message * pMessage)
//<-dceag-c259-e->
{
  cout << "Need to implement command #259 - Report Playback Position" << endl;
  cout << "Parm #9 - Text=" << sText << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void
Hulu_Player::CMD_Set_Media_Position (int iStreamID, string sMediaPosition,
				     string & sCMD_Result, Message * pMessage)
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

void
Hulu_Player::CMD_Menu (string sText, int iStreamID, string & sCMD_Result,
		       Message * pMessage)
//<-dceag-c548-e->
{
  cout << "Need to implement command #548 - Menu" << endl;
  cout << "Parm #9 - Text=" << sText << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  SendLIRCDCommand ("Home");

}

//<-dceag-c812-b->

	/** @brief COMMAND: #812 - Application Exited */
	/** Notify us that Myth Player exited */
		/** @param #227 PID */
			/** Process ID to be passed to the ApplicationExited function */
		/** @param #228 Exit Code */
			/** Exit Code to be passed to the ApplicationExited function */

void
Hulu_Player::CMD_Application_Exited (int iPID, int iExit_Code,
				     string & sCMD_Result, Message * pMessage)
//<-dceag-c812-e->
{
  cout << "Need to implement command #812 - Application Exited" << endl;
  cout << "Parm #227 - PID=" << iPID << endl;
  cout << "Parm #228 - Exit_Code=" << iExit_Code << endl;

#ifndef WIN32
  LoggerWrapper::GetInstance ()->Write (LV_STATUS, "Process exited %d %d",
					iPID, iExit_Code);

  // void *data;

  {
    LoggerWrapper::GetInstance ()->Write (LV_STATUS,
					  "Send go back to the caller!");
    DCE::CMD_MH_Stop_Media_Cat CMD_MH_Stop_Media_Cat (m_dwPK_Device,
						      DEVICECATEGORY_Media_Plugins_CONST,
						      false, BL_SameHouse,
						      m_dwPK_Device, 0, 0, "",
						      false);
    SendCommand (CMD_MH_Stop_Media_Cat);
  }

#endif

}

//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void
Hulu_Player::CMD_Set_Aspect_Ratio (int iStreamID, string sAspect_Ratio,
				   string & sCMD_Result, Message * pMessage)
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

void
Hulu_Player::CMD_Set_Zoom (int iStreamID, string sZoom_Level,
			   string & sCMD_Result, Message * pMessage)
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

void
Hulu_Player::CMD_Set_Media_ID (string sID, int iStreamID,
			       string & sCMD_Result, Message * pMessage)
//<-dceag-c920-e->
{
  cout << "Need to implement command #920 - Set Media ID" << endl;
  cout << "Parm #10 - ID=" << sID << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}


//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Skip_Fwd_ChannelTrack_Greater (int iStreamID,
						string & sCMD_Result,
						Message * pMessage)
//<-dceag-c63-e->
{
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Skip_Back_ChannelTrack_Lower (int iStreamID,
					       string & sCMD_Result,
					       Message * pMessage)
//<-dceag-c64-e->
{
}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Pause (int iStreamID, string & sCMD_Result,
			Message * pMessage)
//<-dceag-c92-e->
{
}

//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** Stop the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void
Hulu_Player::CMD_Stop (int iStreamID, bool bEject, string & sCMD_Result,
		       Message * pMessage)
//<-dceag-c95-e->
{
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Play (int iStreamID, string & sCMD_Result,
		       Message * pMessage)
//<-dceag-c139-e->
{
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void
Hulu_Player::CMD_0 (string & sCMD_Result, Message * pMessage)
//<-dceag-c204-e->
{
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void
Hulu_Player::CMD_1 (string & sCMD_Result, Message * pMessage)
//<-dceag-c205-e->
{
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void
Hulu_Player::CMD_2 (string & sCMD_Result, Message * pMessage)
//<-dceag-c206-e->
{
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void
Hulu_Player::CMD_3 (string & sCMD_Result, Message * pMessage)
//<-dceag-c207-e->
{
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void
Hulu_Player::CMD_4 (string & sCMD_Result, Message * pMessage)
//<-dceag-c208-e->
{
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void
Hulu_Player::CMD_5 (string & sCMD_Result, Message * pMessage)
//<-dceag-c209-e->
{
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void
Hulu_Player::CMD_6 (string & sCMD_Result, Message * pMessage)
//<-dceag-c210-e->
{
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void
Hulu_Player::CMD_7 (string & sCMD_Result, Message * pMessage)
//<-dceag-c211-e->
{
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void
Hulu_Player::CMD_8 (string & sCMD_Result, Message * pMessage)
//<-dceag-c212-e->
{
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void
Hulu_Player::CMD_9 (string & sCMD_Result, Message * pMessage)
//<-dceag-c213-e->
{
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void
Hulu_Player::CMD_Back_Prior_Menu (int iStreamID, string & sCMD_Result,
				  Message * pMessage)
//<-dceag-c240-e->
{
  SendLIRCDCommand ("Home");
}
