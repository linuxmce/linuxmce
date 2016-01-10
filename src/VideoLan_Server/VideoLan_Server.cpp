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
#include "VideoLan_Server.h"
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
#include "pluto_main/Define_CommandParameter.h"

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

VideoLan_Server *g_pVideoLan_Server = NULL;

void sh(int i) /* signal handler */
{
	if ( g_pVideoLan_Server && g_pVideoLan_Server->m_bQuit_get())
		return;

	int status = 0;
	pid_t pid = 0;


	pid = wait(&status);

	if ( g_pVideoLan_Server )
		g_pVideoLan_Server->ProcessExited(pid, status);
}
#endif


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
VideoLan_Server::VideoLan_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: VideoLan_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
, m_VideoLanMutex("videolan")
{
	m_VideoLanMutex.Init(NULL);
	m_bUseMultiCast=false;
#ifndef WIN32
	g_pVideoLan_Server = this;
#endif
}

//<-dceag-getconfig-b->
bool VideoLan_Server::GetConfig()
{
	if( !VideoLan_Server_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

#ifndef WIN32
	signal(SIGCHLD, sh); /* install handler */
#endif
	/* TODO -- DATA_Get_Network_Interfaces
	will give us a multicast address and range.  if specified,
	change GetVlanStream to use multicast */
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
VideoLan_Server::~VideoLan_Server()
//<-dceag-dest-e->
{
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);
	for(map<int,class VideoLanServerInstance *>::iterator it=m_mapVideoLanServerInstance.begin();it!=m_mapVideoLanServerInstance.end();++it)
	{
		// Kill any instances we spawned
		vector<void *> data;
		ProcessUtils::KillApplication(it->second->m_sSpawnName, data);
	}
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool VideoLan_Server::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

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
void VideoLan_Server::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void VideoLan_Server::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void VideoLan_Server::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	cout << "Need to implement command #37 - Play Media" << endl;
	cout << "Parm #13 - Filename=" << sMediaURL << endl;
	cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannt play nonexistant stream");
		return;
	}

	vector<string> vectIPs;
	vector<int> vectDevices;
	string::size_type pos=0;
	while(pos!=string::npos && pos<pVideoLanServerInstance->m_sStreamingTargets.size())
	{
		int PK_Device = atoi(StringUtils::Tokenize(pVideoLanServerInstance->m_sStreamingTargets,",",pos).c_str());
		DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(PK_Device);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d found at %p",PK_Device,pDeviceData_Base);
		while( pDeviceData_Base )
		{
			if( pDeviceData_Base->m_sIPAddress.size() )
			{
				vectIPs.push_back(pDeviceData_Base->m_sIPAddress);
				vectDevices.push_back(PK_Device);
				break;
			}
			pDeviceData_Base = pDeviceData_Base->m_pDevice_ControlledVia;
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found %d IP's",(int) vectIPs.size());

	if( vectIPs.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No IP's to stream to");
		return;
	}

	pVideoLanServerInstance->m_sFilename = sMediaURL;
	pVideoLanServerInstance->m_sCommandLine = "--intf\trc\t" + sMediaURL + 
		"\t--sout\t" + GetVlanStream(vectIPs,iStreamID);
	pVideoLanServerInstance->m_sSpawnName = "vlc_s_" + StringUtils::itos(iStreamID);


	if( !ProcessUtils::SpawnApplication("vlc",pVideoLanServerInstance->m_sCommandLine,pVideoLanServerInstance->m_sSpawnName) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to spawn server");
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Found %d Devices",(int) vectDevices.size());
	for(size_t s=0;s<vectDevices.size();++s)
	{
		DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,vectDevices[s],iPK_MediaType,iStreamID,"","udp:");
		SendCommand(CMD_Play_Media);
	}

	if( sMediaPosition.size() )
		CMD_Set_Media_Position(iStreamID,sMediaPosition);
}

string VideoLan_Server::GetVlanStream(vector<string> &vectIPs,int iStreamID)
{
	string sStream;
	if( vectIPs.size()==1 )
		sStream = "#standard{access=udp,mux=ts,url=" + vectIPs[0] + ",sap,name=\"s" + StringUtils::itos(iStreamID) + "\"}";
	else if( m_bUseMultiCast )
		sStream = "MultiCast";
	else
	{
		sStream = "#duplicate{";
		for(size_t s=0;s<vectIPs.size();++s)
		{
			if( s!=0 )
				sStream += ",";
			sStream += "dst=standard{access=udp,mux=ts,url=" + vectIPs[s] + ",sap,name=\"s" + StringUtils::itos(iStreamID) + "\"}";
		}
		sStream += "}";
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Returning stream %s",sStream.c_str());
	return sStream;
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void VideoLan_Server::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	cout << "Need to implement command #38 - Stop Media" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void VideoLan_Server::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Server::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Server::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	cout << "Need to implement command #41 - Change Playback Speed" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannt change playback on nonexistant stream");
		return;
	}

	string cmd="title_n\n";
	if( iMediaPlaybackSpeed<0 )
		cmd="title_p\n";

	if( ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"next\n")==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to stop VideoLan Server");
}

/*
void VideoLan_Server::CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage)
{
	int iStreamID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());

	cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannt skip fwd on nonexistant stream");
		return;
	}

	if( ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"chapter_n\n")==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to send chapter_n");
}


void VideoLan_Server::CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
	int iStreamID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str());

	cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannt skip fwd on nonexistant stream");
		return;
	}

	if( ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"chapter_p\n")==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to send chapter_p");
}
*/


//<-dceag-c249-b->

	/** @brief COMMAND: #249 - Start Streaming */
	/** Starts streaming */
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

void VideoLan_Server::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	cout << "Need to implement command #249 - Start Streaming" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #59 - MediaURL=" << sMediaURL << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = new VideoLanServerInstance(this,iStreamID,sStreamingTargets,sMediaURL);
	m_mapVideoLanServerInstance[iStreamID]=pVideoLanServerInstance;
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

void VideoLan_Server::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	cout << "Need to implement command #259 - Report Playback Position" << endl;
}

//<-dceag-c262-b->

	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void VideoLan_Server::CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c262-e->
{
	cout << "Need to implement command #262 - Stop Streaming" << endl;
	cout << "Parm #41 - StreamID=" << iStreamID << endl;
	cout << "Parm #105 - StreamingTargets=" << sStreamingTargets << endl;

	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannt play nonexistant stream");
		return;
	}

	vector<void *> data;
	if( ProcessUtils::KillApplication(pVideoLanServerInstance->m_sSpawnName,data)==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to stop VideoLan Server");

	m_mapVideoLanServerInstance.erase(iStreamID);

	string::size_type pos=0;
	while( pos<pVideoLanServerInstance->m_sStreamingTargets.size() )
	{
		string MediaPosition;
		int PK_Device = atoi(StringUtils::Tokenize(pVideoLanServerInstance->m_sStreamingTargets,",",pos).c_str());
		DCE::CMD_Stop_Media CMD_Stop_Media(m_dwPK_Device,PK_Device,iStreamID,&MediaPosition);
		SendCommand(CMD_Stop_Media);
	}
}

void VideoLan_Server::ProcessExited(int pid, int status)
{
	string applicationName;
	void *data;

	if ( ! ProcessUtils::ApplicationExited(pid, applicationName, data) )
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "VideoLan_Server::ProcessExited() Child with pid %d was not found in our internal data structure. Ignoring signal!", pid);
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "VideoLan_Server::ProcessExited() Child with pid %d terminated", pid);
}
//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void VideoLan_Server::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Set media position %d %s",iStreamID,sMediaPosition.c_str());
	PLUTO_SAFETY_LOCK(vlc,m_VideoLanMutex);

	VideoLanServerInstance *pVideoLanServerInstance = m_mapVideoLanServerInstance_Find(iStreamID);
	if( !pVideoLanServerInstance )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannt skip fwd on nonexistant stream");
		return;
	}

	int Pos=-2,Title=-2,Chapter=2,Subtitle=-2,Angle=-2,AudioTrack=-2;
	Pos = CalculatePosition(sMediaPosition,&Title,&Chapter,&Subtitle,&Angle,&AudioTrack);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VideoLan_Server::CMD_Set_Media_Position %d %d",Title,Chapter);

	if( Title!=-2 && ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"title " + StringUtils::itos(Title) + "\n")==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to send chapter_p");
	if( Chapter!=-2 && ProcessUtils::SendKeysToProcess(pVideoLanServerInstance->m_sSpawnName,"chapter " + StringUtils::itos(Chapter) + "\n")==false )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to send chapter_p");
}

int VideoLan_Server::CalculatePosition(string &sMediaPosition,int *Title,int *Chapter,int *Subtitle,int *Angle,int *AudioTrack)
{
	string::size_type pos=0;
	int iPos=0;

	if( Title )
	{
		pos = sMediaPosition.find(" TITLE:");
		if( pos!=string::npos )
			*Title = atoi( sMediaPosition.substr(pos+7).c_str() )+1;
	}

	if( Chapter )
	{
		pos = sMediaPosition.find(" CHAPTER:");
		if( pos!=string::npos )
			*Chapter = atoi( sMediaPosition.substr(pos+9).c_str() )+1;  // 
	}

	if( Subtitle )
	{
		pos = sMediaPosition.find(" SUBTITLE:");
		if( pos!=string::npos )
			*Subtitle = atoi( sMediaPosition.substr(pos+10).c_str() );
	}

	if( AudioTrack )
	{
		pos = sMediaPosition.find(" AUDIO:");
		if( pos!=string::npos )
			*AudioTrack = atoi( sMediaPosition.substr(pos+7).c_str() );
	}

	pos = sMediaPosition.find(" POS:");
	if( pos!=string::npos )
		iPos = atoi( sMediaPosition.substr(pos+5).c_str() );

	return iPos;
}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void VideoLan_Server::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
}
//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

void VideoLan_Server::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
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

void VideoLan_Server::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	//implement me
}
//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */

void VideoLan_Server::CMD_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
}
