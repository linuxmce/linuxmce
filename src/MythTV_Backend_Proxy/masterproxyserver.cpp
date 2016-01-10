/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "masterproxyserver.h"
#include "constants.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include "masterproxypeerthread.h"

#include "DCE/Logger.h"

using namespace std;
using namespace DCE;

namespace MYTHTV
{
	MasterProxyServer::MasterProxyServer()
	{
		setPeerHost(MASTER_BACKEND_HOST);
		setPeerPort(MASTER_BACKEND_PORT);
	};

	MasterProxyServer::~MasterProxyServer()
	{
		Wait(true);
	}

	void MasterProxyServer::handleAccept(int sockfd, int peersockfd)
	{
		ProxyPeerThread *ppeerthr = new MasterProxyPeerThread(this, sockfd, peersockfd);
		ppeerthr->Run(false);
		addThread(ppeerthr);
	}
};
