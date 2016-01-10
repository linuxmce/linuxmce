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

namespace Frog {

class DisplayDevice;
class Rasterizer;
class Rect;


namespace Internal
{
    class GraphicBuffer;
}



//////////////////////////////////////////////////////////////////////////////
//
// Surface
//
//////////////////////////////////////////////////////////////////////////////

class Surface : noncopyable
{
public:

    // Locking information used with Lock()
    struct LockInfo
    {
        int    pitch;
        Pixel* pixels;
    };


    // Destruction
    ~Surface();
    
    // Clone the surface
    Surface* Clone() const;

    // Return the width and height of the surface
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    // Lock the surface for direct pixel manipulations.
    // If "bDiscard" is true, the original data will be discarded.
    // You should set "bDiscard" to true when you are overwritting all of the surface.
    bool Lock( LockInfo& lockinfo, bool bDiscard = false, const Rect* rect = 0 );
    
    // Unlock the surface
    // If "bDiscard" is true, any changed to the buffer will be discarded.
    // You should set "bDiscard" to true when you are only doing read accesses.
    bool Unlock( bool bDiscard = false );

    // Retrieve a GDI device context
    // If "bDiscard" is true, the original data will be discarded.
    // You should set "bDiscard" to true when you are overwritting all of the surface.
    HDC GetDC( bool bDiscard = false );
    
    // Release a GDI device context
    bool ReleaseDC( HDC hdc );

    // Set the color mask (transparent color, use negative number to disable)
    void SetColorMask( int mask ) { m_colorMask = mask; }

    // Retrieve the current color mask (negative value if none)
    int  GetColorMask() const { return m_colorMask; }

    DisplayDevice*  m_display;              // Display
    int             m_width, m_height;      // Dimensions
    unsigned        m_lockCount;            // Lock count
    int             m_colorMask;            // Source color key
    
    scoped_ptr<Internal::GraphicBuffer> m_buffer;       // Main graphic buffer
    scoped_ptr<Internal::GraphicBuffer> m_scratchpad;   // Temporary buffer for direct and GDI access

    // Blits used for direct access and GDI support when orientation is different then NORTH
    void  Swizzle();
    void  Unswizzle();
    
    // You can't create a surface directly, use DisplayDevice::CreateSurface()
    friend class Rasterizer;
    friend class DisplayDevice;
    Surface( DisplayDevice* display, Internal::GraphicBuffer* buffer );
};



} // end of namespace Frog



#endif
