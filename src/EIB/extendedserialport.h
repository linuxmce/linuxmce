/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file extendedserialport.h
Header for ExtendedSerialPort class.
*/
//
// C++ Interface: extendedserialport
//
// Description:
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSEXTENDEDSERIALPORT_H
#define EIBBUSEXTENDEDSERIALPORT_H

#include "Serial/SerialPort.h"

#include <sstream>

/** @namespace EIBBUS
For ???
*/
namespace EIBBUS {

/** @class ExtendedSerialPort
For serial ports.
*/
class ExtendedSerialPort : public CSerialPort
{
public:
    ExtendedSerialPort(string Port, unsigned int BPS, enum eParityBitStop ParityBitStop, bool EnableFlowControl=false);
    virtual ~ExtendedSerialPort();

public:
    virtual bool IsReadEmpty();
    virtual size_t Read(char *Buf, size_t MaxLen, int Timeout=5);
    virtual void Write(char *Buf, size_t Len);

	virtual int UndoRead(char *Buf, size_t Len);

public:
	stringbuf buff_;
};

};

#endif
