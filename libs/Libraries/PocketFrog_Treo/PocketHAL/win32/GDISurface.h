//////////////////////////////////////////////////////////////////////////////
//
// PocketHAL
// Copyright 2004 by Thierry Tremblay
//
//////////////////////////////////////////////////////////////////////////////

/** @file
    Windows only - Surface that wraps a GDI Device Independant Bitmap (DIB).
*/

#ifndef POCKETHAL_GDISURFACE_H
#define POCKETHAL_GDISURFACE_H

#include <PocketHAL/surface.h>
#include <windows.h>



namespace PHAL
{


//////////////////////////////////////////////////////////////////////////////
//
/// A GDISurface is a surface that wraps a Device Independant Bitmap (DIB).
/// You can retrieve a DC to the surface and draw on it using the GDI functions.
/// This class is only available on Windows platforms.
//
//////////////////////////////////////////////////////////////////////////////

class GDISurface : public Surface
{
public:

    /// Static constructor: create a GDI surface.
    /// Returns NULL if there is not enough memory.
    static GDISurface* Create( unsigned width, unsigned height );

    /// Destructor
    ~GDISurface();

    /// Retrieve the internal DC. Do NOT release/destroy it. The DC's lifetime is tied to the GDISurface object.
    HDC GetDC() const { return m_hDC; }


private:

    GDISurface( unsigned width, unsigned height );

    HBITMAP  m_hBitmap;
    HDC      m_hDC;
};



} // end of namespace PHAL



#endif
