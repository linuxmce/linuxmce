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
#ifndef MYTHTVPROXYSERVER_H
#define MYTHTVPROXYSERVER_H

#include <string>
#include <list>

#include "thread.h"
#include "proxyeventhandler.h"


namespace MYTHTV {

class ProxyEventHandler;
class ProxyPeerThread;

/**
@author igor
*/

class ProxyServer : public Thread
{
public:
    ProxyServer();
    ~ProxyServer();

public:
	void setHandler(ProxyEventHandler* phandler = NULL) {
		phandler_ = phandler;
	}
	ProxyEventHandler* getHandler() {
		return phandler_;
	}

public:
	void setHost(const char* host) {
		host_ = host;
	}
	const char* getHost() {
		return host_.c_str();
	}

	void setPort(unsigned port) {
		port_ = port;
	}
	unsigned getPort() {
		return port_;
	}

	void setPeerHost(const char* peerhost) {
		peerhost_ = peerhost;
	}
	const char*  getPeerHost() {
		return peerhost_.c_str();
	}

	void setPeerPort(unsigned peerport) {
		peerport_ = peerport;
	}
	unsigned getPeerPort() {
		return peerport_;
	}

protected:
	virtual void* _Run();

protected:
	virtual bool handleStartup();
	virtual void handleTerminate();

protected:
	virtual void handleAccept(int sockfd, int peersockfd) = 0;

protected:
	void addThread(ProxyPeerThread* pthread);
	void removeThread(ProxyPeerThread* pthread);
	void checkTerminatedThreads(bool forceterminate = false);
	
private:
	ProxyEventHandler* phandler_;

private:
	std::string host_;
	unsigned port_;
	std::string peerhost_;
	unsigned peerport_;

private:
	std::list<ProxyPeerThread*> threads_;
	
private:
	int sockfd_;
};

};

#endif
