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
 * @file PlutoDefs.h
 Header file for ??? .
 */
#if !defined(__PLUTO_DEFS_H__)
#define __PLUTO_DEFS_H__

#define PLUTO_SAFE_DELETE(p) \
{ \
    if(NULL != (p)) { delete (p); (p) = NULL; } \
}

#define PLUTO_SAFE_DELETE_ARRAY(pArray) \
{ \
    if(NULL != (pArray)) { delete [] (pArray); (pArray) = NULL; } \
}

#define PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS(ppArray, nArrayLen) \
{ \
    for(int nArrayIndex = 0; nArrayIndex < nArrayLen; nArrayIndex++) \
        PLUTO_SAFE_DELETE_ARRAY(ppArray[nArrayIndex]); \
    PLUTO_SAFE_DELETE_ARRAY(ppArray); \
}

#endif
