/*
SocketListener

Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU Lesser General Public License for more details.
*/

#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <fcntl.h>

#include "DCE/Logger.h"
#include "ServerSocket.h"
#include "SocketListener.h"

#include <algorithm>

#ifdef __APPLE_CC__
	#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
#endif

using namespace std;

using namespace DCE;

void *BeginListenerThread( void *pSockListener )
{
	SocketListener *pListener = (SocketListener *)pSockListener;
	pListener->Run(); // start the listener
	return NULL;
}

void *BeginSSLListenerThread( void *pSockListener )
{
	SocketListener *pListener = (SocketListener *)pSockListener;
	pListener->RunSSL(); // start the listener
	return NULL;
}

SocketListener::SocketListener( string sName )
#ifdef PTHREAD2
	: m_ListenerThreadID(pthread_t{ NULL, 0 }), m_SSLListenerThreadID(pthread_t{ NULL, 0 }),
#else
	: m_ListenerThreadID((pthread_t)NULL), m_SSLListenerThreadID((pthread_t)NULL),
#endif
	  m_iListenPort(0),
	  m_Socket( INVALID_SOCKET ),
	  m_SSLSocket( INVALID_SOCKET ),
	  m_bAllowIncommingConnections(true),
	  m_sName(sName),
	  m_ListenerMutex( "listener " + sName ),
	  m_bTerminate(false),
	  m_bRunning(false),
	  m_bClosed(false),
	  m_bFailedToBind(false),
	  m_bSendOnlySocket(false)
{
	SSL_library_init();
	SSL_load_error_strings();

	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_ListenerMutex.Init( &m_MutexAttr );
}

SocketListener::~SocketListener()
{
	m_bTerminate = true;
	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "~SocketListener %d", m_Socket );
	if ( m_Socket != INVALID_SOCKET )
	{
		LoggerWrapper::GetInstance()->Write( LV_SOCKET, "closing listener m_Socket: %d", m_Socket );
		closesocket( m_Socket ); // closing the socket
		m_Socket = INVALID_SOCKET; // now it is invalid
	}

#ifdef PTHREAD2
	if (m_ListenerThreadID.p )
#else
	if (m_ListenerThreadID)
#endif
		pthread_join( m_ListenerThreadID, 0 ); // wait for it to finish

	// The same for the SSL Socket
	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "~SocketListener SSL %d", m_SSLSocket );
	if ( m_SSLSocket != INVALID_SOCKET )
	{
		LoggerWrapper::GetInstance()->Write( LV_SOCKET, "closing listener m_SSLSocket: %d", m_SSLSocket );
		closesocket( m_SSLSocket ); // closing the socket
		m_SSLSocket = INVALID_SOCKET; // now it is invalid
	}

#ifdef PTHREAD2
	if (m_SSLListenerThreadID.p )
#else
	if (m_SSLListenerThreadID)
#endif
		pthread_join( m_SSLListenerThreadID, 0 ); // wait for it to finish

	SSL_CTX_free(m_sslctx);
	EVP_cleanup();

	//drop all sockets
	DropAllSockets();

	pthread_mutexattr_destroy(&m_MutexAttr);
	// killing the mutex
	pthread_mutex_destroy( &m_ListenerMutex.mutex );
}

void SocketListener::StartListening( int iPortNumber )
{
	m_bClosed = false;
	m_bTerminate = false;
	m_bRunning = false; // So we know when we started
	m_iListenPort = iPortNumber;
	pthread_create( &m_ListenerThreadID, NULL, BeginListenerThread, (void *)this );
	if (m_bSSL)
		pthread_create( &m_SSLListenerThreadID, NULL, BeginSSLListenerThread, (void *)this );
}

void SocketListener::RunSSL()
{
	struct sockaddr_in6  addrT;
	int iSSLPort = m_iListenPort + 1;
	m_sslctx = SSL_CTX_new(SSLv23_server_method());
	if ( !m_sslctx ) {
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't create SSL context.\r" );
		m_bRunning = false;
		return;
	}
	if ( SSL_CTX_use_certificate_file(m_sslctx, "/etc/pluto/certs/CA/certs/dce-server.crt" , SSL_FILETYPE_PEM) < 0 ) {
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't set SSL public key.\r" );
		m_bRunning = false;
		return;
	}
	if ( SSL_CTX_use_PrivateKey_file(m_sslctx, "/etc/pluto/certs/dce.key.pem", SSL_FILETYPE_PEM) < 0 ) {
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't set SSL private key.\r" );
		m_bRunning = false;
		return;
	}
	if ( !SSL_CTX_check_private_key(m_sslctx) ) {
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Public and private keys does not match.\r" );
		m_bRunning = false;
		return;
	}
	if (SSL_CTX_load_verify_locations(m_sslctx, "/etc/pluto/certs/CA/cacert.pem", NULL) < 0 ) {
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't set SSL CA cert.\r" );
		m_bRunning = false;
		return;
	}
//	SSL_CTX_use_certificate_chain_file(m_sslctx, "/etc/pluto/certs/CA/cacert.pem");

	SSL_CTX_set_verify(m_sslctx, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(m_sslctx, 1);
	SSL_CTX_set_mode(m_sslctx, SSL_MODE_AUTO_RETRY);

	// Open IPv6 socket
	m_SSLSocket = socket( AF_INET6, SOCK_STREAM, 0 );
	/** @todo check comment */
	// setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE)&b, sizeof(b));
	if ( m_SSLSocket == INVALID_SOCKET ) // error
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't create SSL listener socket.\r" );
		m_bRunning = false;
		return;
	}

	// Make socket also listen to IPv4 requests
	int iOptIPv6only = 0;
	if (setsockopt(m_SSLSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&iOptIPv6only, sizeof(iOptIPv6only)) == -1)
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "SOCKET: Cannot set IPV6_V6ONLY socket option.\r" );
		m_SSLSocket = INVALID_SOCKET;
		m_bRunning=false;
		return;
	} 
	else 
	{
		LoggerWrapper::GetInstance()->Write( LV_SOCKET, "SOCKET: IPV6_V6ONLY option set.\r" );
	}
	int iOptReuse = 1;
	int iOptKeepAlive = 1;
	if (setsockopt(m_SSLSocket, SOL_SOCKET, SO_REUSEADDR, (SOCKOPTTYPE)&iOptReuse, sizeof(iOptReuse))
		|| setsockopt(m_SSLSocket, SOL_SOCKET, SO_KEEPALIVE, (SOCKOPTTYPE)&iOptKeepAlive, sizeof(iOptKeepAlive))) // error
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "setsockopt failed" ); // couldn't set the sock option
		closesocket(m_SSLSocket);
		m_SSLSocket = INVALID_SOCKET;
		m_bRunning=false;
		return;
	}

	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "TCPIP: Listening on %d (SSL).", iSSLPort ); // from now trying to start listening
	
	memset( &addrT, 0, sizeof(addrT) ); // clearing it
	addrT.sin6_family = AF_INET6;
	addrT.sin6_port = htons( iSSLPort );
	addrT.sin6_addr = in6addr_any;

	if ( SOCKFAIL( bind( m_SSLSocket, (struct sockaddr*)&addrT, sizeof(addrT) ) ) ) // test if we can bind the socket with the desired properties @todo ask
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed to bind to port (SSL).\r" );
		m_bFailedToBind=true;
		m_bRunning=false;
		return;
	}
	else if ( SOCKFAIL( listen( m_SSLSocket, SOMAXCONN ) ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed to listen to port (SSL).\r" );
	}
	else // no errors
	{
#ifdef WIN32
		unsigned long on = 1;
		::ioctlsocket( m_SSLSocket, FIONBIO, &on );
#else
		long dwFlags = fcntl( m_SSLSocket, F_GETFL );
		dwFlags |= O_NONBLOCK;
		fcntl( m_SSLSocket, F_SETFL, dwFlags );

		// set close-on-exec flag
		dwFlags = fcntl(m_SSLSocket, F_GETFD);
		dwFlags |= FD_CLOEXEC;
		fcntl(m_SSLSocket, F_SETFD, dwFlags);
#endif
		m_bRunning = true; // So we know when we started
		while( !m_bTerminate && m_bAllowIncommingConnections) // while the listener dosen't terminate
		{
			fd_set rfds;
			struct timeval tv;

			int iRet = 0;

			while( !m_bTerminate && iRet != 1 )
			{
				FD_ZERO( &rfds );
				FD_SET( m_SSLSocket, &rfds );
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				fflush( stdout );
				iRet = select( (int) (m_SSLSocket+1), &rfds, NULL, NULL, &tv );
			}
			if( m_bTerminate )
				break;

			struct sockaddr_in6 addr;
			socklen_t len = sizeof( addr );
			SOCKET newsock = accept( m_SSLSocket, (sockaddr *)&addr, &len ); // creating a new socket

			if(!m_bAllowIncommingConnections)
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got an incoming connection, but the server is closing." );
				send( newsock, "CLOSED\n", 6, 0 );
				closesocket( newsock );
				break;
			}

			if ( newsock != INVALID_SOCKET )
			{
				if ( m_bClosed )
				{
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got an incoming connection, but the server state is closed." );
					send( newsock, "CLOSED\n", 6, 0 );
					closesocket( newsock );
				}
				else
				{
					char str[INET6_ADDRSTRLEN];
					inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof(str));
					LoggerWrapper::GetInstance()->Write( LV_SOCKET, "TCPIP: Accepting incoming connection on socket %d (SSL), port %d, from IP %s.", newsock, iSSLPort, str);
#ifdef WIN32
					unsigned long on = 1;
					::ioctlsocket( newsock, FIONBIO, &on );
#else
					long dwFlags = fcntl( newsock, F_GETFL );
					dwFlags |= O_NONBLOCK;
					fcntl( newsock, F_SETFL, dwFlags );

					// set close-on-exec flag
					dwFlags = fcntl(newsock, F_GETFD);
					dwFlags |= FD_CLOEXEC;
					fcntl(newsock, F_SETFD, dwFlags);
#endif
					/** @todo check comment */
					// setsockopt(newsock, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE) &b, sizeof(b));
					inet_ntop(AF_INET6, &(addr.sin6_addr), str, INET6_ADDRSTRLEN);

					SSL *ssl;
					ssl = SSL_new(m_sslctx);
					SSL_set_fd(ssl, newsock);
					int sslret = SSL_accept(ssl);
					int sslerr = -1;
					while (sslret <= 0 && (sslerr == -1 || sslerr == SSL_ERROR_WANT_READ || sslerr == SSL_ERROR_WANT_WRITE ))
					{
						sslerr = SSL_get_error(ssl, sslret);
						if ( sslerr == SSL_ERROR_WANT_READ || sslerr == SSL_ERROR_WANT_WRITE ) {
							LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "(SSL accept failed: WANT READ/WRITE) with %d, error = %d.\r", sslret, sslerr );
							sslret = SSL_accept(ssl);
							if (sslret <= 0)
								sslerr = SSL_get_error(ssl, sslret);
						} else {
							LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Invalid connection socket (SSL accept failed) with %d, error = %d.\r", sslret, sslerr );
							char error[256];
							ERR_error_string(sslerr, error);
							LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "%s", error );
							ERR_print_errors_fp(stdout);
						}
					}
					if ( sslret > 0 ) {
						// TODO: SSL_verify
						CreateSocket( newsock, "Incoming_Conn Socket " + StringUtils::itos(int(newsock)) + " " + str, str, "", ssl, true);
					}
				}
			}
			else
			{
				if ( !m_bTerminate )
				{
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Invalid connection socket (accept failed).\r" );
					Sleep( 1000 );
				}
			}
			Sleep( 0 );
		}
	}

	closesocket( m_SSLSocket );

	m_SSLSocket = INVALID_SOCKET;
	m_bRunning = false;

	return;
}

void SocketListener::Run()
{
	struct sockaddr_in6  addrT;

	// Open IPv6 socket
	m_Socket = socket( AF_INET6, SOCK_STREAM, 0 );
	/** @todo check comment */
	// setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE)&b, sizeof(b));
	if ( m_Socket == INVALID_SOCKET ) // error
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Couldn't create listener socket.\r" );
		m_bRunning = false;
		return;
	}

	// Make socket also listen to IPv4 requests
	int iOptIPv6only = 0;
	if (setsockopt(m_Socket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&iOptIPv6only, sizeof(iOptIPv6only)) == -1)
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "SOCKET: Cannot set IPV6_V6ONLY socket option.\r" );
		m_Socket = INVALID_SOCKET;
		m_bRunning=false;
		return;
	} 
	else 
	{
		LoggerWrapper::GetInstance()->Write( LV_SOCKET, "SOCKET: IPV6_V6ONLY option set.\r" );
	}
	int iOptReuse = 1;
	int iOptKeepAlive = 1;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (SOCKOPTTYPE)&iOptReuse, sizeof(iOptReuse))
		|| setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (SOCKOPTTYPE)&iOptKeepAlive, sizeof(iOptKeepAlive))) // error
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "setsockopt failed" ); // couldn't set the sock option
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		m_bRunning=false;
		return;
	}

	LoggerWrapper::GetInstance()->Write( LV_SOCKET, "TCPIP: Listening on %d.", m_iListenPort ); // from now trying to start listening
	
	memset( &addrT, 0, sizeof(addrT) ); // clearing it
	addrT.sin6_family = AF_INET6;
	addrT.sin6_port = htons( m_iListenPort );
	addrT.sin6_addr = in6addr_any;

	if ( SOCKFAIL( bind( m_Socket, (struct sockaddr*)&addrT, sizeof(addrT) ) ) ) // test if we can bind the socket with the desired properties @todo ask
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed to bind to port.\r" );
		m_bFailedToBind=true;
		m_bRunning=false;
		return;
	}
	else if ( SOCKFAIL( listen( m_Socket, SOMAXCONN ) ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Failed to listen to port.\r" );
	}
	else // no errors
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
		m_bRunning = true; // So we know when we started
		while( !m_bTerminate && m_bAllowIncommingConnections) // while the listener dosen't terminate
		{
			fd_set rfds;
			struct timeval tv;

			int iRet = 0;

			while( !m_bTerminate && iRet != 1 )
			{
				FD_ZERO( &rfds );
				FD_SET( m_Socket, &rfds );
				tv.tv_sec = 1;
				tv.tv_usec = 0;
				fflush( stdout );
				iRet = select( (int) (m_Socket+1), &rfds, NULL, NULL, &tv );
			}
			if( m_bTerminate )
				break;

			struct sockaddr_in6 addr;
			socklen_t len = sizeof( addr );
			SOCKET newsock = accept( m_Socket, (sockaddr *)&addr, &len ); // creating a new socket

			if(!m_bAllowIncommingConnections)
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got an incoming connection, but the server is closing." );
				send( newsock, "CLOSED\n", 6, 0 );
				closesocket( newsock );
				break;
			}

			if ( newsock != INVALID_SOCKET )
			{
				if ( m_bClosed )
				{
					LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got an incoming connection, but the server state is closed." );
					send( newsock, "CLOSED\n", 6, 0 );
					closesocket( newsock );
				}
				else
				{
					char str[INET6_ADDRSTRLEN];
					inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof(str));
					LoggerWrapper::GetInstance()->Write( LV_SOCKET, "TCPIP: Accepting incoming connection on socket %d, port %d, from IP %s.", newsock, m_iListenPort, str);
#ifdef WIN32
					unsigned long on = 1;
					::ioctlsocket( newsock, FIONBIO, &on );
#else
					long dwFlags = fcntl( newsock, F_GETFL );
					dwFlags |= O_NONBLOCK;
					fcntl( newsock, F_SETFL, dwFlags );

					// set close-on-exec flag
					dwFlags = fcntl(newsock, F_GETFD);
					dwFlags |= FD_CLOEXEC;
					fcntl(newsock, F_SETFD, dwFlags);
#endif
					/** @todo check comment */
					// setsockopt(newsock, IPPROTO_TCP, TCP_NODELAY, (SOCKOPTTYPE) &b, sizeof(b));
					inet_ntop(AF_INET6, &(addr.sin6_addr), str, INET6_ADDRSTRLEN);
					CreateSocket( newsock, "Incoming_Conn Socket " + StringUtils::itos(int(newsock)) + " " + str, str);
				}
			}
			else
			{
				if ( !m_bTerminate )
				{
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Invalid connection socket (accept failed).\r" );
					Sleep( 1000 );
				}
			}
			Sleep( 0 );
		}
	}

	closesocket( m_Socket );

	m_Socket = INVALID_SOCKET;
	m_bRunning = false;

	return;
}

Socket *SocketListener::CreateSocket( SOCKET newsock, string sName, string sIPAddress, string sMacAddress, SSL *ssl, bool isSSL )
{
	PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );
	ServerSocket *pSocket = new ServerSocket( this, newsock, sName, sIPAddress, sMacAddress, ssl, isSSL );
	pSocket->m_bSendOnlySocket = m_bSendOnlySocket;
	m_vectorServerSocket.push_back(pSocket);
	pSocket->Run();
	return pSocket;
}

void SocketListener::RemoveAndDeleteSocket( ServerSocket *pServerSocket, bool bDontDelete )
{
	pServerSocket->Close();

	PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );
	if(pServerSocket->m_iReferencesOutstanding_get() > 1)
	{
		// Something besides us is still referencing this pointer
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Removing socket %d, ourstanding refercences: %d",
			pServerSocket->m_dwPK_Device, pServerSocket->m_iReferencesOutstanding_get());
		Sleep(100);
	}

	LoggerWrapper::GetInstance()->Write(LV_SOCKET, "Removing socket %p from socket listener", pServerSocket);

	if(pServerSocket->m_dwPK_Device)
	{
		ServerSocketMap::iterator i = m_mapServerSocket.find( pServerSocket->m_dwPK_Device );
		if (i != m_mapServerSocket.end() && (*i).second == pServerSocket)
		{
			LoggerWrapper::GetInstance()->Write( LV_REGISTRATION, "Command handler (%p) for \x1b[34;1m%d\x1b[0m closed.", pServerSocket, pServerSocket->m_dwPK_Device );
			OnDisconnected( pServerSocket->m_dwPK_Device );
			m_mapServerSocket.erase(i);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write( LV_REGISTRATION, "Stale Command handler (%p) for \x1b[34;1m%d\x1b[0m closed.", pServerSocket, pServerSocket->m_dwPK_Device );
		}
	}

	ServerSocketVector::iterator it_serversocket = std::find(m_vectorServerSocket.begin(), m_vectorServerSocket.end(), pServerSocket);
	if(it_serversocket != m_vectorServerSocket.end())
	{
		m_vectorServerSocket.erase(it_serversocket);

		LoggerWrapper::GetInstance()->Write( LV_REGISTRATION, "Map server socket size %d, vector server socket size %d",
			m_mapServerSocket.size(), m_vectorServerSocket.size());

		// Will be true if the socket's destructor is calling this
		if(!bDontDelete)
		{
			// Otherwise the socket's destructor will call this
			pServerSocket->m_bAlreadyRemoved = true;

			//deallocate the memory for it
			delete pServerSocket;
		}
	}
}

void SocketListener::RegisterEventHandler( ServerSocket *Socket, int iDeviceID )
{
	Socket->m_eSocketType=Socket::st_ServerEvent;
	RegisteredEventHandler( Socket, iDeviceID );
}

void SocketListener::RegisterCommandHandler( ServerSocket *Socket, int iDeviceID )
{
	PLUTO_SAFETY_LOCK( ll, m_ListenerMutex );
	Socket->m_sName += " dev " + StringUtils::itos( iDeviceID );
	Socket->m_ConnectionMutex.m_sName += " dev " + StringUtils::itos( iDeviceID );
	Socket->m_SocketMutex.m_sName += " dev " + StringUtils::itos( iDeviceID );
	ServerSocket *pSocket_Old;
	GET_SERVER_SOCKET(gs, pSocket_Old, iDeviceID ); // search if device is allready in the map
	if ( pSocket_Old )
	{
		LoggerWrapper::GetInstance()->Write( LV_REGISTRATION, "!!! Replacing command handler on device ID \x1b[34;1m%d!\x1b[0m", iDeviceID );
		if( pSocket_Old )
		{
			pSocket_Old->SendString("REPLACE " + Socket->m_sIPAddress);
			Sleep(10);  // Give it just enough time to be sure the message gets through
		}

		ll.Release();
		gs.DeletingSocket();
		RemoveAndDeleteSocket( pSocket_Old );
		ll.Relock();
	}
	m_mapServerSocket[iDeviceID] = Socket; // assigning it the new specified socket
	Socket->SetReceiveTimeout( IsPlugin(iDeviceID) ? SOCKET_TIMEOUT_PLUGIN : SOCKET_TIMEOUT );
	ll.Release();

	LoggerWrapper::GetInstance()->Write( LV_REGISTRATION, "Device ID \x1b[34;1m%d's command handler\x1b[0m registered on socket: %d with counter %d %p",
		iDeviceID,
		Socket->m_Socket, Socket->m_iSocketCounter,
		Socket );

	Socket->m_eSocketType=Socket::st_ServerCommand;
	RegisteredCommandHandler( Socket, iDeviceID );
}


string SocketListener::GetIPAddress()
{
	char acBuf[256];
	gethostname( acBuf, sizeof(acBuf) );

	struct hostent * he;
#ifdef WIN32
	he = gethostbyname(acBuf);
#else
	he = gethostbyname2 ( acBuf, AF_INET6 );
#endif

	char* addr = he->h_addr_list[0];
	if (addr) // got an address
	{
		struct in_addr  inAddr;
		memmove (&inAddr, addr, 4);
		return string(inet_ntoa(inAddr));
	}
	return "127.0.0.1"; // nothing, using loopback
}

string SocketListener::GetIPAddress( int iDeviceID )
{
	ServerSocket *pServerSocket;
	GET_SERVER_SOCKET(ss,pServerSocket,iDeviceID);
	if( pServerSocket == NULL ) return "";

	sockaddr_in6 inAddr;
	socklen_t sl = sizeof(inAddr);
	if( getpeername( pServerSocket->m_Socket, (sockaddr*)&inAddr, &sl ) ) return "";
	char str[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &(inAddr.sin6_addr), str, INET6_ADDRSTRLEN);
	return  string( str );

}

bool SocketListener::SendMessage( int iDeviceID, Message *pMessage )
{
	ServerSocket *pServerSocket;
	GET_SERVER_SOCKET(gs, pServerSocket, iDeviceID );
	if( !pServerSocket )
		return false; // couldn't find the device id
	pServerSocket->SendMessage( pMessage ); // all is ok, sending the message
	return true;
}

bool SocketListener::SendString( int iDeviceID, string sLine )
{
	ServerSocket *pServerSocket;
	GET_SERVER_SOCKET(gs, pServerSocket, iDeviceID );

	if( !pServerSocket )
		return false; // couldn't find the device id
	pServerSocket->SendString( sLine ); // sending the string
	return true;
}

bool SocketListener::SendData( int iDeviceID, int iLength, const char *pcData )
{
	ServerSocket *pServerSocket;
	GET_SERVER_SOCKET(gs, pServerSocket, iDeviceID );

	if( !pServerSocket )
		return false; // couldn't find the device id
	pServerSocket->SendData( iLength, pcData ); // sending the data
	return true;
}

void SocketListener::DropAllSockets()
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Dropping all sockets...");

	time_t tStart = time(NULL);

	PLUTO_SAFETY_LOCK( lm, m_ListenerMutex );
	while(0 != m_mapServerSocket.size())
	{
		if(time(NULL) - tStart > 10)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Big problem: can't wait for server sockets anymore!");
			break;
		}

		ServerSocket *pServerSocket = m_mapServerSocket.begin()->second;

		if(!pServerSocket->IsSelfDestroying())
			RemoveAndDeleteSocket(pServerSocket);

		lm.Release();
		Sleep(10);
		lm.Relock();
	}

	while(0 != m_vectorServerSocket.size())
	{
		if(time(NULL) - tStart > 10)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Big problem: can't wait for server sockets anymore!");
			break;
		}

		ServerSocket *pServerSocket = *m_vectorServerSocket.begin();

		if(!pServerSocket->IsSelfDestroying())
			RemoveAndDeleteSocket(pServerSocket);

		lm.Release();
		Sleep(10);
		lm.Relock();
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Done dropping sockets!");
}
