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
#ifndef VIPEmulator_H
#define VIPEmulator_H

#include "DCE/SocketListener.h"
#include "DCE/HandleRequestSocket.h"

using namespace DCE;

class VIPEmulator : public SocketListener
{
public:
	VIPEmulator() : 
		DCESocketListener("VIPEmulator")
	{
	}

	virtual ~VIPEmulator()
	{
	}

	void Initialize();


	virtual Socket *CreateSocket(SOCKET newsock);

	virtual bool ReceivedString(Socket *pSocket, string Line, int nTimeout = -1)
	{
		return true;
	};

	virtual void ReceivedDCEMessage(Socket *pSocket, Message *pMessage)
	{
	};

	virtual void RegisteredCommandHandler(int DeviceID)
	{
	};

	void Run();
};

#endif

