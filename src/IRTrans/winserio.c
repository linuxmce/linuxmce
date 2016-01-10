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
#ifdef WIN32

#include <windows.h>
#include <stdio.h>

#include "remote.h"
#include "errcode.h"
#include "lowlevel.h"
#include "global.h"


#include "serio.h"

HANDLE hCom;
HANDLE hComEvent;



// Low Level Routinen

// 1. Comm-Device Bereich öffnen
// 2. Daten Lesen mit Timeout Wert:	SetCommTimeouts
// 3. Transceiver zurücksetzen




void msSleep (long time)
{
	Sleep (time);
}

int WriteSerialStringEx (DEVICEINFO *dev,byte pnt[],int len)
{
	DWORD bytes;
	OVERLAPPED ov;

	memset (&ov,0,sizeof (ov));
	ov.hEvent = dev->io.event;

	WriteFile(dev->io.comport,pnt,len,&bytes,&ov);
	WaitForSingleObject (dev->io.event,100);
	GetOverlappedResult (dev->io.comport,&ov,&bytes,FALSE);
	ResetEvent (dev->io.event);
	if ((int)bytes != len) return (ERR_TIMEOUT);
	else return (0);
}

int GetSerialAvailableEx (DEVICEINFO *dev)
{
	return (0);
}

int ReadSerialStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout)
{
	int res,i = 0;
	DWORD bytes = 0,dummy;
	OVERLAPPED ov;

	memset (pnt,0,10);

	memset (&ov,0,sizeof (ov));
	ov.hEvent = dev->io.event;

	SetSerialTimeoutEx (dev,timeout);
	ReadFile(dev->io.comport,pnt,len,&dummy,&ov);
	res = WaitForSingleObject (dev->io.event,5);
	if (res == WAIT_TIMEOUT) res = WaitForSingleObject (dev->io.event,timeout);

	if (res != WAIT_TIMEOUT) GetOverlappedResult (dev->io.comport,&ov,&bytes,FALSE);

	ResetEvent (dev->io.event);

	return (bytes);
}


void FlushComEx(HANDLE fp)
{
	PurgeComm(fp,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}


void SetSerialTimeoutEx (DEVICEINFO *dev,word time)
{
	COMMTIMEOUTS to;

	memset (&to,0,sizeof (to));

	to.ReadIntervalTimeout = time;
	to.ReadTotalTimeoutConstant = time;
	SetCommTimeouts (dev->io.comport,&to);
}


int OpenSerialPortEx (char Pname[],HANDLE *port)
{
	DCB dcb={0};

	char sDCB[]="38400,n,8,1";

	*port=CreateFile(Pname,
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
				    NULL);
	if (*port == INVALID_HANDLE_VALUE) return (ERR_OPEN);

	BuildCommDCB(sDCB, &dcb);
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	SetCommState(*port,&dcb);
	if (mode_flag & NO_RESET) EscapeCommFunction(*port,SETRTS);
	else EscapeCommFunction(*port, CLRRTS);
	EscapeCommFunction(*port, SETDTR);
	Sleep(2000);
	FlushComEx (*port);
	return (0);
}


int OpenSerialPort(char Pname[])
{
	DCB dcb={0};

	char sDCB[]="38400,n,8,1";

	hCom=CreateFile(Pname,
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
				    NULL);
	
	BuildCommDCB(sDCB, &dcb);
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	SetCommState(hCom,&dcb);
	if (mode_flag & NO_RESET) 	EscapeCommFunction(hCom,SETRTS);
	else EscapeCommFunction(hCom, CLRRTS);
	EscapeCommFunction(hCom, SETDTR);
	Sleep(2000);
	FlushCom ();

	hComEvent = CreateEvent (NULL,TRUE,FALSE,NULL);
	return (0);
}


void WriteSerialString (byte pnt[],int len)
{

	DWORD bytes;
	OVERLAPPED ov;

	memset (&ov,0,sizeof (ov));
	ov.hEvent = hComEvent;

	WriteFile(hCom,pnt,len,&bytes,&ov);
	WaitForSingleObject (hComEvent,100);
	GetOverlappedResult (hCom,&ov,&bytes,FALSE);
	ResetEvent (hComEvent);
}


int ReadSerialString (byte pnt[],int len,word timeout)
{

	DWORD bytes;
	OVERLAPPED ov;

	memset (&ov,0,sizeof (ov));
	ov.hEvent = hComEvent;

	SetSerialTimeout (timeout);
	ReadFile(hCom,pnt,len,&bytes,&ov);
	WaitForSingleObject (hComEvent,timeout);
	GetOverlappedResult (hCom,&ov,&bytes,FALSE);
	ResetEvent (hComEvent);
	return (bytes);
}

void FlushCom(void)
{
	PurgeComm(hCom,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

void SetSerialTimeout (word time)
{
	COMMTIMEOUTS to;

	memset (&to,0,sizeof (to));

	to.ReadIntervalTimeout = time;
	to.ReadTotalTimeoutConstant = time;
	SetCommTimeouts (hCom,&to);
}



#endif