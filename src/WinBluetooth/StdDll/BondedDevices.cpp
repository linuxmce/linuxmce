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
// BondedDevices.cpp: implementation of the CBondedDevices class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BondedDevices.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBondedDeviceInfo::CBondedDeviceInfo()
{
	Init();
}

CBondedDevices::CBondedDevices()
{
}

CBondedDevices::~CBondedDevices()
{
	DeleteAll();
}

void CBondedDevices::Refresh()
{
	DeleteAll();

	LONG lRes;
	HKEY hDevices;

	lRes = RegOpenKeyEx
	(
		HKEY_LOCAL_MACHINE,
		TEXT("Software\\Widcomm\\BTConfig\\Devices"),
		0,
		KEY_READ,
		&hDevices
	);

	if( ERROR_SUCCESS == lRes )
	{
		// each bonded device has a subkey

		DWORD dwIndex = 0;
		TCHAR keyName[18]; // exactly enough room for the BT addr string
		DWORD dwSize;
		BD_ADDR addr;
		BD_NAME devName; //definitely bigger than the biggest legal names
		HKEY hEntry;
		DWORD dwType;
		DEV_CLASS devclass;

		while( 1 )
		{
			dwSize = sizeof( keyName );

			memset( keyName, 0, sizeof( keyName ) );

			lRes = RegEnumKeyEx
			(
				hDevices,
				dwIndex,
				keyName,
				&dwSize,
				NULL,
				NULL,
				NULL,
				NULL
			);

			if( ERROR_NO_MORE_ITEMS == lRes )
			{
				break;
			}

			if( (ERROR_SUCCESS == lRes)&& ConvertAddr( keyName, addr ) )
			{
				lRes = RegOpenKeyEx
				(
					hDevices,
					keyName,
					0,
					KEY_READ,
					&hEntry
				);

				if( ERROR_SUCCESS == lRes )
				{
					// read in the name of the device

					dwSize = sizeof( devName );

					lRes = RegQueryValueEx
					(
						hEntry,
						TEXT("Name"),
						NULL,
						&dwType,
						(BYTE*)devName,
						&dwSize
					);

					if( ERROR_SUCCESS != lRes )
					{
						goto SkipIt;
					}

					//if( REG_SZ != dwType )
					//{
					//	goto SkipIt;
					//}

					// device name is stored as string on 2k, and binary on 9x
					// so just check for null termination

					if( (dwSize == 0) || (devName[dwSize-1] != '\0') )
					{
						goto SkipIt;
					}

					// check it's bonded

					dwSize = 0;

					lRes = RegQueryValueEx
					(
						hEntry,
						TEXT("LinkKey"),
						NULL,
						NULL,
						NULL,
						&dwSize
					);

					if( ERROR_SUCCESS != lRes )
					{
						goto SkipIt;
					}

					dwSize = sizeof( devclass );

					lRes = RegQueryValueEx
					(
						hEntry,
						TEXT("DevClass"),
						NULL,
						NULL,
						(BYTE*)devclass,
						&dwSize
					);

					if( ERROR_SUCCESS != lRes )
					{
						goto SkipIt;
					}

					if( DEV_CLASS_LEN != dwSize )
					{
						goto SkipIt;
					}

#define SINCLAIRS_MODS_INCLUDED 1 
#ifdef SINCLAIRS_MODS_INCLUDED
					if ( !
		( ((devclass[ 1 ] & 0x1F) == MAJOR_DEV_CLASS_PHONE )
			&& (devclass[ 2 ] & MINOR_DEV_CLASS_PHONE_CELLULAR) )
						
						)
					{
						goto SkipIt;
					}
#endif
					{
						CBondedDeviceInfo* di = new CBondedDeviceInfo;
						memcpy( di->addr, addr, BD_ADDR_LEN );
						memcpy( di->name, devName, BD_NAME_LEN );
						memcpy( di->devclass, devclass, DEV_CLASS_LEN );

						di->textaddr = keyName;

						di->textaddr.MakeUpper();

						m_BondedDevList.AddTail( di );
					}
SkipIt:

					RegCloseKey( hEntry );
				}
				
			}

			dwIndex++;
		}


		RegCloseKey( hDevices );
	}
}

void CBondedDevices::DeleteAll()
{
	while( !m_BondedDevList.IsEmpty() )
	{
		delete m_BondedDevList.RemoveHead();
	}
}

BOOL CBondedDevices::ConvertAddr(const TCHAR* intext, BD_ADDR addr)
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

BOOL CBondedDevices::hexPair(const TCHAR *text, BYTE *val)
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

BOOL CBondedDeviceInfo::IsCellphone() const
{
		return ( ((devclass[ 1 ] & 0x1F) == MAJOR_DEV_CLASS_PHONE )
			&& (devclass[ 2 ] & MINOR_DEV_CLASS_PHONE_CELLULAR) );
}

void CBondedDeviceInfo::DeepCopy(const CBondedDeviceInfo *di)
{
	memcpy( this->addr, di->addr, BD_ADDR_LEN );
	memcpy( this->devclass, di->devclass, DEV_CLASS_LEN );
	memcpy( this->name, di->name, BD_NAME_LEN );

	this->textaddr = di->textaddr;
}

void CBondedDeviceInfo::Init()
{
	memset( addr, 0, BD_ADDR_LEN );
	memset( devclass, 0, DEV_CLASS_LEN );
	memset( name, 0, BD_NAME_LEN );

	textaddr.Empty();
}

BOOL CBondedDeviceInfo::IsUnknown() const
{
	BOOL fRet = FALSE;
		
	if( (devclass[ 1 ] & 0x1F) == MAJOR_DEV_CLASS_MISC )
	{
		fRet = TRUE;
	}

	if( (devclass[ 1 ] & 0x1F) == MAJOR_DEV_CLASS_UNSPECIFIED )
	{
		fRet = TRUE;
	}

	return fRet;
}

UINT CBondedDevices::GetCount()
{
	return m_BondedDevList.GetCount();
}

const CBondedDeviceInfo* CBondedDevices::GetAt(UINT index)
{
	return m_BondedDevList.GetAt( m_BondedDevList.FindIndex( index ) );
}
