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
#include "WindowUtils/WindowUtils.h"
#include <X11/keysym.h>

// #define INACTIVE_VIDEO "/home/public/data/videos/rejected.dvd"
#define INACTIVE_VIDEO "/usr/pluto/share/black.mpeg"

namespace DCE
{
  VLC::VLC(Config* pConfig)
  {
    m_pInst = NULL;
    m_pMp = NULL;
    m_pConfig = pConfig;
    m_sWindowTitle = "lmcevlc";
    m_iSerialNum=0;
    m_sAudioInfo="";
    m_sVideoInfo="";
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
    XInitThreads();
    if ((m_pDisplay = XOpenDisplay(getenv("DISPLAY"))) == NULL)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Videolan::init() - Could not open X display from %s",getenv("DISPLAY"));
	return false;
      }

    static const char* const args[] = {"--no-video-title-show"};

    // m_pInst = libvlc_new(sizeof args / sizeof *args, args);
    m_pInst = libvlc_new(NULL, NULL);

    if (!m_pInst)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Videolan::init() failed.");
	return false;
      }
    
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
    bool bResult = pWMController->SetVisible("lmcevlc.lmcevlc", false);

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

  bool VLC::PlayURL(string sMediaURL, string sMediaPosition, string& sMediaInfo)
  {
    // libvlc_time_t iLength=0;
    string::size_type pos=0;
    libvlc_media_player_stop(m_pMp);
    if (sMediaURL.find("://") == string::npos)
      {
	sMediaURL = "file://" + sMediaURL;
      }
    
    libvlc_media_t *m = libvlc_media_new_location (m_pInst, sMediaURL.c_str());
    if (!m)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::PlayURL() libvlc_media_new_location() returned NULL.");
	return false;
      }

    libvlc_media_player_set_media(m_pMp, m);
    libvlc_media_release(m);
    libvlc_media_player_set_xwindow (m_pMp, m_Window);
    SetDuration(libvlc_media_player_get_length(m_pMp));
    if (libvlc_media_player_play(m_pMp)<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::PlayURL() - libvlc_media_player_play not successful.");
	SetPlaying(false);
	SetAudioVideoInfo();
	return false;
      }
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"media player is seekable? %d",libvlc_media_player_is_seekable(m_pMp));
    SetPlaying(true);
    return true;
  }

  void VLC::Stop()
  {
    libvlc_media_player_stop(m_pMp);
    SetPlaying(false);
  }

  void VLC::SetDuration(libvlc_time_t newDuration)
  {
    m_fDuration = (float)newDuration / 1000;
  }

  float VLC::GetDuration()
  {
    return m_fDuration;
  }

  float VLC::GetPosition()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GETPOSITION %f",m_fPosition);
    return m_fPosition;
  }

  void VLC::SetPosition(float fPosition)
  {
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::SetPosition(%f)",fPosition);
    libvlc_media_player_set_position(m_pMp, fPosition);
  }

  bool VLC::IsPlaying()
  {
    return m_bIsPlaying;
  }

  void VLC::SetPlaying(bool bIsPlaying)
  {
    m_bIsPlaying = bIsPlaying;
  }

  void VLC::UpdateStatus()
  {
#ifdef TRANSPORT_DEBUG
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TIME: %d",libvlc_media_player_get_time(m_pMp));
    m_fPosition = libvlc_media_player_get_time(m_pMp) / 1000;
    m_fDuration = libvlc_media_player_get_length(m_pMp) / 1000;
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"POSITION: %f",m_fPosition);
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DURATION: %f",m_fDuration);
#endif
  }

  int64_t VLC::SetTime(int64_t iTime)
  {
    libvlc_media_player_set_time(m_pMp,iTime);
    return libvlc_media_player_get_time(m_pMp);
  }

  int64_t VLC::GetTime()
  {
    return libvlc_media_player_get_time(m_pMp);
  }

  void VLC::Pause()
  {
    if (m_pMp)
      libvlc_media_player_set_pause(m_pMp, 1);
  }
  
  void VLC::Restart()
  {
    if (m_pMp)
      libvlc_media_player_set_pause(m_pMp, 0);
  }

  void VLC::SetRate(float fMediaPlayBackSpeed)
  {
    if (m_pMp)
      libvlc_media_player_set_rate(m_pMp, fMediaPlayBackSpeed);
  }

  void VLC::JumpFwd(int iMult)
  {
    // libvlc_time_t iTime;

    if (!m_pMp)
      return;

    libvlc_media_player_set_time(m_pMp, (m_fPosition * 1000) + (iMult * 30 * 1000));    
  }
  
  void VLC::JumpBack(int iMult)
  {
    // libvlc_time_t iTime;
    
    if (!m_pMp)
      return;
    
    libvlc_media_player_set_time(m_pMp, (m_fPosition * 1000) - (iMult * 30 * 1000));
  }

  void VLC::MoveUp()
  {
    if (!IsWindowValid())
      return;

    WindowUtils::SendKeyToWindow(m_pDisplay,m_Window,XK_Up,m_iSerialNum++);

  }
  
  void VLC::MoveDown()
  {
    if (!IsWindowValid())
      return;

    WindowUtils::SendKeyToWindow(m_pDisplay,m_Window,XK_Down,m_iSerialNum++);

  }

  void VLC::MoveLeft()
  {
    if (!IsWindowValid())
      return;

    WindowUtils::SendKeyToWindow(m_pDisplay,m_Window,XK_Left,m_iSerialNum++);

  }

  void VLC::MoveRight()
  {
    if (!IsWindowValid())
      return;

    WindowUtils::SendKeyToWindow(m_pDisplay,m_Window,XK_Up,m_iSerialNum++);

  }

  void VLC::EnterGo()
  {
    if (!IsWindowValid())
      return;

    WindowUtils::SendKeyToWindow(m_pDisplay,m_Window,XK_Return,m_iSerialNum++);
    
  }

  void VLC::SetAudioVideoInfo()
  {
    m_sAudioInfo="";
    m_sVideoInfo="";
  }

}
