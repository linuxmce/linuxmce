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
#ifndef qMediaPlayer_Plugin_h
#define qMediaPlayer_Plugin_h

//	DCE Implemenation for #2206 qMediaPlayer Plug-in

#include "Gen_Devices/qMediaPlayer_PluginBase.h"

//<-dceag-d-e->

#include "DCE/Command_Impl.h"
#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "qmediastream.h"

//<-dceag-decl-b->
namespace DCE
{
    class qMediaPlayer_Plugin : public qMediaPlayer_Plugin_Command, public MediaHandlerBase
	{
//<-dceag-decl-e->
        pluto_pthread_mutex_t m_QMediaMediaMutex; //protect us from ourselves --holdover from pluto because we are in their ecosystem
		// Private member variables
protected:
        class Orbiter_Plugin *m_pOrbiter_Plugin;
		// Private methods
public:
		// Public member variables
        /** Mandatory implementations */

            /**
            * @brief
            */
            virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

            /**
            * @brief Start media playback
            */
            virtual bool StartMedia( class MediaStream *pMediaStream,string &sError );

            /**
            * @brief Stop media playback
            */
            virtual bool StopMedia( class MediaStream *pMediaStream );

            virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);
            /**
            * @brief We need to see all media inserted events so we can start the appropriate media devices
            */

            bool MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

            QMediaStream *ConvertToXineMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

            bool ConfirmSourceIsADestination(string &sMRL,QMediaStream *pQMediaStream,int PK_Device_Drive);

//<-dceag-const-b->
public:
		// Constructors/Destructor
		qMediaPlayer_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~qMediaPlayer_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);

        //<-dceag-const-e->



//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		qMediaPlayer_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #36 - Create Media */
	/** Create a media stream descriptor. */
		/** @param #13 Filename */
			/** The filename of the media stream. */
		/** @param #41 StreamID */
			/** The media descriptor which will be associated with the current media. */

	virtual void CMD_Create_Media(string sFilename,int iStreamID) { string sCMD_Result; CMD_Create_Media(sFilename.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Create_Media(string sFilename,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #37 - Play Media */
	/** Play a media stream descriptor. */
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
	/** Stop playing a media stream descriptor. */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

	virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition) { string sCMD_Result; CMD_Stop_Media(iStreamID,sMediaPosition,sCMD_Result,NULL);};
	virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #39 - Pause Media */
	/** Pause a media playback. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

	virtual void CMD_Pause_Media(int iStreamID) { string sCMD_Result; CMD_Pause_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #40 - Restart Media */
	/** Restart a media playback. */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

	virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Change the playback speed of a media stream. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
		/** @param #220 Report */
			/** If true, report this speed to the user on the OSD */

	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,bReport,sCMD_Result,NULL);};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jumps to a position within some media, such as songs in a playlist, tracks on a cd, etc.  It will assume the sender is an orbiter, and find the entertainment area and stream associated with it.  The track can be an absolute or relative position. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
