/*
	ClientSocket

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU Lesser General Public License for more details.
*/


#include "PlutoUtils/CommonIncludes.h"

#ifndef WIN32
#include <fcntl.h>
#endif

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "ClientSocket.h"

#define DCE_DEFAULT_PORT 3450

using namespace DCE;

#ifdef WINCE
	#include "wince.h"
#endif


ClientSocket::ClientSocket( int iDeviceID, string sIPAddress, string sName, bool bIsSSL ) : Socket( sName, sIPAddress )
{
	m_dwPK_Device = iDeviceID;
	m_eLastError = cs_err_None;
	m_bNeedReload = false;
	m_dwMaxRetries = MAX_RETRIES;
	m_bIsSSL_set(bIsSSL);
	/** @todo check comment */
	//	if( g_pDCELogger ) // This won't be created yet if this is the server logger socket
	//		g_pDCELogger->Write(LV_SOCKET,"Created client socket %p device: %d ip: %s",this,m_DeviceID,m_IPAddress.c_str());
}


ClientSocket::~ClientSocket()
{
	Disconnect();
	if (m_bIsSSL)
	{
		SSL_CTX_free(m_sslctx);
	}
}

bool ClientSocket::Connect( int PK_DeviceTemplate,string sExtraInfo,int iConnectRetries, int nConnectStepsLeft )
{

	if (m_bIsSSL)
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Connecting with SSL.\r" );
		m_sslctx = SSL_CTX_new(SSLv23_client_method()); // TODO: replace with TLS_client_method when using new openssl
		if ( !m_sslctx ) {
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't create SSL context.\r" );
			ERR_print_errors_fp(stderr);
			return false;
		}
		// TODO: configurable paths (for qOrbiter)
		if ( SSL_CTX_use_certificate_file(m_sslctx, "/etc/pluto/certs/dce-client.crt" , SSL_FILETYPE_PEM) <= 0 ) {
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't set SSL public key.\r" );
			return false;
		}
		if ( SSL_CTX_use_PrivateKey_file(m_sslctx, "/etc/pluto/certs/dce-client.key.pem", SSL_FILETYPE_PEM) <= 0 ) {
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't set SSL private key.\r" );
			return false;
		}
		if ( !SSL_CTX_check_private_key(m_sslctx) ) {
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Public and private keys does not match.\r" );
			return false;
		}
/*
		if (SSL_CTX_load_verify_locations(m_sslctx, "/etc/pluto/certs/CA/cacert.pem", NULL) <= 0 ) {
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't set SSL CA cert.\r" );
			return false;
		}
*/
		SSL_CTX_use_certificate_chain_file(m_sslctx, "/etc/pluto/certs/CA/cacert.pem");
		
//		SSL_CTX_set_verify(m_sslctx, SSL_VERIFY_PEER, NULL);
//		SSL_CTX_set_verify_depth(m_sslctx, 1);
		SSL_CTX_set_mode(m_sslctx, SSL_MODE_AUTO_RETRY);
	}	  

	m_bCancelSocketOp = false;

	if( iConnectRetries==-1 )
		iConnectRetries=m_dwMaxRetries;
	bool bSuccess = false;

	// flush the socket buffer
	m_pcCurInsockBuffer = NULL;

	if ( m_Socket != INVALID_SOCKET )
	{
#ifdef DEBUG

			LoggerWrapper::GetInstance()->Write( LV_SOCKET, "ClientSocket::Connect - disconnecting previous socket this: %p device: %d m_Socket: %d", this, m_dwPK_Device, m_Socket );
#endif
		Disconnect();
	}

	m_Socket = socket( AF_INET, SOCK_STREAM, 0 );

	int iOptKeepAlive = 1;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (SOCKOPTTYPE)&iOptKeepAlive, sizeof(iOptKeepAlive))) // error
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "setsockopt failed"); // couldn't set the sock option
		Disconnect();
		return false;
	}

//#ifdef DEBUG
		//commented this because we don't want messagesend to output debug info like this.
		//
		//	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "ClientSocket::Connect - created m_Socket: %d", m_Socket );
//#endif

	/** @todo check comment */
	//int b = 1;
	//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE) &b, sizeof(b));

	if(m_sIPAddress.empty())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Don't know where to connect. The IP address is empty!");
		Disconnect();
		m_eLastError=cs_err_CannotConnect;
		return false;
	}

	if ( m_Socket != INVALID_SOCKET )
	{
		int iRetries = 0, iRet;
		struct sockaddr_in  addrT;
		struct hostent*   phe;

		for ( bSuccess = false;  iRetries < iConnectRetries && !bSuccess;  iRetries++ ) // keep tring until success or MAX_RETRIES is reached
		{
 			memset( &addrT, 0, sizeof( addrT ) );
			addrT.sin_family = AF_INET;
			string sAddress;
			int iPort;
			StringUtils::AddressAndPortFromString( m_sIPAddress, DCE_DEFAULT_PORT, sAddress, iPort );
			if (m_bIsSSL)
				iPort = 12232;
			addrT.sin_port = htons( iPort );

			unsigned long dwAddr = inet_addr( sAddress.c_str() );

			if ( dwAddr == INADDR_NONE )
			{ // server address is a name
				phe = gethostbyname( sAddress.c_str() );
				if ( !phe )
				{
#ifdef _WIN32
					int ec = WSAGetLastError();
#else
					int ec = h_errno;
#endif

						LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "gethostbyname for '%s', failed, Last Error Code %d, Device: %d", sAddress.c_str(), ec, m_dwPK_Device );
					Disconnect();
					break;
				}
				addrT.sin_addr.s_addr = *(long *)phe->h_addr;
			}
			else
			{ // Convert nnn.nnn address to a usable one
				addrT.sin_addr.s_addr = dwAddr;
			}

			iRet = connect( m_Socket, (sockaddr *)&addrT, sizeof(addrT) );
			if ( !SOCKFAIL( iRet ) )
			{
#ifdef _WIN32
                // Determine the local IP address
                socklen_t len = sizeof( struct sockaddr );
                getsockname ( m_Socket, (struct sockaddr*) &addrT, &len );
                char *pAddress = inet_ntoa(addrT.sin_addr);
                m_sMyIPAddress = pAddress;
#else
                // Determine the local IP address
				char pcAddress[512];
				socklen_t len = sizeof( struct sockaddr );
				getsockname ( m_Socket, (struct sockaddr*) &addrT, &len );
                inet_ntop( AF_INET, &( addrT.sin_addr.s_addr), pcAddress, 32 );
				m_sMyIPAddress = pcAddress;
#endif
				if (m_bIsSSL)
				{
					m_pSSL = SSL_new (m_sslctx);
					SSL_set_fd(m_pSSL, m_Socket);
					int err = SSL_connect(m_pSSL);

					if ( err < 1 )
					{
						int err = SSL_get_error(m_pSSL,err);
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "SSL_Connect() failed, Error Code %d (%s))", err);
					}
				}

				bSuccess = true;
			}
			else
			{
#ifdef _WIN32
				int ec = WSAGetLastError();
#else
				int ec = errno;
#endif
				Sleep(1000); // Don't retry too fast and use a lot of cpu
#ifndef WINCE
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Connect() failed, Error Code %d (%s))", ec, strerror(ec));
#endif
			}

		}
		/** @todo check comment */
		//setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
		if ( !bSuccess)
		{

				LoggerWrapper::GetInstance()->Write(LV_WARNING, "ClientSocket::Connect() not successful");
			Disconnect();
		}
	}
	else
	{

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "socket() failed, is the TCP stack initialized?, device: %d", m_dwPK_Device);
	}
	if ( bSuccess )
	{
#ifdef WIN32
		unsigned long on = 1;
		::ioctlsocket( m_Socket, FIONBIO, &on );
#else
		long dwFlags = fcntl( m_Socket, F_GETFL );
		dwFlags |= O_NONBLOCK;
		fcntl( m_Socket, F_SETFL, dwFlags );

		// set close-on-exec flag
		dwFlags = fcntl(m_Socket, F_GETFD);
		dwFlags |= FD_CLOEXEC;
		fcntl(m_Socket, F_SETFD, dwFlags);
#endif
		bSuccess = OnConnect( PK_DeviceTemplate, sExtraInfo );

		if(!bSuccess && m_eLastError == cs_err_CannotConnect && nConnectStepsLeft > 0)
		{
			//handshake failed; we'll retry
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Handshake failed. Retrying in 5 seconds");

			Disconnect();
			m_eLastError = cs_err_None;

			Sleep(5000);

			return Connect(PK_DeviceTemplate, sExtraInfo, iConnectRetries, --nConnectStepsLeft);
		}
	}
	else
		m_eLastError=cs_err_CannotConnect;
	return bSuccess;
}

bool ClientSocket::OnConnect( int PK_DeviceTemplate,string sExtraInfo )
{
	SendString( "EVENT " + StringUtils::itos( m_dwPK_Device ) + (PK_DeviceTemplate ? " T=" + StringUtils::itos(PK_DeviceTemplate) : string("")) + (sExtraInfo.length() ? " " + sExtraInfo : "") );
	string sResponse;
	if ( !ReceiveString( sResponse ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Lost connection device: %d", m_dwPK_Device );
		m_eLastError=cs_err_CannotConnect;
		return false;
	}

	string::size_type pos = sResponse.find("IP=");
	if( pos!=string::npos )
	{
		string::size_type space = sResponse.find(' ',pos+1);
		m_sMyIPAddress = space==string::npos ? sResponse.substr(pos+3) : sResponse.substr(pos+3,space-pos-3);
	}
	pos = sResponse.find("MAC=");
	if( pos!=string::npos )
	{
		string::size_type space = sResponse.find(' ',pos+1);
		m_sMacAddress = space==string::npos ? sResponse.substr(pos+4) : sResponse.substr(pos+3,space-pos-4);
	}

	if ( sResponse.length()<2 || sResponse.substr(0,2) != "OK" )
	{
		if( sResponse.substr(0,11)=="NEED RELOAD" )
			m_eLastError=cs_err_NeedReload;
		else if( sResponse.substr(0,24)=="NOT IN THIS INSTALLATION" || sResponse.substr(0,10)=="BAD DEVICE" )
			m_eLastError=cs_err_BadDevice;
		else
			m_eLastError=cs_err_CannotConnect;

			LoggerWrapper::GetInstance()->Write( m_eLastError==cs_err_NeedReload ? LV_WARNING : LV_CRITICAL, "Connection for client socket reported %s, device %d last error %d",
				sResponse.c_str(), m_dwPK_Device,(int) m_eLastError );

		if( m_eLastError!=cs_err_NeedReload )
		{
			SendString( "CORRUPT SOCKET" );
			Sleep( 500 );
			Disconnect();
		}
		return false;
	}

	int PK_Device_New = 0;
	if( sResponse.length()>3 )
		PK_Device_New = atoi(sResponse.substr(3).c_str());

	if( PK_Device_New )
	{
		if( m_dwPK_Device && m_dwPK_Device != PK_Device_New )
		{

				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Server reported we are dev %d but we are device %d", PK_Device_New, m_dwPK_Device );
		}
		else
			m_dwPK_Device=PK_Device_New;
	}

	m_eLastError=cs_err_None;
	return true;
}

void ClientSocket::Disconnect()
{

	if ( m_Socket != INVALID_SOCKET )
	{
            LoggerWrapper::GetInstance()->Write( LV_WARNING, "void ClientSocket::Disconnect() on this socket: %p (m_Socket: %d)", this, m_Socket);

		// this will usually force it out from the select.
		Close();
	}
}

