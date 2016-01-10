/*
	Other

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "Other.h"

#ifndef SYMBIAN
	#include <stdio.h>
	#include <time.h>
	#include <cctype>
	#include <algorithm>
	#include <stdarg.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#ifdef WIN32
		#include <direct.h>
	#else
		#include <dirent.h>
		#define stricmp(x, y) strcasecmp(x, y)
	#endif

    #include "StringUtils.h"
#endif //#ifndef SYMBIAN

#ifndef WIN32
#include <sys/times.h>
#endif

int FastParser::GetNextNumParm()
{
    if ( !(*m_pcData) || IsAtEnd() )
        return 0;

    register int c;
    int total=0;
    int sign;

    c = (int)*m_pcData++;
    sign = c;
    if (c == '-' || c == '+')
        c = (int)*m_pcData++;

    do
    {
        if (c==m_cDelimiter)
            break;
        if ( c>='0' && c<='9' )
            total = 10 * total + (c-'0');
        c = *m_pcData++;
    }  while(c); //(*m_Data);

    if ( sign == '-' )
        return -total;
    else
        return total;
}


const char *FastParser::GetNextParm()
{
    if ( IsAtEnd() )
        return NULL;

    char *pcReturnValue = m_pcData;

    for ( ; *m_pcData ; m_pcData++ )
        if ( *m_pcData == m_cDelimiter)
        {
            *m_pcData = '\0';
            break;
        }
    m_pcData++;
    return (const char *)pcReturnValue;
}

#ifndef SYMBIAN
string GenerateCallerID(long nAlertType)
{
    //TODO: replace these constants with the right values
    const string csPrefix = "555";
    const int cnNumberLength = 6;

    //the caller id will be like this: 555-000124
    string sCallerID(csPrefix);
    string sNum = StringUtils::ltos(nAlertType);
    sCallerID += StringUtils::RepeatChar('0', - static_cast<int>(sNum.length()) + cnNumberLength) + sNum;

    return sCallerID;
}
#endif

