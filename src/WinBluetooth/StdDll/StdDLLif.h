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
// all dll exports are C functions

#define STD_DLL

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STD_DLL

#define STD_DLL_EXPORT _declspec( dllexport )

#else

#define STD_DLL_EXPORT _declspec( dllimport )

#endif

//STD_DLL_EXPORT unsigned int GoStd( bool bShowBack, bool bShowOK, LPTSTR strProductName );

#include "RfCommExp.h"

#define RFC__PORT_EV_RXCHAR			0x00000001   /* Any Character received */
#define RFC__PORT_EV_RXFLAG			0x00000002   /* Received certain character */
#define RFC__PORT_EV_TXEMPTY		0x00000004   /* Transmitt Queue Empty */
#define RFC__PORT_EV_CTS			0x00000008   /* CTS changed state */
#define RFC__PORT_EV_DSR			0x00000010   /* DSR changed state */
#define RFC__PORT_EV_RLSD			0x00000020   /* RLSD changed state */
#define RFC__PORT_EV_BREAK			0x00000040   /* BREAK received */
#define RFC__PORT_EV_ERR			0x00000080   /* Line status error occurred */
#define RFC__PORT_EV_RING			0x00000100   /* Ring signal detected */
#define RFC__PORT_EV_CTSS			0x00000400   /* CTS state */
#define RFC__PORT_EV_DSRS			0x00000800   /* DSR state */
#define RFC__PORT_EV_RLSDS			0x00001000   /* RLSD state */
#define RFC__PORT_EV_OVERRUN		0x00002000   /* receiver buffer overrun */
#define RFC__PORT_EV_TXCHAR			0x00004000   /* Any character transmitted */

#define RFC__PORT_EV_CONNECTED		0x00000200  /* RFCOMM connection established */
#define RFC__PORT_EV_CONNECT_ERR	0x00008000  /* Was not able to establish connection */
                                     /* or disconnected */
#define RFC__PORT_EV_FC				0x00010000   /* flow control enabled flag changed by remote */
#define RFC__PORT_EV_FCS			0x00020000   /* flow control status true = enabled */


class CRfCommTranspCallbackIf
{
public:
	CRfCommTranspCallbackIf() {};
	virtual ~CRfCommTranspCallbackIf() {};
	virtual void OnTranspEventReceived(  UINT32 event_code ) = 0;
	virtual void OnTranspDataReceived( BYTE* data, ULONG len ) = 0;
};

class STD_DLL_EXPORT CRfCommTransport : public CRfCommExp
{
public:
	CRfCommTransport( CRfCommTranspCallbackIf * callback );

	virtual ~CRfCommTransport();

	bool TransportOpen( const char *device );
	void TransportClose( void );
	void TransportWrite( BYTE* data, ULONG len );

	//virtual void OnTransportData( BYTE* data, ULONG len ) = 0; //{}
	//virtual void OnTransportEvent( UINT32 event_code ) {}
	//virtual void OnTransportDisconnected( void )  {}

protected:
	void OnDataReceived (void *p_data, UINT16 len);
	void OnEventReceived( UINT32 event_code );

	//void OnRfPortDataRcvd( BYTE* data, ULONG len );
	//void OnRfPortEvent( UINT32 event_code );
	//void OnRfPortDisconnected( void );
	//CRfCommExp		m_RfTransp;
private:
	CRfCommTranspCallbackIf *	m_CallBack;
};

#ifdef __cplusplus
}
#endif

