// switches

// defines

#define PROGNAME L"Tablet Orbiter"
#define LONGNAME L"Tablet Orbiter"
#define VERSION L"0.1"

// constants

// standard includes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>
#include <aygshell.h>

// application includes

#include "png.h"
#include "PngFile.h"
#include "JpegFile.h"
#include "resource.h"

#include "ProxyOrbiter.h"
#include "CeGccSupplement.h"
#include "TransformBitmap.h"
#include "WaitingCursor.h"
#include "IniFile.h"

// macros

// function prototypes

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

void DisplayImage(HDC wndDC, HBITMAP Bitmap);
void GetImage(SOCKET Socket, HWND hwnd, HBITMAP *Bitmap);
LPWSTR GetStartDir(HINSTANCE hInstance, LPWSTR StartDir, int Length);

// a few global variables

static WCHAR *szProgName = PROGNAME;
static WCHAR *szAppName = LONGNAME;
static WCHAR *szIconName = PROGNAME;

static const unsigned char *ServerIP, *ServerPortStr;
static unsigned int ServerPort;

static CommSocket = INVALID_SOCKET;
static HBITMAP OffScreenBitmap = NULL;
static CRITICAL_SECTION BitmapCriticalSection;
static WCHAR StartDir[MAX_PATH];

static struct IniStore IniStore;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPWSTR szCmdLine, int iCmdShow)
{
	HACCEL hAccel;
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	InitializeCriticalSection(&BitmapCriticalSection);

	GetStartDir(hInstance, StartDir, MAX_PATH);

	WCHAR IniFile[MAX_PATH];
	memset(IniFile, 0, MAX_PATH * sizeof(WCHAR));
	snwprintf(IniFile, MAX_PATH - 1, L"%s\\OrbiterCE.ini", StartDir);
	InitIniFile(&IniStore);
	if (!ReadIniFile(IniFile, &IniStore))
	{
		MessageBox(NULL, L"Failed to open configuration", szProgName, MB_ICONERROR);
		return 0;
	}

	ServerIP = GetIniData(&IniStore, "Network.ServerIP");
	ServerPortStr = GetIniData(&IniStore, "Network.ServerPort");

	if (ServerIP == NULL || ServerPortStr == NULL)
	{
		MessageBox(NULL, L"Failed to parse configuration", szProgName, MB_ICONERROR);
		return 0;
	}

	ServerPort = atoi(ServerPortStr);

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, szIconName) ;
	wndclass.hCursor = NULL; /*LoadCursor (NULL, IDC_ARROW);*/
	wndclass.hbrBackground = NULL; /*(HBRUSH) GetStockObject(GRAY_BRUSH);*/
	wndclass.lpszMenuName = NULL; /*szProgName;*/
	wndclass.lpszClassName = szProgName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Error: Failed to register window class"),
				szProgName, MB_ICONERROR);
		return 0;
	}

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox(NULL, TEXT ("Error: can't start up sockets"),
				szProgName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szProgName, szAppName,
			WS_POPUP,
			0, 0,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, hInstance, NULL);

	InitWaitingCursor(hwnd, StartDir, &IniStore);

	ShowWindow (hwnd, iCmdShow);
	UpdateWindow (hwnd);

	hAccel = LoadAccelerators(hInstance, szProgName);

	ProxyOrbiter_PollNews(ServerIP, ServerPort, hwnd, WM_APP + 0);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	WSACleanup();
	return msg.wParam;
}

LPWSTR GetStartDir(HINSTANCE hInstance, LPWSTR StartDir, int Length)
{
	if (StartDir == NULL || Length <= 0)
		return NULL;
	if (Length > MAX_PATH)
		Length = MAX_PATH;

	GetModuleFileName(hInstance, StartDir, Length);
	WCHAR *LastBackslash = wcsrchr(StartDir, L'\\');
	*LastBackslash = L'\x00';

	return StartDir;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance ;
	static PAINTSTRUCT ps;

	static int cxWinSize, cyWinSize;

	switch (message)
	{
		case WM_CREATE:
		{
			hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

			int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, ScreenWidth, ScreenHeight, 0);
			HWND TaskBarWnd = FindWindow(L"HHTaskBar", L"");
			if (TaskBarWnd != NULL)
				SetWindowPos(TaskBarWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);

			CommSocket = ProxyOrbiter_Connect(ServerIP, ServerPort);
			if (CommSocket == INVALID_SOCKET)
				MessageBox(NULL, TEXT ("Connection failed"), L"", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);

			GetImage(CommSocket, hwnd, &OffScreenBitmap);
			InvalidateRect(hwnd, NULL, FALSE);

			return 0;
		}

		case WM_SIZE:
			cxWinSize = LOWORD(lParam);
			cyWinSize = HIWORD(lParam);

			InvalidateRect(hwnd, NULL, FALSE);

			return 0;

		case WM_PAINT:
		{
			StopWaitingCursor();
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);

			DisplayImage(hdc, OffScreenBitmap);

			EndPaint(hwnd, &ps);
			return 0;
		}

		case WM_DESTROY:
			if (OffScreenBitmap != NULL)
			{
				DeleteObject(OffScreenBitmap);
				OffScreenBitmap = NULL;
			}
			closesocket(CommSocket);
			PostQuitMessage(0);
			return 0;

		case WM_LBUTTONDOWN:
		{
			int X = LOWORD(lParam);
			int Y = HIWORD(lParam);
			SHRGINFO shrg;

			shrg.cbSize = sizeof(shrg);
			shrg.hwndClient = hwnd;
			shrg.ptDown.x = X;
			shrg.ptDown.y = Y;
			shrg.dwFlags = SHRG_RETURNCMD;

			if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU)
			{
				// Tap and hold
				int button = MessageBox(hwnd, TEXT ("Close application?"),
						szProgName, MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL);
				if (button == IDYES)
					PostQuitMessage(0);
			}
			else
			{
				// Tap and release
				StartWaitingCursor();
				ProxyOrbiter_Touch(CommSocket, X, Y);
				GetImage(CommSocket, hwnd, &OffScreenBitmap);
				InvalidateRect (hwnd, NULL, FALSE);
			}
			return 0;
		}

		case WM_APP + 0:
			if (lParam == 1 || lParam == 2)
			{
				StartWaitingCursor();
				GetImage(CommSocket, hwnd, &OffScreenBitmap);
				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;

		case WM_APP + 1:
			CommSocket = ProxyOrbiter_Connect(ServerIP, ServerPort);
			return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

void DisplayImage(HDC wDC, HBITMAP Bitmap)
{
	if (Bitmap == NULL || wDC == NULL)
		return;

	HDC bmpDC = CreateCompatibleDC(wDC);
	HBITMAP tmpBmp = SelectObject(bmpDC, Bitmap);

	BITMAP BmpInfo;
	GetObject(Bitmap, sizeof(BITMAP), &BmpInfo);

	BitBlt(wDC, 0, 0, BmpInfo.bmWidth, BmpInfo.bmHeight, bmpDC, 0, 0, SRCCOPY);

	SelectObject(bmpDC, tmpBmp);
	DeleteDC(bmpDC);
}

void GetImage(SOCKET Socket, HWND hwnd, HBITMAP *Bitmap)
{
	if (Bitmap == NULL)
		return;
	if (Socket == INVALID_SOCKET)
		return;
	if (*Bitmap != NULL)
		DeleteObject(*Bitmap);

	unsigned long int length = 0;
	png_byte *buffer = NULL;

	BOOL success = ProxyOrbiter_GetImage(Socket, &buffer, &length);
	if (!success)
	{
		PostMessage(hwnd, WM_APP + 1, 0, 0);

		WCHAR filename[MAX_PATH];
		memset(filename, 0, MAX_PATH * sizeof(WCHAR));

		snwprintf(filename, MAX_PATH - 1, L"%s\\NotConnected.png", StartDir);
		if (!FileReadToRAM(filename, &buffer, &length))
			return;
	}
	HBITMAP LoadedBitmap = PngLoadImageFromRAM(buffer, length);
	if (LoadedBitmap == NULL)
		MessageBox(NULL, TEXT("Failed to load PNG"), L"", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
	free(buffer);

	*Bitmap = GetCompatibleBitmap(hwnd, LoadedBitmap);
	DeleteObject(LoadedBitmap);
}
