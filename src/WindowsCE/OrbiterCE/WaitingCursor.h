#ifndef WAITING_CURSOR
#define WAITING_CURSOR

#include <windows.h>
#include "IniFile.h"

BOOL FileReadToRAM(LPWSTR filename, unsigned char **bufferbin, unsigned long int *length);
void InitWaitingCursor(HWND parmhwnd, LPCWSTR WorkDir, struct IniStore *IniStore);
void StartWaitingCursor();
void StopWaitingCursor();

#endif /*WAITING_CURSOR*/
