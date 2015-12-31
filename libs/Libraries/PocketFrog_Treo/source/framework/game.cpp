/////////////////////////////////////////////////////////////////////////////
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

namespace Frog
{



//////////////////////////////////////////////////////////////////////////////
//
// Game
//
//////////////////////////////////////////////////////////////////////////////

Game::Game()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED); 

    m_config.szWindowName = _T( "PocketFrog" );
    m_config.orientation  = ORIENTATION_NORMAL;

    ZeroMemory( &m_keys, sizeof(m_keys) );
}



Game::~Game()
{
	CoUninitialize();
}



Display* Game::GetDisplay()
{
    Display* display = m_display.get();
    
    if (display && display->GetBackBuffer())
        return display;
    else
        return 0;
}



void Game::FatalError( LPCTSTR msg )
{
	m_display.reset();

    if (m_hWnd) DestroyWindow();

    ::MessageBox( 0, msg, _T("Fatal Error"), MB_OK);
    
	exit(-1);
}



bool Game::Init()
{
	
#if defined(FROG_HPC)
    // Handheld PC: force orientation to NORTH
    m_config.orientation = ORIENTATION_NORMAL;
#endif

    m_bSuspended = false;

	//GetWndClassInfo().m_wc.style &= ~CS_DBLCLKS;
	GetWndClassInfo().m_wc.style = CS_HREDRAW | CS_VREDRAW;
	GetWndClassInfo().m_wc.hbrBackground = 0;

    // Create the window
#if defined(_WIN32_WCE)
    Rect r( 0, 0, ::GetSystemMetrics( SM_CXSCREEN ), ::GetSystemMetrics( SM_CYSCREEN ) );
    DWORD style = WS_VISIBLE;
#else
    Rect r( CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 );
    DWORD style = WS_SYSMENU;
#endif

#if defined(FROG_HPC)
    DWORD exStyle = WS_EX_TOPMOST;
#else
    DWORD exStyle = 0;
#endif

    if (!Create( 0, r, m_config.szWindowName, style, exStyle ))
	{
		FatalError( _T("Window creation failed") );
        return false;
	}

#if defined(FROG_PC)
    // Make sure the window is of the proper size
    // Changed 06-10-2003 by Frank W. Zammetti
    int width, height;
    width = 240;
    height = 320;

    if (m_config.orientation == ORIENTATION_ROTATE90CW || m_config.orientation == ORIENTATION_ROTATE90CCW)
        swap( width, height );
    
    ResizeClient( width, height );
#endif

    // Init display
    PHAL::DisplayParameters params;
    params.m_orientation = m_config.orientation;
    m_display.reset( Display::Create( m_hWnd, params ) );
    if (!m_display.get())
        return false;


#if defined(FROG_PC)
    ShowWindow( SW_SHOWNORMAL );
#endif


    // Init input
    if (!PHAL::GetKeyList( &m_keys, m_config.orientation ))
        return false;

    // In case ORIENTATION_UNKNOWN was selected
    m_config.orientation = m_display->GetOrientation();
    
    return true;
}



bool Game::Run()
{
    // Initialization
    if (!Init())
	{
		FatalError( _T("Game::Init() failed") );
        return false;
	}


    // Game will start...
    if (!GameInit())
    {
        m_display.reset();
        DestroyWindow();
        return false;
    }

    
    // Main loop
    for(;;)
    {
        MSG msg;

        if (::PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
        {
            if (msg.message == WM_QUIT)
                break;
            
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }
        else
        {
            if (!m_bSuspended)
                GameLoop();
            else			//Added for PF_0_8_1_bugfix_2.zip
                Sleep(500);	//Added for PF_0_8_1_bugfix_2.zip
        }
    }

    return true;
}



void Game::ShowTaskbar( bool bShow )
{
#if defined(FROG_PPC)

 /*   if (bShow)
        ::SHFullScreen( m_hWnd, SHFS_SHOWTASKBAR | SHFS_SHOWSIPBUTTON | SHFS_SHOWSTARTICON );
    else
        ::SHFullScreen( m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );
*/
#elif defined(FROG_HPC)

    // Nothing to do...

#endif
}



void Game::Shutdown()
{
    GameEnd();

    m_display.reset();

    DestroyWindow();
}




void Game::DeviceToLogical( int& button ) const
{
#if defined(_WIN32_WCE)

    switch (m_config.orientation)
    {
    case ORIENTATION_ROTATE90CCW:
        if (button == m_keys.vkUp) button = m_keys.vkRight;
        else if (button == m_keys.vkLeft) button = m_keys.vkUp;
        else if (button == m_keys.vkDown) button = m_keys.vkLeft;
        else if (button == m_keys.vkRight) button = m_keys.vkDown;
        break;
        
    case ORIENTATION_ROTATE90CW:
        if (button == m_keys.vkUp) button = m_keys.vkLeft;
        else if (button == m_keys.vkLeft) button = m_keys.vkDown;
        else if (button == m_keys.vkDown) button = m_keys.vkRight;
        else if (button == m_keys.vkRight) button = m_keys.vkUp;
        break;
        
    case ORIENTATION_ROTATE180:
        if (button == m_keys.vkUp) button = m_keys.vkDown;
        else if (button == m_keys.vkLeft) button = m_keys.vkRight;
        else if (button == m_keys.vkDown) button = m_keys.vkUp;
        else if (button == m_keys.vkRight) button = m_keys.vkLeft;
        break;
    }

#endif
}



//fixme: don't use hard coded values for width and height
void Game::DeviceToLogical( Point& P ) const
{
    switch (m_config.orientation)
    {
    case ORIENTATION_ROTATE90CCW:
        swap( P.x, P.y );
        P.x = 319 - P.x;
        break;
        
    case ORIENTATION_ROTATE90CW:
        swap( P.x, P.y );
        P.y = 239 - P.y;
        break;
        
    case ORIENTATION_ROTATE180:
        P.x = 239 - P.x;
        P.y = 319 - P.y;
        break;
    }
}



//////////////////////////////////////////////////////////////////////////////
//
// Default implementation of callbacks
//
//////////////////////////////////////////////////////////////////////////////

bool Game::GameInit()
{
    return true;
}


// Added 6/10/2003 by Frank W. Zammetti
void Game::GameSuspend()
{
}


// Added 6/10/2003 by Frank W. Zammetti
void Game::GameResume()
{
}


void Game::GameEnd()
{
}



void Game::GameLoop()
{
}



void Game::ButtonDown( int button )
{
    Shutdown();
}



void Game::ButtonUp( int button )
{
}



void Game::StylusDown( Point p )
{
	Shutdown();
}



void Game::StylusMove( Point p )
{
}



void Game::StylusUp( Point p )
{
}



//////////////////////////////////////////////////////////////////////////////
//
// Message Handlers
//
//////////////////////////////////////////////////////////////////////////////

LRESULT Game::OnClose( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
    Shutdown();
    return 0;
}



LRESULT Game::OnCreate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{

    return 0;
}



LRESULT Game::OnDestroy( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
    PostQuitMessage(0);
    return 0;
}



LRESULT Game::OnKeyDown( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
    if (!(lparam & 0x40000000)) // Prevent auto-repeat
    {
        int button = wparam;
        DeviceToLogical( button );
        ButtonDown( button );
    }
    
#if defined(FROG_PC)
    if (wparam == VK_ESCAPE)
        PostMessage( WM_CLOSE, 0, 0 );
#endif

    return 0;
}



LRESULT Game::OnKeyUp( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
    int button = wparam;
    DeviceToLogical( button );
    ButtonUp( button );

    return 0;
}



LRESULT Game::OnMouseEvent( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
	Point P( LOWORD(lparam), HIWORD(lparam) );

#if defined(FROG_PPC)
    DeviceToLogical( P );
#endif

    switch(msg)
    {
    case WM_LBUTTONDOWN: StylusDown( P ); break;
    case WM_MOUSEMOVE:   StylusMove( P ); break;
    case WM_LBUTTONUP:   StylusUp( P );   break;
    }
    
    return 0;
}



LRESULT Game::OnActivate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
    if (LOWORD(wparam) == WA_INACTIVE)
    {
        if (!m_bSuspended)
        {
            m_bSuspended = true;
            /*Game::*/GameSuspend(); // Added 6/10/2003 by Frank W. Zammetti
        }
    }
    else
    {
        if (m_bSuspended)
        {
            m_bSuspended = false;
			/*Game::*/GameResume(); // Added 6/10/2003 by Frank W. Zammetti
        }
    }
    

    ShowTaskbar( LOWORD(wparam) == WA_INACTIVE );

    return 0;
}



LRESULT Game::OnPaint( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled )
{
    PAINTSTRUCT ps;
    
    BeginPaint( &ps );
    EndPaint( &ps );

	return 0;
}



} // end of namespace Frog
