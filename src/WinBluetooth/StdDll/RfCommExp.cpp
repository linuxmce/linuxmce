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
// RfCommExp.cpp: implementation of the CRfCommExp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdDll.h"
#include "RfCommExp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DWORD CRfCommExp::s_ConnectionTimeout = 5000;

CRfCommExp::CRfCommExp( void )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("Make it");

	m_Connected = FALSE;

	m_hConnectionEvent = CreateEvent
	(
		NULL,
		TRUE,
		FALSE,
		NULL
	);

	if( !m_hConnectionEvent )
	{
		throw X_RESOURCE;
	}
}

CRfCommExp::~CRfCommExp()
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("Kill it");

	Cleanup();

	// assume we can't get any more callbacks => ok to delete our event...

	CloseHandle( m_hConnectionEvent );
}


BOOL CRfCommExp::ConvertAddr(const TCHAR* intext, BD_ADDR addr)
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

BOOL CRfCommExp::hexPair(const TCHAR *text, BYTE *val)
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


bool CRfCommExp::RfPortOpen( const /*CString&*/ char *device )
{
	const CSdpDiscoveryRec* service;
	LinkStatus ls;

	BOOL fRes = ConvertAddr( device, m_bd_addr );

	if( fRes )
	{
		m_ServiceSelect.SetBthAddr( m_bd_addr );

		ls = m_ServiceSelect.Select( &service );

		if( LS_SUCCESS == ls )
		{
			ls = ConnectToService( service );
		}
	}
	else
	{
		ls = LS_INVALID_BT_ADDR;
	}

	return (LS_SUCCESS == ls);
}

void CRfCommExp::RfPortClose( )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRfCommPort::Close();
}

LinkStatus CRfCommExp::ConnectToService(const CSdpDiscoveryRec *service)
{
	UINT8 scn;
	LinkStatus ls = LS_GENERAL_FAILURE;

	CSdpDiscoveryRec* localservice = (CSdpDiscoveryRec*)service; // nasty cast....

	BOOL fRes = localservice->FindRFCommScn( &scn );

	if( fRes )
	{
		fRes = AssignScnValue( &localservice->m_service_guid, scn );

		if( fRes )
		{
			fRes = SetSecurityLevel
			(
				localservice->m_service_name, 
				BTM_SEC_NONE , 
				FALSE
			);

			if( fRes )
			{
				PORT_RETURN_CODE prc;

				m_WaitingEventReceived = TRUE;

				prc = OpenClient( scn, m_bd_addr );
				ls = prc2LinkStatus( prc );

				if( LS_SUCCESS == ls )
				{
					DWORD dwRes;

					dwRes = WaitForSingleObject
					(
						m_hConnectionEvent,
						s_ConnectionTimeout
					);

					if( WAIT_OBJECT_0 == dwRes )
					{
						if( m_Connected )
						{
							ls = LS_SUCCESS;
						}
						else
						{
							TRACE("\nConnection COMPLETION_FAILED");
							ls = LS_CONNECTION_COMPLETION_FAILED;
						}
					}
					else
					{
						ls = LS_CONNECTION_COMPLETION_FAILED;
						TRACE("\nConnection COMPLETION_FAILED 2");
					}
				}
			}
			else
			{
				ls = LS_BAD_SET_SECURITY;
				TRACE("\nConnection BAD_SET_SECURITY");
				AfxMessageBox("\nConnection BAD_SET_SECURITY");
			}
		}
		else
		{
			ls = LS_BAD_SCN;
			TRACE("\nConnection BAD_SCN 1");
		}
	}
	else
	{
		ls = LS_BAD_SCN;
		TRACE("\nConnection BAD_SCN 2");
	}

	if( LS_SUCCESS != ls )
	{
		Cleanup();
	}

	return ls;
}

LinkStatus CRfCommExp::prc2LinkStatus(PORT_RETURN_CODE prc)
{
	LinkStatus ls = LS_GENERAL_FAILURE;

	switch( prc )
	{
    case CRfCommPort::SUCCESS:
		ls = LS_SUCCESS;
		break;
    case CRfCommPort::UNKNOWN_ERROR:
		ls = LS_GENERAL_FAILURE;
		break;
    case ALREADY_OPENED:         // Client tried to open port to existing DLCI/BD_ADDR
		ls = LS_ALREADY_OPENED;
		break;
    case HANDLE_ERROR:		    // Use of a bad connection handle within SDK
		ls = LS_HANDLE_ERROR;
		break;
    case NOT_OPENED:             // Function called before conn opened, or after closed
		ls = LS_NOT_OPENED;
		break;
    case LINE_ERR:               // Line error
		ls = LS_LINE_ERR;
		break;
    case START_FAILED:           // Connection attempt failed
		ls = LS_START_FAILED;
		break;
    case PAR_NEG_FAILED:         // Parameter negotiation failed, currently only MTU
		ls = LS_PAR_NEG_FAILED;
		break;
    case PORT_NEG_FAILED:        // Port negotiation failed
		ls = LS_PORT_NEG_FAILED;
		break;
    case PEER_CONNECTION_FAILED: // Connection ended by remote side
		ls = LS_PEER_CONNECTION_FAILED;
		break;
    case PEER_TIMEOUT:
		ls = LS_PEER_TIMEOUT;
		break;
	default:
		ls = LS_GENERAL_FAILURE;
		break;
	}

	return ls;
}

void CRfCommExp::Cleanup()
{
	RfPortClose();	// only does it if needed
}

void CRfCommExp::OnEventReceived (UINT32 event_code)
{
	if( m_WaitingEventReceived )
	{
		// we're waiting to find out if the connection completed ok
		if( event_code & PORT_EV_CONNECT_ERR )
		{
			m_Connected = FALSE;

			SetEvent( m_hConnectionEvent );

			m_WaitingEventReceived = false;
		}

		if( event_code & PORT_EV_CONNECTED )
		{
			m_Connected = TRUE;

			SetEvent( m_hConnectionEvent );

			m_WaitingEventReceived = false;
		}
	}
}

void CRfCommExp::RfPortWrite( BYTE* data, ULONG len )
{
	// perhaps we need to use a circular buffer here...not sure yet

	UINT16 written;
	PORT_RETURN_CODE prc;

	ULONG bytes_sent = 0;
	BYTE* data_to_send = data;

	const int iMaxValue = 0xFFFF; //(UINT16 max value)

	DWORD StartTime = ::GetTickCount();
	
	while(bytes_sent < len)
	{
		UINT16 iDataToSendSize = len - bytes_sent > iMaxValue ? iMaxValue : len - bytes_sent;

		prc = CRfCommPort::Write( data_to_send, iDataToSendSize, &written );
		ASSERT( CRfCommPort::SUCCESS == prc );
		ASSERT(written <= len - bytes_sent); 

		data_to_send += written;
		bytes_sent += written;


//#ifdef TIMED_OUT
		if(::GetTickCount() - StartTime > 5 * 1000)
		{	
			data = NULL;
	
			break;
		}
//#endif
	}

	ASSERT( bytes_sent == len );

#ifndef _DEBUG

//	FIX THIS!

#endif

	// should at least send an error if either we don't get success or we didn't
	// write all the bytes...
}


/*
void CRfCommExp::OnDataReceived (void *p_data, UINT16 len)
{
	OnRfPortDataRcvd( (BYTE*)p_data, len ); 
}

void CRfCommExp::OnClientStateChange( BD_ADDR bda, 
										DEV_CLASS dev_class, 
										BD_NAME name, short com_port, 
										SPP_STATE_CODE state)
{
	OnRfPortEvent( 0 );
}


*/