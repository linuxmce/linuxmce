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
#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor_Windows_Socket.h"

#include "Socket.h"

using namespace DCE;

BDCommandProcessor_Windows_Socket::BDCommandProcessor_Windows_Socket(string sMacAddressPhone,Socket *pSocket)
: BDCommandProcessor(sMacAddressPhone)
	
{
	m_pSocket=pSocket;
}


bool BDCommandProcessor_Windows_Socket::SendData(int size,const char *data)
{
	return m_pSocket->SendData(size,data);
}

char *BDCommandProcessor_Windows_Socket::ReceiveData(int size)
{
	char *Data = new char[size];
	if( !m_pSocket->ReceiveData(size,Data) )
	{
		delete[] Data;
		return NULL;
	}
	return Data;
}

