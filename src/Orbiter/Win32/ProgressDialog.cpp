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
#include "ProgressDialog.h"
#include "Windows.h"
#include "Resource.h"
#include "MainDialog.h"

static bool g_bStopPopupMessage = false;
static bool g_bPopupMessageIsRunning = false;

static HWND	g_hwndPopupDialog;		// The main dialog window handle
static HWND g_hwndCancelButton;
static DWORD g_dwWorkerThreadId;

static int g_nWindowWidth, g_nWindowHeight;
static const int MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
static bool g_bResponse;
static string g_sMessage;
static int g_nProgress;

#define WINDOW_TITLE     30
#define BUTTON_WIDTH     100
#define BUTTON_HEIGHT    20
#define LABEL_HEIGHT     40
#define PROGRESS_HEIGHT  30
#define BUTTON_SEPARATOR 10
//-----------------------------------------------------------------------------------------------------
static HWND CreateButton(HWND hParentWnd, int x, int y, int width, int height, const char* caption)
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
		WS_CHILD | WS_VISIBLE | BS_CENTER | BS_VCENTER | BS_PUSHBUTTON | WS_TABSTOP, 
		rt_cb.left, rt_cb.top, rt_cb.right, rt_cb.bottom, 
		hParentWnd, NULL, g_hInst, NULL
		);

	return hWndList;
}
//-----------------------------------------------------------------------------------------------------
void DisplayProgressBar(HWND hWnd, HDC hdc)
{
    RECT rt;
    HBRUSH hBrush;
    rt.left = BUTTON_SEPARATOR;
    rt.top = 3 * BUTTON_SEPARATOR + LABEL_HEIGHT;
    rt.right = rt.left + g_nWindowWidth - 3 * BUTTON_SEPARATOR;
    rt.bottom = rt.top + PROGRESS_HEIGHT;

    COLORREF crProgrBkColor = ::GetSysColor(COLOR_3DSHADOW);
    hBrush = CreateSolidBrush(crProgrBkColor);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    FillRect(hdc, &rt, hBrush);
	SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    rt.left++;
    rt.top++;
    rt.right = rt.left + (g_nProgress * (g_nWindowWidth - 3 * BUTTON_SEPARATOR)) / 100 - 1;
    rt.bottom --;

    hBrush = CreateSolidBrush(RGB(0, 0, 255));
    HBRUSH hOldBrush2 = (HBRUSH)SelectObject(hdc, hBrush);
    FillRect(hdc, &rt, hBrush);
	SelectObject(hdc, hOldBrush2);
    DeleteObject(hBrush);
}
//-----------------------------------------------------------------------------------------------------
static void DisplayMessage(HWND hWnd, HDC hdc)
{
    RECT rectLocation = { BUTTON_SEPARATOR, BUTTON_SEPARATOR, 
        g_nWindowWidth - 2 * BUTTON_SEPARATOR, 2 * BUTTON_SEPARATOR + LABEL_HEIGHT }; 

    COLORREF crBkColor = ::GetSysColor(COLOR_3DFACE);
    HBRUSH hBrush = CreateSolidBrush(crBkColor);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    ::FillRect(hdc, &rectLocation, hBrush);
	SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    ::SetTextColor(hdc, RGB(0, 0, 0));
    ::SetBkMode(hdc, TRANSPARENT);

#ifdef WINCE
    wchar_t wText[1024];
    mbstowcs(wText, g_sMessage.c_str(), 1024);
    ::DrawText(hdc, wText, int(g_sMessage.length()), &rectLocation, 
        DT_WORDBREAK | DT_NOPREFIX | DT_VCENTER); 
#else
    ::DrawText(hdc, g_sMessage.c_str(), int(g_sMessage.length()), &rectLocation, 
        DT_WORDBREAK | DT_NOPREFIX | DT_VCENTER); 
#endif
}
//-----------------------------------------------------------------------------------------------------
static void AdjustWindowSize()
{
    RECT rc;
    HWND hWndDesktop = ::GetDesktopWindow();
    ::GetWindowRect(hWndDesktop, &rc);

    int iDesktopWidth = rc.right - rc.left;
    int iDesktopHeight = rc.bottom - rc.top;

#ifdef WINCE
	MoveWindow(g_hwndPopupDialog, 0, 0, g_nWindowWidth, g_nWindowHeight, TRUE);
#else
	rc.left = (iDesktopWidth - g_nWindowWidth) / 2;
	rc.right = rc.left + g_nWindowWidth;
	rc.top = (iDesktopHeight - g_nWindowHeight) / 2;
	rc.bottom = rc.top + g_nWindowHeight;
	MoveWindow(g_hwndPopupDialog, rc.left, rc.top, g_nWindowWidth, g_nWindowHeight, TRUE);
#endif
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
LRESULT CALLBACK WndProcDialog(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
                    if((LPARAM)g_hwndCancelButton == lParam)
                        g_bResponse = true;
					break;
				}
			}
			break;

		case WM_CREATE:
			{
                g_hwndCancelButton = CreateButton(hWnd, 
                        (g_nWindowWidth - BUTTON_WIDTH) / 2,
                        LABEL_HEIGHT + PROGRESS_HEIGHT + 4 * BUTTON_SEPARATOR + 5,
                        BUTTON_WIDTH, BUTTON_HEIGHT, "Cancel");

                g_nWindowHeight = min(g_nWindowHeight, 
                    WINDOW_TITLE + BUTTON_SEPARATOR + 
                    LABEL_HEIGHT + BUTTON_SEPARATOR + BUTTON_SEPARATOR + 
                    PROGRESS_HEIGHT + BUTTON_SEPARATOR +  
                    BUTTON_HEIGHT + BUTTON_SEPARATOR * 2);
			}
			break;

		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);

                RECT rt;
                GetClientRect(hWnd, &rt);
                COLORREF crBkColor = ::GetSysColor(COLOR_3DFACE);
                HBRUSH hBrush = CreateSolidBrush(crBkColor);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
                FillRect(hdc, &rt, hBrush);
				SelectObject(hdc, hOldBrush);
                DeleteObject(hBrush);

                DisplayProgressBar(hWnd, hdc);
                DisplayMessage(hWnd, hdc);
       			EndPaint(hWnd, &ps);
			}
			break; 

        case WM_APP:
            {
                HDC hdc = ::GetDC(g_hwndPopupDialog);
                DisplayProgressBar(g_hwndPopupDialog, hdc);
                DisplayMessage(g_hwndPopupDialog, hdc);
                ::ReleaseDC(g_hwndPopupDialog, hdc);
            }
            break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
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
WORD MyRegisterClassDialog(HINSTANCE hInstance, LPTSTR szWindowClass)
{
#ifdef WINCE
	WNDCLASS	wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC) WndProcDialog;
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
	wcex.lpfnWndProc	= (WNDPROC)WndProcDialog;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ORBITER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //(LPCTSTR)IDC_ORBITER;
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
BOOL InitInstanceDialog(HINSTANCE hInstance, int nCmdShow)
{
#ifdef WINCE    
    g_nWindowWidth = 240;
    g_nWindowHeight = 280;
#else
    g_nWindowWidth = 400;
    g_nWindowHeight = 400;
#endif

	HWND hWnd = NULL;
	g_hInst = hInstance;		// Store instance handle in our global variable

	LPTSTR szWindowClass = TEXT("PlutoProgressDialog");
	LPTSTR szTitle = TEXT("Pluto Orbiter");

	MyRegisterClassDialog(hInstance, szWindowClass);

#ifdef WINCE
    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
#else
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
#endif

	if (!hWnd)
		return FALSE;

	g_hwndPopupDialog = hWnd;
    AdjustWindowSize();
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOSIZE);

#ifdef WINCE
	ShowWindow(hWnd, nCmdShow);
#else
	HWND hMainWindow = ::FindWindow(TEXT("ORBITER"), NULL);

	if(!IsIconic(hMainWindow)) //if main window is minimezed, create minimized
		ShowWindow(hWnd, nCmdShow);
	else
		ShowWindow(hWnd, SW_MINIMIZE);
#endif

	UpdateWindow(hWnd);

	return TRUE;
}


DWORD WINAPI DialogMessageThread( LPVOID lpParameter)
{
    InitInstanceDialog(g_hInst, 1);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) && !g_bStopPopupMessage) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_bPopupMessageIsRunning = false;
    return 0L;
}

void UpdateProgress()
{
    ::PostMessage(g_hwndPopupDialog, WM_APP, 0, 0);
}

bool DialogProgressEx(string sMessage, int nProgress)
{
    if(!nProgress)
        g_bResponse = false;

	if( nProgress>100 )
		nProgress=100;

    g_sMessage = sMessage;
    g_nProgress = nProgress;

    if(!g_bPopupMessageIsRunning)
    {
        g_bPopupMessageIsRunning = true;
        g_bStopPopupMessage = false;
        ::CreateThread(NULL, 0, DialogMessageThread, 0, 0, &g_dwWorkerThreadId);	
    }
    else
        UpdateProgress();

    if(g_nProgress > 100 || g_nProgress < 0)
        PostMessage(g_hwndPopupDialog, WM_CLOSE, 0, 0);

    if(g_bResponse)
        PostMessage(g_hwndPopupDialog, WM_CLOSE, 0, 0);

    return g_bResponse;
}