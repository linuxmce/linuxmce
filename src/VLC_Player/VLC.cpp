/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"
#include "VLC.h"

#include "utilities/linux/window_manager/WMController/WMController.h"

#define INACTIVE_VIDEO "/usr/pluto/share/black.mpeg"

namespace DCE
{
  VLC::VLC(Config* pConfig)
  {
    m_pInst = NULL;
    m_pMp = NULL;
    m_pConfig = pConfig;
    m_sWindowTitle = "linuxmcevlc";
  }

  VLC::~VLC()
  {
    if (m_pMp)
      {
	libvlc_media_player_release(m_pMp);
	m_pMp=NULL;
      }
    if (m_pInst)
      {
	libvlc_release(m_pInst);
	m_pInst=NULL;
      }
  }

  bool VLC::init()
  {
    libvlc_media_t* pInactive_media;
    
    if ((m_pDisplay = XOpenDisplay(getenv("DISPLAY"))) == NULL)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::init() - Could not open X display from %s",getenv("DISPLAY"));
	return false;
      }

    if (!FileUtils::FileExists(INACTIVE_VIDEO))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::init() could not open inactive video: ",INACTIVE_VIDEO);
	return false;
      }

    if (!CreateWindow())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::init() could not create target window.");
	return false;
      }

    pInactive_media = libvlc_media_new_path(m_pInst, INACTIVE_VIDEO);
    m_pMp = libvlc_media_player_new_from_media(pInactive_media);
    libvlc_media_release(pInactive_media);
    libvlc_media_player_set_xwindow (m_pMp, m_Window);
    libvlc_media_player_play(m_pMp);
    
    if (!Minimize()) 
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC::init() could not minimize VLC window.");
	// return false;
      }
    
    return true;
  }

    bool VLC::CreateWindow()
    {
      int xpos, ypos, width, height, border;
      XSizeHints sizeHints;
      XClassHint classHint;
      
      xpos=10;
      ypos=20;
      width=100;
      height=100;
      border=0;
      
      XLockDisplay(m_pDisplay);
      
      m_Window = XCreateSimpleWindow(m_pDisplay, XDefaultRootWindow(m_pDisplay), xpos, ypos, width, height, border, 0, 0);
      
      classHint.res_name = ( char* ) m_sWindowTitle.c_str();
      classHint.res_class = ( char* ) m_sWindowTitle.c_str();
      XSetClassHint ( m_pDisplay, m_Window, &classHint );
    
    classHint.res_class = ( char* ) m_sWindowTitle.c_str();
    XSetClassHint ( m_pDisplay, m_Window, &classHint );
    
    XSetStandardProperties( m_pDisplay, m_Window, m_sWindowTitle.c_str(), m_sWindowTitle.c_str(), None, NULL, 0, 0 );

    // changing wm hints
    sizeHints.win_gravity = StaticGravity;
    sizeHints.flags = PPosition | PSize | PWinGravity;
    
    XSetWMNormalHints( m_pDisplay, m_Window, &sizeHints );
    Atom XA_DELETE_WINDOW = XInternAtom( m_pDisplay, "WM_DELETE_WINDOW", False );
    XSetWMProtocols( m_pDisplay, m_Window, &XA_DELETE_WINDOW, 1 );

    int xcode = XMapWindow( m_pDisplay, m_Window );
    LoggerWrapper::GetInstance()->Write( LV_WARNING, "XMapWindow returned: %i", xcode);
    
    XSync(m_pDisplay, false);

    XUnlockDisplay(m_pDisplay);

    return true;
  }

  bool VLC::Minimize()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_VideoLan::MinimizeVLC - attempting to minimize VLC window.");

    if (!m_pDisplay)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::MinimizeVLC - no display set.");
	return false;
      }
    
    WMControllerImpl *pWMController = new WMControllerImpl();
    bool bResult = pWMController->SetVisible("linuxmcevlc.linuxmcevlc", false);

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC::MinimizeVLC WMController SetVisible returned %d", bResult);
    
    if(!bResult)
      {
	//failed to move the window, try again in half of second
	Sleep(500);
	
	bResult = pWMController->SetVisible("linuxmcevlc.linuxmcevlc", false);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC::MinimizeVLC returned %d", bResult);
	return true;
      }	
    
    delete pWMController;
    pWMController = NULL;
    
    return true;

  }  

}
