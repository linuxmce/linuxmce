/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include <string> // map?
#include <map> // map
#include "../Xine_Player/XineMediaInfo.h"

//<-dceag-d-b->
#include "OMX_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#define RETRIES 50

// Additional required includes
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include "OMXPlayerStream.h"


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
OMX_Player::OMX_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: OMX_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDeviceData_MediaPlugin = NULL;
//	m_pDevice_App_Server = NULL;

	m_pNotificationSocket = new XineNotification_SocketListener(string("m_pNotificationSocket"));
	m_pNotificationSocket->m_bSendOnlySocket = true; // one second
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
OMX_Player::OMX_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: OMX_Player_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
	m_pDeviceData_MediaPlugin = NULL;
}

//<-dceag-dest-b->
OMX_Player::~OMX_Player()
//<-dceag-dest-e->
{
	EVENT_Playback_Completed("",0,false);

	if (m_pNotificationSocket)
	{
		delete m_pNotificationSocket;
		m_pNotificationSocket = NULL;
	}
}

void OMX_Player::PrepareToDelete ()
{
	m_pOMXPlayer->Stop(0);

	Command_Impl::PrepareToDelete ();
//	m_pDevice_App_Server = NULL;
	m_pDevice_OMX_Plugin = NULL;
	m_pDeviceData_MediaPlugin = NULL;
}

//<-dceag-getconfig-b->
bool OMX_Player::GetConfig()
{
	if( !OMX_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	m_pDevice_OMX_Plugin =
		m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_OMX_Plugin_CONST);
	if (!m_pDevice_OMX_Plugin)
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"I need an OMX plugin to function.");
		return false;
	}

//	m_pDevice_App_Server =
//		m_pData->FindFirstRelatedDeviceOfCategory (DEVICECATEGORY_App_Server_CONST, this);
//	if (!m_pDevice_App_Server)
//	{
//		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"I need an App Server to function.");
//		return false;
//	}

	m_pDeviceData_MediaPlugin =
		m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Media_Plugins_CONST);

	// TODO:: Need to get sAudioDevice, bPassthrough, sGpuDeInt data
	int PK_MD = m_pData->m_dwPK_Device_MD;
	m_sAudioDevice = m_pData->m_pEvent_Impl->GetDeviceDataFromDatabase(PK_MD,DEVICEDATA_Audio_settings_CONST);
	// m_sAudioDevice = "hdmi";
       	m_bPassthrough = false;
	if (m_sAudioDevice.length() >=2 && m_sAudioDevice[1] == '3')
	{
		m_sAudioDevice = m_sAudioDevice[0];
                m_bPassthrough = true;
	}

	m_sGpuDeInt = "";
	m_pOMXPlayer = new OMXPlayerStream(m_sAudioDevice, m_bPassthrough, m_sGpuDeInt, this, DATA_Get_Time_Code_Report_Frequency());

	if (!m_pOMXPlayer)
		return false;

	m_pOMXPlayer->Init();

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool OMX_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->!
//OMX_Player_Command *Create_OMX_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
//{
//	return new OMX_Player(pPrimaryDeviceCommand, pData, pEvent, pRouter);
//}
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
void OMX_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void OMX_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void OMX_Player::SomeFunction()
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

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void OMX_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
	cout << "Need to implement command #32 - Update Object Image" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #14 - Type=" << sType << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
}

void OMX_Player::NotifierCallback(void *pObject, int event)
{
	cout << "CALLBACK FUNCTION CALLED!!!  " << event << endl;

//	OMX_Player* pThis = (OMX_Player*) pObject;
//	pThis->EVENT_Playback_Completed(pThis->m_filename,pThis->m_iStreamID,false);
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

void OMX_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
//	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Implemented command #37 - Play Media" << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;

	sCMD_Result = "FAIL";

	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - TRYING iStreamID: %i, sMediaURL: %s", iStreamID, sMediaURL.c_str());


	uint64_t xMediaPosition = 0;
	xMediaPosition = USecFromTime(sMediaPosition);

	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - TRYING Requested start: %s, usec = %i", sMediaPosition.c_str(), xMediaPosition);


	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - TRYING to Play() from %s", sMediaPosition.c_str());

	if ( !m_pOMXPlayer->Play(iStreamID, sMediaURL, sMediaPosition, iPK_MediaType) ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - FAILED to Play(%s)", sMediaURL.c_str());
		return;
	}
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - SUCCESSFUL Play(%s)", sMediaURL.c_str());

	m_iStreamID = iStreamID;
	m_sMediaURL = sMediaURL;

	m_xDuration = m_pOMXPlayer->Get_Duration();
	string sDuration = TimeFromUSec(m_xDuration);
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - sDuration = %s", sDuration.c_str());


	// TODO: set up this data properly
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Play_Media - Send EVENT_Playback_Started()");
	string sMediaInfo = "";
	string sAudioInfo = "";
	string sVideoInfo = "";
	EVENT_Playback_Started(m_sMediaURL, m_iStreamID, sMediaInfo, sAudioInfo, sVideoInfo);

	m_bIsMute = false;

	sCMD_Result="OK";
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void OMX_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
//	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Implemented command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Stop_Media - Reporting postion:%s", m_pOMXPlayer->GetPosition().c_str());

	*sMediaPosition = m_pOMXPlayer->GetPosition();

	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Stop_Media - Calling Stop()");

	m_pOMXPlayer->Stop(iStreamID);
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void OMX_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
//	cout << "Need to implement command #39 - Pause Media" << endl;
	cout << "Implemented command #39 - Pause Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	if ( m_pOMXPlayer->Get_PlaybackStatus() == "Playing" )
		m_pOMXPlayer->Do_Pause();

	sCMD_Result = "OK";
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void OMX_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	cout << "Need to implement command #40 - Restart Media" << endl;
//	cout << "Implemented command #40 - Restart Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	if ( m_pOMXPlayer->Get_PlaybackStatus() == "Paused" )
		m_pOMXPlayer->Do_Pause();

	sCMD_Result = "OK";
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

void OMX_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
//	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Implemented command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
	cout << "Parm #220 - Report=" << bReport << endl;

	string sStatus = m_pOMXPlayer->Get_PlaybackStatus();
	if ( iMediaPlaybackSpeed == 1000 ) {
		if ( sStatus == "Playing" ) {
			m_pOMXPlayer->Do_Pause();
		}
		m_pOMXPlayer->Do_Pause();
	}
	else if ( iMediaPlaybackSpeed > 10 ) {
		m_pOMXPlayer->Do_FastForward();
	}
	else if ( iMediaPlaybackSpeed < 10 ) {
		m_pOMXPlayer->Do_Rewind();
	}
	else if ( iMediaPlaybackSpeed > 0 && iMediaPlaybackSpeed < 10 ) {
		m_pOMXPlayer->Do_IncreaseSpeed();
	}
	else if ( iMediaPlaybackSpeed < 0 && iMediaPlaybackSpeed > -10 ) {
		m_pOMXPlayer->Do_DecreaseSpeed();
	}
	else if ( iMediaPlaybackSpeed == 0 ) {
		if ( sStatus == "Playing" ) {
			m_pOMXPlayer->Do_Pause();
		}
	}
	sCMD_Result = "OK";
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void OMX_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
	cout << "Implemented command #42 - Jump to Position in Stream" << endl;
//	cout << "Need to implement command #42 - Jump to Position in Stream" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	const int64_t msec = 1000;
	const int64_t sec = msec * 1000;
//	const int64_t min = sec * 60;
//	const int64_t hour = min * 60;
//	const int64_t sec30 = min / 2;
//	const int64_t min30 = hour / 2;

	int64_t xValue = atoi(sValue_To_Assign.c_str());
	xValue = xValue * sec;

	cout << "Seeking: " << TimeFromUSec(xValue) << endl;
	m_pOMXPlayer->Do_Seek(xValue);
}

//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
//	cout << "Implemented command #63 - Skip Fwd - Channel/Track Greater" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

//	g_player_client->Next();
	sCMD_Result = "OK";
}

//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
//	cout << "Implemented command #64 - Skip Back - Channel/Track Lower" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

//	g_player_client->Previous();
	sCMD_Result = "OK";
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
//	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Implemented command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	// No playlist capability in omxplayer.
	// Jump FF ~10min or REW ~10min
	// FIXME: Need to check for seeking past end, player does not.

	const int64_t msec = 1000;
	const int64_t sec = msec * 1000;
	const int64_t min = sec * 60;
	const int64_t hour = min * 60;
//	const int64_t sec30 = min / 2;
	const int64_t min30 = hour / 2;

	int64_t xSeekVal = 0;
	int64_t xDuration = m_xDuration;

	xSeekVal = (min) + ((min) * (xDuration / min30));
	cout << "xSeekVal == " << xSeekVal << ", " << TimeFromUSec(xSeekVal) << endl;

	if ( sValue_To_Assign == "+1" ) {
		cout << "Calling - Seek(" << xSeekVal << ") - [" << TimeFromUSec(xSeekVal) << "]" << endl;
		m_pOMXPlayer->Do_Seek(xSeekVal);
	}
	else if ( sValue_To_Assign == "-1" ) {
		xSeekVal = xSeekVal/3;
		cout << "Calling - Seek(" << -xSeekVal << ") - [-" << TimeFromUSec(xSeekVal) << "]" << endl;
		m_pOMXPlayer->Do_Seek(-xSeekVal);
	}

	sCMD_Result = "OK";
}

//<-dceag-c92-b->

	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
//	cout << "Need to implement command #92 - Pause" << endl;
	cout << "Implemented command #92 - Pause" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	if ( m_pOMXPlayer->Get_PlaybackStatus() == "Playing" )
		m_pOMXPlayer->Do_Pause();

	sCMD_Result = "OK";
}

//<-dceag-c95-b->

	/** @brief COMMAND: #95 - Stop */
	/** Stop the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void OMX_Player::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
//	cout << "Need to implement command #95 - Stop" << endl;
	cout << "Implemented command #95 - Stop" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #203 - Eject=" << bEject << endl;

	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Stop - Calling Stop()");

	string sMediaPosition;
	CMD_Stop_Media(0, &sMediaPosition);
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void OMX_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c139-b->

	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	cout << "Need to implement command #190 - Enter/Go" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	cout << "Need to implement command #200 - Move Up" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	cout << "Need to implement command #201 - Move Down" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	cout << "Need to implement command #202 - Move Left" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	cout << "Need to implement command #203 - Move Right" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void OMX_Player::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
	cout << "Need to implement command #204 - 0" << endl;
}

//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void OMX_Player::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
	cout << "Need to implement command #205 - 1" << endl;
}

//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void OMX_Player::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
	cout << "Need to implement command #206 - 2" << endl;
}

//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void OMX_Player::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
	cout << "Need to implement command #207 - 3" << endl;
}

//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void OMX_Player::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
	cout << "Need to implement command #208 - 4" << endl;
}

//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void OMX_Player::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
	cout << "Need to implement command #209 - 5" << endl;
}

//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void OMX_Player::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
	cout << "Need to implement command #210 - 6" << endl;
}

//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void OMX_Player::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
	cout << "Need to implement command #211 - 7" << endl;
}

//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void OMX_Player::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
	cout << "Need to implement command #212 - 8" << endl;
}

//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void OMX_Player::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
	cout << "Need to implement command #213 - 9" << endl;
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Implemented command #259 - Report Playback Position" << endl;
//	cout << "Need to implement command #259 - Report Playback Position" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

	// FIXME: check stream id
	*sMediaPosition = m_pOMXPlayer->GetPosition();
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Report_Playback_Position - %s", sMediaPosition->c_str());
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void OMX_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	cout << "Need to implement command #412 - Set Media Position" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Set_Media_Positon - %s", sMediaPosition.c_str() );

	map<string,string> mapMediaInfo;

	string::size_type tokenPos = 0;
	string sCurValue = StringUtils::TrimSpaces(sMediaPosition);
	sCurValue = StringUtils::Tokenize(sMediaPosition, " ", tokenPos);
	while (sCurValue!="")
	{
		cout << "sCurValue = " << sCurValue << endl;

		string::size_type tokenPos2 = 0;
		std::string sKey = StringUtils::Tokenize(sCurValue, ":", tokenPos2);
		std::string sValue = StringUtils::Tokenize(sCurValue, ":", tokenPos2);

		cout << "Adding: " << sKey << "=" << sValue << endl;

		mapMediaInfo[sKey] = sValue;

		sCurValue = StringUtils::Tokenize(sMediaPosition, " ", tokenPos);
	}

	int64_t xTime = strtoull(mapMediaInfo["POS"].c_str(), NULL, 10) * 1000;
	int64_t xDuration = strtoull(mapMediaInfo["TOTAL"].c_str(), NULL, 10) * 1000;

	xTime = (xTime>0) ? xTime : 0;
	xTime = (xTime < xDuration) ? xTime : 0;

	int iAudioTrack = strtoul( mapMediaInfo["AUDIO"].c_str(), NULL, 10 );
	int iSubtitle =  strtoul( mapMediaInfo["SUBTITLE"].c_str(), NULL, 10 );

	cout << "POS: " << mapMediaInfo["POS"] << ", " << xTime << endl;
	cout << "AUDIO: " << mapMediaInfo["AUDIO"] << endl;
	cout << "SUBTITLE: " << mapMediaInfo["SUBTITLE"] << endl;
	cout << "TOTAL: " << mapMediaInfo["TOTAL"] << ", " << xDuration << endl;

	string sMediaURL = "";
	m_pOMXPlayer->Do_Seek(xTime);
//	m_pOMXPlayer->Do_SetPosition(sMediaURL, xTime);


	if ( m_pOMXPlayer->getCurrentSubtitle() != iSubtitle )
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Set_Media_Positon - Current Sub: %i, New Sub: %i", m_pOMXPlayer->getCurrentSubtitle(), iSubtitle );
	m_pOMXPlayer->setSubtitle(iSubtitle);

	if ( m_pOMXPlayer->getCurrentAudio() != iAudioTrack ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::CMD_Set_Media_Positon - Current Audio: %i, New Audio: %i", m_pOMXPlayer->getCurrentAudio(), iAudioTrack );
		m_pOMXPlayer->setAudio(iAudioTrack);
	}

	// TODO: implement angle/video track set
	//if getvideo != videotrack
	//	m_pOMXPlayer->Set_VideoTrack(XX);
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
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

void OMX_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	cout << "Implemented command #920 - Set Media ID" << endl;
//	cout << "Need to implement command #920 - Set Media ID" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;


       LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting stream media info to %s",sID.c_str());

	m_pOMXPlayer->m_sMediaType = "N";
	m_pOMXPlayer->m_iMediaID = -1;

	if (!sID.empty())
	{
		m_pOMXPlayer->m_sMediaType = sID[0];
		sID.erase(0,1);
		m_pOMXPlayer->m_iMediaID = atoi(sID.c_str());
	}
}


void OMX_Player::Log(string txt)
{
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"%s", txt.c_str());
//	cout << txt << endl;
}

void OMX_Player::StateChanged(OMXPlayerStream::STATE playerState, string sMediaURL, int iStreamID, bool bEvent)
{
	//  enum STATE { UNKNOWN, INITIALIZING, PLAYING, PAUSED, STOPPING, STOPPED, EXITING };

//	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - ******* - iStreamID: %i, MediaURL: %s", iStreamID, sMediaURL.c_str());

	if ( playerState == OMXPlayerStream::STATE::UNKNOWN ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - UNKNOWN - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE0" << endl;
	}
	else if ( playerState == OMXPlayerStream::STATE::INITIALIZING ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - INITIALIZING - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE1" << endl;
		stopped=false;
	}
	else if ( playerState == OMXPlayerStream::STATE::PLAYING ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - PLAYING - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE2" << endl;
		stopped=false;
	}
	else if ( playerState == OMXPlayerStream::STATE::PAUSED ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - PAUSED - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE3" << endl;
		stopped=false;
	}
	else if ( playerState == OMXPlayerStream::STATE::STOPPING ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - STOPPING - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE4" << endl;
	}
	else if ( playerState == OMXPlayerStream::STATE::STOPPED ) {
//		cout << "STATE5" << endl;
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - STOPPED - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
		if (bEvent) {
			LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - STOPPED - Sending EVENT_Playback_Completed(%i, %s)", (int)playerState, iStreamID, sMediaURL.c_str());
			EVENT_Playback_Completed(sMediaURL, iStreamID, false);
		}
		stopped=true;
	}
	else if ( playerState == OMXPlayerStream::STATE::EXITING ) {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - EXITING - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE6" << endl;
		stopped=true;
	}
	else {
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,"OMX_Player::StateChanged - %i - ERROR NO STATE - iStreamID: %i, MediaURL: %s", (int)playerState, iStreamID, sMediaURL.c_str());
//		cout << "STATE-NO" << endl;
	}
}


string OMX_Player::ZeroPad(int num, int width)
{
    std::ostringstream ss;
    ss << std::setw( width ) << std::setfill( '0' ) << num;
    return ss.str();
}

string OMX_Player::TimeFromMSec(uint64_t msecs) {
	string ret;
	ret = ZeroPad(msecs/1000/60/60,2) + ":" + ZeroPad(msecs/1000/60 % 60,2) + ":" + ZeroPad(msecs/1000 % 60,2);// + "." + ZeroPad(usecs/1000 % 1000,3);// + ":" + to_string(usecs % 1000);
	return ret;
}

string OMX_Player::TimeFromUSec(uint64_t usecs) {
	string ret;
	ret = ZeroPad(usecs/1000/1000/60/60,2) + ":" + ZeroPad(usecs/1000/1000/60 % 60,2) + ":" + ZeroPad(usecs/1000/1000 % 60,2);// + "." + ZeroPad(usecs/1000 % 1000,3);// + ":" + to_string(usecs % 1000);
	return ret;
}

int64_t OMX_Player::USecFromTime(string sTime) {

	const int64_t usec = 1;
	const int64_t msec = 1000;
	const int64_t sec = 1000 * msec;
	const int64_t min = sec * 60;
	const int64_t hour = min * 60;

	int64_t usecs = 0;
	int64_t iPlaceholderValue = hour;
	iPlaceholderValue = hour;
	string::size_type tokenPos = 0;
	string curValue = StringUtils::Tokenize(sTime, string(":"), tokenPos);
	while (curValue!="")
	{
		int iValue = atoi(curValue.c_str());
		usecs = usecs + (iPlaceholderValue * iValue);

		if (iPlaceholderValue == hour) {
			iPlaceholderValue = min;
		}
		else if (iPlaceholderValue == min) {
			iPlaceholderValue = sec;
		}
		else if (iPlaceholderValue == sec) {
			iPlaceholderValue = msec;
		}
		else if (iPlaceholderValue == msec) {
			iPlaceholderValue = usec;
		}

		curValue = StringUtils::Tokenize(sTime, string(":"), tokenPos);
	}
	return usecs;
}


void OMX_Player::ReportTimecodeViaIP(int iStreamID, int Speed)
{
//	Xine_Stream *pStream =  ptrFactory->GetStream( iStreamID );
//	if (pStream == NULL)
//	{
//		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Xine_Player::ReportTimecodeViaIP() stream is NULL");
//		return;
//	}

	if( !m_pDeviceData_MediaPlugin )
		return;

	// filling media info structure
	XineMediaInfo mediaInfo;

	mediaInfo.m_iSpeed = m_pOMXPlayer->Get_Speed();
	mediaInfo.m_iPositionInMilliseconds = m_pOMXPlayer->Get_Position() / 1000;
	mediaInfo.m_iTotalLengthInMilliseconds = m_pOMXPlayer->Get_Duration() / 1000;

	mediaInfo.m_iStreamID = m_pOMXPlayer->Get_iStreamID();
	mediaInfo.m_iTitle = m_pOMXPlayer->Get_Title();
	mediaInfo.m_iChapter = m_pOMXPlayer->Get_Chapter();
	mediaInfo.m_sFileName = m_pOMXPlayer->Get_CurrentFile();
	mediaInfo.m_sMediaType = m_pOMXPlayer->Get_MediaType();
	mediaInfo.m_iMediaID = m_pOMXPlayer->Get_MediaID();

	string sIPTimeCodeInfo = mediaInfo.ToString();

	Log("OMX_Player::ReportTimecodeViaIP - " + sIPTimeCodeInfo );
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"reporting timecode stream %d speed %d %s", iStreamID, Speed, sIPTimeCodeInfo.c_str() );
	EVENT_Media_Position_Changed(atoi(mediaInfo.m_sMediaType.c_str()), mediaInfo.m_sFileName, StringUtils::itos(mediaInfo.m_iMediaID), iStreamID, mediaInfo.FormatTotalTime(), mediaInfo.FormatCurrentTime(), Speed);

	m_pNotificationSocket->SendStringToAll( sIPTimeCodeInfo );
}


bool OMX_Player::Connect(int iPK_DeviceTemplate )
{
	if ( ! Command_Impl::Connect(iPK_DeviceTemplate) )
		return false;

	DeviceData_Base *pDevice = m_pData->GetTopMostDevice();
	m_sIPofMD = pDevice->m_sIPAddress;

//	if (!ptrFactory->StartupFactory())
//		return false;

	int iPort = DATA_Get_Port();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Configured port for time/speed notification is: %i, IP is %s", iPort, m_sIPofMD.c_str());

	m_pNotificationSocket->StartListening (iPort);

	EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not

	return true;
}

//<-dceag-c140-b->

	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c140-e->
{
	cout << "Implemented command #140 - Audio Track" << endl;
//	cout << "Need to implement command #140 - Audio Track" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

        LoggerWrapper::GetInstance()->Write(LV_WARNING, "OMX_Player::CMD_Audio_Track() with iStreamID %d.", iStreamID);

	int iCurrentAudio = m_pOMXPlayer->getCurrentAudio();
	if (sValue_To_Assign == "") {
		m_pOMXPlayer->setAudio( iCurrentAudio + 1 );
	}
	else
		m_pOMXPlayer->setAudio( atoi(sValue_To_Assign.c_str()) );
}

//<-dceag-c141-b->

	/** @brief COMMAND: #141 - Subtitle */
	/** Go to a subtitle */
		/** @param #5 Value To Assign */
			/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c141-e->
{
	cout << "Implemented command #141 - Subtitle" << endl;
//	cout << "Need to implement command #141 - Subtitle" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

        LoggerWrapper::GetInstance()->Write(LV_WARNING, "OMX_Player::CMD_Subtitle() with iStreamID %d.", iStreamID);

	int iCurrentSubtitle = m_pOMXPlayer->getCurrentSubtitle();
	if (sValue_To_Assign == "") {
		m_pOMXPlayer->setSubtitle( iCurrentSubtitle + 1 );
	}
	else
		m_pOMXPlayer->setSubtitle( atoi(sValue_To_Assign.c_str()) );
}

//<-dceag-c142-b->

	/** @brief COMMAND: #142 - Angle */
	/** Go to an angle */
		/** @param #5 Value To Assign */
			/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void OMX_Player::CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c142-e->
{
	cout << "Need to implement command #142 - Angle" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** volume up */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void OMX_Player::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
        cout << "Command #89 - Vol Up" << endl;
	cout << "Parm #72 - Repeat_Command=" << iRepeat_Command << endl;

	m_pOMXPlayer->setVolumeUp(iRepeat_Command, sCMD_Result);
	return;
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** volume down */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void OMX_Player::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
        cout << "Command #90 - Vol Down" << endl;
	cout << "Parm #72 - Repeat_Command=" << iRepeat_Command << endl;

	m_pOMXPlayer->setVolumeDown(iRepeat_Command, sCMD_Result);
	return;
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** mute */

void OMX_Player::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	if (m_bIsMute)
	{
	        cout << "Command #97 - Mute - UnMuting" << endl;

		m_pOMXPlayer->setUnMute(sCMD_Result);
		m_bIsMute = false;
	}
	else
	{
	        cout << "Command #97 - Mute - Muting" << endl;

		m_pOMXPlayer->setMute(sCMD_Result);
		m_bIsMute = true;
	}

	return;
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

void OMX_Player::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
        cout << "Need to implement command #XXX - Simulate Mouse Click" << endl;
}
//<-dceag-c81-b->

	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void OMX_Player::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
        cout << "Need to implement command #XXX - Navigate Next" << endl;
}
//<-dceag-c82-b->

	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

void OMX_Player::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
        cout << "Need to implement command #XXX - Navigate Prev" << endl;
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

void OMX_Player::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
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

	if (!m_pOMXPlayer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OMX_Player::CMD_Get_Video_Frame(sDisable_Aspect_Lock=%s,iStreamID=%d,iWidth=%d,iHeight=%d,iData_Size=%d) - m_pVLC == NULL",sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,iData_Size);
		sCMD_Result="ERROR";
		return;
	}

	if (!m_pOMXPlayer->GetScreenShot(iWidth,iHeight,sCMD_Result))
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

void OMX_Player::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
        cout << "Need to implement command #XXX - Goto Media Menu" << endl;
}
