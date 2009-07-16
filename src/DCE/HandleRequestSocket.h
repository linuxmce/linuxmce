/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file HandleRequestSocket.h
 Header file for the HandleRequestSocket class.
 * @todo notcommented
 */


#ifndef HANDLEREQUESTSOCKET_H
#define HANDLEREQUESTSOCKET_H

#include "ClientSocket.h"

namespace DCE
{
	typedef enum { rmr_NotProcessed=0, rmr_Processed, rmr_Buffered } ReceivedMessageResult;

	/** @class HandleRequestSocket
	 * @brief socket that handles requests
	 */
	class HandleRequestSocket : public ClientSocket
	{

	public:

		pthread_t m_RequestHandlerThread; /**< he thread that runs the main loop */

		bool m_bRunning; /**< the socket is running */
		bool m_bTerminate; /**< specifies that the socket terminated (the main loop exits) */
		bool m_bUnexpected; /**< set if the socket got an unexpected error */

		/**
		 * @brief calls the base class constructor and assignes 0 to m_RequestHandlerThread
		 */
		HandleRequestSocket( int iDeviceID, string sIPAddress, string sName );

		/**
		 * @brief calls DisconnectAndWait
		 * @see DisconnectAndWait
		 */
		virtual ~HandleRequestSocket();

		/**
		 * @brief sets the m_bTerminate flag, writes an entry to the logger and calls the base class method
		 */
		virtual void Disconnect();

		/**
		 * @brief waits for the main loop thread to die
		 */
		virtual void DisconnectAndWait();

		/**
		 * @brief creates the main loop thread after it receives the OK
		 */
		virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );

		/**
		 * @brief virtual
		 * @see the definition from the base class
		 */
		virtual void ReceivedString( string sLine, int nTimeout = -1 )=0;

		/**
		 * @brief virtual
		 * @see the definition from the base class
		 */
		virtual ReceivedMessageResult ReceivedMessage( Message *pMessage )=0;

		/**
		 * @brief runs a loop that handles outgoing and incoming messages and strings
		 */
		virtual void RunThread();

		/**
		 * @brief overrideable by the derived classes
		 */
		virtual void OnUnexpectedDisconnect() { };

		/**
		 * @brief overrideable by the derived classes, called when another device connects with the same device ID
		 */
		virtual bool OnReplaceHandler(string /*sIP*/) { return false; };
	};
}
#endif
