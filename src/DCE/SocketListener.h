/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file SocketListener.h
Header file for the SocketListener class.

This listens for server sockets, and contains a m_mapServerSocket with all connections
In order to get a pointer to a server socket you must use the GET_SERVER_SOCKET macro,
 which returns the pointer and automatically increments the
 ServerSocket::m_iReferencesOutstanding.
The macro causes this to decrement automatically when it falls out of scope.
To remove a socket call RemoveAndDeleteSocket.
This will only remove the socket when the number of references is 1.
*/
#ifndef SOCKETLISTENER_H
#define SOCKETLISTENER_H

#include <map>
#include <list>
#include <string>
#include "ServerSocket.h"
#include "DCE/Logger.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace DCE
{
	class ServerSocket;  /**< to be able to use it in declarations; we include it's header in the cpp file */
	class Message;  /**< to be able to use it in declarations; we include it's header in the cpp file */

	typedef ::std::map<int, ServerSocket *> ServerSocketMap; /**< integer map of  */
	typedef ::std::vector<ServerSocket *> ServerSocketVector; /**< vector of  */

	/**
    @class SocketListener
	Opens a socket on a server waiting for incomming connections
	*/
	class SocketListener
	{

	protected:

		pthread_t m_ListenerThreadID; /**< the thread on witch the socket runs */
		int m_iListenPort; /**< the port on witch to listen */
		SOCKET m_Socket; /**< the socket that listens for incomming connections */
		bool m_bAllowIncommingConnections;

		bool m_bSSL; /** true if we should also start a SSL listening socket */
		pthread_t m_SSLListenerThreadID; /**< the thread on witch the socket runs */
		SOCKET m_SSLSocket; /**< the socket that listens for incomming connections */
		SSL_CTX *m_sslctx;
	public:

		string m_sName; /**< the socket listener name */
		pluto_pthread_mutex_t m_ListenerMutex; /**< to control access to the shared memory.  Don't use m_mapServerSocket or m_vectorServerSocket without this  */
		pthread_mutexattr_t m_MutexAttr; /**< make it recursive */

		bool m_bTerminate; /**< set to true when the listener terminates (from the destructor)  */
		bool m_bRunning; /**< specifies if the listener is running - set by StartListening */
		bool m_bClosed; /**< specifies if the socket is closed @todo ask how it's used */
		bool m_bFailedToBind; /**< The listener failed to start because it couldn't bind to the socket */

		bool m_bSendOnlySocket; /**< specifies if this socket works in send-only mode (actually this is a hack around blocking reads) */

		//		::std::list<Socket *> m_listClients; /**< a list of sockets created for incoming connections */
		ServerSocketMap m_mapServerSocket; /**< map of server sockets associated with clients (command handlers) */
		ServerSocketVector m_vectorServerSocket; /**< vector of all created server sockets */

		/** Constructor.
        @param sName is the name of the socket.

        Creates a SocketListener and gives it the name specified by the parameter.
        The other member data receive default values.

        @see the implementation for the mb data default values.
		*/
		SocketListener( string sName );

		/** Destructor.
        Cleans up and waits for the thread to die (writes an entry to the log also).
		*/
		virtual ~SocketListener();

		/** Normally called only by the GET_SERVER_SOCKET macro.
        @param dwPK_Device is device.
		*/
		ServerSocket *GetServerSocket(int dwPK_Device)
		{
			PLUTO_SAFETY_LOCK( ll, m_ListenerMutex );
			ServerSocketMap::iterator i = m_mapServerSocket.find( dwPK_Device );
			if ( i == m_mapServerSocket.end() ) return NULL; // couldn't find the device id
			return i->second;
		}

		/** Creates the thread that will listen at the specified port.
      @param iPortNumber is the port.
		@todo ask
		*/
		void StartListening( int iPortNumber );

		/** Runs the listener by creating the master socket, setting it up and telling it to start listening.
		*/
		void Run();

		/** Runs the SSL listener by creating the master socket, setting it up and telling it to start listening.
		*/
		void RunSSL();

		/** Creates a socket listener based on the parameter data.
		@see the class member data
		*/
		virtual Socket *CreateSocket( SOCKET newsock, string sName, string sIPAddress="", string sMacAddress="", SSL *ssl=NULL, bool isSSL=false );

		/** Removes the socket from the listClients mb data and clears any dependencies.
		*/
		virtual void RemoveAndDeleteSocket( ServerSocket *pServerSocket, bool bDontDelete=false );

		/**
		* @brief Called when an event handler is registered
		*/
		void RegisterEventHandler( ServerSocket *Socket, int iDeviceID );

		/**
		* @brief binds the specified socket to the specified device in the mapCommandHandlers mb data (after setting it up a little)
		*/
		void RegisterCommandHandler( ServerSocket *Socket, int iDeviceID );


		/**
		* @brief waiting to be overriden
		*/
		virtual void RegisteredEventHandler( ServerSocket * /*pSocket*/, int /*iDeviceID*/ ) {};

		/**
		* @brief waiting to be overriden
		*/
		virtual void RegisteredCommandHandler( ServerSocket * /*pSocket*/, int /*iDeviceID*/ ) {};

		/**
		* @brief gets the host IP address
		*/
		::std::string GetIPAddress();

		/**
		* @brief gets the host IP address for the specified device
		*/
		::std::string GetIPAddress( int iDeviceID );

		/**
		* @brief sends the string to the specified device using the associated socket
		*/
		bool SendString( int iDeviceID, string sLine );

		/**
		* @brief sends the message to the specified device using the associated socket
		*/
		bool SendMessage( int iDeviceID, Message *pMessage );

		/**
		* @brief sends the data pointed by pcData to the specified device using the associated socket
		*/
		bool SendData( int iDeviceID, int iLength, const char *pcData );

		/**
		* @brief override to get the string from the specified socket
		*/
		virtual bool ReceivedString( Socket *pSocket, string sLine, int nTimeout = - 1 )=0;

		/**
		* @brief override to get the message from the specified socket
		*/
		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage )=0;

		/**
		* @brief override it to set a action for the disconection of the specified device
		* @todo ask
		*/
		virtual void OnDisconnected( int /*iDeviceID*/ ) {}

		/**
		* @brief If a device connects but doesn't know it's ID, this function can be implemented and it will
		* look up the device id based on the device template and the IP Address
		*/
		virtual int GetDeviceID( int /*iPK_DeviceTemplate*/, string /*sMacAddress*/, string /*sIPAddress*/) { return 0; } // Something else needs to implement this }

		/**
		* @brief Returns 0 if the device doesn't exist, 1 if it does but isn't of the device template, 2 if it's a good match, 3 if it's a new device and the router needs to be reloaded
		*/
		virtual int ConfirmDeviceTemplate( int /*iPK_Device*/, int /*iPK_DeviceTemplate*/ ) { return true; }  // Something else needs to implement this }

		/**
		* @brief Returns 0 if the device doesn't exist, 1 if it does but isn't of the device template, 2 if it's a good match, 3 if it's a new device and the router needs to be reloaded
		*/
		virtual bool IsPlugin(int /*iPK_Device*/) { return false; }  //should be overriden in the derived class

		/**
		* @brief Close any open sockets
		*/
		void DropAllSockets();

		/** Called when a ping test fails. */
		virtual void PingFailed( ServerSocket * /*pServerSocket*/, int /*dwPK_Device*/ ) {};

		virtual void RefuseIncomingConnections() { m_bAllowIncommingConnections = false; }
	};

    /** @class get_server_socket
    Another class.
    */
	class get_server_socket
	{
		SocketListener *m_pSocketListener;
		ServerSocket *m_pServerSocket;
	public:
		get_server_socket(SocketListener *pSocketListener,ServerSocket *&pServerSocket,int dwPK_Device)
		{
			m_pSocketListener=pSocketListener;
			m_pServerSocket=m_pSocketListener->GetServerSocket(dwPK_Device);
			if( m_pServerSocket )
				m_pServerSocket->IncrementReferences();
			pServerSocket=m_pServerSocket;
		}

		~get_server_socket()
		{
			if( m_pServerSocket )
				m_pServerSocket->DecrementReferences();
		}

		ServerSocket * operator -> () { return m_pServerSocket; }

		void DeletingSocket() { m_pServerSocket=NULL; }
	};

}

#define GET_SERVER_SOCKET(local_variable,server_socket,DeviceID) get_server_socket local_variable(this,server_socket,DeviceID);

#endif
