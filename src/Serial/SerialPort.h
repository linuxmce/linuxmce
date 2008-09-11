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
#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <string>

#ifdef WIN32
#include "serio.h"
#endif

using namespace std;

enum eParityBitStop { 
	epbsN81,
	epbsE81,
	epbsO81
};

class   CSerialPort
{
#ifdef WIN32
	serio_t m_Serio;
#else
    int m_fdSerial;
#endif

public:
	// Will throw with an error description as a string if the port cannot be opened.
	CSerialPort(string Port, unsigned int BPS, enum eParityBitStop ParityBitStop, bool EnableFlowControl=false);
	virtual ~CSerialPort();

	virtual bool IsReadEmpty();
	virtual bool IsRngSet();
	virtual size_t Read(char *Buf, size_t MaxLen, int Timeout=5);  // Timeout in millseconds
	virtual void Write(char *Buf, size_t Len);
	virtual void Flush();
	
	/** Sends data to generate a break condition.
	  * @param time how long to keep the break condition
	  * TODO: Windows implementation
	  */
	virtual bool SendBreak(int time=0);
	
	/** Try to detect if there is a serial communication.
	  * TODO: Windows implementation
	  */
	virtual bool IsBusy();

public:
#ifdef WIN32
    serio_t
#else
    int
#endif
	getHandle() {
#ifdef WIN32
		return m_Serio;
#else
		return m_fdSerial;
#endif

	}

private:
#ifndef WIN32
	class Private;
	Private * d;
#endif

};

#endif

