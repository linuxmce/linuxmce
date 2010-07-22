#include "WaitingCursor.h"
#include "TransformBitmap.h"
#include "PngFile.h"

static HWND hwnd = NULL;
static CRITICAL_SECTION CursorCriticalSection;
static BOOL RunCursor = FALSE;

#define CursorWidth 32
#define CursorHeight 32
#define NIMAGES 8
#define ImgName_wprintf L"%s\\WaitCursor\\waiting-%d.png"

static HBITMAP CursorBitmap[NIMAGES];

static BOOL RunCursorIsTrue()
{
	BOOL RunCursorValue;

	EnterCriticalSection(&CursorCriticalSection);
	RunCursorValue = RunCursor;
	LeaveCriticalSection(&CursorCriticalSection);

	return RunCursorValue;
}

static DWORD WaitingCursorThread(LPVOID lpParameter __attribute__((unused)))
{
	int CursorIdx = 0;
	HDC wDC;

	int CursorLeft = (GetSystemMetrics(SM_CXSCREEN) - CursorWidth) / 2;
	int CursorTop = (GetSystemMetrics(SM_CYSCREEN) - CursorHeight) / 2;

	wDC = GetDC(hwnd);
	HDC bmpDC = CreateCompatibleDC(wDC);
	ReleaseDC(hwnd, wDC);

	while (RunCursorIsTrue())
	{
		wDC = GetDC(hwnd);

		HBITMAP origBmp = SelectObject(bmpDC, CursorBitmap[CursorIdx]);
		BitBlt(wDC, CursorLeft, CursorTop, CursorWidth, CursorHeight, bmpDC, 0, 0, SRCCOPY);
		SelectObject(bmpDC, origBmp);

		ReleaseDC(hwnd, wDC);
		CursorIdx++;
		CursorIdx %= 8;
		Sleep(100);
	}

	DeleteDC(bmpDC);
	return 0;
}

BOOL FileReadToRAM(LPWSTR filename, unsigned char **bufferbin, unsigned long int *length)
{
	if (bufferbin == NULL || *bufferbin != NULL || length == NULL)
		return FALSE;

	FILE *f = _wfopen(filename, L"rb");
	if (f == NULL)
		return FALSE;

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);
	*bufferbin = (unsigned char *) malloc(*length);
	fread(*bufferbin, 1, *length, f);
	fclose(f);

	return TRUE;
}

void InitWaitingCursor(HWND parmhwnd, LPCWSTR WorkDir, struct IniStore *IniStore)
{
	int i;

	char *Visible = GetIniData(IniStore, "BusyCursor.Visible");
	if (Visible == NULL)
		return;
	if (strcmp(Visible, "Yes") != 0)
	{
		free(Visible);
		return;
	}
	free(Visible);

	// TODO: Use BusyCursor.CursorSet, BusyCursor.AnchorTop, BusyCursor.AnchorLeft, BusyCursor.OffsetX, BusyCursor.OffsetY
	// TODO: Determine image count, width and height automatically
#if 0
	char *CursorSet = GetIniData(IniStore, "BusyCursor.CursorSet");
	if (CursorSet == NULL)
		return;
	if (strlen(CursorSet) == 0)
	{
		free(CursorSet);
		return;
	}

	// Open cursor set
#endif

	hwnd = parmhwnd;
	InitializeCriticalSection(&CursorCriticalSection);

	for (i = 0; i < NIMAGES; i++)
	{
		WCHAR filename[MAX_PATH];
		memset(filename, 0, MAX_PATH * sizeof(WCHAR));
		snwprintf(filename, MAX_PATH - 1, ImgName_wprintf, WorkDir, i);

		unsigned char *buffer = NULL;
		unsigned long int length = 0;
		if (!FileReadToRAM(filename, &buffer, &length))
			MessageBox(hwnd, L"Failed to load cursor from disk", filename, MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
		HBITMAP BitmapFromPng = PngLoadImageFromRAM(buffer, length);
		if (BitmapFromPng == NULL)
			MessageBox(hwnd, L"Failed to decompress cursor", filename, MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
		CursorBitmap[i] = GetCompatibleBitmap(hwnd, BitmapFromPng);

		DeleteObject(BitmapFromPng);
		free(buffer);
		buffer = NULL;
	}
}

void StartWaitingCursor()
{
	EnterCriticalSection(&CursorCriticalSection);
	if (RunCursor == TRUE)
	{
		LeaveCriticalSection(&CursorCriticalSection);
		return;
	}

	RunCursor = TRUE;
	HANDLE ThreadHandle = CreateThread(NULL, 0, WaitingCursorThread, NULL, 0, NULL);
	CloseHandle(ThreadHandle);

	LeaveCriticalSection(&CursorCriticalSection);
}

void StopWaitingCursor()
{
	EnterCriticalSection(&CursorCriticalSection);
	if (RunCursor == FALSE)
	{
		LeaveCriticalSection(&CursorCriticalSection);
		return;
	}

	RunCursor = FALSE;
	LeaveCriticalSection(&CursorCriticalSection);
}
