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

#ifndef POCKETFROG_SURFACE_H
#define POCKETFROG_SURFACE_H

#include "defs.h"
#include <PocketHAL/surface.h>

namespace Frog {

class Rasterizer;
class Rect;



//////////////////////////////////////////////////////////////////////////////
//
// Surface
//
//////////////////////////////////////////////////////////////////////////////

class Surface : PHAL::NonCopyable
{
public:

    // Construction / destruction
    Surface( unsigned width, unsigned height );
    ~Surface();
    
    // Clone the surface
    Surface*        Clone() const;


    // Return the width, height and pitch of the surface
    unsigned        GetWidth() const                { return m_buffer->GetWidth(); }
    unsigned        GetHeight() const               { return m_buffer->GetHeight(); }

    // Set the color mask (transparent color, use negative number to disable)
    void SetColorMask( int mask )                   { m_colorMask = mask; }

    // Retrieve the current color mask (negative value if none)
    int  GetColorMask() const                       { return m_colorMask; }


    // Locking information used with Lock()
    struct LockInfo
    {
        unsigned pitch;
        Pixel*   pixels;
    };

    // Lock the surface for direct pixel manipulations.
    bool Lock( LockInfo& lockinfo, bool bDiscard = false, const Rect* rect = 0 );
    
    // Unlock the surface
    // If "bDiscard" is true, any changed to the buffer will be discarded.
    // You should set "bDiscard" to true when you are only doing read accesses.
    bool Unlock( bool bDiscard = false );


    // Retrieve a GDI device context
    // If "bDiscard" is true, the original data will be discarded.
    // You should set "bDiscard" to true when you are overwritting all of the surface.
    HDC  GetDC( bool bDiscard = false );
    
    // Release a GDI device context
    bool ReleaseDC( HDC hdc );


private:

    PHAL::Surface*  m_buffer;               // PocketHAL surface
    unsigned        m_colorMask;            // Source color key
    PHAL::Surface*  m_original;             // Backup of original surface
    HDC             m_hdc;                  // DC (or 0 if none available)

    // Temp hack to work with PHAL
    void SetBuffer( PHAL::Surface* buffer );

    // Temp until rasterizers are updated
    friend class Rasterizer;
    friend class Display;

    // Temp until PHAL is updated with proper GDI support
    explicit Surface( PHAL::Surface* buffer );
};



} // end of namespace Frog



#endif
