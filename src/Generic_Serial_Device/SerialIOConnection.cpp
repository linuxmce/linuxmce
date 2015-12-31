/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "SerialIOConnection.h"
#include "DCE/Logger.h"

#include "IOUtils.h"
#include "PlutoUtils/LinuxSerialUSB.h"

#define DEFAULT_SERIAL_PORT	"ttyS0"

using namespace DCE;

namespace DCE {

SerialIOConnection::SerialIOConnection()
	: psp_(NULL), serport_(DEFAULT_SERIAL_PORT), bps_(9600), bs_(epbsN81), fc_(false)
{
}


SerialIOConnection::~SerialIOConnection()
{
}

bool 
SerialIOConnection::Open() {	
	Close();
	serport_ = TranslateSerialUSB(serport_);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Opening connection to %s.", serport_.c_str());
    try {
        psp_ = new CSerialPort(serport_, bps_, bs_, fc_);
		Sleep(100);
	} catch(...) {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed Opening serial port: %s.", serport_.c_str());
		Close();
        return false;
    }
	return true;
}

void 
SerialIOConnection::Close() {
	if(psp_ != NULL) {
		delete psp_;
		psp_ = NULL;
	}
}
	
int 
SerialIOConnection::Send(const char* buff, unsigned int size) {
	if(psp_ != NULL) {
		LoggerWrapper::GetInstance()->Write(LV_SEND_DATA, "Sending buffer to %s with size %d: <%s>.", 
									serport_.c_str(), size, IOUtils::FormatHexAsciiBuffer(buff, size, "31").c_str());
		psp_->Write((char*)buff, size);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Buffer sent.");
		return size;
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Trying to send DATA while not connected.");
		return -1;
	}
}

int 
SerialIOConnection::Recv(char* buff, unsigned int size, int timeout) {
	if(psp_ != NULL) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Receiving buffer from %s with max size %d and timeout %d...", 
									serport_.c_str(), size, timeout);
		size_t retsize = psp_->Read(buff, size, timeout);
		if(retsize == 0) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received nothing.");
		}
		else {
			LoggerWrapper::GetInstance()->Write(LV_RECEIVE_DATA, "Received buffer from %s: <%s>", 
									serport_.c_str(), IOUtils::FormatHexAsciiBuffer(buff, retsize, "33").c_str());
		}
		
		// ugly, the serial API should be consistent between IOConnection and SerialPort
		// try to use size_t
		return (int)retsize;
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Trying to receive DATA while not connected.");
		return -1;
	}
}
	
bool 
SerialIOConnection::isOpened() {
	return (psp_ != NULL);
}

void SerialIOConnection::Flush() {
	if(psp_ != NULL) {
		psp_->Flush();
	}
}

bool 
SerialIOConnection::isDataAvailable(int timeout) {
	if(psp_ != NULL) {
		while(timeout > 0) {
			if(!(psp_->IsReadEmpty())) {
				return true;
			}
			usleep(10);
			timeout -= 10;
		}
		return (!(psp_->IsReadEmpty()));
	}
	return false;
}

};
