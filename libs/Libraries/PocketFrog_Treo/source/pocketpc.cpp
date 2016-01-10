//////////////////////////////////////////////////////////////////////////////
//
// PocketFrog - The Game Library for Pocket PC Devices
// Copyright 2002  Thierry Tremblay
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Thierry Tremblay makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//
//////////////////////////////////////////////////////////////////////////////

#include "pocketpc.h"
#include <tchar.h>


namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// PocketPC
//
//////////////////////////////////////////////////////////////////////////////

size_t PocketPC::GetFullPathName( LPCTSTR filename, TCHAR pathname[MAX_PATH] )
{
    if (filename[0] == '\\')
    {
        // Absolute path was specified
        _tcscpy( pathname, filename );
        return _tcslen( pathname );
    }

#if defined(_WIN32_WCE)

    ::GetModuleFileName( 0, pathname, MAX_PATH );
  
    TCHAR* p = _tcsrchr( pathname, '\\' );
    if (p == 0) return 0;

    _tcscpy( p + 1, filename );

	return _tcslen( pathname );

#else

    TCHAR* p;
    return ::GetFullPathName( filename, MAX_PATH, pathname, &p );

#endif
}



} // end of namespace Frog
