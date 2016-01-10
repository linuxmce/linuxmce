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

#ifndef POCKETFROG_POCKETFROG_H
#define POCKETFROG_POCKETFROG_H

#include "source/defs.h"
#include "source/pocketpc.h"
#include "source/display.h"
#include "source/surface.h"
#include "source/rasterizer.h"
#include "source/image.h"
#include "source/framework/game.h"



//////////////////////////////////////////////////////////////////////////////
//
// Autolink with PocketFrog
//
//////////////////////////////////////////////////////////////////////////////

#if !defined(FROG_NO_FORCE_LIBS)

#if defined(_ARM_)
    #define LIBTARGET "ARM"
#elif defined(_MIPS_)
    #define LIBTARGET "MIPS"
#elif defined(_SH3_)
    #define LIBTARGET "SH3"
#elif defined(_WIN32_WCE) && defined(_X86_)
    #define LIBTARGET "EMU"
#elif defined(_M_IX86)
    #define LIBTARGET "x86"
#else
    #error Unknown platform/compiler.
#endif


#if defined(NDEBUG)
    #define LIBSUFFIX LIBTARGET
#else
    #define LIBSUFFIX LIBTARGET "_Debug"
#endif

#pragma comment( lib, "PocketFrog_" LIBSUFFIX ".lib" )

#endif



//////////////////////////////////////////////////////////////////////////////
//
// Link needed libraries
//
//////////////////////////////////////////////////////////////////////////////

#if !defined(_WIN32_WCE)

#pragma comment( lib, "gdi32.lib" )
#pragma comment( lib, "winmm.lib" )

#endif


#endif
