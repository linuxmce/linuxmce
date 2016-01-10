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
#ifndef Pandora_Player_h
#define Pandora_Player_h

//	DCE Implemenation for #2143 Pandora Player

#include "Gen_Devices/Pandora_PlayerBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class Pandora_Player : public Pandora_Player_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Pandora_Player(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Pandora_Player();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Pandora_Player(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Alsa_Output_Device();
	int DATA_Get_Time_Code_Report_Frequency();
	int DATA_Get_Port();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors);
	void EVENT_Media_Description_Changed(string sText);
	void EVENT_Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #37 - Play Media */
	/** Start playing a stream */
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


	/** @brief COMMAND: #40 - Restart Media */
	/** Restart media after a pause. */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

	virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Skip Forward to next song. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID) { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** In this case, Jump forward to next song. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #92 - Pause */
	/** Pause a stream */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Pause(int iStreamID) { string sCMD_Result; CMD_Pause(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #139 - Play */
	/** Play the stream */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Play(int iStreamID) { string sCMD_Result; CMD_Play(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #651 - Thumbs Down */
	/** Signify currently playing song has thumbs down. */
		/** @param #41 StreamID */
			/** For Smart Media Players, the currently Playing Stream ID */

	virtual void CMD_Thumbs_Down(int iStreamID) { string sCMD_Result; CMD_Thumbs_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Thumbs_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #652 - Thumbs Up */
	/** Signify currently playing song in stream has thumbs up. */
		/** @param #41 StreamID */
			/** For smart media players, the currently playing media stream. */

	virtual void CMD_Thumbs_Up(int iStreamID) { string sCMD_Result; CMD_Thumbs_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Thumbs_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1076 - Add Station */
	/** Add Radio Station to this User's list of stations. */
		/** @param #10 ID */
			/** The Returned Station ID */
		/** @param #14 Type */
			/** Station Type. For Pandora this is default or genre */
		/** @param #41 StreamID */
			/** The Stream ID to modify. */
		/** @param #50 Name */
			/** The name of the Station to add. For genre this is a Genre ID. For Default, This is used as a Search term. */

	virtual void CMD_Add_Station(string sType,int iStreamID,string sName,string *sID) { string sCMD_Result; CMD_Add_Station(sType.c_str(),iStreamID,sName.c_str(),sID,sCMD_Result,NULL);};
	virtual void CMD_Add_Station(string sType,int iStreamID,string sName,string *sID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1077 - Remove Station */
	/** Remove Radio Station from this User's List of stations. */
		/** @param #10 ID */
			/** Station ID to remove. */
		/** @param #41 StreamID */
			/** The Stream ID from which to remove */

	virtual void CMD_Remove_Station(string sID,int iStreamID) { string sCMD_Result; CMD_Remove_Station(sID.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Remove_Station(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1078 - Rename Station */
	/** Rename Radio Station in a user's list of radio stations. */
		/** @param #10 ID */
			/** The Radio Station ID to rename */
		/** @param #41 StreamID */
			/** The Stream ID */
		/** @param #50 Name */
			/** The New Name of the Radio Station */

	virtual void CMD_Rename_Station(string sID,int iStreamID,string sName) { string sCMD_Result; CMD_Rename_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Rename_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1079 - Explain Song */
	/** Ask the radio station for an explanation of why this song is playing. */
		/** @param #41 StreamID */
			/** The Stream ID of the currently playing song. */

	virtual void CMD_Explain_Song(int iStreamID) { string sCMD_Result; CMD_Explain_Song(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Explain_Song(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1080 - Add Music to Station */
	/** Add Music to an already existing Radio Station */
		/** @param #10 ID */
			/** The Radio Station ID to add terms to */
		/** @param #41 StreamID */
			/** The Stream ID */
		/** @param #50 Name */
			/** Search terms such as an artist or song title. */

	virtual void CMD_Add_Music_to_Station(string sID,int iStreamID,string sName) { string sCMD_Result; CMD_Add_Music_to_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Add_Music_to_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1081 - Add Station to QuickMix */
	/** Add A Radio Station to the Currently Playing QuickMix */
		/** @param #10 ID */
			/** The Station ID to add */
		/** @param #41 StreamID */
			/** The StreamID with the quickmix to modify */

	virtual void CMD_Add_Station_to_QuickMix(string sID,int iStreamID) { string sCMD_Result; CMD_Add_Station_to_QuickMix(sID.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Add_Station_to_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1082 - Remove Station from QuickMix */
	/** Remove Radio Station from the currently playing QuickMix */
		/** @param #10 ID */
			/** The Station ID */
		/** @param #41 StreamID */
			/** The Stream ID */

	virtual void CMD_Remove_Station_from_QuickMix(string sID,int iStreamID) { string sCMD_Result; CMD_Remove_Station_from_QuickMix(sID.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Remove_Station_from_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
