
#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <atlbase.h>
#include <shlobj.h>
#include <Aclapi.h>
#include <Winsock2.h>

#include <time.h>

#import "msxml4.dll"

#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>

#ifdef _DEBUG
#	include <fstream>
#	include <iomanip>
#endif // _DEBUG

#include "iTunesCOMInterface.h"

#pragma warning(disable: 4290)

