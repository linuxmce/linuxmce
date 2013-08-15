/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Roku_LTHDXDXS_Streaming_Pla_h
#define Roku_LTHDXDXS_Streaming_Pla_h

//	DCE Implemenation for #2049 Roku LT/HD/XD/XS Streaming Pla

#include "Gen_Devices/Roku_LTHDXDXS_Streaming_PlaBase.h"
//<-dceag-d-e->

#include "RokuDevice.h"

//<-dceag-decl-b->
namespace DCE
{
	class Roku_LTHDXDXS_Streaming_Pla : public Roku_LTHDXDXS_Streaming_Pla_Command
	{
//<-dceag-decl-e->
		// Private member variables
	  RokuDevice* m_pRokuDevice;
	  pluto_pthread_mutex_t m_RokuMutex;
	  bool m_bIsPaused;
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Roku_LTHDXDXS_Streaming_Pla(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Roku_LTHDXDXS_Streaming_Pla();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void CreateChildren();

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Roku_LTHDXDXS_Streaming_Pla(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	int DATA_Get_TCP_Port();
	int DATA_Get_FK_Device_Capture_Card_Port();
	string DATA_Get_AV_Adjustment_Rules();
	bool DATA_Get_Dont_Consolidate_Media();
	int DATA_Get_FK_Device_Capture_Card_Audio_P();

			*****EVENT***** accessors inherited from base class

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


	/** @brief COMMAND: #48 - Eject Disk */
	/**  */
		/** @param #151 Slot Number */
			/** For jukeboxes, which slot to eject */

	virtual void CMD_Eject_Disk(int iSlot_Number) { string sCMD_Result; CMD_Eject_Disk(iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Eject_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Change channels greater (larger numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID) { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Change channels lower (smaller numbers) */
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


	/** @brief COMMAND: #86 - Menu (Show Menu) */
	/** Show the on-screen menu */

	virtual void CMD_Menu_Show_Menu() { string sCMD_Result; CMD_Menu_Show_Menu(sCMD_Result,NULL);};
	virtual void CMD_Menu_Show_Menu(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #92 - Pause */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Pause(int iStreamID) { string sCMD_Result; CMD_Pause(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #93 - Scan Fwd/Fast Fwd */
	/**  */

	virtual void CMD_Scan_FwdFast_Fwd() { string sCMD_Result; CMD_Scan_FwdFast_Fwd(sCMD_Result,NULL);};
	virtual void CMD_Scan_FwdFast_Fwd(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #95 - Stop */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

	virtual void CMD_Stop(int iStreamID,bool bEject) { string sCMD_Result; CMD_Stop(iStreamID,bEject,sCMD_Result,NULL);};
	virtual void CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #98 - Slow Scan Back */
	/**  */

	virtual void CMD_Slow_Scan_Back() { string sCMD_Result; CMD_Slow_Scan_Back(sCMD_Result,NULL);};
	virtual void CMD_Slow_Scan_Back(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #99 - Slow Scan Forward */
	/**  */

	virtual void CMD_Slow_Scan_Forward() { string sCMD_Result; CMD_Slow_Scan_Forward(sCMD_Result,NULL);};
	virtual void CMD_Slow_Scan_Forward(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #102 - Record */
	/**  */

	virtual void CMD_Record() { string sCMD_Result; CMD_Record(sCMD_Result,NULL);};
	virtual void CMD_Record(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #123 - Info */
	/** Info */
		/** @param #9 Text */
			/** nimic */

	virtual void CMD_Info(string sText) { string sCMD_Result; CMD_Info(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Info(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #125 - Scan Back/Rewind */
	/**  */

	virtual void CMD_Scan_BackRewind() { string sCMD_Result; CMD_Scan_BackRewind(sCMD_Result,NULL);};
	virtual void CMD_Scan_BackRewind(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

	virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
	virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #139 - Play */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Play(int iStreamID) { string sCMD_Result; CMD_Play(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Selects an item on the on-screen menu */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #225 - Display */
	/** Toggles the on-screen menu/display on/off */

	virtual void CMD_Display() { string sCMD_Result; CMD_Display(sCMD_Result,NULL);};
	virtual void CMD_Display(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Exits/goes back from the menu */
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


	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

	virtual void CMD_Process_Incoming_Data() { string sCMD_Result; CMD_Process_Incoming_Data(sCMD_Result,NULL);};
	virtual void CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

	virtual void CMD_Process_IDLE() { string sCMD_Result; CMD_Process_IDLE(sCMD_Result,NULL);};
	virtual void CMD_Process_IDLE(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

	virtual void CMD_Process_Initialize() { string sCMD_Result; CMD_Process_Initialize(sCMD_Result,NULL);};
	virtual void CMD_Process_Initialize(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

	virtual void CMD_Process_Release() { string sCMD_Result; CMD_Process_Release(sCMD_Result,NULL);};
	virtual void CMD_Process_Release(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #368 - Help */
	/** Bring up a help menu */

	virtual void CMD_Help() { string sCMD_Result; CMD_Help(sCMD_Result,NULL);};
	virtual void CMD_Help(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

	virtual void CMD_Private_Method_Listing() { string sCMD_Result; CMD_Private_Method_Listing(sCMD_Result,NULL);};
	virtual void CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

	virtual void CMD_Process_Receive_Command_For_Child() { string sCMD_Result; CMD_Process_Receive_Command_For_Child(sCMD_Result,NULL);};
	virtual void CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #521 - Home */
	/** Go Home */

	virtual void CMD_Home() { string sCMD_Result; CMD_Home(sCMD_Result,NULL);};
	virtual void CMD_Home(string &sCMD_Result,Message *pMessage);


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
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
