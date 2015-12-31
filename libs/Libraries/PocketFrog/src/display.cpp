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

#include "display.h"
#include "surface.h"
#include "pocketpc.h"
#include "internal/displaydriver.h"
#include "internal/GAPILibrary.h"
#include "internal/graphicbuffer.h"
#include "internal/raster2D.h"
#include <algorithm>
#include <memory>

namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// Known blitters
//
//////////////////////////////////////////////////////////////////////////////

namespace Internal
{
   void Blit4( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit4DitherFast( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit4DitherNice( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit8( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit8DitherFast( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit8DitherNice( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit16( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   void Blit16Fast( GAPIBuffer* pGAPIBuffer, const GraphicBuffer* pBackBuffer );
   
   void InitializeLUTs8();
}



//////////////////////////////////////////////////////////////////////////////
//
// Display
//
//////////////////////////////////////////////////////////////////////////////

DisplayDevice::DisplayDevice( Internal::DisplayDriver* driver, Orientation orientation, unsigned int desktopZoom )
:   m_orientation( orientation ),
    m_orientationBB( ORIENTATION_UNKNOWN ),
    m_bInScene( false ),
    m_blitFunction( 0 ),
    m_GAPIBuffer( 0 ),
    m_driver( driver )
{
    // If orientation is unspecified, use GAPI's
    if (m_orientation == ORIENTATION_UNKNOWN)
        m_orientation = m_driver->GetOrientation();

    // Create the graphic buffers
		if (desktopZoom > 1) {
        m_GAPIBuffer = new Internal::GAPIBuffer( m_driver->GetWidth() / desktopZoom, m_driver->GetHeight() / desktopZoom, m_driver->GetPitch() );
		} else {
        m_GAPIBuffer = new Internal::GAPIBuffer( m_driver->GetWidth(), m_driver->GetHeight(), m_driver->GetPitch() );
		}
    
    Internal::GraphicBuffer* backbuffer  = 0;
    Internal::GraphicBuffer* frontbuffer = 0;

    if (m_driver->GetPixelDepth() != 16)
    {
        backbuffer = new Internal::MemoryGraphicBuffer( m_driver->GetWidth(), m_driver->GetHeight() );
    }
    else
    if (!m_driver->IsBuffered())
    {
        backbuffer = new Internal::MemoryGraphicBuffer( m_driver->GetWidth(), m_driver->GetHeight() );
        frontbuffer = m_GAPIBuffer;
    }
    else
    {
        backbuffer = m_GAPIBuffer;

        // Create a frontbuffer when we know how
        switch(PocketPC::GetModel())
        {
        case PocketPC::MODEL_CASIO_E115:  // Can someone test this one? Should be same as E-125.
        case PocketPC::MODEL_CASIO_EM500: // Can someone test this one? Should be same as E-125.
        case PocketPC::MODEL_CASIO_E125:
            {
                Internal::GAPIBuffer* buffer = new Internal::GAPIBuffer( 240, 320, 512 );
                buffer->SetVideo( (Pixel*)0xAA200000 );
                frontbuffer = buffer;
            }
            break;

        case PocketPC::MODEL_JORNADA_720:
            {
                Internal::GAPIBuffer* buffer = new Internal::GAPIBuffer( 640, 240, 1280 );
                buffer->SetVideo( (Pixel*)0x82200000 );
                frontbuffer = buffer;
            }
            break;
        }
    }


#if defined(_WIN32_WCE)

    // Look up desired backbuffer's orientation relative to GAPI
    static const Orientation table[4][4] =
    {
        { ORIENTATION_NORTH, ORIENTATION_EAST,  ORIENTATION_SOUTH, ORIENTATION_WEST },
        { ORIENTATION_WEST,  ORIENTATION_NORTH, ORIENTATION_EAST,  ORIENTATION_SOUTH },
        { ORIENTATION_SOUTH, ORIENTATION_WEST,  ORIENTATION_NORTH, ORIENTATION_EAST },
        { ORIENTATION_EAST,  ORIENTATION_SOUTH, ORIENTATION_WEST,  ORIENTATION_NORTH }
    };

    m_orientationBB = table[m_driver->GetOrientation()][m_orientation];

#else

    m_orientationBB = ORIENTATION_NORTH;
    
    if (orientation == ORIENTATION_EAST || orientation == ORIENTATION_SOUTH)
        m_orientationBB = ORIENTATION_SOUTH;

#endif


    // Create the backbuffer surfaces
    m_backSurface.reset( new Surface( this, backbuffer ) );

    // Create the frontbuffer surface if it's available
    if (frontbuffer)
    {
        m_frontSurface.reset( new Surface( this, frontbuffer ) );
    }

    
    // Set width and height
    m_width = m_backSurface->GetWidth();
    m_height = m_backSurface->GetHeight();


    // Find which blit function to use
    switch (m_driver->GetPixelDepth())
    {
    case 8:
        SetupPalette();
        Internal::InitializeLUTs8();
        // No break...
        
    case 4:
        SetDithering( DITHER_NONE );
        break;
        
    case 16:
        if (backbuffer->GetPitch() == m_GAPIBuffer->GetPitch())
            m_blitFunction = Internal::Blit16Fast;
        else
            m_blitFunction = Internal::Blit16;
        break;
        
    default:
        exit(1); //todo: handle properly
    }


    // Initialize base class Rasterizer
    Rasterizer::Init( m_backSurface.get(), m_orientationBB );
}

    

DisplayDevice::~DisplayDevice()
{
    if (m_bInScene)
        m_driver->EndScene(Rect(0,0,0,0));

    // If the pixel depth is 16, the m_GAPIBuffer is used by
    // either the front or back buffer surface and there is
    // no need to destroy it.
    if (m_driver->GetPixelDepth() != 16)
    {
        delete m_GAPIBuffer;
    }

    m_backSurface.reset();

    m_driver->Shutdown();
}



void DisplayDevice::BeginScene()
{
    if (!m_bInScene)
    {
        Pixel* pixels = m_driver->BeginScene();

        // We set the video pointer each frame. Currently, I don't know of any
        // device where the pointer can change while running, but you never
        // know what will appear (ex: DMA transfers with multiple buffers).
        m_GAPIBuffer->SetVideo( pixels );
    
        m_bInScene = true;
    }
}



Rasterizer* DisplayDevice::CreateRasterizer( Surface* target ) const
{
    if (target)
    {
        std::auto_ptr<Rasterizer> rasterizer( new Rasterizer );
        rasterizer->Init( target, m_orientationBB );
        return rasterizer.release();
    }
    else
    {
        return 0;
    }
}



Surface* DisplayDevice::CreateSurface( int width, int height )
{
    if (m_orientationBB == ORIENTATION_WEST || m_orientationBB == ORIENTATION_EAST)
    {
        std::swap( width, height );
    }
    
    Surface* pSurface = new Surface( this, new Internal::MemoryGraphicBuffer( width, height ) );
    return pSurface;
}



Surface* DisplayDevice::GetBackBuffer()
{
    // A DMA transfer from the backbuffer to the frontbuffer might
    // be running. Call BeginScene() to ensure it is completed.
    if (!m_bInScene)
        BeginScene();

    return m_backSurface.get();
}



Surface* DisplayDevice::GetFrontBuffer()
{
    if (!m_frontSurface.get())
        return 0;

    // A DMA transfer from the backbuffer to the frontbuffer might
    // be running. Call BeginScene() to ensure it is completed.
    if (!m_bInScene)
        BeginScene();

    return m_frontSurface.get();
}



void DisplayDevice::SetDithering( DitherMode mode )
{
   switch (m_driver->GetPixelDepth())
   {
   case 4:
      switch (mode)
      {
      case DITHER_NONE: m_blitFunction = Internal::Blit4; break;
      case DITHER_FAST: m_blitFunction = Internal::Blit4DitherFast; break;
      case DITHER_NICE: m_blitFunction = Internal::Blit4DitherNice; break;
      }
      break;

   case 8:
      switch (mode)
      {
      case DITHER_NONE: m_blitFunction = Internal::Blit8; break;
      case DITHER_FAST: m_blitFunction = Internal::Blit8DitherFast; break;
      case DITHER_NICE: m_blitFunction = Internal::Blit8DitherNice; break;
      }
      break;
   }
}



Surface* DisplayDevice::SetRenderTarget( Surface* target )
{
    if (target==0)
        target = m_backSurface.get();

    return Rasterizer::SetRenderTarget( target );
}



bool DisplayDevice::SetupPalette()
{
    // Setup a 332 color palette
    BYTE buffer[ sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY) ];
    
    LOGPALETTE*   pLogical = (LOGPALETTE*)buffer;
    PALETTEENTRY* entries  = pLogical->palPalEntry;
    
    for (int i = 0; i < 256; ++i)
    {
        // Find each color's bit pattern
        int rbits = (i >> 5) & 0x07;
        int gbits = (i >> 2) & 0x07;
        int bbits = (i >> 0) & 0x03;
        
        // Find intensity by replicating the bit patterns over a byte
        entries[i].peRed   = (rbits << 5) | (rbits << 2) | (rbits >> 1);
        entries[i].peGreen = (gbits << 5) | (gbits << 2) | (gbits >> 1);
        entries[i].peBlue  = (bbits << 6) | (bbits << 4) | (bbits << 2) | bbits;
        entries[i].peFlags = 0;
    }
    
    // Create the GDI palette object
    pLogical->palVersion    = 0x0300;
    pLogical->palNumEntries = 256;
    
    HPALETTE hPalette = ::CreatePalette( pLogical );
    if (!hPalette) return false;
    
    // Realize the palette
    HDC hdc = ::GetDC(0);
    
    ::SelectPalette( hdc, hPalette, FALSE );
    ::RealizePalette( hdc );
    
    ::ReleaseDC( 0, hdc );
    ::DeleteObject( hPalette );
    
    return true;
}



void DisplayDevice::Update( const Rect* r )
{
		
    // Prepare GAPI if needed
    if (!m_bInScene)
        BeginScene();

    // Handle update rectangle if specified
    Rect rect;
    
    if (r)
    {
        rect = *r;
        Transform( rect );
    }
    else
    {
        rect.Set( 0, 0, m_driver->GetWidth(), m_driver->GetHeight() );
    }
    
    // Check if we need to blit the backbuffer to GAPI
    if (m_backSurface->m_buffer.get() != m_GAPIBuffer)
    {
        //todo: use update rect
        (*m_blitFunction)( m_GAPIBuffer, m_backSurface->m_buffer.get() );
    }

    m_driver->EndScene( rect );

    m_bInScene = false;
}



} // end of namespace Frog
