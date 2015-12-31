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
#ifndef MYTHTVBACKENDPROXYPEERTHREAD_H
#define MYTHTVBACKENDPROXYPEERTHREAD_H

#include "statefullproxypeerthread.h"

namespace MYTHTV
{

	#define BACKEND_STATES				(0x100)

	#define PEER_STATE_GETPROGRAMINFO	BACKEND_STATES + 1
	#define PEER_STATE_CHANGECHANNEL	BACKEND_STATES + 2
	#define PEER_STATE_SETCHANNEL		BACKEND_STATES + 3

	/**
	@author igor
	*/
	class BackendProxyPeerThread : public StatefullProxyPeerThread
	{
		public:
			BackendProxyPeerThread(ProxyServer* pserver, int srcsockfd, int destsockfd);
			~BackendProxyPeerThread();

		protected:
			virtual bool processData(const char* data, bool fromsrc);

		private:
			unsigned int channel_;

		private:
			/*states temporary data*/
			//int direction_;
			//int pendingchannel_;
	};

};

#endif
