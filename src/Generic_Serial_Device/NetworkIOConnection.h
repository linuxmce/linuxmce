/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file NetworkIOConnection.h
Header file.
*/
#ifndef DCENETWORKIOCONNECTION_H
#define DCENETWORKIOCONNECTION_H

#include <string>

#include "IOConnection.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class NetworkIOConnection
A class to ???
*/
class NetworkIOConnection : public IOConnection {
public:
    NetworkIOConnection();
    virtual ~NetworkIOConnection();

public:
	void setHost(const char* host) {
		host_ = host;
	}
	const char* getHost() {
		return host_.c_str();
	}

	void setPort(int port) {
		port_ = port;
	}
	int getPort() {
		return port_;
	}

public:
	virtual bool Open();
	virtual void Close();

	virtual int Send(const char* buff, unsigned int size);
	virtual int Recv(char* buff, unsigned int size, int timeout);

	virtual bool isOpened();
	virtual bool isDataAvailable(int timeout);

	void Flush() {};
private:
	std::string host_;
	int port_;
	int sockfd_;
};

};

#endif
