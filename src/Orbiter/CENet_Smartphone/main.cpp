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
 
//<-dceag-incl-b->!
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

// In source files stored in archives and packages, these 2 lines will have the release version (build)
// and the svn revision as a global variable that can be inspected within a core dump
//#define  VERSION "<=version=>"
//const char *g_szCompile_Date="<=compile_date=>";
/*SVN_REVISION*/


using namespace DCE;
//<-dceag-incl-e->

#include "OrbiterApp.h"

bool IsOrbiterAlreadyRunning();
void BringOrbiterToFront();
static HWND g_hWnd = NULL;
#define MAX_STRING 1024

//<-dceag-plug-b->! never a plug-in

//<-dceag-main-b->!  **DON'T AUTOMATICALLY OVERWRITE THIS SECTIONS, IT'S CUSTOM
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
    _Module.Init( 0, hInstance );

	if(IsOrbiterAlreadyRunning())
	{
		BringOrbiterToFront();
		return 0;
	}

	//get the binary name and the binary path
	char pFullPath[256];
	wchar_t pProcessNameW[256];
	::GetModuleFileName(NULL, pProcessNameW, sizeof(pProcessNameW));
	wcstombs(pFullPath, pProcessNameW, 256);

	g_sBinary = FileUtils::FilenameWithoutPath(pFullPath);
	g_sBinaryPath = FileUtils::BasePath(pFullPath) + "/";

	string sLogger = g_sBinaryPath + "OrbiterLog.txt";
	

    try
    {
		LoggerWrapper::SetType(LT_LOGGER_NULL);
    }
    catch(...)
    {
		printf("Unable to create logger\n");
		return -1;
    }

	WORD    wVersion;
	WSADATA wsaData;
	wVersion = MAKEWORD( 1, 1 );
	if (WSAStartup(wVersion, &wsaData)!=0)
	{
		int ec = WSAGetLastError();
		printf("WSAStartup err %d", ec);
		return -2;
	}

	OrbiterApp app(hInstance);
	app.Run();

    WSACleanup();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to delete logger...");
	

	return 0;
}
//<-dceag-main-e-> 

BOOL CALLBACK WindowsHandle(HWND hWnd, LPARAM lParam )
{
    static wchar_t pwTitle[MAX_STRING];
    static wchar_t pwClassName[MAX_STRING];
    ::GetWindowText(hWnd, pwTitle, MAX_STRING);
    ::GetClassName(hWnd, pwClassName, MAX_STRING);

	char pTitle[MAX_STRING];
	wcstombs(pTitle, pwTitle, MAX_STRING);
    string sTitle(pTitle);
    if(sTitle == "PocketFrog") 
    {
        g_hWnd = hWnd;
        return FALSE;
    }

    return TRUE;
}


bool IsOrbiterAlreadyRunning()
{
    ::EnumWindows(WindowsHandle, 0);
	return g_hWnd != NULL;
}

void BringOrbiterToFront()
{
    ::SetForegroundWindow(g_hWnd); 
	::ShowWindow(g_hWnd, SW_SHOW);
}
