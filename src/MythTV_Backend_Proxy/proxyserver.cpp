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
#include "proxyserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include "DCE/Logger.h"
#include "proxypeerthread.h"

#define SOCK_WAITQUEUE_LENGTH 	10

using namespace std;
using namespace DCE;

namespace MYTHTV
{
	ProxyServer::ProxyServer()
		: port_(0), peerport_(0) {
	}

	ProxyServer::~ProxyServer()
	{
		checkTerminatedThreads(true);
	}

	bool ProxyServer::handleStartup()
	{
		if((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't open socket.");
			return false;
		}

		struct hostent *hent;
		if ((hent = gethostbyname(host_.c_str())) == 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error resolving host name: %s", host_.c_str());
			return false;
		}

		int reuseport = 1;
		setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &reuseport, sizeof(reuseport));

		struct sockaddr_in serv_addr;
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr = *((in_addr *)hent->h_addr);
		serv_addr.sin_port = htons(port_);

		if(bind(sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't bind local address %s:%d.", host_.c_str(), port_);
			return false;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Listening on interface: %s, port: %d", host_.c_str(), port_);
		listen(sockfd_, SOCK_WAITQUEUE_LENGTH);
		return true;
	}

	void ProxyServer::handleTerminate()
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Shutting down Listen SOCKET.");
		shutdown(sockfd_, SHUT_RDWR);
		close(sockfd_);
	}

	void* ProxyServer::_Run()
	{
		fd_set rfds;

		struct sockaddr_in client_addr;
		int len_client_addr = sizeof(client_addr);

		while(!isStopRequested())
		{
			// check cleanups
			checkTerminatedThreads(false);

			FD_ZERO(&rfds);
			FD_SET(sockfd_, &rfds);

			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;

			int retval = select(sockfd_ + 1, &rfds, NULL, NULL, &tv);

			if(retval <= 0)
				continue;

			if(FD_ISSET(sockfd_, &rfds))
			{
				/*accept connection*/
				int client_sockfd =
					accept(sockfd_, (struct sockaddr *) &client_addr, (socklen_t*)&len_client_addr);

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connection accepted.");

				/*open peer connection to backend*/
				struct hostent *hent;
				if ((hent = gethostbyname(peerhost_.c_str())) == 0)
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error resolving host name: %s", peerhost_.c_str());
					continue;
				}

				int backend_sockfd =
					socket(AF_INET, SOCK_STREAM, 0);

				sockaddr_in backend_addr;
				backend_addr.sin_family = AF_INET;
				backend_addr.sin_port = htons(peerport_);
				backend_addr.sin_addr = *((in_addr *)hent->h_addr);
				memset(&(backend_addr.sin_zero), 0, 8);

				if (::connect(backend_sockfd, (sockaddr *) &backend_addr, sizeof(sockaddr)))
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error connecting to server %s on port %d", peerhost_.c_str(), peerport_);
					shutdown(client_sockfd, SHUT_RDWR);
					close(client_sockfd);
					continue;
				}

				handleAccept(client_sockfd, backend_sockfd);
			}
		}

		return 0;
	}

	void ProxyServer::checkTerminatedThreads(bool bForceTerminate)
	{
		std::list<ProxyPeerThread*>::iterator it = threads_.begin();
		while(it != threads_.end())
		{
			if(!(*it)->isRunning() || bForceTerminate)
			{
				delete (*it);
				it = threads_.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	void ProxyServer::addThread(ProxyPeerThread* pthread)
	{
		threads_.push_back(pthread);
	}

	void ProxyServer::removeThread(ProxyPeerThread* pthread)
	{
		std::list<ProxyPeerThread*>::iterator it = threads_.begin();
		while( it != threads_.end() )
		{
			if( *it == pthread )
				it = threads_.erase(it);
			else
				it++;
		}
	}
};
