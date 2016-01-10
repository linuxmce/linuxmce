
#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <shlwapi.h>
#include <sddl.h>
#include <Lmcons.h>

#pragma warning(disable: 4995)

#include <strsafe.h>

#include <string>
#include <vector>
#include <process.h>
#include <sstream>
#include <map>

#ifdef _DEBUG
#	include <fstream>
#endif // _DEBUG

#pragma warning(default: 4995)

#import "msxml4.dll"

#pragma warning(disable: 4290)
