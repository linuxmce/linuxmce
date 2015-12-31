/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Interface: statefullproxypeerthread
//
// Description:
//
//
// Author: igor <igor@dexx>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYTHTVSTATEFULLPROXYPEERTHREAD_H
#define MYTHTVSTATEFULLPROXYPEERTHREAD_H

#include "proxypeerthread.h"

namespace MYTHTV
{

#define PEER_STATE_NONE		0

	/**
	@author igor
	*/
	class StatefullProxyPeerThread : public ProxyPeerThread
	{
		public:
			StatefullProxyPeerThread(ProxyServer *pserver, int srcsockfd, int destsockfd);
			~StatefullProxyPeerThread();

		public:
			inline void setState(int state) {
				state_ = state;
			}

			inline int getState() {
				return state_;
			}

		private:
			int state_;
	};
};

#endif
