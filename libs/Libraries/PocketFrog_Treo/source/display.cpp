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

namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// Display
//
//////////////////////////////////////////////////////////////////////////////

Display* Display::Create( HWND hwnd, PHAL::DisplayParameters& params )
{
    Display* display = new Display( hwnd, params );
    if (!display->m_driver)
    {
        delete display;
        return 0;
    }
    else
    {
        return display;
    }
}



Display::Display( HWND hwnd, PHAL::DisplayParameters& params )
:   Rasterizer( 0 ),
    m_params( params ),
    m_backbuffer( 0 )
{
    m_driver = PHAL::Display::Create( hwnd, params );
    m_params = m_driver->GetParameters();
}



Display::~Display()
{
    delete m_driver;
}



Rasterizer* Display::CreateRasterizer( Surface* target ) const
{
    if (target)
    {
        return new Rasterizer( target );
    }
    else
    {
        return 0;
    }
}



Surface* Display::CreateSurface( int width, int height )
{
    return new Surface( width, height );
}



Surface* Display::GetBackBuffer()
{
    if (m_driver->BeginScene())
    {
        m_backbuffer.SetBuffer( m_driver->GetBackBuffer() );
    }

    // You shouldn't call GetBackBuffer() before calling Update()
    if (!m_backbuffer.m_buffer)
        return 0;

    SetRenderTarget( &m_backbuffer );

    return &m_backbuffer;
}



Surface* Display::GetFrontBuffer()
{
    return 0;
}



Surface* Display::SetRenderTarget( Surface* target )
{
    if (target==0)
        target = GetBackBuffer();

    return Rasterizer::SetRenderTarget( target );
}



void Display::Update( const Rect* r )
{
    if (r)
    {
        m_driver->SetViewport( reinterpret_cast<const PHAL::Rect*>(r) );
    }

    m_driver->Swap();

    m_backbuffer.SetBuffer( 0 );
}



} // end of namespace Frog
