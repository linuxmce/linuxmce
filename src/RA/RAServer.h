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
#ifndef RAServer_H
#define RAServer_H

#include "SocketListener.h"
#include "HandleRequestSocket.h"

class RAServer : public DCE::SocketListener  // Borrow DCE's Socket listener
{
public:
	RAServer();
	virtual ~RAServer();

	void Initialize();
	void Run();

	virtual DCE::Socket *CreateSocket( SOCKET newsock, string sName, string sIPAddress="", string sMacAddress=""  );

	// We're just borrowing DCE's socket listener class, which requires the following that are only for DCE.
	// We don't need them, but the base class is abstract, so we'll define them anyway
	virtual bool ReceivedString(DCE::Socket *pSocket, string Line, int nTimeout = -1) { return true; };
	virtual void ReceivedMessage(DCE::Socket *pSocket, DCE::Message *pMessage) {};
	virtual void RegisteredCommandHandler(int DeviceID)	{};

};

#endif

