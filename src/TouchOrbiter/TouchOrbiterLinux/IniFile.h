#ifndef INIFILE_H
#define INIFILE_H

#include "custom.h"

struct IniData
{
	const char *Key;
	const char *Value;
};

struct IniStore
{
	struct IniData *Data;
	int Count;
	pthread_mutex_t CriticalSection;
};

char InitIniFile(struct IniStore *IniStore);
char ReadIniFile(char *Filename, struct IniStore *IniStore);
const char * GetIniData(struct IniStore *IniStore, const char *Key);

#endif /*INIFILE_H*/
