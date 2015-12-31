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
// ServiceSelect.h: interface for the CServiceSelect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVICESELECT_H__EB6BB31E_D439_42AD_AEE9_5064DD732934__INCLUDED_)
#define AFX_SERVICESELECT_H__EB6BB31E_D439_42AD_AEE9_5064DD732934__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BtIfDefinitions.h"
#include "BtIfClasses.h"

enum LinkStatus
{
	LS_SUCCESS,
	LS_GENERAL_FAILURE,
	LS_INVALID_BT_ADDR,
	LS_DISCO_START_FAILURE,
	LS_ZERO_DISCO_RECORDS,
	LS_NO_SUITABLE_SERVICE,
	LS_BAD_SCN,
	LS_BAD_SET_SECURITY,
    LS_UNKNOWN_ERROR,
	LS_CONNECTION_COMPLETION_FAILED,
    LS_ALREADY_OPENED,         // Client tried to open port to existing DLCI/BD_ADDR
    LS_HANDLE_ERROR,		    // Use of a bad connection handle within SDK
    LS_NOT_OPENED,             // Function called before conn opened, or after closed
    LS_LINE_ERR,               // Line error
    LS_START_FAILED,           // Connection attempt failed
    LS_PAR_NEG_FAILED,         // Parameter negotiation failed, currently only MTU
    LS_PORT_NEG_FAILED,        // Port negotiation failed
    LS_PEER_CONNECTION_FAILED, // Connection ended by remote side
    LS_PEER_TIMEOUT
};

class CServiceSelect : public CBtIf 
{
public:
	LinkStatus Select( const CSdpDiscoveryRec** chosenRecord );
	CServiceSelect();
	virtual ~CServiceSelect();
	void SetBthAddr( BD_ADDR );
	virtual void OnDiscoveryComplete();
    virtual void OnDeviceResponded (BD_ADDR bda, DEV_CLASS devClass, BD_NAME bdName, BOOL bConnected);
    virtual void OnInquiryComplete (BOOL success, short num_responses);
    BOOL StartDiscovery (BD_ADDR p_bda, GUID *p_service_guid);

protected:
	int RankService(CSdpDiscoveryRec *rec);
	CSdpDiscoveryRec* m_ChosenService;
	void WaitForDiscoToComplete();
	HANDLE m_hDiscoCompleteEvent;
	BD_ADDR m_Addr;
	int m_nDiscoRecordsRead;
	int m_nDiscoSlots;
	CSdpDiscoveryRec* m_DiscoRecords;
	void PickService();
	static DWORD s_DiscoTimeout;

};

#endif // !defined(AFX_SERVICESELECT_H__EB6BB31E_D439_42AD_AEE9_5064DD732934__INCLUDED_)
