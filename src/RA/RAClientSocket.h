/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef RAClientSocket_H
#define RAClientSocket_H

#include "ClientSocket.h"

class RAClientSocket : public DCE::ClientSocket
{
public:
	RAClientSocket(long DeviceID, string IPAddress,string Name) :
		ClientSocket(DeviceID,IPAddress,Name) {};
	bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );
};

#endif
