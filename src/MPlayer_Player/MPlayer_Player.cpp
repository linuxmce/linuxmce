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
#include "MPlayer_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#include <sstream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <cmath>
#include "pluto_main/Define_MediaType.h"
#include "../Xine_Player/XineMediaInfo.h"
			 
#include <signal.h>
			 

MPlayer_Player *g_pPlayerInstance=NULL;
int g_iMplayerChildPID=0;

void SignalHandler_PIPE (int signum)
{
	if (signum!=SIGPIPE)
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SignalHandler_PIPE: received not SIGPIPE signal: %i, ignoring", signum);
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SignalHandler_PIPE: received SIGPIPE signal, ignoring");
}

void SignalHandler_CHLD (int signum, siginfo_t *info, void *context)
{
	if (info->si_signo!=SIGCHLD)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SignalHandler_CHLD: received not SIGCHLD signal: %i, ignoring", info->si_signo);
		return;
	}
	
	if (info->si_pid!=g_iMplayerChildPID)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SignalHandler_CHLD: received SIGCHLD signal, but with not from mplayer child (from %i, we expect from %i), ignoring", info->si_pid, g_iMplayerChildPID );
		return;
	}
	
	if (info->si_code!=CLD_EXITED && info->si_code!=CLD_KILLED && info->si_code!=CLD_DUMPED)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SignalHandler_CHLD: received SIGCHLD signal, but with not expected sig_code %i, ignoring", info->si_code);
		return;
	}
	
	if (g_pPlayerInstance)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SignalHandler_CHLD: mplayer child exited, reloading MPlayer DCE device");
		// do reload
		g_pPlayerInstance->OnReload();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SignalHandler_CHLD: mplayer child exited, but cannot reload MPlayer DCE device as g_pPlayerInstance is NULL");
	}	
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
MPlayer_Player::MPlayer_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: MPlayer_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_bMediaPaused = false;
	m_bPlayerEngineInitialized = false;
	m_pPlayerEngine = NULL;
	m_iCurrentStreamID = -1;
	m_bRunPlayerEnginePoll = false;
	m_bMediaOpened = false;
	m_fCurrentFileTime = 0;
	m_fCurrentFileLength = 0;
	
	m_iCurrentAudioTrack = 0;
	
	m_pNotificationSocket = new TimecodeNotification_SocketListener(string("m_pTimecodeNotificationSocket"));
	m_pNotificationSocket->m_bSendOnlySocket = true; // one second
	
	sGraphicsDriver = "";
	sGraphicsDeinterlace = "";
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
MPlayer_Player::MPlayer_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: MPlayer_Player_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
	m_bMediaPaused = false;
	m_bPlayerEngineInitialized = false;
	m_pPlayerEngine = NULL;
	m_iCurrentStreamID = -1;
	m_bRunPlayerEnginePoll = false;
	m_bMediaOpened = false;
	m_fCurrentFileTime = 0;
	m_fCurrentFileLength = 0;
}

//<-dceag-dest-b->
MPlayer_Player::~MPlayer_Player()
//<-dceag-dest-e->
{
	if (m_bMediaOpened)
	{
		m_bMediaOpened = false;
	}
	
	if (m_bRunPlayerEnginePoll)
	{
		m_bRunPlayerEnginePoll = false;
		pthread_join(m_tPlayerEnginePollThread, NULL);
	}
	
	EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not
	signal(SIGCHLD, SIG_IGN);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer - removed SIGCHLD handler");
	
	if (m_pPlayerEngine)
	{
		delete m_pPlayerEngine;
		m_pPlayerEngine = NULL;
	}
	
	if (m_pNotificationSocket)
	{
		delete m_pNotificationSocket;
		m_pNotificationSocket = NULL;
	}
}

//<-dceag-getconfig-b->
bool MPlayer_Player::GetConfig()
{
	if( !MPlayer_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool MPlayer_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
MPlayer_Player_Command *Create_MPlayer_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new MPlayer_Player(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void MPlayer_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void MPlayer_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void MPlayer_Player::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Skip_Fwd_ChannelTrack_Greater is not implemented");
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Skip_Back_ChannelTrack_Lower is not implemented");
}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Pause received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Pause aborts because Player Engine is not initialized");
		return;
	}
	
	if (!m_bMediaPaused)
	{
		m_pPlayerEngine->ExecuteCommand("pause");
		m_bMediaPaused = true;
	}
}

//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** Stop the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void MPlayer_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Stop received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Stop aborts because Player Engine is not initialized");
		return;
	}
	
	//m_bMediaOpened = false;
	m_pPlayerEngine->StopPlayback();
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Play aborts because Player Engine is not initialized");
		return;
	}
	
	if (m_bMediaPaused)
	{
		m_pPlayerEngine->ExecuteCommand("pause");
		m_bMediaPaused = false;
	}
}

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void MPlayer_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Simulate_Keypress is not implemented");
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

void MPlayer_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Update_Object_Image is not implemented");
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

void MPlayer_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media called for media type %i stream %i, MediaURL %s", iPK_MediaType, iStreamID, sMediaURL.c_str());
	
	m_iCurrentPlaybackSpeed = 0;
	
	if (!m_bPlayerEngineInitialized)
	{
		InitializePlayerEngine();
	}
	else
	{
		//EVENT_Playback_Completed(m_sCurrentFileName, m_iCurrentStreamID, false);
	}
	
	m_vCurrentPlaylist.clear();
	list<string> vFiles;
	string sInitialFile;
	
	// populate full list of files in HD-DVD/BD folder
	// if HDDVD is passed
	// sLargestFile is set to largest file
	
	switch (iPK_MediaType)
	{
		case MEDIATYPE_pluto_HDDVD_CONST:
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media detected HD-DVD disk");
				// even for custom file, the type is passed as MEDIATYPE_pluto_HDDVD_CONST
				int iLargestFilePosition = -1;
				string sFolder;				
				
				if ( !StringUtils::EndsWith(sMediaURL, ".EVO", true) )
				{
					sFolder = sMediaURL+"/HVDVD_TS";
				}
				else
				{
					sFolder = FileUtils::BasePath(sMediaURL);
				}
				
				SmartLoadPlaylist(sFolder, "*.evo", vFiles, sInitialFile, iLargestFilePosition);
				
				if ( StringUtils::EndsWith(sMediaURL, ".EVO", true) )
				{
					sInitialFile = sMediaURL;
				}
			}
			break;
			
		case MEDIATYPE_pluto_BD_CONST:
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media detected Bluray disk");
				// even for custom file, the type is passed as MEDIATYPE_pluto_BD_CONST
				int iLargestFilePosition = -1;
				string sFolder;				
				
				if ( !StringUtils::EndsWith(sMediaURL, ".M2TS", true) )
				{
					sFolder = sMediaURL+"/BDMV/STREAM";
				}
				else
				{
					sFolder = FileUtils::BasePath(sMediaURL);
				}
				
				SmartLoadPlaylist(sFolder, "*.m2ts", vFiles, sInitialFile, iLargestFilePosition);
			
				if ( StringUtils::EndsWith(sMediaURL, ".M2TS", true) )
				{
					sInitialFile = sMediaURL;
				}
			}
			break;
			
		case MEDIATYPE_pluto_StoredVideo_CONST:
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media detected stored video");
				
				vFiles.push_back(sMediaURL);
				sInitialFile = sMediaURL;
			}
			break;
			
		default:
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Play_Media unknown media, aborting");
			EVENT_Playback_Completed(sMediaURL, iStreamID, true);
			return;
			break;
	}
	
	// log
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media complete playlist contains items:");
	for (list<string>::iterator li=vFiles.begin(); li!=vFiles.end(); ++li)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media item: %s", li->c_str());
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media complete playlist ends here");
	
	string sMediaToPlay = sInitialFile;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media file to play before start position analysis: %s", sMediaToPlay.c_str());
	
	// if the actual file to play is passed as FILE:name.ext in position information, extracting it
	sMediaToPlay = ExtractFileFromPosition(sMediaPosition, sMediaToPlay);
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Play_Media file to play after start position analysis: %s", sMediaToPlay.c_str());
	
	m_iCurrentStreamID = iStreamID;
	m_sCurrentFileName = sMediaToPlay;
	
	vector<string> vFilesToPlay;
	//CopyListToVectorFromItem(vFiles, vFilesToPlay, sMediaToPlay);
	vFilesToPlay.push_back(sMediaToPlay);
	
	// initializing list
	//copy( vFiles.begin(), vFiles.end(), back_inserter(m_vCurrentPlaylist) );
	m_vCurrentPlaylist.clear();
	m_vCurrentPlaylist.push_back(sMediaToPlay);
	
	//TODO check for playback start errors
	CreateVideoConfigFiles(vFilesToPlay);
	if (m_pPlayerEngine->StartPlaylist(vFilesToPlay) )
	{
		m_bMediaOpened = true;
		m_fCurrentFileLength = 0.0;
		m_fCurrentFileTime = 0.0;
		m_bMediaPaused = false;
		m_iCurrentPlaybackSpeed = 1000;
	}
	else
	{
		// fire errors
	}
	
	// TODO should we sleep?
	Sleep(1500);
	
	CMD_Set_Media_Position(iStreamID, sMediaPosition);
	
	string sMediaInfo, sAudioInfo, sVideoInfo;
	
	// codec information is not used
	
	sAudioInfo = m_pPlayerEngine->GetAudioCodec();
	sVideoInfo = m_pPlayerEngine->GetVideoCodec();
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::EVENT_Playback_Started(streamID=%i)", iStreamID);
	EVENT_Playback_Started(sMediaURL,iStreamID,sMediaInfo,sAudioInfo,sVideoInfo);

	m_iCurrentAudioTrack = 0;
	UpdateTracksInfo();
	
	//TODO implement setting media subtitles info
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void MPlayer_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Stop_Media received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Stop aborts because Player Engine is not initialized");
		return;
	}
	
	m_iCurrentPlaybackSpeed = 0;
	//m_bMediaOpened = false;
	*sMediaPosition = GetPlaybackPosition();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Stop_Media last position is %s", sMediaPosition->c_str());
	m_pPlayerEngine->StopPlayback();
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void MPlayer_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Pause_Media received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Pause_Media aborts because Player Engine is not initialized");
		return;
	}
	
	if (!m_bMediaPaused)
	{
		m_pPlayerEngine->ExecuteCommand("pause");
		m_bMediaPaused = true;
		m_iCurrentPlaybackSpeed = 0;
	}
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void MPlayer_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Restart_Media received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Restart_Media aborts because Player Engine is not initialized");
		return;
	}
	
	if (m_bMediaPaused)
	{
		m_pPlayerEngine->ExecuteCommand("pause");
		m_bMediaPaused = false;
		m_iCurrentPlaybackSpeed = 1000;
	}
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

void MPlayer_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Change_Playback_Speed called for stream %i, speed %i", iStreamID, iMediaPlaybackSpeed);
	
	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Change_Playback_Speed aborts because Player Engine is not initialized");
		return;
	}
	
	
	if (iMediaPlaybackSpeed < 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reverse playback is not supported by MPlayer");
		return;
	}
	
	if (iMediaPlaybackSpeed == 0)
	{
		CMD_Pause_Media(iStreamID);
		return;
	}
	
	if (iMediaPlaybackSpeed > 0)
	{
		m_iCurrentPlaybackSpeed = iMediaPlaybackSpeed;
		string sSpeed = StringUtils::itos(iMediaPlaybackSpeed/1000) + "." + StringUtils::itos(iMediaPlaybackSpeed%1000);
		m_pPlayerEngine->ExecuteCommand("speed_set " + sSpeed);
		
		// for the case when prev speed was 0
		CMD_Restart_Media(iStreamID);
	}
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void MPlayer_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Jump_to_Position_in_Stream received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Jump_to_Position_in_Stream aborts because Player Engine is not initialized");
		return;
	}
	
	// relative seek
	if ( StringUtils::StartsWith(sValue_To_Assign, "+") || StringUtils::StartsWith(sValue_To_Assign, "+") )
	{
		m_pPlayerEngine->ExecuteCommand("seek "+sValue_To_Assign);
	}
	else
	{
		m_pPlayerEngine->ExecuteCommand("seek "+sValue_To_Assign+" 2");
	}
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Jump_Position_In_Playlist is not implemented");
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void MPlayer_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Guide is not implemented");
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

void MPlayer_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Start_Streaming is not implemented");
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

void MPlayer_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Report_Playback_Position received");
	
	*sMediaPosition = GetPlaybackPosition();
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void MPlayer_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Set_Media_Position received: %s", sMediaPosition.c_str());
	
	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Set_Media_Position aborts because Player Engine is not initialized");
		return;
	}
	
	string sNewFile = ExtractFileFromPosition(sMediaPosition, m_sCurrentFileName);
	
	float fPosition = 0;
	bool bSetPosition = false;

	vector<string> vParams;
	Tokenize(sMediaPosition, vParams);
	for (vector<string>::iterator i=vParams.begin(); i!=vParams.end(); ++i)
	{
		if (i->substr(0,4) == "POS:")
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Set_Media_Position requested to seek %s", i->c_str());
			int iPosition = atoi(i->substr(4).c_str());
			fPosition = (float) iPosition / 1000;
			bSetPosition = true;
		}
	}
	
	if (sNewFile != m_sCurrentFileName)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Set_Media_Position jumping to different file in set: %s", sNewFile.c_str() );
		
		vector<string> vFiles;
		CopyVectorToVectorFromItem(m_vCurrentPlaylist, vFiles, sNewFile);
		//TODO process errors if any
		CreateVideoConfigFiles(vFiles);
		m_pPlayerEngine->StartPlaylist(vFiles);		
		m_sCurrentFileName = sNewFile;
	}
	
	if (bSetPosition)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Set_Media_Position seeking to new time position");
		m_pPlayerEngine->Seek(fPosition, MPlayerEngine::SEEK_ABSOLUTE_TIME);
	}
	
	// switching audio track if info present
	int iAudioTrack = 0;
	string::size_type sPos = sMediaPosition.find("AUDIO:");
	if (sPos != string::npos)
	{
		iAudioTrack = atoi(sMediaPosition.substr(sPos+6).c_str());
		if (iAudioTrack == -1)
			iAudioTrack = 0;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Play_Media - audio track #%i will be used", iAudioTrack);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Play_Media - audio track info not found");
	}
	
	// 0th track is started by default
	if (m_iCurrentAudioTrack!=iAudioTrack)
	{
		m_iCurrentAudioTrack = iAudioTrack;
		CMD_Audio_Track(StringUtils::itos(m_iCurrentAudioTrack), 0);
	}
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Menu is not implemented");
}

//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void MPlayer_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Set_Aspect_Ratio received");

	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Set_Aspect_Ratio aborts because Player Engine is not initialized");
		return;
	}
	
	string sMPlayerAspect;
	
	if (sAspect_Ratio == "auto")
	{
		sMPlayerAspect = "";
	}
	else if (sAspect_Ratio == "1:1")
	{
		sMPlayerAspect = "1";
	}
	else if (sAspect_Ratio == "4:3")
	{
		sMPlayerAspect = "1.33333";
	}
	else if (sAspect_Ratio == "16:9")
	{
		sMPlayerAspect = "1.77778";
	}
	else if (sAspect_Ratio == "2.11:1")
	{
		sMPlayerAspect = "2.11";
	}
	
	m_pPlayerEngine->ExecuteCommand("switch_ratio " + sMPlayerAspect);
}

//<-dceag-c917-b->

	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

void MPlayer_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Zoom is not implemented");
}

//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void MPlayer_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Set_Media_ID is not implemented");
}

bool MPlayer_Player::ParseAudioConfig(string sAudioSettings, string &sAlsaDevice, bool &bUsePassThrough)
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MPlayer_Player::ParseAudioConfig  M/D Audio Settings: %s", sAudioSettings.c_str());
	
	if (sAudioSettings=="")
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MPlayer_Player::ParseAudioConfig  M/D Audio Settings are empty, assuming 'M'");
		sAudioSettings = "M";
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MPlayer_Player::ParseAudioConfig  M/D Audio Settings: %s", sAudioSettings.c_str());

	// temporary workaround for #3995
	if (sAudioSettings=="S3")
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "MPlayer_Player::ParseAudioConfig  M/D Audio Settings are broken (S3), assuming 'S'");
		sAudioSettings = "S";
	}
	
	bool updateConfig = true;
	sAlsaDevice = "plug:dmix";
	bUsePassThrough = false;
	
	for (uint i=0; i<sAudioSettings.length(); i++)
	{
		switch (sAudioSettings[i])
		{
			case 'C':
			case 'O':
				sAlsaDevice = "asym_spdif";
				break;
		
			case 'S':		
			case 'L':
				sAlsaDevice = "plug:dmix";
				break;
		
			case '3':
				bUsePassThrough = true;
				break;
		
			case 'M':
				updateConfig = false;
				break;
		
			default:
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "MPlayer_Player::ParseAudioConfig  Unknown audio settings flag: '%c'", sAudioSettings[i]);
		}
	}
	
	if (!updateConfig)
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MPlayer_Player::ParseAudioConfig  Flag 'M' found, we won't override the defaults from /root/.mplayer/config");
	}
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MPlayer_Player::ParseAudioConfig  Final audio settings: audio device=%s, use passthrough=%s, use these settings=%s", sAlsaDevice.c_str(), bUsePassThrough?"yes":"no", updateConfig?"yes":"no");
	
	return updateConfig;
}

void MPlayer_Player::InitializePlayerEngine()
{
	// get audio settings from the M/D
	string sAudioSettings = m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_MD,
			DEVICEDATA_Audio_settings_CONST);
	
	// parse audio settings
	string sAudioDevice;
	bool bUsePassthrough;
	bool bUseAudioSettings = ParseAudioConfig(sAudioSettings, sAudioDevice, bUsePassthrough);
	
	//TODO wrap into try-catch block
	sGraphicsDriver = DATA_Get_Hardware_acceleration();
	sGraphicsDeinterlace = DATA_Get_Deinterlacing_Mode();
	m_pPlayerEngine = new MPlayerEngine(bUseAudioSettings, sAudioDevice, bUsePassthrough, sGraphicsDriver, sGraphicsDeinterlace);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Started MPlayer slave instance");
	m_bPlayerEngineInitialized = true;
	
	int iCounter=100;
	
	// giving engine time to finish playing black file
	while (m_pPlayerEngine->GetEngineState()!=MPlayerEngine::PLAYBACK_FINISHED && iCounter!=0)
	{
		Sleep(100);
		iCounter--;
	}
	
	if (m_pPlayerEngine->GetEngineState()==MPlayerEngine::PLAYBACK_FINISHED)
	{
		g_pPlayerInstance = this;
		g_iMplayerChildPID = m_pPlayerEngine->GetChildPID();
		
		struct sigaction new_action;
		new_action.sa_sigaction = SignalHandler_CHLD;
		new_action.sa_flags = SA_SIGINFO /*| SA_NOCLDSTOP*/;		
		sigaction(SIGCHLD, &new_action, NULL);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer - installed SIGCHLD handler");
		
		signal(SIGPIPE, SignalHandler_PIPE);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer - installed SIGPIPE handler");

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer engine initialized and ready to go");
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MPlayer engine didn't set up within 10 seconds, something is wrong");
	
	m_bRunPlayerEnginePoll = true;
	pthread_create(&m_tPlayerEnginePollThread, NULL, &PlayerEnginePoll, this);
}

void *DCE::PlayerEnginePoll(void *pInstance)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlayerEnginePoll - started");
	
	MPlayer_Player* pThis = (MPlayer_Player*) pInstance;
	
	while (pThis->m_bRunPlayerEnginePoll)
	{
		Sleep(500);
//		LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlayerEnginePoll - loop");
		string sPlayedFile;
		
		if (pThis->m_bMediaOpened)
		{
			if ( pThis->m_pPlayerEngine->GetEngineState() != MPlayerEngine::PLAYBACK_STARTED )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "PlayerEnginePoll - detected end of playlist");
				
				pThis->EVENT_Playback_Completed(pThis->m_sCurrentFileName, pThis->m_iCurrentStreamID, false);

				pThis->m_fCurrentFileTime = 0.0;
				pThis->m_fCurrentFileLength = 0.0;	
				pThis->m_bMediaOpened = false;	
			}
			else
			{
				string sFile = pThis->m_pPlayerEngine->GetCurrentFile();
				if ( sFile != pThis->m_sCurrentFileName )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "PlayerEnginePoll - detected playlist entry change: %s -> %s",
						pThis->m_sCurrentFileName.c_str(), sFile.c_str());

					pThis->m_fCurrentFileLength = 0.0;
					string sMediaInfo, sAudioInfo, sVideoInfo;
	
					sAudioInfo = pThis->m_pPlayerEngine->GetAudioCodec();
					sVideoInfo = pThis->m_pPlayerEngine->GetVideoCodec();
	
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::EVENT_Playback_Started(streamID=%i)", 
							pThis->m_iCurrentStreamID);
					
					pThis->EVENT_Playback_Started(sFile,pThis->m_iCurrentStreamID,sMediaInfo,sAudioInfo,sVideoInfo);
					pThis->m_sCurrentFileName = sFile;
					
					pThis->m_iCurrentAudioTrack = 0;
					pThis->UpdateTracksInfo();
				}

				if ( ! pThis->m_bMediaPaused ) {					
					// Only query position when NOT pausing. Otherwise MPlayer un-pauses
					pThis->m_fCurrentFileTime = pThis->m_pPlayerEngine->GetCurrentPosition();
					
					// querying this only if it is unknown, to minimize delay
					if (pThis->m_fCurrentFileLength == 0.0)
						pThis->m_fCurrentFileLength = pThis->m_pPlayerEngine->GetFileLength();
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlayerEnginePoll - time %.1f of %.1f", pThis->m_fCurrentFileTime, pThis->m_fCurrentFileLength);
				}
			}
			
			
		}
		else
		{
			if ( pThis->m_pPlayerEngine->GetEngineState() != MPlayerEngine::PLAYBACK_FINISHED )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "PlayerEnginePoll - detected start of playlist");
				pThis->m_fCurrentFileTime = 0.0;
				pThis->m_fCurrentFileLength = 0.0;	
				
				pThis->m_bMediaOpened = true;
				sPlayedFile = pThis->m_pPlayerEngine->GetCurrentFile();
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "PlayerEnginePoll - engine plays file %s", sPlayedFile.c_str() );
			}
		}
		
		// sending timecode
		if (pThis->m_pPlayerEngine->GetEngineState() == MPlayerEngine::PLAYBACK_STARTED)
		{
			XineMediaInfo mediaInfo;
			mediaInfo.m_iSpeed = pThis->m_iCurrentPlaybackSpeed;
			mediaInfo.m_iStreamID = pThis->m_iCurrentStreamID;
			mediaInfo.m_iTitle = 0;
			mediaInfo.m_iChapter = 0;
			mediaInfo.m_sFileName = pThis->m_sCurrentFileName;
			mediaInfo.m_iPositionInMilliseconds = (int)(pThis->m_fCurrentFileTime*1000.0);
			mediaInfo.m_iTotalLengthInMilliseconds = (int)(pThis->m_fCurrentFileLength*1000.0);
			
			//TODO fix if necessary
			mediaInfo.m_sMediaType = "N";
			mediaInfo.m_iMediaID = -1;
			
			string sIPTimeCodeInfo = mediaInfo.ToString();
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"reporting timecode stream %d speed %d %s", mediaInfo.m_iStreamID, mediaInfo.m_iSpeed, sIPTimeCodeInfo.c_str() );
			
			pThis->m_pNotificationSocket->SendStringToAll( sIPTimeCodeInfo );
		}
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PlayerEnginePoll - exited");
	
	return NULL;
}

string MPlayer_Player::GetPlaybackPosition()
{
	// TODO retrieve subtitles and audio track
	const int buf_size = 1024;
	char buf[buf_size];

	string sShortFile = FileUtils::FilenameWithoutPath(m_sCurrentFileName);
	
	snprintf(buf, buf_size, " POS:%i SUBTITLE:-1 AUDIO:%i TOTAL:%i FILE:%s", (int)(m_fCurrentFileTime*1000), m_iCurrentAudioTrack, (int)(m_fCurrentFileLength*1000), sShortFile.c_str());
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::GetPlaybackPosition position info: %s", buf);
	
	return buf;
}

bool MPlayer_Player::Connect(int iPK_DeviceTemplate )
{
	if ( ! Command_Impl::Connect(iPK_DeviceTemplate) )
		return false;

	DeviceData_Base *pDevice = m_pData->GetTopMostDevice();
	m_sIPofMD = pDevice->m_sIPAddress;

	int iPort = DATA_Get_Port();
	if (iPort==0)
		iPort = 12010;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Configured port for time/speed notification is: %i, IP is %s", iPort, m_sIPofMD.c_str());

	m_pNotificationSocket->StartListening (iPort);	
	
	EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not
	
	return true;
}

void MPlayer_Player::SmartLoadPlaylist(string sFolder, string sExtensions, list<string>& vFiles, string &sLargestFile, int& iLargestFilePosition)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::SmartLoadPlaylist scanning folder %s", sFolder.c_str());
	
	list<string> vNames;
	FileUtils::FindFiles(vNames, sFolder, sExtensions);
	
	if (vNames.empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::SmartLoadPlaylist no files found with mask %s", sExtensions.c_str() );
		return;
	}
	else
		vNames.sort();
	
	long int iMaxSize = -1;
	int iPosition = 0;
	
	// HD-DVD workaround to start from feature_1
	bool bHDDVD_Workaround = false;
	string sHDDVD_Workaround;
	int iHDDVD_Workaround=-1;
	int isHDDVD_Workaround=0;
	//
	
	for (list<string>::iterator li=vNames.begin(); li!=vNames.end(); ++li)
	{
		string sFullName = sFolder+"/"+ *li;
		vFiles.push_back(sFullName);
		long int iFileSize = FileUtils::FileSize(sFullName);
		if ( iFileSize > iMaxSize )
		{
			iMaxSize = iFileSize;
			sLargestFile = sFullName;
			iLargestFilePosition = iPosition;
		}
		
		// HD-DVD workaround to start from feature_1
		if (*li == "FEATURE_1.EVO" || *li == "feature_1.evo")
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::SmartLoadPlaylist FEATURE_1.EVO found, will use HD-DVD workaround" );
			sHDDVD_Workaround = sFullName;
			iHDDVD_Workaround = iPosition;
			bHDDVD_Workaround = true;
			isHDDVD_Workaround = iFileSize;
		}
		//
		
		iPosition++;
	}
	
	if (bHDDVD_Workaround)
	{
		sLargestFile = sHDDVD_Workaround;
		iLargestFilePosition = iHDDVD_Workaround;
		iMaxSize = isHDDVD_Workaround;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::SmartLoadPlaylist winner is %s with size %i on position %i", sLargestFile.c_str(), iMaxSize, iLargestFilePosition );
}

string MPlayer_Player::ExtractFileFromPosition(string sMediaPosition, string sMediaToPlay)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::ExtractFileFromPosition sMediaPosition=|%s| sMediaToPlay=%s", sMediaPosition.c_str(), sMediaToPlay.c_str() );
	string::size_type sPos = sMediaPosition.find("FILE:");
	if (sPos == string::npos)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::ExtractFileFromPosition - position file info not found, reverting to default fallback");
		return sMediaToPlay;
	}
	else
	{
		string sPositionFile = FileUtils::BasePath(sMediaToPlay) + "/" + sMediaPosition.substr(sPos+5);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::ExtractFileFromPosition - position file is %s", sPositionFile.c_str() );
		return sPositionFile;
	}
}

void MPlayer_Player::CopyListToVectorFromItem(const list<string> &vList, vector<string> &vVector, const string &sItem)
{
	bool bCopy=false;
	for (list<string>::const_iterator li=vList.begin(); li!=vList.end(); ++li)
	{
		bCopy = bCopy || (*li == sItem);
		if (bCopy)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CopyListToVectorFromItem added to player list item: %s", li->c_str());
			vVector.push_back(*li);
		}
	}
}

void MPlayer_Player::CopyVectorToVectorFromItem(const vector<string> &vList, vector<string> &vVector, const string &sItem)
{
	bool bCopy=false;
	for (vector<string>::const_iterator vi=vList.begin(); vi!=vList.end(); ++vi)
	{
		bCopy = bCopy || (*vi == sItem);
		if (bCopy)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CopyVectorToVectorFromItem added to player vist item: %s", vi->c_str());
			vVector.push_back(*vi);
		}
	}
}

void MPlayer_Player::CreateVideoConfigFiles(const vector<string> &vFiles)
{
	for (vector<string>::const_iterator vi=vFiles.begin(); vi!=vFiles.end(); ++vi)
	{
		// EVO files requires forced demuxer to be set to lavf 
		// to speedup startup, plus trying to use multiple threads
		// if they are present/supported. Also, doing the aggressive 
		// caching to work better over network share
		if (StringUtils::EndsWith(*vi, ".EVO", true))
		{
			vector<string> vOptions;
			vOptions.push_back("demuxer=lavf");
			vOptions.push_back("lavdopts=fast=yes:threads=2");
			vOptions.push_back("cache=131072");
			vOptions.push_back("cache-min=50");
	
			string sConfigName = "/root/.mplayer/" + FileUtils::FilenameWithoutPath(*vi) + ".conf";
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CreateVideoConfigFiles writing options to: %s", vi->c_str());
			FileUtils::WriteVectorToFile(sConfigName, vOptions);
		}
		
		
		// M2TS uses internal mplayer demuxer, so we just doing the 
		// aggressive caching to work better over network share
		if (StringUtils::EndsWith(*vi, ".M2TS", true))
		{
			vector<string> vOptions;
			vOptions.push_back("lavdopts=fast=yes:threads=2");
			vOptions.push_back("cache=131072");
			vOptions.push_back("cache-min=50");
			
			// bluray is 24 fps
			vOptions.push_back("fps=24");
	
			string sConfigName = "/root/.mplayer/" + FileUtils::FilenameWithoutPath(*vi) + ".conf";
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CreateVideoConfigFiles writing options to: %s", vi->c_str());
			FileUtils::WriteVectorToFile(sConfigName, vOptions);
		}
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

void MPlayer_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
}

//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MPlayer_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
}

//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void MPlayer_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
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

void MPlayer_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "MPlayer_Player::CMD_Get_Video_Frame received");
  
  stringstream out;
  out << pMessage->m_dwPK_Device_From;
  string sDeviceFrom = out.str();

  string sCmd = "scrot -u -t 1280x720 /tmp/mplayer_videoframe_"+sDeviceFrom+".jpg";
  system(sCmd.c_str());
  sCmd = "mv /tmp/mplayer_videoframe_"+sDeviceFrom+"-thumb.jpg /tmp/mplayer_videoframe_"+sDeviceFrom+".jpg";
  system(sCmd.c_str());
  *sFormat = "2";  // JPEG
  
  size_t size;
  *pData = FileUtils::ReadFileIntoBuffer("/tmp/mplayer_videoframe_"+sDeviceFrom+".jpg",size);
  *iData_Size = size;

  FileUtils::DelFile("/tmp/mplayer_videoframe_"+sDeviceFrom+".jpg");

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

void MPlayer_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Goto_Media_Menu is not implemented");
}

//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_EnterGo is not implemented");
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Move_Up is not implemented");
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Move_Down is not implemented");
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Move_Left is not implemented");
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Move_Right is not implemented");
}

void MPlayer_Player::UpdateTracksInfo()
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::UpdateTracksInfo - detecting count of tracks");
	
	string sInfoFile = "/tmp/MPlayer_" + StringUtils::itos(m_dwPK_Device)+".ffmpeg.info";
	string sCMD = "/usr/bin/ffmpeg -i \"" + m_sCurrentFileName + "\" 2>&1 | grep \"Stream.*Audio\" > " + sInfoFile;
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::UpdateTracksInfo - detect command: %s", sCMD.c_str());
	int iRetCode = system(sCMD.c_str());
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::UpdateTracksInfo - detect command returned code: %i", iRetCode);
	
	vector<string> sOutput;
	FileUtils::ReadFileIntoVector(sInfoFile, sOutput);
	
	if (sOutput.empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::UpdateTracksInfo - no audio tracks found");
	}
	else
	{
		int iTracksCount = sOutput.size();
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::UpdateTracksInfo - found %i audio tracks", iTracksCount);

		string sAudioTracks = StringUtils::itos(m_iCurrentAudioTrack)+"\n";
	
		for (int i=0; i<iTracksCount; i++)
		{
			string::size_type sPos = sOutput[i].find(", ");
			string sCodecInfo;
			
			if (sPos != string::npos)
			{
				sCodecInfo = sOutput[i].substr(sPos);
				StringUtils::TrimSpaces(sCodecInfo);
			}
			
			sAudioTracks += "Track-" + StringUtils::itos(i+1) + sCodecInfo + "\n";
		}
	
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::UpdateTracksInfo - sending info about audio tracks: %s", sAudioTracks.c_str());

		DATA_Set_Audio_Tracks(sAudioTracks);
	}
	
}
//<-dceag-c140-b->

	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c140-e->
{
	if (!m_bPlayerEngineInitialized)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Audio_Track aborts because Player Engine is not initialized");
		return;
	}
	
	int iAudioTrack = atoi(sValue_To_Assign.c_str());
	m_iCurrentAudioTrack = iAudioTrack;
	
	// mplayer tracks start from
	//iAudioTrack += 1;
	
	m_pPlayerEngine->ExecuteCommand("switch_audio " + StringUtils::itos(iAudioTrack));
	
	UpdateTracksInfo();
}

//<-dceag-c141-b->

	/** @brief COMMAND: #141 - Subtitle */
	/** Go to a subtitle */
		/** @param #5 Value To Assign */
			/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c141-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Subtitle is not implemented");
}

//<-dceag-c142-b->

	/** @brief COMMAND: #142 - Angle */
	/** Go to an angle */
		/** @param #5 Value To Assign */
			/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void MPlayer_Player::CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c142-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MPlayer_Player::CMD_Angle is not implemented");
}
