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
#include "MainDialog.h"
#include "Resource.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "Commdlg.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "SelfUpdate.h"
#include "Simulator.h"
#include "PopupMessage.h"

#ifdef POCKETFROG
#include "PocketFrog/OrbiterRenderer_PocketFrog.h"
#endif

#if !defined(POCKETFROG) && !defined(WINCE)
	#include "Commctrl.h"
#endif

#pragma warning(disable : 4311 4312)

#ifdef WINCE
#define PLAYER_OFFSET 0
#define BOTTOM_OFFSET 60
#define SMALL_BOTTOM_OFFSET 5
#define BOTTOM_ADJUSTMENT 2
#else
#define PLAYER_OFFSET 5
#define BOTTOM_OFFSET 30 
#define SMALL_BOTTOM_OFFSET 3
#define BOTTOM_ADJUSTMENT 0
#endif

#define MAX_LOADSTRING 100
#define MENU_HEIGHT	   25
//-----------------------------------------------------------------------------------------------------
static int g_iDialogWidth;
static int g_iDialogHeight;
static bool g_bSimplifyUI = false;
static bool g_bSmartPhone = false;
//-----------------------------------------------------------------------------------------------------
const int MAX_STRING_LEN = 4096;
static bool bStartRecording = true;
static long OldTick = ::GetTickCount();
//-----------------------------------------------------------------------------------------------------
enum PageIndex
{
	piLogger,
	piRecorder,
	piRandom
};
//-----------------------------------------------------------------------------------------------------
DWORD OrbiterThreadId;
DWORD PlayerThreadId;
DWORD GeneratorThreadId;
//-----------------------------------------------------------------------------------------------------
// Global Variables:
HINSTANCE			g_hInst;				// The current instance
HWND				g_hwndMainDialog;		// The main dialog window handle
LONG                g_OldEditWndProc;

#ifdef WINCE
HWND				g_hwndCB;				// The command bar handle
#endif
//-----------------------------------------------------------------------------------------------------
HWND				g_hWndTab;
HWND				g_hWndPage1;
HWND				g_hWndPage2;
HWND				g_hWndPage3;
//-----------------------------------------------------------------------------------------------------
HWND				g_hWnd_FullScreenCheckBox;
HWND				g_hWnd_LogToServerCheckBox;
HWND				g_hWnd_DeviceIDEdit;
HWND				g_hWnd_RouterIPEdit;
HWND				g_hWnd_ApplyButton;
//-----------------------------------------------------------------------------------------------------
//page 1
/*extern*/ HWND				g_hWndList;
//-----------------------------------------------------------------------------------------------------
//page 2
HWND				g_hWndRecord_MouseCheckBox; 
HWND				g_hWndRecord_KeyboardCheckBox;
HWND				g_hWndRecord_List;
HWND				g_hWndRecord_SaveButton;
HWND				g_hWndRecord_LoadButton;
HWND				g_hWndRecord_ClearButton;
HWND				g_hWndRecord_GoButton;
HWND				g_hWndRecord_StopButton;
HWND				g_hWndRecord_RepeatEdit;
//-----------------------------------------------------------------------------------------------------
//page 3
HWND				g_hWndRandom_DelayMin;
HWND				g_hWndRandom_DelayMax;

HWND				g_hWndRandom_ButtonsPerClick;

HWND				g_hWndRandom_MouseCheckBox;
HWND				g_hWndRandom_KeyboardCheckBox;

HWND				g_hWndRandom_KeyOption1RadioBox;
HWND				g_hWndRandom_KeyOption2RadioBox;
HWND				g_hWndRandom_KeyOption3RadioBox;

HWND				g_hWndRandom_HomeEdit;

HWND				g_hWndRandom_GenerateButton;
HWND				g_hWndRandom_StopButton;
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
#ifndef WINCE_x86
static SHACTIVATEINFO s_sai;
#endif
#endif
//-----------------------------------------------------------------------------------------------------
/*extern*/ CommandLineParams CmdLineParams;
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PagesWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
#ifndef WINCE_x86
	HWND CreateRpCommandBar(HWND);
#endif
#endif

HWND CreateTabControl(HWND hWnd);
HWND CreatePage(HWND hWnd);
HWND CreateListBox(HWND hWnd, RECT rt);			
HWND CreateCheckBox(HWND hParentWnd, int x, int y, const char* caption, int width = 155, int height = 18);
HWND CreateButton(HWND hParentWnd, int x, int y, const char* caption, int width = 50);
HWND CreateLabel(HWND hParentWnd, int x, int y, int width, char* caption, int height = 20);
HWND CreateEdit(HWND hParentWnd, int x, int y,  int width, char* caption, bool bNumber, bool bAlignRight);
HWND CreateRadioBox(HWND hParentWnd, int x, int y, const char* caption, int width = 130);

void SetActivePage(int TabIndex);
void RecordAction(const char* sAction);
void RecordDelay();

void SaveUI_To_ConfigurationData();
void LoadUI_From_ConfigurationData();
//-----------------------------------------------------------------------------------------------------
void OnRecord_Load();  
void OnRecord_Save();  
void OnRecord_Clear(); 
void OnRecord_Go();
void OnRecord_Stop();
void OnRandom_Generate();
void OnRandom_Stop();
void OnApply();
void OnTryAutomCheckBoxChanged();

void GetEditText(HWND hWndEdit, string& Text);

bool g_bStopPlayerThread = false;
//-----------------------------------------------------------------------------------------------------
DWORD WINAPI OrbiterThread( LPVOID lpParameter);

void StartOrbiterThread()
{
	::CreateThread(NULL, 0, OrbiterThread, 0, 0, &OrbiterThreadId);
}

void StopOrbiterThread()
{
	::CloseHandle((HANDLE)OrbiterThreadId);
}
//-----------------------------------------------------------------------------------------------------
DWORD WINAPI OrbiterThread( LPVOID lpParameter)
{
	try
	{
#ifdef POCKETFROG
    _Module.Init( 0, g_hInst );
#endif

		StartOrbiter(
			CmdLineParams.PK_Device, 
			CmdLineParams.PK_DeviceTemplate, 
			CmdLineParams.sRouter_IP,
			CmdLineParams.sLocalDirectory,
			CmdLineParams.bLocalMode,
			CmdLineParams.Width,
			CmdLineParams.Height,
			CmdLineParams.bFullScreen
		);
	}
	catch(string s)
	{
		s = "Fatal error: " + s;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, s.c_str());
		WriteStatusOutput(s.c_str());
		exit(1);
	}
	catch(const char *s)
	{
		string sErrorMessage = string("Fatal error: ") + s;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, sErrorMessage.c_str());
		WriteStatusOutput(sErrorMessage.c_str());
		exit(1);
	}
	
	PostMessage(g_hwndMainDialog, WM_QUIT, 0, 0);

	return 0L;
}
//-----------------------------------------------------------------------------------------------------
DWORD WINAPI PlayerThread( LPVOID lpParameter)
{
	Sleep(1000);

	string sTimesText;
	GetEditText(g_hWndRecord_RepeatEdit, sTimesText);

	int Times = atoi(sTimesText.c_str());

	int Count = (int)::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);
	Orbiter *pOrbiter = Orbiter::Instance();

	while(Times--)
	{
		for(int i = 0; i < Count; i++)
		{
			if(g_bStopPlayerThread)
				return 0L;

	#ifdef WINCE
			wchar_t lpszBuffer[256];
	#else
			char lpszBuffer[256];
	#endif
			::SendMessage(g_hWndRecord_List, LB_GETTEXT, i, (LPARAM)(LPCTSTR)lpszBuffer);

	#ifdef WINCE
			char pItemBuffer[MAX_STRING_LEN];
			wcstombs(pItemBuffer, lpszBuffer, MAX_STRING_LEN);
			string sItemBuffer = pItemBuffer;
	#else
			string sItemBuffer = lpszBuffer;
	#endif

			string::size_type CurPos = 0;
			string sAction = StringUtils::Tokenize(sItemBuffer, " ", CurPos);

			if(sAction == "delay")
			{
				string sDelay = StringUtils::Tokenize(sItemBuffer, " ", CurPos);
				long delay = atoi(sDelay.c_str());

				Simulator::SimulateActionDelay(delay);
			}
			else
			{
				if(sAction == "button")
				{
					string sKey = StringUtils::Tokenize(sItemBuffer, " ", CurPos);
					long key = atoi(sKey.c_str());

					pOrbiter->SimulateKeyPress(key);
				}
				else
				{
					string sClickX = StringUtils::Tokenize(sItemBuffer, ",", CurPos);
					int x = atoi(sClickX.c_str());

					CurPos++; //skip space too

					string sClickY = StringUtils::Tokenize(sItemBuffer, " ", CurPos);
					int y = atoi(sClickY.c_str());

					pOrbiter->SimulateMouseClick(x, y);
				}
			}
		}
	}

	return 0L;
}
//-----------------------------------------------------------------------------------------------------
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    It is important to call this function so that the application 
//    will get 'well formed' small icons associated with it.
//
WORD MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
#ifdef WINCE
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ORBITER));
	wc.hCursor			= 0;
    wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);

#else
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ORBITER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= g_bSmartPhone? 0 : (LPCTSTR)IDC_ORBITER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
#endif
}
//-----------------------------------------------------------------------------------------------------
//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	RECT rc;
    HWND hWndDesktop = ::GetDesktopWindow();
    GetWindowRect(hWndDesktop, &rc);

#ifdef WINCE 
    rc.left = 0;
    rc.top = 0;

    #if !defined(SMARTPHONE) || defined(ARMV4I)
        rc.right = 240;
        rc.bottom = 280;
    #else
        rc.right = 176;
        rc.bottom = 220;
	#endif
#endif

    int iWidth = rc.right - rc.left;
    int iHeight = rc.bottom - rc.top;

    if(iWidth >= 640 && iHeight >= 480)
    {		
        g_iDialogWidth = 640;
        g_iDialogHeight = 480;
    }
    else //match the desktop resolution
    {
		g_bSimplifyUI = true;
        g_iDialogWidth = iWidth;
        g_iDialogHeight = iHeight;

        if(iWidth < 240 && iHeight < 280)
            g_bSmartPhone = true;
    }
    
    HWND	hWnd = NULL;
	g_hInst = hInstance;		// Store instance handle in our global variable

	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	// Initialize global strings
	LoadString(hInstance, IDC_ORBITER, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	MyRegisterClass(hInstance, szWindowClass);

#ifdef WINCE
	#if defined(SMARTPHONE) //&& !defined(ARMV4I)
		hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	#else
		hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	#endif
#else
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
#endif

	if (!hWnd)
	{	
		return FALSE;
	}

#ifndef WINCE
	rc.left = (iWidth - g_iDialogWidth) / 2;
	rc.right = rc.left + g_iDialogWidth;
	rc.top = (iHeight - g_iDialogHeight) / 2;
	rc.bottom = rc.top + g_iDialogHeight;
#endif

#ifdef WINCE
	if (g_hwndCB)
	{
		RECT rcMenuBar;
		GetWindowRect(g_hwndCB, &rcMenuBar);
		rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
	}
#endif		

	MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hwndMainDialog = hWnd;

	return TRUE;
}
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
    {
        case WM_KEYDOWN:
        if(wParam == VK_RETURN)
        {
            ::EnableWindow(hWnd, false);
            ::EnableWindow(hWnd, true);
            ::SetFocus(g_hwndMainDialog);
			return 0;
        }
        break;
    }

    return CallWindowProc((WNDPROC)g_OldEditWndProc, hWnd, message, wParam, lParam);
}
//-----------------------------------------------------------------------------------------------------

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int wmId, wmEvent;
	PAINTSTRUCT ps;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:

			switch(wmEvent)
			{
			case BN_CLICKED:
				{
					if(lParam == (LPARAM)g_hWnd_ApplyButton)
						OnApply();

					break;
				}
			}

			switch (wmId)
			{	
				case IDM_MAIN_FILE_QUIT:
				case IDOK:
					SendMessage (hWnd, WM_CLOSE, 0, 0);
					break;
				case IDM_MAIN_SHOW_SDLWINDOW:
					ShowSDLWindow();
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
			{
				
#ifdef WINCE
				//make sure the that task bar is visible
				HWND hTaskBarWindow = ::FindWindow(TEXT("HHTaskBar"), NULL);
				::ShowWindow(hTaskBarWindow, SW_SHOWNORMAL);

#ifndef WINCE_x86
				g_hwndCB = CreateRpCommandBar(hWnd);

				// Initialize the shell activate info structure
				memset (&s_sai, 0, sizeof (s_sai));
				s_sai.cbSize = sizeof (s_sai);
#endif

#endif //WINCE


				//configuration info
                if(g_bSmartPhone)
                {
                    CreateLabel(hWnd, 10, 10, 13, "1. "); 
                    g_hWnd_FullScreenCheckBox = CreateCheckBox(hWnd, 25, 10, "Full screen", 120);
                    CreateLabel(hWnd, 10, 33, 13, "2. "); 
                    g_hWnd_LogToServerCheckBox = CreateCheckBox(hWnd, 25, 30, "Log to server for debugging", 120, 40);
                    CreateLabel(hWnd, 10, 75, 90, "3. Device ID:");	
                    g_hWnd_DeviceIDEdit = CreateEdit(hWnd, 105, 75, 50, "", true, true);
                    CreateLabel(hWnd, 10, 95, 90, "4. Router IP:");	
                    g_hWnd_RouterIPEdit = CreateEdit(hWnd, 35, 115, 120, "", false, false);
                    CreateLabel(hWnd, 10, 140, 100, "5. Apply");	
                    CreateLabel(hWnd, 10, 160, 100, "6. Exit");	

					g_OldEditWndProc = ::GetWindowLong(g_hWnd_DeviceIDEdit, GWL_WNDPROC);
                    ::SetWindowLong(g_hWnd_DeviceIDEdit, GWL_WNDPROC, reinterpret_cast<long>(EditWndProc));
                    ::SetWindowLong(g_hWnd_RouterIPEdit, GWL_WNDPROC, reinterpret_cast<long>(EditWndProc));
                }
                else
                {
                    int iBase = g_iDialogHeight - 5 * MENU_HEIGHT - SMALL_BOTTOM_OFFSET - BOTTOM_ADJUSTMENT - 10;
                    g_hWnd_FullScreenCheckBox = CreateCheckBox(hWnd, 10, iBase, "Full screen", 
#ifdef WIN32
						120
#else
						180
#endif						
					);
                    g_hWnd_LogToServerCheckBox = CreateCheckBox(hWnd, 10, iBase + 20, "Log to server for debugging", 400, 20);
                    CreateLabel(hWnd, 10, iBase + 42, 80, "Device ID: ");	
                    g_hWnd_DeviceIDEdit = CreateEdit(hWnd, 90, iBase + 42, 50, "", true, true);
                    CreateLabel(hWnd, 10, iBase + 64, 80, "Router IP: ");	
                    g_hWnd_RouterIPEdit = CreateEdit(hWnd, 90, iBase + 64, 120, "", false, false);
                    g_hWnd_ApplyButton = CreateButton(hWnd, g_iDialogWidth - 70, iBase, "&Apply");
                }

				g_hWndTab = CreateTabControl(hWnd);
				g_hWndPage1 = CreatePage(g_hWndTab);
				g_hWndPage2 = CreatePage(g_hWndTab);
				g_hWndPage3 = CreatePage(g_hWndTab);

				//page 1
				RECT rt_list = { 1, 1, g_iDialogWidth - 16, g_iDialogHeight - 5 * MENU_HEIGHT - 5 - BOTTOM_OFFSET };
				g_hWndList = CreateListBox(g_hWndPage1, rt_list);

				//page 2
				g_hWndRecord_MouseCheckBox = CreateCheckBox(g_hWndPage2, 10, 10, "Record mouse clicks", 160, 36);
				::SendMessage(g_hWndRecord_MouseCheckBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRecord_KeyboardCheckBox = CreateCheckBox(g_hWndPage2, 10, 50, "Record keyboard", 160, 36);
				::SendMessage(g_hWndRecord_KeyboardCheckBox, BM_SETCHECK, BST_CHECKED, 0);
				
				RECT rt_record_list = { 170, 10, g_iDialogWidth - 190, g_iDialogHeight - 150 - BOTTOM_OFFSET - MENU_HEIGHT };
				g_hWndRecord_List = CreateListBox(g_hWndPage2, rt_record_list);

				g_hWndRecord_SaveButton = CreateButton(g_hWndPage2, 120 + 170, g_iDialogHeight - 140 - BOTTOM_OFFSET - MENU_HEIGHT, "&Save");
				g_hWndRecord_LoadButton = CreateButton(g_hWndPage2, 120 + 230, g_iDialogHeight - 140 - BOTTOM_OFFSET - MENU_HEIGHT, "&Load");
				g_hWndRecord_ClearButton = CreateButton(g_hWndPage2, 120 + 290, g_iDialogHeight - 140 - BOTTOM_OFFSET - MENU_HEIGHT, "&Clear");

				CreateLabel(g_hWndPage2, 10, 200, 25 + PLAYER_OFFSET + 4, "Play");
				g_hWndRecord_RepeatEdit = CreateEdit(g_hWndPage2, 35 + 4 * PLAYER_OFFSET + 4, 200, 32, "1", true, true);
				CreateLabel(g_hWndPage2, 70 + 3 * PLAYER_OFFSET + 8, 200, 30 + PLAYER_OFFSET + 8, "times");
				g_hWndRecord_GoButton = CreateButton(g_hWndPage2, 10, 230, "&Go");
				g_hWndRecord_StopButton = CreateButton(g_hWndPage2, 80, 230, "&Stop");

				::SetWindowLong(g_hWndPage2, GWL_WNDPROC, reinterpret_cast<long>(PagesWndProc));

				//page 3
				CreateLabel(g_hWndPage3, 10, 10, 150, "Delay between events:");	
				g_hWndRandom_DelayMin = CreateEdit(g_hWndPage3, 160, 10, 50, "500", true, true);
				CreateLabel(g_hWndPage3, 215, 10, 5, "-");	
				g_hWndRandom_DelayMax = CreateEdit(g_hWndPage3, 225, 10, 50, "5000", true, true);

				CreateLabel(g_hWndPage3, 10, 40, 200, "Number of buttons per click: ");	
				g_hWndRandom_ButtonsPerClick = CreateEdit(g_hWndPage3, 225, 40, 50, "0", true, true);

				g_hWndRandom_MouseCheckBox = CreateCheckBox(g_hWndPage3, 10, 70, "Generate mouse clicks", 400);
				::SendMessage(g_hWndRandom_MouseCheckBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRandom_KeyboardCheckBox = CreateCheckBox(g_hWndPage3, 10, 90, "Generate keyboard events", 400);
				::SendMessage(g_hWndRandom_KeyboardCheckBox, BM_SETCHECK, BST_CHECKED, 0);

				g_hWndRandom_KeyOption1RadioBox = CreateRadioBox(g_hWndPage3, 10, 120, "A - Z, 0 - 9", 400);
				g_hWndRandom_KeyOption2RadioBox = CreateRadioBox(g_hWndPage3, 10, 140, "UP, DOWN, LEFT, RIGHT, ENTER", 400);
				g_hWndRandom_KeyOption3RadioBox = CreateRadioBox(g_hWndPage3, 10, 160, "Phone keys: 0 - 9, *, #, C, n", 400);
				::SendMessage(g_hWndRandom_KeyOption2RadioBox, BM_SETCHECK, BST_CHECKED, 0);

				CreateLabel(g_hWndPage3, 10, 200, 200, "Home screen: ");	
				g_hWndRandom_HomeEdit = CreateEdit(g_hWndPage3, 225, 200, 50, "1255", true, true);

				g_hWndRandom_GenerateButton = CreateButton(g_hWndPage3, 10, 230, "&Generate", 100);
				g_hWndRandom_StopButton = CreateButton(g_hWndPage3, 120, 230, "&Stop");

				::SetWindowLong(g_hWndPage3, GWL_WNDPROC, reinterpret_cast<long>(PagesWndProc));
				SetActivePage(piLogger);
				LoadUI_From_ConfigurationData();
			}
			break;

		case WM_PAINT:
			{
				RECT rt;
				hdc = BeginPaint(hWnd, &ps);
				GetClientRect(hWnd, &rt);
#if defined(SMARTPHONE) && !defined(ARMV4I)
				COLORREF crBkColor = 0x00FFFFFF;
#else
				COLORREF crBkColor = ::GetSysColor(COLOR_3DFACE);
#endif
				HBRUSH hBrush = CreateSolidBrush(crBkColor);
				HBRUSH hBrushOld = (HBRUSH)SelectObject(hdc, hBrush);
				FillRect(hdc, &rt, hBrush);//(HBRUSH)::GetStockObject(GRAY_BRUSH));
                SelectObject(hdc, hBrushOld);
				DeleteObject(hBrush);
				EndPaint(hWnd, &ps);
			}
			break; 
		case WM_DESTROY:
#ifdef WINCE
			CommandBar_Destroy(g_hwndCB);
#endif
			{
#ifdef POCKETFROG
				if(NULL != Orbiter::Instance())
					Orbiter::Instance()->OnQuit();
				Sleep(500);
#else
				HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);						
				::PostMessage(hSDLWindow, WM_QUIT, 0L, 0L);
				Sleep(250);
#endif
			}

			PostQuitMessage(0);
			break;
		case WM_ACTIVATE:
			// Notify shell of our activate message
#ifdef WINCE			
#ifndef WINCE_x86
			SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
#endif
#endif
     		break;
		case WM_SETTINGCHANGE:
#ifdef WINCE
#ifndef WINCE_x86
			SHHandleWMSettingChange(hWnd, wParam, lParam, &s_sai);
#endif
#endif
     		break;

        case WM_KEYDOWN:
            if(g_bSmartPhone)
            {
                if(wParam == (WPARAM)'1')
                {
                    bool bFullScreen = BST_CHECKED == ::SendMessage(g_hWnd_FullScreenCheckBox, BM_GETCHECK, 0, 0);
                    ::SendMessage(g_hWnd_FullScreenCheckBox, BM_SETCHECK, bFullScreen ? BST_UNCHECKED : BST_CHECKED, 0);
                }

                if(wParam == (WPARAM)'2')
                {
                    bool bLogToServer = BST_CHECKED == ::SendMessage(g_hWnd_LogToServerCheckBox, BM_GETCHECK, 0, 0);
                    ::SendMessage(g_hWnd_LogToServerCheckBox, BM_SETCHECK, bLogToServer ? BST_UNCHECKED : BST_CHECKED, 0);
                }

                if(wParam == (WPARAM)'3')
                {
                    ::SetFocus(g_hWnd_DeviceIDEdit);
                }

                if(wParam == (WPARAM)'4')
                {
                    ::SetFocus(g_hWnd_RouterIPEdit);
                }

                if(wParam == (WPARAM)'5')
                {
                    OnApply();
                }

                if(wParam == (WPARAM)'6')
                {
                    ::PostMessage(g_hwndMainDialog, WM_QUIT, 0, 0);
                }
            }
            break;

		case WM_NOTIFY:
		{
			NMHDR *lpnmhdr = (LPNMHDR) lParam;

			switch(lpnmhdr->code)
			{
				case TCN_SELCHANGE:
					if(lpnmhdr->hwndFrom == g_hWndTab)
						SetActivePage(TabCtrl_GetCurSel(g_hWndTab));
					break;
			}

			break;
		}


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK PagesWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	int wmId, wmEvent;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			switch(wmEvent)
			{
				case BN_CLICKED:
				{
					if(lParam == (LPARAM)g_hWndRecord_LoadButton)
						OnRecord_Load();
					else if(lParam == (LPARAM)g_hWndRecord_SaveButton)
						OnRecord_Save();  
					else if(lParam == (LPARAM)g_hWndRecord_ClearButton) 
						OnRecord_Clear(); 
					else if(lParam == (LPARAM)g_hWndRecord_GoButton)    
						OnRecord_Go(); 
					else if(lParam == (LPARAM)g_hWndRecord_StopButton)    
						OnRecord_Stop(); 
					else if(lParam == (LPARAM)g_hWndRandom_GenerateButton)
						OnRandom_Generate();
					else if(lParam == (LPARAM)g_hWndRandom_StopButton)
						OnRandom_Stop();

					break;
				}
			}
		break;

		case WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);

			FillRect(hdc, &rt, (HBRUSH)::GetSysColorBrush(COLOR_3DFACE));
			EndPaint(hWnd, &ps);
			break; 

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);

	}

	return 0;
}
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
#ifndef WINCE_x86
HWND CreateRpCommandBar(HWND hwnd)
{
	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd;
	mbi.nToolBarId = IDM_MENU;
	mbi.hInstRes   = g_hInst;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;

	if (!SHCreateMenuBar(&mbi)) 
		return NULL;

	return mbi.hwndMB;
}
#endif
#endif
//-----------------------------------------------------------------------------------------------------
HWND CreateTabControl(HWND hWnd)
{
    if(g_bSmartPhone)
        return NULL;

#ifdef WINCE
	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
	iccex.dwSize = ICC_TAB_CLASSES;
	InitCommonControlsEx (&iccex);
	#define OFFSET MENU_HEIGHT
#else
	#define OFFSET 1
#endif

	RECT rt = {1, OFFSET, g_iDialogWidth - 5, g_iDialogHeight - 4 * MENU_HEIGHT - BOTTOM_OFFSET - 10};

	HWND hTabCtrl = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE ,
		rt.left, rt.top, rt.right, rt.bottom, hWnd,(HMENU)"", g_hInst, NULL );

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;


	if(!g_bSimplifyUI)
	{
		tcItem.pszText = _T("Random");
		::SendMessage(hTabCtrl, TCM_INSERTITEM, (WPARAM)(int)0, (LPARAM)(const TC_ITEM *)&tcItem);
		tcItem.pszText = _T("Recorder");
		::SendMessage(hTabCtrl, TCM_INSERTITEM, (WPARAM)(int)0, (LPARAM)(const TC_ITEM *)&tcItem);
	}

	tcItem.pszText = _T("Logger");
	::SendMessage(hTabCtrl, TCM_INSERTITEM, (WPARAM)(int)0, (LPARAM)(const TC_ITEM *)&tcItem);

    return hTabCtrl;
}
//-----------------------------------------------------------------------------------------------------
HWND CreatePage(HWND hWnd)
{
	RECT rt = {3, MENU_HEIGHT, g_iDialogWidth - 12, g_iDialogHeight - 5 * MENU_HEIGHT - BOTTOM_OFFSET - 4 - 10};

	HWND hWndPage = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER,
		rt.left, rt.top, rt.right, rt.bottom, hWnd, (HMENU)"", g_hInst, NULL );

	return hWndPage;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateListBox(HWND hWnd, RECT rt_list)
{
	HWND hWndList = 
		CreateWindow(TEXT("LISTBOX"), NULL, 
			(WS_CHILD | WS_VISIBLE | LBS_STANDARD) & ~LBS_SORT, 
			rt_list.left, rt_list.top, rt_list.right, rt_list.bottom, 
			hWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateCheckBox(HWND hParentWnd, int x, int y, const char* caption, int width/* = 130*/, int height/* = 18*/)
{
	RECT rt_cb = { x, y, width, height };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
	#define CAPTION wTextBuffer
#else
	#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), CAPTION, 
			WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_AUTOCHECKBOX | BS_MULTILINE, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateRadioBox(HWND hParentWnd, int x, int y, const char* caption, int width/* = 130*/)
{
	RECT rt_cb = { x, y, width, 18 };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
	#define CAPTION wTextBuffer
#else
	#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), CAPTION, 
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateButton(HWND hParentWnd, int x, int y, const char* caption, int width/*=50*/)
{
	RECT rt_cb = { x, y, width, 20 };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
	#define CAPTION wTextBuffer
#else
	#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("BUTTON"), CAPTION, 
			WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | WS_TABSTOP, 
			rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateLabel(HWND hParentWnd, int x, int y, int width, char* caption, int height)
{
	RECT rt_label = { x, y, width, height };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
	#define CAPTION wTextBuffer
#else
	#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(TEXT("STATIC"), CAPTION, 
			WS_CHILD | WS_VISIBLE, 
			rt_label.left, rt_label.top, rt_label.right, rt_label.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
HWND CreateEdit(HWND hParentWnd, int x, int y,  int width, char* caption, bool bNumber, bool bAlignRight)
{
	RECT rt_edit = { x, y, width, 20 };

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, caption, MAX_STRING_LEN);
	#define CAPTION wTextBuffer
#else
	#define CAPTION caption
#endif

	HWND hWndList = 
		CreateWindow(
			TEXT("EDIT"), CAPTION, 
			ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER |
				(bAlignRight ? ES_RIGHT : ES_LEFT) |
				(bNumber ? ES_NUMBER : 0), 
			rt_edit.left, rt_edit.top, rt_edit.right, rt_edit.bottom, 
			hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
void SetActivePage(int TabIndex)
{
	TabCtrl_SetCurSel(g_hWndTab, TabIndex);

	//hides all windows
	::ShowWindow(g_hWndPage1, SW_HIDE);
	::ShowWindow(g_hWndPage2, SW_HIDE);
	::ShowWindow(g_hWndPage3, SW_HIDE);

	switch(TabIndex)
	{
		case 0:	::ShowWindow(g_hWndPage1, SW_SHOW); break;
		case 1:	::ShowWindow(g_hWndPage2, SW_SHOW); break;
		case 2:	::ShowWindow(g_hWndPage3, SW_SHOW); break;
		default:
			WriteStatusOutput("ERROR: invalid page index");
	}
}
//-----------------------------------------------------------------------------------------------------
void WriteStatusOutput(const char* pMessage)
{
#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, pMessage, MAX_STRING_LEN);
	#define MESSAGE wTextBuffer
#else
	#define MESSAGE pMessage
#endif

	const int cTimeOutInterval = 50; //miliseconds
	DWORD Count;
	if(0 == ::SendMessageTimeout(g_hWndList, LB_ADDSTRING, 0L, (LPARAM)MESSAGE, SMTO_NORMAL, cTimeOutInterval, &Count))
		return; //send message timed out

	DWORD Result;
	::SendMessageTimeout(g_hWndList, LB_SETTOPINDEX, Count - 1, 0L, SMTO_NORMAL, cTimeOutInterval, &Result);
}
//-----------------------------------------------------------------------------------------------------
void ShowMainDialog() //actually, hides the sdl window
{
	WriteStatusOutput("Minimizing SDL window...");

#ifdef POCKETFROG
	HWND hSDLWindow = ::FindWindow(TEXT("PocketFrog"), NULL);
#else
	HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);						
#endif

	::ShowWindow(hSDLWindow, SW_HIDE);

#ifdef WINCE
	HWND hTaskBarWindow = ::FindWindow(TEXT("HHTaskBar"), NULL);
	::ShowWindow(hTaskBarWindow, SW_SHOWNORMAL);
#endif
}
//-----------------------------------------------------------------------------------------------------
void ShowSDLWindow() 
{
	WriteStatusOutput("Showing SDL window...");
#ifdef POCKETFROG
	HWND hSDLWindow = ::FindWindow(TEXT("PocketFrog"), NULL);
#else
	HWND hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);						
#endif

#ifndef WINCE
	if(!IsIconic(hSDLWindow))
#endif
		::ShowWindow(hSDLWindow, SW_SHOWNORMAL);
}
//-----------------------------------------------------------------------------------------------------
void RecordAction(const char* sAction)
{
	string s = string(sAction) + "\n";
	FILE* f = fopen("record.out", "a+");
	fseek(f, 0, SEEK_END);
	fwrite(s.c_str(), s.length(), 1, f);
	fclose(f);

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, sAction, MAX_STRING_LEN);
	#define MESSAGE_RECORD wTextBuffer
#else
	#define MESSAGE_RECORD sAction
#endif

	const int cTimeOutInterval = 50; //miliseconds
	DWORD Count;
	if(0 == ::SendMessageTimeout(g_hWndRecord_List, LB_ADDSTRING, 0L, (LPARAM)MESSAGE_RECORD, SMTO_NORMAL, cTimeOutInterval, &Count))
		return; //send message timed out

	DWORD Result;
	::SendMessageTimeout(g_hWndRecord_List, LB_SETTOPINDEX, Count - 1, 0L, SMTO_NORMAL, cTimeOutInterval, &Result);
}
//-----------------------------------------------------------------------------------------------------
void RecordDelay()
{
	long Tick = ::GetTickCount();

	string sDelay = "delay " + StringUtils::ltos(Tick - OldTick) + " ";

	OldTick = Tick;

	RecordAction(sDelay.c_str());
}
//-----------------------------------------------------------------------------------------------------
void RecordMouseAction(int x, int y)
{	
	DWORD Result;
	
	if(0 == ::SendMessageTimeout(g_hWndRecord_MouseCheckBox, BM_GETCHECK, 0, 0, SMTO_NORMAL, 50, &Result))
		return; //send message timed out


	if(BST_CHECKED == Result)
	{
		if(bStartRecording)
		{
			bStartRecording = false;
			OldTick = ::GetTickCount();
			string sDelay = "delay 2000"; 
			RecordAction(sDelay.c_str());
		}
		else
			RecordDelay();

		string sAction = "click " + StringUtils::ltos(x) + ", " + StringUtils::ltos(y) + " ";
		RecordAction(sAction.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void RecordKeyboardAction(long key)
{
	DWORD Result;
	
	if(0 == ::SendMessageTimeout(g_hWndRecord_KeyboardCheckBox, BM_GETCHECK, 0, 0, SMTO_NORMAL, 1000, &Result))
		return; //send message timed out


	if(BST_CHECKED == Result)
	{
		if(key == 121) //don't record key F10
			return;

		if(bStartRecording)
		{
			bStartRecording = false;
			OldTick = ::GetTickCount();
			string sDelay = "delay 2000"; 
			RecordAction(sDelay.c_str());
		}
		else
			RecordDelay();

		string sAction = "button " + StringUtils::ltos(key) + " (char: '" + char(key) + "')";
		RecordAction(sAction.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Load()
{
	WriteStatusOutput("RECORD: LOAD");

	OPENFILENAME ofn;

#ifdef WINCE
	wchar_t lpstrFile[256];
#else
	char lpstrFile[256];
#endif

	ZeroMemory(lpstrFile, sizeof(lpstrFile));
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hwndMainDialog;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

	if(GetOpenFileName(&ofn))
	{
#ifdef WINCE
		char sTextBuffer[256];
		wcstombs(sTextBuffer, ofn.lpstrFile, 256);
#else
		#define sTextBuffer ofn.lpstrFile
#endif

		vector<string> vectString;
		FileUtils::ReadFileIntoVector( sTextBuffer, vectString ); /** < reads file into a vector of strings for each line */

		for(vector<string>::iterator it = vectString.begin(); it != vectString.end(); it++)
		{
#ifdef WINCE
			wchar_t wTextBuffer[MAX_STRING_LEN];
			mbstowcs(wTextBuffer, (*it).c_str(), MAX_STRING_LEN);
			#define MY_LINE_LOAD wTextBuffer
#else
			#define MY_LINE_LOAD (*it).c_str()
#endif

			::SendMessage(g_hWndRecord_List, LB_ADDSTRING, 0L, (LPARAM)MY_LINE_LOAD);
		}

		int Count = (int)::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);
		::SendMessage(g_hWndRecord_List, LB_SETTOPINDEX, Count - 1, 0L);

		vectString.clear();
	}
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Save()
{
	WriteStatusOutput("RECORD: SAVE");

	OPENFILENAME ofn;

#ifdef WINCE
	wchar_t lpstrFile[256];
	ZeroMemory(lpstrFile, sizeof(lpstrFile));
	wcscpy(lpstrFile, TEXT("RecordOutput.txt"));
#else
	char lpstrFile[256];
	ZeroMemory(lpstrFile, sizeof(lpstrFile));
	strcpy(lpstrFile, "RecordOutput.txt");
#endif

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hwndMainDialog;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;

	if(GetSaveFileName(&ofn))
	{
		int Count = (int)::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L);

		vector<string> vectString;
		for(int i = 0; i < Count; i++)
		{
#ifdef WINCE
			wchar_t lpszBuffer[256];
#else
			char lpszBuffer[256];
#endif
			::SendMessage(g_hWndRecord_List, LB_GETTEXT, i, (LPARAM)(LPCTSTR)lpszBuffer);

#ifdef WINCE
			char sItemBuffer[MAX_STRING_LEN];
			wcstombs(sItemBuffer, lpszBuffer, MAX_STRING_LEN);
			#define MY_LINE_SAVE sItemBuffer
#else
			#define MY_LINE_SAVE lpszBuffer
#endif
			vectString.push_back(string(MY_LINE_SAVE));
		}

#ifdef WINCE
		char sTextBuffer[MAX_STRING_LEN];
		wcstombs(sTextBuffer, ofn.lpstrFile, MAX_STRING_LEN);
#else
		#define sTextBuffer ofn.lpstrFile
#endif

		FileUtils::WriteVectorToFile( sTextBuffer, vectString ); /** < writes a file where the vector of strings are the lines */

		vectString.clear();
	}
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Clear()
{
	WriteStatusOutput("RECORD: CLEAR");
	bStartRecording = true;

	while(::SendMessage(g_hWndRecord_List, LB_GETCOUNT, 0L, 0L) > 0)
		::SendMessage(g_hWndRecord_List, LB_DELETESTRING, 0L, 0L);
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Go()
{
	WriteStatusOutput("RECORD: GO");

	::SendMessage(g_hWndRecord_MouseCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);
	::SendMessage(g_hWndRecord_KeyboardCheckBox, BM_SETCHECK, BST_UNCHECKED, 0);

	ShowSDLWindow();

	g_bStopPlayerThread = false;
	::CreateThread(NULL, 0, PlayerThread, 0, 0, &PlayerThreadId);
}
//-----------------------------------------------------------------------------------------------------
void OnRecord_Stop()
{
	WriteStatusOutput("RECORD: STOP");

	g_bStopPlayerThread = true;
}
//-----------------------------------------------------------------------------------------------------
void OnRandom_Generate()
{
	WriteStatusOutput("RANDOM: GENERATE");
	ShowSDLWindow();

	SaveUI_To_ConfigurationData();

	Simulator::GetInstance()->StartRandomEventGenerator();
}
//-----------------------------------------------------------------------------------------------------
void OnRandom_Stop()
{
	WriteStatusOutput("RANDOM: STOP");

	Simulator::GetInstance()->StopRandomEventGenerator();
}
//-----------------------------------------------------------------------------------------------------
void OnApply()
{
	SaveUI_To_ConfigurationData();

	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = 0;

	string sCmdLine;

	if(!Simulator::GetInstance()->m_bTryToDetermineAutomatically)
	{
		sCmdLine = "-d " + Simulator::GetInstance()->m_sDeviceID + " -r " + Simulator::GetInstance()->m_sRouterIP + 
			" -l " + "\"" + CmdLineParams.sLogger + "\"";
	}

#ifdef WINCE
	wchar_t pProcessNameW[256];
	::GetModuleFileName(NULL, pProcessNameW, sizeof(pProcessNameW));

	wchar_t wCmdLine[256];
	mbstowcs(wCmdLine, sCmdLine.c_str(), 256);

	::CreateProcess(pProcessNameW, wCmdLine, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);
#else

	char pProcessFilePath[256];
	OrbiterSelfUpdate::GetProcessFilePath(pProcessFilePath);

	sCmdLine = string(pProcessFilePath) + " " + sCmdLine;
	::CreateProcess(NULL, const_cast<char *>(sCmdLine.c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);
#endif

	exit(1);
}
//-----------------------------------------------------------------------------------------------------
void OnTryAutomCheckBoxChanged()
{
	bool bAutomDetection = false;

	if(bAutomDetection) 
	{
#ifdef WINCE
		wchar_t wTextBuffer[MAX_STRING_LEN];
		mbstowcs(wTextBuffer, "", MAX_STRING_LEN);

		::SetWindowText(g_hWnd_DeviceIDEdit, wTextBuffer);
		::SetWindowText(g_hWnd_RouterIPEdit, wTextBuffer);
#else
		::SetWindowText(g_hWnd_DeviceIDEdit, "");
		::SetWindowText(g_hWnd_RouterIPEdit, "");
#endif
	}
}
//-----------------------------------------------------------------------------------------------------
void GetEditText(HWND hWndEdit, string& Text)
{
#ifdef WINCE
	wchar_t pwText[256]; 
	::GetWindowText(hWndEdit, pwText, 256); 
	char pText[MAX_STRING_LEN];
	wcstombs(pText, pwText, MAX_STRING_LEN);
#else
	char pText[256];
	GetWindowText(hWndEdit,pText,256);
#endif

	Text = pText;
	return;
}
//-----------------------------------------------------------------------------------------------------
void SaveUI_To_ConfigurationData()
{
	bool bGenerateMouseClicks = BST_CHECKED == ::SendMessage(g_hWndRandom_MouseCheckBox, BM_GETCHECK, 0, 0);
	bool bGenerateKeyboardEvents = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyboardCheckBox, BM_GETCHECK, 0, 0);

	bool bOption1 = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyOption1RadioBox, BM_GETCHECK, 0, 0);
	bool bOption2 = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyOption2RadioBox, BM_GETCHECK, 0, 0);
	bool bOption3 = BST_CHECKED == ::SendMessage(g_hWndRandom_KeyOption3RadioBox, BM_GETCHECK, 0, 0);

	string DelayMin;
	GetEditText(g_hWndRandom_DelayMin, DelayMin);

	string DelayMax;
	GetEditText(g_hWndRandom_DelayMax, DelayMax);

	string ButtonsPerClick;
	GetEditText(g_hWndRandom_ButtonsPerClick, ButtonsPerClick);

	string HomeScreen;
	GetEditText(g_hWndRandom_HomeEdit, HomeScreen);

	bool bAutomDetection = false;

	string DeviceID;
	GetEditText(g_hWnd_DeviceIDEdit, DeviceID);
	string RouterIP;
	GetEditText(g_hWnd_RouterIPEdit, RouterIP);

	bool bFullScreen = BST_CHECKED == ::SendMessage(g_hWnd_FullScreenCheckBox, BM_GETCHECK, 0, 0);
	bool bLogToServer = BST_CHECKED == ::SendMessage(g_hWnd_LogToServerCheckBox, BM_GETCHECK, 0, 0);

	Simulator::GetInstance()->m_dwDelayMin = atoi(DelayMin.c_str());
	Simulator::GetInstance()->m_dwDelayMax = atoi(DelayMax.c_str());
	Simulator::GetInstance()->m_iNumberOfButtonsPerClick = atoi(ButtonsPerClick.c_str());
	Simulator::GetInstance()->m_bGenerateMouseClicks = bGenerateMouseClicks;
	Simulator::GetInstance()->m_bGenerateKeyboardEvents = bGenerateKeyboardEvents;
	Simulator::GetInstance()->m_iKeysSetToGenerate = bOption2 * 1 + bOption3 * 2;
	Simulator::GetInstance()->m_sHomeScreen = HomeScreen;
	Simulator::GetInstance()->m_bTryToDetermineAutomatically = bAutomDetection;
	Simulator::GetInstance()->m_sDeviceID = DeviceID;
	Simulator::GetInstance()->m_sRouterIP = RouterIP;
	Simulator::GetInstance()->m_bFullScreen = bFullScreen;

	Simulator::GetInstance()->m_bLogToServer = bLogToServer;

	if(Simulator::GetInstance()->m_sConfigurationFile != "")
		Simulator::GetInstance()->SaveConfigurationFile(Simulator::GetInstance()->m_sConfigurationFile);
}
//-----------------------------------------------------------------------------------------------------
void SyncConfigurationData()
{
	if(CmdLineParams.PK_Device != 0)
		Simulator::GetInstance()->m_sDeviceID = StringUtils::ltos(CmdLineParams.PK_Device);

	Simulator::GetInstance()->m_sRouterIP = CmdLineParams.sRouter_IP;
	Simulator::GetInstance()->m_bTryToDetermineAutomatically = false;
	Simulator::GetInstance()->m_bFullScreen = CmdLineParams.bFullScreen;

}
//-----------------------------------------------------------------------------------------------------
void LoadUI_From_ConfigurationData()
{
	::SendMessage(g_hWndRandom_MouseCheckBox, BM_SETCHECK, 
		Simulator::GetInstance()->m_bGenerateMouseClicks ? BST_CHECKED : BST_UNCHECKED, 0);
	::SendMessage(g_hWndRandom_KeyboardCheckBox, BM_SETCHECK, 
		Simulator::GetInstance()->m_bGenerateKeyboardEvents ? BST_CHECKED : BST_UNCHECKED, 0);

	if(Simulator::GetInstance()->m_sDeviceID == "0")
		Simulator::GetInstance()->m_sDeviceID = "";

#ifdef WINCE
	wchar_t wTextBuffer[MAX_STRING_LEN];

	mbstowcs(wTextBuffer, StringUtils::ltos(Simulator::GetInstance()->m_dwDelayMin).c_str(), MAX_STRING_LEN);
	::SetWindowText(g_hWndRandom_DelayMin, wTextBuffer);

	mbstowcs(wTextBuffer, StringUtils::ltos(Simulator::GetInstance()->m_dwDelayMax).c_str(), MAX_STRING_LEN);
	::SetWindowText(g_hWndRandom_DelayMax, wTextBuffer);

	mbstowcs(wTextBuffer, StringUtils::ltos(Simulator::GetInstance()->m_iNumberOfButtonsPerClick).c_str(), MAX_STRING_LEN);
	::SetWindowText(g_hWndRandom_ButtonsPerClick, wTextBuffer);

	mbstowcs(wTextBuffer, Simulator::GetInstance()->m_sHomeScreen.c_str(), MAX_STRING_LEN);
	::SetWindowText(g_hWndRandom_HomeEdit, wTextBuffer);

	mbstowcs(wTextBuffer, Simulator::GetInstance()->m_sDeviceID.c_str(), MAX_STRING_LEN);
	::SetWindowText(g_hWnd_DeviceIDEdit, wTextBuffer);

	mbstowcs(wTextBuffer, Simulator::GetInstance()->m_sRouterIP.c_str(), MAX_STRING_LEN);
	::SetWindowText(g_hWnd_RouterIPEdit, wTextBuffer);
#else
	::SetWindowText(g_hWndRandom_DelayMin, 
		StringUtils::ltos(Simulator::GetInstance()->m_dwDelayMin).c_str());
	::SetWindowText(g_hWndRandom_DelayMax, 
		StringUtils::ltos(Simulator::GetInstance()->m_dwDelayMax).c_str());
	::SetWindowText(g_hWndRandom_ButtonsPerClick, 
		StringUtils::ltos(Simulator::GetInstance()->m_iNumberOfButtonsPerClick).c_str());

	::SetWindowText(g_hWndRandom_HomeEdit, Simulator::GetInstance()->m_sHomeScreen.c_str());

	::SetWindowText(g_hWnd_DeviceIDEdit, Simulator::GetInstance()->m_sDeviceID.c_str());
	::SetWindowText(g_hWnd_RouterIPEdit, Simulator::GetInstance()->m_sRouterIP.c_str());
#endif

	switch(Simulator::GetInstance()->m_iKeysSetToGenerate)
	{
		case 0: ::SendMessage(g_hWndRandom_KeyOption1RadioBox, BM_SETCHECK, BST_CHECKED, 0); break;
		case 1: ::SendMessage(g_hWndRandom_KeyOption2RadioBox, BM_SETCHECK, BST_CHECKED, 0); break;
		case 2: ::SendMessage(g_hWndRandom_KeyOption3RadioBox, BM_SETCHECK, BST_CHECKED, 0); break;
	}

	::SendMessage(g_hWnd_FullScreenCheckBox, BM_SETCHECK, 
		Simulator::GetInstance()->m_bFullScreen ? BST_CHECKED : BST_UNCHECKED, 0);
	::SendMessage(g_hWnd_LogToServerCheckBox, BM_SETCHECK, 
		Simulator::GetInstance()->m_bLogToServer ? BST_CHECKED : BST_UNCHECKED, 0);
}
//-----------------------------------------------------------------------------------------------------
#pragma warning( default : 4311 4312)
//-----------------------------------------------------------------------------------------------------
