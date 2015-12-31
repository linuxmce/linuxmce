/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//
// C++ Implementation: socket
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "socket.h"
#include "DCE/Logger.h"
#include "asteriskmanager.h"

#ifdef WIN32

#else
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

using namespace std;
using namespace DCE;


#define ASMANAGER_PORT	5038

namespace ASTERISK {

Socket::Socket() : socket_fd(0)
{
}

Socket::~Socket()
{
}

int Socket::Connect() {
	int ret = 0;
	
	socket_fd = (int)socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd > 0) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "socket created.");
	} else {
		socket_fd = 0;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "socket creation failed.");
		return -1;
	}
	
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
		
	address.sin_family = AF_INET;
	address.sin_port = htons(ASMANAGER_PORT);

	string sIP = AsteriskManager::getInstance()->Get_Server_IP();

#ifdef WIN32
	address.sin_addr.s_addr = inet_addr(sIP.c_str());
#else
	inet_pton(AF_INET, sIP.c_str(), &address.sin_addr);
#endif
	
	if((ret = connect(socket_fd, (struct sockaddr*)&address, sizeof(address)))) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failed connecting to asterisk manager");
		socket_fd = 0;
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "connected to asterisk manager");
		recvGreeting();
	}
	
	return ret;	
}

int Socket::Disconnect() {
	if(!socket_fd) {
		return -1;
	}
	close(socket_fd);
	socket_fd = 0;
	return 0;
};

bool Socket::isConnected() {
	return socket_fd != 0;
}

int Socket::sendToken(const Token& token) {
	if(!socket_fd) {
		return -1;
	}
	string str = token.serialize();
	if(!str.empty()) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "sending command:\n%s", str.c_str());
		if(send(socket_fd, str.c_str(), (int)str.length(), 0) > 0) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "command sent successfully.");
			return 0;
		} else {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failed sending command.");
			return -1;
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Ignored sending empty token.");		
	}
	return -1;
}

bool Socket::isReceivable() {
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(socket_fd, &rfds);
	
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 10*1000;
	
	if(select((int)(socket_fd + 1), &rfds, 0, 0, &tv) > 0)  {
		return true;
	} else {
		return false;
	}
}

#define RECV_BUFFER_SIZE  128

int Socket::recvGreeting() {
	char recvbuff[RECV_BUFFER_SIZE];

	string recvstr;
	while(1) {
		int recvlen = recv(socket_fd, recvbuff, RECV_BUFFER_SIZE - 1, 0);	
		if(recvlen > 0) {
			recvbuff[recvlen] = 0;
			recvstr += recvbuff;
			if(recvlen < RECV_BUFFER_SIZE - 1) {
				break;
			}
		} else {
			break;
		}
	}

	if(recvstr.empty()) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failed receiving greeting");
		return -1;
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "received greeting:\n%s", recvstr.c_str());
	}

	return 0;
}

string Socket::recvLine() {
	string recvline;
	char recvbuff[RECV_BUFFER_SIZE];
	
	while(1) {
		if(!recvtail.empty()) {
			int lineend = (int)recvtail.find('\x0A', 0);
			if(lineend != -1) {
				recvline.append(recvtail.substr(0, lineend + 1));
				recvtail.erase(0, lineend + 1);
				break;
			} else {
				recvline.append(recvtail);
				recvtail.clear();
			}
		} else {
			int recvlen = recv(socket_fd, recvbuff, RECV_BUFFER_SIZE - 1, 0);
			if(recvlen > 0) {
				recvbuff[recvlen] = 0;
				recvtail.append(recvbuff);
			} else {
				break;
			}
		}
	}
	
	return recvline;
}

int Socket::recvToken(Token& token) {
	string recvreply, recvline;
	while(1) {
		recvline = recvLine();
		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "received line:\n%s", recvline.c_str());
		if(recvline.empty() || recvline == "\x0D\x0A") {
			break;
		}
		recvreply += recvline;
	}

	if(!recvreply.empty()) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "received message:\n%s", recvreply.c_str());
		if(!token.unserialize(recvreply)) {
			return 0;
		} else {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "failed unserializing message.");
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "failed receiving message:\n%s", recvreply.c_str());
	}
	
	return -1;	
}

}
