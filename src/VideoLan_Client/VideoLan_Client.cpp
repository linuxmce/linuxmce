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
#include "VideoLan_Client.h"
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
#include "utilities/linux/RatpoisonHandler.h"

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/keysym.h"
#include <X11/extensions/XTest.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define VLC_WINDOW_NAME "Unnamed"

#include "PlutoUtils/ProcessUtils.h"
/*
class RatPoisonWrapper : public RatpoisonHandler<RatPoisonWrapper>
{
    Display *display;

public:
    RatPoisonWrapper(Display *display) : display(display) {}
    Display *getDisplay() { return display; }
    void commandRatPoison(string command) { RatpoisonHandler<RatPoisonWrapper>::commandRatPoison(command); }
};
*/

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
VideoLan_Client *g_pVideoLan_Client = NULL;

void sh(int i) /* signal handler */
{
    if ( g_pVideoLan_Client && g_pVideoLan_Client->m_bQuit_get())
		return;

    int status = 0;
    pid_t pid = 0;


    pid = wait(&status);

    if ( g_pVideoLan_Client )
        g_pVideoLan_Client->ProcessExited(pid, status);
}
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VideoLan_Client::VideoLan_Client(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VideoLan_Client_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pRatWrapper = NULL;
    m_iVideoLanWindowId = 0;
	m_dwPK_Device_VideoLan_Server = 0;
#ifndef WIN32
	g_pVideoLan_Client = this;
#endif
}

//<-dceag-getconfig-b->
bool VideoLan_Client::GetConfig()
{
	if( !VideoLan_Client_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

//    m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));
    m_iVideoLanWindowId = 0;
	m_dwPK_Device_VideoLan_Server = 0;
#ifndef WIN32
    signal(SIGCHLD, sh); /* install handler */
#endif
	return true;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
VideoLan_Client::VideoLan_Client(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: VideoLan_Client_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
VideoLan_Client::~VideoLan_Client()
//<-dceag-dest-e->
{
	// Kill any instances we spawned
	vector<void *> data;
	ProcessUtils::KillApplication("vlc_c", data);
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VideoLan_Client::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
VideoLan_Client_Command *Create_VideoLan_Client(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new VideoLan_Client(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void VideoLan_Client::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VideoLan_Client::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

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

void VideoLan_Client::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #13 - Filename=" << sMediaURL << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	m_dwPK_Device_VideoLan_Server = pMessage->m_dwPK_Device_From;

	string sCommand = "--intf\trc\t-f\t" + sMediaURL;
/*
	if ( ! m_pRatWrapper )
        m_pRatWrapper = new RatPoisonWrapper(XOpenDisplay(getenv("DISPLAY")));
*/
	if( ProcessUtils::SpawnApplication("vlc", sCommand.c_str(), "vlc_c")==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to start videolan client");

    selectWindow();
/*
    locateVlcFrontendWindow(DefaultRootWindow(m_pRatWrapper->getDisplay()));

	VideoLanClientInstance *pVideoLanClientInstance = new VideoLanClientInstance(this,iStreamID,sCommand);
	pthread_create(&pVideoLanClientInstance->m_pthread_t, NULL, SpawnVideoLanClient, (void *) pVideoLanClientInstance);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sleeping for 5 seconds");
	Sleep(5000);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"active frame");
    makeActive("frame");
	Sleep(5000);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"active Frame");
    makeActive("Frame");
	Sleep(5000);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"active Unnamed");
    makeActive("Unnamed");
	Sleep(5000);
*/
}

bool VideoLan_Client::checkXServerConnection()
{
	/*
	if ( ! m_pRatWrapper || ! m_pRatWrapper->getDisplay() )
	{
		if ( !m_pRatWrapper )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The ratpoison command handler value is null. This usually means the XServer connection is down or useless");
			return false;
		}

		if ( !m_pRatWrapper->getDisplay() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The Display* value in the ratpoison command handler is null. This ususally means the XServer connection is down or useless");
			return false;
		}
	}
*/
	return true;
}

void VideoLan_Client::selectWindow()
{
//    m_pRatWrapper->commandRatPoison(":select " VLC_WINDOW_NAME);
}

bool VideoLan_Client::checkWindowName(long unsigned int window, string windowName)
{
    XTextProperty text;
/*
	if ( ! checkXServerConnection())
		return false;

	if ( XGetWMName (m_pRatWrapper->getDisplay(), window, &text) && windowName == string((const char*)text.value) )
        return true;
*/
    return false;
}

bool VideoLan_Client::locateVlcFrontendWindow(long unsigned int window)
{
    Window parent_win, root_win, *child_windows;
    unsigned int num_child_windows;

	if ( ! checkXServerConnection())
		return false;

    if ( checkWindowName(window, VLC_WINDOW_NAME ) )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found window id: 0x%x", window );
        m_iVideoLanWindowId = window;
        return true;
    }
/*
    XQueryTree(m_pRatWrapper->getDisplay(), (Window)window, &root_win, &parent_win, &child_windows, &num_child_windows);

    for ( unsigned int i = 0; i < num_child_windows; i++ )
        if ( locateVlcFrontendWindow(child_windows[i]) )
            return true;
*/
    /* we need to free the list of child IDs, as it was dynamically allocated */
    /* by the XQueryTree function.                                            */
    XFree(child_windows);
    return false;
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void VideoLan_Client::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	vector<void *> data;
	if( ProcessUtils::KillApplication("vlc_c" + StringUtils::itos(iStreamID),data)==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to stop VideoLan Client");
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void VideoLan_Client::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Client::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Client::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
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

void VideoLan_Client::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
}

void VideoLan_Client::ProcessExited(int pid, int status)
{
	string applicationName;
	void *data;

	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "VideoLan_Client::ProcessExited() Child with pid %d was not found in our internal data structure. Ignoring signal!", pid);
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "VideoLan_Client::ProcessExited() Child with pid %d terminated", pid);
}
//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void VideoLan_Client::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
}
//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void VideoLan_Client::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
}

//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void VideoLan_Client::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
}

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void VideoLan_Client::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	//implement me
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */

void VideoLan_Client::CMD_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
}
