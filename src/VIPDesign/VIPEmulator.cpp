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
#include <string>
#include <fstream>
#include <stdio.h>
#include <list>


#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
//#include "DeviceCreator.h" //TODO : what/was is this?
#include "PlutoUtils/MyStl.h"
#include "VIPEmulatorSocket.h"
#include "VIPEmulator.h"
#include "VIPShared/PlutoConfig.h"


class PlutoConfig *gp_PlutoConfig=NULL;

void* EmulatorThread(void* param)
{
	LoggerWrapper::SetType(LT_LOGGER_FILE,"C:\\temp.log");

	VIPEmulator *pServer = new VIPEmulator();
	pServer->Initialize();
	pServer->StartListening(3461);//gp_PlutoConfig->m_iServerPort);
	pServer->Run();
	delete pServer;
	
	return NULL;
}

DCESocket *VIPEmulator::CreateSocket(SOCKET newsock)
{
	DCESocket *pSocket = new VIPEmulatorSocket(this, newsock);
	return pSocket;
}

void VIPEmulator::Initialize()
{
}


void VIPEmulator::Run()
{
	if (m_ListenerThreadID)
		pthread_join(m_ListenerThreadID, NULL);
}
