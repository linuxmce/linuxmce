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
#ifndef Slim_Server_Streamer_h
#define Slim_Server_Streamer_h

//	DCE Implemenation for #53 Slim Server Streamer

#include "Gen_Devices/Slim_Server_StreamerBase.h"
//<-dceag-d-e->

#include "SqueezeBox_Player.h"
#include "DCE/SocketListener.h"
#include "DCE/ServerSocket.h"
#include "Xine_Player/XineMediaInfo.h"

#include <map>

using namespace std;

typedef enum _StreamState {
	STATE_UNDEFINED,
	STATE_STOP,
	STATE_PAUSE,
	STATE_PLAY,
	STATE_CHANGING,
	STATE_FFWD_2000,
	STATE_FFWD_4000,
	STATE_FFWD_8000,
	STATE_FFWD_16000,
	STATE_FFWD_32000,
	STATE_FFWD_64000,
	STATE_REW_250,
	STATE_REW_500,
	STATE_REW_1000,
	STATE_REW_2000,
	STATE_REW_4000,
	STATE_REW_8000,
	STATE_REW_16000,
	STATE_REW_32000,
	STATE_REW_64000
} StreamStateType;

//<-dceag-decl-b->
namespace DCE
{
	class Slim_Server_Streamer : public Slim_Server_Streamer_Command
	{
//<-dceag-decl-e->
		pthread_t               m_threadPlaybackCompletedChecker;

        pluto_pthread_mutex_t   m_dataStructureAccessMutex;
		pluto_pthread_mutex_t	m_stateChangedMutex;

		pthread_cond_t			m_stateChangedCondition;

        // map<int, list<string> > m_mapStreamsToSqueezeBoxesPlayers;
        string  m_strSlimServerCliAddress;
        int m_iSlimServerCliPort;

        // Semantics:
        //   streamID ->
        //              [
        //                  state { playing, stopped }
        //                  a vector of SqueezeBox_Player objects ( the devices on which this stream is playing ).
        //              ]
        map<int, pair<StreamStateType, vector<DeviceData_Base *> > > m_mapStreamsToPlayers;
        map<int, time_t > m_mapStreamsToTimeStreamStarted;

        // the socket used to communicate with the server
        SOCKET m_iServerSocket;

        // Private member variables

        // Private methods
        bool ConnectToSlimServerCliCommandChannel();
        string SendReceiveCommand(string command, bool bLogCommand = true);

	void TurnOffAllSqueezeboxes();
	bool isValidTimecode(string strResultTimecode);
	int stateToSpeed(StreamStateType state);

	XineMediaInfo parseTimeCode(string strResultTimecode, string strDurationtimecode, int iStreamID, StreamStateType state);

        static void *checkForPlaybackCompleted(void *pSlim_Server_Streamer);

		string FindControllingMacForStream(int iStreamID);

		void SetStateForStream(int iStreamID, StreamStateType newState);
		StreamStateType GetStateForStream(int iStreamID);

		string getMacAddressForDevice(DeviceData_Base *pDevice);
		void StartSlimServer();

public:
        // Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Slim_Server_Streamer(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Slim_Server_Streamer();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void PostConnect();

		virtual void OnQuit();
//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Only_One_Per_PC();
	int DATA_Get_Port();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors);
	void EVENT_Media_Position_Changed(int iFK_MediaType,string sMRL,string sID,int iStream_ID,string sDateTime,string sCurrent_Time,int iSpeed);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

	virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { string sCMD_Result; CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL) { string sCMD_Result; CMD_Play_Media(iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

	virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition) { string sCMD_Result; CMD_Stop_Media(iStreamID,sMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

	virtual void CMD_Pause_Media(int iStreamID) { string sCMD_Result; CMD_Pause_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

	virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
		/** @param #220 Report */
			/** If true, report this speed to the user on the OSD */

	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,bReport,sCMD_Result,NULL);};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

	virtual void CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_to_Position_in_Stream(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #89 - Vol Up */
	/** volume up */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

	virtual void CMD_Vol_Up(int iRepeat_Command) { string sCMD_Result; CMD_Vol_Up(iRepeat_Command,sCMD_Result,NULL);};
	virtual void CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #90 - Vol Down */
	/** volume down */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

	virtual void CMD_Vol_Down(int iRepeat_Command) { string sCMD_Result; CMD_Vol_Down(iRepeat_Command,sCMD_Result,NULL);};
	virtual void CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #97 - Mute */
	/** mute */

	virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
	virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

	virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
	virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets) { string sCMD_Result; CMD_Start_Streaming(iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str(),sStreamingTargets.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

	virtual void CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition) { string sCMD_Result; CMD_Report_Playback_Position(iStreamID,sText,sMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

	virtual void CMD_Stop_Streaming(int iStreamID,string sStreamingTargets) { string sCMD_Result; CMD_Stop_Streaming(iStreamID,sStreamingTargets.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Menu(string sText,int iStreamID) { string sCMD_Result; CMD_Menu(sText.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

	virtual void CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio) { string sCMD_Result; CMD_Set_Aspect_Ratio(iStreamID,sAspect_Ratio.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

	virtual void CMD_Set_Zoom(int iStreamID,string sZoom_Level) { string sCMD_Result; CMD_Set_Zoom(iStreamID,sZoom_Level.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

	virtual void CMD_Set_Media_ID(string sID,int iStreamID) { string sCMD_Result; CMD_Set_Media_ID(sID.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

	// socket listener for playback info notification
	
	class SlimServerStreamerNotification_SocketListener : public SocketListener
	{
	public:
		SlimServerStreamerNotification_SocketListener(string sName):SocketListener(sName){};
	
		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage ){};
		virtual bool ReceivedString( Socket *pSocket, string sLine, int nTimeout = - 1 )
		{
			std::cout << "Socket got: " << sLine << std::endl; 
			return true; 
		};
		
		void SendStringToAll(string sString)
		{
			PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );
			for(std::vector<ServerSocket *>::iterator i=m_vectorServerSocket.begin(); i!=m_vectorServerSocket.end(); i++)
			{
				if ((*i)->SendString(sString))
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending time code %s to %s",sString.c_str(),(*i)->m_sHostName.c_str());
				}
				else
				{
					std::cout << "Not sent timecode to " << (*i)->m_sHostName<<  std::endl;
				}
			}
		}
	};

	SlimServerStreamerNotification_SocketListener *m_pNotificationSocket;

    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->

//<-dceag-const2-b->!

