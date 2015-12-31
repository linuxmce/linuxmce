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
#include "ftd2xx.h"

typedef unsigned short word;

HMODULE hdll;

typedef enum FT_STATUS (WINAPI *PtrToListDevices)(PVOID, PVOID, DWORD);
PtrToListDevices m_pListDevices; 
enum FT_STATUS F_ListDevices(PVOID, PVOID, DWORD);

typedef enum FT_STATUS (WINAPI *PtrToOpen)(PVOID, FT_HANDLE *); 
PtrToOpen m_pOpen; 
enum FT_STATUS F_Open(PVOID, FT_HANDLE *);

typedef enum FT_STATUS (WINAPI *PtrToOpenEx)(PVOID, DWORD, FT_HANDLE *); 
PtrToOpenEx m_pOpenEx; 
enum FT_STATUS F_OpenEx(PVOID, DWORD, FT_HANDLE *);

typedef enum FT_STATUS (WINAPI *PtrToRead)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
PtrToRead m_pRead;
enum FT_STATUS F_Read(FT_HANDLE,LPVOID, DWORD, LPDWORD);

typedef enum FT_STATUS (WINAPI *PtrToClose)(FT_HANDLE);
PtrToClose m_pClose;
enum FT_STATUS F_Close(FT_HANDLE);

typedef enum FT_STATUS (WINAPI *PtrToGetQueueStatus)(FT_HANDLE, LPDWORD);
PtrToGetQueueStatus m_pGetQueueStatus;
enum FT_STATUS F_GetQueueStatus(FT_HANDLE,LPDWORD);

typedef enum FT_STATUS (WINAPI *PtrToWrite)(FT_HANDLE, LPVOID, DWORD, LPDWORD);
PtrToWrite m_pWrite;
enum FT_STATUS F_Write(FT_HANDLE,LPVOID, DWORD, LPDWORD);

typedef enum FT_STATUS (WINAPI *PtrToResetDevice)(FT_HANDLE);
PtrToResetDevice m_pResetDevice;
enum FT_STATUS F_ResetDevice(FT_HANDLE);

typedef enum FT_STATUS (WINAPI *PtrToPurge)(FT_HANDLE, ULONG);
PtrToPurge m_pPurge;
enum FT_STATUS F_Purge(FT_HANDLE,ULONG);

typedef enum FT_STATUS (WINAPI *PtrToSetTimeouts)(FT_HANDLE, ULONG, ULONG);
PtrToSetTimeouts m_pSetTimeouts;
enum FT_STATUS F_SetTimeouts(FT_HANDLE,ULONG, ULONG);

typedef enum FT_STATUS (WINAPI *PtrToSetEvent)(FT_HANDLE, DWORD, LPVOID);
PtrToSetEvent m_pSetEvent;
enum FT_STATUS F_SetEventNotification(FT_HANDLE,DWORD,LPVOID);

int		LoadUSBLibrary (void);
void	cleanup_exit (void);
void	break_signal (int sig);
int		OpenUSBPort (void);
int		WriteUSBString (byte pnt[],int len);
int		ReadUSBString (byte pnt[],int len,word timeout);
void	FlushUSB (void);
void	SetUSBEvent (PVOID,DWORD);
void	SetUSBEventEx (DEVICEINFO *dev,DWORD mask);
void	FlushUSBEx (FT_HANDLE hndl);
void	WriteUSBStringEx (DEVICEINFO *dev,byte pnt[],int len);
int		ReadUSBStringEx (DEVICEINFO *dev,byte pnt[],int len,word timeout);
int		GetUSBAvailableEx (DEVICEINFO *dev);
