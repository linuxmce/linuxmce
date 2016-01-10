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
#include "backendproxypeerthread.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// #include <netdb.h>
#include <cstdlib>

#include <iostream>

#include "token.h"
#include "proxyeventhandler.h"

using namespace std;

namespace MYTHTV
{
	BackendProxyPeerThread::BackendProxyPeerThread(ProxyServer* pserver, int srcsockfd, int destsockfd)
		: StatefullProxyPeerThread(pserver, srcsockfd, destsockfd), channel_(0)
	{
	//	cout << "Backend peer thread Created." << endl;
	};

	BackendProxyPeerThread::~BackendProxyPeerThread()
	{
	//	cout << "Backend peer thread Destroyed." << endl;
	}

	bool
	BackendProxyPeerThread::processData(const char* data, bool fromsrc) {
		ProxyEventHandler *phandler = getServer()->getHandler();
		if(phandler == NULL) {
			return false;
		}

		Token tok(data);
		int index = 0;

		if(fromsrc) {
			if((index = tok.findValue("GET_PROGRAM_INFO")) >= 0) {
				setState(PEER_STATE_GETPROGRAMINFO);
			}
			// else if ( (index = tok.findValue("QUERY_INPUT)))
			/*else
			if((index = tok.findValue("CHANGE_CHANNEL")) >= 0 && tok.getValuesNum() > index + 1) {
				direction_ = atoi(tok.getValue(index + 1).c_str());
				setState(PEER_STATE_CHANGECHANNEL);
			} else
			if((index = tok.findValue("SET_CHANNEL")) >= 0 && tok.getValuesNum() > index + 1) {
				pendingchannel_ = atoi(tok.getValue(index + 1).c_str());
				setState(PEER_STATE_SETCHANNEL);
			} else {
				setState(PEER_STATE_NONE);
			}*/
		} else {
			unsigned int newchannel = channel_;
			switch(getState()) {
				case PEER_STATE_GETPROGRAMINFO: {
					if(tok.getValuesNum() > 8) {
						newchannel = atoi(tok.getValue(8).c_str());
					}
				} break; /*
				case PEER_STATE_CHANGECHANNEL: {
					if(tok.findValue("ok") >= 0) {
						newchannel += ((direction_) ? -1 : 1);
					}
				} break;
				case PEER_STATE_SETCHANNEL: {
					if(tok.findValue("ok") >= 0) {
						newchannel = pendingchannel_;
					}
				}*/
			}

			setState(PEER_STATE_NONE);

			if(newchannel != channel_)
			{
	//			cout << "Channel changed from  " << channel_ << " to " << newchannel << endl;
				channel_ = newchannel;
				phandler->ChannelChanged(this->getSourceIP().c_str(), channel_);
			}
		}

		return false;
	}

};
