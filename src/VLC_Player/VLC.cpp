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

#include "VLC_Player.h"
#include "Position.h"

#include <algorithm>

#define INACTIVE_VIDEO "/usr/pluto/share/vlcblack.mpeg"

/**
 * Convert a string to a c string for use in char** array. 
 */
char *convert(const std::string & s)
{
   char *pc = new char[s.size()+1];
   std::strcpy(pc, s.c_str());
   return pc; 
}

namespace DCE
{

  VLC::Config::Config()
  {
    m_sSoundCard="";
    m_sAudioSettings="";
    m_sHardwareAcceleration="";
    m_sDeinterlacingMode="";
    m_sVideoSettings="";
    m_iWidth=0;
    m_iHeight=0;
  }

  VLC::Config::~Config()
  {
    
  }
  
  bool VLC::Config::parseVideoSettings()
  {
    if (m_sVideoSettings.empty())
      return false;

    string::size_type pos=0;
    int m_iWidth=atoi(StringUtils::Tokenize(m_sVideoSettings," ",pos).c_str());
    int m_iHeight=atoi(StringUtils::Tokenize(m_sVideoSettings,"/",pos).c_str());

    if (m_iWidth==0 || m_iHeight==0)
      return false;

    return true;
    
  }

  VLC::VLC(Config* pConfig, VLC_Player* pVLC_Player)
  {
    m_pVLC_Player = pVLC_Player;
    m_pInst = NULL;
    m_pMp = NULL;
    m_pMediaEventManager=NULL;
    m_pMediaPlayerEventManager=NULL;
    m_pConfig = pConfig;
    m_sWindowTitle = "lmcevlc";
    m_iSerialNum=0;
    m_sAudioInfo="";
    m_sVideoInfo="";
    m_iStreamID=0;
    m_iPreviousAudioTrack=1;
    m_bEventsAttached=false;
    m_iZoomPercent=100;
  }

  VLC::~VLC()
  {
    if (m_bEventsAttached)
      {
	libvlc_event_detach(m_pMediaEventManager,libvlc_MediaParsedChanged,Media_Callbacks,this);
	libvlc_event_detach(m_pMediaEventManager,libvlc_MediaStateChanged,Media_Callbacks,this);
	libvlc_event_detach(m_pMediaPlayerEventManager,libvlc_MediaPlayerPlaying,Media_Callbacks,this);
	libvlc_event_detach(m_pMediaPlayerEventManager,libvlc_MediaPlayerVout,Media_Callbacks,this);
	m_bEventsAttached=false;
      }

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
    XInitThreads();
    if ((m_pDisplay = XOpenDisplay(getenv("DISPLAY"))) == NULL)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AirPlay_Videolan::init() - Could not open X display from %s",getenv("DISPLAY"));
	return false;
      }

    if (FileUtils::FileExists("/etc/pluto/libvlc.conf"))
      {
	vector<string> vs;
	vector<char*> vc;
	FileUtils::ReadFileIntoVector("/etc/pluto/libvlc.conf",vs);
	if (vs.size()>0)
	  {
	    std::transform(vs.begin(), vs.end(), std::back_inserter(vc), convert);
	    
	    // Pass parameters to libvlc.
	    m_pInst=libvlc_new(vc.size(),&vc[0]);

	    // char** parameter array no longer needed, blast it. 
	    for (size_t i=0; i<vc.size(); i++)
	      {
		delete [] vc[i];
	      }
	  }
	else
	  {
	    // Empty configuration file found, assume default parameters.
	    m_pInst=libvlc_new(NULL,NULL);
	  }
      }
    else
      {
	// No configuration file found, assume default parameters.
	m_pInst=libvlc_new(NULL,NULL);
      }
    
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
    
    if (!Minimize()) 
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VLC::init() could not minimize VLC window.");
	// return false;
      }

    if (!m_pConfig->parseVideoSettings())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::init() could not parse media director video settings.");
	return false;
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

  void VLC::UpdateMediaState(const libvlc_event_t* event)
  {
    switch (event->u.media_state_changed.new_state)
      {
      case libvlc_NothingSpecial:
	break;
      case libvlc_Opening:
	break;
      case libvlc_Buffering:
	break;
      case libvlc_Playing:
	m_pVLC_Player->StartTimecodeReporting();
	break;
      case libvlc_Paused:
	break;
      case libvlc_Stopped:
	m_pVLC_Player->StopTimecodeReporting();
	break;
      case libvlc_Ended:
	m_pVLC_Player->StopTimecodeReporting();
	m_pVLC_Player->EVENT_Playback_Completed(GetMediaURL(),GetStreamID(),false); // Normal end of playback
	break;
      case libvlc_Error:
	m_pVLC_Player->StopTimecodeReporting();
	m_pVLC_Player->EVENT_Playback_Completed(GetMediaURL(),GetStreamID(),true); // Something went wrong. 
	break;
      }
  }

  void VLC::Media_Callbacks(const libvlc_event_t* event, void* ptr)
  {

    VLC* self = reinterpret_cast<VLC*>(ptr);

    if (!self->m_bIsPlaying)
      return;  // Don't even come here, if the media player is stopped.

    switch(event->type)
      {
      case libvlc_MediaParsedChanged:
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Media_Callbacks(): MEDIA PARSED CHANGED!!!");
	self->UpdateNav();
	break;
      case libvlc_MediaStateChanged:
	self->UpdateMediaState(event);
	self->UpdateNav();
	break;
      case libvlc_MediaPlayerPlaying:
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Media_Callbacks(): MEDIA PLAYING!!");
	self->UpdateNav();
	break;
      case libvlc_MediaPlayerVout:
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Media_Callbacks(): MEDIA VOUT!!");
	self->m_iPreviousAudioTrack = self->GetVLCAudioTrack();
	if (self->m_pVLC_Player->m_bIsStreaming)
	  {
	    self->Pause();
	  }
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Media_Callbacks(): Previous audio track reset to %d",self->m_iPreviousAudioTrack);
	self->ReportPlaybackStarted();
	self->UpdateNav();
	break;
      case libvlc_MediaPlayerStopped:
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Media_Callbacks(): MEDIA STOPPED!!");
	self->m_pVLC_Player->EVENT_Playback_Completed(self->GetMediaURL(),self->GetStreamID(),false);
	break;
      }
    
  }

  void VLC::UpdateTracks()
  {
    if (!m_pMp)
      return;

    m_pVLC_Player->DATA_Set_Audio_Tracks(GetAllAudioTracks());
    m_pVLC_Player->DATA_Set_Subtitles(GetAllSubtitles());
    m_pVLC_Player->DATA_Set_Angles(GetAllAngles());
  }

  void VLC::SetMediaURL(string sMediaURL)
  {
    m_sMediaURL=sMediaURL;
  }

  string VLC::GetMediaURL()
  {
    return m_sMediaURL;
  }

  bool VLC::PlayURL(string sMediaURL, int iStreamID, string sMediaPosition, string& sMediaInfo, bool bIsStreaming)
  {
    if (m_pMp)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL - Media already playing, stopping it.");
	SetPlaying(false);
	libvlc_media_player_stop(m_pMp);
	libvlc_media_player_release(m_pMp);
      }

    m_iChapter=0;
    m_iPreviousAudioTrack=1; // Hopefully, this fixes single track audio files.

    if (sMediaURL.find("://") == string::npos)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL - file based media detected.");
	sMediaURL = "file://" + sMediaURL;
      }

    libvlc_media_t *m;

    // Handle audio CDs a bit differently. Since the Disk Drive 
    // already reports tracks, there is no need for libvlc to do this
    // and we can just tell it to play specific tracks. 
    // cdda:///dev/sr0/4
    if (sMediaURL.find("cdda:") != string::npos)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL - Audio CD track detected. Parsing CD Track data.");
	string::size_type pos=0;
	string sProtocol = StringUtils::Tokenize(sMediaURL,":",pos);
	string sDummy1 = StringUtils::Tokenize(sMediaURL,"/",pos);
	string sDummy2 = StringUtils::Tokenize(sMediaURL,"/",pos);
	string sDummy3 = StringUtils::Tokenize(sMediaURL,"/",pos);
	string sDev = StringUtils::Tokenize(sMediaURL,"/",pos);
	string sDrive = StringUtils::Tokenize(sMediaURL,"/",pos);
	int iTrack = atoi(StringUtils::Tokenize(sMediaURL,"/",pos).c_str());
	char cTrack[16];
	char cDev[16]; 
	sprintf(cTrack,"cdda-track=%d",iTrack);
	sprintf(cDev,"cdda:///dev/%s",sDrive.c_str());
	m = libvlc_media_new_location(m_pInst,cDev);
	libvlc_media_add_option(m,cTrack);
      }
    else if (sMediaURL.find("fifo:") != string::npos)
      {
	// Issue #2730
	// This is a local capture card requested by the Media Plugin, for now, assume that this is a 
	// MPEGx stream of some sort that the system can directly process. I will add V4L2 URL processing
	// once we can differentiate this via device data -TSCHAK

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL - Capture Card FIFO URL detected. translating.");
	sMediaURL = "file://" + sMediaURL.substr(7,sMediaURL.size());
	m = libvlc_media_new_location (m_pInst, sMediaURL.c_str());

      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL - Setting libvlc_media to new location %s",sMediaURL.c_str());
	m = libvlc_media_new_location (m_pInst, sMediaURL.c_str());
      }

    // Issue #2736
    // If Hardware Accelleration is set to something libvlc can handle (vdpau, vaapi), then turn it on.
    
    if (m_pVLC_Player->DATA_Get_Hardware_acceleration() == "vdpau")
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL() - Hardware Acceleration set to VDPAU.");
	libvlc_media_add_option(m, ":avcodec-hw=vdpau");
      }
    else if (m_pVLC_Player->DATA_Get_Hardware_acceleration() == "vaapi")
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL() - Hardware Acceleration set to VAAPI.");
	libvlc_media_add_option(m, ":avcodec-hw=vaapi");
      }
      
    m_pMp = libvlc_media_player_new_from_media(m);
    libvlc_media_player_set_xwindow (m_pMp, m_Window);

    if (!m)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::PlayURL() libvlc_media_new_location() returned NULL.");
	return false;
      }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL() - Stream ID set to %d",iStreamID);
    SetStreamID(iStreamID);

    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::playURL - Setting URL to %s",sMediaURL.c_str());
    SetMediaURL(sMediaURL);

    libvlc_media_player_set_media(m_pMp, m);
    
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::playURL - deploying event callbacks.");

    m_pMediaEventManager=libvlc_media_event_manager(m);
    m_pMediaPlayerEventManager=libvlc_media_player_event_manager(m_pMp);

    libvlc_event_attach(m_pMediaEventManager,libvlc_MediaParsedChanged,Media_Callbacks,this);
    libvlc_event_attach(m_pMediaEventManager,libvlc_MediaStateChanged,Media_Callbacks,this);
    libvlc_event_attach(m_pMediaPlayerEventManager,libvlc_MediaPlayerPlaying,Media_Callbacks,this);
    libvlc_event_attach(m_pMediaPlayerEventManager,libvlc_MediaPlayerVout,Media_Callbacks,this);
    // libvlc_event_attach(m_pMediaPlayerEventManager,libvlc_MediaPlayerStopped,Media_Callbacks,this);
    m_bEventsAttached=true;

    libvlc_media_release(m);
    libvlc_media_player_set_xwindow (m_pMp, m_Window);
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::playURL - Attached media to output window.");
    SetDuration(libvlc_media_player_get_length(m_pMp));
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::playURL - Attempting to play media.");
    if (libvlc_media_player_play(m_pMp)<0)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::PlayURL() - libvlc_media_player_play not successful.");
	SetPlaying(false);
	SetAudioVideoInfo();
	return false;
      }
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"media player is seekable? %d",libvlc_media_player_is_seekable(m_pMp));
    SetPlaying(true);

    // Parse Media Position, if available, and set state.
    if (!sMediaPosition.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PlayURL - Position provided. Parsing.");
	Position* pPosition = new Position(sMediaPosition);
	if (pPosition->getTitle()!=0)
	  {
	    SetTitle(pPosition->getTitle());
	  }
	if (pPosition->getChapter()!=0)
	  {
	    SetChapter(pPosition->getChapter());
	  }
	if (pPosition->getPosition()!=0)
	  {
	    SetTime(pPosition->getPosition());
	  }
	if (pPosition->getSubtitle()!=0)
	  {
	    SetSubtitle(pPosition->getSubtitle());
	  }
	if (pPosition->getAudio()!=0)
	  {
	    SetAudioTrack(pPosition->getAudio());
	  }
	delete pPosition;
      }

    return true;
  }

  void VLC::Stop()
  {
    if (!m_pMp)
      return;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Stop() - Clearing media, and stopping player.");
    SetPlaying(false);
    SetMediaURL("");
    libvlc_media_player_stop(m_pMp);
    
    if (m_bEventsAttached)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Stop() - Clearing extant media callbacks.");
	libvlc_event_detach(m_pMediaEventManager,libvlc_MediaParsedChanged,Media_Callbacks,this);
	libvlc_event_detach(m_pMediaEventManager,libvlc_MediaStateChanged,Media_Callbacks,this);
	libvlc_event_detach(m_pMediaPlayerEventManager,libvlc_MediaPlayerPlaying,Media_Callbacks,this);
	libvlc_event_detach(m_pMediaPlayerEventManager,libvlc_MediaPlayerVout,Media_Callbacks,this);
	m_bEventsAttached=false;
      }
    
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Stop() - Releasing media player.");
    libvlc_media_player_release(m_pMp);
    m_pMp=NULL;
    m_pMediaEventManager=NULL;
    m_pMediaPlayerEventManager=NULL;

  }

  void VLC::SetDuration(libvlc_time_t newDuration)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetDuration - Setting duration to %f",(float)newDuration/1000);
    m_fDuration = (float)newDuration / 1000;
  }

  float VLC::GetDuration()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetDuration - Get Duration(%f)",m_fDuration);
    return m_fDuration;
  }

  int64_t VLC::GetCurrentDuration()
  {
    if (!m_pMp)
      return -1;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetCurrentDuration(%lu)",libvlc_media_player_get_length(m_pMp));
    return libvlc_media_player_get_length(m_pMp);
  }

  float VLC::GetPosition()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GETPOSITION %f",m_fPosition);
    return m_fPosition;
  }

  void VLC::SetPosition(float fPosition)
  {
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::SetPosition(%f)",fPosition);
    m_fPosition=fPosition;
    libvlc_media_player_set_position(m_pMp, fPosition);
  }

  bool VLC::IsPlaying()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::IsPlaying? %d",m_bIsPlaying);
    return m_bIsPlaying;
  }

  void VLC::SetPlaying(bool bIsPlaying)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetPlaying %d",bIsPlaying);
    m_bIsPlaying = bIsPlaying;
  }

  void VLC::UpdateStatus()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::updatEStatus");
    m_pVLC_Player->ReportTimecodeViaIP(GetStreamID(),m_pVLC_Player->m_iMediaPlaybackSpeed);
  }

  void VLC::UpdateNav()
  {
    bool hasMenu = ((libvlc_media_player_get_chapter_count_for_title(m_pMp,0) > 0) && libvlc_media_player_get_title_count(m_pMp) > 1);
    bool isNavMenu = ((hasMenu) && (libvlc_media_player_get_title(m_pMp) == 0));
    
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"hasMenu %d isNavMenu %d",hasMenu,isNavMenu);
    
    if (isNavMenu && m_pVLC_Player->m_iPK_MediaType != 28)
      {
	m_pVLC_Player->EVENT_Menu_Onscreen(GetStreamID(),true);
      }
    else
      {
	m_pVLC_Player->EVENT_Menu_Onscreen(GetStreamID(),false);
      }
  }

  int64_t VLC::SetTime(int64_t iTime)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetTime - media time set to %lu",iTime);
    if (!IsPlaying())
      return -1;
    libvlc_media_player_set_time(m_pMp,iTime);
    return libvlc_media_player_get_time(m_pMp);
  }

  int64_t VLC::GetTime()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetTime() - Time %lu returned from libvlc",libvlc_media_player_get_time(m_pMp));
    if (!IsPlaying())
      return -1;
    return libvlc_media_player_get_time(m_pMp);
  }

  void VLC::Pause()
  {
    if (!m_pMp)
      return;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Pause() - Asked to pause.");

    // Got rid of audio track 0 check, as this is a valid entry for audio files! grrrr libvlc!
    if (m_iPreviousAudioTrack == -1 || GetVLCAudioTrack() == -1)
      {
	// Do not set previous audio track.
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Pause() - Not keeping previous audio track.");
      }
    else
      { 
	m_iPreviousAudioTrack=GetVLCAudioTrack();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Pause() - pre-pause audio track %d",m_iPreviousAudioTrack);
	cout << "Previous Audio Track: " << m_iPreviousAudioTrack << endl;
      }
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Pause() - Freeing Audio Channel.");
    SetAudioTrack(0); // Temporarily disable audio, free sound card for other uses.
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Pause() - Pausing media.");
    libvlc_media_player_set_pause(m_pMp, 1);
  }
  
  void VLC::Restart()
  {
    if (!m_pMp)
      return;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Restart() - Restoring audio track %d",m_iPreviousAudioTrack);
    cout << "Previous Audio Track: " << m_iPreviousAudioTrack << endl;

    libvlc_audio_set_track(m_pMp,m_iPreviousAudioTrack);
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::Restart() - Disabling pause.");
    libvlc_media_player_set_pause(m_pMp, 0);
  }

  void VLC::SetRate(float fMediaPlayBackSpeed)
  {
    if (m_pMp)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetRate(%f)",fMediaPlayBackSpeed);
	libvlc_media_player_set_rate(m_pMp, fMediaPlayBackSpeed);
      }
  }

  void VLC::JumpFwd(int iMult)
  {
    if (!m_pMp)
      return;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::JumpFwd(%i)",libvlc_media_player_get_time(m_pMp) + (m_fPosition * 1000) + (iMult * 30 * 1000));

    libvlc_media_player_set_time(m_pMp, libvlc_media_player_get_time(m_pMp) + (m_fPosition * 1000) + (iMult * 30 * 1000));    
  }
  
  void VLC::JumpBack(int iMult)
  {
    // libvlc_time_t iTime;
    
    if (!m_pMp)
      return;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::JumpBack(%i)",libvlc_media_player_get_time(m_pMp) + (m_fPosition * 1000) + (iMult * 30 * 1000));
    
    libvlc_media_player_set_time(m_pMp, libvlc_media_player_get_time(m_pMp) - (m_fPosition * 1000) - (iMult * 30 * 1000));
  }

  void VLC::MoveUp()
  {
    if (m_pMp)
      libvlc_media_player_navigate(m_pMp,libvlc_navigate_up);
  }
  
  void VLC::MoveDown()
  {
    if (m_pMp)
      libvlc_media_player_navigate(m_pMp,libvlc_navigate_down);
  }

  void VLC::MoveLeft()
  {
    if (m_pMp)
      libvlc_media_player_navigate(m_pMp,libvlc_navigate_left);
  }

  void VLC::MoveRight()
  {
    if (m_pMp)
      libvlc_media_player_navigate(m_pMp,libvlc_navigate_right);
  }

  void VLC::EnterGo()
  {
    if (m_pMp)
      libvlc_media_player_navigate(m_pMp,libvlc_navigate_activate);
  }

  void VLC::SetAudioVideoInfo()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetAudioVideo - clear values.");
    m_sAudioInfo="";
    m_sVideoInfo="";
  }

  void VLC::NextChapter()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::NextChapter");
    m_iChapter++;
    if (m_pMp)
      libvlc_media_player_next_chapter(m_pMp);
  }
  
  void VLC::PreviousChapter()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::PreviousChapter");
    m_iChapter--;
    if (m_pMp)
      libvlc_media_player_previous_chapter(m_pMp);
  }

  void VLC::GotoMediaMenu(int iMenu)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GotoMediaMenu");
    if (m_pMp)
      {
	libvlc_media_player_set_pause(m_pMp,0);
	libvlc_media_player_set_title(m_pMp,0);
	// This is broken until the new API is finished.
	//	libvlc_media_player_set_chapter(m_pMp,iMenu);
      }
  }

  void VLC::SetStreamID(int iStreamID)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetStreamID - Setting stream ID to %d",iStreamID);
    m_iStreamID=iStreamID;
  }

  int VLC::GetStreamID()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetStreamID - Stream ID is %d",m_iStreamID);
    return m_iStreamID;
  }

  bool VLC::GetScreenShot(int iWidth, int iHeight, string& sCMD_Result)
  {
    // size_t size;
    if (libvlc_video_take_snapshot(m_pMp,0,"/tmp/shot.png",iWidth,iHeight)==-1)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VLC::GetScreenShot(): Could not take snapshot.");
	sCMD_Result="ERROR";
	return false;
      }
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetScreenShot - success.");
    sCMD_Result="OK";
    return true;
  }

  bool VLC::hasChapters()
  {
    if (!m_pMp)
      return false;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::hasChapters() - %d",libvlc_media_player_get_chapter_count(m_pMp)>1);
    return libvlc_media_player_get_chapter_count(m_pMp)>1;
  }

  int VLC::numChapters()
  {
    if (!m_pMp)
      return false;
    
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::numChapters() - %d",libvlc_media_player_get_chapter_count(m_pMp));
    return libvlc_media_player_get_chapter_count(m_pMp);
  }


  int VLC::GetCurrentChapter()
  {
    if (!m_pMp)
      return false;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetCurrentChapter() - %d",m_iChapter);
    return m_iChapter;
    
    // This seems to be _very_ broken, so we will keep chapter state, ourselves.
    // return libvlc_media_player_get_chapter(m_pMp);
  }

  int VLC::GetCurrentTitle()
  {
    if (!m_pMp)
      return -1;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetCurrentTitle() - %d",libvlc_media_player_get_title(m_pMp));
    return libvlc_media_player_get_title(m_pMp);
  }

  void VLC::SetAudioTrack(int iAudioTrack)
  {
    if (!m_pMp)
      return;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetAudioTrack() - Setting audio track to %d",iAudioTrack);

    if (iAudioTrack == -1)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting audio track to previously set audio track. %d",m_iPreviousAudioTrack);
	libvlc_audio_set_track(m_pMp,m_iPreviousAudioTrack);
	return;
      }

    if (m_mapDgIndexToAudioTrackId.find(iAudioTrack) == m_mapDgIndexToAudioTrackId.end())
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::SetAudioTrack - Could not find matching datagrid row for audio track.");
	return;
      }
    LoggerWrapper::GetInstance()->Write(LV_WARNING,"Setting Audio track to: %d",m_mapDgIndexToAudioTrackId[iAudioTrack]);
    libvlc_audio_set_track(m_pMp,m_mapDgIndexToAudioTrackId[iAudioTrack]);
  }

  int VLC::GetVLCAudioTrack()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetVLCAudioTrack - Current libvlc audio track is %d",libvlc_audio_get_track(m_pMp));
    return libvlc_audio_get_track(m_pMp);
  }

  int VLC::GetAudioTrack()
  {
    map<int, int>::iterator it;
    
    if (!m_pMp)
      return -1;
    
    if (m_mapDgIndexToAudioTrackId.size()>0)
      {
	for (it=m_mapDgIndexToAudioTrackId.begin();it!=m_mapDgIndexToAudioTrackId.end();++it)
	  {
	    if (it->second == libvlc_audio_get_track(m_pMp))
	      {
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAudioTrack - Found datagrid rowf or audio track %d",libvlc_audio_get_track(m_pMp));
		return it->first;
	      }
	  }
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAudiotrack - Could not find matching audio track.");
	return -1;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAudioTrack - Empty datagrid.");
	return -1;
      }
    
  }

  void VLC::SetSubtitle(int iSubtitle)
  {
    if (!m_pMp)
      return;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetSubtitle() - Setting Subtitle to: %d",m_mapDgIndexToSubtitleId[iSubtitle]);

    if (m_mapDgIndexToSubtitleId.find(iSubtitle) == m_mapDgIndexToSubtitleId.end())
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::SetSubTitle() - Could not find matching datagrid row for subtitle.");
	return;
      }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetSubTitle() - Setting SPU to %d",m_mapDgIndexToSubtitleId[iSubtitle]);
    libvlc_video_set_spu(m_pMp,m_mapDgIndexToSubtitleId[iSubtitle]);
  }

  int VLC::GetSubtitle()
  {
    map<int, int>::iterator it;

    if (!m_pMp)
      return -1;

    if (m_mapDgIndexToSubtitleId.size()>0)
      {
	for (it=m_mapDgIndexToSubtitleId.begin();it!=m_mapDgIndexToSubtitleId.end();++it)
	  {
	    if (it->second == libvlc_video_get_spu(m_pMp))
	      {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::GetSubtitle - returning SPU %d",libvlc_video_get_spu(m_pMp));
		return it->first;
	      }
	  }
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::GetSubtitle - Could not find matching datagrid row for subtitle %d",libvlc_video_get_spu(m_pMp));	
	return -1;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::GetSubtitle - empty datagrid.");
	return -1;
      }

  }
  
  string VLC::GetAllSubtitles()
  {
    string sRet = "";
    int i=0;
    
    if (!m_pMp)
      return sRet;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllSubtitles() - Returning current subtitle %d",GetSubtitle());
    sRet += StringUtils::itos(GetSubtitle()) + "\n";

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllSubtitles() - Clearing subtitle datagrid row to SPU map.");
    m_mapDgIndexToSubtitleId.clear();

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllSubtitles() - traversing libvlc SPU list...");

    libvlc_track_description_t* first = libvlc_video_get_spu_description(m_pMp);
    libvlc_track_description_t* track = first;

    if (track == NULL)
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::GetAllSubtitles() - subtitle list is empty.");
	return sRet;
      }

    while (track)
      {
	string sTrackName = string(track->psz_name);
	if (sTrackName == "Disable")
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllSubtitles() - Found DISABLE entry, adding -1 entry.");
	    m_mapDgIndexToSubtitleId[-1]=track->i_id;
	    track = track->p_next;
	    // Do not add this to the datagrid, because Off is already added. Stupid, I know...backward compatibility. 
	  }
	else
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllSubtitles() - Adding subtitle ID %d track ID %d track name %s",i,track->i_id,sTrackName.c_str());
	    m_mapDgIndexToSubtitleId[i]=track->i_id;
	    sRet += sTrackName;
	    sRet += "\n";
	    track = track->p_next;
	    i++;
	  }
      }

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllSubtitles() - Cleaning up.");
    libvlc_track_description_list_release(first); // clean up. 
    track=NULL;
    first=NULL;

    return sRet;

  }

  string VLC::GetAllAudioTracks()
  {
    string sRet = "";
    int i=0;
    if (!m_pMp)
      return sRet;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllAudioTracks() - Returning current audio track %d",GetAudioTrack());
    sRet += StringUtils::itos(GetAudioTrack()) + "\n";

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllAudioTracks() - Clearing audio track datagrid row to SPU map.");
    m_mapDgIndexToAudioTrackId.clear();

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllAudioTracks() - traversing libvlc audio track list...");

    libvlc_track_description_t* track = libvlc_audio_get_track_description(m_pMp);
    libvlc_track_description_t* first = track;

    if (track == NULL)
      {
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"VLC::GetAllAudioTracks() - audio track list is empty.");
	return sRet;
      }

    while (track)
      {
	string sTrackName = string(track->psz_name);
	if (sTrackName == "Disable")
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllAudioTracks() - Found DISABLE entry, adding -1 entry.");
	    sTrackName = "Off";
	  }

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetAllAudioTracks() - Adding audio track ID %d track ID %d track name %s",i,track->i_id,sTrackName.c_str());
	m_mapDgIndexToAudioTrackId[i]=track->i_id;
	sRet += sTrackName;
	sRet += "\n";
	track = track->p_next;
	i++;
      }

    libvlc_track_description_list_release(first);
    first=NULL;
    track=NULL;

    return sRet;
  }

  string VLC::GetAllAngles()
  {
    return "0\nNone\n";
  }

  void VLC::SetAspectRatio(string sAspectRatio)
  {
    if (!m_pMp)
      return;
    if (sAspectRatio == "auto")
      libvlc_video_set_aspect_ratio(m_pMp,0);
    else
      libvlc_video_set_aspect_ratio(m_pMp,sAspectRatio.c_str());
  }

  void VLC::SetZoomFactor(string sZoomPercent)
  {
    unsigned px=0;
    unsigned py=0;
    int windowWidth=0;
    float fZoomFactor=0.0; 
    float fZoomScale=0.0;
    int iZoomPercent=0;
    int iDelta=0;
    XWindowAttributes windowattr;

    if (!m_pMp)
      return;

    if (libvlc_video_get_size(m_pMp,0,&px,&py) != 0)
      return;

    XLockDisplay(m_pDisplay);
    XGetWindowAttributes(m_pDisplay,m_Window,&windowattr);

    // Get the 100% zoom factor, which will be scaled.
    fZoomFactor = (float)windowattr.width / (float)px;
    
    if (sZoomPercent == "100")
      {
	// Special case for 100%, use 0 to fill drawable.
	XUnlockDisplay(m_pDisplay);
	libvlc_video_set_scale(m_pMp,0);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetZoomFactor - Special case for 100%, setting to 0 to fill drawable.");
	m_iZoomPercent=100;
	return;
      }
    else if (sZoomPercent[0] == '+')
      {
	// Zoom in X percent
	iDelta=atoi(sZoomPercent.substr(1).c_str());
	iZoomPercent=m_iZoomPercent+iDelta;
      }
    else if (sZoomPercent[0] == '-')
      {
	// Zoom out X percent
	iDelta=atoi(sZoomPercent.substr(1).c_str());
	iZoomPercent=m_iZoomPercent-iDelta;
      }
    else
      {
	// Absolute percentage.
	iZoomPercent = atoi(sZoomPercent.c_str());
      }
    
    XSync(m_pDisplay,false);
    XUnlockDisplay(m_pDisplay);

    // Calculate scale, and apply it.
    fZoomScale = fZoomFactor * ((float)iZoomPercent / (float)100.0);
    libvlc_video_set_scale(m_pMp,fZoomScale);

    m_iZoomPercent=iZoomPercent;
    
    return;
  }

  void VLC::SetMediaType(string sMediaType, int iMediaID)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetMediaType - Media Type %s Media ID %d",sMediaType.c_str(),iMediaID);
    m_sMediaType=sMediaType;
    m_iMediaID=iMediaID;
  }

  string VLC::GetMediaType()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetMediaType - Media type is %s",m_sMediaType.c_str());
    return m_sMediaType;
  }

  int VLC::GetMediaID()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::GetMediaID - Media ID %i",m_iMediaID);
    return m_iMediaID;
  }

  void VLC::SetChapter(int iChapter)
  {
    if (!m_pMp)
      return;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetChapter - Setting chapter to %d",iChapter);
    m_iChapter=iChapter;
    libvlc_media_player_set_chapter(m_pMp,iChapter);
  }

  void VLC::SetTitle(int iTitle)
  {
    if (!m_pMp)
      return;
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"VLC::SetTitle - Setting title to %d",iTitle);
    libvlc_media_player_set_title(m_pMp,iTitle);
  }

  bool VLC::GetSize(unsigned& iWidth, unsigned& iHeight)
  {
    unsigned tiWidth, tiHeight;
    tiWidth=tiHeight=0;
    iWidth=iHeight=0;

    if (!m_pMp)
      return false;
    
    if (!libvlc_video_get_size(m_pMp,0,&tiWidth,&tiHeight))
      {
	return false;
      }
    else
      {
	iWidth=tiWidth;
	iHeight=tiHeight;
      }
    return true;
  }

  string VLC::GetMediaInfo()
  {
    if (!m_pMp)
      return "";

    m_sMediaInfo = "";
    for (int i=1; i<=libvlc_media_player_get_title_count(m_pMp); i++)
      {
	if (libvlc_media_player_get_chapter_count_for_title(m_pMp,i) == 0)
	  {
	    m_sMediaInfo += "Title " + StringUtils::itos(i) + "\t\t" + StringUtils::itos(i) + "\n";
	  }
	else
	  {
	    for (int j=1; j<=libvlc_media_player_get_chapter_count(m_pMp); j++)
	      {
		m_sMediaInfo += "Title " + StringUtils::itos(i) + " Chapter " + StringUtils::itos(j) + "\t" + StringUtils::itos(j) + "\t" + StringUtils::itos(i) + "\n";
	      }
	  }
      }
    return m_sMediaInfo;
  }

  void VLC::ReportPlaybackStarted()
  {
    if (!m_pMp)
      return;

    m_pVLC_Player->EVENT_Playback_Started(m_sMediaURL,m_iStreamID,GetMediaInfo(),"","");
  }

  string VLC::PreProcessMediaURL(string sMediaURL)
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PreProcessMediaUrl in %s",sMediaURL.c_str());
    if (m_pVLC_Player->m_iPK_MediaType == 28 && sMediaURL.find("/dev/") != string::npos)
	{
		sMediaURL = "bluray://" + sMediaURL;	
	}
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PreProcessMediaUrl out %s",sMediaURL.c_str());
    return sMediaURL;
  }

  void VLC::OSD_Status(string sStatusString)
  {
    if (!m_pMp)
      return;

    libvlc_video_set_marquee_string(m_pMp,1,sStatusString.c_str());
    libvlc_video_set_marquee_int(m_pMp,6,36);
    libvlc_video_set_marquee_int(m_pMp,3,255); //opacity
    libvlc_video_set_marquee_int(m_pMp,2,0x00FFFFFF); //color
    libvlc_video_set_marquee_int(m_pMp,7,5000); //timeout
    libvlc_video_set_marquee_int(m_pMp,8,100);  //x-coordinate
    libvlc_video_set_marquee_int(m_pMp,9,100);  //y-coordinate
    libvlc_video_set_marquee_int(m_pMp,0,1);

  }

  string VLC::OSD_Time(int64_t time)
  {
    int seconds = (int) (time / 1000) % 60 ;
    int minutes = (int) ((time / (1000*60)) % 60);
    int hours   = (int) ((time / (1000*60*60)) % 24);
    char output[64];

    sprintf(output,"%02d:%02d:%02d",hours,minutes,seconds);
    return string(output);

  }

}
