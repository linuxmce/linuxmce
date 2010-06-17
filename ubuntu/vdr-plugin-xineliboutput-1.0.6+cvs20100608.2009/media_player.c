/*
 * media_player.c: 
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: media_player.c,v 1.74 2010/02/23 12:18:47 phintuka Exp $
 *
 */

#include <unistd.h>

#include <vdr/config.h>
#include <vdr/status.h>
#include <vdr/interface.h>
#include <vdr/tools.h>

#include "config.h"
#include "media_player.h"
#include "device.h"
#include "tools/playlist.h"
#include "tools/metainfo_menu.h"
#include "menu.h"

#include "logdefs.h"

static void BackToMenu(void)
{
  cRemote::CallPlugin("xineliboutput");
}


//
// cXinelibPlayer
//

class cXinelibPlayer : public cPlayer 
{
  private:
    cString m_File;
    cString m_ResumeFile;
    cString m_SubFile;

    cPlaylist m_Playlist;

    bool m_Error;
    bool m_UseResumeFile;
    int  m_Speed;

    void UpdateNumTracks(void);

    cXinelibDevice *m_Dev;

  protected:
    virtual void Activate(bool On);

  public:
    cXinelibPlayer(cXinelibDevice *Dev, const char *File, bool Queue = false, const char *SubFile = NULL);
    virtual ~cXinelibPlayer();

    // cPlayer
    virtual void SetAudioTrack(eTrackType Type, const tTrackId *TrackId);
    virtual void SetSubtitleTrack(eTrackType Type, const tTrackId *TrackId);
    virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame = false);
    virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);

    // cXinelibPlayer
    void Control(const char *s) { (void)m_Dev->PlayFileCtrl(s); }
    void Control(const char *s, int i) {
      cString cmd = cString::sprintf(s, i);
      Control(cmd);
    }
    void SetSpeed(int Speed);
    int  Speed(void) { return m_Speed; };

    int  GetPos(void)    { return m_Dev->PlayFileCtrl("GETPOS");    }
    int  GetLength(void) { return m_Dev->PlayFileCtrl("GETLENGTH"); }
    const char *GetMetaInfo(eMetainfoType mi) { return m_Dev->GetMetaInfo(mi); }

    bool NextFile(int step);
    bool Playing(void) { return !(m_Error || m_Dev->EndOfStreamReached()); }
    bool Error(void)   { return m_Error; }
    void UseResumeFile(bool Val) { m_UseResumeFile = Val; }

    /* Playlist access */
    cPlaylist& Playlist(void) { return m_Playlist; }
    const cString& File(void) { return m_File; }
    int  CurrentFile(void) { return m_Playlist.Current()->Index(); } 
    int  Files(void) { return m_Playlist.Count(); }
};

cXinelibPlayer::cXinelibPlayer(cXinelibDevice *Dev, const char *File, bool Queue, const char *SubFile)
{
  m_ResumeFile = NULL;
  m_UseResumeFile = true;
  m_Error = false;
  m_Speed = 1;
  m_Dev = Dev;

  if(File) {
    size_t len = strlen(File);
    if(len && File[len-1] == '/') { 
      // whole directory, create temporary playlist
      m_Playlist.Read(File, true);
      m_Playlist.Sort();
    } else if(xc.IsPlaylistFile(File)) {
      m_Playlist.Read(File);
    } else {
      // a single file but not a playlist file, create playlist with only one item
      m_Playlist.Read(File);
    }

    if(m_Playlist.Count() < 1)
      LOGMSG("cXinelibPlayer: nothing to play !");

    if(m_Playlist.Count() > 0)
      m_Playlist.StartScanner();

    m_File = m_Playlist.Current()->Filename;
    m_SubFile = SubFile;
  }
}

cXinelibPlayer::~cXinelibPlayer()
{
  Activate(false);
  Detach();
}

void cXinelibPlayer::SetAudioTrack(eTrackType Type, const tTrackId *TrackId)
{
  if(IS_DOLBY_TRACK(Type))
    Control("AUDIOSTREAM AC3 %d", (int)(Type - ttDolbyFirst));
  if(IS_AUDIO_TRACK(Type))
    Control("AUDIOSTREAM AC3 %d", (int)(Type - ttAudioFirst));
}

void cXinelibPlayer::SetSubtitleTrack(eTrackType Type, const tTrackId *TrackId)
{
  m_Dev->SetSubtitleTrackDevice(Type);
}

bool cXinelibPlayer::GetIndex(int &Current, int &Total, bool SnapToIFrame) 
{ 
  // Returns the current and total frame index, optionally snapped to the
  // nearest I-frame.
  int msCurrent = GetPos();
  int msTotal   = GetLength();
  if(msCurrent>=0 && msTotal>=0) {
    Current = msCurrent * 25 / 1000;
    Total = msTotal * 25 / 1000;
    return true;
  }
  return false; 
}

bool cXinelibPlayer::GetReplayMode(bool &Play, bool &Forward, int &Speed) 
{
  // Returns the current replay mode (if applicable).
  // 'Play' tells whether we are playing or pausing, 'Forward' tells whether
  // we are going forward or backward and 'Speed' is -1 if this is normal
  // play/pause mode, 0 if it is single speed fast/slow forward/back mode
  // and >0 if this is multi speed mode.
  Play = (m_Speed>0);
  Forward = true;
  Speed = abs(m_Speed) - 2;
  if(Speed<-1) Speed=-1;

  return true; 
}

void cXinelibPlayer::SetSpeed(int Speed)
{
  m_Speed = Speed;
  switch(Speed) {
    case -4: Control("TRICKSPEED 8");   break;
    case -3: Control("TRICKSPEED 4");   break;
    case -2: Control("TRICKSPEED 2");   break;
    case  0: Control("TRICKSPEED 0");   break;
    default: m_Speed = 1;
    case  1: Control("TRICKSPEED 1");   break;
    case  2: Control("TRICKSPEED -2");  break;
    case  3: Control("TRICKSPEED -4");  break;
    case  4: Control("TRICKSPEED -12"); break;
  }
}

bool cXinelibPlayer::NextFile(int step)
{
  if (m_Playlist.Count() > 0) {
    for (;step < 0; step++)
      m_Playlist.Prev();
    for (;step > 0; step--)
      m_Playlist.Next();

    if (!m_Playlist.Current())
      LOGERR("!m_Playlist.Get(m_CurrInd)");
    m_File = *m_Playlist.Current()->Filename;
    m_ResumeFile = NULL;
    m_SubFile = NULL;

    Activate(true);
    return !m_Error;
  }

  return false;
}

void cXinelibPlayer::UpdateNumTracks(void)
{
  // cdda tracks
  if(m_Playlist.Count() == 1 && !strcmp("cdda:/", m_Playlist.First()->Filename)) {
    int count = m_Dev->PlayFileCtrl("GETAUTOPLAYSIZE CD", 10000);
    if(count>0) {
      for(int i=0; i<count; i++)
        m_Playlist.Read(cString::sprintf("cdda:/%d", i+1));
      m_Playlist.Del(m_Playlist.First());
    }
  }
}

void cXinelibPlayer::Activate(bool On)
{
  int pos = 0, len = 0, fd = -1;
  if(On) {
    if(m_UseResumeFile && !*m_ResumeFile)
      m_ResumeFile = cString::sprintf("%s.resume", *m_File);
    if(m_UseResumeFile && 0 <= (fd = open(m_ResumeFile, O_RDONLY))) {
      if(read(fd, &pos, sizeof(int)) != sizeof(int))
	pos = 0;
      close(fd);
    }
    // escape raw file names and join subtitle file to the mrl
    cString mrl = m_File[0] == '/' ? cPlaylist::BuildMrl("file", m_File) : m_File;
    if (*m_SubFile)
      mrl = cString::sprintf("%s#subtitle:%s", *mrl,
                             m_SubFile[0] == '/' ? *cPlaylist::BuildMrl("file", m_SubFile) : *m_SubFile);

    // Start replay
    UpdateNumTracks();
    m_Error = !m_Dev->PlayFile(mrl, pos);
    LOGDBG("cXinelibPlayer playing %s (%s)", *m_File, m_Error ? "FAIL" : "OK");

    if(!m_Error) {
      // update playlist metainfo
      const char *ti = GetMetaInfo(miTitle);
      const char *tr = GetMetaInfo(miTracknumber);
      const char *al = GetMetaInfo(miAlbum);
      const char *ar = GetMetaInfo(miArtist);
      if(ti && ti[0] && (!*m_Playlist.Current()->Title || !strstr(m_Playlist.Current()->Title, ti)))
	m_Playlist.Current()->Title = ti;
      if(tr && tr[0])
        m_Playlist.Current()->Tracknumber = tr;
      if(al && al[0])
	m_Playlist.Current()->Album = al;
      if(ar && ar[0])
	m_Playlist.Current()->Artist = ar;

      UpdateNumTracks();
    }
  } else {
    if(m_UseResumeFile && *m_ResumeFile) {
      pos = GetPos();
      len = GetLength();
      if(pos>10000 && pos < (len-10000)) {
	pos = (pos/1000) - 10; // skip back 10 seconds ("VDR style")
	if(0 <= (fd = open(m_ResumeFile, O_WRONLY | O_CREAT, 
			   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))) {
	  if(write(fd, &pos, sizeof(int)) != sizeof(int)) {
	    Skins.QueueMessage(mtInfo, "Error writing resume position !", 5, 30);
	  }
	  close(fd);
	} else {
	  Skins.QueueMessage(mtInfo, "Error creating resume file !", 5, 30);
	}
      } else {
	unlink(m_ResumeFile);
      }
      m_ResumeFile = NULL;
    }
    m_Dev->PlayFile(NULL);
    m_Error = false;
  }
}

//
// cPlaylistMenu
//


class cPlaylistMenu : public cOsdMenu, cPlaylistChangeNotify
{
  protected:

    cPlaylist& m_Playlist;
    bool       m_NeedsUpdate;
    bool&      m_RandomPlay;
    cCharSetConv m_IC;

  public:

    cPlaylistMenu(cPlaylist &Playlist, bool& RandomPlay);
    virtual ~cPlaylistMenu();

    void Set(bool setCurrentPlaying = false);
    void SetCurrentExt(int i);
    void SetHelpButtons(void);

    // cOsdMenu
    virtual eOSState ProcessKey(eKeys Key);

    // cPlaylistChangeNotify
    virtual void PlaylistChanged(const cPlaylistItem *item);
};

cPlaylistMenu::cPlaylistMenu(cPlaylist &Playlist, bool& RandomPlay) : 
     cOsdMenu(tr("Playlist")),
     m_Playlist(Playlist),
     m_RandomPlay(RandomPlay),
     m_IC("UTF-8", cCharSetConv::SystemCharacterTable())
{
  SetTitle(cString::sprintf("%s: %s", tr("Playlist"), m_IC.Convert(*Playlist.Name())));
  Playlist.Listen(this);
  Set(true);
}

cPlaylistMenu::~cPlaylistMenu()
{
  m_Playlist.Listen(NULL);
}

void cPlaylistMenu::PlaylistChanged(const cPlaylistItem *item)
{
  m_NeedsUpdate = true; 
}

eOSState cPlaylistMenu::ProcessKey(eKeys Key) 
{
  bool hadSubMenu = HasSubMenu();

  if(m_NeedsUpdate)
    Set();

  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state == osUnknown) {
    switch(Key) {
      case kBack:   
                    return osEnd;
      case kRed:    
                    m_RandomPlay = !m_RandomPlay;
	            SetHelpButtons();
	            return osContinue;
      case kGreen:  
                    return AddSubMenu(cMenuXinelib::CreateMenuBrowseFiles(ShowMusic));
      case kYellow: if(m_Playlist.Count() > 1) {
	              eOSState result = osContinue;
	              cPlaylistItem *i = m_Playlist.Current();
		      if(i->Index() == Current()) {
			if(i->Next())
			  result = (eOSState)(os_User + i->Index()); /* forces jump to next item */
			else
			  result = (eOSState)(os_User + i->Index() - 1);/* forces jump to last item */
		      }
		      for(i = m_Playlist.First(); i && i->Index() != Current(); i = m_Playlist.Next(i));
		      if(i)
			m_Playlist.Del(i);
		      if(Current() == Count()-1)
			SetCurrent(Get(Current()-1));
		      Set();
	              return result;
                    }
      case kBlue:   
                    m_Playlist.Sort();
		    Set();
	            return osContinue;
      default: break;
    }
  }

  if(hadSubMenu && !HasSubMenu())
     Set();

  return state;
}

void cPlaylistMenu::SetCurrentExt(int i) 
{
  SetCurrent(Get(i));
  Set();
}

void cPlaylistMenu::SetHelpButtons(void)
{
  SetHelp(!m_RandomPlay ? tr("Button$Random") : tr("Button$Normal"),
	  tr("Button$Add files"),
	  m_Playlist.Count()>1 ? tr("Button$Remove") : NULL,
	  tr("Button$Sort"));
  Display();
}

void cPlaylistMenu::Set(bool setCurrentPlaying)
{
  m_NeedsUpdate = false;

  int currentItem = Current();
  Clear();
  SetHasHotkeys();
  SetCols(2, 30);
  SetHelpButtons();

  int currentPlaying = m_Playlist.Current()->Index();
  int j = 0;

  for(cPlaylistItem *i = m_Playlist.First(); i; i = m_Playlist.Next(i), j++) {
    cString Title = cPlaylist::GetEntry(i, true, j==currentPlaying);
    Add(new cOsdItem(m_IC.Convert(*Title), (eOSState)(os_User + j)));
  }

  if(setCurrentPlaying)
    SetCurrent(Get(currentPlaying));
  else
    SetCurrent(Get(currentItem));
  
  Display();
}


//
// cXinelibPlayerControl
//

#include <vdr/skins.h>

cXinelibPlayer *cXinelibPlayerControl::m_Player = NULL;
cMutex cXinelibPlayerControl::m_Lock;

cXinelibPlayerControl::cXinelibPlayerControl(eMainMenuMode Mode, const char *File, const char *SubFile) :
  cControl(OpenPlayer(File, false, SubFile))
{
  m_DisplayReplay = NULL;
  m_PlaylistMenu = NULL;
  m_ShowModeOnly = true;
  m_Mode = Mode;
  m_RandomPlay = false;
  m_AutoShowStart = time(NULL);
  m_BlinkState = true;

  number = 0;
  lastTime.Set();

  m_Player->UseResumeFile( (Mode==ShowFiles) );

  MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
}

cXinelibPlayerControl::~cXinelibPlayerControl()
{
  CloseMenus();

  MsgReplaying(NULL, NULL);
  Close();
}

void cXinelibPlayerControl::CloseMenus(void)
{
  if (m_PlaylistMenu) {
    delete m_PlaylistMenu;
    m_PlaylistMenu = NULL;
  }
  if (m_DisplayReplay) {
    delete m_DisplayReplay;
    m_DisplayReplay = NULL;
  }
}

void cXinelibPlayerControl::MsgReplaying(const char *Title, const char *File)
{
  cStatus::MsgReplaying(this, NULL, NULL, false);
  if(Title || File)
    cStatus::MsgReplaying(this, Title, File, true);
}

void cXinelibPlayerControl::Queue(const char *File)
{
  if(!File)
    return;

  m_Lock.Lock();

  LOGMSG("cXinelibPlayerControl::Queue(%s)", File);

  if(!m_Player) {
    OpenPlayer(File, true);
    cControl::Launch(new cXinelibPlayerControl(ShowMusic, NULL));
  } else {
    size_t len = strlen(File);
    if(len && File[len-1] == '/')
      m_Player->Playlist().Read(File, true);
    else
      m_Player->Playlist().Read(File);
  }

  Skins.Message(mtInfo, tr("Queued to playlist"));

  m_Lock.Unlock();

  if(m_Player->Playlist().Count() > 0)
    m_Player->Playlist().StartScanner();

}

cXinelibPlayer *cXinelibPlayerControl::OpenPlayer(const char *File, bool Queue, const char *SubFile)
{
  m_Lock.Lock();
  if(!m_Player)
    m_Player = new cXinelibPlayer(&(cXinelibDevice::Instance()), File, Queue, SubFile);
  m_Lock.Unlock();
  return m_Player;
}

void cXinelibPlayerControl::Close(void)
{
  m_Lock.Lock();
  if(m_Player)
    delete m_Player;
  m_Player = NULL;
  m_Lock.Unlock();
}

void cXinelibPlayerControl::Show()
{
  bool Play    = (m_Player->Speed() > 0);
  bool Forward = true;
  int  Speed   = abs(m_Player->Speed()) - 2;
  if(Speed<-1) Speed=-1;

  if(!m_DisplayReplay) {
    if(cOsd::IsOpen())
      return;
    m_DisplayReplay = Skins.Current()->DisplayReplay(m_ShowModeOnly);
  }

  if(!m_ShowModeOnly) {
    char t[128] = "";
    int  Current = m_Player->GetPos();
    int  Total   = m_Player->GetLength();
    if(Current>=0) m_CurrentPos = Current;
    if(Total>=0) m_CurrentLen = Total;

    if(m_CurrentLen >= 0 /*&& Total >= 0*/) {
      Total = (m_CurrentLen + 500) / 1000;   // ms --> s
      Current = (m_CurrentPos + 500) / 1000;

      cString Title = cPlaylist::GetEntry(m_Player->Playlist().Current());
      cCharSetConv ic("UTF-8", cCharSetConv::SystemCharacterTable());
      m_DisplayReplay->SetTitle(ic.Convert(*Title));

      m_DisplayReplay->SetProgress(Current, Total);
      sprintf(t, "%d:%02d:%02d", Total/3600, (Total%3600)/60, Total%60);
      m_DisplayReplay->SetTotal( t );
      sprintf(t, "%d:%02d:%02d", Current/3600, (Current%3600)/60, Current%60);
      m_BlinkState = (m_Player->Speed() != 0) || (!m_BlinkState);
      m_DisplayReplay->SetCurrent( m_BlinkState ? t : "");
    }
  }

  m_DisplayReplay->SetMode(Play, Forward, Speed);

  m_DisplayReplay->Flush();
}

void cXinelibPlayerControl::Hide()
{
  CloseMenus();
}

cOsdObject *cXinelibPlayerControl::GetInfo(void)
{
  return new cMetainfoMenu(m_Player->Playlist().Current()->Filename);
}

eOSState cXinelibPlayerControl::ProcessKey(eKeys Key)
{
  if ( !m_Player->Playing() ) {
    LOGDBG("cXinelibPlayerControl: EndOfStreamReached");
    if (m_Mode == ShowMusic && m_Player->Files() == 1 && !m_Player->Error()) {
      m_Player->NextFile(0);
      return osContinue;
    }
    int Jump = 1;
    if(m_RandomPlay) {
      srand((unsigned int)time(NULL));
      Jump = (random() % m_Player->Files()) - m_Player->CurrentFile();
    } 
    if(m_Player->Files() < 2 || !m_Player->NextFile(Jump)) {
      Hide();
      return osEnd;
    }
    if(m_PlaylistMenu) {
      m_PlaylistMenu->PlaylistChanged(m_Player->Playlist().Current());
      m_PlaylistMenu->SetCurrentExt(m_Player->CurrentFile());
    }

    if(!m_DisplayReplay)
      m_AutoShowStart = time(NULL);

    MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
  }

  else {
    // metainfo may change during playback (DVD titles, CDDA tracks)
    const char *ti = m_Player->GetMetaInfo(miTitle);
    if(ti && ti[0] && (!*m_Player->Playlist().Current()->Title ||
		       !strstr(m_Player->Playlist().Current()->Title, ti))) {
      const char *tr = m_Player->GetMetaInfo(miTracknumber);
      const char *al = m_Player->GetMetaInfo(miAlbum);
      const char *ar = m_Player->GetMetaInfo(miArtist);
      LOGDBG("metainfo changed: %s->%s %s->%s %s->%s %s->%s",
	     *m_Player->Playlist().Current()->Artist?:"-", ar?:"-", 
	     *m_Player->Playlist().Current()->Album ?:"-", al?:"-", 
	     *m_Player->Playlist().Current()->Tracknumber ?:"-", tr?:"-",
             *m_Player->Playlist().Current()->Title ?:"-", ti?:"-");
      m_Player->Playlist().Current()->Title = ti;
      if(tr && tr[0])
        m_Player->Playlist().Current()->Tracknumber = tr;
      if(al && al[0])
	m_Player->Playlist().Current()->Album = al;
      if(ar && ar[0])
	m_Player->Playlist().Current()->Artist = ar;
      MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
    }
  }

  // playlist menu
  if (m_PlaylistMenu) {
    m_AutoShowStart = 0;

    eOSState state = osUnknown;

    switch (state = m_PlaylistMenu->ProcessKey(Key)) {
      case osBack:
      case osEnd:   Hide(); break;
      default:      if (state >= os_User) {
                      m_Player->NextFile( (int)state - (int)os_User - m_Player->CurrentFile());
                      m_PlaylistMenu->SetCurrentExt(m_Player->CurrentFile());
                      MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
                    }
	            break;
    }

    if (state != osUnknown)
      return osContinue;
  }

  if (m_DisplayReplay)
    Show();

  if ( m_Mode == ShowFiles ) {
    switch(Key) {
      // replay menu
      case kRed:    if(m_Player->Playlist().Count() > 1) {
                      Hide();
                      m_PlaylistMenu = new cPlaylistMenu(m_Player->Playlist(), m_RandomPlay);
                      m_AutoShowStart = 0;
                    } else {
                      m_Player->Control("SEEK 0");    break;
                    }
                    break;
      case kUser8:
      case k1:      m_Player->Control("SEEK -20");  break;
      case kUser9:
      case k3:      m_Player->Control("SEEK +20");  break;
      case k2:      xc.subtitle_vpos -= 10;
      case k5:      xc.subtitle_vpos += 5;
                    m_Player->Control("SUBTITLES %d", xc.subtitle_vpos);
                    break;
      case kRight:
                    {
                      static const int speeds[] = { -3, -2, 1, 2, -4, 2, 3, 4, 4 };
                      m_Player->SetSpeed(speeds[m_Player->Speed() + 4]);
                      if(m_Player->Speed() != 1)
                        Show();
                      else
                        Hide();
                      break;
                    }
      case kLeft:
                    {
                      static const int speeds[] = { 0, -4, -3, -2, 0, -2, 1, 2, 3 };
                      m_Player->SetSpeed(speeds[m_Player->Speed() + 4]);
                      if(m_Player->Speed() != 1 || !m_ShowModeOnly)
                        Show();
                      else
                        Hide();
                      break;
                    }
      default:      break;
    }
  }

  if ( m_Mode == ShowMusic ) {
    switch(Key) {
      case kRed:    Hide();
                    m_PlaylistMenu = new cPlaylistMenu(m_Player->Playlist(), m_RandomPlay);
                    m_AutoShowStart = 0;
                    break;
      case kNext:
      case kRight:  if(m_RandomPlay) {
                      srand((unsigned int)time(NULL));
                      m_Player->NextFile((random() % m_Player->Files()) - m_Player->CurrentFile());
                    }
                    else {
                      m_Player->NextFile(1);
                    }
                    if(!m_DisplayReplay)
                      m_AutoShowStart = time(NULL);
                    MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
                    break;
      case kPrev:
      case kLeft:   if(m_Player->GetPos() < 3000) {
                      m_Player->NextFile(-1);
                      if(!m_DisplayReplay)
                        m_AutoShowStart = time(NULL);
                      MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
                    }
                    else {
                      m_Player->NextFile(0);
                      if(!m_DisplayReplay)
                        m_AutoShowStart = time(NULL);
                    }
                    break;
      case k0 ... k9:
                    if (number >= 0) {
                       if (number * 10 + Key - k0 > m_Player->Files())
                          number = m_Player->Files();
                       else
                          number = number * 10 + Key - k0;
                    }
                    break;
      case kNone:
                    if (number > 0 && int(lastTime.Elapsed()) > 3000) {
                       m_Player->NextFile( number - (m_Player->CurrentFile() + 1) );
                       if (!m_DisplayReplay)
                          m_AutoShowStart = time(NULL);
                       MsgReplaying(*m_Player->Playlist().Current()->Title, *m_Player->File());
                       number = 0;
                       lastTime.Set();
                    }
                    break;
      default:      break;
    }
  }

  switch(Key) { // key bindings common for both players
    case kBack:   xc.main_menu_mode = m_Mode;
                  Hide(); 
		  BackToMenu();
                  break;
    case kStop:
    case kBlue:   Hide();
                  Close();
                  return osEnd;
    case kUser7:  if(m_Player->Playlist().Count()>1) {
                    m_RandomPlay = !m_RandomPlay;
		    if(m_RandomPlay)
		      Skins.Message(mtInfo, tr("Random play"));
		    else
		      Skins.Message(mtInfo, tr("Normal play"));
                  }
                  break;
    case kGreen:  m_Player->Control("SEEK -60");  break;
    case kYellow: m_Player->Control("SEEK +60");  break;
    case kUser8:  m_Player->Control("SEEK -20");  break;
    case kUser9:  m_Player->Control("SEEK +20");  break;
    case kDown:
    case kPause:  if(m_Player->Speed()) {
                    m_Player->SetSpeed(0);
		    if(!m_DisplayReplay)
		      m_ShowModeOnly = true;
		    Show();
		    break;
                  }
                  // fall thru
    case kUp:
    case kPlay:   m_Player->SetSpeed(1);
                  if(m_ShowModeOnly && m_DisplayReplay)
		    Hide();
		  else if(m_DisplayReplay)
		    Show();
		  m_ShowModeOnly = false;
                  break;
    case kFastFwd:
                  {
                    static const int speeds[] = { -3, -2, 1, 2, -4, 2, 3, 4, 4 };
		    m_Player->SetSpeed(speeds[m_Player->Speed() + 4]);
                    if(m_Player->Speed() != 1)
                      Show();
                    else 
                      Hide();
                    break;
		  }
    case kFastRew:
                  {
                    static const int speeds[] = { 0, -4, -3, -2, 0, -2, 1, 2, 3 };
		    m_Player->SetSpeed(speeds[m_Player->Speed() + 4]);
                    if(m_Player->Speed() != 1 || !m_ShowModeOnly)
                      Show();
                    else 
                      Hide();
                    break;
		  }
    case kOk:     
                  m_AutoShowStart = 0;
                  if(m_Player->Speed() != 1) {
		    Hide();
		    m_ShowModeOnly = !m_ShowModeOnly;
		    Show();
		  } else {
		    if(m_DisplayReplay) {
		      m_ShowModeOnly = true;
		      Hide();
		    } else {
		      Hide();
		      m_ShowModeOnly = false;
		      Show();
		    }
                  }
                  break;
    default:      break;
  }

  if(m_DisplayReplay && 
     m_AutoShowStart &&
     time(NULL) - m_AutoShowStart > 5) {
    m_AutoShowStart = 0;
    Hide();
  }

  if(!m_DisplayReplay && 
     m_AutoShowStart) {
    m_ShowModeOnly = false;
    Show();
  }

  return osContinue;
}

//
// cDvdMenu
//

class cDvdMenu : public cOsdMenu {
  private:
    cXinelibPlayer *m_Player;

  public:
    cDvdMenu(cXinelibPlayer *Player);
};

cDvdMenu::cDvdMenu(cXinelibPlayer *Player) : cOsdMenu(tr("DVD Menu")), m_Player(Player)
{
  Add(new cOsdItem(tr("Exit DVD menu"),  osUser1));
  Add(new cOsdItem(tr("DVD Root menu"),  osUser2));
  Add(new cOsdItem(tr("DVD Title menu"), osUser3));
  Add(new cOsdItem(tr("DVD SPU menu"),   osUser4));
  Add(new cOsdItem(tr("DVD Audio menu"), osUser5));
  Add(new cOsdItem(tr("Close menu"),     osEnd));
  Display();
}

//
// cXinelibDvdPlayerControl
//

cXinelibDvdPlayerControl::~cXinelibDvdPlayerControl() 
{
  CloseDvdMenu();
}

void cXinelibDvdPlayerControl::Hide(void)
{
  CloseDvdMenu();
  cXinelibPlayerControl::Hide();
}

void cXinelibDvdPlayerControl::CloseDvdMenu(void)
{
  if (m_DvdMenu) {
    delete m_DvdMenu;
    m_DvdMenu = NULL;
  }
}

void cXinelibDvdPlayerControl::Show(void)
{
  if (!m_DvdMenu)
    cXinelibPlayerControl::Show();
  else
    cXinelibPlayerControl::Hide();
}

eOSState cXinelibDvdPlayerControl::ProcessKey(eKeys Key)
{
  // Check for end of stream and failed open
  if ( !m_Player->Playing() ) {
    LOGDBG("cXinelibDvdPlayerControl: EndOfStreamReached");
    Hide();
    return osEnd;
  }

  // Update DVD title information
  const char *ti = m_Player->GetMetaInfo(miTitle);
  if (ti && ti[0] && (!m_CurrentDVDTitle || !strstr(m_CurrentDVDTitle, ti))) {
    memset(m_CurrentDVDTitle, 0, 63);
    strn0cpy(m_CurrentDVDTitle, ti, 63);
    m_Player->Playlist().Current()->Title = m_CurrentDVDTitle;
    MsgReplaying(m_CurrentDVDTitle, NULL);
  }

  // Handle menu selection
  if (m_DvdMenu) {
    if (Key == kRed)
      Hide();
    else switch(m_DvdMenu->ProcessKey(Key)) {
      case osUser1: Hide(); m_Player->Control("EVENT XINE_EVENT_INPUT_MENU1"); break;
      case osUser2: Hide(); m_Player->Control("EVENT XINE_EVENT_INPUT_MENU2"); break;
      case osUser3: Hide(); m_Player->Control("EVENT XINE_EVENT_INPUT_MENU3"); break;
      case osUser4: Hide(); m_Player->Control("EVENT XINE_EVENT_INPUT_MENU4"); break;
      case osUser5: Hide(); m_Player->Control("EVENT XINE_EVENT_INPUT_MENU5"); break;
      case osBack:
      case osEnd:   Hide(); break;
      default:      break;
    }
    return osContinue;
  }

  // Update progress bar display
  if (m_DisplayReplay)
    Show();

  // Detect DVD menus
  bool MenuDomain = !xc.dvd_arrow_keys_control_playback;
  if(Key != kNone || m_DisplayReplay) {
    const char *dt = m_Player->GetMetaInfo(miDvdTitleNo);
    if(dt && !strcmp("0", dt))
      MenuDomain = true;
    else {
      dt = m_Player->GetMetaInfo(miDvdButtons);
      if(dt && *dt && *dt != '0')
	MenuDomain = true;
    }
  }

  // DVD menu navigation
  if(MenuDomain) {
    if(m_DisplayReplay)
      Hide();
    switch(Key) {
      case kUp:    m_Player->Control("EVENT XINE_EVENT_INPUT_UP");     return osContinue;
      case kDown:  m_Player->Control("EVENT XINE_EVENT_INPUT_DOWN");   return osContinue;
      case kLeft:  m_Player->Control("EVENT XINE_EVENT_INPUT_LEFT");   return osContinue;
      case kRight: m_Player->Control("EVENT XINE_EVENT_INPUT_RIGHT");  return osContinue;
      case kOk:    m_Player->Control("EVENT XINE_EVENT_INPUT_SELECT"); return osContinue;
      case kBack:  m_Player->Control("EVENT XINE_EVENT_INPUT_MENU1");  return osContinue;
      default:     break;
    }
  }

  // Handle normal keys

  if(!MenuDomain) {
    switch(Key) {
      // Replay control
      case kUp:    Key = kPlay;    break;
      case kDown:  Key = kPause;   break;
      case kLeft:  Key = kFastRew; break;
      case kRight: Key = kFastFwd; break;
      case kOk:    
		   if(m_Player->Speed() != 1) {
		     Hide();
		     m_ShowModeOnly = !m_ShowModeOnly;
		     Show();
		     break;
		   }
	           if(m_DisplayReplay) {
		     Hide();
		     m_ShowModeOnly = true;
		   } else {
		     Hide();
		     m_ShowModeOnly = false;
		     Show();
		   }
		   break;
      case kInfo:  Hide();
	           if(m_DisplayReplay && !m_ShowModeOnly) {
		     m_ShowModeOnly = true;
		   } else {
		     m_ShowModeOnly = false;
		     Show();
		   }
		   break;
      case kBack:  if (config_t::IsDvdImage(m_Player->File())) {
                     xc.main_menu_mode = m_Mode;
                   } else {
                     xc.main_menu_mode = ShowMenu;
                   }
                   Hide();
		   Close();
                   BackToMenu();
		   return osEnd;
      default:     break;
    }
  }

  switch(Key) {
    // DVD menus
    case kRed:    Hide();
                  m_DvdMenu = new cDvdMenu(m_Player);
		  break;
    // Playback control
    case kGreen:  m_Player->Control("SEEK -60");  break;
    case kYellow: m_Player->Control("SEEK +60");  break;
    case kUser8:
    case k1:      m_Player->Control("SEEK -20");  break;
    case kUser9:
    case k3:      m_Player->Control("SEEK +20");  break;

    case kStop:
    case kBlue:   Hide();
                  Close();
                  return osEnd;

    case k9:      m_Player->Control("EVENT XINE_EVENT_INPUT_NEXT TITLE"); break;
    case k7:      m_Player->Control("EVENT XINE_EVENT_INPUT_PREVIOUS TITLE"); break;
    case k6:
    case kNext:   m_Player->Control("EVENT XINE_EVENT_INPUT_NEXT CHAPTER"); break;
    case k4:
    case kPrev:   m_Player->Control("EVENT XINE_EVENT_INPUT_PREVIOUS CHAPTER"); break;

    case kFastFwd:
                  {
                    static const int speeds[] = { -3, -2, 1, 2, -4, 2, 3, 4, 4 };
		    m_Player->SetSpeed(speeds[m_Player->Speed() + 4]);
                    if(m_Player->Speed() != 1)
                      Show();
                    else
                      Hide();
                    break;
		  }
    case kFastRew:
                  {
                    static const int speeds[] = { 0, -4, -3, -2, 0, -2, 1, 2, 3 };
		    m_Player->SetSpeed(speeds[m_Player->Speed() + 4]);
                    if(m_Player->Speed() != 1 || !m_ShowModeOnly)
                      Show();
                    else
                      Hide();
                    break;
		  }
    case kInfo:   if(m_DisplayReplay) {
                    Hide();
                  } else {
                    m_ShowModeOnly = false;
                    Show();
		  }
                  break;
    case kPause:  if(m_Player->Speed()) {
                    m_Player->SetSpeed(0);
		    m_ShowModeOnly = false;
		    Show();
		    break;
                  }
                  // fall thru
    case kPlay:   m_Player->SetSpeed(1);
                  m_ShowModeOnly = true;
		  Hide();
                  break;
    default:      break;
  }

  return osContinue;
}

//
// cXinelibImagePlayer
//

class cXinelibImagePlayer : public cPlayer {
  private:
    cString m_Mrl;
    bool    m_Active;
    bool    m_Error;
    cXinelibDevice *m_Dev;

    bool    Play(void);

  protected:
    virtual void Activate(bool On);

  public:
    cXinelibImagePlayer(cXinelibDevice *Dev, const char *File);
    virtual ~cXinelibImagePlayer();

    bool ShowImage(const char *File);
    bool Error(void)                 { return m_Error; } 
};

cXinelibImagePlayer::cXinelibImagePlayer(cXinelibDevice *Dev, const char *File)
{
  m_Mrl    = File;
  m_Active = false;
  m_Error  = false;
  m_Dev    = Dev;
}

cXinelibImagePlayer::~cXinelibImagePlayer()
{
  Activate(false);
  Detach();
}

bool cXinelibImagePlayer::Play(void)
{
  if (m_Mrl[0] == '/')
    m_Mrl = cPlaylist::BuildMrl("file", m_Mrl);

  return ! (m_Error = !m_Dev->PlayFile(m_Mrl, 0, true));
}

void cXinelibImagePlayer::Activate(bool On)
{
  m_Active = On;
  m_Error  = false;
  if (On)
    Play();
  else
    m_Dev->PlayFile(NULL);
}

bool cXinelibImagePlayer::ShowImage(const char *File)
{
  m_Mrl = File;
  if (m_Active)
    return Play();
  return true;
}


//
// cXinelibImagesControl
//

cXinelibImagePlayer *cXinelibImagesControl::m_Player = NULL;
cMutex cXinelibImagesControl::m_Lock;

cXinelibImagesControl::cXinelibImagesControl(char **Files, int Index, int Count) :
  cControl(OpenPlayer(Files[Index]))
{
  m_DisplayReplay = NULL;
  m_Files = Files;
  m_File = NULL;
  m_Index = Index;
  m_Count = Count;
  m_Speed = 0;
  m_ShowModeOnly = false;

  Seek(0);
}

cXinelibImagesControl::~cXinelibImagesControl()
{
  if(m_DisplayReplay)
    delete m_DisplayReplay;
  m_DisplayReplay = NULL;

  cStatus::MsgReplaying(this, NULL, NULL, false);
  Close();

  if(m_Files) {
    int i=0;
    while(m_Files[i]) {
      free(m_Files[i]);
      m_Files[i] = NULL;
      i++;
    }
    delete [] m_Files;
    m_Files = NULL;
  }
}

cXinelibImagePlayer *cXinelibImagesControl::OpenPlayer(const char *File)
{
  m_Lock.Lock();
  if(!m_Player)
    m_Player = new cXinelibImagePlayer(&(cXinelibDevice::Instance()), File);
  m_Lock.Unlock();
  return m_Player;
}

void cXinelibImagesControl::Close(void)
{
  m_Lock.Lock();
  if(m_Player)
    delete m_Player;
  m_Player = NULL;
  m_Lock.Unlock();
}

void cXinelibImagesControl::Delete(void)
{
  if(Interface->Confirm(tr("Delete image ?"))) {
    if(!unlink(m_Files[m_Index])) {
      free(m_Files[m_Index]);
      for(int i=m_Index; i<m_Count; i++)
	m_Files[i] = m_Files[i+1];
      m_Count--;
      m_Files[m_Count] = NULL;      
      Seek(0);
    }
  }
}

cOsdObject *cXinelibImagesControl::GetInfo(void)
{
  return new cMetainfoMenu(m_Files[m_Index]);
}

void cXinelibImagesControl::Seek(int Rel)
{
  if(m_Index == m_Count-1 && Rel>0)
    m_Index = 0;
  else if(m_Index == 0 && Rel<0)
    m_Index = m_Count-1;
  else
    m_Index += Rel;

  if(m_Index < 0) 
    m_Index = 0; 
  else if(m_Index >= m_Count) 
    m_Index = m_Count;
  
  char *pt;
  free(m_File);
  m_File = strdup(m_Files[m_Index]);
  if(NULL != (pt=strrchr(m_File, '/')))
    strcpy(m_File, pt+1); 
  if(NULL != (pt=strrchr(m_File, '.')))
    *pt = 0;

  cStatus::MsgReplaying(this, m_File, m_Files[m_Index], true);

  m_Player->ShowImage(m_Files[m_Index]);
  m_LastShowTime = time(NULL);
  strn0cpy(xc.browse_images_dir, m_Files[m_Index], sizeof(xc.browse_images_dir));
}

void cXinelibImagesControl::Show(void)
{
  bool Play = (m_Speed!=0), Forward = m_Speed>=0;
  int Speed = abs(m_Speed);
 
  if(!m_DisplayReplay) {
    m_DisplayReplay = Skins.Current()->DisplayReplay(m_ShowModeOnly);
  }

  if(!m_ShowModeOnly) {
    char t[128] = "";
    m_DisplayReplay->SetTitle(m_File);
    m_DisplayReplay->SetProgress(m_Index, m_Count);
    sprintf(t, "%d", m_Count);
    m_DisplayReplay->SetTotal( t );
    sprintf(t, "%d", m_Index+1);
    m_DisplayReplay->SetCurrent( t );
  }

  m_DisplayReplay->SetMode(Play, Forward, Speed);
  m_DisplayReplay->Flush();
}

void cXinelibImagesControl::Hide(void)
{
  if(m_DisplayReplay) {
    delete m_DisplayReplay;
    m_DisplayReplay = NULL;
  }
}

eOSState cXinelibImagesControl::ProcessKey(eKeys Key)
{
  switch(Key) {
    case kBack:    xc.main_menu_mode = ShowImages;
                   Hide(); 
                   Close(); 
                   BackToMenu();
                   //return osPlugin;		   
		   return osEnd;
    case kYellow:  Delete(); 
                   break;
    case kStop:
    case kBlue:    Hide();
                   Close();
                   return osEnd;
    case kPrev:
    case kLeft:    Seek(-1);
                   break;
    case kNext:
    case kRight:   Seek(1);
                   break;
    case kUp:      Seek(5);  
                   break;
    case kDown:    Seek(-5);
                   break;
    case kPause:   m_Speed = 0;
                   break;
    case kPlay:    m_Speed = 2;
                   break;
    case kFastFwd: m_Speed++;
                   break;
    case kFastRew: m_Speed--;
                   break;
    case kOk:      if(m_DisplayReplay) {
                     if(m_ShowModeOnly) {
                       Hide();
                       m_ShowModeOnly = false;
                       Show();
                     } else {
		       Hide();
		     }
                   } else {
                     m_ShowModeOnly = true;
                     Show();
		   }
                   break;
    default:       break;
  }

  static const int Speed2Time[] = { 0, 5, 3, 1 };
  if(m_Speed > 3)
    m_Speed = 3;
  if(m_Speed < -3)
    m_Speed = -3;

  if(Key == kNone && m_Speed != 0) {
    if(m_LastShowTime + Speed2Time[m_Speed<0 ? -m_Speed : m_Speed] <= time(NULL))
      Seek(sgn(m_Speed));
  }

  if (m_DisplayReplay) 
    Show();

  return osContinue;
}
