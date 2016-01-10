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
// OCDeviceTemplate.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <list>
#include <sstream>
#include <assert.h>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "RAServerSocket.h"
#include "RAServer.h"

RAServer::RAServer() : SocketListener("RAServer")
{
}

RAServer::~RAServer()
{
}

DCE::Socket *RAServer::CreateSocket( SOCKET newsock, string sName, string sIPAddress, string sMacAddress )
{
	RAServerSocket *pSocket = new RAServerSocket(this, newsock);
	if (pSocket->IsInitedOK())
	{
		((DCE::Socket *)pSocket)->m_sIPAddress = sIPAddress;
	}
	else
	{
		delete pSocket;
		pSocket = NULL;
	}

	return (DCE::Socket *)pSocket;
}

void RAServer::Initialize()
{
}


void RAServer::Run()
{
	if (m_ListenerThreadID)
		pthread_join(m_ListenerThreadID, NULL);
}
