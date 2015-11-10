/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file CommonIncludes.h
 Header file for the DeviceCategory, DeviceData_Base and AllDevices classes

 @todo not commented
 */
#ifndef COMMONINCLUDES_H
#define COMMONINCLUDES_H

#include <string>
#include <map>
#include <list>
#include <vector>

#ifndef _WIN32_WCE
#include <fstream>
#endif //_WIN32_WCE

#include <stdarg.h>

using namespace std;

#ifndef WIN32
	// Linux stuff ------------------------
	#include <stdio.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <sys/stat.h>
#ifdef ANDROID
        #include <signal.h>
#else
	#include <sys/signal.h>
#endif
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netinet/tcp.h>

	#include <netdb.h>
	#include <string.h>

	//#define Sleep(x)   usleep(x*1000)
	//#define Sleep(x) { struct timeval t; t.tv_sec = (x / 1000) ; t.tv_usec = (x % 1000) * 1000000; select(0, NULL, NULL, NULL, &t); }
	void Sleep(int miliseconds);

#if !defined(MOXI)
	#define stricmp(x, y) strcasecmp(x, y)
#endif

	typedef unsigned int UINT;
	typedef unsigned int LPARAM;
	typedef unsigned int WPARAM;
	#define LPVOID void *
	#define SOCKET int
	#define SOCKFAIL(x) (x < 0)
	#define SOCKOPTTYPE void *
	#define INVALID_SOCKET -1
	#define closesocket(a) close(a)
	#define UINT64_PRINTF "%llu"
#else
// Windows Stuff ----------------------
	#pragma warning( disable : 4996 )

	#define trunc(s) s

#ifndef WIN32
	typedef __int8  int8_t;
#endif
	typedef __int16 int16_t;
	typedef __int32 int32_t;
	typedef __int64 int64_t;
	typedef unsigned __int8  uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 u_int64_t;
	typedef int socklen_t;
	#define u_int64_t __int64

	#define snprintf _snprintf // @#$#!%!!

	#include <windows.h>
	#include <objbase.h>
	#ifndef UNDER_CE
		#include <io.h>
	#else
		struct tm * __stdcall wce_localtime(const time_t *);
		time_t		__stdcall wce_time(time_t *);
	#endif

	#define SOCKOPTTYPE char *
	#include <time.h>
	#include <winsock.h>
	#include <TCHAR.H>

	#define vsnprintf _vsnprintf
	#define SOCKFAIL(x) (x == SOCKET_ERROR)
	#define UINT64_PRINTF "%I64u"
#endif

#ifdef PROFILING
	#define PROFILE_START(x) clock_t x = clock();
	#define PROFILE_STOP(x, Desc)	LoggerWrapper::GetInstance()->Write(LV_WARNING, "%s took %d ms", Desc, clock()-x);

	#define PROF_START() clock_t x = clock();
	#define PROF_STOP(Desc) LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s took %d ms", Desc, clock() - x);
#else
	#define PROFILE_START(x)
	#define PROFILE_STOP(x, Desc)

	#define PROF_START()
	#define PROF_STOP(Desc)
#endif

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#define STRINGIFY(x) #x  // Allows you to have a #define that's a number (#define MYNUM 2) that you concatenate to a string: string x = "abc" + TOSTRING(MYNUM)
#define TOSTRING(x) STRINGIFY(x)

#ifndef _WIN32_WCE


    /**
       @class PlutoFile
     This class ???
     */
class PlutoFile
{
public:
	int *m_FD;
	PlutoFile(int *FD)
	{
		m_FD = FD;
	}
	~PlutoFile()
	{
		if (*m_FD)
		{
			close(*m_FD);
			*m_FD = 0;
		}
	}
};
#endif

#ifdef CENET
	extern "C" // Use extern "c" for C++ file only
	{
		#include "PKFUNCS.H"
	};
#endif

bool AskYNQuestion(string Question,bool bDefault,int Timeout=0);
char AskMCQuestion(string Question,string Prompts,int Timeout=0);
string GetPrompt(int Timeout);
#endif
