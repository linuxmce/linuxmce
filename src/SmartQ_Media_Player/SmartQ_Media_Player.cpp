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
#include "SmartQ_Media_Player.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <iostream>
#include <sstream>

#define VLC_SOCKET_TIMEOUT	10  // SECONDS

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
SmartQ_Media_Player::SmartQ_Media_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: SmartQ_Media_Player_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_VLCMutex("vlc"), m_pVLCSocket(NULL)
{
	m_iVolume = 0;
	m_bMute = false;

	m_pNotificationSocket = new SmartQNotification_SocketListener(string("m_pNotificationSocket"));
	m_pNotificationSocket->m_bSendOnlySocket = true; // one second

}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
SmartQ_Media_Player::SmartQ_Media_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: SmartQ_Media_Player_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
	,m_VLCMutex("vlc"), m_pVLCSocket(NULL)
{
	m_iVolume = 0;
	m_bMute = false;
}

//<-dceag-dest-b->
SmartQ_Media_Player::~SmartQ_Media_Player()
//<-dceag-dest-e->
{

	if (m_pNotificationSocket)
	{
		delete m_pNotificationSocket;
		m_pNotificationSocket = NULL;
	}

	sendCommand("shutdown");	
}

//<-dceag-getconfig-b->
bool SmartQ_Media_Player::GetConfig()
{
	if( !SmartQ_Media_Player_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

  m_pDevice_SmartQ_Media_Plugin =
    m_pData->m_AllDevices.
    m_mapDeviceData_Base_FindFirstOfTemplate
    (DEVICETEMPLATE_SmartQ_Media_Plugin_CONST);
  if (!m_pDevice_SmartQ_Media_Plugin)
    {
      LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					    "I need a SmartQ Media plugin to function.");
      return false;
    }

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SmartQ_Media_Player::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
SmartQ_Media_Player_Command *Create_SmartQ_Media_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new SmartQ_Media_Player(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void SmartQ_Media_Player::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void SmartQ_Media_Player::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/**
 * override CreateChildren to spawn VLC 
 */
void SmartQ_Media_Player::CreateChildren()
{
	system("screen -d -m -h 3000 -S vlcSession vlc -I rc --lua-config \"rc={host='localhost:4212'}\"");
}

bool SmartQ_Media_Player::Connect(int iPK_DeviceTemplate )
{
    if ( ! Command_Impl::Connect(iPK_DeviceTemplate) )
		return false;

	DeviceData_Base *pDevice = m_pData->GetTopMostDevice();
	m_sIPofMD = pDevice->m_sIPAddress;

	m_pNotificationSocket->StartListening (12000);

	EVENT_Playback_Completed("",0,false);  // In case media plugin thought something was playing, let it know that there's not

	return true;
}

/**
 * Accessors for: m_iVolume - The Current volume of the string.
 */
int SmartQ_Media_Player::getVolume()
{
	return m_iVolume;
}

void SmartQ_Media_Player::setVolume(int iNewVolume)
{
	m_iVolume = iNewVolume;
	cout << "New Volume is: " << iNewVolume << endl;
}

/**
 * Send a command to the rc.lua remote control port and spit back the
 * response.
 */
string SmartQ_Media_Player::sendCommand(const char *Cmd, bool bExpectResponse )
{
	string sResponse;
	char ch=0;

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Going to send command %s",Cmd);
	
	if (m_pVLCSocket == NULL)
	{
		m_pVLCSocket = new PlainClientSocket("localhost:4212");
		if( !m_pVLCSocket->Connect() )
		{
			delete m_pVLCSocket;
			m_pVLCSocket = NULL;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to connect to VLC");
			return "";
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connected");
	}
	

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== Before prompt");
	// Wait for VLC's console prompt.
	do
	{
		if ( !m_pVLCSocket->ReceiveData( 1, &ch, VLC_SOCKET_TIMEOUT ) ) 
		{
/*			delete m_pVLCSocket;
			m_pVLCSocket = NULL;*/
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Timeout waiting for VLC prompt: socket %d, buffer %c", m_pVLCSocket->m_Socket, ch);
			return "";
		}
	} while(ch!='>');
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== After VLC Prompt");
	if( !m_pVLCSocket->SendString(Cmd) )
	{
		delete m_pVLCSocket;
		m_pVLCSocket = NULL;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send string");
		return "";
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"========== Command is sent");

	if (bExpectResponse)
	{
	
		char *pData = NULL;
		int nSize = 0;

                if(!m_pVLCSocket->ReceiveDataDelimited(nSize, pData, '\n') || NULL == pData)
                {
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC not ok with command");
              		return ""; 
		}

                //we want a string
		sResponse.assign(pData);

                // Done with it...
                delete [] pData;
                pData = NULL;
	
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"========== Got the answer: %s", sResponse.c_str());
		
		sResponse = StringUtils::TrimSpaces(sResponse);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC Responded: %s",sResponse.c_str());
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"========= No Response Required.");	
		sResponse=""; // nothing.	
	}	

	return sResponse;

}

/**
 * Given a path, /home/public/data/videos/SomeDisk [xx]/somemedia.avi ...
 * translate the path to an appropriate /mnt/device/xx/public/data/videos/somemedia.avi
 * so that it can be mounted and found. Returns the transformed path as a string.
 */
string SmartQ_Media_Player::getRealPath(string originalPath)
{
	if (originalPath.find("://") == string::npos)
	{
		// no URL, append /mnt/ to the beginning.
		return "/mnt/" + originalPath;	
	}	
	else
	{
		// a URL was found, simply pass it back untranslated.
		return originalPath;	
	}
}

int SmartQ_Media_Player::getSeconds()
{
	return m_iSeconds;
}

void SmartQ_Media_Player::setSeconds(int iSeconds)
{
	m_iSeconds = iSeconds;
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

void SmartQ_Media_Player::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);
	string sAddString = "add " + getRealPath(sMediaURL) + "\r";
	int iNewVolume;
	string sNewVolume = "1234567890";
	istringstream ssNewVolume;

	// Get current volume and set our state appropriately.
	sNewVolume = sendCommand("volume\r",true);
	cout << "Volume is now: " << sNewVolume << endl;	
	ssNewVolume.str(sNewVolume);
	ssNewVolume >> iNewVolume;

	sendCommand("clear\r");
	sendCommand(sAddString.c_str());
//	sendCommand("fullscreen\r");

	setVolume(iNewVolume);	
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void SmartQ_Media_Player::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
        PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);	
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

	sendCommand("clear\r");
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void SmartQ_Media_Player::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
        PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);
	sendCommand("pause\r");
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void SmartQ_Media_Player::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
        PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);
	sendCommand("play\r");
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

void SmartQ_Media_Player::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
        PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);
	bool		bHasModif; // If a modifier is used.
	bool		bHasPlus;  // Is a + used?
	bool 		bHasMinus; // Is a - used?
	int 		iSeconds;  // Converted from string value.
 	istringstream 	isConvert; // Used for string to int conversion.
	stringstream    ssFromInt; // Used for int to string conversion.
	string		sNewSeconds; // The final time value to set.

	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	bHasPlus = (sValue_To_Assign.find("+") != string::npos);
	bHasMinus = (sValue_To_Assign.find("-") != string::npos);
	bHasModif = bHasPlus || bHasMinus;

	if ( bHasModif )	// Is a modifier + or - present
	{
		if ( bHasPlus  )
		{
			// Go forward X number of seconds.
			sValue_To_Assign.erase(0,1);  // Remove the +
			isConvert.str(sValue_To_Assign);
			isConvert >> iSeconds;
			setSeconds(getSeconds() + iSeconds);	
		}
	
		if ( bHasMinus )
		{
			// Go backward X number of seconds.
			sValue_To_Assign.erase(0,1);  // Remove the -
			isConvert.str(sValue_To_Assign);
			isConvert >> iSeconds;
			setSeconds(getSeconds() - iSeconds);
		}
	
		// Convert int back into string
		ssFromInt << getSeconds();
		sNewSeconds = ssFromInt.str(); 
	}
	else
	{
		// If no modifier sent, simply pass the value.
		sNewSeconds = sValue_To_Assign;	
	}

	string sSeekCommand = "seek " + sNewSeconds + "\r";	
	sendCommand(sSeekCommand.c_str());

}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void SmartQ_Media_Player::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
	cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void SmartQ_Media_Player::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
	cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c140-b->

	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void SmartQ_Media_Player::CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c140-e->
{
	cout << "Need to implement command #140 - Audio Track" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c141-b->

	/** @brief COMMAND: #141 - Subtitle */
	/** Go to a subtitle */
		/** @param #5 Value To Assign */
			/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void SmartQ_Media_Player::CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c141-e->
{
	cout << "Need to implement command #141 - Subtitle" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c142-b->

	/** @brief COMMAND: #142 - Angle */
	/** Go to an angle */
		/** @param #5 Value To Assign */
			/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void SmartQ_Media_Player::CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c142-e->
{
	cout << "Need to implement command #142 - Angle" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
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

void SmartQ_Media_Player::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
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

void SmartQ_Media_Player::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
	cout << "Need to implement command #920 - Set Media ID" << endl;
	cout << "Parm #10 - ID=" << sID << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}


//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** volume up */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void SmartQ_Media_Player::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
        PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);	
	string sNewVolume;
	istringstream ssNewVolume;
	int iNewVolume;
	sendCommand("volup\r");
	sNewVolume = sendCommand("volume");
	ssNewVolume.str(sNewVolume);
	ssNewVolume >> iNewVolume;
	setVolume(iNewVolume);		
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** volume down */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void SmartQ_Media_Player::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
        PLUTO_SAFETY_LOCK(vlcLock, m_VLCMutex);
        string sNewVolume;
        istringstream ssNewVolume;
        int iNewVolume;
        sendCommand("voldown\r");
        sNewVolume = sendCommand("volume");
        ssNewVolume.str(sNewVolume);
        ssNewVolume >> iNewVolume;
        setVolume(iNewVolume);
}

bool SmartQ_Media_Player::getMute() 
{
	return m_bMute;
}

void SmartQ_Media_Player::setMute(bool bNewMute)
{
	m_bMute = bNewMute;
}

void SmartQ_Media_Player::toggleMute()
{
	setMute(!getMute());
}

//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** mute */

void SmartQ_Media_Player::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->        
{
	string sCurrentVolume;
	stringstream ssCurrentVolume;
	
	ssCurrentVolume << getVolume();
	sCurrentVolume = ssCurrentVolume.str();
	toggleMute();
	string sVolumeCommand = "volume " + (getMute() ? "0" : sCurrentVolume);
	sendCommand(sVolumeCommand.c_str());
}

