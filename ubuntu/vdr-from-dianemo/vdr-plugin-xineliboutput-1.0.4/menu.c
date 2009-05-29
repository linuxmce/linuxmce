/*
 * menu.c: Main Menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menu.c,v 1.57.2.1 2009/02/12 11:04:03 phintuka Exp $
 *
 */

#include <dirent.h>

#include <vdr/config.h>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>
#include <vdr/videodir.h>

#include "logdefs.h"
#include "config.h"
#include "menu.h"
#include "menuitems.h"
#include "tools/metainfo_menu.h"
#include "device.h"
#include "media_player.h"
#include "equalizer.h"
#include "i18n.h"          // trVDR for VDR-1.4.x

#ifndef HOTKEY_START
# define HOTKEY_START        kRed

# define HOTKEY_DVD          k0    /* */
# define HOTKEY_DVD_TRACK1   k1    /* */
# define HOTKEY_DVD_SPU      k2    /* */

# define HOTKEY_NEXT_ASPECT  k3    /* auto, 4:3, 16:9 */
# define HOTKEY_TOGGLE_CROP  k4    /* off, force, auto */
# define HOTKEY_UPMIX        k5    /* off, on */
# define HOTKEY_DOWNMIX      k6    /* off, on */
# define HOTKEY_DEINTERLACE  k7    /* off, on */
# define HOTKEY_LOCAL_FE     k8    /* off, on */

# define HOTKEY_PLAYLIST     k9    /* Start replaying playlist or file pointed by
				      symlink $(CONFDIR)/plugins/xineliboutput/default_playlist */
# define HOTKEY_ADELAY_UP    kUp   /* audio delay up */
# define HOTKEY_ADELAY_DOWN  kDown /* audio delay down */
# define HOTKEY_TOGGLE_VO_ASPECT kRight
#endif

//#define OLD_TOGGLE_FE

#define ISNUMBERKEY(k) (RAWKEY(k) >= k0 && RAWKEY(k) <= k9)

//--------------------------- cMenuBrowseFiles -------------------------------

class cMenuBrowseFiles : public cOsdMenu 
{
  protected:
    const eMainMenuMode m_Mode;
    bool          m_OnlyQueue;
    char         *m_CurrentDir;
    char         *m_ConfigLastDir;

    virtual bool ScanDir(const char *DirName);
    virtual eOSState Open(bool ForceOpen = false, bool Parent = false, bool Queue = false);
    virtual eOSState Delete(void);
    virtual eOSState Info(void);
    virtual void Set(void);
    virtual void SetHelpButtons(void);
    cFileListItem *GetCurrent() { return (cFileListItem *)Get(Current()); }
    void StoreConfig(void);
    char *GetLastDir(void);

  public:
    cMenuBrowseFiles(eMainMenuMode mode = ShowFiles, bool OnlyQueue=false);
    ~cMenuBrowseFiles();

    virtual eOSState ProcessKey(eKeys Key);
};

static char *ParentDir(const char *dir)
{
  char *result = strdup(dir);
  char *pt = strrchr(result, '/');
  if(pt) {
    *(pt+1)=0;
    if(pt != result)
      *pt = 0;
  }
  return result;
}

static char *LastDir(const char *dir)
{
  char *pt = strrchr(dir, '/');
  if(pt && pt[0] && pt[1])
    return strdup(pt+1);
  return NULL;
}

cMenuBrowseFiles::cMenuBrowseFiles(eMainMenuMode mode, bool OnlyQueue) :
    cOsdMenu( ( mode==ShowImages ? tr("Images") :
		mode==ShowMusic ? (!OnlyQueue ? tr("Play music") : tr("Add to playlist")) :
		/*mode==ShowFiles ?*/ tr("Play file")),
	      2, 4),
    m_Mode(mode)
{
  m_CurrentDir = NULL;
  m_OnlyQueue  = OnlyQueue;

  m_ConfigLastDir = GetLastDir();
  Set();
}

cMenuBrowseFiles::~cMenuBrowseFiles()
{
  Setup.Save();
  free(m_CurrentDir);
}

char *cMenuBrowseFiles::GetLastDir(void)
{
  switch(m_Mode) {
    case ShowMusic:  return xc.browse_music_dir;
    case ShowImages: return xc.browse_images_dir;
    default:
    case ShowFiles:  return xc.browse_files_dir;
  }
  return xc.browse_files_dir;
}

void cMenuBrowseFiles::Set(void)
{
  Clear();

  if(!m_CurrentDir) 
    m_CurrentDir = strdup(m_ConfigLastDir);

  if(m_CurrentDir[0] != '/') {
    free(m_CurrentDir);
    m_CurrentDir = strdup(VideoDirectory);
  }

  // find deepest accessible directory from path
  while(!ScanDir(m_CurrentDir) && strlen(m_CurrentDir) > 1) {
    char *n = ParentDir(m_CurrentDir);
    free(m_CurrentDir);
    m_CurrentDir = n;
  }

  // add link to parent folder
  if(strlen(m_CurrentDir) > 1)
    Add(new cFileListItem("..",true));

  Sort();

  SetCurrent(Get(Count()>1 && strlen(m_CurrentDir)>1 ? 1 : 0));

  // select last selected item

  char *lastParent = ParentDir(m_ConfigLastDir);
  if(!strncmp(m_CurrentDir,lastParent,strlen(m_CurrentDir))) {
    char *item = LastDir(m_ConfigLastDir);
    if(item) {
      for(cFileListItem *it = (cFileListItem*)First(); it; it = (cFileListItem*)Next(it))
	if(!strcmp(it->Name(),item))
	  SetCurrent(it);
      free(item);
    }
  }
  free(lastParent);

  strn0cpy(m_ConfigLastDir, m_CurrentDir, sizeof(xc.browse_files_dir));
  StoreConfig();

  SetHelpButtons();
}

void cMenuBrowseFiles::StoreConfig(void)
{
  cPluginManager::GetPlugin(PLUGIN_NAME_I18N)->SetupStore("Media.BrowseMusicDir",  
							  xc.browse_music_dir);
  cPluginManager::GetPlugin(PLUGIN_NAME_I18N)->SetupStore("Media.BrowseFilesDir",  
							  xc.browse_files_dir);
  cPluginManager::GetPlugin(PLUGIN_NAME_I18N)->SetupStore("Media.BrowseImagesDir", 
							  xc.browse_images_dir);
#if 1
  // delete old keys (<1.0.0)
  cPluginManager::GetPlugin(PLUGIN_NAME_I18N)->SetupStore("BrowseMusicDir");
  cPluginManager::GetPlugin(PLUGIN_NAME_I18N)->SetupStore("BrowseFilesDir");
  cPluginManager::GetPlugin(PLUGIN_NAME_I18N)->SetupStore("BrowseImagesDir");
#endif
}

void cMenuBrowseFiles::SetHelpButtons(void)
{
  bool isDir = !GetCurrent() || GetCurrent()->IsDir();
  bool isDvd = GetCurrent() && GetCurrent()->IsDvd();
  SetHelp((isDir && isDvd) ? trVDR("Button$Open") : !m_OnlyQueue ? trVDR("Button$Play"): NULL,
	  (m_Mode == ShowMusic) ? tr("Button$Queue") : 
	           strlen(m_CurrentDir)>1 ? "[..]" : NULL,
	  (isDir && !isDvd) ? NULL : trVDR("Button$Delete"),
	  isDir ? NULL : trVDR("Button$Info"));
  Display();
}

eOSState cMenuBrowseFiles::Delete(void)
{
  cFileListItem *it = GetCurrent();
  if(!it->IsDir()) {
    if (Interface->Confirm(trVDR("Delete recording?"))) {
      cString name = cString::sprintf("%s/%s", m_CurrentDir, it->Name());
      if(!unlink(name)) {
        isyslog("file %s deleted", *name);
	if(m_Mode != ShowImages) {
	  name = cString::sprintf("%s.resume", *name);
	  unlink(name);
	}
        cOsdMenu::Del(Current());
	SetHelpButtons();
        Display();
      } else {
        Skins.Message(mtError, trVDR("Error while deleting recording!"));
        isyslog("Error deleting file %s", *name);
      }
    }
  }
  return osContinue;
}

eOSState cMenuBrowseFiles::Open(bool ForceOpen, bool Parent, bool Queue)
{
  if(!GetCurrent()) {
    return osContinue;
  }

  /* parent directory */
  if(Parent || !strcmp("..", GetCurrent()->Name())) {
    char *n = ParentDir(m_CurrentDir);
    free(m_CurrentDir);
    m_CurrentDir = n;
    Set();
    return osContinue;

  /* directory */
  } else if (GetCurrent()->IsDir()) {

    if(!ForceOpen && GetCurrent()->IsDvd()) {
      /* play dvd */
      cString f = cString::sprintf("dvd:%s/%s", m_CurrentDir, GetCurrent()->Name());
      cControl::Shutdown();
      cControl::Launch(new cXinelibDvdPlayerControl(f));
      return osEnd;
    }
    if(ForceOpen && GetCurrent()->IsDir() && !GetCurrent()->IsDvd()) {
      /* play all files */ 
      if(m_Mode != ShowImages) {

	if(m_OnlyQueue && !Queue)
	  return osContinue;

	cString f = cString::sprintf("%s/%s/", m_CurrentDir, GetCurrent()->Name());

	if(!Queue || !cXinelibPlayerControl::IsOpen())
	  cControl::Shutdown();
	if(Queue)
	  cXinelibPlayerControl::Queue(f);
	else 
	  cControl::Launch(new cXinelibPlayerControl(m_Mode, f));
	return Queue ? osContinue : osEnd;

      } else {
	// TODO: show all images
      }
    }
    const char *d = GetCurrent()->Name();
    char *buffer = NULL;
    if (asprintf(&buffer, "%s/%s", m_CurrentDir, d) > 0) {
      while(buffer[0] == '/' && buffer[1] == '/')
        strcpy(buffer, buffer+1);
      free(m_CurrentDir);
      m_CurrentDir = buffer;
    }
    Set();
    return osContinue;
    
  /* regular file */
  } else {
    cString f = cString::sprintf("%s%s/%s", 
				 GetCurrent()->IsDvd() ? "dvd:" : "",
				 m_CurrentDir, GetCurrent()->Name());
    if (GetCurrent()->IsDvd()) 
      strn0cpy(m_ConfigLastDir, m_CurrentDir, sizeof(xc.browse_files_dir));
    else
      strn0cpy(m_ConfigLastDir, f, sizeof(xc.browse_files_dir));
    StoreConfig();

    if(m_Mode != ShowImages) {
      /* video/audio */
      if(m_OnlyQueue && !Queue)
	return osContinue;
      if(!Queue || !cXinelibPlayerControl::IsOpen())
	cControl::Shutdown();
      if(Queue)
	cXinelibPlayerControl::Queue(f);
      if(!cXinelibPlayerControl::IsOpen())
	cControl::Launch(GetCurrent()->IsDvd()
			 ? new cXinelibDvdPlayerControl(f)
			 : new cXinelibPlayerControl(m_Mode, f, GetCurrent()->SubFile()));
      if(Queue)
	return osContinue;
    } else {
      /* image */
      char **files = new char*[Count()+1];
      int i = 0, index = 0;
      memset(files, 0, sizeof(char*)*(Count()+1));
      for(cFileListItem *it = (cFileListItem*)First(); it; it=(cFileListItem*)Next(it)) {
	if(it==Get(Current()))
	  index = i;
	if(!it->IsDir())
	  if (asprintf(&files[i++], "%s/%s", m_CurrentDir, it->Name()) < 0)
            i--;
      }
      cControl::Shutdown();
      cControl::Launch(new cXinelibImagesControl(files, index, i));
    }
    return osEnd;
  }
  return osContinue;
}

eOSState cMenuBrowseFiles::Info(void)
{
  if(GetCurrent() && !GetCurrent()->IsDir()) {
    cString filename = cString::sprintf("%s/%s", m_CurrentDir, GetCurrent()->Name());
    return AddSubMenu(new cMetainfoMenu(filename));
  }
  return osContinue;
}

bool cMenuBrowseFiles::ScanDir(const char *DirName)
{
  DIR *d = opendir(DirName);
  if (d) {
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
      if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) {
	cString buffer = cString::sprintf("%s/%s", DirName, e->d_name);
        struct stat st;
        if (stat(buffer, &st) == 0) {

	  // check symlink destination
          if (S_ISLNK(st.st_mode)) {
	    buffer = ReadLink(buffer);
	    if (!*buffer || stat(buffer, &st))
	      continue;
	  }

	  // folders
          if (S_ISDIR(st.st_mode)) {

	    if(m_Mode == ShowImages || m_Mode == ShowMusic)
	      Add(new cFileListItem(e->d_name, true));
	    else {
	      // check if DVD
              bool dvd = false;
	      buffer = cString::sprintf("%s/%s/VIDEO_TS/VIDEO_TS.IFO", DirName, e->d_name);
              if (stat(buffer, &st) == 0)
                dvd = true;
	      else {
		buffer = cString::sprintf("%s/%s/video_ts/video_ts.ifo", DirName, e->d_name);
		if (stat(buffer, &st) == 0)
		  dvd = true;
	      }
	      Add(new cFileListItem(e->d_name, true, false, false, dvd));
	    }

          // regular files
          } else if(e->d_name[0] != '.') {

	    // audio
	    if (m_Mode == ShowMusic && xc.IsAudioFile(buffer)) {
	      Add(new cFileListItem(e->d_name, false));

	    // images
            } else if(m_Mode == ShowImages && xc.IsImageFile(buffer)) {
	      Add(new cFileListItem(e->d_name, false));

	    // video
	    } else if (m_Mode == ShowFiles && xc.IsVideoFile(buffer)) {
	      bool resume = false, subs = false, dvd = false;
	      char *pos = strrchr(e->d_name, '.');
	      cString subfile;

	      if(pos) {
		// .iso image -> dvd
		if(pos && !strcasecmp(pos, ".iso"))
		  dvd = true;

		// separate subtitles ?
		subfile = cString::sprintf("%s/%s____", DirName, e->d_name);
		char *p = strrchr(subfile, '.');
		if( p ) {
		  int i;
		  for(i=0; xc.s_subExts[i] && !subs; i++) {
		    strcpy(p, xc.s_subExts[i]);
		    if (stat(subfile, &st) == 0)
		      subs = true;
		  }
		}
	      }

	      // resume file ?
	      buffer = cString::sprintf("%s/%s.resume", DirName, e->d_name);
	      if (stat(buffer, &st) == 0)
		resume = true;

	      Add(new cFileListItem(e->d_name, false, resume, subs?*subfile:NULL, dvd));
	    }
          }
        }
      }
    }
    closedir(d);
    return true;
  }
  return false;
}

eOSState cMenuBrowseFiles::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kPlay:   
       case kOk:     return Open(false, false, m_OnlyQueue);
       case kRed:    return Open(true);
       case kGreen:  return Open(true, m_Mode != ShowMusic, 
				 m_Mode==ShowMusic ? m_OnlyQueue=true : false);
       case kYellow: return Delete();
       case kBlue:   return Info();
       default: break;
       }
     }

  if (state == osUnknown)
    state = osContinue;

  if(!HasSubMenu())
    SetHelpButtons();

  return state;
}


#if VDRVERSNUM < 10515

//-------------------------- cDisplaySpuTracks ------------------------------
//
// cDisplaySpuTracks : almost identical copy of VDR 1.4.5 cDisplayTracks
//

#define TRACKTIMEOUT 5000 //ms

class cDisplaySpuTracks : public cOsdObject {
private:
  cSkinDisplayTracks *displayTracks;
  cTimeMs timeout;
  eTrackType types[64+2];
  char *descriptions[64+2];
  int numTracks, track;
  static cDisplaySpuTracks *currentDisplayTracks;
  virtual void Show(void);
  cDisplaySpuTracks(void);
public:
  virtual ~cDisplaySpuTracks();
  static bool IsOpen(void) { return currentDisplayTracks != NULL; }
  static cDisplaySpuTracks *Create(void);
  static void Process(eKeys Key);
  eOSState ProcessKey(eKeys Key);
  };

cDisplaySpuTracks *cDisplaySpuTracks::currentDisplayTracks = NULL;

cDisplaySpuTracks::cDisplaySpuTracks(void) : cOsdObject(true)
{
  currentDisplayTracks = this;
  numTracks = track = 0;
  int CurrentTrack = cXinelibDevice::Instance().GetCurrentDvdSpuTrack();

  track = numTracks;
  types[numTracks] = eTrackType(ttXSubtitleNone);
  descriptions[numTracks] = strdup("(none)");
  numTracks++;

  for (int i = 0; i <= 63; i++) {
      const tTrackId *TrackId = cXinelibDevice::Instance().GetDvdSpuTrack(i);
      if (TrackId) {
         types[numTracks] = eTrackType(i);
         descriptions[numTracks] = strdup(*TrackId->description ? TrackId->description : *TrackId->language ? TrackId->language : *itoa(i));
         if (i == CurrentTrack)
            track = numTracks;
         numTracks++;
         }
      }
  timeout.Set(TRACKTIMEOUT);
  displayTracks = NULL;
}

cDisplaySpuTracks::~cDisplaySpuTracks()
{
  delete displayTracks;
  currentDisplayTracks = NULL;
  for (int i = 0; i < numTracks; i++)
      free(descriptions[i]);
}

void cDisplaySpuTracks::Show(void)
{
  if(!displayTracks)
    displayTracks = Skins.Current()->DisplayTracks(tr("Subtitles"), numTracks, descriptions);

  displayTracks->SetTrack(track, descriptions);
  displayTracks->SetAudioChannel(-1);
  displayTracks->Flush();
}

cDisplaySpuTracks *cDisplaySpuTracks::Create(void)
{
  if (cXinelibDevice::Instance().NumDvdSpuTracks() > 0) {
     if (!currentDisplayTracks)
        new cDisplaySpuTracks;
     return currentDisplayTracks;
     }
  return NULL;
}

void cDisplaySpuTracks::Process(eKeys Key)
{
  if (currentDisplayTracks)
     currentDisplayTracks->ProcessKey(Key);
}

eOSState cDisplaySpuTracks::ProcessKey(eKeys Key)
{
  if(!displayTracks) {
    Show();
  }

  int oldTrack = track;
  switch (Key) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
         if (NORMALKEY(Key) == kUp && track > 0)
            track--;
         else if (NORMALKEY(Key) == kDown && track < numTracks - 1)
            track++;
         timeout.Set(TRACKTIMEOUT);
         break;
    case kNext:
    //case kSubtitle|k_Repeat:
    //case kSubtitle:
         if (++track >= numTracks)
            track = 0;
         timeout.Set(TRACKTIMEOUT);
         break;
    case kOk:
         if (track != cXinelibDevice::Instance().GetCurrentDvdSpuTrack())
            oldTrack = -1; // make sure we explicitly switch to that track
         timeout.Set();
         break;
    case kNone: break;
    default: if ((Key & k_Release) == 0)
                return osEnd;
    }
  if (track != oldTrack)
     Show();
  if (track != oldTrack) {
     cXinelibDevice::Instance().SetCurrentDvdSpuTrack(types[track], true);
     }
  return timeout.TimedOut() ? osEnd : osContinue;
}
#endif // VDRVERSNUM < 10515

//----------------------------- cMenuXinelib ---------------------------------

#include "tools/display_message.h"

static cOsdItem *NewTitle(const char *s)
{
  cString str = cString::sprintf("----- %s -----", s);
  cOsdItem *tmp = new cOsdItem(str);
  tmp->SetSelectable(false);
  return tmp;
}


extern cOsdObject *g_PendingMenuAction;

time_t cMenuXinelib::g_LastHotkeyTime = 0;
eKeys  cMenuXinelib::g_LastHotkey = kNone;

cMenuXinelib::cMenuXinelib()
{
  field_order = xc.field_order;
  compression = xc.audio_compression;
  headphone = xc.headphone;
  autocrop = xc.autocrop;
  overscan = xc.overscan;

  hotkey_state = hkInit;

  novideo = cXinelibDevice::Instance().GetPlayMode() == pmAudioOnlyBlack ? 1 : 0;

  Add(NewTitle(tr("Media")));
  Add(new cOsdItem(tr("Play file >>"), osUser1));
  Add(new cOsdItem(tr("Play music >>"), osUser2));
  Add(new cOsdItem(tr("View images >>"), osUser3));
  if(xc.remote_mode)
    Add(new cOsdItem(tr("Play remote DVD >>"), osUser4));
  else
    Add(new cOsdItem(tr("Play DVD disc >>"), osUser4));
  if(xc.remote_mode)
    Add(new cOsdItem(tr("Play remote CD >>"), osUser6));
  else
    Add(new cOsdItem(tr("Play audio CD >>"), osUser6));
#if VDRVERSNUM < 10515
  if(cXinelibDevice::Instance().NumDvdSpuTracks() > 0)
    Add(new cOsdItem(tr("Select subtitle track >>"), osUser5));
#endif
  Add(NewTitle(tr("Video settings")));
  Add(ctrl_novideo = new cMenuEditBoolItem(tr("Play only audio"), 
					   &novideo));
  Add(ctrl_autocrop = new cMenuEditBoolItem(tr("Crop letterbox 4:3 to 16:9"), 
					    &autocrop));
  Add(ctrl_overscan = new cMenuEditTypedIntItem(tr("Overscan (crop image borders)"), "%",
						&overscan, 0, 10,
						tr("Off")));
#ifdef HAVE_XV_FIELD_ORDER
  Add(video_ctrl_interlace_order = new cMenuEditStraI18nItem(tr("Interlaced Field Order"), 
							     &field_order, 2, xc.s_fieldOrder));
#endif

  Add(NewTitle(tr("Audio settings")));
#ifdef ENABLE_TEST_POSTPLUGINS
  Add(ctrl_headphone = new cMenuEditBoolItem(tr("Headphone audio mode"), 
					     &headphone));
#else
  ctrl_headphone = NULL;
#endif

  Add(audio_ctrl_compress = new cMenuEditTypedIntItem(tr("Audio Compression"),"%", 
						      &compression, 100, 500, NULL, tr("Off")));

  Add(new cOsdItem(tr("Audio equalizer >>"), osUser7));

  switch(xc.main_menu_mode) {
    case ShowFiles:  AddSubMenu(new cMenuBrowseFiles(ShowFiles)); break;
    case ShowMusic:  AddSubMenu(new cMenuBrowseFiles(ShowMusic)); break;
    case ShowImages: AddSubMenu(new cMenuBrowseFiles(ShowImages)); break;
    default: break;
  }

  xc.main_menu_mode = ShowMenu;
}

cMenuXinelib::~cMenuXinelib()
{
#ifdef HAVE_XV_FIELD_ORDER
  if(xc.field_order != field_order )
    cXinelibDevice::Instance().ConfigureWindow(xc.fullscreen, xc.width, xc.height, 
					       xc.modeswitch, xc.modeline, xc.display_aspect, 
					       xc.scale_video, xc.field_order);
#endif

  if(xc.audio_compression != compression)
    cXinelibDevice::Instance().ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay, 
						       xc.audio_compression, xc.audio_equalizer, 
						       xc.audio_surround, xc.speaker_type);

  if(xc.overscan != overscan)
    cXinelibDevice::Instance().ConfigureVideo(xc.hue, xc.saturation, xc.brightness,
					      xc.contrast, xc.overscan, xc.vo_aspect_ratio);

  if(xc.headphone != headphone)
    cXinelibDevice::Instance().ConfigurePostprocessing("headphone", 
						       xc.headphone ? true : false);

  if(xc.autocrop != autocrop)
    cXinelibDevice::Instance().ConfigurePostprocessing("autocrop", 
						       xc.autocrop ? true : false,
						       xc.AutocropOptions());

  int dev_novideo = cXinelibDevice::Instance().GetPlayMode() == pmAudioOnlyBlack ? 1 : 0;
  if(dev_novideo != novideo) 
    cXinelibDevice::Instance().SetPlayMode(novideo ? pmAudioOnlyBlack : pmNone);
}

cOsdMenu *cMenuXinelib::CreateMenuBrowseFiles(eMainMenuMode mode, bool Queue)
{
  return new cMenuBrowseFiles(mode, true);
}

eOSState cMenuXinelib::ProcessKey(eKeys Key)
{
  /* Hot key support */
  if(hotkey_state == hkInit && Key == kNone)
    return osContinue;
  if(hotkey_state == hkInit && Key == HOTKEY_START) {
    hotkey_state = hkSeen;
    return osContinue;
  } else if(hotkey_state == hkSeen && Key != kNone) {
    hotkey_state = hkNone;
    return ProcessHotkey(Key);
  }
  hotkey_state = hkNone;
    
  cOsdItem *item = Get(Current());

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if(HasSubMenu())
    return state;

  switch(state) {
    case osUser1:
      AddSubMenu(new cMenuBrowseFiles(ShowFiles));
      return osUnknown;
    case osUser2:
      AddSubMenu(new cMenuBrowseFiles(ShowMusic));
      return osUnknown;
    case osUser3:
      AddSubMenu(new cMenuBrowseFiles(ShowImages));
      return osContinue;
    case osUser4:
      cControl::Shutdown();
      cControl::Launch(new cXinelibDvdPlayerControl("dvd:/"));
      return osEnd;
    case osUser6:
      cControl::Shutdown();
      cControl::Launch(new cXinelibPlayerControl(ShowMusic, "cdda:/"));
      return osEnd;
#if VDRVERSNUM < 10515
    case osUser5:
      if(!g_PendingMenuAction) {
	g_PendingMenuAction = cDisplaySpuTracks::Create();
	return osPlugin;
      }
      return osContinue;
#endif
    case osUser7:
      if(!g_PendingMenuAction) {
	g_PendingMenuAction = new cEqualizer();
	return osPlugin;
      }
      return osContinue;
    default: ;
  }

  Key = NORMALKEY(Key);

  if(Key==kLeft || Key==kRight || ISNUMBERKEY(Key)) {
    if(item == audio_ctrl_compress)
      cXinelibDevice::Instance().ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay, 
							 compression, xc.audio_equalizer, 
							 xc.audio_surround, xc.speaker_type);
    else if(item == ctrl_overscan)
      cXinelibDevice::Instance().ConfigureVideo(xc.hue, xc.saturation, xc.brightness,
                                                xc.contrast, overscan, xc.vo_aspect_ratio);
  }
  if(Key==kLeft || Key==kRight) {
    if(item == ctrl_headphone)
      cXinelibDevice::Instance().ConfigurePostprocessing("headphone", headphone?true:false);    
    else if(item == ctrl_autocrop)
      cXinelibDevice::Instance().ConfigurePostprocessing("autocrop", autocrop?true:false,
							 xc.AutocropOptions());
    else if(item == ctrl_novideo)
      cXinelibDevice::Instance().SetPlayMode(novideo ? pmAudioOnlyBlack : pmNone);
#ifdef HAVE_XV_FIELD_ORDER
    else if(video_ctrl_interlace_order && item == video_ctrl_interlace_order)
      cXinelibDevice::Instance().ConfigureWindow(xc.fullscreen, xc.width, xc.height, 
						 xc.modeswitch, xc.modeline, 
						 xc.display_aspect, xc.scale_video, 
						 field_order);
#endif
  }
  
  return state;
}
  
void cMenuXinelib::Store(void)
{
#ifdef HAVE_XV_FIELD_ORDER
  xc.field_order = field_order;
#endif
  xc.audio_compression = compression;
  xc.autocrop = autocrop;
  xc.overscan = overscan;
  xc.headphone = headphone;
}

#if APIVERSNUM < 10404
#  warning Using hotkeys may segfault with VDR version < 1.4.3-2
#endif

eOSState cMenuXinelib::ProcessHotkey(eKeys Key)
{
  eOSState NewState = osEnd;
  cString  Message;
  time_t   now      = time(NULL);
  bool     OnlyInfo = ((g_LastHotkeyTime < now-3) || g_LastHotkey != Key);

  switch(Key) {
    case HOTKEY_DVD:
      cControl::Shutdown();
      cControl::Launch(new cXinelibDvdPlayerControl("dvd:/"));
      break;

    case HOTKEY_DVD_TRACK1:
      cControl::Shutdown();
      cControl::Launch(new cXinelibDvdPlayerControl("dvd:/1"));
      break;

#if VDRVERSNUM < 10515
    case HOTKEY_DVD_SPU:
      /* use audio track display menu */
      if(!g_PendingMenuAction) {
	bool WasOpen = cDisplaySpuTracks::IsOpen();
	g_PendingMenuAction = cDisplaySpuTracks::Create();
	if(g_PendingMenuAction) {
	  cRemote::CallPlugin("xineliboutput");
	  if(WasOpen || !OnlyInfo) cRemote::Put(kNext);
	} else {
	  Message = tr("No subtitles available!");
	}
      }
      break;
#endif
    case HOTKEY_LOCAL_FE:
      /* off, on */
      {
	int local_frontend = strstra(xc.local_frontend, xc.s_frontends, 0);

#ifndef OLD_TOGGLE_FE
	if(local_frontend==FRONTEND_NONE)
	  // no need to show current frontend if there is no output device ...
	  OnlyInfo = false;
#endif
	if(!OnlyInfo) {
#ifndef OLD_TOGGLE_FE
	  static int orig_frontend = -1;
	  if(orig_frontend < 0)
	    orig_frontend = local_frontend;

	  if(orig_frontend == FRONTEND_NONE) {
	    // no frontends were loaded at startup -> loop thru all frontends
	    local_frontend++;
	  } else {
	    // frontend was loaded at startup -> toggle it on/off 
	    if(local_frontend == FRONTEND_NONE)
	      local_frontend = orig_frontend;
	    else
	      local_frontend = FRONTEND_NONE;
	  }
#else
	  local_frontend++;
#endif
	  if(local_frontend >= FRONTEND_count)
	    local_frontend = 0;
	  strn0cpy(xc.local_frontend, xc.s_frontends[local_frontend], sizeof(xc.local_frontend));
	  cXinelibDevice::Instance().ConfigureWindow(
	      xc.fullscreen, xc.width, xc.height, xc.modeswitch, xc.modeline, 
	      xc.display_aspect, xc.scale_video, xc.field_order);
	}
	Message = cString::sprintf("%s %s %s", tr("Local Frontend"), 
				   OnlyInfo ? ":" : "->", 
				   xc.s_frontendNames[local_frontend]);
      }
      break;

    case HOTKEY_NEXT_ASPECT:
      /* auto, 4:3, 16:9, ... */
      if(!OnlyInfo) {
	xc.display_aspect = (xc.display_aspect < ASPECT_count-1) ? xc.display_aspect+1 : 0;
	cXinelibDevice::Instance().ConfigureWindow(xc.fullscreen, xc.width, xc.height, 
						   xc.modeswitch, xc.modeline, xc.display_aspect, 
						   xc.scale_video, xc.field_order);      
      }
      Message = cString::sprintf("%s %s %s", tr("Aspect ratio"), 
				 OnlyInfo ? ":" : "->",
				 tr(xc.s_aspects[xc.display_aspect]));
      break;

    case HOTKEY_TOGGLE_VO_ASPECT:
      /* auto, square, 4:3, anamorphic or DVB */
      if(!OnlyInfo) {
        xc.vo_aspect_ratio = (xc.vo_aspect_ratio < VO_ASPECT_count-1) ? xc.vo_aspect_ratio + 1 : 0;
        cXinelibDevice::Instance().ConfigureVideo(xc.hue, xc.saturation, xc.brightness,
                                              xc.contrast, xc.overscan, xc.vo_aspect_ratio);
      }
      Message = cString::sprintf("%s %s %s", tr("Video aspect ratio"),
				 OnlyInfo ? ":" : "->",
				 tr(xc.s_vo_aspects[xc.vo_aspect_ratio]));
      break;

    case HOTKEY_TOGGLE_CROP:    
      /* off, force, auto */
      if(!OnlyInfo) {
	if(!xc.autocrop) {
	  xc.autocrop = 1;
	  xc.autocrop_autodetect = 1;
	} else if(xc.autocrop_autodetect) {
	  xc.autocrop_autodetect = 0;
	} else {
	  xc.autocrop = 0;
	}
	cXinelibDevice::Instance().ConfigurePostprocessing("autocrop", 
							   xc.autocrop ? true : false,
							   xc.AutocropOptions());
      }

      Message = cString::sprintf("%s %s %s", tr("Crop letterbox 4:3 to 16:9"), 
				 OnlyInfo ? ":" : "->",
				 !xc.autocrop ? tr("Off") : xc.autocrop_autodetect ? tr("automatic") : tr("On"));
      break;

    case HOTKEY_DEINTERLACE:    
      {
	/* off, on */
	int off = !strcmp(xc.deinterlace_method, "none");
	if(!OnlyInfo) {
	  off = !off;
	  if(off)
	    strcpy(xc.deinterlace_method, "none");
	  else
	    strcpy(xc.deinterlace_method, "tvtime");
	  cXinelibDevice::Instance().ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay, 
							     compression, xc.audio_equalizer, 
							     xc.audio_surround, xc.speaker_type);
	}
	Message = cString::sprintf("%s %s %s", tr("Deinterlacing"), 
				   OnlyInfo ? ":" : "->", 
				   tr(off ? "Off":"On"));
      }
      break;

    case HOTKEY_UPMIX:    
      /* off, on */
      if(!OnlyInfo) {
	xc.audio_upmix = xc.audio_upmix ? 0 : 1;
	cXinelibDevice::Instance().ConfigurePostprocessing(
		  "upmix", xc.audio_upmix ? true : false, NULL);
      }
      Message = cString::sprintf("%s %s %s", 
				 tr("Upmix stereo to 5.1"), 
				 OnlyInfo ? ":" : "->",
				 tr(xc.audio_upmix ? "On" : "Off"));
      break;

    case HOTKEY_DOWNMIX:    
      /* off, on */
      if(!OnlyInfo) {
	xc.audio_surround = xc.audio_surround ? 0 : 1;
	cXinelibDevice::Instance().ConfigurePostprocessing(
	    xc.deinterlace_method, xc.audio_delay, xc.audio_compression, 
	    xc.audio_equalizer, xc.audio_surround, xc.speaker_type);
      }
      Message = cString::sprintf("%s %s %s", 
				 tr("Downmix AC3 to surround"), 
				 OnlyInfo ? ":" : "->",
				 tr(xc.audio_surround ? "On":"Off"));
      break;

      case HOTKEY_PLAYLIST:
	/* Start replaying playlist or file pointed by 
	   symlink $(CONFDIR)/plugins/xineliboutput/default_playlist */
	{
	  struct stat st;
	  cString file = cString::sprintf("%s%s", cPlugin::ConfigDirectory("xineliboutput"), "/default_playlist");
	  if (lstat(file, &st) == 0) {
	    if (S_ISLNK(st.st_mode)) {
	      cString buffer(ReadLink(file), true);
	      if (!*buffer || stat(buffer, &st)) {
		Message = tr("Default playlist not found");
	      } else {
		LOGDBG("Replaying default playlist: %s", *file);
		cControl::Shutdown();
		cControl::Launch(new cXinelibPlayerControl(CloseOsd, buffer));
	      }
	    } else {
	      Message = tr("Default playlist is not symlink");
	    }
	  } else {
	    Message = tr("Default playlist not defined");
	  }
	}
	break;

    case HOTKEY_ADELAY_UP:
      /* audio delay up */
      if(!OnlyInfo) {
	xc.audio_delay++;
        cXinelibDevice::Instance().ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay,
                                                           xc.audio_compression, xc.audio_equalizer,
                                                           xc.audio_surround, xc.speaker_type);
      }
      Message = cString::sprintf("%s %s %d %s", tr("Delay"),
				 OnlyInfo ? ":" : "->",
				 xc.audio_delay, tr("ms"));
      break;

    case HOTKEY_ADELAY_DOWN:
      /* audio delay up */
      if(!OnlyInfo) {
	xc.audio_delay--;
        cXinelibDevice::Instance().ConfigurePostprocessing(xc.deinterlace_method, xc.audio_delay,
                                                           xc.audio_compression, xc.audio_equalizer,
                                                           xc.audio_surround, xc.speaker_type);
      }
      Message = cString::sprintf("%s %s %d %s", tr("Delay"),
				 OnlyInfo ? ":" : "->",
				 xc.audio_delay, tr("ms"));
      break;

    default:
      Message = cString::sprintf(tr("xineliboutput: hotkey %s not binded"), cKey::ToString(Key));
      break;
  }

  if(*Message) {
    if(!g_PendingMenuAction &&
       !cRemote::HasKeys() &&
       cRemote::CallPlugin("xineliboutput"))
      g_PendingMenuAction = new cDisplayMessage(Message);
  }

  g_LastHotkeyTime = now;
  g_LastHotkey = Key;

  return NewState;
}
