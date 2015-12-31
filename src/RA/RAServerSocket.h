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
#ifndef RASERVER_SOCKET_H
#define RASERVER_SOCKET_H

#include "Socket.h"

using namespace std;

class RAServerSocket : public DCE::Socket
{
public:
	pthread_t m_ClientThreadID;
    
	class Logger *m_pLogger;

	class RAServer *m_pListener;

	RAServerSocket(class RAServer *pListener, SOCKET Sock);
	virtual ~RAServerSocket();

	void Run();

	bool IsInitedOK();
	
private:
	bool m_bInitedOK;
};

#endif

