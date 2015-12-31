/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/**
 *
 * @file Xine_Plugin.h
 * @brief header file for the Xine_Plugin class
 * @author
 *
 */


//<-dceag-d-b->
#ifndef Xine_Plugin_h
#define Xine_Plugin_h

//	DCE Implemenation for #6 Xine Plug-in

#include "Gen_Devices/Xine_PluginBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"

#include "XineMediaStream.h"

//<-dceag-decl-b->! custom
namespace DCE
{

	class Xine_Plugin : public Xine_Plugin_Command, public MediaHandlerBase
	{
	//<-dceag-decl-e->

		pluto_pthread_mutex_t m_XineMediaMutex; //protect us from ourselves

	//<-dceag-const-b->
public:
		// Constructors/Destructor
		Xine_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Xine_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
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

		XineMediaStream *ConvertToXineMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

		bool ConfirmSourceIsADestination(string &sMRL,XineMediaStream *pXineMediaStream,int PK_Device_Drive);

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
//<-dceag-const2-b->!
