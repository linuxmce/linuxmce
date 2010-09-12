/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "NetworkIOConnection.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>

#include "DCE/Logger.h"

#include "IOUtils.h"

namespace DCE {

NetworkIOConnection::NetworkIOConnection()
	: sockfd_(-1)
{
}


NetworkIOConnection::~NetworkIOConnection()
{
}

bool 
NetworkIOConnection::Open() {
	if(host_.empty()) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Host ADDRESS was not specified.");
		return false;
	}

	Close();
	
	struct hostent *hent;
	if ((hent = gethostbyname(host_.c_str())) == 0) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error resolving host name: %s", host_.c_str());
		return false;
	}

	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_);
	addr.sin_addr = *((in_addr *)hent->h_addr);
	memset(&(addr.sin_zero), 0, 8);

	if (::connect(sockfd_, (sockaddr *) &addr, sizeof(sockaddr)))
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Error connecting to %s on port %d", host_.c_str(), port_);
		Close();
		return false;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connected to %s on port %d, socket %d", host_.c_str(), port_, sockfd_);
	return true;
}

void 
NetworkIOConnection::Close() {
	if (sockfd_ > -1)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Closing connection to %s", host_.c_str());
		shutdown(sockfd_, SHUT_RDWR);
		close(sockfd_);
		sockfd_ = -1;
	}
}
	
int 
NetworkIOConnection::Send(const char* buff, unsigned int size) {
	if(sockfd_ < 0) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Trying to send DATA while not connected.");
		return -1;
	}
	LoggerWrapper::GetInstance()->Write(LV_SEND_DATA, "Sending buffer to %s with size %d: <%s>.", 
									host_.c_str(), size, IOUtils::FormatHexAsciiBuffer(buff, size,"31").c_str());
	size = send(sockfd_, buff, size, 0);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Buffer sent.");
	return size;
}

int 
NetworkIOConnection::Recv(char* buff, unsigned int size, int timeout) {
	if(sockfd_ < 0) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Trying to receive DATA while not connected.");
		return -1;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Receiving buffer from %s with max size %d and timeout %d...", 
									host_.c_str(), size, timeout);
	
	int retsize = 0;
	if(timeout < 0 || isDataAvailable(timeout)) {
		retsize = recv(sockfd_, buff, size, 0);
		if(retsize <= 0) {
			Close();		
			return retsize;
		}
	}
	
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "Received buffer from %s: <%s>", 
									host_.c_str(), IOUtils::FormatHexAsciiBuffer(buff, retsize,"33").c_str());
#endif

	return retsize;
}

bool 
NetworkIOConnection::isOpened() {
	return (sockfd_ >= 0);
}

bool 
NetworkIOConnection::isDataAvailable(int timeout) {
	if(sockfd_ < 0) {
		return false;
	}
	
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sockfd_, &rfds);
	
	struct timeval tv;
    tv.tv_sec = timeout / 1000 ;
    tv.tv_usec = (timeout % 1000) * 1000;

	int retval = select(sockfd_ + 1, &rfds, NULL, NULL, (timeout >= 0) ? &tv : NULL);
	if(retval < 0) {
		Close();
		return false;
	} else 
	if(retval == 0) {
		return false;
	} else {
		return true;
	}
}

};
