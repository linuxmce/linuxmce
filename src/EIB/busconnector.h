/***************************************************************************
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

/** @file busconnector.h
Header for BusConnector class.
*/
#ifndef EIBBUSCONNECTOR_H
#define EIBBUSCONNECTOR_H

#include "mutex.h"
#include "extendedserialport.h"
#include "Serial/SerialPort.h"

/** @namespace EIBBUS
For EIB (European Installation Bus) Lighting control.
*/
namespace EIBBUS {

/** @class BusConnector
For ???
*/
class BusConnector {
public:
    BusConnector();
    ~BusConnector();

public:
	/*connection methods*/
	int Open(const char* serport,int serbps, enum eParityBitStop serparity);
	void Close();
	bool isOpened();

public:
	/*send/recv options*/
	bool isDataAvailable();
	int Recv(unsigned char* buff, unsigned int size, int timeout);
	int UndoRecv(const unsigned char* buff, unsigned int size);
	int Send(const unsigned char* buff, unsigned int size);
	void Skip(unsigned int size);
	void Clear();

public:
	inline void incFrameBit() {
		framebit_ = !framebit_;
	}
	inline bool getFrameBit() {
		return framebit_;
	}
	inline unsigned short getLastFrameTimestamp() {
		return framets_;
	}
	inline void setLastFrameTimestamp(unsigned short ts) {
		framets_ = ts;
	}

private:
	ExtendedSerialPort* psp_;
	bool framebit_;
	unsigned short framets_;

public:
	static BusConnector* getInstance();

private:
	static Mutex s_minst;
	static BusConnector* s_instance;
};

};

#endif
