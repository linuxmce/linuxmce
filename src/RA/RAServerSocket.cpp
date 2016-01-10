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
#include <iostream>
#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "DCE/DCEMySqlConfig.h"

#include "RAServer.h"
#include "RAServerSocket.h"
#include "RA/RA_Request.h"
#include "RA/RA_Processor.h"
#include <iostream>

DCEMySqlConfig *g_pDCEMySqlConfig = new DCEMySqlConfig();

void *BeginClientThread(void *param)
{	
	RAServerSocket *pCS = (RAServerSocket *)param;
	pCS->Run();
	delete pCS;
	return NULL;
}

RAServerSocket::RAServerSocket(class RAServer *pListener, SOCKET Sock) : Socket("raserver")
{
	m_bInitedOK = true;
	m_Socket = Sock;
	m_pListener = pListener;

	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

	int iRes = pthread_create(&m_ClientThreadID, &threadAttr, BeginClientThread, (void *)this);

	pthread_attr_destroy(&threadAttr);

	if (iRes!=0)
	{
		printf("Error creating new thread, pthread_create returned: %i\n", iRes);
		if (!SOCKFAIL(m_Socket))
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}

		m_bInitedOK = false;
	}
}

RAServerSocket::~RAServerSocket()
{
}

bool RAServerSocket::IsInitedOK()
{
	return m_bInitedOK;
}

void RAServerSocket::Run()
{
	RA_Processor *pRA_Processor = RA_Processor::CreateRA_Processor(this,g_pDCEMySqlConfig);
	if( !pRA_Processor )
	{
		SendString("FAILURE");
	}
	else
	{
		SendString("OK");
		while( pRA_Processor->ReceiveRequests(this) );
		delete pRA_Processor;
	}
	if (!SOCKFAIL(m_Socket))
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	return;
}
