/*
	Event_Impl

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
#include "Event_Impl.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "pluto_main/Define_CommandParameter.h"

#define	INITIAL_CONNECT_RETRIES		100

using namespace DCE;

Event_Impl::Event_Impl( long dwDevice, long dwDeviceTemplate, string sServerAddress, bool bConnectEventHandler, int nSocketTimeout, int nConnectRetries, bool bIsSSL )
{
	m_dwPK_Device = dwDevice;
	m_pClientSocket = new ClientSocket( dwDevice, sServerAddress, string( "Event Dev #" ) + StringUtils::itos( dwDevice ), bIsSSL );
	if( bConnectEventHandler )
	{
		m_pClientSocket->Connect(dwDeviceTemplate,"Event #" + StringUtils::itos(dwDevice),nConnectRetries==-1 ? INITIAL_CONNECT_RETRIES : nConnectRetries);

		if(-1 != nSocketTimeout)
			m_pClientSocket->SetReceiveTimeout(nSocketTimeout);

		if( m_pClientSocket->m_dwPK_Device )
			m_dwPK_Device = m_pClientSocket->m_dwPK_Device;
	}

	/** @todo check comment */
	/*
	#ifdef DEBUG
		g_pDCELogger->Write(LV_SOCKET,"DeviceEvent constructor %p device ID %d server IP %s socket %p %d",
			this,m_DeviceID,ServerAddress.c_str(),m_pDCEClientSocket,m_pDCEClientSocket->m_Socket);
	#endif
	*/
}

Event_Impl::Event_Impl( ClientSocket *pClientSocket, long dwDevice)
{
	m_pClientSocket = pClientSocket;
	m_dwPK_Device = dwDevice;
}

Event_Impl::~Event_Impl()
{
	if ( m_dwPK_Device == m_pClientSocket->m_dwPK_Device ) // to make sure we're not clearing a master's device stuff (if this is a sub-device's event)
		delete m_pClientSocket;
}

bool Event_Impl::SendMessage( Message *pMessage )
{
	return m_pClientSocket->SendMessage( pMessage );
}

bool Event_Impl::SendMessage( Message *pMessage, string &sResponse )
{
	pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
	return m_pClientSocket->SendMessageWithConfirmation(pMessage, sResponse);
	//if( !m_pClientSocket->SendMessage( pMessage ) )
	//	return false;
	//return m_pClientSocket->ReceiveString( sResponse );
}

Message *Event_Impl::SendReceiveMessage( Message *pMessage )
{
	pMessage->m_eExpectedResponse = ER_ReplyMessage;
	return m_pClientSocket->SendReceiveMessage( pMessage );
}

string Event_Impl::SendReceiveString( string s, int nTimeout/* = -1*/)
{
	return m_pClientSocket->SendReceiveString(s, nTimeout);
}

char *Event_Impl::GetConfig( /*unsigned long*/int &dwSize )
{
	// Create a loop so we can keep waiting
	while(true)
	{
		dwSize=0;
		m_pClientSocket->SendString( "CONFIG" );
		string sResponse;
		m_pClientSocket->ReceiveString( sResponse ); // getting the router's response
		if( sResponse == "WAIT" ) // router busy
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "DCE Router asked us to wait--it's busy at the moment" );
			Sleep( 1000 );
			continue;
		}
		if ( sResponse.substr(0,6) == "CONFIG" ) // got it!
		{
			dwSize = atoi( &sResponse.c_str()[6] ); // reading it
			char *pcBuff = new char[dwSize+1];
			m_pClientSocket->ReceiveData( dwSize, pcBuff );
			return pcBuff;
		}

		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "DCE Router cannot give us config: %s", sResponse.c_str() ); // logging it
		return NULL;
	}
	return NULL;
}

char *Event_Impl::GetDeviceList( /*unsigned long*/ int &dwSize )
{
	dwSize=0;
	m_pClientSocket->SendString( "DEVICELIST" );
	string sResponse;
	m_pClientSocket->ReceiveString( sResponse );
	if ( sResponse.substr(0,10) == "DEVICELIST" ) // got it!
	{
		dwSize = atoi( &sResponse.c_str()[10] );
		char *pcBuff = new char[dwSize+1];
		m_pClientSocket->ReceiveData( dwSize, pcBuff );
		return pcBuff;
	}
	return NULL;
}

string Event_Impl::GetCurrentDeviceData( int PK_Device, int PK_DeviceData )
{
	Message *pMessage = new Message(m_dwPK_Device,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_DATAPARM_REQUEST,PK_DeviceData,0);
	Message *pResponse = SendReceiveMessage(pMessage);

	if( pResponse )
		return pResponse->m_mapParameters[PK_DeviceData];

	return "";
}

string Event_Impl::GetDeviceDataFromDatabase( int PK_Device, int PK_DeviceData )
{
	Message *pMessage = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_DATAPARM_REQUEST,PK_DeviceData,1,
		COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(PK_Device).c_str());
	Message *pResponse = SendReceiveMessage(pMessage);

	if( pResponse )
	{
		string sValue = pResponse->m_mapParameters[PK_DeviceData];
		delete pResponse;
		return sValue;
	}

	return "";
}
