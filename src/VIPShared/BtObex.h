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
// BtObex.h: interface for the CBtObex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BTOBEX_H__46EFF81E_869B_4E73_A1F6_607422429C33__INCLUDED_)
#define AFX_BTOBEX_H__46EFF81E_869B_4E73_A1F6_607422429C33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WinBluetooth/inc/BtIfDefinitions.h"
#include "WinBluetooth/inc/BtIfClasses.h"


class CBtObexCallback
{
public:
	CBtObexCallback() {};
	virtual ~CBtObexCallback() {};

typedef enum
{
    OBEX__SEND_FAILED,
    OBEX__SEND_SUCCESS,
    OBEX__SEND_PROGRESS,
    OBEX__SEND_ABORTED,
    OBEX__SEND_NONE
} eObexEvents;

	virtual void OnObexEventReceived(  eObexEvents, long param1 = 0, long param2 = 0) = 0;
};

class CBtObex : public COppClient
{
public:
	CBtObex( CBtObexCallback * cbck );
	virtual ~CBtObex();

	OPP_RETURN_CODE Push(BD_ADDR bda,WCHAR *psz_path_name,CSdpDiscoveryRec & sdp_rec);

private:
	CBtObexCallback			*	m_Callback;

protected:
    // methods to replace virtual methods in base class COppClient 
	void OnProgress(OPP_RESULT_CODE result_code, BD_ADDR bda, WCHAR * string, long current, long total);
    void OnPushResponse(OPP_RESULT_CODE result_code,  BD_ADDR bda, WCHAR * string);
    void OnAbortResponse (OPP_RESULT_CODE result_code);
};

#endif // !defined(AFX_BTOBEX_H__46EFF81E_869B_4E73_A1F6_607422429C33__INCLUDED_)
