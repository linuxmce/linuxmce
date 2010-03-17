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
#ifndef Picture_Plugin_h
#define Picture_Plugin_h

//	DCE Implemenation for #2123 Picture Plugin

#include "Gen_Devices/Picture_PluginBase.h"
//<-dceag-d-e->
#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "../Orbiter_Plugin/Orbiter_Plugin.h"

//<-dceag-decl-b->!
namespace DCE
{
	class Picture_Plugin : public Picture_Plugin_Command, public MediaHandlerBase
	{
//<-dceag-decl-e->
		// Private member variables
		pluto_pthread_mutex_t m_PicturePluginMutex;
		map<int, int> m_mapDevicesToStreams;
		Orbiter_Plugin *m_pOrbiter_Plugin;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Picture_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Picture_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Picture_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

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

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
