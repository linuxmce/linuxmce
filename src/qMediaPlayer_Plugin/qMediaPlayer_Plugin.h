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

//#include "DCE/Command_Impl.h"
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
        map<int, int> m_mapDevicesToStreams;

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

            QMediaStream *ConvertToQtMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");

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

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
