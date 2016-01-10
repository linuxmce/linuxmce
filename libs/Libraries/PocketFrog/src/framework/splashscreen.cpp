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

#include "game.h"
#include "../surface.h"

#if defined(_WIN32_WCE) && !defined(ARMV4I)
namespace
{
    inline int MulDiv( int a, int b, int c)
    {
        return (a*b)/c;
    }
}
#endif



namespace Frog {



//////////////////////////////////////////////////////////////////////////////
//
// This array contains the frog image used on the splash screen.
//
//////////////////////////////////////////////////////////////////////////////

static const uint8_t image[] =
{
#include "splashimg.h"
};



//////////////////////////////////////////////////////////////////////////////
//
// SplashScreen() - Display the splash screen
//
//////////////////////////////////////////////////////////////////////////////

void Game::SplashScreen()
{
    DisplayDevice* display = GetDisplay();

    int band = 0;
    if (display->GetWidth() < display->GetHeight())
        band = 20;
    
    display->Clear(0);
    HDC hdc = display->GetBackBuffer()->GetDC( true );
    
    // Get the font Tahoma
    LOGFONT lf;
    memset( &lf, 0, sizeof(lf) );
    lf.lfHeight     = - MulDiv( 28, ::GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
    lf.lfWeight     = FW_HEAVY;
    lf.lfQuality    = ANTIALIASED_QUALITY;
    _tcscpy( lf.lfFaceName, _T("Tahoma") );
    
    HFONT hFont = ::CreateFontIndirect( &lf );
    SelectObject( hdc, hFont );
    
    // Select the colors (white on black)
    ::SetTextColor( hdc, RGB(255,255,255) );
    ::SetBkColor( hdc, RGB(0,0,0) );
    
    // Draw "PocketFrog"
    TCHAR s1[] = _T("Pocket");
    TCHAR s2[] = _T("Frog");
    
    SIZE xs1, xs2;
    ::GetTextExtentPoint( hdc, s1, 6, &xs1 );
    ::GetTextExtentPoint( hdc, s2, 4, &xs2 );
    
    int x = (display->GetWidth() - (xs1.cx + xs2.cx)) / 2;
    int y = 40 + band;
    
    ::ExtTextOut( hdc, x, y, 0, 0, s1, 6, 0 );
    ::SetTextColor( hdc, RGB(0,255,0) );
    ::ExtTextOut( hdc, x + xs1.cx, y, 0, 0, s2, 4, 0 );
    ::SetTextColor( hdc, RGB(255,255,255) );
    
    // Change font to a smaller size
    ::DeleteObject( hFont );
    lf.lfHeight = - MulDiv( 10, ::GetDeviceCaps( hdc, LOGPIXELSY ), 72 );
    lf.lfWeight = FW_BOLD;
    hFont = ::CreateFontIndirect( &lf );
    SelectObject( hdc, hFont );
    
    // Draw "Powered by"
    TCHAR s3[] = _T("Powered by");
    ::ExtTextOut( hdc, x, 20 + band, 0, 0, s3, 10, 0 );
    
    // Draw "Loading..."
    TCHAR s4[] = _T("Loading...");
    SIZE xs4;
    ::GetTextExtentPoint( hdc, s4, 10, &xs4 );
    ::ExtTextOut( hdc, (display->GetWidth() - xs4.cx)/2, (display->GetHeight() - xs4.cy)/2 - 20, 0, 0, s4, 10, 0 );
    
    ::DeleteObject( hFont );
    display->GetBackBuffer()->ReleaseDC( hdc );
    
    // Draw the splash image
    scoped_ptr<Surface> surface( LoadImage( display, image, image + sizeof(image) ) );

    if (surface.get())
    {
        int y = display->GetHeight() - surface->GetHeight() - band - 20;
        display->Blit( x, y, surface.get() );
    }
    
    // Update the display
    display->Update();
}



} // end of namespace Frog
