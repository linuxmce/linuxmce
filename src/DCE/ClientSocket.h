/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/**
@file ClientSocket.h
Header file for the ClientSocket class

@todo finish comments
 */
#ifndef DCECLIENTSOCKET_H
#define DCECLIENTSOCKET_H

#ifdef WINCE
	#include <time.h>
	#ifndef _CLOCK_T_DEFINED
		typedef unsigned long clock_t;
		#define _CLOCK_T_DEFINED
	#endif
#endif

#include "Socket.h"

#define MAX_RETRIES 3	// Maximum retries to establish TCP connection
#define MAX_CONNECT_STEPS 10

namespace DCE
{

	class SocketListener;

    /**
    @class ClientSocket
Defines a ClientSocket.

The derived ClientSocket's responsibility is to make, and keep a reliable connection to the server.

    @todo ask !!!
     */
	class ClientSocket : public Socket
	{

	public:
		bool m_bNeedReload; /**< the device was recently added and the router not yet reloaded, so functionality may be limited */
		long m_dwPK_Device; /**< the device identifier */
		int m_dwMaxRetries; /**< the maximum number of times to retry making a connection */
		clock_t m_clockTimeout; /**< used to set timeouts */
		string m_sMyIPAddress; /**< this client's machine IP address */
		enum {cs_err_None=0,cs_err_CannotConnect,cs_err_NeedReload,cs_err_BadDevice} m_eLastError;

		/** Constructor
        @param iDeviceID is ???
        @param sIPAddress is the IP address.
        @param sName is the socketrs name.
        This assigns values to the member data
		 */
		ClientSocket( int iDeviceID, string sIPAddress, string sName );

		virtual ~ClientSocket();

		/**
		 * @brief it establishes the connection to the server and eventually send the ExtraInfo using OnConnect; also writes to the logger
		 * @see OnConnect
		 */
		virtual bool Connect( int PK_DeviceTemplate=0,string sExtraInfo="",int iConnectRetries=-1, int nConnectStepsLeft = MAX_CONNECT_STEPS);

		/**
		 * @brief sends the extra info and expects for an OK signal from the server
		 */
		virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );

		/**
		 * @brief closes the socket, used after errors to make sure we have no hanging connections
		 */
		virtual void Disconnect();

	};
}

#endif
