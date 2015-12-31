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
#ifndef Game_PlugIn_h
#define Game_PlugIn_h

//	DCE Implemenation for #1964 Game PlugIn

#include "Gen_Devices/Game_PlugInBase.h"
//<-dceag-d-e->

////////////////////////////////////////////////////////////////////////
// Custom Includes
////////////////////////////////////////////////////////////////////////

#define CHECK_FOR_NEW_ROMS 1

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"

#include "GameMediaStream.h"
#include "AlarmManager.h"
#include "GAMEROM.h"


//<-dceag-decl-b->
namespace DCE
{
	class Game_PlugIn : public Game_PlugIn_Command, public MediaHandlerBase, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables

		int m_dwPK_File_LastCheckedForNewRecordings;
		GAMEROM *m_pGAMEROM;

		pluto_pthread_mutex_t m_GameMediaMutex; //protect us from ourselves
		map<int, int> m_mapDevicesToStreams;
		// Private methods
protected:
		class Orbiter_Plugin *m_pOrbiter_Plugin;
		class Datagrid_Plugin *m_pDatagrid_Plugin;

		// Private methods
public:
		// Public member variables
		int m_iPriority;

		class DataGridTable *SwapDisksGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );


//<-dceag-const-b->
public:
		// Constructors/Destructor
		Game_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Game_PlugIn();
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

		GameMediaStream *ConvertToGameMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

		class AlarmManager *m_pAlarmManager;

		void CheckForNewROMs();

		void AlarmCallback(int id, void* param);

		bool MenuOnScreen( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool PlaybackCompleted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		virtual void PrepareToDelete();



//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Game_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();
	bool DATA_Get_Only_One_Per_PC();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #764 - Set Active Menu */
	/** Used to set which screens to show given monitor mode, options, etc. */
		/** @param #9 Text */
			/** The menu currently active */

	virtual void CMD_Set_Active_Menu(string sText) { string sCMD_Result; CMD_Set_Active_Menu(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Menu(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1119 - Bind Keypad */
	/** Used on Remotes that have keypads to update the Keypad overlay, if available. */
		/** @param #3 PK_DesignObj */
			/** The designobj of the keypad overlay. */
		/** @param #45 PK_EntertainArea */
			/** The Entertainment area of the stream.  */

	virtual void CMD_Bind_Keypad(string sPK_DesignObj,string sPK_EntertainArea) { string sCMD_Result; CMD_Bind_Keypad(sPK_DesignObj.c_str(),sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Bind_Keypad(string sPK_DesignObj,string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
