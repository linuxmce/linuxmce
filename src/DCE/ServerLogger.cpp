/* 
	ServerLogger
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU Lesser General Public License for more details.
*/

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

#include "PlutoUtils/CommonIncludes.h"	
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Message.h"

using namespace DCE;

/////////////////////////////////////////////////////////////////////////////
void* ServerLoggerThread(void* param)
{
	ServerLogger* sl = (ServerLogger*)param;
	sl->RunConnectThread();	
	return NULL;
}


ServerLogger::ServerLogger(int DeviceID, int PK_DeviceTemplate, string server) :ClientSocket(DeviceID, server, string("Logger ") + StringUtils::itos(DeviceID))
{
#ifdef PTHREAD2
    m_Thread.p = 0;
#else
    m_Thread = 0;
#endif
	m_bConnected = false;
	m_Name = "*DEV* " + StringUtils::itos(DeviceID);
	Connect(PK_DeviceTemplate, "serv log" + m_Name);

	// Start a thread that will attempt to reconnect to the server every 5 seconds if the log ever has a problem
	Start();
}

ServerLogger::~ServerLogger()
{
	m_bQuit_set(true);
#ifdef PTHREAD2
        if (m_Thread.p)
#else
        if (m_Thread)
#endif
            pthread_join(m_Thread,NULL);
}


void ServerLogger::Start()
{
	pthread_create(&m_Thread, NULL, ServerLoggerThread, this);
}

void ServerLogger::RunConnectThread()
{
	while(!m_bQuit_get())
	{
		if(m_Socket == INVALID_SOCKET)
		{
			m_bConnected = false;
			Connect(0);
		}
		Sleep(5000);
	}
}

void ServerLogger::WriteEntry(Logger::Entry& entry)
{
	PLUTO_SAFETY_LOCK_LOGGER(sSM,m_Lock);  // Don't log anything but failures

	if(!m_bConnected || m_Socket == INVALID_SOCKET)
	{
		fprintf(stderr, "Trying to log to a disconnected server (%s)\n", entry.m_sData.c_str());
		return;
	}

	Message* msg = new Message(m_dwPK_Device, DEVICEID_LOGGER,PRIORITY_NORMAL, MESSAGETYPE_LOG, 0, 0);
	entry.SerializeWrite();
/*
	m_dwIDeviceStructure_Size=(unsigned long) allDevices.CurrentSize();
	m_pDeviceStructure=new char[m_dwIDeviceStructure_Size];
	memcpy(m_pDeviceStructure,allDevices.m_pcDataBlock,m_dwIDeviceStructure_Size);
*/
	msg->m_mapData_Parameters[0] = entry.m_pcDataBlock;
	msg->m_mapData_Lengths[0] = entry.CurrentSize();
/*
	m_mapParameters[LOG_PARAM_TIMESTAMP] = string(c);
	msg->m_mapParameters[LOG_PARAM_NAME] = entry.Name;
	msg->m_mapParameters[LOG_PARAM_DEVICE] = StringUtils::itos(entry.DeviceID);
	msg->m_mapParameters[LOG_PARAM_DATA] = entry.m_sData;
	msg->m_mapParameters[LOG_PARAM_LEVEL] = StringUtils::itos(entry.Level);
*/
	SendMessage(msg);
}
