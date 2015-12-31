/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "IncomingCallNotifier.h"
#include "OrbiterApp.h"

#include "DCE/Logger.h"
using namespace DCE;
//---------------------------------------------------------------------------------------------------------
#define TAPI_API_LOW_VERSION    0x00020000
#define TAPI_API_HIGH_VERSION   0x00020000
#define EXT_API_LOW_VERSION     0x00010000
#define EXT_API_HIGH_VERSION    0x00010000
#define CELLTSP_LINENAME_STRING (L"Cellular Line")
#define MAX_LOADSTRING 100
//---------------------------------------------------------------------------------------------------------
VOID CALLBACK lineCallbackFunc (DWORD hDevice, DWORD dwMsg, DWORD dwCallbackInstance, DWORD dwParam1,
	DWORD dwParam2, DWORD dwParam3)
{
	switch (dwMsg)
	{
		case LINE_APPNEWCALL:
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LINE_APPNEWCALL"); 
			OrbiterApp::GetInstance()->Hide();
			break;
		}
			
		/*
		case LINE_REPLY:	
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LINE_REPLY"); 
			break;
		}
			
		case LINE_CALLSTATE:	
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LINE_CALLSTATE"); 
			break;
		}
			
		case LINECALLSTATE_CONNECTED:	
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LINECALLSTATE_CONNECTED"); 
			break;
		}
			
		case LINECALLSTATE_DISCONNECTED:	
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LINECALLSTATE_DISCONNECTED"); 
			break;
		}

		case LINE_CALLINFO:
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "LINE_CALLINFO"); 
			break;
		} */

		default:
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "unknown msg for incoming message"); 
	}
}
//---------------------------------------------------------------------------------------------------------
IncomingCallNotifier::IncomingCallNotifier()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Initializing TAPI"); 
	
	DWORD dwReturn = InitializeTAPI();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Initialized TAPI, return code %d", dwReturn); 
}
//---------------------------------------------------------------------------------------------------------
IncomingCallNotifier::~IncomingCallNotifier()
{
}
//---------------------------------------------------------------------------------------------------------
DWORD IncomingCallNotifier::InitializeTAPI()
{
	DWORD dwReturn = lineInitialize (&m_hLineApp, OrbiterApp::GetInstance()->m_hInstance, 
		(LINECALLBACK) lineCallbackFunc, TEXT("orbiter notifier"), &m_dwNumDevs);

	if(dwReturn != 0)
		return dwReturn;

	DWORD dwMediaMode = LINEMEDIAMODE_INTERACTIVEVOICE | LINEMEDIAMODE_DATAMODEM;
	DWORD dwTAPIVer = 0x00020000;
	DWORD dwDevId = SelectLine(m_hLineApp, m_dwNumDevs, TAPI_API_LOW_VERSION, TAPI_API_HIGH_VERSION,
		CELLTSP_LINENAME_STRING);

	if(dwDevId == 0xffffffff)
	{
		lineShutdown(m_hLineApp);
		return 10;
	}

	DWORD lRes = lineOpen(m_hLineApp, dwDevId, &m_hLine, 0x00020000, 0x00000000, 1, 
		LINECALLPRIVILEGE_MONITOR, dwMediaMode, NULL);
	

	if(lRes != 0)
	{	
		lineShutdown(m_hLineApp);
		return 11;
	}

	lRes = lineSetStatusMessages(m_hLine, 0x1ffffff, 0);
	
	if(lRes != 0)
	{	
		lineClose(m_hLine);
		lineShutdown(m_hLineApp);
		return 12;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Initialized TAPI successfully!"); 

	return dwReturn;
}
//---------------------------------------------------------------------------------------------------------
DWORD IncomingCallNotifier::SelectLine(const HLINEAPP hLineApp, const DWORD dwNumberDevices, 
	const DWORD dwAPIVersionLow, const DWORD dwAPIVersionHigh, const TCHAR* const psTSPLineName)
{
	DWORD dwReturn = 0xffffffff;
	for(DWORD dwCurrentDevID = 0 ; dwCurrentDevID < dwNumberDevices; dwCurrentDevID++)
	{
		DWORD dwAPIVersion;
		LINEEXTENSIONID LineExtensionID ;
		if(0 == lineNegotiateAPIVersion(hLineApp, dwCurrentDevID, dwAPIVersionLow, dwAPIVersionHigh, 
			&dwAPIVersion, &LineExtensionID)
		) 
		{
			LINEDEVCAPS LineDevCaps;
			LineDevCaps.dwTotalSize = sizeof(LineDevCaps);
			
			if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, dwAPIVersion, 0, &LineDevCaps)) 
			{
				BYTE* pLineDevCapsBytes = new BYTE[LineDevCaps.dwNeededSize];
				if(0 != pLineDevCapsBytes) 
				{
					LINEDEVCAPS* pLineDevCaps = (LINEDEVCAPS*)pLineDevCapsBytes;
					pLineDevCaps->dwTotalSize = LineDevCaps.dwNeededSize;
					
					if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, dwAPIVersion, 0, pLineDevCaps)) 
					{
						if(!(_tcscmp((TCHAR*)((BYTE*)pLineDevCaps+pLineDevCaps->dwLineNameOffset), psTSPLineName)))
						{
							dwReturn = dwCurrentDevID;
						}	
					}

					delete[]  pLineDevCapsBytes;
				}
			}
		}
	}
	
	return dwReturn;
}
//---------------------------------------------------------------------------------------------------------