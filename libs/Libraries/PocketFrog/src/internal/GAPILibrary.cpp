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

#include "GAPILibrary.h"
#include "../pocketpc.h"

#if defined(_M_IX86)
#include "../foreign/GAPI/gx.h"
#endif

#include <cassert>

namespace Frog {
namespace Internal {



//////////////////////////////////////////////////////////////////////////////
//
// Local helpers
//
//////////////////////////////////////////////////////////////////////////////

Orientation DetectOrientation( int pitchX, int pitchY )
{
   if (pitchX > 0)
   {
      // This check also works for landscape monochrome modes (pitchY==0)
      if (pitchY > 0)
         return ORIENTATION_NORTH;
      else
         return ORIENTATION_WEST;
   }
   else
   {
      // This check also works for inverse-landscape monochrome modes (pitchY==0)
      if (pitchY >=  0)
         return ORIENTATION_EAST;
      else
         return ORIENTATION_SOUTH;
   }
}


    
//////////////////////////////////////////////////////////////////////////////
//
// GAPILibrary
//
//////////////////////////////////////////////////////////////////////////////

GAPILibrary::GAPILibrary()
:   m_hGAPI(0),
    m_bDisplayOpen(false),
    m_bInputOpen(false),
    m_suspendCount(0)
{
    memset( &m_keyList, 0, sizeof(m_keyList) );
}



GAPILibrary::~GAPILibrary()
{
    Shutdown();
}



bool GAPILibrary::Initialize( HWND hwnd )
{
    if (!LoadGAPI())
        return false;

    if (!OpenDisplay( hwnd ))
        return false;

    if (!OpenInput())
    {
        CloseDisplay();
        return false;
    }

    return true;
}



void GAPILibrary::Shutdown()
{
    if (m_bInputOpen)
        CloseInput();

    if (m_bDisplayOpen)
        CloseDisplay();

    if (m_hGAPI)
        FreeLibrary( m_hGAPI );
}



#define LINK(name,import) name = (PFN##name)GetProcAddress( m_hGAPI, _T(import) );



bool GAPILibrary::LoadGAPI()
{
    // Already loaded?
    if (m_hGAPI) return true;
    
    
    // Load GAPI.DLL
    m_hGAPI = LoadLibrary( _T("GX.dll") );
    if (!m_hGAPI)
    {
#if defined(FROG_PPC) && !defined(_X86_)
        MessageBox( 0, _T("GAPI (GX.DLL) wasn't found."), _T("PocketFrog"), MB_ICONERROR );
#endif
        return false;
    }

    // Find the needed functions
    LINK( GXOpenDisplay,         "?GXOpenDisplay@@YAHPAUHWND__@@K@Z" )
    LINK( GXCloseDisplay,        "?GXCloseDisplay@@YAHXZ" )
    LINK( GXBeginDraw,           "?GXBeginDraw@@YAPAXXZ" )
    LINK( GXEndDraw,             "?GXEndDraw@@YAHXZ" )
    LINK( GXOpenInput,           "?GXOpenInput@@YAHXZ" )
    LINK( GXCloseInput,          "?GXCloseInput@@YAHXZ" )
    LINK( GXGetDisplayProperties,"?GXGetDisplayProperties@@YA?AUGXDisplayProperties@@XZ" )
    LINK( GXGetDefaultKeys,      "?GXGetDefaultKeys@@YA?AUGXKeyList@@H@Z" )
    LINK( GXSuspend,             "?GXSuspend@@YAHXZ" )
    LINK( GXResume,              "?GXResume@@YAHXZ" )
    LINK( GXSetViewport,         "?GXSetViewport@@YAHKKKK@Z" )
    LINK( GXIsDisplayDRAMBuffer, "?GXIsDisplayDRAMBuffer@@YAHXZ" )

    return true;
}

#undef LINK



bool GAPILibrary::OpenDisplay( HWND hwnd )
{
    if (m_bDisplayOpen)
    {
        assert(0);
        return false;
    }

	// Don't validate return value, it's sometime wrong!
	BOOL bResult = GXOpenDisplay( hwnd, GX_FULLSCREEN );

    m_GAPIBypass = 0;
    m_GAPIOffset = 0;
    m_bBuffered  = GXIsDisplayDRAMBuffer() != 0;

    GXDisplayProperties GAPIProps = GXGetDisplayProperties();


#if defined(_ARM_)
    
    // If we have the iPAQ 38xx, make sure it's not the buggy GX.DLL
    if (PocketPC::GetModel() == PocketPC::MODEL_IPAQ_3800)
    {
        if ( GAPIProps.cbxPitch < 0 && GAPIProps.cbyPitch > 0 )
        {
            // Seems to be a patched version...
        }
        else
        {
            // It's the buggy version, patch it
            m_GAPIBypass       = (Pixel*)0xAC0755A0;
            GAPIProps.cxWidth  = 240;
            GAPIProps.cyHeight = 320;
            GAPIProps.cbxPitch = -640;
            GAPIProps.cbyPitch = 2;
            GAPIProps.cBPP     = 16;
            GAPIProps.ffFormat = 0xA8;
            m_bBuffered        = false;
        }
    }
    
#endif
    
    
    // Figure out the orientation and properties of the GAPI buffer
    m_orientation = DetectOrientation( GAPIProps.cbxPitch, GAPIProps.cbyPitch );    

    // Find the GAPI video pointer offset
    if (m_orientation == ORIENTATION_WEST)
    {
        m_GAPIOffset = - (int)(GAPIProps.cyHeight * GAPIProps.cBPP) / 8 + (GAPIProps.cBPP + 7) / 8;
    }
    else if (m_orientation == ORIENTATION_EAST)
    {
        m_GAPIOffset = (GAPIProps.cxWidth - 1) * GAPIProps.cbxPitch;
    }
    else if (m_orientation == ORIENTATION_SOUTH)
    {
        m_GAPIOffset = - abs(GAPIProps.cbxPitch * GAPIProps.cbyPitch) + 2;
    }

    // Find out width and height of the GAPI buffer
    if (m_orientation == ORIENTATION_NORTH || m_orientation == ORIENTATION_SOUTH)
    {
        m_width  = GAPIProps.cxWidth;
        m_height = GAPIProps.cyHeight;
        m_pitch  = abs( GAPIProps.cbyPitch );
    }
    else
    {
        m_width  = GAPIProps.cyHeight;
        m_height = GAPIProps.cxWidth;
        m_pitch  = abs( GAPIProps.cbxPitch );
    }

    m_bpp = GAPIProps.cBPP;

    
    m_bDisplayOpen = true;
    
    return true;
}



void GAPILibrary::CloseDisplay()
{
    if (!m_bDisplayOpen)
    {
        assert(0);
        return;
    }

    GXCloseDisplay();
    
    m_bDisplayOpen = false;
}



void GAPILibrary::Suspend()
{
    if (m_suspendCount++ == 0)
    {
        GXSuspend();
    }
}



void GAPILibrary::Resume()
{
    if (--m_suspendCount == 0)
    {
        GXResume();
    }
}



Pixel* GAPILibrary::BeginScene()
{
    Pixel* pVideo;
    
    if (m_GAPIBypass)
        pVideo = m_GAPIBypass;
    else
        pVideo = (Pixel*) GXBeginDraw();
    
    pVideo = (Pixel*)((char*)pVideo + m_GAPIOffset);
    
    return pVideo;
}



void GAPILibrary::EndScene( const Rect& rect )
{
    if (!m_GAPIBypass)
    {
        GXSetViewport( rect.top, rect.GetHeight(), rect.left, rect.GetWidth() );
        GXEndDraw();
    }
}



bool GAPILibrary::OpenInput()
{
    if (m_bInputOpen)
    {
        assert(0);
        return false;
    }


	// Don't validate return value, it's sometimes wrong!
    GXOpenInput();


    GXKeyList keys = GXGetDefaultKeys( GX_NORMALKEYS );

    m_keyList.vkUp    = keys.vkUp;
    m_keyList.vkDown  = keys.vkDown;
    m_keyList.vkLeft  = keys.vkLeft;
    m_keyList.vkRight = keys.vkRight;
    m_keyList.vkA     = keys.vkA;
    m_keyList.vkB     = keys.vkB;
    m_keyList.vkC     = keys.vkC;
    m_keyList.vkStart = keys.vkStart;


    switch(PocketPC::GetModel())
    {
    case PocketPC::MODEL_CASIO_E115:    // Assuming same as E-125
    case PocketPC::MODEL_CASIO_E125:
        m_keyList.vkAux1 = 0xC0;        // E-125 aux1
        m_keyList.vkAux2 = 0xC1;        // E-125 aux2
        break;

    case PocketPC::MODEL_IPAQ_3800:     // Assuming same as iPAQ 3600
    case PocketPC::MODEL_IPAQ_3600:
        m_keyList.vkAux1 = 0x86;        // iPAQ d-pad button
        break;

    case PocketPC::MODEL_JORNADA_568:
        m_keyList.vkAux1 = 0xD5;        // Jornada 568 aux1
        break;
    }

    
    m_bInputOpen = true;
    
    return true;
}



void GAPILibrary::CloseInput()
{
    if (!m_bInputOpen)
    {
        assert(0);
        return;
    }

    GXCloseInput();

    m_bInputOpen = false;
}



} // end of namespace Internal
} // end of namespace Frog
