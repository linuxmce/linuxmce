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
#ifndef VDR_h
#define VDR_h

//	DCE Implemenation for #1705 VDR

#include "Gen_Devices/VDRBase.h"
//<-dceag-d-e->

#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "X11/keysym.h"
#include <X11/extensions/XTest.h>
#include "DCE/PlainClientSocket.h"
class RatPoisonWrapper;
typedef enum  { VDRSTATUS_DISCONNECTED=0, VDRSTATUS_STARTUP, VDRSTATUS_LIVETV, VDRSTATUS_MENU, VDRSTATUS_PLAYBACK, VDRSTATUS_GUIDEGRID } eVDRState;

//<-dceag-decl-b->
namespace DCE
{
	class VDR : public VDR_Command
	{
//<-dceag-decl-e->
private:
				DeviceData_Base *m_pDevice_VDR_Plugin;
	      long unsigned int            m_iVDRWindowId;
	      pluto_pthread_mutex_t m_VDRMutex;
	      PlainClientSocket *m_pVDRSocket; 
	      int                          m_iControllingDevice;
        pthread_t                    m_qApplicationThreadId;
        pthread_t		     m_threadMonitorVDR;
				DeviceData_Base *m_pDevice_DVBCard,*m_pDevice_Xine,*m_pDevice_MediaPlugin;
				int m_iStreamID;  // The current stream ID
				int m_iChannelNumber; // The current channelNo
				string m_sChannelName; // The current Name
				string m_sSeriesDescription; // The current showname
				string m_sEpisodeDescription; // The current episode
				string m_sDescription; // the current chan-Description
				string m_sXineIP;
				Display *m_pDisplay;
				string m_CurrentMode, m_CurrentProgram;
				int m_CurTime, m_EndTime;
				string m_sChannel,m_sInitialChannel;
        bool checkWindowName(long unsigned int window, string windowName);
        void selectWindow();
        string m_sVDRIp;
        

protected:                                        
        bool LaunchVDR(bool bSelectWindow=true);
        bool StopVDRFrontend();
	      bool locateVDRWindow(long unsigned int window);
		    Display *getDisplay() { return m_pDisplay; };
        void processKeyBoardInputRequest(int iXKeySym);                     

		// Private methods
public:
		// Public member variables
			bool m_bExiting;
			eVDRState		     m_VDRStatus;	
			int m_menustatus;  //menu an/aus
			int m_menubefore;
			string m_sVDRmodus;
			string m_sVDRstatus;
			                                

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VDR(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VDR();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

    virtual void KillSpawnedDevices();
    virtual void CreateChildren();
		//bool SendVDRCommand(string sIP, string sCommand,string &sVDRResponse);
		void ParseCurrentChannel(string sChannel);
		
		void updateMode(string toMode);
		void pollVDRStatus();
	 eVDRState m_VDRStatus_get() { return m_VDRStatus; }
		void m_VDRStatus_set(eVDRState _eVDRState)
		{ 
#ifdef DEBUG
			if( m_VDRStatus!=_eVDRState )
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"VDR::m_VDRStatus_set was %d now %d",m_VDRStatus,_eVDRState);
#endif
			m_VDRStatus = _eVDRState;
		}

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_File_Name_and_Path();
	int DATA_Get_TCP_Port();
	string DATA_Get_Name();
	bool DATA_Get_Only_One_Per_PC();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors);
	void EVENT_Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo);

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
	/** VDR chan + */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID) { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** VDR chan - */
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
	/** Capture a Video frame */
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


	/** @brief COMMAND: #91 - Input Select */
	/** toggle inputs */
		/** @param #71 PK_Command_Input */
			/** The Input to select, or 0=toggle */

	virtual void CMD_Input_Select(int iPK_Command_Input) { string sCMD_Result; CMD_Input_Select(iPK_Command_Input,sCMD_Result,NULL);};
	virtual void CMD_Input_Select(int iPK_Command_Input,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #92 - Pause */
	/** VDR pause */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Pause(int iStreamID) { string sCMD_Result; CMD_Pause(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #95 - Stop */
	/** VDR stop */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

	virtual void CMD_Stop(int iStreamID,bool bEject) { string sCMD_Result; CMD_Stop(iStreamID,bEject,sCMD_Result,NULL);};
	virtual void CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #97 - Mute */
	/** VDR mute */

	virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
	virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #102 - Record */
	/** Record the current show */

	virtual void CMD_Record() { string sCMD_Result; CMD_Record(sCMD_Result,NULL);};
	virtual void CMD_Record(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #123 - Info */
	/** Info about the currently playing program */
		/** @param #9 Text */
			/** nimic */

	virtual void CMD_Info(string sText) { string sCMD_Result; CMD_Info(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Info(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #124 - Favorites */
	/** Show favorites */

	virtual void CMD_Favorites() { string sCMD_Result; CMD_Favorites(sCMD_Result,NULL);};
	virtual void CMD_Favorites(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

	virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
	virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #129 - PIP - Channel Up */
	/** Go the next channel */

	virtual void CMD_PIP_Channel_Up() { string sCMD_Result; CMD_PIP_Channel_Up(sCMD_Result,NULL);};
	virtual void CMD_PIP_Channel_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #130 - PIP - Channel Down */
	/** Go the previous channel. */

	virtual void CMD_PIP_Channel_Down() { string sCMD_Result; CMD_PIP_Channel_Down(sCMD_Result,NULL);};
	virtual void CMD_PIP_Channel_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #139 - Play */
	/** vdr play */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Play(int iStreamID) { string sCMD_Result; CMD_Play(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #187 - Tune to channel */
	/** This will make the device to tune to a specific channel. */
		/** @param #39 Options */
			/** Extra data to allow the receiver to properly identify the channel ( this is the xmltvid value inside the mythtv database). */
		/** @param #68 ProgramID */
			/** The Program ID that we need to tune to. */

	virtual void CMD_Tune_to_channel(string sOptions,string sProgramID) { string sCMD_Result; CMD_Tune_to_channel(sOptions.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Tune_to_channel(string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #367 - Text */
	/** Text */

	virtual void CMD_Text() { string sCMD_Result; CMD_Text(sCMD_Result,NULL);};
	virtual void CMD_Text(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #368 - Help */
	/** Help */

	virtual void CMD_Help() { string sCMD_Result; CMD_Help(sCMD_Result,NULL);};
	virtual void CMD_Help(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #455 - Blue */
	/** VDR blue */

	virtual void CMD_Blue() { string sCMD_Result; CMD_Blue(sCMD_Result,NULL);};
	virtual void CMD_Blue(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #476 - Day Down */
	/** Day + */

	virtual void CMD_Day_Down() { string sCMD_Result; CMD_Day_Down(sCMD_Result,NULL);};
	virtual void CMD_Day_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #477 - Day Up */
	/** Day - */

	virtual void CMD_Day_Up() { string sCMD_Result; CMD_Day_Up(sCMD_Result,NULL);};
	virtual void CMD_Day_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #518 - Green */
	/** VDR Green */

	virtual void CMD_Green() { string sCMD_Result; CMD_Green(sCMD_Result,NULL);};
	virtual void CMD_Green(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #528 - Instant Replay */
	/** Instant replay */

	virtual void CMD_Instant_Replay() { string sCMD_Result; CMD_Instant_Replay(sCMD_Result,NULL);};
	virtual void CMD_Instant_Replay(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #535 - Last Play */
	/** Last */

	virtual void CMD_Last_Play() { string sCMD_Result; CMD_Last_Play(sCMD_Result,NULL);};
	virtual void CMD_Last_Play(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Menu(string sText,int iStreamID) { string sCMD_Result; CMD_Menu(sText.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #571 - Page Down */
	/** Page + */

	virtual void CMD_Page_Down() { string sCMD_Result; CMD_Page_Down(sCMD_Result,NULL);};
	virtual void CMD_Page_Down(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #572 - Page Up */
	/** Page - */

	virtual void CMD_Page_Up() { string sCMD_Result; CMD_Page_Up(sCMD_Result,NULL);};
	virtual void CMD_Page_Up(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #592 - PVR Menu */
	/** Show the pvr menu */

	virtual void CMD_PVR_Menu() { string sCMD_Result; CMD_PVR_Menu(sCMD_Result,NULL);};
	virtual void CMD_PVR_Menu(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #605 - Red */
	/** VDR red */

	virtual void CMD_Red() { string sCMD_Result; CMD_Red(sCMD_Result,NULL);};
	virtual void CMD_Red(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #682 - Yellow */
	/** VDR yellow */

	virtual void CMD_Yellow() { string sCMD_Result; CMD_Yellow(sCMD_Result,NULL);};
	virtual void CMD_Yellow(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #762 - Live TV */
	/** Go to Live TV */

	virtual void CMD_Live_TV() { string sCMD_Result; CMD_Live_TV(sCMD_Result,NULL);};
	virtual void CMD_Live_TV(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #763 - Exit */
	/** Exit guide */

	virtual void CMD_Exit() { string sCMD_Result; CMD_Exit(sCMD_Result,NULL);};
	virtual void CMD_Exit(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #777 - Recorded TV Menu */
	/** go to the recorded tv menu */

	virtual void CMD_Recorded_TV_Menu() { string sCMD_Result; CMD_Recorded_TV_Menu(sCMD_Result,NULL);};
	virtual void CMD_Recorded_TV_Menu(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #809 - Display Alert */
	/** Displays alert on the box */
		/** @param #9 Text */
			/** The text in the alert */
		/** @param #70 Tokens */
			/** File this alert with this token, and if another alert comes in before timeout with the same token, replace it. */
		/** @param #182 Timeout */
			/** Make the alert go away after this many seconds */
		/** @param #251 Interruption */
			/** How to interrupt the user if something is happening */

	virtual void CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption) { string sCMD_Result; CMD_Display_Alert(sText.c_str(),sTokens.c_str(),sTimeout.c_str(),iInterruption,sCMD_Result,NULL);};
	virtual void CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #870 - Get Data */
	/** Get data from a device */
		/** @param #9 Text */
			/** What data to return.  This is free form */
		/** @param #19 Data */
			/** The data being returned */

	virtual void CMD_Get_Data(string sText,char **pData,int *iData_Size) { string sCMD_Result; CMD_Get_Data(sText.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Get_Data(string sText,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #919 - On Demand */
	/** Go to Video on demand */

	virtual void CMD_On_Demand() { string sCMD_Result; CMD_On_Demand(sCMD_Result,NULL);};
	virtual void CMD_On_Demand(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

	virtual void CMD_Set_Media_ID(string sID,int iStreamID) { string sCMD_Result; CMD_Set_Media_ID(sID.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
