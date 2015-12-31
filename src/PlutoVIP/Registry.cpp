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
// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Registry.h"
#include "PlutoRegistryKeyDef.h"

//Shell API is required for SHDeleteKey used in RemoveEntireKey
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
 *	Here is an example of how to use CRegistry class

	#include "Registry.h"
	#include "PlutoRegistryKeyDef.h"

	CRegistry m_Registry;
	HKEY hKey=m_Registry.OpenKey(HKEY_LOCAL_MACHINE, REGISTRY_KEY_VIPSERVICE_FULL);

	//if the key does not exist, hKey=NULL!
	if (hKey==NULL)
	{
		DWORD dwDisp=0;
		m_Registry.CreateNewKey(HKEY_LOCAL_MACHINE, 
			REGISTRY_KEY_VIPSERVICE_FULL, &dwDisp);
	}
	
	  m_Registry.SetValueString("Test String", "String");
	  m_Registry.SetValueString("Test", "Test");
	  m_Registry.SetValueDWORD("Test DWORD", (DWORD) 25);
	  m_Registry.DeleteValue("Test");
	  m_Registry.CloseKey();
 */
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
	m_Key=NULL;
}

CRegistry::~CRegistry()
{
	if (m_Key!=NULL)
		RegCloseKey(m_Key);
}

HKEY CRegistry::OpenKey(HKEY hParent=HKEY_LOCAL_MACHINE, 
						CString szSubKey=REGISTRY_KEY_VIPSERVICE_FULL)
{
	//if key was opened previously
	if (m_Key!=NULL)
		RegCloseKey(m_Key);

	LONG ReturnValue=RegOpenKeyEx(hParent, szSubKey, 0, KEY_ALL_ACCESS, &m_Key);
	if (ReturnValue!=ERROR_SUCCESS)
		m_Key=NULL;

	return m_Key;
}


HKEY CRegistry::CreateNewKey(HKEY hParent, CString szSubKey, LPDWORD lpdwDisposition)
{
	/*lpdwDisposition: Pointer to a variable that receives one of the following 
	disposition values:

		REG_CREATED_NEW_KEY: The key did not exist and was created. 
		REG_OPENED_EXISTING_KEY: The key existed and 
								 was simply opened without being changed. 

  		If lpdwDisposition is NULL, no disposition information is returned.
	*/
	
	RegCreateKeyEx(hParent, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE,
					KEY_ALL_ACCESS, NULL, &m_Key, lpdwDisposition);

	return m_Key;
}


void CRegistry::CloseKey()
{
	if (m_Key!=NULL)
	{
		RegCloseKey(m_Key);
		m_Key=NULL;
	}
}



void CRegistry::DeleteKey(CString szSubKey)
{
	/*The subkey to be deleted must not have subkeys. 
	To delete a key and all its subkeys, you need to recursively enumerate the 
	subkeys and delete them individually. 
	To recursively delete keys, use the SHDeleteKey function*/
	ASSERT(m_Key!=NULL);
	RegDeleteKey(m_Key, szSubKey);
}

void CRegistry::RemoveEntireKey(CString szSubKey)
{
	ASSERT(m_Key!=NULL);
	SHDeleteKey(m_Key, szSubKey);
}


void CRegistry::DeleteValue(CString szValue)
{
	ASSERT(m_Key!=NULL);
	RegDeleteValue(m_Key, szValue);
}


//The programmer is responsible for allocating enough memory for saving 
//the key name. lpdwName is number of elements in szName array
LONG CRegistry::EnumerateKey(DWORD dwIndex, TCHAR* szName, LPDWORD lpdwName)
{
	ASSERT(m_Key!=NULL);

	FILETIME ft;
	LONG ReturnValue=RegEnumKeyEx(m_Key, dwIndex, szName, lpdwName, 0, NULL, 0, &ft);

	return ReturnValue;
}


//The programmer is responsible for allocating enough memory for getting 
//value name and data
LONG CRegistry::EnumerateValue(DWORD dwIndex, TCHAR* szName, 
							   LPDWORD lpdwName, LPDWORD lpdwType, 
							   LPBYTE lpData, LPDWORD lpdwData)
{
	/* Registry Types:
	Value			Meaning 
	REG_BINARY					Binary data in any form. 
	REG_DWORD					A 32-bit number. 
	REG_DWORD_LITTLE_ENDIAN		A 32-bit number in little-endian format. 
	REG_DWORD_BIG_ENDIAN		A 32-bit number in big-endian format. 
	REG_EXPAND_SZ				Null-terminated string that contains unexpanded 
								references to environment variables 
								(for example, "%PATH%"). It will be a Unicode or ANSI 
								string depending on whether you use the Unicode or 
								ANSI functions. To expand the environment variable 
								references, use the ExpandEnvironmentStrings function.
	REG_LINK					Reserved for system use. 
	REG_MULTI_SZ				Array of null-terminated strings, terminated by two 
								null characters. 
	REG_NONE					No defined value type. 
	REG_QWORD					A 64-bit number. 
	REG_QWORD_LITTLE_ENDIAN		A 64-bit number in little-endian format. 	
	REG_SZ						Null-terminated string. It will be a Unicode or ANSI 
								string, depending on whether you use the Unicode or 
								ANSI functions. 		  
	*/
	ASSERT(m_Key!=NULL);
	LONG ReturnValue=RegEnumValue(m_Key, dwIndex, szName, lpdwName, 0,
									lpdwType, lpData, lpdwData);

	return ReturnValue;
}


//The programmer is responsible for allocating enough memory for getting 
//value name and data
LONG CRegistry::QueryValue(CString szValue, LPDWORD lpdwType, 
						LPBYTE lpData, LPDWORD lpdwData)
{
	ASSERT(m_Key!=NULL);
	return (RegQueryValueEx(m_Key, szValue, 0, lpdwType, lpData, lpdwData));
}


void CRegistry::SetValue(CString szValue, DWORD dwType, 
						 const LPBYTE lpData, DWORD dwData)
{
	ASSERT(m_Key!=NULL);
	RegSetValueEx(m_Key, szValue, 0, dwType, lpData, dwData);
}


void CRegistry::SetValueString(CString szValue, CString szData)
{
	SetValue(szValue, REG_SZ, (LPBYTE) szData.GetBuffer(0), szData.GetLength());
}


void CRegistry::SetValueDWORD(CString szValue, DWORD dwData)
{
	SetValue(szValue, REG_DWORD, (LPBYTE) &dwData, sizeof(DWORD));
}



void CRegistry::SetValueBinary(CString szValue, const LPBYTE lpData, DWORD dwData)
{
	SetValue(szValue, REG_BINARY, lpData, dwData);
}


CString CRegistry::GetValueString(CString szValueName)
{
	TCHAR temp[200];
	ZeroMemory(temp, sizeof(temp));
	DWORD tempSize=sizeof(temp)/sizeof(TCHAR);
	DWORD ValueType=REG_SZ;
	RegQueryValueEx(m_Key, szValueName, 0, &ValueType, 
		(BYTE*) temp, &tempSize);

	return temp;
}

DWORD CRegistry::GetValueDWORD(CString szValueName)
{
	DWORD temp=0;
	DWORD tempSize=sizeof(DWORD);
	DWORD ValueType=REG_DWORD;
	RegQueryValueEx(m_Key, szValueName, 0, &ValueType, 
		(BYTE*) &temp, &tempSize);

	return temp;
}

BYTE* CRegistry::GetValueBinary(CString szValueName, 
								LPBYTE pValue, 
								LPDWORD pValueSize)
{
	DWORD ValueType=REG_BINARY;
	RegQueryValueEx(m_Key, szValueName, 0, &ValueType, pValue, pValueSize);

	return pValue;
}

void CRegistry::GetValue(CString szValueName, 
						 LPDWORD pType, 
						 LPBYTE pValue, 
						 LPDWORD pValueSize)
{
	RegQueryValueEx(m_Key, szValueName, 0, pType, pValue, pValueSize);
}

BOOL CRegistry::IsOpen()
{
	return (m_Key!=NULL ? TRUE : FALSE);
}
