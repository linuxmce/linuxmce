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

#ifndef POCKETFROG_DEFS_H
#define POCKETFROG_DEFS_H

#include "config.h"
#undef NOMINMAX
#define NOMINMAX
#include <windows.h>
#include <tchar.h>

#if !defined(_WIN32_WCE) && defined(WIN32_LEAN_AND_MEAN)
#include <mmsystem.h>
#endif


#include <atlbase.h>

#if !defined(_WIN32_WCE)
#define FROG_PC
#elif defined(_WIN32_WCE_PSPC)
#define FROG_PPC
#else
#define FROG_HPC
#endif



//////////////////////////////////////////////////////////////////////////////
//
// <stdint,h>
//
//////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)

typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

#else

#error Must define basic integer types

#endif



//////////////////////////////////////////////////////////////////////////////
//
// Commonly used headers
//
//////////////////////////////////////////////////////////////////////////////

#include "rect.h"
#include "foreign/boost/scoped_ptr.hpp"
#include "foreign/boost/scoped_array.hpp"
#include "foreign/boost/utility.hpp"



namespace Frog {

//////////////////////////////////////////////////////////////////////////////
//
// Orientation
//
//////////////////////////////////////////////////////////////////////////////

enum Orientation
{
    ORIENTATION_UNKNOWN = -1,   // Unknown / default orientation
    ORIENTATION_NORTH,          // ex: Casio E-125
    ORIENTATION_WEST,           // ex: iPAQ 36xx
    ORIENTATION_SOUTH,          // ex: ???
    ORIENTATION_EAST            // ex: IPAQ 38xx
};



//////////////////////////////////////////////////////////////////////////////
//
// Pixel - PocketFrog always work with a 16 bit pixel (565 color format)
//
//////////////////////////////////////////////////////////////////////////////

typedef uint16_t Pixel;



//////////////////////////////////////////////////////////////////////////////
//
// Color masks, shifts and bits
//
//////////////////////////////////////////////////////////////////////////////

const Pixel RED_MASK    = 0x1F << 11;
const Pixel GREEN_MASK  = 0x3F << 5;
const Pixel BLUE_MASK   = 0x1F;
const int   RED_SHIFT   = 11;
const int   GREEN_SHIFT = 5;
const int   BLUE_SHIFT  = 0;
const int   RED_BITS    = 5;
const int   GREEN_BITS  = 6;
const int   BLUE_BITS   = 5;



//////////////////////////////////////////////////////////////////////////////
//
// DrawVGAText alignment constants
// Added 5/9/2003 by Frank W. Zammetti
//
//////////////////////////////////////////////////////////////////////////////
const int DVT_NONE	 = 0;
const int DVT_LEFT	 = 1;
const int DVT_CENTER = 2;
const int DVT_RIGHT  = 3;



//////////////////////////////////////////////////////////////////////////////
//
// Color() - Helpers to create Pixels
//
//////////////////////////////////////////////////////////////////////////////

// Return a "Pixel" from the specified color components.
// Each component value must be between 0 and 255
inline Pixel Color( int red, int green, int blue )
{
   return ((red << 8) & RED_MASK) | ((green << 3) & GREEN_MASK) | (blue >> 3);
}


// Return a "Pixel" from the specified COLORREF structure
inline Pixel Color( const COLORREF& color )
{
    return Color( GetRValue(color), GetGValue(color), GetBValue(color) );
}




} // end of namespace Frog



#endif
