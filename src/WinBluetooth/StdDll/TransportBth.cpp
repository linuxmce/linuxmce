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
// TransportBth.cpp: implementation of the CTransportBth class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "transp_bth.h"
#include "TransportBth.h"
#include "BondedDevices.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) CTransport* CreateTransportInstance( CTransportCallback* callback )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTransport* tr;

	try
	{
		tr = new CTransportBth( callback );
	}
	catch( CTransportBth::Xcept )
	{
		tr = NULL;
	}
	catch( ... )
	{
		tr = NULL;
	}

	return tr;
}

DWORD CTransportBth::s_ConnectionTimeout = 5000;

CTransportBth::CTransportBth( CTransportCallback* callback )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_Callback = callback;

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

CTransportBth::~CTransportBth()
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Cleanup();

	// assume we can't get any more callbacks => ok to delete our event...

	CloseHandle( m_hConnectionEvent );
}

bool CTransportBth::TransportOpen( const CString& device )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const CSdpDiscoveryRec* service;
	LinkStatus ls;

	BOOL fRes = CBondedDevices::ConvertAddr( device, m_bd_addr );

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

void CTransportBth::TransportClose( )
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRfCommPort::Close();
}

LinkStatus CTransportBth::ConnectToService(const CSdpDiscoveryRec *service)
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

				// not really sure when to do this set, so...
				// do it before

				//(void)SetModemSignal( PORT_SET_DTRDSR );
				//(void)SetModemSignal( PORT_SET_CTSRTS );
				//(void)SetModemSignal( PORT_SET_DCD );
				//(void)SetFlowEnabled( TRUE );

				m_WaitingEventReceived = TRUE;

				prc = OpenClient( scn, m_bd_addr );

				// and after...

				//(void)SetModemSignal( PORT_SET_DTRDSR );
				//(void)SetModemSignal( PORT_SET_CTSRTS );
				//(void)SetModemSignal( PORT_SET_DCD );
				//(void)SetFlowEnabled( TRUE );

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
							ls = LS_CONNECTION_COMPLETION_FAILED;
						}
					}
					else
					{
						ls = LS_CONNECTION_COMPLETION_FAILED;
					}
				}
			}
			else
			{
				ls = LS_BAD_SET_SECURITY;
			}
		}
		else
		{
			ls = LS_BAD_SCN;
		}
	}
	else
	{
		ls = LS_BAD_SCN;
	}

	if( LS_SUCCESS != ls )
	{
		Cleanup();
	}

	return ls;
}

LinkStatus CTransportBth::prc2LinkStatus(PORT_RETURN_CODE prc)
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

void CTransportBth::Cleanup()
{
	TransportClose();	// only does it if needed
}

void CTransportBth::TransportWrite( BYTE* data, ULONG len )
{
	// perhaps we need to use a circular buffer here...not sure yet

	UINT16 written;
	PORT_RETURN_CODE prc;

	prc = CRfCommPort::Write( data, (UINT16)len, &written );

	ASSERT( written == len );
	ASSERT( CRfCommPort::SUCCESS == prc );

#ifndef _DEBUG

//	FIX THIS!

#endif

	// should at least send an error if either we don't get success or we didn't
	// write all the bytes...
}

void CTransportBth::OnDataReceived (void *p_data, UINT16 len)
{
	m_Callback->TransportDataReceived( (BYTE*)p_data, len );
}

void CTransportBth::OnEventReceived (UINT32 event_code)
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
	else
	{
		if( event_code & PORT_EV_CONNECT_ERR )
		{
			m_Callback->TransportDisconnected();
		}
	}
}

void CTransportBth::TransportGetDevices( DeviceList& deviceList )
{
	CDeviceInfo di;

	CBondedDevices bd;

	bd.Refresh();

	for( UINT i = 0; i < bd.GetCount(); ++i )
	{
		di.m_Description1 = bd.GetAt(i)->name;
		di.m_Description2 = bd.GetAt(i)->textaddr;
		di.m_DeviceId = CString( "BTH:" ) + bd.GetAt(i)->textaddr;

		deviceList.Add( di );
	}

}
void CTransportBth::OnClientStateChange( BD_ADDR bda, 
										DEV_CLASS dev_class, 
										BD_NAME name, short com_port, 
										SPP_STATE_CODE state)
{

}
