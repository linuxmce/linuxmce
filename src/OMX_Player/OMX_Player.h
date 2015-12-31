/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef OMX_Player_h
#define OMX_Player_h

//	DCE Implemenation for #2259 OMX Player

#include "Gen_Devices/OMX_PlayerBase.h"
//<-dceag-d-e->

#include "DCE/SocketListener.h"
#include "DCE/ServerSocket.h"

#include "OMXPlayerStream.h"


//<-dceag-decl-b->
namespace DCE
{
	class OMX_Player : public OMX_Player_Command
	{
//<-dceag-decl-e->

private:
		// Private member variables
		DeviceData_Base *m_pDevice_OMX_Plugin;
//		DeviceData_Base *m_pDevice_App_Server;

		OMXPlayerStream *m_pOMXPlayer;

		string m_sIPofMD;
		string m_sAudioDevice;
		bool m_bPassthrough;
		string m_sGpuDeInt;
		bool m_bIsMute;

		bool stopped = true;
		int64_t m_xDuration;

		// Private methods
		string ZeroPad(int num, int width);
		string TimeFromUSec(uint64_t usecs);
		string TimeFromMSec(uint64_t msecs);
		int64_t USecFromTime(string sTime);

public:
		int m_iStreamID;
		string m_sMediaURL;

		void StateChanged(OMXPlayerStream::STATE playerState, string sFilename, int iStreamID, bool bError);
		void Log(string txt);

		// Public member variables
		virtual void PrepareToDelete();

//<-dceag-const-b->
public:
		// Constructors/Destructor
		OMX_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~OMX_Player();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		static void NotifierCallback(void* pObject, int event);
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		OMX_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Alsa_Output_Device();
	string DATA_Get_Subtitles();
	void DATA_Set_Subtitles(string Value,bool bUpdateDatabase=false);
	string DATA_Get_Audio_Tracks();
	void DATA_Set_Audio_Tracks(string Value,bool bUpdateDatabase=false);
	string DATA_Get_Angles();
	void DATA_Set_Angles(string Value,bool bUpdateDatabase=false);
	int DATA_Get_Time_Code_Report_Frequency();
	string DATA_Get_Name();
	string DATA_Get_Deinterlacing_Mode();
	int DATA_Get_Port();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors);
	void EVENT_Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo);
	void EVENT_Media_Position_Changed(int iFK_MediaType,string sMRL,string sID,int iStream_ID,string sDateTime,string sCurrent_Time,int iSpeed);
	void EVENT_Subtitle_Track_Changed(string sID,int iStream_ID,string sName,string sFormat,string sLanguage);
	void EVENT_Audio_Track_Changed(string sID,int iStream_ID,string sName,string sFormat,string sLanguage);
	void EVENT_Video_Track_Changed(string sID,int iStream_ID,string sName,string sFormat,string sLanguage);

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


	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simlate a mouse click at a certain position on the screen */
		/** @param #11 Position X */
			/** position X */
		/** @param #12 Position Y */
			/** position Y */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID) { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Back_ChannelTrack_Lower(int iStreamID) { string sCMD_Result; CMD_Skip_Back_ChannelTrack_Lower(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #81 - Navigate Next */
	/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

	virtual void CMD_Navigate_Next(int iStreamID) { string sCMD_Result; CMD_Navigate_Next(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #82 - Navigate Prev */
	/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

	virtual void CMD_Navigate_Prev(int iStreamID) { string sCMD_Result; CMD_Navigate_Prev(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType) { string sCMD_Result; CMD_Goto_Media_Menu(iStreamID,iMenuType,sCMD_Result,NULL);};
	virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #92 - Pause */
	/** Pause the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Pause(int iStreamID) { string sCMD_Result; CMD_Pause(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #95 - Stop */
	/** Stop the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

	virtual void CMD_Stop(int iStreamID,bool bEject) { string sCMD_Result; CMD_Stop(iStreamID,bEject,sCMD_Result,NULL);};
	virtual void CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #97 - Mute */
	/** mute */

	virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
	virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

	virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
	virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #139 - Play */
	/** Play the media */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Play(int iStreamID) { string sCMD_Result; CMD_Play(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #140 - Audio Track */
	/** Go to an audio track */
		/** @param #5 Value To Assign */
			/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Audio_Track(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Audio_Track(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #141 - Subtitle */
	/** Go to a subtitle */
		/** @param #5 Value To Assign */
			/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Subtitle(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Subtitle(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #142 - Angle */
	/** Go to an angle */
		/** @param #5 Value To Assign */
			/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Angle(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Angle(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the currently highlighted menu item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #204 - 0 */
	/** 0 */

	virtual void CMD_0() { string sCMD_Result; CMD_0(sCMD_Result,NULL);};
	virtual void CMD_0(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #205 - 1 */
	/** 1 */

	virtual void CMD_1() { string sCMD_Result; CMD_1(sCMD_Result,NULL);};
	virtual void CMD_1(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #206 - 2 */
	/** 2 */

	virtual void CMD_2() { string sCMD_Result; CMD_2(sCMD_Result,NULL);};
	virtual void CMD_2(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #207 - 3 */
	/** 3 */

	virtual void CMD_3() { string sCMD_Result; CMD_3(sCMD_Result,NULL);};
	virtual void CMD_3(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #208 - 4 */
	/** 4 */

	virtual void CMD_4() { string sCMD_Result; CMD_4(sCMD_Result,NULL);};
	virtual void CMD_4(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #209 - 5 */
	/** 5 */

	virtual void CMD_5() { string sCMD_Result; CMD_5(sCMD_Result,NULL);};
	virtual void CMD_5(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #210 - 6 */
	/** 6 */

	virtual void CMD_6() { string sCMD_Result; CMD_6(sCMD_Result,NULL);};
	virtual void CMD_6(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #211 - 7 */
	/** 7 */

	virtual void CMD_7() { string sCMD_Result; CMD_7(sCMD_Result,NULL);};
	virtual void CMD_7(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #212 - 8 */
	/** 8 */

	virtual void CMD_8() { string sCMD_Result; CMD_8(sCMD_Result,NULL);};
	virtual void CMD_8(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #213 - 9 */
	/** 9 */

	virtual void CMD_9() { string sCMD_Result; CMD_9(sCMD_Result,NULL);};
	virtual void CMD_9(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Back_Prior_Menu(int iStreamID) { string sCMD_Result; CMD_Back_Prior_Menu(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage);


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
        void ReportTimecodeViaIP(int iStreamID, int Speed);
        bool Connect(int iPK_DeviceTemplate );

private:
	DeviceData_Base *m_pDeviceData_MediaPlugin;


	// From Xine_Player
	// socket listener for playback info notification
	class XineNotification_SocketListener : public SocketListener
	{
	public:
		XineNotification_SocketListener(string sName):SocketListener(sName){};

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

	XineNotification_SocketListener *m_pNotificationSocket;
};



//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
