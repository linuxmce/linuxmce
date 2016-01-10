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

#ifndef POCKETFROG_INTERNAL_GAPILIB_H
#define POCKETFROG_INTERNAL_GAPILIB_H

#include "displaydriver.h"
#include "inputdriver.h"
#include "../input.h"

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// GAPILibrary
//
//////////////////////////////////////////////////////////////////////////////

class GAPILibrary : public DisplayDriver, public InputDriver
{
public:

    // Construction / destruction
    GAPILibrary();
    ~GAPILibrary();

    // Driver interface
    bool Initialize( HWND hwnd );
    void Shutdown();
    void Suspend();
    void Resume();

    // Scene management
    Pixel* BeginScene();
    void   EndScene( const Rect& rect );

    // Accessors
    int         GetWidth() const       { return m_width; }
    int         GetHeight() const      { return m_height; }
    int         GetPitch() const       { return m_pitch; }
    int         GetPixelDepth() const  { return m_bpp; }
    Orientation GetOrientation() const { return m_orientation; }
    bool        IsBuffered() const     { return m_bBuffered; }

    // Get the list of virtual key codes
    const KeyList& GetKeyList() const  { return m_keyList; }


private:

    // GAPI
    HINSTANCE m_hGAPI;

    struct GXDisplayProperties
    {
        DWORD cxWidth, cyHeight;
        long  cbxPitch, cbyPitch;
        long  cBPP;
        DWORD ffFormat;
    };

    struct GXKeyList
    {
        short vkUp;     POINT ptUp;
        short vkDown;   POINT ptDown;
        short vkLeft;   POINT ptLeft;
        short vkRight;  POINT ptRight;
        short vkA;      POINT ptA;
        short vkB;      POINT ptB;
        short vkC;      POINT ptC;
        short vkStart;  POINT ptStart;
    };

    enum
    {
        GX_FULLSCREEN	 = 0x01,
        GX_NORMALKEYS	 = 0x02,
        GX_LANDSCAPEKEYS = 0x03
    };

    typedef int   (*PFNGXOpenDisplay)(HWND hWnd, DWORD dwFlags);
    typedef int   (*PFNGXCloseDisplay)();
    typedef void* (*PFNGXBeginDraw)();
    typedef int   (*PFNGXEndDraw)();
    typedef int   (*PFNGXOpenInput)();
    typedef int   (*PFNGXCloseInput)();
    typedef GXDisplayProperties (*PFNGXGetDisplayProperties)();
    typedef GXKeyList (*PFNGXGetDefaultKeys)(int iOptions);
    typedef int   (*PFNGXSuspend)();
    typedef int   (*PFNGXResume)();
    typedef int   (*PFNGXSetViewport)( DWORD dwTop, DWORD dwHeight, DWORD dwReserved1, DWORD dwReserved2 );
    typedef BOOL  (*PFNGXIsDisplayDRAMBuffer)();
    
    PFNGXOpenDisplay          GXOpenDisplay;
    PFNGXCloseDisplay         GXCloseDisplay;
    PFNGXBeginDraw            GXBeginDraw;
    PFNGXEndDraw              GXEndDraw;
    PFNGXOpenInput            GXOpenInput;
    PFNGXCloseInput           GXCloseInput;
    PFNGXGetDisplayProperties GXGetDisplayProperties;
    PFNGXGetDefaultKeys       GXGetDefaultKeys;
    PFNGXSuspend              GXSuspend;
    PFNGXResume               GXResume;
    PFNGXSetViewport          GXSetViewport;
    PFNGXIsDisplayDRAMBuffer  GXIsDisplayDRAMBuffer;

    // Load GAPI
    bool LoadGAPI();
    
    bool OpenDisplay( HWND hwnd );
    bool OpenInput();
    void CloseDisplay();
    void CloseInput();

    // Graphics
    bool        m_bDisplayOpen;
    Pixel*      m_GAPIBypass;
    int         m_GAPIOffset;
    int         m_width, m_height;
    int         m_pitch;
    int         m_bpp;
    Orientation m_orientation;
    bool        m_bBuffered;

    // Input
    bool        m_bInputOpen;
    KeyList     m_keyList;

    // Suspension
    int         m_suspendCount;
};



} // end of namespace Internal
} // end of namespace Frog



#endif
