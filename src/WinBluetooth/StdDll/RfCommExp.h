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
// RfCommExp.h: interface for the CRfCommExp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFCOMMEXP_H__A36F867B_930C_47D2_AF4A_464D2B597055__INCLUDED_)
#define AFX_RFCOMMEXP_H__A36F867B_930C_47D2_AF4A_464D2B597055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../inc/BtIfDefinitions.h"
#include "../inc/BtIfClasses.h"
#include "BtServiceSelection.h"

class STD_DLL_EXPORT CRfCommExp : public CRfCommIf, public CRfCommPort
{
public:
	CRfCommExp();
	virtual ~CRfCommExp();

	int CRfCommExp::foo();

	bool RfPortOpen( const char *device );
	void RfPortClose( );
	void RfPortWrite( BYTE* data, ULONG len );

	//virtual void OnRfPortDataRcvd( BYTE* data, ULONG len ) = 0;
	//virtual void OnRfPortEvent( UINT32 event_code ) {} //= 0;
	//virtual void OnRfPortDisconnected( void ) {} //= 0;

	enum Xcept
	{
		X_RESOURCE
	};

	//void OnDataReceived (void *p_data, UINT16 len);
	void OnEventReceived (UINT32 event_code);
	//void OnClientStateChange( BD_ADDR bda, 
	//							DEV_CLASS dev_class, 
	//							BD_NAME name, short com_port, 
	//							SPP_STATE_CODE state);

protected:
	void					Cleanup();
	LinkStatus				prc2LinkStatus( PORT_RETURN_CODE prc );
	BD_ADDR					m_bd_addr;
	LinkStatus				ConnectToService( const CSdpDiscoveryRec* service );
	CBtServiceSelection		m_ServiceSelect;
private:
	BOOL					ConvertAddr(const TCHAR* intext, BD_ADDR addr);
	BOOL					hexPair(const TCHAR *text, BYTE *val);
	BOOL					m_Connected;
	HANDLE					m_hConnectionEvent;
	static DWORD			s_ConnectionTimeout;
public:
	BOOL					m_WaitingEventReceived;
};




#endif // !defined(AFX_RFCOMMEXP_H__A36F867B_930C_47D2_AF4A_464D2B597055__INCLUDED_)
