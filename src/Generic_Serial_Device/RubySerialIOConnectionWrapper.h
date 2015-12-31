/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubySerialIOConnectionWrapper.h
For ?
*/
#ifndef DCERUBYSERIALIOCONNECTIONWRAPPER_H
#define DCERUBYSERIALIOCONNECTIONWRAPPER_H

#include "BufferedIOConnection.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class RubySerialIOConnectionWrapper
For ?
*/
class RubySerialIOConnectionWrapper : protected BufferedIOConnection {
public:
    RubySerialIOConnectionWrapper(IOConnection* pconn = NULL);
    virtual ~RubySerialIOConnectionWrapper();

public:
	/*limit access to only these members*/
	int Recv(char* buff, unsigned int size, int timeout) {
		return BufferedIOConnection::Recv(buff, size, timeout);
	}
	std::string
	RecvDelimited(const char* delimbuff, unsigned int delimsize, int timeout) {
		return BufferedIOConnection::RecvDelimited(delimbuff, delimsize, timeout);
	}
	void UndoRecv(const char* buff, unsigned int size) {
		BufferedIOConnection::UndoRecv(buff, size);
	}
	int Send(const char* buff, unsigned int size) {
		return BufferedIOConnection::Send(buff, size);
	}
	bool isDataAvailable(int timeout) {
		return BufferedIOConnection::isDataAvailable(timeout);
	}

	void Flush() {
		BufferedIOConnection::Flush();
	}

	bool Reconnect();

	void Close() {
		BufferedIOConnection::Close();
	}

};

};

#endif
