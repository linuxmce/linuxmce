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

#ifndef POCKETFROG_POCKETPC_H
#define POCKETFROG_POCKETPC_H

#include "defs.h"


namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// PocketPC
//
//////////////////////////////////////////////////////////////////////////////

namespace PocketPC
{
    // Retrieve the full path and file name for the specified file
    // The specified buffer "pathname" must be at least MAX_PATH in length.
    // The value returned is the length of the string copied to "pathname",
    // not including the terminating NULL character.
    size_t GetFullPathName( LPCTSTR filename, TCHAR pathname[MAX_PATH] );

    // Return the current tick count (in ms)
    inline uint32_t GetTickCount() { return PHAL::GetClock(); }
}



} // end of namespace Frog



#endif
