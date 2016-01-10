//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

#ifndef POCKETHAL_SYMBIAN_DEFS_H
#define POCKETHAL_SYMBIAN_DEFS_H

#include <w32std.h>
#include <string.h>



namespace PHAL
{

    typedef RWindow& WindowHandle;

#if defined(PHAL_COMPILER_GCC)
    typedef __wchar_t TCHAR;
#else
    typedef unsigned short TCHAR;
#endif

    typedef const TCHAR* LPCTSTR;


} // end of namespace PHAL



#define TEXT(x) L##x



#endif
