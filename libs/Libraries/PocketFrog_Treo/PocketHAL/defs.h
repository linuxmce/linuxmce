//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Basic definitions for PocketHAL.
*/

#ifndef POCKETHAL_DEFS_H
#define POCKETHAL_DEFS_H

#include <PocketHAL/config.h>

#if defined(PHAL_PLATFORM_WIN32)
#include <PocketHAL/win32/win32_defs.h>

#elif defined(PHAL_PLATFORM_SYMBIAN)
#include <PocketHAL/symbian/symbian_defs.h>

#endif



namespace PHAL
{

//////////////////////////////////////////////////////////////////////////////
//
// Types
//
//////////////////////////////////////////////////////////////////////////////

/// Orientation is used to specify your game's orientation relative to the
/// normal way of holding your device. The display and input orientations do
/// not have to be the same.

enum Orientation
{
    ORIENTATION_NORMAL,         //!< Normal orientation (no rotation)
    ORIENTATION_ROTATE90CCW,    //!< Rotated 90 degrees to the left
    ORIENTATION_ROTATE180,      //!< Rotated 180 degrees (upside-down)
    ORIENTATION_ROTATE90CW,     //!< Rotated 270 degrees to the right
    ORIENTATION_UNKNOWN         //!< Unknown orientation (unknown or default one)
};



} // end of namespace PHAL





#endif
