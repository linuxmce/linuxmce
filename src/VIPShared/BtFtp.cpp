/*
 BtFtp
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPIncludes.h"
#include "BtFtp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBtFtp::CBtFtp( CBtFtpCallback * cbck )
{
	m_Callback = cbck;
}

CBtFtp::~CBtFtp()
{

}

CFtpClient::FTP_RETURN_CODE CBtFtp::OpenConnection(BD_ADDR bda,CSdpDiscoveryRec &sdp_rec)
{
	return CFtpClient::OpenConnection(bda, sdp_rec);
}

CFtpClient::FTP_RETURN_CODE CBtFtp::PutFile( string strFilename )
{
	WCHAR *p_s_wide = (WCHAR *)malloc (2*strFilename.length()+2);
	mbstowcs(p_s_wide, strFilename.c_str(), strFilename.length()+1);
	return CFtpClient::PutFile(p_s_wide);
}

void CBtFtp::OnProgress(FTP_RESULT_CODE result_code, WCHAR * name, long current, long total)
{
	m_Callback->OnFtpEventReceived( CBtFtpCallback::FTP__SEND_PROGRESS, current, total );
}

void CBtFtp::OnOpenResponse( FTP_RESULT_CODE  result_code)
{
//	string s;

	if (result_code != CFtpClient::COMPLETED)
	{
//		s = "FTP open failed.\nError Code" + StringUtils::itos(result_code);  //Filename: %s, m_p_loc_name,
		m_Callback->OnFtpEventReceived( CBtFtpCallback::FTP__SEND_FAILED );
	}
	else
	{
//		s = _T("FTP open complete");
		m_Callback->OnFtpEventReceived( CBtFtpCallback::FTP__SEND_SUCCESS );
	
	}
//	TRACE( s );
}

void CBtFtp::OnAbortResponse (FTP_RESULT_CODE result_code)
{
	m_Callback->OnFtpEventReceived( CBtFtpCallback::FTP__SEND_ABORTED );
}

void CBtFtp::OnPutResponse(FTP_RESULT_CODE result_code, WCHAR * name)
{
//	string s;

	if (result_code != CFtpClient::COMPLETED)
	{
//		s.Format("FTP put failed.\nError Code %d", result_code);  //Filename: %s, m_p_loc_name,
		m_Callback->OnFtpEventReceived( CBtFtpCallback::FTP__SEND_FAILED );
	}
	else
	{
//		s = _T("FTP put transfer complete");
		m_Callback->OnFtpEventReceived( CBtFtpCallback::FTP__SEND_SUCCESS );
	}
//	TRACE( s );
}

void CBtFtp::OnCloseResponse(FTP_RESULT_CODE result_code)
{
}
