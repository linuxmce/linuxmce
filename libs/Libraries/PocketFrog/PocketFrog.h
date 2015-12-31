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

#include "src/defs.h"
#include "src/pocketpc.h"
#include "src/display.h"
#include "src/surface.h"
#include "src/rasterizer.h"
#include "src/image.h"
#include "src/input.h"
#include "src/framework/game.h"



//////////////////////////////////////////////////////////////////////////////
//
// Autolink with PocketFrog
//
//////////////////////////////////////////////////////////////////////////////

#if !defined(FROG_NO_FORCE_LIBS)

#if defined(_ARM_)
	#ifdef ARMV4I
	    #define LIBTARGET "ARMV4I"
	#else
		#define LIBTARGET "ARM"
	#endif
#elif defined(_MIPS_)
    #define LIBTARGET "MIPS"
#elif defined(_SH3_)
    #define LIBTARGET "SH3"
#elif defined(_WIN32_WCE) && defined(_X86_)
    #if defined(_WIN32_WCE_EMULATION)
    #define LIBTARGET "EMU_PPC2000"
    #else
    #define LIBTARGET "EMU_PPC2002"
    #endif
#elif defined(_VC80_UPGRADE)
	#define LIBTARGET "x86_vc8"
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
