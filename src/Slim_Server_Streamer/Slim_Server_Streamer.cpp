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
#include "Slim_Server_Streamer.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Command.h"
#include "PlutoUtils/ProcessUtils.h"
#include <math.h>

#define SOCKET_TIMEOUT		30

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Slim_Server_Streamer::Slim_Server_Streamer(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Slim_Server_Streamer_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
    , m_dataStructureAccessMutex("internal-data-structure-mutex"), m_stateChangedMutex("state-changed-mutex")
{
    // Usually the slim server and the application server that is going to spawn it are going to be on the same machine.
    // This is good because you can also restrict the control of the server only to connections from that machine.
    m_strSlimServerCliAddress = "127.0.0.1"; // hard code it here for now.
    m_iSlimServerCliPort = 9090;
	m_threadPlaybackCompletedChecker = 0;

    m_iServerSocket = -1;

    pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );

	m_dataStructureAccessMutex.Init( &mutexAttr );

	pthread_cond_init( &m_stateChangedCondition, NULL );
	m_stateChangedMutex.Init( &mutexAttr, &m_stateChangedCondition );

	m_pNotificationSocket = new SlimServerStreamerNotification_SocketListener(string("m_pNotificationSocket"));
	m_pNotificationSocket->m_bSendOnlySocket = true;

}

//<-dceag-getconfig-b->
bool Slim_Server_Streamer::GetConfig()
{
	if( !Slim_Server_Streamer_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	return true;
}

void Slim_Server_Streamer::PostConnect()
{
	DeviceData_Base *pDevice_AppServer = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_App_Server_CONST,this);
	if (! pDevice_AppServer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Slim_Server_Streamer::PostConnect App_Server device not found in installation. Bailing out.");
		return;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "App_Server started");

	if (! ConnectToSlimServerCliCommandChannel())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connection failed. Sending a start command to the application server.");

		StartSlimServer();

		Sleep(3000);  // Wait 3 seconds for it to finish spawning
		if (! ConnectToSlimServerCliCommandChannel()) // try again the connection after the command was sent.
		{
			Sleep(3000);  // Wait another 3 seconds for it to finish spawning
			if (! ConnectToSlimServerCliCommandChannel()) // try again the connection after the command was sent.
			{
				Sleep(6000);  // Wait another 6 seconds for it to finish spawning
				if (! ConnectToSlimServerCliCommandChannel()) // try again the connection after the command was sent.
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "I could not connect to the streaming server after i have launched it with the application server.");
			}
		}
	}
	
	TurnOffAllSqueezeboxes();
    pthread_create(&m_threadPlaybackCompletedChecker, NULL, checkForPlaybackCompleted, this);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Slim_Server_Streamer::~Slim_Server_Streamer()
//<-dceag-dest-e->
{
	m_bQuit_set(true);
	pthread_cond_signal(&m_stateChangedCondition);
	if( m_threadPlaybackCompletedChecker )
		pthread_join(m_threadPlaybackCompletedChecker, NULL);

	if (m_pNotificationSocket)
	{
		delete m_pNotificationSocket;
		m_pNotificationSocket = NULL;
	}

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Slim_Server_Streamer::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Slim_Server_Streamer::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Slim_Server_Streamer::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

    COMMANDS TO IMPLEMENT

*/



//<-dceag-c249-b->

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

void Slim_Server_Streamer::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing Start streaming command for target devices: %s", sStreamingTargets.c_str());

	vector<string> vectPlayersIds;
	vector<DeviceData_Base*> vectDevices;

	StringUtils::Tokenize(sStreamingTargets, ",", vectPlayersIds);

	string currentPlayerAddress = "-";

	vector<string>::iterator itPlayerIds;
	for (itPlayerIds = vectPlayersIds.begin(); itPlayerIds != vectPlayersIds.end(); ++itPlayerIds)
	{
		int iPlayerId = atoi((*itPlayerIds).c_str());

		if (iPlayerId == 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Player id string %s parsed to 0. Ignoring.", (*itPlayerIds).c_str());
			continue;
		}

		DeviceData_Base *pPlayerDeviceData = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(iPlayerId);
		if (pPlayerDeviceData == NULL)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Child with id: %d was not found. Ignoring", iPlayerId);
			continue;
		}

		currentPlayerAddress = getMacAddressForDevice(pPlayerDeviceData);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Slim_Server_Streamer::CMD_Start_Streaming() Making sure that the player \"%s\" is connected before doing anything.", StringUtils::URLDecode(currentPlayerAddress).c_str());
		int nTries = 10;
		while (nTries && SendReceiveCommand(currentPlayerAddress + " connected ?") != currentPlayerAddress + " connected 1")
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Slim_Server_Streamer::CMD_Start_Streaming() Not yet connected");
			Sleep(500);
			nTries--;
		}

		if (nTries == 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Slim_Server_Streamer::CMD_Start_Streaming() Device %d (%s) with mac address %s was not detected as connected. Ignoring device.",
				pPlayerDeviceData->m_dwPK_Device, pPlayerDeviceData->m_sDescription.c_str(), currentPlayerAddress.c_str());
			continue;
		}
//		else
//		{
			vectDevices.push_back(pPlayerDeviceData);
			SendReceiveCommand(currentPlayerAddress + " power 1"); // Power on device.
#if 0
			SendReceiveCommand(currentPlayerAddress + " stop"); // stop playback (if any)
			SendReceiveCommand(currentPlayerAddress + " playlist clear"); // clear previous playlist (if any)
			SendReceiveCommand(currentPlayerAddress + " playlist repeat 0"); // set the playlist to non repeating.
			SendReceiveCommand(currentPlayerAddress + " sync -"); // break previous syncronization;
#endif
//		}
	}

	// add this stream to the list of playing streams.
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Adding iStreamID %d to m_mapStreamsToPlayers.",iStreamID);
	m_mapStreamsToPlayers[iStreamID] = make_pair(STATE_PAUSE, vectDevices);
	pthread_cond_signal(&m_stateChangedCondition);
}

//<-dceag-c262-b->

	/** @brief COMMAND: #262 - Stop Streaming */
	/** Stop the streaming of a particular media stream. */
		/** @param #41 StreamID */
			/** The ID of the stream to be stopped. */
		/** @param #105 StreamingTargets */
			/** Target destinations for streaming. Semantics dependent on the target device. */

void Slim_Server_Streamer::CMD_Stop_Streaming(int iStreamID,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c262-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	// PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing Stop Streaming command for target devices: %s size: %d",
		sStreamingTargets.c_str(),(int) m_mapStreamsToPlayers.size());

	map<int, pair<StreamStateType, vector<DeviceData_Base *> > >::iterator itPlayers;

	itPlayers = m_mapStreamsToPlayers.find(iStreamID);
	if ( itPlayers == m_mapStreamsToPlayers.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Slim_Server_Streamer::CMD_Stop_Streaming() got a command for a stream that is not registered here: %d", iStreamID);
		return;
	}

	string currentPlayerAddress;
    vector<string> vectPlayersIds;
    vector<DeviceData_Base*> &vectDevices = (*itPlayers).second.second;

    StringUtils::Tokenize(sStreamingTargets, ",", vectPlayersIds);

	vector<DeviceData_Base*>::iterator itPlaybackDevices;
	vector<string>::iterator itPlayerIds = vectPlayersIds.begin();
    while ( itPlayerIds != vectPlayersIds.end() )
	{
		int iPlayerId = atoi((*itPlayerIds).c_str());

        if ( iPlayerId == 0 )
        {
			itPlayerIds++;
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Player id string %s parsed to 0. Ignoring.", (*itPlayerIds).c_str() );
            continue;
        }

        DeviceData_Base *pPlayerDeviceData = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(iPlayerId);
        if ( pPlayerDeviceData == NULL )
        {
			itPlayerIds++;
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Child with id: %d was not found. Ignoring", iPlayerId);
            continue;
        }

		currentPlayerAddress = getMacAddressForDevice(pPlayerDeviceData);

		// break this players syncronization.
	        SendReceiveCommand(currentPlayerAddress + " sync -");
		SendReceiveCommand(currentPlayerAddress + " playlist clear");
		SendReceiveCommand(currentPlayerAddress + " stop");
		SendReceiveCommand(currentPlayerAddress + " power 0");

		itPlaybackDevices = vectDevices.begin();
		while ( itPlaybackDevices != vectDevices.end() && *itPlaybackDevices != pPlayerDeviceData )
			itPlaybackDevices++;

		if ( itPlaybackDevices == vectDevices.end() )
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Asked to stop a stream on a client that was not playing the stream!");
		else
			vectDevices.erase(itPlaybackDevices);

		itPlayerIds++;
	}

	if ( vectDevices.size() == 0 )
	  {
	    m_mapStreamsToPlayers.erase(iStreamID);
	  }
}

bool Slim_Server_Streamer::ConnectToSlimServerCliCommandChannel()
{
    bool socketNotOpen  = true;
    struct sockaddr_in serverSocket;
    struct hostent *host;

    m_iServerSocket = socket(PF_INET, SOCK_STREAM, 0);
    if ( m_iServerSocket == -1 )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could create client test socket. This should not happend here. Returning empty mrl.");
        return false;
    }

    if( (host = gethostbyname(m_strSlimServerCliAddress.c_str())) == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not resolve IP address: %s. Not connecting.", m_strSlimServerCliAddress.c_str());
        return false;
    }

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Trying to connect to SlimServer at address: %s:%d", m_strSlimServerCliAddress.c_str(), m_iSlimServerCliPort );
    serverSocket.sin_family = AF_INET;
    memcpy((char *) &serverSocket.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    serverSocket.sin_port = htons(m_iSlimServerCliPort);

    long timeSpent;
    struct timeval startTime, currentTime;

    gettimeofday(&startTime, NULL);
    while ( socketNotOpen )
    {
        gettimeofday(&currentTime, NULL);
        timeSpent = (currentTime.tv_sec - startTime.tv_sec) * 1000000 + (currentTime.tv_usec - startTime.tv_usec);

        if ( connect(m_iServerSocket, (struct sockaddr*)&serverSocket, sizeof(serverSocket)) == 0 )
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "We have made a successful connection to the slim server on %s:%d. Life is good.", m_strSlimServerCliAddress.c_str(), m_iSlimServerCliPort);
            socketNotOpen = false;
        }
        else
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "We can't connect yet to the server. Waiting!");
            Sleep(500);
        }

        if ( socketNotOpen && timeSpent >= 15 * 1000000 ) // wait at most 15 seconds;
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't contact the slim server for the past 3 seconds..");
            m_iServerSocket = -1;
            return false;
        }
    }

    return true;
}

string Slim_Server_Streamer::SendReceiveCommand(string command, bool bLogCommand)
{
    if ( m_iServerSocket == 0 || m_iServerSocket == -1 )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connection is down. Trying to reconnect.");
        if ( ! ConnectToSlimServerCliCommandChannel() )
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reconnection failed. Sending a start command to the application server.");

			StartSlimServer();

            Sleep(3000);
            if ( ! ConnectToSlimServerCliCommandChannel() ) // try again the connection after the command was sent.
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "I could not connect to the streaming server after i have launched it with the application server.");
                return "";
            }
        }
    }

	if (bLogCommand)
    	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending command: %s", StringUtils::URLDecode(command).c_str());

    unsigned int nBytes;
    const char *sendBuffer;
    char *receiveBuffer;

    command += "\n";

    sendBuffer = command.c_str();

    nBytes = 0;
    while ( nBytes >= 0 && nBytes < (unsigned int)command.length() - 1  )
    {
        int result = write((int)m_iServerSocket, sendBuffer + nBytes, (unsigned int)command.length() - nBytes);

        if ( result == 0 || result == -1 )
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error while sending to socket: %s.", strerror(errno));
            close((int)m_iServerSocket);
            m_iServerSocket = -1;
        }

        nBytes += result;
    }

    receiveBuffer = new char[1024];

    int pos = 0;
	int nBytesRead;
	int iRet;

	fd_set readFdSet;
	struct timeval tv;

	do
    {
		FD_ZERO( &readFdSet );
		FD_SET( m_iServerSocket, &readFdSet );

		tv.tv_sec = 0;
		tv.tv_usec = 100;

		iRet = select( (int)(m_iServerSocket+1), &readFdSet, NULL, NULL, NULL );

		if(iRet <= 0 || iRet > 1)
		{
	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Invalid select on the socket. Closing connection");
    	    close((int)m_iServerSocket);
        	m_iServerSocket = -1;
			return "";
		}

		nBytesRead = recv( m_iServerSocket, receiveBuffer + pos, 1023 - pos, 0 );
		if ( nBytesRead == -1 )
		{
	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Invalid recv from socket. Closing connection");
    	    close((int)m_iServerSocket);
        	m_iServerSocket = -1;
			return "";
		}

		pos += nBytesRead;

	} while( iRet != -1 && iRet != 1 && pos < 1023 && receiveBuffer[pos] != '\n' );

    receiveBuffer[pos - 1] = '\0';
    if ( pos == 1023 )
    {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Invalid read from socket. Closing connection");
        close((int)m_iServerSocket);
        m_iServerSocket = -1;
    }

    string result(receiveBuffer);
    delete receiveBuffer;

	if (bLogCommand)
    	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got response: %s", StringUtils::URLDecode(result).c_str());

    return result;
}

/**
 * Turn off all squeezeboxes listed in device tree, to reset.
 */
void Slim_Server_Streamer::TurnOffAllSqueezeboxes()
{
  DeviceData_Base *pSlim_Server_Streamer = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device); // Find the Slim Server Streamer
  if (!pSlim_Server_Streamer)
    {
      // Could not find Slim Server Streamer
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TurnOffAllSqueezeboxes - Could not find Slim Server Streamer device to query children.");
      return;
    }

  vector<DeviceData_Base *> vectSlim_Server_Children = pSlim_Server_Streamer->m_vectDeviceData_Base_Children;

  // Let's iterate over the children, and grab their devices, get their MAC addresses, and tell them to turn OFF.
  for (vector<DeviceData_Base *>::iterator it=vectSlim_Server_Children.begin(); it!=vectSlim_Server_Children.end(); ++it)
    {
      DeviceData_Base *pCurrent = *it;
      string sMacAddress = getMacAddressForDevice(pCurrent);
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Slim_Server_Streamer::TurnOffAllSqueezeBoxes() - Sending Power off to device %d, MAC Address %s",pCurrent->m_dwPK_Device,sMacAddress.c_str());
      SendReceiveCommand(sMacAddress + " power 0");
    }

}

bool Slim_Server_Streamer::isValidTimecode(string strResultTimecode)
{
  // TODO: implement.
  return true;
}

int Slim_Server_Streamer::stateToSpeed(StreamStateType state)
{
  int iState;

  if (state == STATE_UNDEFINED)
    {
      iState = -1;
    }
  else if (state == STATE_STOP)
    {
      iState = -2;
    }
  else if (state == STATE_PAUSE)
    {
      iState = 0;
    }
  else if (state == STATE_PLAY)
    {
      iState = 1000;
    }
  else if (state == STATE_CHANGING)
    {
      iState = 0;
    }
  else if (state == STATE_FFWD_2000)
    {
      iState = 2000;
    }
  else if (state == STATE_FFWD_4000)
    {
      iState = 4000;
    }
  else if (state == STATE_FFWD_8000)
    {
      iState = 8000;
    }
  else if (state == STATE_FFWD_16000)
    {
      iState = 16000;
    }
  else if (state == STATE_FFWD_32000)
    {
      iState = 32000;
    }
  else if (state == STATE_FFWD_64000)
    {
      iState = 64000;
    }
  else if (state == STATE_REW_250)
    {
      iState = -250;
    }
  else if (state == STATE_REW_500)
    {
      iState = -500;
    }
  else if (state == STATE_REW_1000)
    {
      iState = -1000;
    }
  else if (state == STATE_REW_2000)
    {
      iState = -2000;
    }
  else if (state == STATE_REW_4000)
    {
      iState = -4000;
    }
  else if (state == STATE_REW_8000)
    {
      iState = -8000;
    }
  else if (state == STATE_REW_16000)
    {
      iState = -16000;
    }
  else if (state == STATE_REW_32000)
    {
      iState = -32000;
    }
  else if (state == STATE_REW_64000)
    {
      iState = -64000;
    }

  return iState;

}

XineMediaInfo Slim_Server_Streamer::parseTimeCode(string strResultTimecode, string strTotalTimecode, int iStreamID, StreamStateType state)
{
  XineMediaInfo mediaInfo;
  string sTmp;
  size_t pos=0;
  bool bIsTimeCmd=false;
  bool bIsDurCmd=false;
  float fTime=0.0;
  int iTime=0;
  float fDur=0.0;
  int iDur=0;

  // Parse timecode.
  sTmp = StringUtils::Tokenize(strResultTimecode, " ",pos);
  bIsTimeCmd = (StringUtils::Tokenize(strResultTimecode, " ",pos) == "time" ? true : false);
  fTime = atof(StringUtils::Tokenize(strResultTimecode, " ",pos).c_str())*1000;
  iTime = ceil(fTime);

  // Parse duration.
  pos=0;
  sTmp = StringUtils::Tokenize(strTotalTimecode, " ",pos);
  bIsDurCmd = (StringUtils::Tokenize(strTotalTimecode, " ",pos) == "duration" ? true : false);
  fDur = atof(StringUtils::Tokenize(strTotalTimecode, " ",pos).c_str())*1000;
  iDur = ceil(fDur);
  
  // fill media info structure
  mediaInfo.m_iSpeed = stateToSpeed(state);
  mediaInfo.m_iPositionInMilliseconds = iTime;
  mediaInfo.m_iTotalLengthInMilliseconds = iDur;
  mediaInfo.m_iStreamID = iStreamID;
  mediaInfo.m_iTitle = -1;
  mediaInfo.m_iChapter = -1;
  mediaInfo.m_sFileName = "";
  mediaInfo.m_sMediaType = "4";
  mediaInfo.m_iMediaID = 0;
    
  return mediaInfo;
}

void *Slim_Server_Streamer::checkForPlaybackCompleted(void *pSlim_Server_Streamer)
{
    Slim_Server_Streamer *pStreamer = (Slim_Server_Streamer*)pSlim_Server_Streamer;
    int iSleep=500;

    while ( true )
    {
		string macAddress, strResult;

		PLUTO_SAFETY_LOCK(stateChangedLock, pStreamer->m_stateChangedMutex);
		PLUTO_SAFETY_LOCK(dataAccessLock, pStreamer->m_dataStructureAccessMutex);
		// AB 5-Jun-05 -- This code is crashing all the time accessing the vect.  Mihai didn't protect it here, maybe that's
		// why???  Without digging in deeper, I moved m_dataStructureAccessMutex above, and release it before CondWait
		while ( pStreamer->m_mapStreamsToPlayers.size() == 0 && ! pStreamer->m_bQuit_get())
		{
			dataAccessLock.Release();
			stateChangedLock.CondWait();
			dataAccessLock.Relock();
		}
		stateChangedLock.Release();

		if ( pStreamer->m_bQuit_get())
			return NULL;

		// PlutoLock pm(LOCK_PARAMS(pStreamer->m_dataStructureAccessMutex));

        map<int, pair<StreamStateType, vector<DeviceData_Base *> > >::iterator itStreamsToPlayers;
        itStreamsToPlayers = pStreamer->m_mapStreamsToPlayers.begin();
        while ( itStreamsToPlayers != pStreamer->m_mapStreamsToPlayers.end() )
		{
			macAddress = pStreamer->FindControllingMacForStream(itStreamsToPlayers->first);

			// do a SendReceive without actually logging the command ( this will potentially fill out the logs. );
 			strResult = pStreamer->SendReceiveCommand(macAddress + " mode ?", false);

// 			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Current status for stream %d is %d (result: %s)",
// 					itStreamsToPlayers->first,
// 					itStreamsToPlayers->second.first,
// 					StringUtils::URLDecode(strResult).c_str());

			if ( itStreamsToPlayers->second.first == STATE_PLAY && ( strResult == macAddress + " mode stop" || strResult == macAddress + " mode %3F" ) )
			{
				time_t tNow = time(NULL);
				time_t tStreamStart = pStreamer->m_mapStreamsToTimeStreamStarted[itStreamsToPlayers->first];
				if( tNow - tStreamStart<3 )
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring stop mode since it has only been %d seconds",tNow-tStreamStart);
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending playback completed event for stream %d started %d seconds ago", itStreamsToPlayers->first,tNow-tStreamStart);
					pStreamer->SetStateForStream((*itStreamsToPlayers).first, STATE_STOP);
					pStreamer->EVENT_Playback_Completed("",(*itStreamsToPlayers).first,false);
				}
			}
			else if ( strResult == macAddress + " mode pause" && itStreamsToPlayers->second.first != STATE_PAUSE )
			{
				pStreamer->SetStateForStream((*itStreamsToPlayers).first, STATE_PAUSE);
			}
			else if ( itStreamsToPlayers->second.first == STATE_FFWD_2000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time +2");
			    iSleep=250;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_FFWD_4000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time +4");
			    iSleep=125;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_FFWD_8000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time +8");
			    iSleep=62;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_FFWD_16000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time +16");
			    iSleep=31;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_FFWD_32000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time +32");
			    iSleep=31;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_FFWD_64000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time +64");
			    iSleep=31;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_250 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -0.25");
			    iSleep=1000;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_500 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -0.5");
			    iSleep=750;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_1000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -1");
			    iSleep=500;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_2000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -2");
			    iSleep=250;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_4000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -4");
			    iSleep=125;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_8000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -8");
			    iSleep=62;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_16000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -16");
			    iSleep=31;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_32000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -32");
			    iSleep=31;
			  }
			else if ( itStreamsToPlayers->second.first == STATE_REW_64000 )
			  {
			    pStreamer->SendReceiveCommand(macAddress + " time -64");
			    iSleep=31;
			  }
			else if ( strResult == macAddress + " mode play" && itStreamsToPlayers->second.first == STATE_PLAY )
			  {
			    // Just update the timecode.
			    string strResultTimecode = pStreamer->SendReceiveCommand(macAddress + " time ?");
			    if (pStreamer->isValidTimecode(strResultTimecode))
			      {
				string strTotalTimecode = pStreamer->SendReceiveCommand(macAddress + " duration ?");
                XineMediaInfo mediaInfo = pStreamer->parseTimeCode(strResultTimecode,
										   strTotalTimecode,
										   itStreamsToPlayers->first, 
										   itStreamsToPlayers->second.first);

                vector<DeviceData_Base *> *pvectDeviceData_Base = &(itStreamsToPlayers->second.second);
                for(size_t i = 0; i < pvectDeviceData_Base->size(); i++) {
                    if (pvectDeviceData_Base->at(i) != NULL) {
                        pStreamer->EVENT_Media_Position_Changed(pvectDeviceData_Base->at(i)->m_dwPK_Device,
                                             atoi(mediaInfo.m_sMediaType.c_str()), mediaInfo.m_sFileName,
                                             StringUtils::itos(mediaInfo.m_iMediaID),
                                             itStreamsToPlayers->first, mediaInfo.FormatTotalTime(),
                                             mediaInfo.FormatCurrentTime(), mediaInfo.m_iSpeed);
                    }
                 }
                }
			  }
            itStreamsToPlayers++;
        }

        dataAccessLock.Release();

        Sleep(iSleep);
    }

    return NULL;
}

void Slim_Server_Streamer::EVENT_Media_Position_Changed(long pkDeviceFrom, int iFK_MediaType,string sMRL,string sID,int iStream_ID,
                                                        string sDateTime,string sCurrent_Time,int iSpeed)
{
    GetEvents()->SendMessage(new Message(pkDeviceFrom, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,
            EVENT_Media_Position_Changed_CONST,
            7 /* number of parameter's pairs (id, value) */,
            EVENTPARAMETER_FK_MediaType_CONST, StringUtils::itos(iFK_MediaType).c_str(),
            EVENTPARAMETER_MRL_CONST, sMRL.c_str(),
            EVENTPARAMETER_ID_CONST, sID.c_str(),
            EVENTPARAMETER_Stream_ID_CONST, StringUtils::itos(iStream_ID).c_str(),
            EVENTPARAMETER_DateTime_CONST, sDateTime.c_str(),
            EVENTPARAMETER_Current_Time_CONST, sCurrent_Time.c_str(),
            EVENTPARAMETER_Speed_CONST, StringUtils::itos(iSpeed).c_str()));
}

string Slim_Server_Streamer::FindControllingMacForStream(int iStreamID)
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);

    if ( m_mapStreamsToPlayers.find(iStreamID) != m_mapStreamsToPlayers.end() )
    {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Slim_Server_Streamer::FindControllingMacForStream vect: found the m_mapStreamsToPlayers");
		vector<DeviceData_Base *> *pvectDeviceData_Base = &(m_mapStreamsToPlayers[iStreamID].second);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Slim_Server_Streamer::FindControllingMacForStream vect: %p size: %d",
			pvectDeviceData_Base,(int) pvectDeviceData_Base->size());

		if( pvectDeviceData_Base->size()==0 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Slim_Server_Streamer::FindControllingMacForStream vect is 0");
			return "";
		}

        DeviceData_Base *playerDevice = pvectDeviceData_Base->front();

		return getMacAddressForDevice(playerDevice);
    }

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find an controlling device for stream: %d", iStreamID);
	return "";
}

StreamStateType Slim_Server_Streamer::GetStateForStream(int iStreamID)
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	//PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	if ( m_mapStreamsToPlayers.find(iStreamID) == m_mapStreamsToPlayers.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Can't get the state of an invalid stream: %d!", iStreamID);
		return STATE_UNDEFINED;
	}

	return m_mapStreamsToPlayers[iStreamID].first;
}

void Slim_Server_Streamer::SetStateForStream(int iStreamID, StreamStateType newState)
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	//PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting state for stream %d to %d", iStreamID, newState);

    if ( m_mapStreamsToPlayers.find(iStreamID) == m_mapStreamsToPlayers.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Can't change the state of an invalid stream: %d!", iStreamID);
		return;
	}

	m_mapStreamsToPlayers[iStreamID].first = newState;
	if( newState==STATE_PLAY )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Noting start time for stream %d", iStreamID);
		m_mapStreamsToTimeStreamStarted[iStreamID] = time(NULL);
	}
}

//<-dceag-c37-b->

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

void Slim_Server_Streamer::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "No controlling mac was found for this stream id: %d", iStreamID);
		return;
	}

 	// synchronize the rest of the devices in the stream's list with the main one
 	vector<DeviceData_Base *> *pvectDeviceData_Base = &(m_mapStreamsToPlayers[iStreamID].second);
 	vector<DeviceData_Base *>::iterator itDeviceData;
	SendReceiveCommand(sControlledPlayerMac + " sync -");
 	for (itDeviceData = pvectDeviceData_Base->begin(); itDeviceData != pvectDeviceData_Base->end(); ++itDeviceData)
 	{
 		string sMacCurrentDevice = getMacAddressForDevice(*itDeviceData);
 		if (sMacCurrentDevice == sControlledPlayerMac)
 			continue; // don't sync with ourselves
 		SendReceiveCommand(sControlledPlayerMac + " sync " + sMacCurrentDevice);
 	}
 
	SetStateForStream(iStreamID, STATE_CHANGING);
	
	// tschak - adding an if here to handle other URL types. I am looking for two slashes in succession, if so, then 
	// i simply do not prepend the URL stuff and decode it.
	
	if (sMediaURL.find("://") != string::npos)
	{
		SendReceiveCommand(sControlledPlayerMac + " playlist play " + StringUtils::URLEncode(StringUtils::Replace(&sMediaURL,"/", "/")));
	} else if (sMediaURL.find("/asterisk/voicemail") != string::npos)
	{
		SendReceiveCommand(sControlledPlayerMac + " playlist play " + sMediaURL);
	} else	
	{
		SendReceiveCommand(sControlledPlayerMac + " playlist play " + StringUtils::URLEncode(string("file://") + StringUtils::Replace(&sMediaURL,"//", "/")));
	}

#if 0
	string strResult = SendReceiveCommand(sControlledPlayerMac + " mode ?", false);
	if( strResult == sControlledPlayerMac + " mode stop" || strResult == sControlledPlayerMac + " mode %3F" )
	{
		// 2005-10-8 For some reason, slimserver often reports its state is stopped the first time it tries to play a file,
		// but it works on subsequent plays.  It seems to be a bug in slimserver, but since I don't know this code well,
		// just put a hack in here for the moment by making it try again
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Squeeze box reports it's state is stopped right after a new start.  Try again");
		SendReceiveCommand(sControlledPlayerMac + " playlist play " + StringUtils::URLEncode(string("file://") + StringUtils::Replace(&sMediaURL,"//", "/")));
		strResult = SendReceiveCommand(sControlledPlayerMac + " mode ?", false);
		if( strResult == sControlledPlayerMac + " mode stop" || strResult == sControlledPlayerMac + " mode %3F" )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"A second time!  Try again");
			SendReceiveCommand(sControlledPlayerMac + " playlist clear");
			SendReceiveCommand(sControlledPlayerMac + " stop");
			SendReceiveCommand(sControlledPlayerMac + " playlist play " + StringUtils::URLEncode(string("file://") + StringUtils::Replace(&sMediaURL,"//", "/")));
		}
	}
#endif

	CMD_Set_Media_Position(iStreamID, sMediaPosition, sCMD_Result, pMessage);

	SetStateForStream(iStreamID, STATE_PLAY);
}

//<-dceag-c38-b->

	/** @brief COMMAND: #38 - Stop Media */
	/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
		/** @param #41 StreamID */
			/** The media needing to be stopped. */
		/** @param #42 MediaPosition */
			/** The position at which this stream was last played. */

void Slim_Server_Streamer::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	// PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stop Media for stream %d",iStreamID);
	string sText;
	CMD_Report_Playback_Position(iStreamID,&sText,sMediaPosition,sCMD_Result,pMessage);

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"CMD_Stop_Media - Erase StreamID %d from m_mapStreamsToPlayers",iStreamID);
	// 	m_mapStreamsToPlayers.erase(iStreamID);
	SendReceiveCommand(sControlledPlayerMac + " playlist clear");
	SendReceiveCommand(sControlledPlayerMac + " stop");
	SendReceiveCommand(sControlledPlayerMac + " power 0");
}

//<-dceag-c39-b->

	/** @brief COMMAND: #39 - Pause Media */
	/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
		/** @param #41 StreamID */
			/** The media stream for which we need to pause playback. */

void Slim_Server_Streamer::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	//PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	// SendReceiveCommand(lastPlayerAddress + " playlist play " + StringUtils::URLEncode(string("file://") + sFilename).c_str());

	if ( GetStateForStream(iStreamID) == STATE_PLAY ||
	     GetStateForStream(iStreamID) == STATE_FFWD_2000 ||
	     GetStateForStream(iStreamID) == STATE_FFWD_4000 ||
	     GetStateForStream(iStreamID) == STATE_FFWD_8000 ||
	     GetStateForStream(iStreamID) == STATE_FFWD_16000 || 
	     GetStateForStream(iStreamID) == STATE_FFWD_32000 ||
	     GetStateForStream(iStreamID) == STATE_FFWD_64000 ||
	     GetStateForStream(iStreamID) == STATE_REW_250 ||
	     GetStateForStream(iStreamID) == STATE_REW_500 ||
	     GetStateForStream(iStreamID) == STATE_REW_1000 ||
	     GetStateForStream(iStreamID) == STATE_REW_2000 ||
	     GetStateForStream(iStreamID) == STATE_REW_4000 ||
	     GetStateForStream(iStreamID) == STATE_REW_8000 ||
	     GetStateForStream(iStreamID) == STATE_REW_16000 ||
	     GetStateForStream(iStreamID) == STATE_REW_32000 ||
	     GetStateForStream(iStreamID) == STATE_REW_64000)
	{
		SendReceiveCommand(sControlledPlayerMac + " pause 1");
		SetStateForStream(iStreamID, STATE_PAUSE);
	}
}

//<-dceag-c40-b->

	/** @brief COMMAND: #40 - Restart Media */
	/** This will restart a media was paused with the above command */
		/** @param #41 StreamID */
			/** The media stream that we need to restart playback for. */

void Slim_Server_Streamer::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
	CMD_Pause_Media(iStreamID,sCMD_Result,pMessage);
}

//<-dceag-c41-b->

	/** @brief COMMAND: #41 - Change Playback Speed */
	/** Will make the playback to FF with a configurable amount of speed. */
		/** @param #41 StreamID */
			/** The media needing the playback speed change. */
		/** @param #43 MediaPlaybackSpeed */
			/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
		/** @param #220 Report */
			/** If true, report this speed to the user on the OSD */

void Slim_Server_Streamer::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	if ( GetStateForStream(iStreamID) == STATE_PAUSE)
	{
		SendReceiveCommand(sControlledPlayerMac + " pause 0");
		SetStateForStream(iStreamID, STATE_PLAY);
	}
	else if ( iMediaPlaybackSpeed == 1000 )
	  {
	    SetStateForStream(iStreamID, STATE_PLAY);
	  }
	else if ( iMediaPlaybackSpeed == 2000 )
	  {
	    SetStateForStream(iStreamID, STATE_FFWD_2000);
	  }
	else if ( iMediaPlaybackSpeed == 4000 )
	  {
	    SetStateForStream(iStreamID, STATE_FFWD_4000);
	  }
	else if ( iMediaPlaybackSpeed == 8000 )
	  {
	    SetStateForStream(iStreamID, STATE_FFWD_8000);
	  }
	else if ( iMediaPlaybackSpeed == 16000 )
	  {
	    SetStateForStream(iStreamID, STATE_FFWD_16000);
	  }
	else if ( iMediaPlaybackSpeed == 32000 )
	  {
	    SetStateForStream(iStreamID, STATE_FFWD_32000);
	  }
	else if ( iMediaPlaybackSpeed == 64000 )
	  {
	    SetStateForStream(iStreamID, STATE_FFWD_64000);
	  }
	else if ( iMediaPlaybackSpeed == -250 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_250);
	  }
	else if ( iMediaPlaybackSpeed == -500 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_500);
	  }
	else if ( iMediaPlaybackSpeed == -1000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_1000);
	  }
	else if ( iMediaPlaybackSpeed == -2000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_2000);
	  }
	else if ( iMediaPlaybackSpeed == -4000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_4000);
	  }
	else if ( iMediaPlaybackSpeed == -8000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_8000);
	  }
 	else if ( iMediaPlaybackSpeed == -16000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_16000);
	  }
	else if ( iMediaPlaybackSpeed == -32000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_32000);
	  }
	else if ( iMediaPlaybackSpeed == -64000 )
	  {
	    SetStateForStream(iStreamID, STATE_REW_64000);
	  }

}

//<-dceag-c259-b->

	/** @brief COMMAND: #259 - Report Playback Position */
	/** This will report the playback position of the current stream. */
		/** @param #9 Text */
			/** A human readable representation of the current position */
		/** @param #41 StreamID */
			/** The stream ID on which to report the position. */
		/** @param #42 MediaPosition */
			/** A media player readable representation of the current position including all options */

void Slim_Server_Streamer::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
	string commandResult;
	string command;
	string sPlayerMac;
	float floatValue;
	int iMediaPosition=0, iMedia_Length=0;

	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);

	if ( (sPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
		return;

	command = sPlayerMac + " gototime ";
	if ( (commandResult = SendReceiveCommand(command + "?")) == "")
	{
		sCMD_Result = "Could not get current position.";
		return;
	}

	if ( sscanf(commandResult.substr(command.length()).c_str(), "%f", &floatValue) == 0 )
	{
		sCMD_Result = "Could not parse current position";
		return;
	}

	iMediaPosition = (int)(floatValue * 1000);

	command = sPlayerMac + " duration ";
	if ( (commandResult = SendReceiveCommand(command + "?")) == "")
	{
		sCMD_Result = "Could not get duration.";
		return;
	}

	if ( sscanf(commandResult.substr(command.length()).c_str(), "%f", &floatValue) == 0 )
	{
		sCMD_Result = "Could not parse duration.";
		return;
	}

	iMedia_Length = (int)(floatValue * 1000);


	string sPosition;
	sPosition += " POS:" + StringUtils::itos(iMediaPosition) + " TOTAL:"+ StringUtils::itos(iMedia_Length);
	*sMediaPosition = sPosition;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Detected data: position %d from %d", iMediaPosition, iMedia_Length);
}
//<-dceag-createinst-b->!
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/** volume up */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void Slim_Server_Streamer::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	//PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	if ( pMessage->m_mapParameters.find(COMMANDPARAMETER_StreamID_CONST) == pMessage->m_mapParameters.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The stream id was not set on the message! The media plugin should have set this parameter in this message!");
		return;
	}

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str()))) == "" )
		return;

	if ( iRepeat_Command == 0 )
		iRepeat_Command = 1;

	if ( iRepeat_Command >= 100 )
		iRepeat_Command = 100;

	SendReceiveCommand(sControlledPlayerMac + " mixer volume +" + StringUtils::itos(iRepeat_Command));
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/** volume down */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void Slim_Server_Streamer::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	// PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	if ( pMessage->m_mapParameters.find(COMMANDPARAMETER_StreamID_CONST) == pMessage->m_mapParameters.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The stream id was not set on the message! The media plugin should have set this parameter in this message!");
		return;
	}

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str()))) == "" )
		return;

	if ( iRepeat_Command == 0 )
		iRepeat_Command = 1;

	if ( iRepeat_Command >= 100 )
		iRepeat_Command = 100;

	SendReceiveCommand(sControlledPlayerMac + " mixer volume -" + StringUtils::itos(iRepeat_Command));
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/** mute */

void Slim_Server_Streamer::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);
	//PlutoLock dataMutexLock(LOCK_PARAMS(m_dataStructureAccessMutex));

	if ( pMessage->m_mapParameters.find(COMMANDPARAMETER_StreamID_CONST) == pMessage->m_mapParameters.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The stream id was not set on the message! The media plugin should have set this parameter in this message!");
		return;
	}

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str()))) == "" )
		return;

	SendReceiveCommand(sControlledPlayerMac + " mixer muting");
}

void Slim_Server_Streamer::OnQuit()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Calling base impl ");

	Command_Impl::OnQuit();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Quitting and signalling. ");
	pthread_cond_signal(&m_stateChangedCondition);
}

string Slim_Server_Streamer::getMacAddressForDevice(DeviceData_Base *pDevice)
{
	string macAddress;

	if ( pDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Xine_Player_CONST )
		macAddress = StringUtils::makeUpPlayerAddressFromPlayerId(pDevice->m_dwPK_Device);
	else
		macAddress = pDevice->GetMacAddress();

	return StringUtils::URLEncode(StringUtils::ToLower(macAddress));
}

//<-dceag-c412-b->

	/** @brief COMMAND: #412 - Set Media Position */
	/** Jump to a certain media position */
		/** @param #41 StreamID */
			/** The stream to set */
		/** @param #42 MediaPosition */
			/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void Slim_Server_Streamer::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{

	PLUTO_SAFETY_LOCK(dataMutexLock, m_dataStructureAccessMutex);

	string sControlledPlayerMac;
	if ( (sControlledPlayerMac = FindControllingMacForStream(iStreamID)) == "" )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "No controlling mac was found for this stream id: %d", iStreamID);
		return;
	}

	uint posPos = sMediaPosition.find(" POS:");
	int iPos = 0;
	if( posPos != string::npos )
	        iPos = atoi( sMediaPosition.substr(posPos+5).c_str() );
	iPos = iPos / 1000; // slim uses seconds

	if ( iPos > 0 )
	{
		SendReceiveCommand(sControlledPlayerMac + " gototime " + StringUtils::itos(iPos));
	}

}

//<-dceag-c42-b->

	/** @brief COMMAND: #42 - Jump to Position in Stream */
	/** Jump to a position in the stream, specified in seconds. */
		/** @param #5 Value To Assign */
			/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
		/** @param #41 StreamID */
			/** The stream */

void Slim_Server_Streamer::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
}
//<-dceag-c65-b->

	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Slim_Server_Streamer::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
}

void Slim_Server_Streamer::StartSlimServer()
{
	DeviceData_Base *pDevice_AppServer = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_App_Server_CONST);
	if( pDevice_AppServer )
	{
		time_t tStop = time(NULL)+90;
		while( time(NULL)<tStop )
		{
			string sResponse;
			DCE::CMD_Kill_Application CMD_Kill_Application(
					m_dwPK_Device, pDevice_AppServer->m_dwPK_Device,
					"slim-server",                            // reference it with this name
					true								// Send the message
				);                                        // execute this serialized message on exit with success
			if( !SendCommand(CMD_Kill_Application,&sResponse) && sResponse!="Cannot kill" )
			{
				Sleep(2000);  // App Server must not be registered yet.  Try again
				continue;
			}

			DCE::CMD_Spawn_Application spawnApplication(
					m_dwPK_Device, pDevice_AppServer->m_dwPK_Device,
					"/usr/pluto/bin/LaunchSlimServer.sh",  // launch this
					"slim-server",                              // reference it with this name
					StringUtils::itos(m_iSlimServerCliPort),    // pass it the desired port number for reconnection.
					"",                                         // execute this serialized message on exit with failure
					"",											// execute this serialized message on exit with success
					false,false,false,true);
			SendCommand(spawnApplication);
			break; //
		}
	}
}
//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void Slim_Server_Streamer::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	//TODO: implement me!
}

//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Show a menu associated with this media */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Slim_Server_Streamer::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
}
//<-dceag-c32-b->

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

void Slim_Server_Streamer::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
}
//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void Slim_Server_Streamer::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
}
//<-dceag-c916-b->

	/** @brief COMMAND: #916 - Set Aspect Ratio */
	/** Force aspect ratio */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #260 Aspect Ratio */
			/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void Slim_Server_Streamer::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
}
//<-dceag-c917-b->

	/** @brief COMMAND: #917 - Set Zoom */
	/** Sets zoom level, relative, absolute or 'auto' */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #261 Zoom Level */
			/** Zoom level to set */

void Slim_Server_Streamer::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
}
//<-dceag-c920-b->

	/** @brief COMMAND: #920 - Set Media ID */
	/** Set Media ID - information about media stream */
		/** @param #10 ID */
			/** Media ID (special format) */
		/** @param #41 StreamID */
			/** ID of stream to set media information for */

void Slim_Server_Streamer::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
}
