/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef VLC_Plugin_h
#define VLC_Plugin_h

//	DCE Implemenation for #2301 VLC Plug-in

#include "Gen_Devices/VLC_PluginBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"

#include "VLCMediaStream.h"
#include "AlarmManager.h"

#include <algorithm>

//<-dceag-decl-b->
namespace DCE
{
  class VLC_Plugin : public VLC_Plugin_Command, public MediaHandlerBase, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables
	  pluto_pthread_mutex_t m_VLCMediaMutex;
	  
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VLC_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VLC_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void CreateChildren();
		/* virtual ReceivedMessageResult ReceivedMessage(class Message *pMessage); */

	protected:
		class Orbiter_Plugin *m_pOrbiter_Plugin;

	public:
		class VLCSyncListener;

		VLCSyncListener *m_pSyncSocket;

		class AlarmManager *m_pAlarmManager;
		virtual void AlarmCallback(int id, void* param);

		// MediaHandlerBase implementations
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
		
		VLCMediaStream *ConvertToVLCMediaStream(MediaStream *pMediaStream, string callerIdMessage = "");
		
		bool ConfirmSourceIsADestination(string &sMRL,VLCMediaStream *pVLCMediaStream,int PK_Device_Drive);
		
		// Alarm callback to send sync timecode.
		time_t m_dwSec;
		void SendSync();

		
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


		class VLCSyncListener : public SocketListener
		{

		public:
		  
		VLCSyncListener(string sName):SocketListener(sName)
		  {
		  };

		  virtual void ReceivedMessage(Socket *pSocket, Message* pMessage){};
		  virtual bool ReceivedString( Socket *pSocket, string sLine, int nTimeout = - 1 )
		  {
		    std::cout << "Socket got: " << sLine << std::endl;
		    return true;
		  };
		  
		  void SendToAll(string sString)
		  {
		    for (vector<ServerSocket *>::iterator it=m_vectorServerSocket.begin(); it!=m_vectorServerSocket.end(); ++it)
		      {
			if ((*it)->SendString(sString))
			  {
			    /* LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending data to: %s - %s",(*it)->m_sHostName.c_str(), sString.c_str()); */
			  }
			else  // failed to send to a client.
			  {
			    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Could not send data to: %s - %s",(*it)->m_sHostName.c_str(), sString.c_str());
			  }
		      }
		  }
		  
		};
		
		
	};
  
  //<-dceag-end-b->
}
#endif
//<-dceag-end-e->
