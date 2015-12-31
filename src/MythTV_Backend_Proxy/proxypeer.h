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
#ifndef MYTHTHPROXYPEER_H
#define MYTHTHPROXYPEER_H

#include <string>

namespace MYTHTV {

/**
@author igor
*/
class ProxyPeer {
public:
    ProxyPeer();
    ProxyPeer(int srcsockfd, int destsockfd);
    ~ProxyPeer();

public:

	inline void setSrcSock(int srcsockfd) {
		srcsockfd_ = srcsockfd;
	}
	inline int getSrcSock() {
		return srcsockfd_;
	}

	inline void setDestSock(int destsockfd) {
		destsockfd_ = destsockfd;
	}
	inline int getDestSock() {
		return destsockfd_;
	}

	std::string getSourceIP();

public:
	int srcsockfd_;
	int destsockfd_;

};

};

#endif
