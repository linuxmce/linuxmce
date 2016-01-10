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

#ifndef POCKETFROG_FRAMEWORK_GAME_H
#define POCKETFROG_FRAMEWORK_GAME_H

#ifdef WINCE
//other defs
#endif

#define WM_MOUSEWHEEL 0x020A

#include "../../PocketFrog.h"

#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>



namespace Frog
{

enum MouseButton
{
	mbLeft,
	mbRight,
	mbMiddle,
	mbCustom1,
	mbCustom2,
	mbCustom3
};

//////////////////////////////////////////////////////////////////////////////
//
// Game
//
//////////////////////////////////////////////////////////////////////////////

class Game : public CWindowImpl<Game>
{
	BEGIN_MSG_MAP( Game )
        MESSAGE_HANDLER( WM_CLOSE,       OnClose )
        MESSAGE_HANDLER( WM_DESTROY,     OnDestroy)
        MESSAGE_HANDLER( WM_KEYDOWN,     OnKeyDown )
        MESSAGE_HANDLER( WM_KEYUP,       OnKeyUp )
        MESSAGE_HANDLER( WM_LBUTTONDOWN, OnMouseEvent )
        MESSAGE_HANDLER( WM_LBUTTONUP,   OnMouseEvent )
		MESSAGE_HANDLER( WM_RBUTTONDOWN, OnMouseEvent )
		MESSAGE_HANDLER( WM_RBUTTONUP,   OnMouseEvent )
		MESSAGE_HANDLER( WM_MBUTTONDOWN, OnMouseEvent )
		MESSAGE_HANDLER( WM_MBUTTONUP,   OnMouseEvent )
		MESSAGE_HANDLER( WM_MOUSEMOVE,   OnMouseEvent )
		MESSAGE_HANDLER( WM_MOUSEWHEEL,  OnMouseEvent )
		MESSAGE_HANDLER( WM_ACTIVATE,    OnActivate )
        MESSAGE_HANDLER( WM_SETFOCUS,    OnSetFocus )
        MESSAGE_HANDLER( WM_KILLFOCUS,   OnKillFocus )
		MESSAGE_HANDLER( WM_PAINT,		 OnPaint )
    END_MSG_MAP()


public:

	DECLARE_WND_CLASS(TEXT("PocketFrog"));

    Game();
    virtual ~Game();

    // Start running the game
    bool Run();
	virtual bool IsFullScreen() { return false; }
	virtual int GetWindowWidth() { return 800;  } //by default
	virtual int GetWindowHeight() { return 600; } //by default

	HWND m_hWnd;
protected:

    // Access to devices
    DisplayDevice* GetDisplay();
    InputDevice*   GetInput();

    // Game callbacks
    virtual void GameSuspend(); // Added 6/10/2003 by Frank W. Zammetti
    virtual void GameResume(); // Added 6/10/2003 by Frank W. Zammetti
    virtual bool GameInit();
    virtual void GameEnd();
    virtual void GameLoop();
    
    // Button press/release events
    virtual void ButtonDown( int button );
    virtual void ButtonUp( int button );

    // Stylus events
    virtual void StylusDown( Point p, MouseButton aMouseButton );
    virtual void StylusMove( Point p );
    virtual void StylusUp( Point p, MouseButton aMouseButton );
	virtual void StylusWheel( Point p, int delta);
    
    // Splashscreen
    virtual void SplashScreen();

	virtual void TryToUpdate() {}
    
    // Shutdown the game
    void Shutdown();

	// Report a fatal error
	void FatalError( LPCTSTR msg );

	//Show window - custom
	virtual void ShowWindowCustom();


protected:
    
    // Configuration
    struct Config
    {
        LPCTSTR     szWindowName;     // Window title
        unsigned    splashScreenTime; // Time in ms for PocketFrog's splash screen
        Orientation orientation;      // Screen orientation (default = ORIENTATION_NORTH)
				unsigned		desktopZoom;      // Set to zoom the size of the window on a desktop PC
				unsigned    frameRate;        // The maximum frame rate the app will run at
    } m_config;
private:

    // Initialize the game
    bool Init();

    // Display (or hide) the taskbar
    void ShowTaskbar( bool bShow );

    // Message handlers
    LRESULT OnClose( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnCreate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnDestroy( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnKeyDown( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnKeyUp( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnMouseEvent( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnActivate( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnSetFocus( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnKillFocus( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );
    LRESULT OnPaint( UINT msg, WPARAM wparam, LPARAM lparam, BOOL& bHandled );

    // Data
    bool                        m_bSuspended;   // Is the game suspended?
    scoped_ptr<DisplayDevice>   m_display;      // Display device
    scoped_ptr<InputDevice>     m_input;        // Input device

	bool m_bShuttedDown;
};



} // end of namespace Frog



#endif
