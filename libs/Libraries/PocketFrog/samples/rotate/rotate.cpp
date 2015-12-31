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

#include "rotate.h"
#include "resource.h"

using namespace Frog;



//////////////////////////////////////////////////////////////////////////////
//
// _Module : This is needed to use ATL Windows
//
//////////////////////////////////////////////////////////////////////////////

CComModule _Module;



//////////////////////////////////////////////////////////////////////////////
//
// RotateSample
//
//////////////////////////////////////////////////////////////////////////////

RotateSample::RotateSample()
{
    m_config.szWindowName     = _T("Rotate Sample");
    m_config.orientation      = ORIENTATION_NORTH;
    m_config.splashScreenTime = 1000;
}



bool RotateSample::GameInit()
{
    // Initialize random seed
    srand( PocketPC::GetTickCount() );
    
    DisplayDevice* display = GetDisplay();
    
    m_image   = LoadImage( display, IDB_BITMAP1 );
    m_numbers = LoadImage( display, IDB_NUMBERS );

    m_FPSCounter = 0;
    m_angle      = 0.0f;

    return true;
}



void RotateSample::GameEnd()
{
    delete m_image;
    delete m_numbers;
}



void RotateSample::GameLoop()
{
    const float PI = 3.1415926535897932384626433832795f;

    DisplayDevice* display = GetDisplay();

    display->Clear( Color(255,255,255) );

    display->BlitRotated( display->GetWidth() - m_image->GetWidth()/2, m_image->GetHeight()/2, m_angle, m_image );
    display->BlitRotated( display->GetWidth()/2, display->GetHeight()/2, m_angle, m_image );
    display->BlitRotated( m_image->GetWidth()/2, display->GetHeight() - m_image->GetHeight()/2, m_angle, m_image );


    // Update angle
    m_angle += 0.03f;
    if (m_angle > 2.0f * PI)
        m_angle -= 2.0f * PI;


    // Update FPS
    m_FPSTicks[ m_FPSCounter & 15 ] = PocketPC::GetTickCount();
    if (m_FPSCounter > 15)
    {
        uint32_t totalTime = m_FPSTicks[ m_FPSCounter & 15 ] - m_FPSTicks[ (m_FPSCounter+1) & 15 ];
        if (totalTime == 0) totalTime = 1;
        uint32_t fps  = 16000 / totalTime;
        uint32_t n100 = fps / 100; fps -= n100 * 100;
        uint32_t n10  = fps / 10;  fps -= n10 * 10;
        uint32_t n1   = fps;

        if (n100 > 9) { n100 = 9; n10 = 9; n1 = 9; }
        
        // Display fps
        display->Blit(  0, 0, m_numbers, &Rect( n100 * 8, 0, (n100+1)*8, 11 ) );
        display->Blit(  8, 0, m_numbers, &Rect( n10 * 8, 0, (n10+1)*8, 11 ) );
        display->Blit( 16, 0, m_numbers, &Rect( n1 * 8, 0, (n1+1)*8, 11 ) );
    }
    ++m_FPSCounter;

    
    // Update screen
    display->Update();
}



void RotateSample::StylusDown( Point stylus )
{
}



void RotateSample::StylusUp( Point stylus )
{
}



void RotateSample::StylusMove( Point stylus )
{
}




//////////////////////////////////////////////////////////////////////////////
//
// WinMain - Entry point
//
//////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
    _Module.Init( 0, hInstance );
    
    RotateSample game;
    
    game.Run();
    
    return 0;
}
