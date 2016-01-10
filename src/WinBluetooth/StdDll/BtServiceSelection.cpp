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
// BtServiceSelection.cpp: implementation of the CBtServiceSelection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdDLL.h"
#include "BtServiceSelection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG

#define DEBUGTRACE( x ) \
	do \
	{ \
		CString dbgstr;\
		dbgstr.Format x;\
		OutputDebugString( dbgstr );\
	}\
	while(0)

#else

#define DEBUGTRACE( x )

#endif

void GUID2CString( GUID* g, CString& str )
{
	BYTE* b = (BYTE*)g;

	str.Format( TEXT("{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x}"),
		b[0],
		b[1],
		b[2],
		b[3],

		b[4],
		b[5],

		b[6],
		b[7],

		b[8],
		b[9],

		b[10],
		b[11],
		b[12],
		b[13],
		b[14],
		b[15]
		);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD CBtServiceSelection::s_DiscoTimeout = 5000;

CBtServiceSelection::CBtServiceSelection()
{
	m_hDiscoCompleteEvent = CreateEvent
	(
		NULL,
		TRUE,
		FALSE,
		NULL
	);
	
	if( NULL == m_hDiscoCompleteEvent )
	{
		AfxThrowResourceException();
	}

	memset( m_Addr, 0, sizeof( BD_ADDR ) );

	m_nDiscoRecordsRead = 0;
	m_nDiscoSlots = 12;

	m_DiscoRecords = new CSdpDiscoveryRec[ m_nDiscoSlots ];

	m_ChosenService = NULL;
}

CBtServiceSelection::~CBtServiceSelection()
{
	TRACE("\nDecon CBtServiceSelection");
	delete[] m_DiscoRecords;
}

BOOL CBtServiceSelection::StartDiscovery (BD_ADDR p_bda, GUID *p_service_guid)
{
	ResetEvent( m_hDiscoCompleteEvent );

	return CBtIf::StartDiscovery( p_bda, p_service_guid );
}

void CBtServiceSelection::OnDiscoveryComplete()
{
	SetEvent( m_hDiscoCompleteEvent );
}

void CBtServiceSelection::OnDeviceResponded (BD_ADDR bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected) 
{
}

void CBtServiceSelection::OnInquiryComplete (BOOL success, short num_responses) 
{
}

void CBtServiceSelection::WaitForDiscoToComplete()
{
	WaitForSingleObject( m_hDiscoCompleteEvent, s_DiscoTimeout );
}

void CBtServiceSelection::SetBthAddr(BD_ADDR addr)
{
	memcpy( m_Addr, addr, sizeof( BD_ADDR ) );
}


LinkStatus CBtServiceSelection::Select( const CSdpDiscoveryRec** chosenRecord )
{
	BOOL fRes;
	LinkStatus ls = LS_GENERAL_FAILURE;

	DEBUGTRACE( (TEXT("About to start discovery\n") ) );

	fRes = this->StartDiscovery( m_Addr, NULL );

	if( !fRes )
	{
		ls = LS_DISCO_START_FAILURE;

		goto exit;
	}

	this->WaitForDiscoToComplete();

	DEBUGTRACE( (TEXT("Discovery complete....scanning service records\n") ) );

	while( 1 )
	{
		m_nDiscoRecordsRead = ReadDiscoveryRecords
		( 
			m_Addr, 
			m_nDiscoSlots, 
			m_DiscoRecords, 
			NULL 
		);

		if( m_nDiscoRecordsRead < m_nDiscoSlots )
		{
			break; // we had enough space
		}

		// increase the space

		m_nDiscoSlots += 5;

		delete[] m_DiscoRecords;

		m_DiscoRecords = new CSdpDiscoveryRec[ m_nDiscoSlots ];
	}

	DEBUGTRACE( (TEXT("Found %d service records\n"), m_nDiscoRecordsRead) );

	if( 0 == m_nDiscoRecordsRead )
	{
		ls = LS_ZERO_DISCO_RECORDS;
		
		goto exit;
	}

	PickService();

	if( !m_ChosenService )
	{
		ls = LS_NO_SUITABLE_SERVICE;

		goto exit;
	}

	*chosenRecord = m_ChosenService;

	ls = LS_SUCCESS;

exit:
	return ls;
}

void CBtServiceSelection::PickService()
{
	m_ChosenService = NULL;

	if( 0 == m_nDiscoRecordsRead )
	{
		// oh dear

		return;
	}

	int i;
	int nBestRank = 0;
	int nRank;

	for( i = 0; i < m_nDiscoRecordsRead; i++ )
	{
#ifdef _DEBUG
		CString otherstr;
		DEBUGTRACE(( TEXT("\n\nService %d\n"), i ));
		//DEBUGTRACE(( TEXT("Name: %S\n"), m_DiscoRecords[i].m_service_name ));
		TRACE( m_DiscoRecords[i].m_service_name );
		GUID2CString( &m_DiscoRecords[i].m_service_guid, otherstr );
		DEBUGTRACE(( TEXT("GUID: %s\n"), otherstr ));
#endif // _DEBUG

		nRank = RankService( &m_DiscoRecords[i] );

		if( nRank > nBestRank )
		{
			nBestRank = nRank;
			m_ChosenService = &m_DiscoRecords[i];
		}
	}
}

int CBtServiceSelection::RankService(CSdpDiscoveryRec *rec)
{
	// here are the rules we'll use (maybe a bit arbitrary)

	// 0. If the SERVICE_CLASS_ID_LIST includes SERVCLASS_SERIAL_PORT with the
	//    service name Pluto this is best

	// 1. If the SERVICE_CLASS_ID_LIST includes SERVCLASS_SERIAL_PORT this is best
	//    (because this is the service BT mobile will use in preference, I think)

	// 2. If the SERVICE_CLASS_ID_LIST includes SERVCLASS_DIALUP_NETWORKING this
	//    is second best (eg. nokia 6210 will do this)

	// Actually, that'll do for now.

	SDP_DISC_ATTTR_VAL attr;
	BOOL fRes;
	int nRank = 0;
	int i;

	BOOL fFound_ClassSerialPort = FALSE;
	BOOL fFound_ClassDialupNetworking = FALSE;

	fRes = rec->FindAttribute( ATTR_ID_SERVICE_CLASS_ID_LIST, &attr );

	if( fRes )
	{
		for( i = 0; i < attr.num_elem; i++ )
		{
			if( 
				(2 == attr.elem[i].len) &&
				(ATTR_TYPE_UUID == attr.elem[i].type)
			  )
			{
				if( UUID_SERVCLASS_SERIAL_PORT == attr.elem[i].val.u16 )
				{
					fFound_ClassSerialPort = TRUE;
				}
				else
				if( UUID_SERVCLASS_DIALUP_NETWORKING == attr.elem[i].val.u16 )
				{
					fFound_ClassDialupNetworking = TRUE;
				}
			}
		}
	}

	CString serviceName;
	serviceName = rec->m_service_name;

	int nNokia = serviceName.Find(_T("Nokia PC Suite"));
	int nPluto = serviceName.Find(_T("Pluto"));

	//s.Format("%s\t%s", p_si[i].m_service_name, "");
	if(( fFound_ClassSerialPort ) && (nPluto != -1))//
	{
		nRank = 15;
	}
	else 
	if(( fFound_ClassSerialPort ) && (nNokia==-1))//
	{
		nRank = 10;
	}
	else
	if( fFound_ClassDialupNetworking )
	{
		nRank = 5;
	}

	return nRank;
}
