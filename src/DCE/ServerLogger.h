/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file ServerLogger.h
 Header file for the ServerLogger class.
 @todo notcommented
 */
#ifndef ServerLogger_H
#define ServerLogger_H

#include "ClientSocket.h"
#include "AlarmManager.h"
#include "DCE/Logger.h"

namespace DCE
{

    /**
    @class ServerLogger
    This class ???
     */
class ServerLogger : public Logger, public ClientSocket
{
public:
	//ServerLogger();
	ServerLogger(int DeviceID, int PK_DeviceTemplate, ::std::string server);
	virtual ~ServerLogger();

	virtual void WriteBlock(const char *pBlock, size_t sBlockLen ) {}
	virtual void WriteEntry(class Logger::Entry& entry);

	virtual void Start();
	virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" )
	{
		m_bConnected = ClientSocket::OnConnect(PK_DeviceTemplate,"servlog");
		return m_bConnected;
	};

	void RunConnectThread();

	virtual bool IsConnected() { return m_bConnected; }

	virtual int GetType() { return LT_LOGGER_SERVER; }

protected:
	bool m_bConnected;

	pthread_t m_Thread;
};
}

#endif
