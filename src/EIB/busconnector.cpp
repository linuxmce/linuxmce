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
#include "busconnector.h"

#include <termios.h>

#include "DCE/Logger.h"

#define FOO_BUFF_SIZE		256
#define SKIP_WAIT_TIME		100

using namespace DCE;

namespace EIBBUS {

Mutex BusConnector::s_minst;
BusConnector* BusConnector::s_instance = NULL;

BusConnector::BusConnector()
	: framebit_(false), framets_(0)
{
}


BusConnector::~BusConnector()
{
}

BusConnector* 
BusConnector::getInstance() {
	static BusConnector connector;
	
	s_minst.Lock();
	if(!s_instance)	{
		s_instance = &connector;
	}
	s_minst.Unlock();
	
	return s_instance;
}

int 
BusConnector::Open(const char* serport,int serbps, eParityBitStop serparity) {
	try {
		psp_ = new ExtendedSerialPort(serport, serbps, serparity);
		int hserial = psp_->getHandle();

		struct termios terminal;
		if ( -1 == tcgetattr(hserial, &terminal) ) {
			return -1;
		}
		
		// set EIB specific values
		terminal.c_iflag = terminal.c_iflag | (INPCK);
		terminal.c_iflag = terminal.c_iflag & ~ (IXON | ICRNL);
		terminal.c_cflag = terminal.c_cflag & ~(CSIZE | CSTOPB | CRTSCTS | PARODD);
		terminal.c_cflag = terminal.c_cflag |  (CS8 | CREAD | HUPCL
														| CLOCAL | PARENB);
   
		terminal.c_oflag = 0;
		terminal.c_lflag = 0;
		
		int termios_speed=0;
		switch(serbps)
		{
			case 0    : termios_speed = B0;break;
			case 50   : termios_speed = B50;break;
			case 75   : termios_speed = B75;break;
			case 110  : termios_speed = B110;break;
			case 134  : termios_speed = B134;break;
			case 150  : termios_speed = B150;break;
			case 200  : termios_speed = B200;break;
			case 300  : termios_speed = B300;break;
			case 600  : termios_speed = B600;break;
			case 1200 : termios_speed = B1200;break;
			case 1800 : termios_speed = B1800;break;
			case 2400 : termios_speed = B2400;break;
			case 4800 : termios_speed = B4800;break;
			case 9600 : termios_speed = B9600;break;
			case 19200: termios_speed = B19200;break;
			case 38400: termios_speed = B38400;break;
		}
		if ( -1 == cfsetospeed(&terminal,termios_speed) ) {
			return -1;
		}
		if ( -1 == cfsetispeed(&terminal,termios_speed) ) {
			return -1;
		}
		terminal.c_cc[VMIN]=0;
		terminal.c_cc[VTIME]=1;
		
		if ( -1 == tcsetattr(hserial, TCSANOW, &terminal) ) {
			return -1;
		}
		if ( -1 == tcflush(hserial, TCIOFLUSH) ) {
			return -1;
		}
   	
	} catch(...) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed Opening EIB serial port: %s", serport);
		return -1;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "EIB serial port opened.");
	return 0;
}

void 
BusConnector::Close() {
	if(psp_) {
		delete psp_;
	}
}

bool 
BusConnector::isOpened() {
	return psp_ != NULL;
}

bool 
BusConnector::isDataAvailable() {
	return !psp_->IsReadEmpty();
}

int 
BusConnector::Recv(unsigned char* buff, unsigned int size, int timeout) {
	if(!psp_) { return -1; };
	return psp_->Read((char*)buff, size, timeout);
}
int 
BusConnector::UndoRecv(const unsigned char* buff, unsigned int size) {
	if(!psp_) { return -1; };
	return psp_->UndoRead((char*)buff, size);
}

int 
BusConnector::Send(const unsigned char* buff, unsigned int size) {	
	if(!psp_) { return -1; };
	psp_->Write((char*)buff, size);
	return size;
}

void 
BusConnector::Skip(unsigned int size) {
	char inchar[FOO_BUFF_SIZE];
	int nread;
	while(size > 0) {
		nread = psp_->Read(inchar, (size > FOO_BUFF_SIZE) ? FOO_BUFF_SIZE : size, SKIP_WAIT_TIME);
		if(nread < 0) {
			break;
		}
		size -= nread;
	}
}

void 
BusConnector::Clear() {
	char inchar[FOO_BUFF_SIZE];
	while(!psp_->IsReadEmpty()) {
		psp_->Read(inchar, 1, FOO_BUFF_SIZE);
	}
}

};

