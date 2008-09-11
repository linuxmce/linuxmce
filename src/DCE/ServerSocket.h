/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file ServerSocket.h
Header file for the ServerSocket class
* @todo notcommented
*/
#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using namespace std;

namespace DCE
{
	class SocketListener; /**< to be able to use it in declarations, we include it's header in the cpp file */

	/**
    @class ServerSocket
	 implements a server socket (to handle a specific client command).

	See notes about mutex protection in SocketListener.h
	*/
	class ServerSocket : public Socket
	{
	private:
		int m_iReferencesOutstanding; /**< How many references to this are outstanding, we can only delete it when this is 0. */
		bool m_bSelfDestroying; /**< The thread is responsible for destroying the server socket and removing it from the map. */

	public:
		bool m_bAskBeforeReload; /**< If true ask this device if it's ok to reload before processing a reload request. */
		bool m_bImplementsPendingTasks; /**< If true, this device implements the PendingTasks() to report pending tasks it may be doing. */

		long m_dwPK_Device; /**< the device ID */

		pluto_pthread_mutex_t m_ConnectionMutex; /**< for controlling access to the shared memory. */
		pthread_t m_ClientThreadID; /**< the thread running the main loop. */

		SocketListener *m_pListener; /**< the listener that created this command handler. */

		int m_iInstanceID; // The ID from Command_Impl.  See notes for same variable in Command_Impl.h
		bool m_bSendOnlySocket; // This socket doesn't receive anything

		bool m_bThreadRunning,m_bAlreadyRemoved;
		/**
		* @brief constructor, assignes values to the member data and starts the looping thread
		*/
		ServerSocket( SocketListener *pListener, SOCKET Sock, string sName, string sIPAddress="", string sMacAddress="" );

		/**
		* @brief destructor, kills the tread
		*/
		virtual ~ServerSocket();


		void Run();

		// Special behavior for the server
		virtual void PingFailed();

		void IncrementReferences() { m_iReferencesOutstanding++; }
		void DecrementReferences() { m_iReferencesOutstanding--; }
		int m_iReferencesOutstanding_get() { return m_iReferencesOutstanding; }

		bool IsSelfDestroying() { return m_bSelfDestroying; }
		void SelfDestroying() { m_bSelfDestroying = true; }

		bool SafeToReload(string &sReason);

	private:
		/** Runs a loop that handles the client

		@Returns true if the socket is to be deleted after this and false if not.
		*/
		bool ServeClient();

		static void *BeginWapClientThread(void*);

	};
}

#endif
