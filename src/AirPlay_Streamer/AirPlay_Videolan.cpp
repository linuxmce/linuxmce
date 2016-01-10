/**
 * AirPlay_Videolan - A class that talks to libVLC and provides the media player
 * for the AirPlay part of the protocol
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"
#include "AirPlay_Videolan.h"

#include "utilities/linux/window_manager/WMController/WMController.h"

#define INACTIVE_VIDEO "/usr/pluto/share/black.mpeg"

namespace DCE
{

  AirPlay_Videolan::AirPlay_Videolan()
  {
    m_pInst = NULL;
    m_pMp = NULL;
    m_bIsPlaying=false;
    m_sWindowTitle = "airplay";
  }

  AirPlay_Videolan::~AirPlay_Videolan()
  {
    libvlc_media_player_release(m_pMp);
    libvlc_release(m_pInst);
    m_pInst = NULL;
    m_pMp = NULL;
    m_bIsPlaying=false;
  }

  bool AirPlay_Videolan::init()
  {
    libvlc_media_t *pInactive_Media;

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

    if (!FileUtils::FileExists(INACTIVE_VIDEO))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Videolan::init() could not open inactive video: ",INACTIVE_VIDEO);
	return false;
      }

    if (!CreateWindow())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Videolan::init() could not create target window.");
	return false;
      }

    pInactive_Media = libvlc_media_new_path(m_pInst, INACTIVE_VIDEO);
    m_pMp = libvlc_media_player_new_from_media(pInactive_Media);
    libvlc_audio_output_device_set(m_pMp, "alsa", "plughw:0,3");
    libvlc_media_release(pInactive_Media);
    libvlc_media_player_set_xwindow (m_pMp, m_Window);
    libvlc_media_player_play(m_pMp);
    
    if (!Minimize()) 
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Videolan::init() could not minimize VLC window.");
	// return false;
      }
    
    return true;

  }

  bool AirPlay_Videolan::CreateWindow()
  {
    XColor black;
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

  bool AirPlay_Videolan::Minimize()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_VideoLan::MinimizeVLC - attempting to minimize VLC window.");

    if (!m_pDisplay)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Videolan::MinimizeVLC - no display set.");
	return false;
      }
    
    WMControllerImpl *pWMController = new WMControllerImpl();
    bool bResult = pWMController->SetVisible("airplay.airplay", false);

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Videolan::MinimizeVLC WMController SetVisible returned %d", bResult);
    
    if(!bResult)
      {
	//failed to move the window, try again in half of second
	Sleep(500);
	
	bResult = pWMController->SetVisible("airplay.airplay", false);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "AirPlay_Videolan::MinimizeVLC returned %d", bResult);
	return true;
      }	
    
    delete pWMController;
    pWMController = NULL;
    
    return true;

  }

  void AirPlay_Videolan::PlayURL(string sMediaURL)
  {
    libvlc_time_t iLength=0;
    string::size_type pos=0;
    libvlc_media_player_stop(m_pMp);
    if (sMediaURL.find("://") == string::npos)
      {
	sMediaURL = "file://" + sMediaURL;
      }
    
    sMediaURL=StringUtils::Tokenize(sMediaURL,"|",pos); // we only want the main part.

    libvlc_media_t *m = libvlc_media_new_location (m_pInst, sMediaURL.c_str());
    libvlc_media_player_set_media(m_pMp, m);
    libvlc_media_release(m);
    libvlc_media_player_set_xwindow (m_pMp, m_Window);
    SetDuration(libvlc_media_player_get_length(m_pMp));
    libvlc_media_player_play(m_pMp);
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"media player is seekable? %d",libvlc_media_player_is_seekable(m_pMp));
    SetPlaying(true);
  }

  void AirPlay_Videolan::Stop()
  {
    libvlc_media_player_stop(m_pMp);
    SetPlaying(false);
  }

  void AirPlay_Videolan::SetDuration(libvlc_time_t newDuration)
  {
    m_fDuration = (float)newDuration / 1000;
  }

  float AirPlay_Videolan::GetDuration()
  {
    return m_fDuration;
  }

  float AirPlay_Videolan::GetPosition()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GETPOSITION %f",m_fPosition);
    return m_fPosition;
  }

  void AirPlay_Videolan::SetPosition(float fPosition)
  {
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"AirPlay_Videolan::SetPosition(%f)",fPosition);
    libvlc_media_player_set_position(m_pMp, fPosition);
  }

  bool AirPlay_Videolan::IsPlaying()
  {
    return m_bIsPlaying;
  }

  void AirPlay_Videolan::SetPlaying(bool bIsPlaying)
  {
    m_bIsPlaying = bIsPlaying;
  }

  void AirPlay_Videolan::UpdateStatus()
  {
    SetDuration(0);
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"TIME: %d",libvlc_media_player_get_time(m_pMp));
    m_fPosition = libvlc_media_player_get_time(m_pMp) / 1000;
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CALCTIME: %f",m_fPosition);
  }

  void AirPlay_Videolan::SetTime(float fTime)
  {
    libvlc_time_t iTime = fTime * 1000;
    libvlc_media_player_set_time(m_pMp,iTime);
  }

  void AirPlay_Videolan::Pause()
  {
    if (m_pMp)
      libvlc_media_player_set_pause(m_pMp, 1);
  }
  
  void AirPlay_Videolan::Restart()
  {
    if (m_pMp)
      libvlc_media_player_set_pause(m_pMp, 0);
  }

  void AirPlay_Videolan::SetRate(float fMediaPlayBackSpeed)
  {
    if (m_pMp)
      libvlc_media_player_set_rate(m_pMp, fMediaPlayBackSpeed);
  }

  void AirPlay_Videolan::JumpFwd(int iMult)
  {
    libvlc_time_t iTime;

    if (!m_pMp)
      return;

    libvlc_media_player_set_time(m_pMp, (m_fPosition * 1000) + (iMult * 30 * 1000));    
  }
  
  void AirPlay_Videolan::JumpBack(int iMult)
  {
    libvlc_time_t iTime;
    
    if (!m_pMp)
      return;
    
    libvlc_media_player_set_time(m_pMp, (m_fPosition * 1000) - (iMult * 30 * 1000));
  }

}
