/*
     Copyright (C) 2008 LOCALE|concept 

     www.localeconcept.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef MAME_PlugIn_h
#define MAME_PlugIn_h

//	DCE Implemenation for #1926 MAME Plug-In

#include "Gen_Devices/MAME_PlugInBase.h"
//<-dceag-d-e->

#define CHECK_FOR_NEW_ROMS 1

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"

#include "MAMEMediaStream.h"
#include "MAMEROM.h"
#include "AlarmManager.h"

using namespace std;
using namespace DCE;

//<-dceag-decl-b->! custom
namespace DCE
{
	class MAME_PlugIn : public MAME_PlugIn_Command, public MediaHandlerBase, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables


		int m_dwPK_File_LastCheckedForNewRecordings;
		MAMEROM *m_pMAMEROM;

		pluto_pthread_mutex_t m_MAMEMediaMutex; //protect us from ourselves
		map<int, int> m_mapDevicesToStreams;
		// Private methods
protected:
		class Orbiter_Plugin *m_pOrbiter_Plugin;

		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		MAME_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~MAME_PlugIn();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

public:
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

		MAMEMediaStream *ConvertToMAMEMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

		class AlarmManager *m_pAlarmManager;

		void CheckForNewROMs();

		void AlarmCallback(int id, void* param);

		bool MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		virtual void PrepareToDelete();

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

/*	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage); */


	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #14 Type */
			/** The type of recording: O=Once, C=Channel */
		/** @param #39 Options */
			/** Options for this recording, tbd later */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

	virtual void CMD_Schedule_Recording(string sType,string sOptions,string sProgramID) { string sCMD_Result; CMD_Schedule_Recording(sType.c_str(),sOptions.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Schedule_Recording(string sType,string sOptions,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID) { string sCMD_Result; CMD_Get_Extended_Media_Data(sPK_DesignObj.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #764 - Set Active Menu */
	/** Indicate which menu is active, options are:
live, nonlive, osd */
		/** @param #9 Text */
			/** The menu currently active */

	virtual void CMD_Set_Active_Menu(string sText) { string sCMD_Result; CMD_Set_Active_Menu(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #824 - Sync Providers and Cards */
	/** Synchronize settings for pvr cards and provders */
		/** @param #2 PK_Device */
			/** If specified, this is the capture card that triggered this change to call checktvproviders for */
		/** @param #198 PK_Orbiter */
			/** If specified, this is the orbiter to notify of the progress if this results in scanning for channels */

	virtual void CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter) { string sCMD_Result; CMD_Sync_Providers_and_Cards(iPK_Device,iPK_Orbiter,sCMD_Result,NULL);};
	virtual void CMD_Sync_Providers_and_Cards(int iPK_Device,int iPK_Orbiter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #846 - Make Thumbnail */
	/** Thumbnail the current frame */
		/** @param #13 Filename */
			/** Can be a fully qualified filename, or a !F+number, or !A+number for an attribute */
		/** @param #19 Data */
			/** The picture */

	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size) { string sCMD_Result; CMD_Make_Thumbnail(sFilename.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Make_Thumbnail(string sFilename,char *pData,int iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #910 - Reporting EPG Status */
	/** Reporting the status of an EPG update */
		/** @param #9 Text */
			/** Any messages about this */
		/** @param #40 IsSuccessful */
			/** true if the process succeeded */
		/** @param #257 Task */
			/** The type of EPG task: channel (retrieving channels), guide (retrieving guide) */

	virtual void CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask) { string sCMD_Result; CMD_Reporting_EPG_Status(sText.c_str(),bIsSuccessful,sTask.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reporting_EPG_Status(string sText,bool bIsSuccessful,string sTask,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #911 - Remove Scheduled Recording */
	/** Remove a scheduled recording */
		/** @param #10 ID */
			/** The ID of the recording rule to remove.  This will remove all recordings with this ID, and ProgramID is ignored if this is specified. */
		/** @param #68 ProgramID */
			/** The ID of the program to remove.  If ID is empty, remove just this program. */

	virtual void CMD_Remove_Scheduled_Recording(string sID,string sProgramID) { string sCMD_Result; CMD_Remove_Scheduled_Recording(sID.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Remove_Scheduled_Recording(string sID,string sProgramID,string &sCMD_Result,Message *pMessage);


//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
