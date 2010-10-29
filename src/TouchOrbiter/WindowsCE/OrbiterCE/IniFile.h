#ifndef INIFILE_H
#define INIFILE_H

#include <windows.h>

struct IniData
{
	const char *Key;
	const char *Value;
};

struct IniStore
{
	struct IniData *Data;
	int Count;
	CRITICAL_SECTION CriticalSection;
};

BOOL InitIniFile(struct IniStore *IniStore);
BOOL ReadIniFile(LPCWSTR Filename, struct IniStore *IniStore);
const char * GetIniData(struct IniStore *IniStore, const char *Key);

#endif /*INIFILE_H*/
