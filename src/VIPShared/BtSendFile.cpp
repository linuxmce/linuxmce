/*
 BtSendFile
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPIncludes.h"
#include "BtSendFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD CBtSendFile::s_DiscoTimeout = 5000;	//5 seconds

CBtSendFile::CBtSendFile( CBtSendFileCallback * cb)
{
	m_Callback = cb;
	m_hDiscoCompleteEvent = CreateEvent
	(
		NULL,
		TRUE,
		FALSE,
		NULL
	);
	
	if( NULL == m_hDiscoCompleteEvent )
	{
		throw("CBtSendFile - discocomplete");
	}

	memset( m_bda, 0, sizeof( BD_ADDR ) );

	m_nDiscoRecordsRead = 0;
	m_nDiscoSlots = 12;
	m_DiscoRecords = new CSdpDiscoveryRec[ m_nDiscoSlots ];
	m_nObexServiceIndex = -1;
	m_nFtpServiceIndex = -1;
	m_ptr_BtObex = NULL;
	m_ptr_BtFtp  = NULL;

}

CBtSendFile::~CBtSendFile()
{
	delete[] m_DiscoRecords;
	if ( m_ptr_BtObex != NULL )
		delete m_ptr_BtObex;

//	TRACE("\nDecon CBtSendFile");
}

void CBtSendFile::OnDiscoveryComplete()
{
	SetEvent( m_hDiscoCompleteEvent );
}

void CBtSendFile::WaitForDiscoToComplete()
{
	WaitForSingleObject( m_hDiscoCompleteEvent, s_DiscoTimeout );
}

BOOL CBtSendFile::StartDiscovery (BD_ADDR p_bda, GUID *p_service_guid)
{
	ResetEvent( m_hDiscoCompleteEvent );
	return CBtIf::StartDiscovery( p_bda, p_service_guid );
}

BOOL CBtSendFile::SendFile( string strFullPathFilename, string bda, eSendMethod ePref )
{
	if ( FALSE == ConvertAddr( bda.c_str(), m_bda ) )
		return FALSE;

//	TRACE( (TEXT("About to start discovery\n") ) );

	BOOL fRes;
	fRes = this->StartDiscovery( m_bda, NULL );

	if( !fRes )
		return FALSE;

	this->WaitForDiscoToComplete();

//	TRACE( (TEXT("Discovery complete....scanning service records\n") ) );

	while( 1 )
	{
		m_nDiscoRecordsRead = ReadDiscoveryRecords
		( 
			m_bda, 
			m_nDiscoSlots, 
			m_DiscoRecords, 
			NULL 
		);

		if( m_nDiscoRecordsRead < m_nDiscoSlots )
		{
			break; // we had enough splace
		}

		// increase the space

		m_nDiscoSlots += 5;

		delete[] m_DiscoRecords;

		m_DiscoRecords = new CSdpDiscoveryRec[ m_nDiscoSlots ];
	}

#ifdef _DEBUG
//	string otherstr;
//	otherstr.Format("\nFound %d service records\n", m_nDiscoRecordsRead );
//	TRACE(otherstr);
#endif // _DEBUG

	if( 0 == m_nDiscoRecordsRead )
		return FALSE;

	m_eSendMethod = ePref;
	m_eSendSts = SendIdle;
	m_nObexServiceIndex = -1;
	m_nFtpServiceIndex = -1;

	if ( FALSE == PickService() )		//sets up m_eSendSts to new state if available
		return FALSE;

	m_strFile = strFullPathFilename;

	m_eSendProgress = ProgressIdle;
	ChangeSendState();					//uses send sts etc.

	return TRUE;
}


BOOL CBtSendFile::ConvertAddr(const TCHAR* intext, BD_ADDR addr)
{
	// the array in text must be null terminated

	if( _tcslen( intext ) != 17 )
	{
		return FALSE;
	}

	TCHAR text[18];

	_tcscpy( text, intext );

	_tcsupr( text );

	if( text[2] != ':' ) return FALSE;
	if( text[5] != ':' ) return FALSE;
	if( text[8] != ':' ) return FALSE;
	if( text[11] != ':' ) return FALSE;
	if( text[14] != ':' ) return FALSE;

	if( !hexPair( text + 0, addr + 0 ) ) return FALSE;
	if( !hexPair( text + 3, addr + 1  ) ) return FALSE;
	if( !hexPair( text + 6, addr + 2  ) ) return FALSE;
	if( !hexPair( text + 9, addr + 3  ) ) return FALSE;
	if( !hexPair( text + 12, addr + 4  ) ) return FALSE;
	if( !hexPair( text + 15, addr + 5  ) ) return FALSE;

	if( text[17] != '\0' ) return FALSE;

	return TRUE;
}

BOOL CBtSendFile::hexPair(const TCHAR *text, BYTE *val)
{
	*val = 0;

	if( (*(text + 0) >= '0') && (*(text + 0) <= '9') )
	{
		*val += *(text + 0) - '0';
	}
	else
	if( (*(text + 0) >= 'A') && (*(text + 0) <= 'F') )
	{
		*val += *(text + 0) - 'A' + 10;
	}
	else
	{
		return FALSE;
	}

	*val = *val << 4;

	if( (*(text + 1) >= '0') && (*(text + 1) <= '9') )
	{
		*val += *(text + 1) - '0';
	}
	else
	if( (*(text + 1) >= 'A') && (*(text + 1) <= 'F') )
	{
		*val += *(text + 1) - 'A' + 10;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CBtSendFile::PickService( void )
{
	if( 0 == m_nDiscoRecordsRead )
		return FALSE;

	int i;

	for( i = 0; i < m_nDiscoRecordsRead; i++ )
	{
#ifdef _DEBUG
//		string otherstr;
//		otherstr.Format("\nService %d :: Name: ", i);
//		otherstr += m_DiscoRecords[i].m_service_name;
//		TRACE(otherstr);
#endif // _DEBUG

		if (m_DiscoRecords[i].m_service_guid == guid_SERVCLASS_OBEX_OBJECT_PUSH)
		{
			m_nObexServiceIndex = i;
		}

		if ((m_DiscoRecords[i].m_service_guid == guid_SERVCLASS_OBEX_FILE_TRANSFER) ||
		    (m_DiscoRecords[i].m_service_guid == guid_SERVCLASS_GENERIC_FILETRANSFER))
		{
			m_nFtpServiceIndex = i;
		}
	}
	return SelectService();
}

void CBtSendFile::ChangeSendState( void )
{
	if ( m_eSendProgress == ProgressIdle )
	{
	//if m_eSendSts = SendingByObex, send by obex (push)
	//if m_eSendSts = SendingByFtp, send by ftp(open,put)
		if (m_eSendSts == SendingByObex) //send by obex (push)
		{
SendByObex:
			WCHAR *p_s_wide = (WCHAR *)malloc (2*m_strFile.length()+2);
			mbstowcs(p_s_wide, m_strFile.c_str(), m_strFile.length()+1);

			if ( m_ptr_BtObex == NULL )
				m_ptr_BtObex = new CBtObex( this );

			CBtObex::OPP_RETURN_CODE opret;
			opret = m_ptr_BtObex->Push(m_bda, p_s_wide, m_DiscoRecords[m_nObexServiceIndex]);
			free( p_s_wide );

			if (opret == SUCCESS )
			{//we are running.... callbacks from OpClient will progress us...
				m_eSendProgress = ProgressActive;
			}
			else
			{//we failed, but can we try FTP?
				delete m_ptr_BtObex;
				m_ptr_BtObex = NULL;
				if ( SelectService() == FALSE )
				{
					m_Callback->OnSendFileEventReceived( CBtSendFileCallback::eSendFileCBEvents::FILE_SEND_FAILED );
					return;
				}
				else
				{
					goto	SendByFtp;
				}
			}
		}
		else if (m_eSendSts == SendingByFtp) //send by ftp(open,put)
		{
SendByFtp:
			m_ptr_BtFtp  = new CBtFtp( this );
			CFtpClient::FTP_RETURN_CODE ftpret;
			ftpret = m_ptr_BtFtp->OpenConnection( m_bda, m_DiscoRecords[m_nFtpServiceIndex]);
			if (ftpret == CFtpClient::SUCCESS )
			{//we are running.... callbacks from FtpClient will progress us...
				m_eSendProgress = ProgressActive;
			}
			else
			{//we failed, but can we try Obex?
				delete m_ptr_BtFtp;
				m_ptr_BtFtp = NULL;
				if ( SelectService() == FALSE )
				{
					m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_FAILED );
					return;
				}
				else
				{
					goto	SendByObex;
				}
			}
		}
	}
	else if( m_eSendProgress == ProgressActive )
	{
		if (m_eSendSts == SendingByFtp) //send by ftp(put file stage)
		{
			CFtpClient::FTP_RETURN_CODE ftpret;
			ftpret = m_ptr_BtFtp->PutFile( m_strFile );
			if (ftpret != CFtpClient::SUCCESS )
			{
				delete m_ptr_BtFtp;
				m_ptr_BtFtp = NULL;
				if ( SelectService() == FALSE )
				{
					m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_FAILED );
					return;
				}
				else
				{
					goto	SendByObex;
				}
			}
		}
	}
	else if ( m_eSendProgress == ProgressCompleted )
	{
	}
}


BOOL CBtSendFile::SelectService( void )
{
	if ( m_eSendSts == SendIdle )
	{
		if ( m_eSendMethod == BY_OBEX_THEN_FTP  )
		{
			if ( m_nObexServiceIndex == -1 )
			{
				if ( m_nFtpServiceIndex == -1 )
					return FALSE;		//cannot support this;
				else
				{
					m_eSendSts = SendingByFtp;
					return TRUE;
				}
			}
			else
			{
				m_eSendSts = SendingByObex;
				return TRUE;
			}
		}
		else if ( m_eSendMethod == BY_OBEX_THEN_NONE )
		{
			if ( m_nObexServiceIndex == -1 )
				return FALSE;		//cannot support this;
			else
			{
				m_eSendSts = SendingByObex;
				return TRUE;
			}
		}
		else if (  m_eSendMethod == BY_FTP_THEN_NONE )
		{
			if ( m_nFtpServiceIndex == -1 )
				return FALSE;		//cannot support this;
			else
			{
				m_eSendSts = SendingByFtp;
				return TRUE;
			}
		}
		else if ( m_eSendMethod == BY_FTP_THEN_OBEX )
		{
			if ( m_nFtpServiceIndex == -1 )
			{
				if ( m_nObexServiceIndex == -1 )
					return FALSE;		//cannot support this;
				else
				{
					m_eSendSts = SendingByObex;
					return TRUE;
				}
			}
			else
			{
				m_eSendSts = SendingByFtp;
				return TRUE;
			}
		}
	}
	else if ( m_eSendSts == SendingByFtp )
	{
		if ( m_eSendMethod == BY_OBEX_THEN_FTP  )
		{
			return FALSE;		//cannot support this;
		}
		else if ( m_eSendMethod == BY_OBEX_THEN_NONE )
		{
			throw("no by obex then none");
//			ASSERT( FALSE );		//cannot support this;
		}
		else if (  m_eSendMethod == BY_FTP_THEN_NONE )
		{
			return FALSE;		//cannot support this;
		}
		else if ( m_eSendMethod == BY_FTP_THEN_OBEX )
		{
			if ( m_nObexServiceIndex == -1 )
				return FALSE;		//cannot support this;
			else
			{
				m_eSendSts = SendingByObex;
				return TRUE;
			}
		}
	}
	else if (m_eSendSts == SendingByObex)
	{
		if ( m_eSendMethod == BY_OBEX_THEN_FTP  )
		{
			if ( m_nFtpServiceIndex == -1 )
				return FALSE;		//cannot support this;
			else
			{
				m_eSendSts = SendingByFtp;
				return TRUE;
			}
		}
		else if ( m_eSendMethod == BY_OBEX_THEN_NONE )
		{
			return FALSE;		//cannot support this;
		}
		else if (  m_eSendMethod == BY_FTP_THEN_NONE )
		{
			throw("no ftp then none");
//			ASSERT( FALSE );		//cannot support this;
		}
		else if ( m_eSendMethod == BY_FTP_THEN_OBEX )
		{
			return FALSE;		//cannot support this;
		}
	}
	return FALSE;
}

void CBtSendFile::OnObexEventReceived( CBtObexCallback::eObexEvents eEvent, long param1, long param2)
{
	//param1, param2 are generally only used for progress control bars.
	if ( eEvent == CBtObexCallback::OBEX__SEND_FAILED )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_FAILED );
	}
	else if ( eEvent == CBtObexCallback::OBEX__SEND_SUCCESS )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_COMPLETED );
	}
	else if ( eEvent == CBtObexCallback::OBEX__SEND_PROGRESS )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_PROGRESS, param1, param2 );
	}
	else if ( eEvent == CBtObexCallback::OBEX__SEND_ABORTED )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_ABORTED );
	}
}

void CBtSendFile::OnFtpEventReceived( CBtFtpCallback::eFtpEvents eEvent, long param1, long param2)
{
	//param1, param2 are generally only used for progress control bars.
	if ( eEvent == CBtFtpCallback::FTP__SEND_FAILED )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_FAILED );
	}
	else if ( eEvent == CBtFtpCallback::FTP__SEND_SUCCESS )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_COMPLETED );
	}
	else if ( eEvent == CBtFtpCallback::FTP__SEND_PROGRESS )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_PROGRESS, param1, param2 );
	}
	else if ( eEvent == CBtFtpCallback::FTP__SEND_ABORTED )
	{
		m_Callback->OnSendFileEventReceived( CBtSendFileCallback::FILE_SEND_ABORTED );
	}
}
