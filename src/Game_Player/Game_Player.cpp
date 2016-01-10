/*
     Copyright (C) 2008 LOCALE|concept

     www.localeconcept.com

     Phone: +1 (617) 319-8219
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

//<-dceag-d-b->
#include "Game_Player.h"
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
#include "WindowUtils/WindowUtils.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_Variable.h"
#include "Gen_Devices/AllScreens.h"

#include <pthread.h>
#include <math.h>

#include <X11/Xlib.h>

Game_Player *g_pGame_Player = NULL;

/**
 * This is a castrated window handler so that the program will keep running.
 * This is okay, because any failed X requests will be retried until timeout.
 */
int _defaultWindowHandler(Display *disp, XErrorEvent *err)
{
  char msg[80];
  XGetErrorText(disp, err->error_code, msg, 80);
  fprintf(stderr, "Error code %s\n", msg);
  if (g_pGame_Player)
    {
      g_pGame_Player->pleaseResend();
    }
  return 0;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Game_Player::Game_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Game_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
,
	  m_GameMutex ("game_player"), m_X11ControllerMutex("x11_controller")
{
  g_pGame_Player = this;
  m_GameMutex.Init (NULL);
  m_X11ControllerMutex.Init(NULL);
  m_pAlarmManager = NULL;
  m_pDevice_App_Server = NULL;
  m_pEmulatorFactory=NULL;
  m_pEmulatorController=NULL;
  m_pEmulatorController_prev=NULL;
  m_iPK_MediaType = 0;
  m_iStreamID = 0;
  m_bLoadSavedGame = 0;
  m_iModifier = 0;
  m_bIsRecording = false;
  m_bStateDirExists=false;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Game_Player::~Game_Player()
//<-dceag-dest-e->
{
  EVENT_Playback_Completed ("", 0, false);	// In case media plugin thought something was playing, let it know that there's not
  if (m_pVirtualKeyboard)
    {
      delete m_pVirtualKeyboard;
    }
}

void Game_Player::pleaseResend()
{
  if (m_pEmulatorController)
    m_pEmulatorController->pleaseResend();
}

bool Game_Player::Connect (int iPK_DeviceTemplate)
{
  if (!Command_Impl::Connect (iPK_DeviceTemplate))
    return false;

  DeviceData_Base *pDevice = m_pData->GetTopMostDevice();
  m_sIPofMD = pDevice->m_sIPAddress;

  EVENT_Playback_Completed ("", 0, false);	// In case media plugin thought something was playing, let it know that there's not

  return true;

}

void Game_Player::DoExitActions()
{
  delete m_pAlarmManager;
  delete m_pEmulatorFactory;
  m_pAlarmManager = NULL;
  m_pDevice_App_Server = NULL;
  m_iPK_MediaType = 0;
  XSetErrorHandler(NULL);
  if (FileUtils::DirExists(GAME_PLAYER_STATE_DIR))
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Game Player State directory exists. Cleaning up, and deleting.");
      if (!FileUtils::DelDir(GAME_PLAYER_STATE_DIR))
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game Player State directory could not be deleted.");
	}
      else
	{
	  m_bStateDirExists=false;
	}
    }
}

void Game_Player::OnQuit()
{
  DoExitActions();
  Command_Impl::OnQuit();
}

void Game_Player::OnReload()
{
  DoExitActions();
  Command_Impl::OnReload();
}

void
Game_Player::PrepareToDelete ()
{
  Command_Impl::PrepareToDelete ();
  delete m_pAlarmManager;
  delete m_pEmulatorFactory;
  m_pAlarmManager = NULL;
  m_pDevice_App_Server = NULL;
  m_iPK_MediaType = 0;
  XSetErrorHandler(NULL);
  if (FileUtils::DirExists(GAME_PLAYER_STATE_DIR))
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Game Player State directory exists. Cleaning up, and deleting.");
      if (!FileUtils::DelDir(GAME_PLAYER_STATE_DIR))
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game Player State directory could not be deleted.");
	}
      else
	{
	  m_bStateDirExists=false;
	}
    }
}

//<-dceag-getconfig-b->
bool Game_Player::GetConfig()
{
	if( !Game_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

  // Put your code here to initialize the data in this class
  // The configuration parameters DATA_ are now populated

	// This next chunk of code attempts as defensively as possible, to keep a 
	// clean game state directory, after each launch of Game_Player.
	if (!FileUtils::DirExists(GAME_PLAYER_STATE_DIR))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Game Player State directory does not exist. Creating.");
	    // Create state dir if it does not exist (e.g. after media director boots)
	    FileUtils::MakeDir(GAME_PLAYER_STATE_DIR);
	    if (!FileUtils::DirExists(GAME_PLAYER_STATE_DIR))
	      {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Could not create Game Player State directory.");
		m_bStateDirExists=false;
	      }
	    else
	      {
		m_bStateDirExists=true;
	      }
	  }
	else // Directory already exists, delete it; remake it.
	  {
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Game Player State dir " GAME_PLAYER_STATE_DIR " exists, possible crash? Deleting and recreating.");
	    FileUtils::DelDir(GAME_PLAYER_STATE_DIR);
	    FileUtils::MakeDir(GAME_PLAYER_STATE_DIR);
	    if (!FileUtils::DirExists(GAME_PLAYER_STATE_DIR))
	      {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Could not recreate Game Player State Dir after cleanup.");
		m_bStateDirExists=false;
	      }
	    else
	      {
		m_bStateDirExists=true;
	      }
	  }

  XInitThreads();
  XSetErrorHandler(_defaultWindowHandler);

  m_pDevice_Game_Plugin =
    m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate
    (DEVICETEMPLATE_Game_PlugIn_CONST);
  if (!m_pDevice_Game_Plugin)
    {
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "I need a Game plugin to function.");
      return false;
    }

  m_pDevice_App_Server =
    m_pData->FindFirstRelatedDeviceOfCategory
    (DEVICECATEGORY_App_Server_CONST, this);
  if (!m_pDevice_App_Server)
    {
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "I need an App Server to function.");
      return false;
    }

  m_pDevice_Joystick =
    m_pData->FindFirstRelatedDeviceOfCategory
    (DEVICECATEGORY_Joysticks_CONST);
  if (!m_pDevice_Joystick)
    {
      LoggerWrapper::GetInstance ()->Write (LV_STATUS,
					    "No Special Joysticks Found");
    }
  else
    {
      if (!m_pDevice_Joystick->m_bDisabled)
	{
	  m_sJoystick_Configuration =
	    m_pData->
	    m_pEvent_Impl->GetDeviceDataFromDatabase (m_pDevice_Joystick->
						      m_dwPK_Device,
						      DEVICEDATA_Configuration_CONST);
	}
      else
	{
	  m_sJoystick_Configuration = "";
	}
    }

  m_pVirtualKeyboard=VirtualKeyboard::GetInstance();

  return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Game_Player::Register()
//<-dceag-reg-e->
{
  return Connect (PK_DeviceTemplate_get ());
}

void
Game_Player::AlarmCallback (int id, void *param)
{
  switch (id)
    {
    case CHECK_MAME:
      break;
    }
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
void Game_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Game_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
  sCMD_Result = "UNKNOWN COMMAND";
}

/**
 * CreateChildren() - This is used to instantiate the EmulatorFactory, which
 * will instantiate the individual EmulatorControllers for use. 
 * 
 * This is deliberately done after GetConfig() so that we do not waste memory
 * if the initial bootstrap to the DCE Router can't be made.
 */
void
Game_Player::CreateChildren ()
{
  m_pEmulatorFactory = new EmulatorFactory(this);
  m_pEmulatorFactory->init();
}

/** 
 * Called from the emulator controller to launch the emulator
 */
void Game_Player::LaunchEmulator()
{

    string sMessage = 
      StringUtils::itos(m_dwPK_Device) + " " +
      StringUtils::itos(m_dwPK_Device) +
      " 1 " TOSTRING (COMMAND_Application_Exited_CONST) " "
      TOSTRING (COMMANDPARAMETER_Exit_Code_CONST) " ";

    DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,
						     m_pDevice_App_Server->m_dwPK_Device,
						     m_pEmulatorController->m_pEmulatorModel->m_sEmulatorBinary,
						     "emulator",
						     m_pEmulatorController->m_pEmulatorModel->m_sArgs,
						     sMessage + "1",
						     sMessage + "0",
						     false,
						     false,
						     true,
						     false);

    SendCommand(CMD_Spawn_Application);

}

/**
 * Called from the emulator controller to kill the current emulator engine.
 */
void Game_Player::KillEmulator()
{
    DCE::CMD_Kill_Application CMD_Kill_Application(m_dwPK_Device,
						   m_pDevice_App_Server->m_dwPK_Device,
						   "emulator",
						   false);
    
    SendCommand(CMD_Kill_Application);
}

//<-dceag-sample-b->!
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

void Game_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
  cout << "Need to implement command #28 - Simulate Keypress" << endl;
  cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #50 - Name=" << sName << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  int iPK_Button = atoi(sPK_Button.c_str());

  if (!m_pEmulatorController)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CMD_Simulate_Keypress called, but no emulator controller! bsiling!");
      return;
    }

  m_pEmulatorController->pressButton(iPK_Button, pMessage);

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

void Game_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
  cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
  cout << "Parm #11 - Position_X=" << iPosition_X << endl;
  cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  if (!m_pEmulatorController)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CMD_Simulate_Keypress called, but no emulator controlelr! Bailing!");
    }
  else
    {
      m_pEmulatorController->pressClick(iPosition_X,iPosition_Y,pMessage);
    }

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

void Game_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
  cout << "Need to implement command #37 - Play Media" << endl;
  cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
  cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

  m_iPreviousStreamID = m_iStreamID;
  m_iStreamID = iStreamID;

  if (m_iPreviousStreamID == m_iStreamID) // We are on the same stream, wait for emulator to exit.
    {
      if (m_pEmulatorController)
	{
	  m_pEmulatorController->stop();
	  m_pEmulatorController->waitForEmulatorExit();
	}
    }

  if (m_pEmulatorController)
    m_pEmulatorController=NULL; 
 
  m_pEmulatorController = m_pEmulatorFactory->getEmulatorForMediaType(iPK_MediaType);

  if (!m_pEmulatorController)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"I could not get an emulator controller for type %d from the EmulatorFactory. Bailing!",iPK_MediaType);
      return;
    }

  // Eject all media from all slots.
  m_pEmulatorController->ejectAllMedia();
  // Detect if we are a streaming slave, and if so, parse URL, and point back to server.
  if (sMediaURL.find("slave://") != string::npos)
    {
      size_t pos=0;
      m_pEmulatorController->setStreaming(true);
      m_pEmulatorController->setStreamingMaster(false);
      string sURL1 = StringUtils::Tokenize(sMediaURL,"/",pos); // slave:
      string sURL2 = StringUtils::Tokenize(sMediaURL,"/",pos); // [ ]
      string sURL3 = StringUtils::Tokenize(sMediaURL,"/",pos); // ip address
      string sURL4 = sMediaURL.substr(pos,sMediaURL.size()); // The rest of URL
      m_pEmulatorController->setHostName(sURL3);
      sMediaURL=sURL4;
    }

  // If the media URL contains a system configuration bit, send it to the controller,
  // and strip it away from the media URL for further processing.
  if (sMediaURL.find("~") != string::npos)
    {
      m_pEmulatorController->setSystemConfiguration(sMediaURL.substr(sMediaURL.find("~")+1));
      sMediaURL = sMediaURL.substr(0,sMediaURL.find("~"));
    }

  m_pEmulatorController->insertMediaNamed(sMediaURL);

  // If this was filled, then the media was called from a bookmark in the file browser.
  if (!sMediaPosition.empty() && !m_pEmulatorController->m_pEmulatorModel->m_bIsStreaming)
    {
      m_pEmulatorController->setMediaPosition(sMediaPosition);
    }
  else
    {
      m_pEmulatorController->setMediaPosition("");
    }

  m_pEmulatorController->setStreamID(iStreamID);
  m_pEmulatorController->run();

}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Game_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
  cout << "Need to implement command #38 - Stop Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

  string sSavedMediaPosition = "";
  string sSavedText = "";

  if (!m_pEmulatorController)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Stop_Media called with no EmulatorController; Probably because Game Player was restarted after a crash. Not sending a stop.");
      return;
    }

  if (m_pEmulatorController->m_pEmulatorModel)
    {
      if (m_pEmulatorController->m_pEmulatorModel->emulatorHasCrashed())
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Stop_Media - Emulator has crashed. Reporting it to Orbiters.");
	  string sText = "I am sorry, the Game you were playing has unexpectedly crashed. If this persists, please try a different game.";
	  string sOptions = "Ok|";
	  // CMD_Display_Dialog_Box_On_Orbiter_Cat db(m_dwPK_Device,DEVICECATEGORY_Orbiter_Plugins_CONST,
	  //				       false, BL_SameHouse,
	  //				       sText,
	  // 				       sOptions,
	  //				       ""); // FIXME: make proper list.
	  // SendCommandNoResponse(db);
	  if (m_bIsRecording)
	    {
	      m_bIsRecording = !m_bIsRecording;
	      // This is simply a toggle, the EmulatorController will do the right thing.
	      m_pEmulatorController->record();
	    }
	  m_pEmulatorController->setStreaming(false);
	  m_pEmulatorController->setStreamingMaster(false);
	  return;
	}
    }

  if (m_bIsRecording)
    {
      m_bIsRecording = !m_bIsRecording;
      // This is simply a toggle, the EmulatorController will do the right thing.
      m_pEmulatorController->record();
    }

  if (!m_pEmulatorController->saveState(sSavedMediaPosition,sSavedText,true))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Stop_Media - EmulatorController's saveState failed. See above messages for further explanation.");
    }
  else
    {
      *sMediaPosition  = sSavedMediaPosition; // sSavedText ultimately not used.
    }

  m_pEmulatorController->stop();
  m_pEmulatorController->setStreaming(false);
  m_pEmulatorController->setStreamingMaster(false);
  m_pEmulatorController=NULL;
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Game_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
  cout << "Need to implement command #39 - Pause Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  if (m_pEmulatorController)
    m_pEmulatorController->pause();
  
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void Game_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
  cout << "Need to implement command #40 - Restart Media" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  if (m_pEmulatorController)
    m_pEmulatorController->unpause();

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

void Game_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
  cout << "Need to implement command #41 - Change Playback Speed" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
  cout << "Parm #220 - Report=" << bReport << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  if (m_pEmulatorController)
    m_pEmulatorController->setSpeed(iMediaPlaybackSpeed);

}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void Game_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
  cout << "Need to implement command #42 - Jump to Position in Stream" <<
    endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
  cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater"
    << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
  cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" <<
    endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
  cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
  cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  // Ironically, this is here to implement the save game stuff.
  if (sValue_To_Assign == "0")
    {
      // This is the original game, just send a reset. 
      CMD_Game_Reset ();
      return;
    }
  else
    {
      gm.Release();
      // This is a save point, call Set Media Position.
      CMD_Set_Media_Position (m_iStreamID, sValue_To_Assign);
      gm.Relock();
    }

}

//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void Game_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
  cout << "Need to implement command #81 - Navigate Next" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

  m_pVirtualKeyboard->ClickKey(BTN_LEFT);
  
}

//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void Game_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
  cout << "Need to implement command #84 - Get Video Frame" << endl;
  cout << "Parm #19 - Data  (data value)" << endl;
  cout << "Parm #20 - Format=" << sFormat << endl;
  cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #60 - Width=" << iWidth << endl;
  cout << "Parm #61 - Height=" << iHeight << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  *sFormat="2"; // We are assuming JPEG now, this may change later!
  m_pEmulatorController->getSnap(pMessage->m_dwPK_Device_From, iWidth, iHeight, pData, *iData_Size);

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

void Game_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
  cout << "Need to implement command #87 - Goto Media Menu" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #64 - MenuType=" << iMenuType << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  m_pEmulatorController->gotoMenu(iMenuType);

  CMD_Set_Active_Menu CMD_Set_Active_Menu(m_dwPK_Device,m_pDevice_Game_Plugin->m_dwPK_Device,StringUtils::itos(iMenuType));
  SendCommand(CMD_Set_Active_Menu);

  if (iMenuType > 0)
    {
      // Fire a menu onscreen event.
      EVENT_Menu_Onscreen(iStreamID,true);
    }
  else
    {
      EVENT_Menu_Onscreen(iStreamID,false);
    }

}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
  cout << "Need to implement command #92 - Pause" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;

}

//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** Stop the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void Game_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
  cout << "Need to implement command #95 - Stop" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #203 - Eject=" << bEject << endl;
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void Game_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
  cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{
  cout << "Need to implement command #139 - Play" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #190 - Enter/Go" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pEmulatorController->uiOK();
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #200 - Move Up" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pEmulatorController->uiUp();
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #201 - Move Down" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pEmulatorController->uiDown();
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #202 - Move Left" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pEmulatorController->uiLeft();
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #203 - Move Right" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pEmulatorController->uiRight();
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void Game_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #204 - 0" << endl;
  m_pEmulatorController->press0(pMessage);
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void Game_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #205 - 1" << endl;
  m_pEmulatorController->press1(pMessage);
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void Game_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #206 - 2" << endl;
  m_pEmulatorController->press2(pMessage);
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void Game_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #207 - 3" << endl;
  m_pEmulatorController->press3(pMessage);
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void Game_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #208 - 4" << endl;
  m_pEmulatorController->press4(pMessage);
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void Game_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #209 - 5" << endl;
  m_pEmulatorController->press5(pMessage);
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void Game_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #210 - 6" << endl;
  m_pEmulatorController->press6(pMessage);
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void Game_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #211 - 7" << endl;
  m_pEmulatorController->press7(pMessage);
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void Game_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #212 - 8" << endl;
  m_pEmulatorController->press8(pMessage);
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void Game_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #213 - 9" << endl;
  m_pEmulatorController->press9(pMessage);
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #240 - Back / Prior Menu" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  m_pEmulatorController->uiBack();
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

void Game_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
  cout << "Need to implement command #249 - Start Streaming" << endl;
  cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
  cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
  cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;
  
  string sFinalURL;

  m_pEmulatorController = m_pEmulatorFactory->getEmulatorForMediaType(iPK_MediaType);

  if (!sStreamingTargets.empty())
    {
      m_pEmulatorController->setStreaming(true);
      m_pEmulatorController->setStreamingMaster(true);
      size_t pos=0;
      while (pos<sStreamingTargets.size())
	{
	  string sCurrentTarget = StringUtils::Tokenize(sStreamingTargets,",",pos);
	  if (atoi(sCurrentTarget.c_str()) == m_dwPK_Device)
	    {
	      // Master
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MASTER Device %d sending Local play",atoi(sCurrentTarget.c_str()));
	      sFinalURL = sMediaURL;
	      CMD_Play_Media(iPK_MediaType,iStreamID,"",sFinalURL); // smediaposition blank for now
	    }
	  else
	    {
	      if (m_sIPAddress == "127.0.0.1")
		m_sIPAddress = "192.168.80.1";   // derp!
	      sFinalURL = "slave://" + m_sIPofMD + "/" + sMediaURL;
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SLAVE Device %d sending Remote play streamid=%d mediaType=%d sFinalURL=%s",atoi(sCurrentTarget.c_str()),iStreamID,iPK_MediaType,sFinalURL.c_str());
	      DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,
					    atoi(sCurrentTarget.c_str()),
					    iPK_MediaType,iStreamID,
						 "", // smediaposition blank for now
					    sFinalURL);
	      SendCommand(CMD_Play_Media);
	    }
	}      
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

void Game_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
  cout << "Need to implement command #259 - Report Playback Position" << endl;
  cout << "Parm #9 - Text=" << sText << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  if (m_pEmulatorController->canSaveState())
    {
      string sSavePosition, sSaveText;
      
      if (m_pEmulatorController->saveState(sSavePosition,sSaveText))
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Game_Player::CMD_Report_Playback_Position - Successfully saved state %s",sSavePosition.c_str());
	  *sMediaPosition = sSavePosition;
	  *sText = sSaveText;
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Report_Playback_Position - Failed saving state. See previous messages for more explanation.");
	}
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Report_Playback_Position - Emulation does not support save states, returning ERROR.");
      sCMD_Result = "ERROR";
    }
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void Game_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #412 - Set Media Position" << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

  if (m_pEmulatorController->canSaveState())
    {
      if (!m_pEmulatorController->loadState(sMediaPosition))
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Set_Media_Position - unable to restore state %s bailing!",sMediaPosition.c_str());
	  // TODO: Add A call to Orbiter Plugin's Display Alert to display an error to the user.
	}
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Set_Media_Position - Emulation does not support save state restore. Sending ERROR");
      sCMD_Result = "ERROR";
    }
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Game_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
  cout << "Need to implement command #548 - Menu" << endl;
  cout << "Parm #9 - Text=" << sText << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c812-b->

	/** @brief COMMAND: #812 - Application Exited */
	/** Notify us that Myth Player exited */
		/** @param #227 PID */
			/** Process ID to be passed to the ApplicationExited function */
		/** @param #228 Exit Code */
			/** Exit Code to be passed to the ApplicationExited function */

void Game_Player::CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage)
//<-dceag-c812-e->
{
  cout << "Need to implement command #812 - Application Exited" << endl;
  cout << "Parm #227 - PID=" << iPID << endl;
  cout << "Parm #228 - Exit_Code=" << iExit_Code << endl;

#ifndef WIN32
  LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "Process exited %d %d",
					iPID, iExit_Code);

  // Tell the controller the emulator process has exited.
  if (!m_pEmulatorController)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Application Exited called, probably due to emulator crash. not calling EmulatorHasExited()");
      return;
    }
  m_pEmulatorController->EmulatorHasExited(iExit_Code);

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

void Game_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
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

void Game_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
  cout << "Need to implement command #920 - Set Media ID" << endl;
  cout << "Parm #10 - ID=" << sID << endl;
  cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c943-b->

	/** @brief COMMAND: #943 - Game 1P Start */
	/** 1P start */

void Game_Player::CMD_Game_1P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c943-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  m_pEmulatorController->P1Start();
}

//<-dceag-c944-b->

	/** @brief COMMAND: #944 - Game 2P Start */
	/** 2P Start */

void Game_Player::CMD_Game_2P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c944-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #944 - Game 2P Start" << endl;
  m_pEmulatorController->P2Start();
}

//<-dceag-c945-b->

	/** @brief COMMAND: #945 - Game 3P Start */
	/** 3P Start */

void Game_Player::CMD_Game_3P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c945-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #945 - Game 3P Start" << endl;
  m_pEmulatorController->P3Start();
}

//<-dceag-c946-b->

	/** @brief COMMAND: #946 - Game 4P Start */
	/** 4P Start */

void Game_Player::CMD_Game_4P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c946-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #946 - Game 4P Start" << endl;
  m_pEmulatorController->P4Start();
}

//<-dceag-c947-b->

	/** @brief COMMAND: #947 - Game Insert Coin */
	/** Insert Coin */

void Game_Player::CMD_Game_Insert_Coin(string &sCMD_Result,Message *pMessage)
//<-dceag-c947-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #947 - Game Insert Coin" << endl;
  m_pEmulatorController->coin1();
}

//<-dceag-c948-b->

	/** @brief COMMAND: #948 - Game Service */
	/** Service Mode */

void Game_Player::CMD_Game_Service(string &sCMD_Result,Message *pMessage)
//<-dceag-c948-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #948 - Game Service" << endl;
  m_pEmulatorController->service1();
}

//<-dceag-c949-b->

	/** @brief COMMAND: #949 - Game Start */
	/** Game Start */

void Game_Player::CMD_Game_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c949-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #949 - Game Start" << endl;
  m_pEmulatorController->start();
}

//<-dceag-c950-b->

	/** @brief COMMAND: #950 - Game Select */
	/** Game Select */

void Game_Player::CMD_Game_Select(string &sCMD_Result,Message *pMessage)
//<-dceag-c950-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  m_pEmulatorController->select();
}

//<-dceag-c951-b->

	/** @brief COMMAND: #951 - Game Option */
	/** Game Option */

void Game_Player::CMD_Game_Option(string &sCMD_Result,Message *pMessage)
//<-dceag-c951-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  cout << "Need to implement command #951 - Game Option" << endl;
  m_pEmulatorController->option();
}

//<-dceag-c942-b->

	/** @brief COMMAND: #942 - Get Ripping Status */
	/** Tell Game to Start 1 Player */
		/** @param #199 Status */
			/** Ripping status */

void Game_Player::CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage)
//<-dceag-c942-e->
{
}

//<-dceag-c982-b->

	/** @brief COMMAND: #982 - Set Game Options */
	/** Set Options for the running Game System driver. */
		/** @param #5 Value To Assign */
			/** Dependent on driver, but usually a single line in the format of key,value */
		/** @param #219 Path */
			/** The Option to Set */

void Game_Player::CMD_Set_Game_Options(string sValue_To_Assign,string sPath,string &sCMD_Result,Message *pMessage)
//<-dceag-c982-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  if (m_pEmulatorController)
    {
      m_pEmulatorController->setOption(sPath,sValue_To_Assign,pMessage);
    }
}

//<-dceag-c983-b->

	/** @brief COMMAND: #983 - Get Game Options */
	/** Get Options for the currently running driver. */
		/** @param #5 Value To Assign */
			/** The Returned value. */
		/** @param #219 Path */
			/** The Parameter to return, usually left side of comma in Set Game Options. */

void Game_Player::CMD_Get_Game_Options(string sPath,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c983-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  string sTmpValue;
  if (m_pEmulatorController)
    {
      m_pEmulatorController->getOption(sPath,sTmpValue,pMessage);
      *sValue_To_Assign=sTmpValue;
    }
}

//<-dceag-c952-b->

	/** @brief COMMAND: #952 - Game Reset */
	/** Game Reset */

void Game_Player::CMD_Game_Reset(string &sCMD_Result,Message *pMessage)
//<-dceag-c952-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  m_pEmulatorController->reset();
}

void Game_Player::TranscodeAfterRecord(string sPath,string sFilename, long int dwPK_Device_Orbiter)
{
  
  // Find Media Plugin
  DeviceData_Base *pDevice_Media_Plugin = 
    m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_Media_Plugin_CONST);

  if (!pDevice_Media_Plugin)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::TranscodeAfterRecord - Could not find Media Plugin Device. Sorry.");
      return;
    }

  // The following sformat string is for ffmpeg.
  //  string sFormat = "acodec=ac3|ab=128k|vcodec=mpeg4|b=4000k|mbd=2|flags=+mv4+aic|trellis=2|cmp=2|subcmp=2";
  string sFormat = "oac=mp3lame|ovc=xvid|xvidbitrate=687";
  string sCaption="Enter a name for the game recording. I will notify you when the file is ready for playback.";
  string sCommand = StringUtils::itos(dwPK_Device_Orbiter) + " " +
    StringUtils::itos(pDevice_Media_Plugin->m_dwPK_Device) + " 1 " +
    TOSTRING (COMMAND_Transcode_File_CONST) + " " +
    TOSTRING(COMMANDPARAMETER_Format_CONST) + " \"" + sFormat + "\" " +
    TOSTRING(COMMANDPARAMETER_Filename_CONST) + " \"" + sPath + "/" + sFilename + "\" " +
    TOSTRING(COMMANDPARAMETER_Directory_CONST) + " " + "\"<%=9%>\" " +
    TOSTRING(COMMANDPARAMETER_Name_CONST) + " " + "\"<%=17%>.avi\"";
  
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::TranscodeAfterRecord - sCommand is %s",sCommand.c_str());

  DCE::SCREEN_FileSave screen_FileSave(m_dwPK_Device,
				       dwPK_Device_Orbiter,
				       MEDIATYPE_pluto_StoredVideo_CONST,
				       0,
				       sCaption,
				       sCommand,
				       true);

  SendCommand(screen_FileSave);

}

//<-dceag-c102-b->

	/** @brief COMMAND: #102 - Record */
	/** Record the current game. Toggle on off */

void Game_Player::CMD_Record(string &sCMD_Result,Message *pMessage)
//<-dceag-c102-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  m_bIsRecording = !m_bIsRecording;
  // This is simply a toggle, the EmulatorController will do the right thing.
  m_pEmulatorController->setOrbiter(pMessage->m_dwPK_Device_From);
  m_pEmulatorController->record();
}
//<-dceag-c616-b->

	/** @brief COMMAND: #616 - Select A */
	/** Select P1 keypad on supported game systems. */

void Game_Player::CMD_Select_A(string &sCMD_Result,Message *pMessage)
//<-dceag-c616-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  if (m_pEmulatorController)
    {
      m_pEmulatorController->selectA();
    }
}
//<-dceag-c617-b->

	/** @brief COMMAND: #617 - Select B */
	/** Select P2 keypad on supported game systems. */

void Game_Player::CMD_Select_B(string &sCMD_Result,Message *pMessage)
//<-dceag-c617-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);
  if (m_pEmulatorController)
    {
      m_pEmulatorController->selectB();
    }
}
//<-dceag-c1158-b->

	/** @brief COMMAND: #1158 - Swap Media */
	/** Swap media in a given slot. */
		/** @param #151 Slot Number */
			/** The slot number to swap in.  */
		/** @param #291 Slot */
			/** Game Player slot to swap Filename into. */

void Game_Player::CMD_Swap_Media(int iSlot_Number,string sSlot,string &sCMD_Result,Message *pMessage)
//<-dceag-c1158-e->
{
  PLUTO_SAFETY_LOCK (gm, m_GameMutex);

  if (!m_pEmulatorController)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Game_Player::CMD_Swap_Media() - Tried to send command with NULL EmulatorController. Bailing!");
      return;
    }

  m_pEmulatorController->swapMedia(iSlot_Number,sSlot);

}
