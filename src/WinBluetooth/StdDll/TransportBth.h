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
// TransportBth.h: interface for the CTransportBth class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPORTBTH_H__4CF1A1AB_AD2F_4672_A102_EE8305DB27E5__INCLUDED_)
#define AFX_TRANSPORTBTH_H__4CF1A1AB_AD2F_4672_A102_EE8305DB27E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\TRANSP\Transport.h"
#include "ServiceSelect.h"	// Added by ClassView

class CTransportBth : public CTransport, public CRfCommIf, public CRfCommPort
{
public:
	CTransportBth( CTransportCallback* callback );
	virtual ~CTransportBth( );
	virtual bool TransportOpen( const CString& device );
	virtual void TransportClose( );
	virtual void TransportWrite( BYTE* data, ULONG len );
	virtual void OnDataReceived (void *p_data, UINT16 len);
	virtual void OnEventReceived (UINT32 event_code);
	virtual void TransportGetDevices( DeviceList& deviceList );
	virtual void OnClientStateChange( BD_ADDR bda, 
										DEV_CLASS dev_class, 
										BD_NAME name, short com_port, 
										SPP_STATE_CODE state);

	enum Xcept
	{
		X_RESOURCE
	};

protected:
	CTransportCallback* m_Callback;
	void Cleanup();
	LinkStatus prc2LinkStatus( PORT_RETURN_CODE prc );
	BD_ADDR m_bd_addr;
	LinkStatus ConnectToService( const CSdpDiscoveryRec* service );
	CServiceSelect m_ServiceSelect;
private:
	BOOL m_Connected;
	HANDLE m_hConnectionEvent;
	static DWORD s_ConnectionTimeout;
	BOOL m_WaitingEventReceived;
};

#endif // !defined(AFX_TRANSPORTBTH_H__4CF1A1AB_AD2F_4672_A102_EE8305DB27E5__INCLUDED_)
