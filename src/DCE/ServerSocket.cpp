/*
ServerSocket

Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU Lesser General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"

#include "SocketListener.h"
#include "ServerSocket.h"

#ifndef WIN32
#include "arp-cache.h"
#endif

using namespace DCE;

void *ServerSocket::BeginWapClientThread(void *SvSock)
{
	ServerSocket *pServerSocket = (ServerSocket *)SvSock;

	//if the router is already reloading, we'll let the listener to take care of our deallocation
	if(pServerSocket->ServeClient() && !pServerSocket->m_bAlreadyRemoved && NULL != pServerSocket->m_pListener && !pServerSocket->m_pListener->m_bTerminate)
	{
		//I'll do it by myself
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Detected disconnect: detaching thread for %d", pServerSocket->m_dwPK_Device);
		pServerSocket->SelfDestroying();
		pthread_detach(pServerSocket->m_ClientThreadID);
//#ifdef WIN32
//		pServerSocket->m_ClientThreadID = pthread_t{ NULL,0 };
//#else
//		pServerSocket->m_ClientThreadID = (pthread_t)NULL;
//#endif
		//Remove me from listener's lists
		pServerSocket->m_pListener->RemoveAndDeleteSocket(pServerSocket);
	}

	return NULL;
}

ServerSocket::ServerSocket( SocketListener *pListener, SOCKET Sock, string sName, string sIPAddress, string sMacAddress, SSL* ssl, bool isSSL ) :
  Socket( sName, sIPAddress, sMacAddress, ssl, isSSL ),
	m_bSelfDestroying(false),
	m_ConnectionMutex( "connection " + sName ),
#ifdef PTHREAD2
	m_ClientThreadID(pthread_t { NULL, 0 })
#else
	m_ClientThreadID((pthread_t)NULL)
#endif
{
	m_iInstanceID = 0;
	m_bSendOnlySocket = false;
	m_bAlreadyRemoved = false;
	m_iReferencesOutstanding=0;
	m_dwPK_Device = (long unsigned int)-1;
	m_Socket = Sock;

	m_bAskBeforeReload=false;
	m_bImplementsPendingTasks=false;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "ServerSocket::Created %p m_Socket: %d", this, m_Socket );
#endif
	m_pListener = pListener;
	m_bThreadRunning=true;

	m_ConnectionMutex.Init( NULL );
}

ServerSocket::~ServerSocket()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "ServerSocket::~ServerSocket(): @%p/%d Is it running %d?", this, m_Socket, m_bThreadRunning);
#endif

	//Announce socket we're done
	Close();

	//Make sure we are no longer in listener's lists
	if(!m_bAlreadyRemoved && m_dwPK_Device != DEVICEID_MESSAGESEND)
		m_pListener->RemoveAndDeleteSocket(this, true /*don't delete me*/);

	//Wait for our thread to finish
#ifdef PTHREAD2
	if (NULL != m_ClientThreadID.p)
#else
	if ( ((pthread_t)NULL) != m_ClientThreadID )
#endif
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Forcing a disconnect: joining thread for %d", m_dwPK_Device);
		pthread_join(m_ClientThreadID, NULL);
#ifdef PTHREAD2
		m_ClientThreadID = pthread_t { NULL, 0 };
#else
		m_ClientThreadID = (pthread_t)NULL;
#endif
	}

	//Wait for any outstanding locks to finish
	PLUTO_SAFETY_LOCK(cm, m_ConnectionMutex);
	cm.Release();

	//Destroy our mutex
	pthread_mutex_destroy(&m_ConnectionMutex.mutex);

	//make sure we won't hang on Socket's destructor
	m_bUsePingToKeepAlive = false;
}


void ServerSocket::Run()
{
	int iResult = pthread_create(&m_ClientThreadID, NULL, BeginWapClientThread, (void *)this);

	if(iResult != 0)
	{
		m_bThreadRunning = false;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Pthread create returned %d %s dev %d ptr %p", (int)iResult, m_sName.c_str(), m_dwPK_Device,this );

		//TODO: leaks ?
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread created for %s dev %d ptr %p", m_sName.c_str(), m_dwPK_Device, this);
	}
}

bool ServerSocket::ServeClient()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Running socket %p... m_bTerminate: %d", this, m_pListener->m_bTerminate );
#endif

	string sMessage;
	while( !m_pListener->m_bTerminate )
	{
		if (m_bSendOnlySocket)
		{
			if (m_Socket == INVALID_SOCKET)
				break;
			else
			{
				Sleep(100); // sleep 100ms
				continue;
			}
		}

		if ( !ReceiveString( sMessage ) || m_pListener->m_bTerminate )
		{
			break;
		}

		if( sMessage.size()<2 )  // Got to be at least 2 characters
			continue;

		if ( sMessage.length() >= 4 && sMessage.substr(0,4) == "TIME" )
		{
			time_t t;
			time(&t);
			struct tm lt;
			struct tm gt;
			{
				PLUTO_SAFETY_LOCK( cm, m_ConnectionMutex );
				memcpy( &lt, localtime(&t), sizeof(struct tm) );
				memcpy( &gt, gmtime(&t), sizeof(struct tm) );
			}
			char s[128];
			sprintf( s, "TIME|%d|%d|%d|%d|%d|%d|UGMT|%lu|BIAS|%lu|%d",
				lt.tm_hour, lt.tm_min, lt.tm_sec, lt.tm_mon+1, lt.tm_mday,
				lt.tm_year+1900, (long unsigned int)t, (long unsigned int) mktime(&lt) - mktime(&gt), lt.tm_isdst );
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device %d requested time: sending %s",m_dwPK_Device,s);
#endif
			SendString( s );
			continue;
		}

		if ( sMessage.length() >= 5 && (sMessage.substr(0,5) == "MY_IP") )
		{
#ifndef WIN32
			string sMacAddress=arpcache_MACfromIP(m_sIPAddress);
#else
			string sMacAddress="WINDOWS";
#endif
			SendString("OK IP:" + m_sIPAddress + " MAC:" + sMacAddress);
		}

		if ( sMessage.length() >= 5 && (sMessage.substr(0,5) == "EVENT" || /* TODO - REMVOE HELLO ONCE ALL DEVICES USE EVENT */ sMessage.substr(0,5)=="HELLO") )
		{
#ifdef LL_DEBUG
			SocketInfo *pSocketInfo = g_mapSocketInfo_Find(m_iSocketCounter,m_sName,this);
			pSocketInfo->m_sDevice=sMessage;
#endif

			m_dwPK_Device = atoi( sMessage.substr(6).c_str() );

			string sMacAddress;
#ifndef WIN32
			if( m_sIPAddress.size() )
				sMacAddress=arpcache_MACfromIP(m_sIPAddress);
#else
			sMacAddress="11:22";
#endif
			string sIPAndMac = (m_sIPAddress.size() ? " IP=" + m_sIPAddress : "") + (sMacAddress.size() ? " MAC=" + sMacAddress : "");
			if( m_dwPK_Device == DEVICEID_MESSAGESEND )
			{
				SendString("OK");
				continue;
			}

			string::size_type pos=sMessage.find("T=");
			int PK_DeviceTemplate = 0;
			if( pos!=string::npos )
				PK_DeviceTemplate = atoi(sMessage.substr(pos+2).c_str());

			// See if the device sent us a device template only
			if( !m_dwPK_Device && PK_DeviceTemplate )
			{
				m_dwPK_Device = m_pListener->GetDeviceID(PK_DeviceTemplate,sMacAddress,m_sIPAddress);
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Could not determine device id based on ip or mac with %s",sMessage.c_str());
			}

			if( !m_dwPK_Device )
				SendString( "BAD DEVICE" + sIPAndMac );
			else
			{
				int iResponse = m_pListener->ConfirmDeviceTemplate( m_dwPK_Device, PK_DeviceTemplate );

				if( iResponse==0 )
				{
					SendString( "NOT IN THIS INSTALLATION" + sIPAndMac);
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device %d registered but it doesn't exist",m_dwPK_Device);
					m_bThreadRunning=false;
					return true;
				}

				if( PK_DeviceTemplate && iResponse!=2 )
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %d connected as foreign template %d",m_dwPK_Device, PK_DeviceTemplate);

				if( iResponse==3 )
				{
					SendString( "NEED RELOAD" + sIPAndMac);
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Device %d registered but the router needs to be reloaded",m_dwPK_Device);
				}
				else
					SendString( "OK " + StringUtils::itos(m_dwPK_Device) + sIPAndMac );

				m_sName += sMessage;
			}

			m_pListener->RegisterEventHandler( this, m_dwPK_Device );
			continue;
		}

#ifdef LL_DEBUG
		if (  sMessage.length() >= 7 && sMessage.substr(0,7) == "COMMENT" )
		{
			SocketInfo *pSocketInfo = g_mapSocketInfo_Find(m_iSocketCounter,m_sName,this);
			pSocketInfo->m_sComment=sMessage;
		}
#endif

		if ( (sMessage.length() > 7 && sMessage.substr(0,7) == "COMMAND") || /* TODO -- DELETE THIS ONCE ALL USE COMMAND */ (sMessage.length() > 14 && sMessage.substr(0,14) == "REQUESTHANDLER") )
		{
#ifdef LL_DEBUG
			SocketInfo *pSocketInfo = g_mapSocketInfo_Find(m_iSocketCounter,m_sName,this);
			pSocketInfo->m_sDevice=sMessage;
#endif
			SendString( "OK" );

			if( sMessage[0]=='C' )
				m_dwPK_Device = atoi( sMessage.substr(7).c_str() );
			else  // TODO -- DELETE THIS ONCE ALL USE COMMAND
				m_dwPK_Device = atoi( sMessage.substr(14).c_str() );

			string::size_type pos_instance = sMessage.find("INSTANCE");
			if( pos_instance!=string::npos )
				m_iInstanceID = atoi(sMessage.substr(pos_instance+8).c_str());

			if( sMessage.find("ASK_BEFORE_RELOAD")!=string::npos )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "ServerSocket::ServeClient m_bAskBeforeReload device %d %s", m_dwPK_Device, sMessage.c_str());
				m_bAskBeforeReload=true;
			}
			if( sMessage.find("IMPLEMENTS_PENDING_TASKS")!=string::npos )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "ServerSocket::ServeClient m_bImplementsPendingTasks device %d %s", m_dwPK_Device, sMessage.c_str());
				m_bImplementsPendingTasks=true;
			}

			//string::size_type pos_flags = sMessage.find("INSTANCE");
			if( pos_instance!=string::npos )
				m_iInstanceID = atoi(sMessage.substr(pos_instance+8).c_str());

			if( m_dwPK_Device!=DEVICEID_MESSAGESEND )  // Special for temporary, 1 way devices
				m_pListener->RegisterCommandHandler( this, m_dwPK_Device );

			// This is a request handler. It will only be used for messages
			// originating FROM other sources, so no thread is necessary.
			// The socket will be periodically checked by the core
			// or closed if replaced by another command handler.
			m_bThreadRunning=false;

			return false;
		}
		else if( sMessage.substr(0,8) == "INSTANCE" )
		{
			m_iInstanceID = atoi(sMessage.substr(8).c_str());
			continue;
		}

		if ( sMessage == "BYE" )
		{
			SendString( "OK" );
			break;
		}

		if ( m_dwPK_Device == -1 )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Received %s, but device hasn't identified itself yet.", sMessage.c_str() );
			m_pListener->ReceivedString( this, sMessage );
		}
		else
		{
			if(sMessage.find("MESSAGE") == 0)
			{
				Message *pMessage = ReceiveMessageRaw(sMessage);
				if ( pMessage )
				{
					if ( pMessage->m_vectExtraMessages.size() )
					{
						for( size_t s=0; s < pMessage->m_vectExtraMessages.size(); ++s )
						{
							Message *pMessage_Child = pMessage->m_vectExtraMessages[s];
							if ( pMessage_Child->m_dwPK_Device_To != pMessage->m_dwPK_Device_To )
							{
								// This embedded message has a different destination.  We need to send it separately
								m_pListener->ReceivedMessage( this, pMessage_Child );
								pMessage->m_vectExtraMessages[s] = NULL; // So it doesn't go to the primary destination also
							}
						}
					}
					m_pListener->ReceivedMessage( this, pMessage );
				}
			}
			else
				m_pListener->ReceivedString( this, sMessage );
		}
	}


	LoggerWrapper::GetInstance()->Write( LV_WARNING, "TCPIP: Closing connection to %d (%s) %p m_Socket: %d", m_dwPK_Device,m_pListener->m_sName.c_str(), this, m_Socket );

	m_pListener->OnDisconnected( m_dwPK_Device );
	m_bThreadRunning=false;

	return true;
}

void ServerSocket::PingFailed()
{
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "ServerSocket::PingFailed");
	Close();
	m_pListener->PingFailed( this, m_dwPK_Device );
}

bool ServerSocket::SafeToReload(string &sReason)
{
	string sResponse = SendReceiveString("SAFE_TO_RELOAD");
	if( StringUtils::StartsWith(sResponse,"RELOAD_YES") )
		return true;
	else if( StringUtils::StartsWith(sResponse,"RELOAD_NO") )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "ServerSocket::SafeToReload device %d returned %s", m_dwPK_Device, sResponse.c_str() );
		sReason = sResponse.substr(10);
		return false;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "SafeToReload failed with %d (%s) %p m_Socket: %d", m_dwPK_Device,m_pListener->m_sName.c_str(), this, m_Socket );
		return true; // Don't let this device stop a reload, it's not responding anyway
	}
}
