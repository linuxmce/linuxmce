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
#ifndef BDCommandProcessor_Windows_Socket_h
#define BDCommandProcessor_Windows_Socket_h

#include "BDCommandProcessor.h"
#include "Socket.h"

using namespace DCE;

class BDCommandProcessor_Windows_Socket : public BDCommandProcessor
{
protected:
	class Socket *m_pSocket;

public:
	pthread_t m_BDSockThreadID;

	BDCommandProcessor_Windows_Socket(string sMacAddressPhone,Socket *pSocket);

	bool SendData(int size,const char *data);
	char *ReceiveData(int size);

};

#endif
