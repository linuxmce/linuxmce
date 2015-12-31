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

#include "Win32Drivers.h"

//globals
long g_lWindowWidth = 0;
long g_lWindowHeight = 0;

namespace Frog {
namespace Internal {


    
//////////////////////////////////////////////////////////////////////////////
//
// Win32Display
//
//////////////////////////////////////////////////////////////////////////////

Win32Display::Win32Display(unsigned int desktopZoom)
:   m_hwnd(0)
{
		m_desktopZoom = desktopZoom;
}



Win32Display::~Win32Display()
{
    m_buffer.reset(0);
}



bool Win32Display::Initialize( HWND hwnd )
{
    if (m_buffer.get() || !IsWindow(hwnd))
        return false;

    m_hwnd = hwnd;

    // Construct rendering buffer
	Rect r;
	r.Set(0, 0, g_lWindowWidth, g_lWindowHeight);
    //GetClientRect( hwnd, &r );

    m_buffer.reset( new GDIBuffer( r.GetWidth(), r.GetHeight() ) );

    return true;
}



void Win32Display::Shutdown()
{
    m_buffer.reset(0);
    m_hwnd = 0;
}



void Win32Display::Suspend()
{
    // Nothing to do
}



void Win32Display::Resume()
{
    // Nothing to do
}



Pixel* Win32Display::BeginScene()
{
    if (!m_buffer.get())
        return 0;

    return m_buffer->GetPixels();
}



void Win32Display::EndScene( const Rect& rect )
{
    if (!m_buffer.get() || !IsWindow(m_hwnd))
        return;
        
    HDC hDestDC = ::GetDC( m_hwnd );
    HDC hSrcDC  = m_buffer->GetDC();

    if (m_desktopZoom > 1) {
			::StretchBlt( hDestDC, 0, 0, GetWidth() * m_desktopZoom, GetHeight() * m_desktopZoom,
                    hSrcDC,  0, 0, GetWidth(),                 GetHeight(), SRCCOPY );
    } else {
//      ::BitBlt( hDestDC, 0, 0, GetWidth(), GetHeight(),
//                hSrcDC,  0, 0, SRCCOPY );
		::BitBlt( hDestDC, rect.left, rect.top, rect.GetWidth(), rect.GetHeight(),
			hSrcDC, rect.left, rect.top, SRCCOPY);
		}

    m_buffer->ReleaseDC( hSrcDC );
    ::ReleaseDC( m_hwnd, hDestDC );
}



//////////////////////////////////////////////////////////////////////////////
//
// Win32Input
//
//////////////////////////////////////////////////////////////////////////////

Win32Input::Win32Input()
{
    memset( &m_keyList, 0, sizeof(m_keyList) );

    m_keyList.vkUp    = VK_UP;
    m_keyList.vkDown  = VK_DOWN;
    m_keyList.vkLeft  = VK_LEFT;
    m_keyList.vkRight = VK_RIGHT;
    m_keyList.vkA     = '1';
    m_keyList.vkB     = '2';
    m_keyList.vkC     = '3';
    m_keyList.vkStart = '\r';
}



} // end of namespace Internal
} // end of namespace Frog
