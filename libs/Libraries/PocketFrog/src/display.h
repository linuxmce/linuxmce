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

#ifndef POCKETFROG_DISPLAY_H
#define POCKETFROG_DISPLAY_H

#include "defs.h"
#include "rasterizer.h"

namespace Frog {

class Surface;
class Rect;

namespace Internal
{
    class DisplayDriver;
    class GAPIBuffer;
    class GraphicBuffer;
}



//////////////////////////////////////////////////////////////////////////////
//
// DisplayDevice
//
//////////////////////////////////////////////////////////////////////////////

class DisplayDevice : public Rasterizer
{
public:

    // Dithering modes
    enum DitherMode
    {
        DITHER_NONE,      // No dithering, use closest color / intensity
        DITHER_FAST,      // Use ordered dithering
        DITHER_NICE       // Use error diffusion
    };

    // Destruction
    ~DisplayDevice();

    // Must be called before starting to render a frame
    void BeginScene();

    // Create a surface
    Surface* CreateSurface( int width, int height );

    // Return the backbuffer
    Surface* GetBackBuffer();
    
    // Return the frontbuffer (Warning: might not be available on some devices)
    Surface* GetFrontBuffer();

    // Return the orientation of the display relative to the device
    Orientation GetOrientation() const { return m_orientation; }

    // Return the orientation of the display relative to the GAPI buffer orientation.
    // This is not really usefull outside the library code.
    Orientation GetOrientationBB() const { return m_orientationBB; }

    // Return the width and height of the display
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // Create a rasterizer for the specified surface
    Rasterizer* CreateRasterizer( Surface* target ) const;

    // Set the dithering mode (monochrome and palletized devices only)
    void SetDithering( DitherMode mode );
    
    // Change the rendering target (specify NULL to restore rendering to backbuffer)
    Surface* SetRenderTarget( Surface* target );

    // Update the display with the content of the backbuffer.
    // The optional rectangle parameter specifies the region to update.
    void Update( const Rect* rect = 0 );


private:

    // Blit function definition
    typedef void (*PtrBlitFunction)( Internal::GAPIBuffer* GAPIBuffer, const Internal::GraphicBuffer* backbuffer );
    
    // Setup a 332 palette
    bool  SetupPalette();

    
    Orientation                 m_orientation;      // Display's orientation relative to device
    Orientation                 m_orientationBB;    // Backbuffer's orientation relative to GAPI
    int                         m_width, m_height;  // Display width and height
    bool                        m_bInScene;         // Between GXBeginDraw() / GXEndDraw()?
    PtrBlitFunction             m_blitFunction;     // Blit function
    Internal::GAPIBuffer*       m_GAPIBuffer;       // Buffer returned by GAPI
    scoped_ptr<Surface>         m_frontSurface;     // Frontbuffer surface (not always available)
    scoped_ptr<Surface>         m_backSurface;      // Backbuffer surface (rendering buffer)
    Internal::DisplayDriver*    m_driver;           // Display driver

    
    // You can't create a DisplayDevice directly, use PocketPC::CreateDisplay()
    friend class PocketPC;
    DisplayDevice( Internal::DisplayDriver* driver, Orientation orientation, unsigned int desktopZoom );
};



} // end of namespace Frog



#endif
