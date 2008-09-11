/*
	HandleRequestSocket

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Socket.h"
#include "Message.h"
#include "HandleRequestSocket.h"

using namespace DCE;

void *BeginHandleRequestThread( void *HRqSock )
{
	HandleRequestSocket *pCS = (HandleRequestSocket *)HRqSock;
	pCS->RunThread();

	LoggerWrapper::GetInstance()->Write(LV_SOCKET,"Exiting BeginHandleRequestThread thread...");
	return NULL;
}

HandleRequestSocket::HandleRequestSocket( int iDeviceID, string sIPAddress, string sName ) :
	ClientSocket( iDeviceID, sIPAddress, sName )
{
	m_RequestHandlerThread = 0;
	m_bUnexpected = false;
	m_bRunning = false;
	/** @todo check comment */
	//g_pDCELogger->Write(LV_SOCKET,"Created handle request socket %p device: %d ip: %s",this,DeviceID,IPAddress.c_str());
}

HandleRequestSocket::~HandleRequestSocket()
{
	DisconnectAndWait();
}

void HandleRequestSocket::Disconnect()
{
	// We override Disconnect so our processor thread knows the difference
	// between an explicit disconnect and an unexpected one.

	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "RequestSocket::Disconnect %p device: %d", this, m_dwPK_Device );

	m_bTerminate = true;
	ClientSocket::Disconnect();
}

void HandleRequestSocket::DisconnectAndWait()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "~HandleRequestSocket %p device: %d ip: %s", this, m_dwPK_Device, m_sIPAddress.c_str() );
#endif
	Disconnect();

	time_t tTimeout = time(NULL) + 15;  // Wait only 15 seconds
	// This used to be pthread_join, but it crashed without logical explanation on a very intermittent basis.
	// Try just waiting this way.
	while( m_bRunning && tTimeout > time(NULL) )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Requesthandler %p (device: %d) waiting for runThread", this, m_dwPK_Device );
		Sleep(10);
	}

	if( m_bRunning && m_RequestHandlerThread )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Requesthandler %p (device: %d) runThread won't die!", this, m_dwPK_Device );

#ifndef WINCE //not defined under ce :(
		pthread_cancel(m_RequestHandlerThread);
#endif
	}

	m_RequestHandlerThread = 0;
}

bool HandleRequestSocket::OnConnect( int PK_DeviceTemplate,string sExtraInfo )
{
	SendString( "COMMAND " + StringUtils::itos( m_dwPK_Device ) + " " + sExtraInfo );
	string sResponse;
	if ( !ReceiveString( sResponse ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Requesthandler %p (device: %d) lost connection. %s", this, m_dwPK_Device, m_sName.c_str() );
		return false;
	}
	if ( sResponse == "CLOSED" ) return false;

	if ( sResponse != "OK" )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Connection for requesthandler %p (device: %d) reported %s. %s", this, m_dwPK_Device, sResponse.c_str(), m_sName.c_str() );
		return false;
	}

	m_bTerminate = false;

	if( 0 != pthread_create( &m_RequestHandlerThread, NULL, BeginHandleRequestThread, (void *)this ) )
		return false;

	return true;
}

void HandleRequestSocket::RunThread()
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Requesthandler %p (device: %d) runThread now running",
		this, m_dwPK_Device );
	m_bRunning = true;

	string sMessage = "";
	while( !m_bTerminate && !m_bQuit_get() )
	{
		if ( !ReceiveString( sMessage ) )
		{
			if( m_bQuit_get() )
				break;
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Receive string failed in HandleRequestSocket %d:%s %s", sMessage.size(), sMessage.c_str(), m_sName.c_str() );
			if (m_bTerminate==false)
			{
				PlutoLock::DumpOutstandingLocks();
				m_bUnexpected = true;
				m_bTerminate = true;
			}
		}
		else
		{
			if( m_bQuit_get() )
				break;

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Receive string: %s", sMessage.c_str());
#endif
			if ( sMessage == "CORRUPT SOCKET" )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Socket flagged as corrupted %p device: %d", this, m_dwPK_Device );
				m_bUnexpected = true;
				m_bTerminate = true;
			}
			else if ( sMessage.substr(0,7) == "REPLACE" && sMessage.size()>7 )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Another device with IP %s connected as device: %d", sMessage.c_str(), m_dwPK_Device );
                m_bTerminate = true;
                if(OnReplaceHandler(sMessage.substr(7)))
				{
					m_bRunning = false;
                    return;
				}
			}
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Received %s %p device: %d", sMessage.c_str(), this, m_dwPK_Device);
#endif
			if ( sMessage.substr(0,7)  == "MESSAGE" && sMessage.size()>7 )
			{
				Message *pMessage = ReceiveMessageRaw(sMessage);
				if ( pMessage )
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Received Message type %d ID %d from %d to %d (device: %d) resp %d",
						pMessage->m_dwMessage_Type, pMessage->m_dwID, pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To, m_dwPK_Device,
						(int) pMessage->m_eExpectedResponse);
#endif
					ReceivedMessageResult receivedMessageResult = ReceivedMessage( pMessage );
					if ( receivedMessageResult == rmr_NotProcessed )
					{
						LoggerWrapper::GetInstance()->Write( LV_STATUS, "Could not find a handler for message - from %d to %d Type: %d ID: %d (device: %d) %s",
							pMessage->m_dwPK_Device_From, pMessage->m_dwPK_Device_To,
							pMessage->m_dwMessage_Type, pMessage->m_dwID, m_dwPK_Device, m_sName.c_str() );
						if( pMessage->m_eExpectedResponse!=ER_None && !pMessage->m_bRespondedToMessage )
						{
							SendString("FAIL");
							pMessage->m_bRespondedToMessage=true;
						}
					}
					if( receivedMessageResult != rmr_Buffered )
						delete pMessage;
				}
				else
				{
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Incomplete message received %p (device: %d) %s", this, m_dwPK_Device, m_sName.c_str() );
				}
			}
			else
				ReceivedString( sMessage );
		}
	}
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Requesthandler %p (device: %d) Closing request handler connection",
		this, m_dwPK_Device );

//	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Closing event handler connection %d (%d,%s), Terminate: %d %s\n",
//		m_dwPK_Device, (int) m_bUnexpected, sMessage.c_str(), (int) m_bTerminate, m_sName.c_str() );

	m_bRunning = false;
	if ( m_bUnexpected )
	{
		OnUnexpectedDisconnect();
	}

	return;
}

