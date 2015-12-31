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
#include "internal/graphicbuffer.h"
#include <algorithm>

namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// Surface
//
//////////////////////////////////////////////////////////////////////////////

Surface::Surface( DisplayDevice* display, Internal::GraphicBuffer* buffer )
:   m_display( display ),
    m_width( buffer->GetWidth() ),
    m_height( buffer->GetHeight() ),
    m_lockCount( 0 ),
    m_colorMask( -1 ),
    m_buffer( buffer )
{
    // Adjust width/height based on orientation
    if (m_display->GetOrientationBB() == ORIENTATION_WEST || m_display->GetOrientationBB() == ORIENTATION_EAST)
    {
        std::swap( m_width, m_height );
    }

	m_buffer->m_height = buffer->m_height;
}



Surface::~Surface()
{
}



Surface* Surface::Clone() const
{
    // If surface is locked, we don't allow cloning
    if (m_scratchpad.get())
    {
        return 0;
    }
    else
    {
        Surface* surface = m_display->CreateSurface( m_width, m_height );
        Internal::Blit( *m_buffer, *surface->m_buffer );
        return surface;
    }
}



void Surface::Swizzle()
{
    switch (m_display->GetOrientationBB())
    {
    case ORIENTATION_NORTH: Internal::Blit( *m_scratchpad, *m_buffer); break;
    case ORIENTATION_WEST:  Internal::BlitRotateCCW( *m_scratchpad, *m_buffer ); break;
    case ORIENTATION_SOUTH: Internal::BlitUpsideDown( *m_scratchpad, *m_buffer ); break;
    case ORIENTATION_EAST:  Internal::BlitRotateCW( *m_scratchpad, *m_buffer ); break;
    }
}




void Surface::Unswizzle()
{
    switch (m_display->GetOrientationBB())
    {
    case ORIENTATION_NORTH: Internal::Blit( *m_buffer, *m_scratchpad ); break;
    case ORIENTATION_WEST:  Internal::BlitRotateCW( *m_buffer, *m_scratchpad ); break;
    case ORIENTATION_SOUTH: Internal::BlitUpsideDown( *m_buffer, *m_scratchpad ); break;
    case ORIENTATION_EAST:  Internal::BlitRotateCCW( *m_buffer, *m_scratchpad ); break;
    }
}




bool Surface::Lock( LockInfo& lockinfo, bool bDiscard, const Rect* rect )
{
    // For now, GDI and direct accesses are exclusives...
    if (m_scratchpad.get() && m_lockCount==0)
    {
        lockinfo.pitch  = 0;
        lockinfo.pixels = 0;
        return false;
    }

    
    Internal::GraphicBuffer* buffer = m_buffer.get();

    // Create the scratchpad if needed
    if (m_lockCount == 0 && m_display->GetOrientationBB() != ORIENTATION_NORTH)
    {
        m_scratchpad.reset( new Internal::MemoryGraphicBuffer( m_width, m_height ) );
        buffer = m_scratchpad.get();
        
        if (!bDiscard)
            Unswizzle();
    }

    // Fill lockinfo and return success
    int x = 0, y = 0;
    if (rect) x = rect->left, y = rect->top;
    
    lockinfo.pitch  = buffer->GetPitch();
    lockinfo.pixels = buffer->GetPixels( x, y );

    ++m_lockCount;

    return true;
}




bool Surface::Unlock( bool bDiscard )
{
    if (m_lockCount==0)
        return false;

    if (--m_lockCount==0)
    {
        if (m_scratchpad.get())
        {
            if (!bDiscard)
                Swizzle();

            m_scratchpad.reset();
        }
    }

    return true;
}



HDC Surface::GetDC( bool bDiscard )
{
    // For now, GDI and direct accesses are exclusives...
    if (m_lockCount != 0)
        return 0;
    
    // For now, don't allow multiple calls to GetDC()
    if (m_scratchpad.get())
        return 0;
    
    m_scratchpad.reset( new Internal::GDIBuffer( m_width, m_height ) );
    
    if (!bDiscard)
        Unswizzle();
    
    return ((Internal::GDIBuffer*)(m_scratchpad.get()))->GetDC();
}



bool Surface::ReleaseDC( HDC hdc )
{
    // For now, GDI and direct accesses are exclusives...
    if (m_lockCount != 0)
        return false;

    if (m_scratchpad.get() && hdc)
    {
        ((Internal::GDIBuffer*)(m_scratchpad.get()))->ReleaseDC( hdc );
        
        Swizzle();
        
        m_scratchpad.reset();
    }

    return true;
}



} // end of namespace Frog
