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
#ifndef MYTHTVPROXYPEERTHREAD_H
#define MYTHTVPROXYPEERTHREAD_H

#include <list>
#include <string>

#include "proxypeer.h"
#include "thread.h"
#include "proxyserver.h"

namespace MYTHTV
{
	/**
	 * @author igor
	 *
	 * s/ProxyPeer/Tunnel
	 */
	class ProxyPeerThread : public ProxyPeer, public Thread
	{
	public:
		ProxyPeerThread(ProxyServer *pserver, int srcsockfd, int destsockfd);
		virtual ~ProxyPeerThread();

	protected:
		virtual void* _Run();

	public:
		ProxyServer* getServer() {
			return pserver_;
		};

	public:
		inline int writeDataToSrc(const std::string& data) {
			return writeData(getSrcSock(), data);
		}
		inline int readDataFromSrc(std::string& data) {
			return readData(getSrcSock(), data);
		}
		inline int writeDataToDest(const std::string& data) {
			return writeData(getDestSock(), data);
		}
		inline int readDataFromDest(std::string& data) {
			return readData(getDestSock(), data);
		}

	protected:
		virtual bool processData(const char* data, bool fromsrc) = 0;

	protected:
		virtual void handleTerminate();

	protected:
		int readData(int sockfd, std::string& data);
		int writeData(int sockfd, const std::string& data);

		int replData(bool fromsrc);

	private:
		ProxyServer* pserver_;
	};
};

#endif
