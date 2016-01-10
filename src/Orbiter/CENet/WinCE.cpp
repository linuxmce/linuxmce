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
#include <time.h>
#include "wince.h"



// Windows CE implementations of normal C functions.   Grrr..
static struct tm tb = { 0 };    /* time block */

static int _lpdays[] = { -1, 30, 59, 90, 120, 151, 181, 212, 243, 273,
304, 334, 365 };
static int _days[] = { -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303,
333, 364 };


struct tm * __cdecl gmtime (
        time_t *timp
        )
{

        time_t caltim = *timp;          /* calendar time to convert */
        int islpyr = 0;                 /* is-current-year-a-leap-year flag */
        int tmptim;
        int *mdays;                /* pointer to days or lpdays */

        struct tm *ptb = &tb;



        /*
         * Determine years since 1970. First, identify the four-year interval
         * since this makes handling leap-years easy (note that 2000 IS a
         * leap year and 2100 is out-of-range).
         */
        tmptim = (int)(caltim / _FOUR_YEAR_SEC);
        caltim -= ((time_t)tmptim * _FOUR_YEAR_SEC);

        /*
         * Determine which year of the interval
         */
        tmptim = (tmptim * 4) + 70;         /* 1970, 1974, 1978,...,etc. */

        if ( caltim >= _YEAR_SEC ) {

            tmptim++;                       /* 1971, 1975, 1979,...,etc. */
            caltim -= _YEAR_SEC;

            if ( caltim >= _YEAR_SEC ) {

                tmptim++;                   /* 1972, 1976, 1980,...,etc. */
                caltim -= _YEAR_SEC;

                /*
                 * Note, it takes 366 days-worth of seconds to get past a leap
                 * year.
                 */
                if ( caltim >= (_YEAR_SEC + _DAY_SEC) ) {

                        tmptim++;           /* 1973, 1977, 1981,...,etc. */
                        caltim -= (_YEAR_SEC + _DAY_SEC);
                }
                else {
                        /*
                         * In a leap year after all, set the flag.
                         */
                        islpyr++;
                }
            }
        }

        /*
         * tmptim now holds the value for tm_year. caltim now holds the
         * number of elapsed seconds since the beginning of that year.
         */
        ptb->tm_year = tmptim;

        /*
         * Determine days since January 1 (0 - 365). This is the tm_yday value.
         * Leave caltim with number of elapsed seconds in that day.
         */
        ptb->tm_yday = (int)(caltim / _DAY_SEC);
        caltim -= (time_t)(ptb->tm_yday) * _DAY_SEC;

        /*
         * Determine months since January (0 - 11) and day of month (1 - 31)
         */
        if ( islpyr )
            mdays = _lpdays;
        else
            mdays = _days;


        for ( tmptim = 1 ; mdays[tmptim] < ptb->tm_yday ; tmptim++ ) ;

        ptb->tm_mon = --tmptim;

        ptb->tm_mday = ptb->tm_yday - mdays[tmptim];

        /*
         * Determine days since Sunday (0 - 6)
         */
        ptb->tm_wday = ((int)(*timp / _DAY_SEC) + _BASE_DOW) % 7;

        /*
         *  Determine hours since midnight (0 - 23), minutes after the hour
         *  (0 - 59), and seconds after the minute (0 - 59).
         */
        ptb->tm_hour = (int)(caltim / 3600);
        caltim -= (time_t)ptb->tm_hour * 3600L;

        ptb->tm_min = (int)(caltim / 60);
        ptb->tm_sec = (int)(caltim - (ptb->tm_min) * 60);

        ptb->tm_isdst = 0;
        return( (struct tm *)ptb );

}

time_t timediff()
{
	static time_t diff = -1;

    if (diff==-1)
	{
		SYSTEMTIME sLocaltime, sGmtime;
		::GetSystemTime(&sGmtime);
		::GetLocalTime(&sLocaltime);
		
		FILETIME fLocaltime, fGmtime;
		::SystemTimeToFileTime(&sLocaltime, &fLocaltime);
		::SystemTimeToFileTime(&sGmtime, &fGmtime);
		ULARGE_INTEGER liLocaltime, liGmtime;
		memcpy(&liLocaltime, &fLocaltime, sizeof(ULARGE_INTEGER));
		memcpy(&liGmtime, &fGmtime, sizeof(ULARGE_INTEGER));
		liLocaltime.QuadPart /= 10000000;
		liGmtime.QuadPart /= 10000000;

		diff = (time_t)((ULONGLONG )liLocaltime.QuadPart - (ULONGLONG )liGmtime.QuadPart);
	}
	return diff;
}

struct tm *wince_localtime(time_t *ptime)
{
	time_t adjustedtime = *ptime + timediff();
	return gmtime(&adjustedtime);
}

time_t wince_time(time_t *pT)
{
	SYSTEMTIME stime;
	::GetSystemTime(&stime);

	FILETIME ftime;
	::SystemTimeToFileTime(&stime, &ftime);
	ULARGE_INTEGER li;
	memcpy(&li, &ftime, sizeof(ULARGE_INTEGER));
	li.QuadPart /= 10000000;

	time_t val = (time_t)((ULONGLONG )li.QuadPart - (ULONGLONG)11644473600);
	if (pT)
		*pT = val;
	return val;
}

char *wince_strdup(const char *s)
{
	char *newstring = (char *)malloc(strlen(s)+1);
	if (newstring)
		strcpy(newstring, s);
	return newstring;
}

time_t wince_mktime(tm *)
{
	//TODO be implemented!!!
	return 0;
}

