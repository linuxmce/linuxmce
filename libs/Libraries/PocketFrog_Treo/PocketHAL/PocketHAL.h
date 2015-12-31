//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Main header file.
*/

#ifndef POCKETHAL_POCKETHAL_H
#define POCKETHAL_POCKETHAL_H



//////////////////////////////////////////////////////////////////////////////
//
// Include everything
//
//////////////////////////////////////////////////////////////////////////////

#include <PocketHAL/autolink.h>
#include <PocketHAL/display.h>
#include <PocketHAL/surface.h>
#include <PocketHAL/input.h>
#include <PocketHAL/fastcopy.h>

#if defined(PHAL_PLATFORM_WIN32)
#include <PocketHAL/win32/GDISurface.h>
#endif



//////////////////////////////////////////////////////////////////////////////
//
// Definitions
//
//////////////////////////////////////////////////////////////////////////////

#define POCKETHAL_VERSION_MAJOR    0                            ///< Major version number.
#define POCKETHAL_VERSION_MINOR    7                            ///< Minor version number.
#define POCKETHAL_VERSION_SUBMINOR 3                            ///< Revision number.

#define POCKETHAL_VERSION_STRING   "PocketHAL (Version 0.7.3)"  ///< String representation of the version.



//////////////////////////////////////////////////////////////////////////////
//
// API functions
//
//////////////////////////////////////////////////////////////////////////////

/// PocketHAL's namespace
namespace PHAL
{

/// Return the time in milliseconds.
/// @remark Symbian: the resolution of the OS timer is limited to 15.625 ms (64 ticks per seconds).
///         Different consecutive values returned by GetClock() will thus differ by 15 or 16 ms.
unsigned GetClock();



} // end of namespace PHAL



#endif
