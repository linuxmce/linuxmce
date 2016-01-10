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
#include "PocketFrog/PNGWrapper.h"

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
}



bool BlitSample::GameInit()
{

		// Initialize random seed
    srand( PocketPC::GetTickCount() );
    
    DisplayDevice* display = GetDisplay();
    
    m_images[0] = LoadImage( display, IDB_BITMAP1 );
    m_images[1] = LoadImage( display, IDB_BITMAP2 );
    m_images[2] = LoadImage( display, IDB_BITMAP3 );
    m_images[3] = LoadImage( display, IDB_BITMAP4 );

	m_background = LoadPNG(display, "/bubu.png");
	display->Blit( rand() % 300, rand() % 300, m_background );

    //m_background = LoadImage( display, "/bubu.bmp" );

   
    m_numbers = LoadImage( display, IDB_NUMBERS );
    m_FPSCounter = 0;

    m_dragImage = -1;   // Not dragging any image

#ifdef WINCESS
	DEVMODE dmSettings;                          // Device Mode variable - Needed to change modes
	memset(&dmSettings,0,sizeof(dmSettings));    // Makes Sure Memory's Cleared

	// Get current settings -- This function fills our the settings
	// This makes sure NT and Win98 machines change correctly
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		// Display error message if we couldn't get display settings
		return false;
	}

	dmSettings.dmPelsWidth = 800;                        // Set the desired Screen Width
	dmSettings.dmPelsHeight = 600;                      // Set the desired Screen Height
	dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;    // Set the flags saying we're changing the Screen Width and Height

	// This function actually changes the screen to full screen
	// CDS_FULLSCREEN Gets Rid Of Start Bar.
	// We always want to get a result from this function to check if we failed
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN); 
	// Check if we didn't recieved a good return message From the function
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		PostQuitMessage(0);
		return false;
	}

	ModifyStyle(WS_TILEDWINDOW , 0);
	SetWindowPos(HWND_TOPMOST, 0, 0, 800, 600, SWP_SHOWWINDOW);
	SetForegroundWindow(m_hWnd);
#endif

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

#include <pocketfrog.h>
#include "../../src/internal/GraphicBuffer.h" 

void BlitSample::GameLoop()
{

    DisplayDevice* display = GetDisplay();

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
