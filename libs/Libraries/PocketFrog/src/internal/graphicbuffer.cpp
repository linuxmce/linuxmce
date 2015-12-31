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

#include "graphicbuffer.h"
#include <cassert>

namespace Frog {
namespace Internal {


    
//////////////////////////////////////////////////////////////////////////////
//
// MemoryGraphicBuffer
//
//////////////////////////////////////////////////////////////////////////////

// Pitch is uint32_t aligned
MemoryGraphicBuffer::MemoryGraphicBuffer( int width, int height )
:   GraphicBuffer( new Pixel[((width+1)&~1) * height], width, height, (width*2+3) & ~3 )
{
}



MemoryGraphicBuffer::~MemoryGraphicBuffer()
{
    delete [] m_pixels;
}



//////////////////////////////////////////////////////////////////////////////
//
// GAPIBuffer
//
//////////////////////////////////////////////////////////////////////////////

GAPIBuffer::GAPIBuffer( int width, int height, int pitch )
:   GraphicBuffer( 0, width, height, pitch )
{
}



//////////////////////////////////////////////////////////////////////////////
//
// GDIBuffer
//
//////////////////////////////////////////////////////////////////////////////

// Pitch is uint32_t aligned
GDIBuffer::GDIBuffer( int width, int height )
:   GraphicBuffer( 0, width, height, (width*2+3) & ~3 )
{
    // Create a DIB
    BYTE buffer[sizeof(BITMAPINFOHEADER) + 3 * sizeof(RGBQUAD)];
    
    // Handy pointers
    BITMAPINFO*       pBMI    = (BITMAPINFO*)buffer;
    BITMAPINFOHEADER* pHeader = &pBMI->bmiHeader;
    DWORD*            pColors = (DWORD*)&pBMI->bmiColors;   
    
    // DIB Header
    pHeader->biSize            = sizeof(BITMAPINFOHEADER);
    pHeader->biWidth           = width;
    pHeader->biHeight          = -height;
    pHeader->biPlanes          = 1;
    pHeader->biBitCount        = 16;
    pHeader->biCompression     = BI_BITFIELDS;
    pHeader->biSizeImage       = width * height * 2;
    pHeader->biXPelsPerMeter   = 0;
    pHeader->biYPelsPerMeter   = 0;
    pHeader->biClrUsed         = 0;
    pHeader->biClrImportant    = 0;
    
    // Color masks
    pColors[0] = RED_MASK;
    pColors[1] = GREEN_MASK;
    pColors[2] = BLUE_MASK;
    
    // Create the DIB
    m_hBitmap = ::CreateDIBSection( 0, pBMI, DIB_RGB_COLORS, (void**)&m_pixels, 0, 0 );
}



GDIBuffer::~GDIBuffer()
{
    ::DeleteObject( m_hBitmap );
}



HDC GDIBuffer::GetDC()
{
    HDC hDisplayDC = ::GetDC(0);
    HDC hMemoryDC  = ::CreateCompatibleDC( hDisplayDC );
    
    ::SelectObject( hMemoryDC, m_hBitmap );
    ::ReleaseDC( 0, hDisplayDC );
    
    return hMemoryDC;
}



void GDIBuffer::ReleaseDC( HDC hMemoryDC )
{
    ::DeleteDC( hMemoryDC );
}



//////////////////////////////////////////////////////////////////////////////
//
// Blits with rotation
//
//////////////////////////////////////////////////////////////////////////////

void Blit( const GraphicBuffer& source, GraphicBuffer& destination )
{
    assert( source.GetWidth() == destination.GetWidth() );
    assert( source.GetHeight() == destination.GetHeight() );
    
    const Pixel* pSrc  = source.GetPixels();
    Pixel*       pDest = destination.GetPixels();
    
    if (source.GetPitch() == destination.GetPitch())
    {
        memcpy( pDest, pSrc, source.GetHeight() * source.GetPitch() );
    }
    else
    {
        for (int h = source.GetHeight(); h; --h )
        {
            memcpy( pDest, pSrc, source.GetWidth() * 2 );
            
            pSrc  = (Pixel*)((char*)pSrc + source.GetPitch());
            pDest = (Pixel*)((char*)pDest + destination.GetPitch());
        }
    }
}



void BlitRotateCCW( const GraphicBuffer& source, GraphicBuffer& destination )
{
    assert( source.GetWidth() == destination.GetHeight() );
    assert( source.GetHeight() == destination.GetWidth() );
    
    const Pixel* pSrc  = source.GetPixels();
    Pixel*       pDest = destination.GetPixels( source.GetHeight() - 1, 0 );
    
    for (int h = source.GetHeight(); h; --h )
    {
        const Pixel* plSrc  = pSrc;
        Pixel*       plDest = pDest;
        
        for (int w = source.GetWidth(); w; --w )
        {
            *plDest = *plSrc;
            ++plSrc;
            
            plDest = (Pixel*)((char*)plDest + destination.GetPitch());
        }
        
        pSrc = (Pixel*)((char*)pSrc + source.GetPitch());
        --pDest;
    }
}



void BlitRotateCW( const GraphicBuffer& source, GraphicBuffer& destination )
{
    assert( source.GetWidth() == destination.GetHeight() );
    assert( source.GetHeight() == destination.GetWidth() );
    
    const Pixel* pSrc  = source.GetPixels();
    Pixel*       pDest = destination.GetPixels( 0, source.GetWidth() - 1 );
    
    for (int h = source.GetHeight(); h; --h )
    {
        const Pixel* plSrc  = pSrc;
        Pixel*       plDest = pDest;
        
        for (int w = source.GetWidth(); w; --w )
        {
            *plDest = *plSrc;
            ++plSrc;
            
            plDest = (Pixel*)((char*)plDest - destination.GetPitch());
        }
        
        pSrc = (Pixel*)((char*)pSrc + source.GetPitch());
        ++pDest;
    }
}



void BlitUpsideDown( const GraphicBuffer& source, GraphicBuffer& destination )
{
    assert( source.GetWidth() == destination.GetWidth() );
    assert( source.GetHeight() == destination.GetHeight() );
    
    const Pixel* pSrc  = source.GetPixels();
    Pixel*       pDest = destination.GetPixels( source.GetWidth() - 1, source.GetHeight() - 1 );
    
    for (int h = source.GetHeight(); h; --h )
    {
        const Pixel* plSrc  = pSrc;
        Pixel*       plDest = pDest;
        
        for (int w = source.GetWidth(); w; --w )
        {
            *plDest = *plSrc;
            ++plSrc;
            --plDest;
        }
        
        pSrc  = (Pixel*)((char*)pSrc + source.GetPitch());
        pDest = (Pixel*)((char*)pDest - destination.GetPitch());
    }
}



} // end of namespace Internal
} // end of namespace Frog
