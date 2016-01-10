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

#include "blit.h"
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
// BlitSample
//
//////////////////////////////////////////////////////////////////////////////

BlitSample::BlitSample()
{

    m_config.szWindowName = _T( "Blit Sample" );
    m_config.orientation  = ORIENTATION_ROTATE90CCW;
}



bool BlitSample::GameInit()
{

		// Initialize random seed
    srand( PocketPC::GetTickCount() );
    
    Display* display = GetDisplay();
    
    m_images[0] = LoadImage( display, IDB_BITMAP1 );
    m_images[1] = LoadImage( display, IDB_BITMAP2 );
    m_images[2] = LoadImage( display, IDB_BITMAP3 );
    m_images[3] = LoadImage( display, IDB_BITMAP4 );
    m_background = LoadImage( display, IDR_IMAGE1 );
    
    for (int i = 0; i < 4; ++i)
    {
        // Set color mask
        m_images[i]->SetColorMask( Color( 255, 0, 255) );
        
        // Set initial position
        m_position[i].Set( 0, 0, m_images[i]->GetWidth(), m_images[i]->GetHeight() );
        
        int x = rand() % (display->GetWidth() - m_images[i]->GetWidth());
        int y = rand() % (display->GetHeight() - m_images[i]->GetHeight());
        
        m_position[i].Move( x, y );
    }
    
    
    m_numbers = LoadImage( display, IDB_NUMBERS );
    m_FPSCounter = 0;

    m_dragImage = -1;   // Not dragging any image

    return true;

}



void BlitSample::GameEnd()
{

    for (int i = 0; i < 4; ++i)
    {
        delete m_images[i];
    }
    
    delete m_background;
    delete m_numbers;

}



void BlitSample::GameLoop()
{
    Display* display = GetDisplay();

    display->GetBackBuffer();

    display->Blit( 0, 0, m_background );
    
#if defined(FROG_HPC)
    display->Blit( 320, 0, m_background );
#endif
    
    for (int i = 0; i < 4; ++i)
    {
        Rect& pos = m_position[i];
        
        // Display image
        display->Blit( pos.left, pos.top, m_images[i] );
        
        if (i != m_dragImage)
        {
            // Move image (horizontal)
            switch (rand() % 3)
            {
            case 0: break;  // Don't move
            case 1: if (pos.left > 0) pos.Translate( -1, 0 ); break;
            case 2: if (pos.right < display->GetWidth()) pos.Translate( 1, 0 ); break;
            }
            
            // Move image (vertical)
            switch (rand() % 3)
            {
            case 0: break;  // Don't move
            case 1: if (pos.top > 0) pos.Translate( 0, -1 ); break;
            case 2: if (pos.bottom < display->GetHeight()) pos.Translate( 0, 1 ); break;
            }
        }
    }

    
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



void BlitSample::StylusDown( Point stylus )
{


    // Check if the point is on one of the images
    for (int i = 3; i >= 0; --i)
    {
        if (m_position[i].Contains( stylus ))
        {
            m_dragImage  = i;
            m_dragOffset = stylus - Point( m_position[i].left, m_position[i].top );
            break;
        }
    }

}




void BlitSample::StylusUp( Point stylus )
{

		m_dragImage = -1;

}



void BlitSample::StylusMove( Point stylus )
{

		if (m_dragImage == -1)
        return;
    
    Point newPosition ( stylus - m_dragOffset );
    
    const Surface* image = m_images[m_dragImage];
    Rect limit( 0, 0, GetDisplay()->GetWidth() - image->GetWidth(), GetDisplay()->GetHeight() - image->GetHeight() );
    
    if (newPosition.x < limit.left) newPosition.x = limit.left;
    else if (newPosition.x > limit.right) newPosition.x = limit.right;
    
    if (newPosition.y < limit.top) newPosition.y = limit.top;
    else if (newPosition.y > limit.bottom) newPosition.y = limit.bottom;
    
    m_dragOffset = stylus - newPosition;
    
    m_position[m_dragImage].Move( newPosition.x, newPosition.y );

}




//////////////////////////////////////////////////////////////////////////////
//
// WinMain - Entry point
//
//////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
    _Module.Init( 0, hInstance );
    
    BlitSample game;
    
    game.Run();
    
    return 0;
}
