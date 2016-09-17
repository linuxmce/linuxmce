/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file Event_Impl.h
 Header file for the Event_Impl class.
 @todo notcommented
 */
#ifndef OCDEVICEEVENT_H
#define OCDEVICEEVENT_H

#include "ClientSocket.h"

namespace DCE
{
	class Message;

	/** @class Event_Impl
	 * @brief implementation for an event
	 */
	class Event_Impl
	{

	public:

		long m_dwPK_Device; /**< the id for the device the event is asociated with */

		ClientSocket *m_pClientSocket; /**< pointer to the client socket the event came from @todo ask*/

		/**
		 * @brief If this is the master device's event, create a connection to the server
		 * @param bConnectEventHandler specifies if we should connect the client socket after creating it
		 */
		Event_Impl( long dwDevice, long dwDeviceTemplate, string sServerAddress, bool bConnectEventHandler=true, int nSocketTimeout = -1, int nConnectRetries = -1 /* default */, bool bIsSSL=false );

		/**
		 * @brief If this is a sub-device's event, just use pointer to the connection
		 */
		Event_Impl( ClientSocket *pClientSocket, long dwDevice );

		/**
		 * @brief destructor, just frees memory pointed by m_pClientSocket (if this is a master's device event)
		 */
		virtual ~Event_Impl();

		/**
		 * @brief uses the client socket to send the specified message
		 * @return true if successfull
		 */
		bool SendMessage( Message *pMessage );

		/**
		 * @brief uses the client socket to send the specified message with an expected response of type ER_DeliveryConfirmation and puts the response in sResponse
		 * @return true if successfull
		 */
		bool SendMessage( Message *pMessage, string &sResponse );

		/**
		 * @brief uses the client socket to send the specified message with an expected response of ER_ReplyMessage
		 * @return the reply message
		 */
		Message *SendReceiveMessage( Message *pMessage );

		/**
		 * @brief uses the client socket to send the specified string
		 */
		string SendReceiveString( string s, int nTimeout = -1 );

		/** @todo check comment */
		//string GetParameter(int ToDevice,int ParmType);

		/**
		 * @brief requests the confiuration from the router
		 * Event gets tasked with pulling the config, because Event has control
		 * over the socket on which the Device Manager is listening.
		 * @returns a pointer to the configuration data or NULL if errors encountered
		 */
		char *GetConfig( /*unsigned long*/ int &dwSize );

		/**
		 * @brief requests and tries to receive the device list from the server
		 * @returns NULL on errors, pointer to the data otherwise
		 */
		char *GetDeviceList( /*unsigned long*/ int &dwSize );

		/**
		 * @brief sets the specified parameter from the specified device to the specified value
		 * @warning not implemented here
		 */
		void SetParameter( unsigned long dwToDevice, unsigned long dwParmType, string sValue );

		/**
		 * @brief gets the specified parameter from the specified device and returns it's value in the sResult parameter
		 * @warning not implemented here
		 */
		string GetCurrentDeviceData( int PK_Device, int PK_DeviceData );  // Get it from the device itself
		string GetDeviceDataFromDatabase( int PK_Device, int PK_DeviceData );  // Get the latest value from the router

		/**
		 * @brief should create a new Event_Impl object based on the parameters
		 * @see the second constructor
		 * @warning this must be implemented in a derived class
		 * @todo ask for more specific
		 */
		virtual class Event_Impl *CreateEvent( unsigned long /*dwPK_DeviceTemplate*/, ClientSocket * /*pOCClientSocket*/,
				unsigned long /*dwDevice*/ ) {return NULL;};
	};
}
#endif
