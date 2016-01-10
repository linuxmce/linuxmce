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
// BtFtp.h: interface for the CBtFtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BTFTP_H__27769BDB_1D86_4E84_B816_6D22C003B48D__INCLUDED_)
#define AFX_BTFTP_H__27769BDB_1D86_4E84_B816_6D22C003B48D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WinBluetooth/inc/BtIfDefinitions.h"
#include "WinBluetooth/inc/BtIfClasses.h"

class CBtFtpCallback
{
public:
	CBtFtpCallback() {};
	virtual ~CBtFtpCallback() {};

typedef enum
{
    FTP__SEND_FAILED,
    FTP__SEND_SUCCESS,
    FTP__SEND_PROGRESS,
    FTP__SEND_ABORTED,
    FTP__SEND_NONE
} eFtpEvents;

	virtual void OnFtpEventReceived(  eFtpEvents, long param1 = 0, long param2 = 0) = 0;
};

class CBtFtp : public CFtpClient
{
public:
	CBtFtp( CBtFtpCallback * cb );
	virtual ~CBtFtp();

	CFtpClient::FTP_RETURN_CODE OpenConnection(BD_ADDR bda,CSdpDiscoveryRec &sdp_rec);
	CFtpClient::FTP_RETURN_CODE CBtFtp::PutFile( string strFilename );

private:
	CBtFtpCallback		*		m_Callback;

protected:
    // methods to replace virtual methods in base class CFtpClient 
    void OnOpenResponse(FTP_RESULT_CODE result_code);
    void OnCloseResponse(FTP_RESULT_CODE result_code);
    void OnProgress(FTP_RESULT_CODE result_code, WCHAR * name, long current, long total);
    void OnPutResponse(FTP_RESULT_CODE result_code, WCHAR * name);
    //void OnGetResponse(FTP_RESULT_CODE result_code, WCHAR * name);
    //void OnCreateResponse(FTP_RESULT_CODE result_code, WCHAR * name);
    //void OnDeleteResponse(FTP_RESULT_CODE result_code, WCHAR * name);
    
    //void OnChangeFolderResponse(FTP_RESULT_CODE result_code, tFtpFolder folder_type, WCHAR * szFolder);
    //void OnFolderListingResponse(FTP_RESULT_CODE result_code, tFTP_FILE_ENTRY * listing, long entries);
    //void OnXmlFolderListingResponse(FTP_RESULT_CODE rc, WCHAR * pfolder_listing, long folder_length );
    void OnAbortResponse(FTP_RESULT_CODE result_code);
};

#endif // !defined(AFX_BTFTP_H__27769BDB_1D86_4E84_B816_6D22C003B48D__INCLUDED_)
