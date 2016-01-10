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
#include <signal.h>

#include "errcode.h"
#include "remote.h"
#include "lowlevel.h"
#include "global.h"
#include "winusbio.h"


FT_HANDLE usb;

int OpenUSBPort (void)
{
	enum FT_STATUS stat;

	stat = F_OpenEx ("IRTrans USB",FT_OPEN_BY_DESCRIPTION,&usb);

	if (stat) return (ERR_OPENUSB);

	return (0);
}


int WriteUSBString (byte pnt[],int len)
{
	DWORD num;
	enum FT_STATUS stat;

	stat = F_Write (usb,pnt,len,&num);
	if (stat) return (0);

	return (num);
}


int	ReadUSBString (byte pnt[],int len,word timeout)
{
	DWORD num;
	enum FT_STATUS stat;

	F_SetTimeouts (usb,timeout,0);

	stat = F_Read (usb,pnt,len,&num);

	if (stat) return (0);

	return (num);
}


int GetUSBAvailableEx (DEVICEINFO *dev)
{
	DWORD num;
	enum FT_STATUS stat;

	stat = F_GetQueueStatus (dev->io.usbport,&num);

	if (stat) return (0);
	
	return (num);
}


int	ReadUSBStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout)
{
	DWORD num;
	enum FT_STATUS stat;

	F_SetTimeouts (dev->io.usbport,timeout,0);

	stat = F_Read (dev->io.usbport,pnt,len,&num);

	if (stat) return (0);

	return (num);
}


void FlushUSB (void)
{
	F_Purge (usb,FT_PURGE_RX | FT_PURGE_TX);
}


void FlushUSBEx (FT_HANDLE hndl)
{
	F_Purge (hndl,FT_PURGE_RX | FT_PURGE_TX);
}

void WriteUSBStringEx (DEVICEINFO *dev,byte pnt[],int len)
{
	DWORD num;
	enum FT_STATUS stat;

	stat = F_Write (dev->io.usbport,pnt,len,&num);
	if (stat == 4) {
		F_Close (dev->io.usbport);
		if (mode_flag & NO_RECONNECT) {
			log_print ("IRTrans Connection lost. Aborting ...\n",LOG_FATAL);
			exit (-1);
		}
		while (stat) {
			log_print ("Trying reconnect ...\n",LOG_DEBUG);
			stat = F_OpenEx (dev->usb_serno,FT_OPEN_BY_SERIAL_NUMBER,&dev->io.usbport);
			if (stat) Sleep (1000);
			else {
				Sleep (2000);
				log_print ("Reconnected ...\n",LOG_DEBUG);
			}
		}
	}
}


void SetUSBEventEx (DEVICEINFO *dev,DWORD mask)
{
	F_SetEventNotification (dev->io.usbport,mask,dev->io.event);
}

void break_signal (int sig)
{
	log_print ("Abort ...\n",LOG_FATAL);
	exit (0);
}

void cleanup_exit (void)
{
	int i;
	for (i=0;i < device_cnt;i++) if (IRDevices[i].io.if_type == IF_USB) F_Close (IRDevices[i].io.usbport);
	if (hdll) FreeLibrary(hdll);
}


int LoadUSBLibrary (void)
{

	char msg[256];

	atexit (cleanup_exit);
	signal (SIGINT,break_signal);

	hdll = LoadLibrary("Ftd2xx.dll");	
	if(hdll == NULL)
	{
		sprintf (msg,"Error: Can't Load ftd2xx.dll\n");
		log_print (msg,LOG_FATAL);
		return (-1);
	}

	m_pListDevices = (PtrToListDevices)GetProcAddress(hdll, "FT_ListDevices");

	m_pOpen = (PtrToOpen)GetProcAddress(hdll, "FT_Open");

	m_pOpenEx = (PtrToOpenEx)GetProcAddress(hdll, "FT_OpenEx");

	m_pRead = (PtrToRead)GetProcAddress(hdll, "FT_Read");

	m_pClose = (PtrToClose)GetProcAddress(hdll, "FT_Close");

	m_pGetQueueStatus = (PtrToGetQueueStatus)GetProcAddress(hdll, "FT_GetQueueStatus");

	m_pWrite = (PtrToWrite)GetProcAddress(hdll, "FT_Write");

	m_pResetDevice = (PtrToResetDevice)GetProcAddress(hdll, "FT_ResetDevice");

	m_pPurge = (PtrToPurge)GetProcAddress(hdll, "FT_Purge");

	m_pSetTimeouts = (PtrToSetTimeouts)GetProcAddress(hdll, "FT_SetTimeouts");

	m_pSetEvent = (PtrToSetEvent)GetProcAddress(hdll, "FT_SetEventNotification");

	return (0);
}


enum FT_STATUS F_SetEventNotification(FT_HANDLE usb,DWORD mask,PVOID event)
{
	return (*m_pSetEvent)(usb, mask, event);
}


enum FT_STATUS F_ListDevices(PVOID pArg1, PVOID pArg2, DWORD dwFlags)
{
	return (*m_pListDevices)(pArg1, pArg2, dwFlags);
}	

enum FT_STATUS F_Open(PVOID pvDevice,FT_HANDLE *usb)
{
	return (*m_pOpen)(pvDevice, usb );
}	

enum FT_STATUS F_OpenEx(PVOID pArg1, DWORD dwFlags,FT_HANDLE *usb)
{
	return (*m_pOpenEx)(pArg1, dwFlags, usb);
}	

enum FT_STATUS F_Read(FT_HANDLE usb,LPVOID lpvBuffer, DWORD dwBuffSize, LPDWORD lpdwBytesRead)
{
	return (*m_pRead)(usb, lpvBuffer, dwBuffSize, lpdwBytesRead);
}	

enum FT_STATUS F_Close(FT_HANDLE usb)
{
	return (*m_pClose)(usb);
}	

enum FT_STATUS F_GetQueueStatus(FT_HANDLE usb,LPDWORD lpdwAmountInRxQueue)
{
	return (*m_pGetQueueStatus)(usb, lpdwAmountInRxQueue);
}	

enum FT_STATUS F_Write(FT_HANDLE usb,LPVOID lpvBuffer, DWORD dwBuffSize, LPDWORD lpdwBytes)
{
	return (*m_pWrite)(usb, lpvBuffer, dwBuffSize, lpdwBytes);
}	


enum FT_STATUS F_ResetDevice(FT_HANDLE usb)
{
	return (*m_pResetDevice)(usb);
}	

enum FT_STATUS F_Purge(FT_HANDLE usb,ULONG dwMask)
{
	return (*m_pPurge)(usb, dwMask);
}	


enum FT_STATUS F_SetTimeouts(FT_HANDLE usb,ULONG dwReadTimeout, ULONG dwWriteTimeout)
{
	return (*m_pSetTimeouts)(usb, dwReadTimeout, dwWriteTimeout);
}	

#endif