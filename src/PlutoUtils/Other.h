/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file Other.h
 Header file for the FastParser class.
 */
#ifndef OTHERH
#define OTHERH

#ifndef SYMBIAN
#include <cstring>
#include <string>
#include <vector>
using namespace ::std;

#if defined(ARMV4I)
	#include "WinCE.h"
#endif

#ifndef WIN32
#include "getch.h"	// Windows already has a getch
#endif

#else

#include "../PlutoUtils/MyString.h"

#define abs(x) Abs(x)
#define atoi(x) SymbianAtoi(x, 0)
#define itos(x) SymbianItoa(x)
#define strchr(x, y) (x)

#endif

#ifndef SYMBIAN

/** @class FastParser
For ???

Includes windows specific code that may not show up if documented on a Linux box.
 */
class FastParser
{

    /** The delimiter used by the parser.
    */
    char m_cDelimiter;

    /** The pointer to the data to be parsed.
    */
    char *m_pcData;

    /** Pointer to show the end of the data.
    */
    char *m_pcEnd;

public:
    /** Checks to see if the end of the data has been reached.
    */
    inline bool IsAtEnd() { return ( m_pcData > m_pcEnd ); };

    /** Returns current position of the pointer to the data.
    */
    inline const char *GetCurrentPtr() { return m_pcData; };

    /** Sets the member variables with initial values.
    @param pcData is data to be parsed.
    @param cDelimiter is the delimiter character.
     */
    void Setup(char *pcData, const char cDelimiter) { m_pcEnd = pcData + strlen(pcData); m_pcData = pcData; m_cDelimiter = cDelimiter; };

    /** Sets the member variables with initial values
    @param pcData is data to be parsed.
    @param cDelimiter is the delimiter character.
    @param iLength the length of the data to be considered.
     */
    void Setup(char *pcData, const char cDelimiter, int iLength) { m_pcEnd = pcData + iLength - 1; m_pcData = pcData; m_cDelimiter = cDelimiter; };

    /** Returns the next numeric parameter.
     */
    int GetNextNumParm();

    /** Returns the next parameter
     */
    const char *GetNextParm();
};

#ifndef WIN32


/** GetTickCount.
Windows only function.

Putting it here for a lack of a better place
 */
extern long GetTickCount();

#endif //#ifndef WIN32

/** Generate.

Looks like it is country specific.
*/
string GenerateCallerID(long nAlertType);

#endif //#ifndef SYMBIAN

#endif

