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
// BtSendFile.h: interface for the CBtSendFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BTSENDFILE_H__5C781562_DD2A_41C7_9743_2C052DA3CFAC__INCLUDED_)
#define AFX_BTSENDFILE_H__5C781562_DD2A_41C7_9743_2C052DA3CFAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WinBluetooth/inc/BtIfDefinitions.h"
#include "WinBluetooth/inc/BtIfClasses.h"

#include "BtObex.h"
#include "BtFtp.h"

class CBtSendFileCallback
{
public:
	CBtSendFileCallback() {};
	virtual ~CBtSendFileCallback() {};

typedef enum
{
    FILE_SEND_COMPLETED,
    FILE_SEND_ABORTED,
    FILE_SEND_FAILED,
    FILE_SEND_PROGRESS
} eSendFileCBEvents;

	virtual void OnSendFileEventReceived(  eSendFileCBEvents e_event_code, long param1 = 0, long param2 = 0) = 0;
};

class CBtSendFile : public CBtIf, public CBtObexCallback, public CBtFtpCallback
{
public:
	CBtSendFile( CBtSendFileCallback * cb);
	virtual ~CBtSendFile();

typedef enum
{
    BY_OBEX_THEN_FTP,
    BY_OBEX_THEN_NONE,
    BY_FTP_THEN_OBEX,
    BY_FTP_THEN_NONE
} eSendMethod;

typedef enum
{
    SendIdle,
    SendingByObex,
    SendingByFtp
} eSendStatus;

typedef enum
{
    ProgressIdle,
    ProgressActive,
    ProgressCompleted
} eSendProgess;

	
protected:
	static DWORD				s_DiscoTimeout;
	HANDLE						m_hDiscoCompleteEvent;
	int							m_nDiscoRecordsRead;
	int							m_nDiscoSlots;
	CSdpDiscoveryRec	*		m_DiscoRecords;

	// methods to replace virtual methods in base class CBtIf 
    //void OnDiscoveryComplete (UINT16 nRecs, long lResultCode);
	void OnDiscoveryComplete();

	// methods to replace virtual methods in base class CBtObexCallback
	void OnObexEventReceived( CBtObexCallback::eObexEvents eEvent, long param1 = 0, long param2 = 0);

	// methods to replace virtual methods in base class CBtFtpCallback
	void OnFtpEventReceived( CBtFtpCallback::eFtpEvents eEvent, long param1 = 0, long param2 = 0);

public:
	BOOL	SendFile( string strFullPathFilename, string bda, eSendMethod ePref );
	CBtSendFileCallback	*		m_Callback;
private:
	BD_ADDR						m_bda;

	CBtObex				*		m_ptr_BtObex;
	CBtFtp				*		m_ptr_BtFtp;
	eSendStatus					m_eSendSts;
	eSendMethod					m_eSendMethod;
	eSendProgess				m_eSendProgress;

	string						m_strFile;

	int							m_nObexServiceIndex;
	int							m_nFtpServiceIndex;

	BOOL				StartDiscovery (BD_ADDR p_bda, GUID *p_service_guid);
	void				WaitForDiscoToComplete();

	BOOL				hexPair(const TCHAR *text, BYTE *val);
	BOOL				ConvertAddr(const TCHAR* intext, BD_ADDR addr);
	BOOL				PickService( void );
	BOOL				SelectService( void );
	void				ChangeSendState( void );

};

#endif // !defined(AFX_BTSENDFILE_H__5C781562_DD2A_41C7_9743_2C052DA3CFAC__INCLUDED_)
