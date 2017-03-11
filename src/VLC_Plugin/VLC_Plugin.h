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

#include <algorithm>

//<-dceag-decl-b->
namespace DCE
{
  class VLC_Plugin : public VLC_Plugin_Command, public MediaHandlerBase
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

	protected:
		class Orbiter_Plugin *m_pOrbiter_Plugin;

	public:
		class VLCSyncListener;

		VLCSyncListener *m_pSyncSocket;

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

		void SendTimecodeToPlayer(int iTimecode, int dwPK_Device);
		void GetTimeCodeFromPlayer(long dwPK_Device_Player, int iStreamID, string& sText, string& sPosition);

		
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
		  
		  DCE::VLC_Plugin* m_pVLC_Plugin;
		  map<int, vector<ServerSocket *> > m_mapStreamIDSockets;         // Holds map of sockets given a stream ID
		  map<ServerSocket*, int> m_mapServerSocketToStreamID;            // Holds map of stream ID to a single socket.
		  map<int, ServerSocket* > m_mapControllingSocketForStream;
		  map<ServerSocket*, int> m_mapServerSocketToBias;

		VLCSyncListener(string sName, DCE::VLC_Plugin* pVLC_Plugin):SocketListener(sName)
		  {
		    m_pVLC_Plugin = pVLC_Plugin;
		  };


		  ServerSocket* ServerSocket_find(Socket *pSocket)
		  {
		    for (vector<ServerSocket *>::iterator it=m_vectorServerSocket.begin(); it!=m_vectorServerSocket.end(); ++it)
		      {
			if ((*it) == (ServerSocket *)pSocket)
			  {
			    return (*it);
			  }
		      }
		    return NULL;
		  }

		  int ServerSocketToStreamID_find(ServerSocket *pServerSocket)
		  {
		    if (m_mapServerSocketToStreamID.find(pServerSocket) == m_mapServerSocketToStreamID.end())
		      return 0;

		    return m_mapServerSocketToStreamID[pServerSocket];

		  }

		  bool DeviceIsRegistered(Socket *pSocket)
		  {
		    ServerSocket *pServerSocket = ServerSocket_find(pSocket);
		    if (!pServerSocket)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::DeviceIsRegistered() - No device registered for socket %p",pSocket);
			return false;
		      }
		    else
		      {
			return true;
		      }
		  }


		  long GetDeviceFromSocket(Socket *pSocket)
		  {
		    ServerSocket *pServerSocket = ServerSocket_find(pSocket);
		    if (!pServerSocket)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::GetDeviceFromSocket() - No device # for socket %p",pSocket);
			return 0;
		      }
		    else
		      {
			return pServerSocket->m_dwPK_Device;
		      }
		  }

		  void GiveControlToSocket(Socket* pSocket)
		  {
		    ServerSocket *pServerSocket = ServerSocket_find(pSocket);
		 
		    if (!pServerSocket)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::GiveControlToSocket() - No server socket found for socket %p",pSocket);
			return;
		      }
		    
		    int iStreamID = m_mapServerSocketToStreamID[pServerSocket];

		    if (iStreamID == 0)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::GiveControlToSocket() - No Stream ID found for server socket %p",pServerSocket);
			return;
		      }

		    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener::GiveControlToSocket() - Give control of Stream ID %d to player device %d",iStreamID,pServerSocket->m_dwPK_Device);
		    m_mapControllingSocketForStream[iStreamID] = pServerSocket;

		  }
		  
		  bool DeviceHasControl(Socket* pSocket)
		  {
		    ServerSocket *pServerSocket = ServerSocket_find(pSocket);
		 
		    if (!pServerSocket)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::GiveControlToSocket() - No server socket found for socket %p",pSocket);
			return false;;
		      }

		    int iStreamID = m_mapServerSocketToStreamID[pServerSocket];
		    
		    if (iStreamID == 0)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::DeviceHasControl() - No stream ID for server socket %p",pServerSocket);
			return false;
		      }

		    return m_mapControllingSocketForStream[iStreamID] == pServerSocket;
		    
		  }

		  void AddSocketToStream(Socket* pSocket, int iStreamID)
		  {
		    ServerSocket* pServerSocket = ServerSocket_find(pSocket);
		    vector<ServerSocket *> vectSockets = m_mapStreamIDSockets[iStreamID];
		    if (std::find(vectSockets.begin(), vectSockets.end(), pServerSocket) == vectSockets.end())
		      {
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener::AddSocketToStream() - Adding socket %p to stream ID %d",pSocket,iStreamID);
			vectSockets.push_back(pServerSocket);
			m_mapStreamIDSockets[iStreamID] = vectSockets;
			m_mapServerSocketToStreamID[pServerSocket] = iStreamID;
			m_mapServerSocketToBias[pServerSocket] = 0; // Initially no bias applied to newly registered players.
		      }
		    else
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::AddSocketToStream() - Socket %p already exists in stream ID %d. Ignoring.",pSocket,iStreamID);
			return;
		      }
		  }

		  void DeleteSocketFromStream(Socket* pSocket, int iStreamID)
		  {
		    ServerSocket* pServerSocket = ServerSocket_find(pSocket);
		    vector<ServerSocket *> vectSockets = m_mapStreamIDSockets[iStreamID];

		    if (vectSockets.size() > 0)
		      {
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener::DeleteSocketFromStream() - Deleting socket %p from stream ID %d",pServerSocket,iStreamID);
			vectSockets.erase(std::remove(vectSockets.begin(), vectSockets.end(), pServerSocket), vectSockets.end());
			m_mapStreamIDSockets[iStreamID] = vectSockets;
			m_mapServerSocketToStreamID.erase(pServerSocket);
		      }
		    else
		      {
			// Return, because we don't want to do anything else.
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::DeleteSocketFromstream() - No sockets in stream ID %d to delete, ignoring.",iStreamID);
			return;
		      }

		    // If the stream vector is now empty, go ahead and remove it completely from the map.
		    if (vectSockets.size() == 0)
		      {
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener::DeleteSocketFromStream() - No more sockets. Removing stream ID %d from map",iStreamID);
			m_mapStreamIDSockets.erase(iStreamID);
		      }

		  }

		  virtual void ReceivedMessage(Socket *pSocket, Message* pMessage){};
		  virtual bool ReceivedString(Socket *pSocket, string sLine, int nTimeout = -1)
		  {
		    if (!DeviceIsRegistered(pSocket))
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener: unregistered device attempting transport commands. Ignoring.");
			return false;
		      }


		    if ((sLine.find("T:") != string::npos) && DeviceHasControl(pSocket))
		      {
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener: Sending new timecode %s to all players",sLine.substr(2).c_str());
			SendTimecodeToPlayers(pSocket,sLine.substr(2));
			
		      }
		    else if (sLine.find("GC") != string::npos)
		      {
			long dwPK_Device = GetDeviceFromSocket(pSocket);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener: Giving control to player with device ID: %d",dwPK_Device);
			GiveControlToSocket(pSocket);
		      }
		    else if ((sLine.find("STREAM_ENTER") != string::npos) && DeviceIsRegistered(pSocket))
		      {
			int iStreamID = atoi(sLine.substr(13).c_str());
			AddSocketToStream(pSocket,iStreamID);
		      }
		    else if ((sLine.find("STREAM_EXIT") != string::npos) && DeviceIsRegistered(pSocket))
		      {
			int iStreamID = atoi(sLine.substr(12).c_str());
			DeleteSocketFromStream(pSocket,iStreamID);
		      }
		    return false; // for now.
		  }
		  
		  void SendTimecodeToPlayers(Socket *pSocket, string sTimecode)
		  {
		    PLUTO_SAFETY_LOCK(lm, m_ListenerMutex);
		    ServerSocket *pServerSocket = ServerSocket_find(pSocket);
		    
		    if (!pServerSocket)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::SendTimecodeToPlayers() Could not find a server socket for socket %p",pSocket);
			return;
		      }

		    int iStreamID = m_mapServerSocketToStreamID[pServerSocket];
		    
		    if (iStreamID == 0)
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLCSyncListener::SendTimecodeToPlayers() - No Stream ID found for server socket %p",pServerSocket);
			return;
		      }

		    vector<ServerSocket *> vectSockets = m_mapStreamIDSockets[iStreamID];

		    for (vector<ServerSocket *>::iterator it=vectSockets.begin(); it!=vectSockets.end(); ++it)
		      {
			if ((*it)->m_dwPK_Device != 0)
			  {
			    ServerSocket* pServerSocket = (*it);
			    struct timespec t;
			    clock_gettime(CLOCK_REALTIME,&t);
			    time_t newTime = (t.tv_sec)+1;
			    
			    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLCSyncListener: Sending timecode %s to player %d, Destination time %lu",sTimecode.c_str(), (*it)->m_dwPK_Device,newTime);
			    pServerSocket->SendString("T|"+sTimecode+"|"+StringUtils::i64tos(newTime));
			  }
		      }
		  }
		  
		  void SendToAll(string sString)
		  {
		    PLUTO_SAFETY_LOCK(lm, m_ListenerMutex);
		    for (vector<ServerSocket *>::iterator it=m_vectorServerSocket.begin(); it!=m_vectorServerSocket.end(); ++it)
		      {
			if ((*it)->SendString(sString))
			  {
			    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sending data to: %s - %s",(*it)->m_sHostName.c_str(), sString.c_str());
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
