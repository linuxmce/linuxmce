/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file SerialIOConnection.h
Header file.
*/
#ifndef DCESERIALIOCONNECTION_H
#define DCESERIALIOCONNECTION_H

#include "IOConnection.h"
#include "Serial/SerialPort.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class SerialIOConnection
A class for using I/O connections over a serial device.
*/
class SerialIOConnection : public IOConnection {
public:
    SerialIOConnection();
    virtual ~SerialIOConnection();

public:
	void setSerialPort(const char* serport) {
		serport_ = serport;
	}
	const char* getSerialPort() {
		return serport_.c_str();
	}

	void setBPS(unsigned int bps) {
		bps_ = bps;
	}
	unsigned int getBPS() {
		return bps_;
	}

	void setParityBitStop(eParityBitStop bs) {
		bs_ = bs;
	}
	unsigned int getParityBitStop() {
		return bs_;
	}

	void setEnableFlowControl(bool fc) {
		fc_ = fc;
	}
	bool getEnableFlowControl() {
		return fc_;
	}

public:
	virtual bool Open();
	virtual void Close();

	virtual int Send(const char* buff, unsigned int size);
	virtual int Recv(char* buff, unsigned int size, int timeout);

	virtual bool isOpened();
	virtual bool isDataAvailable(int timeout);

	void Flush();

private:
	CSerialPort* psp_;

	std::string serport_;
	unsigned int bps_;
	eParityBitStop bs_;
	bool fc_;
};

};

#endif
