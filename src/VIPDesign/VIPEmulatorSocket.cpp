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

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

#include "DCE/MyStl.h"
#include "VIPEmulator.h"
#include "VIPEmulatorSocket.h"
#include "VIPShared/RequestProcessor.h"
#include "RA/RA_Request.h"
#include "VIPShared/VR_UnknownRequest.h"
#include "VIPShared/VR_RequestPayment.h"
#include "VIPShared/VIPAction.h"
#include "BD/BDCommandProcessor_Windows_Socket.h"
#include "VIPShared/PlutoConfig.h"
#include <iostream>

#include "VIPShared/BT_PC_KeyWasPressed.h"

static const char *HTTPHeader = "HTTP/1.1 200 OK\nMax-Age: 0\nExpires: -1\n"
	    	"Cache-control: no-cache\nCache-control: private\nPragma: no-cache\n";
	
void *BeginClientThread(void *param)
{	
	VIPEmulatorSocket *pCS = (VIPEmulatorSocket *)param;
	pCS->Run();
	return NULL;
}

VIPEmulatorSocket::VIPEmulatorSocket(VIPEmulator *pListener, SOCKET Sock) : 
	DCESocket("VIPEmulator")
{
	m_DeviceID = -1;
	m_Socket = Sock;
	m_pListener = pListener;

	pthread_create(&m_ClientThreadID, NULL, BeginClientThread, (void *)this);
}

VIPEmulatorSocket::~VIPEmulatorSocket()
{
}

void VIPEmulatorSocket::Run()
{
	BDCommandProcessor_Windows_Socket *pProcessor = new BDCommandProcessor_Windows_Socket("",this);

	PLUTO_SAFETY_LOCK(pm,pProcessor->m_PollingMutex);
	while(true)
	{
		if( pProcessor->MYSTL_SIZEOF_LIST(m_listCommands)==0 && pProcessor->m_pCommand_Sent==NULL )
		{
int a=EINVAL;
int b=EFAULT;
int c=ETIMEDOUT;

			timespec timeout;
			timeout.tv_sec = 500000; 
	g_pPlutoConfig->m_iPollingInterval = 2000000000;
			timeout.tv_nsec = 0; g_pPlutoConfig->m_iPollingInterval;
			int ret = pm.TimedCondWait(timeout);
Sleep(500); // This is screwed up, it doesn't matter if it's seconds, ms, nanoseconds, the cond_timedwait always returns immediately
int k=2;
		}

		// We are holding the mutex
		pProcessor->SendCommand();
Sleep(25); // Somehow designer is still tying up all the CPU time????
	}

   	if (!SOCKFAIL(m_Socket))
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	delete this;
	return;
}


string VIPEmulatorSocket::MakeUrl(map<long,string>::iterator &i, string Page)
{
	return "<a href=\"/"+Page+"_"+StringUtils::itos((*i).first)+".htm\">"+(*i).second+"</a>";
}
