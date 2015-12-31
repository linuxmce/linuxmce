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

#ifndef POCKETFROG_INTERNAL_WIN32DRIVERS_H
#define POCKETFROG_INTERNAL_WIN32DRIVERS_H

#include "displaydriver.h"
#include "inputdriver.h"
#include "graphicbuffer.h"
#include "../input.h"

extern long g_lWindowWidth;
extern long g_lWindowHeight;

namespace Frog {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////
//
// Win32Display
//
//////////////////////////////////////////////////////////////////////////////

class Win32Display : public DisplayDriver
{
public:
    
    // Construction / destruction
    Win32Display(unsigned int desktopZoom = 0);
    ~Win32Display();

    // Display
    bool Initialize( HWND hwnd );
    void Shutdown();
    void Suspend();
    void Resume();

    // Scene management
    Pixel* BeginScene();
    void   EndScene( const Rect& rect );

    // Accessors
    int         GetWidth() const       { return m_buffer->GetWidth(); }
    int         GetHeight() const      { return m_buffer->GetHeight(); }
    int         GetPitch() const       { return m_buffer->GetPitch(); }
    int         GetPixelDepth() const  { return 16; }
    Orientation GetOrientation() const { return ORIENTATION_NORTH; }
    bool        IsBuffered() const     { return true; }


private:

    scoped_ptr<GDIBuffer> m_buffer;
    HWND                  m_hwnd;
		unsigned int          m_desktopZoom;
};



//////////////////////////////////////////////////////////////////////////////
//
// Win32Input
//
//////////////////////////////////////////////////////////////////////////////

class Win32Input : public InputDriver
{
public:

    // Construction / destruction
    Win32Input();

    // Input
    bool Initialize( HWND hwnd ) { return true; }
    void Shutdown() {}
    void Suspend() {}
    void Resume() {}

    // Retrieve the list of button scan codes
    const KeyList& GetKeyList() const { return m_keyList; }


private:

   KeyList m_keyList;
};



} // end of namespace Internal
} // end of namespace Frog



#endif
