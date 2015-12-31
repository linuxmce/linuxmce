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

#include <PocketHAL/display.h>
#include "defs.h"
#include "rasterizer.h"
#include "Surface.h"


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
// Display
//
//////////////////////////////////////////////////////////////////////////////

class Display : public Rasterizer
{
public:

    // Construction
    static Display* Create( HWND hwnd, PHAL::DisplayParameters& params );

    // Destruction
    ~Display();

    // Return the backbuffer
    Surface*    GetBackBuffer();
    
    // Return the orientation of the display relative to the device
    Orientation GetOrientation() const              { return m_params.m_orientation; }

    // Return the width and height of the display
    int GetWidth() const                            { return m_params.m_width; }
    int GetHeight() const                           { return m_params.m_height; }

    // Change the rendering target (specify NULL to restore rendering to backbuffer)
    Surface* SetRenderTarget( Surface* target );

    // Update the display with the content of the backbuffer.
    // The optional rectangle parameter specifies the region to update.
    void Update( const Rect* rect = 0 );



//////////////////////////////////////////////////////////////////////////////////
// The following is deprecated and will go away in the next version of PocketFrog 
//////////////////////////////////////////////////////////////////////////////////

    // Create a rasterizer for the specified surface
    Rasterizer* CreateRasterizer( Surface* target ) const;

    // Create a surface
    Surface* CreateSurface( int width, int height );

    // Return the frontbuffer (Warning: might not be available on some devices)
    Surface* GetFrontBuffer();


private:

    PHAL::DisplayParameters m_params;       // Display parameters
    PHAL::Display*          m_driver;       // Display driver
    Surface                 m_backbuffer;   // The backbuffer
    
    // To create a Display, use Display::Create
    Display( HWND hwnd, PHAL::DisplayParameters& params );
};



} // end of namespace Frog



#endif
