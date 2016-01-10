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
/*
 * StdDLLif.CPP
 *
 * This file defines the exported functions
 */

// include files

#include "StdAfx.h"
#include "StdDLLif.h"
#include "StdDLL.h"
 
extern "C" STD_DLL_EXPORT unsigned int GoStd
( 
	bool bShowBackOnFirstDlg, //in
	bool bShowOKOnLastDlg, //in
	LPTSTR strProductName	//in
)
{
	try
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		//stuff();
		return 0;
	}
	catch( CMemoryException* e )
	{
		MessageBox( NULL, "Out-of-memory error!", "Std", MB_OK );

		e->Delete();
	}
	catch( CFileException* e )
	{
		MessageBox( NULL, "Unexpected file error!", "Std", MB_OK );

		e->Delete();
	}
	catch( CResourceException* e )
	{
		MessageBox( NULL, "Out-of-resources error!", "Std", MB_OK );

		e->Delete();
	}
	catch( CException* )
	{
		MessageBox( NULL, "Unknown error condition!", "Std", MB_OK );

		throw;
	}

	return 0;
}

extern "C" STD_DLL_EXPORT CRfCommTransport::CRfCommTransport(CRfCommTranspCallbackIf * callback )
{
	m_CallBack = callback;

//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("Make it");
}

extern "C" STD_DLL_EXPORT CRfCommTransport::~CRfCommTransport()
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("Kill it");
	Close();
}


extern "C" STD_DLL_EXPORT bool CRfCommTransport::TransportOpen( /*const CString& device*/ const char* device )
{
	return RfPortOpen( device );
}

extern "C" STD_DLL_EXPORT void CRfCommTransport::TransportClose( )
{
	RfPortClose();
}

extern "C" STD_DLL_EXPORT void CRfCommTransport::TransportWrite( BYTE* data, ULONG len )
{
	RfPortWrite( data, len );
}

void CRfCommTransport::OnDataReceived (void *p_data, UINT16 len)
{
	m_CallBack->OnTranspDataReceived( (BYTE*)p_data, len ); 
}

void CRfCommTransport::OnEventReceived( UINT32 event_code )
{
	if( CRfCommExp::m_WaitingEventReceived )
	{
		CRfCommExp::OnEventReceived (event_code);
	}
	else
	{
		m_CallBack->OnTranspEventReceived( event_code );
	}
}

/*
extern "C" STD_DLL_EXPORT void CRfCommTransport::OnRfPortEvent( UINT32 event_code )
{
	m_CallBack->OnTranspEventReceived( event_code );
}

extern "C" STD_DLL_EXPORT void CRfCommTransport::OnRfPortDisconnected( void )
{
	OnTransportDisconnected();
}
*/
