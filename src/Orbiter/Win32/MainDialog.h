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
#if !defined(__MAIN_DIALOG_H__)
#define __MAIN_DIALOG_H__
//-----------------------------------------------------------------------------------------------------
#include "StartOrbiter.h"
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
	#include "stdafx.h"
	#include <commctrl.h>
#ifndef WINCE_x86
	#include <aygshell.h>
#endif
	#include <sipapi.h>
#else
	#include "windows.h"
	#include <string>
	using namespace std;
#endif

extern HINSTANCE	g_hInst; // The current instance
extern HWND			g_hwndMainDialog;		// The main dialog window handle
//-----------------------------------------------------------------------------------------------------
WORD				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);

void				WriteStatusOutput(const char* pMessage);
void				ShowMainDialog();
void				ShowSDLWindow();

void				RecordMouseAction(int x, int y);
void				RecordKeyboardAction(long key);
//-----------------------------------------------------------------------------------------------------
void				StartOrbiterThread();
void				StopOrbiterThread();
void				LoadUI_From_ConfigurationData();
void				SyncConfigurationData();
//-----------------------------------------------------------------------------------------------------
struct CommandLineParams
{
	string sRouter_IP;
	int PK_Device;
	int PK_DeviceTemplate;
	string sLogger;
	int Width, Height;
	bool bLocalMode; 
	bool bFullScreen;
	bool bUseOpenGL;
	string sLocalDirectory;
	string sNestedDisplay;

	bool bRouterIPSpecified;
	bool bDeviceIDSpecified;
};
//-----------------------------------------------------------------------------------------------------
extern CommandLineParams CmdLineParams;
//-----------------------------------------------------------------------------------------------------
#endif //__MAIN_DIALOG_H__