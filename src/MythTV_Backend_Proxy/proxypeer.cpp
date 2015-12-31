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
#include "proxypeer.h"

#include <netdb.h>

namespace MYTHTV
{
	using namespace std;

	ProxyPeer::ProxyPeer()
		: srcsockfd_(0), destsockfd_(0)
	{
	}

	ProxyPeer::ProxyPeer(int srcsockfd, int destsockfd)
		: srcsockfd_(srcsockfd), destsockfd_(destsockfd)
	{
	}

	ProxyPeer::~ProxyPeer()
	{
	}

	string ProxyPeer::getSourceIP()
	{
		sockaddr_in peer;
		socklen_t peerlen = sizeof(peer);
		char host[NI_MAXHOST];

		getpeername(getSrcSock(), (struct sockaddr*)&peer, &peerlen);
		getnameinfo((struct sockaddr*)&peer, peerlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
		return string(host);
	}
};
