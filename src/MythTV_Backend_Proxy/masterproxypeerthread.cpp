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
#include "masterproxypeerthread.h"

#include <cstdlib>

#include <iostream>

#include "constants.h"

using namespace std;

namespace MYTHTV {

unsigned
MasterProxyPeerThread::s_port_ = FOO_MIN_BACKENDPORT;

MasterProxyPeerThread::MasterProxyPeerThread(ProxyServer* pserver, int srcsockfd, int destsockfd)
	: ProxyPeerThread(pserver, srcsockfd, destsockfd), backrequested_(false)
{
	server_.setHandler(getServer()->getHandler());
}


MasterProxyPeerThread::~MasterProxyPeerThread()
{
	Wait(true);
}

bool
MasterProxyPeerThread::processData(const char* data, bool fromsrc)
{
	Token tok(data);
	if(fromsrc && tok.findValue("GET_NEXT_FREE_RECORDER") >= 0) {
		backrequested_ = true;
		return false;
	}
	else if(!fromsrc && backrequested_)
	{
		backrequested_ = false;
//		cout << "(!)This must be a reply with adress/port for backend server: " << data << endl;
		if(tok.getValuesNum() >= 3) {
			/*open proxy server*/
			string host = tok.getValue(1);

			if(host == "nohost") {
//				cout << "No host available. " << endl;
				sleep(5);
				return false;
			} else {
				unsigned port = atoi(tok.getValue(2).c_str());

				/*close the previous instance*/
				server_.Wait(true);

				/*reconfigure new instance*/
				server_.setHost(getServer()->getHost());

				getServer()->getHandler()->FrontendConnected(getSourceIP().c_str());

				mp.Lock();
				server_.setPort(s_port_);
				if(s_port_ >= FOO_MAX_BACKENDPORT) {
					s_port_ = FOO_MIN_BACKENDPORT;
				} else {
					s_port_++;
				}
				mp.Unlock();

				server_.setPort(s_port_);
				server_.setPeerHost(host.c_str());
				server_.setPeerPort(port);

				/*run new instance*/
//				cout << "Starting FOO Backend server for Host: " << host << ", Port: " << port << endl;

				server_.Run(false);

				/*populate token*/
				tok.setValue(1, getServer()->getHost());

				char portbuff[13];
				sprintf(portbuff, "%d", server_.getPort());
				tok.setValue(2, portbuff);

				writeData(getSrcSock(), tok.Serialize().c_str());
				return true;
			}
		}
	}

	return false;
}

};
