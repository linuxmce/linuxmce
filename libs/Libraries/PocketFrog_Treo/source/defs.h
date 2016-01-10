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

#include <windows.h>
#include <tchar.h>
#include <atlbase.h>

#if !defined(_WIN32_WCE)
#define FROG_PC
#elif defined(_WIN32_WCE_PSPC)
#define FROG_PPC
#else
//#define FROG_HPC
#endif



//////////////////////////////////////////////////////////////////////////////
//
// Commonly used headers
//
//////////////////////////////////////////////////////////////////////////////

#include "rect.h"
#include <PocketHAL/PocketHAL.h>
#include "foreign/boost/scoped_ptr.hpp"
#include "foreign/boost/scoped_array.hpp"
#include "foreign/boost/utility.hpp"




namespace Frog {

//////////////////////////////////////////////////////////////////////////////
//
// Using declarations
//
//////////////////////////////////////////////////////////////////////////////

using PHAL::int8_t;
using PHAL::int16_t;
using PHAL::int32_t;
using PHAL::int64_t;
using PHAL::uint8_t;
using PHAL::uint16_t;
using PHAL::uint32_t;
using PHAL::uint64_t;
using PHAL::swap;
using PHAL::Orientation;
using PHAL::ORIENTATION_NORMAL;
using PHAL::ORIENTATION_ROTATE90CCW;
using PHAL::ORIENTATION_ROTATE180;
using PHAL::ORIENTATION_ROTATE90CW;
using PHAL::ORIENTATION_UNKNOWN;



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
