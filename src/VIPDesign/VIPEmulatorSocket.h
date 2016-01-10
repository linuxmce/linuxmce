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
#ifndef VIPEmulator_SOCKET_H
#define VIPEmulator_SOCKET_H

#include "DCE/Socket.h"

using namespace std;
using namespace DCE;

class OCServerCore;
class VIPEmulator;

using namespace DCE;

class VIPEmulatorSocket : public Socket
{
public:
	long m_DeviceID;
	pthread_t m_ClientThreadID;
	map<string,string> m_mapHTTPHeaders;
	string m_CMDString;
    
	VIPEmulator *m_pListener;
	class OCDeviceCommand *m_pCommand;

	VIPEmulatorSocket(VIPEmulator *pListener, SOCKET Sock);
	virtual ~VIPEmulatorSocket();

	string MakeUrl(map<long,string>::iterator &i, string Page);

	void Run();
};

#endif

