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
// BondedDevices.h: interface for the CBondedDevices class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BONDEDDEVICES_H__EE0E8196_D3F0_4CD6_9CC3_E7B97D2F8DD2__INCLUDED_)
#define AFX_BONDEDDEVICES_H__EE0E8196_D3F0_4CD6_9CC3_E7B97D2F8DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BtIfDefinitions.h"

class CBondedDeviceInfo
{
public:
	BOOL IsUnknown() const;
	void Init();
	void DeepCopy( const CBondedDeviceInfo* di );
	BOOL IsCellphone() const;
	CBondedDeviceInfo();

	CString textaddr;
	BD_ADDR addr;
	BD_NAME name;
	DEV_CLASS devclass;
};

class CBondedDevices  
{
public:
	UINT GetCount();
	void Refresh();
	CBondedDevices();
	virtual ~CBondedDevices();
	const CBondedDeviceInfo* GetAt( UINT index );
	static BOOL ConvertAddr( const TCHAR* text, BD_ADDR addr );

protected:
	CList<CBondedDeviceInfo*,CBondedDeviceInfo*> m_BondedDevList;
	static BOOL hexPair( const TCHAR* text, BYTE* val );
	void DeleteAll();
};

#endif // !defined(AFX_BONDEDDEVICES_H__EE0E8196_D3F0_4CD6_9CC3_E7B97D2F8DD2__INCLUDED_)
