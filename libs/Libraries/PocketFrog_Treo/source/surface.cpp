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

#include "surface.h"
#include "display.h"
#include <PocketHAL/Win32/GDISurface.h>



namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// Surface
//
//////////////////////////////////////////////////////////////////////////////

Surface::Surface( unsigned width, unsigned height )
:   m_buffer( PHAL::Surface::Create( width, height ) ),
    m_colorMask( -1 ),
    m_original( 0 ),
    m_hdc( 0 )
{
}



Surface::Surface( PHAL::Surface* buffer )
:   m_buffer( buffer ),
    m_colorMask( -1 ),
    m_original( 0 ),
    m_hdc( 0 )
{
}



Surface::~Surface()
{
    if (m_original)
        ReleaseDC( m_hdc );

    delete m_buffer;
}



Surface* Surface::Clone() const
{
    Surface* copy = new Surface( GetWidth(), GetHeight() );

    if (copy)
    {
        Rasterizer rasterizer( copy );
        rasterizer.Blit( 0, 0, this );
    }

    return copy;
}



bool Surface::Lock( LockInfo& lockinfo, bool bDiscard, const Rect* rect )
{
    // Fill lockinfo and return success
    int x = 0, y = 0;
    if (rect) x = rect->left, y = rect->top;
    
    lockinfo.pitch  = m_buffer->GetPitch();
    lockinfo.pixels = m_buffer->GetPixels( x, y );

    return true;
}




bool Surface::Unlock( bool bDiscard )
{
    return true;
}



HDC Surface::GetDC( bool bDiscard )
{
    if (m_hdc)
    {
        // Refuse nested calls
        return 0;
    }


    PHAL::GDISurface* gdibuffer = PHAL::GDISurface::Create( GetWidth(), GetHeight() );
    if (gdibuffer)
    {
        if (!bDiscard)
        {
            Surface gdisurface( gdibuffer );
            Rasterizer rasterizer( &gdisurface );
            rasterizer.Blit( 0, 0, this );
            gdisurface.m_buffer = 0;    // Prevent the destruction of the actual buffer
        }

        m_original = m_buffer;
        m_buffer   = gdibuffer;
        m_hdc      = gdibuffer->GetDC();
    }

    return m_hdc;
}



bool Surface::ReleaseDC( HDC hdc )
{
    if (hdc==0 || m_hdc != hdc)
        return false;

    Surface original( m_original );
    Rasterizer rasterizer( &original );
    rasterizer.Blit( 0, 0, this );
    original.m_buffer = 0;  // Prevent destruction
    
    delete m_buffer;
    m_buffer   = m_original;
    m_original = 0;
    m_hdc      = 0;

    return true;
}



void Surface::SetBuffer( PHAL::Surface* buffer )
{
    if (m_hdc)
        m_original = buffer;
    else
        m_buffer = buffer;
}



} // end of namespace Frog
