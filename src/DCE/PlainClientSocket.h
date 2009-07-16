/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file PlainClientSocket.h
 Header file for the PlainClientSocket.
 @todo notcommented
 */
#ifndef PlainClientSocket_H
#define PlainClientSocket_H

#include "ClientSocket.h"

    /**
    @class PlainClientSocket
    This class is derived from DCE::ClientSocket.
     */
class PlainClientSocket : public DCE::ClientSocket
{
public:
    /** Constructor.
    @param IPAddress is the IP address for the socket.
    */
	PlainClientSocket(string IPAddress) :
		ClientSocket(1,IPAddress,"Plain Socket") {};

	bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );
};

#endif
