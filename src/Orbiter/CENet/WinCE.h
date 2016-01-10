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
#ifndef __PLUTO_WINCE_H__
#define __PLUTO_WINCE_H__

#include <windows.h>
//#include <windef.h>

#ifdef ARMV4I
	#define time_t __int64
#endif

#define endl '\n'
#define localtime(x) wince_localtime(x)
#define time(x) wince_time(x)
#define strdup(x) wince_strdup(x)
#define mktime(x) wince_mktime(x)


#define _CLOCALECP      CP_ACP  /* "C" locale Code page */
#define _DAY_SEC           (24L * 60L * 60L)    /* secs in a day */
#define _YEAR_SEC          (365L * _DAY_SEC)    /* secs in a year */
#define _FOUR_YEAR_SEC     (1461L * _DAY_SEC)   /* secs in a 4 year interval */
#define _DEC_SEC           315532800L           /* secs in 1970-1979 */
#define _BASE_YEAR         70L                  /* 1970 is the base year */
#define _BASE_DOW          4                    /* 01-01-70 was a Thursday */
#define _LEAP_YEAR_ADJUST  17L                  /* Leap years 1900 - 1970 */
#define _MAX_YEAR          138L                 /* 2038 is the max year */
#define DAY_MILLISEC    (24L * 60L * 60L * 1000L)
#define IS_LEAP_YEAR(year)  ( (year & 3) == 0 )

struct tm *wince_localtime(time_t *);
time_t wince_time(time_t *);
char *wince_strdup(const char *s);
time_t wince_mktime(tm *);

// CE doesn't have a clock() function but it does have a Tick function

#define clock_t DWORD
#define clock() GetTickCount()
#define CLOCKS_PER_SEC 1000

#endif //__PLUTO_WINCE_H__