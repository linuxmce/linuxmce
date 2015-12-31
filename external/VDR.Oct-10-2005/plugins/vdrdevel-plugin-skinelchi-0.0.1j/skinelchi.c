/*
 * $Id:
 */
#ifdef HAVE_IMAGEMAGICK
#include <getopt.h>
#ifdef HAVE_C295
#include <stl.h>
#endif
#endif
#include "setup.h"
#include "DisplayChannel.h"
#include "DisplayMenu.h"
#include "DisplayReplay.h"
#include "DisplayVolume.h"
#include "DisplayTracks.h"
#include "DisplayMessage.h"
#include "skinelchi.h"
#include "i18n.h"
#ifdef HAVE_IMAGEMAGICK
#include "bitmap.h"
#endif
#include <vdr/font.h>
#include <vdr/interface.h>
#include <vdr/osd.h>
#include <vdr/menu.h>
#include <vdr/themes.h>
#include <vdr/plugin.h>

static const char *VERSION	= "0.0.1j";
static const char *DESCRIPTION	= "Elchi VDR Skin-Plugin";
const char *OSDSKIN		= "Elchi_Plugin";

#ifdef VIDEOSTATUS
video_status videostatus;
int fd_video;
video_size_t vs;
#endif

// --- cSkinElchi ----------------------------------------------------------

cSkinElchi::cSkinElchi(void)
:cSkin(OSDSKIN, &::Theme)//XXX naming problem???
{
}

const char *cSkinElchi::Description(void)
{
  return tr("Elchi (Plugin)");
}

cSkinDisplayChannel *cSkinElchi::DisplayChannel(bool WithInfo)
{
  return new cSkinElchiDisplayChannel(WithInfo);
}

cSkinDisplayMenu *cSkinElchi::DisplayMenu(void)
{
  return new cSkinElchiDisplayMenu;
}

cSkinDisplayReplay *cSkinElchi::DisplayReplay(bool ModeOnly)
{
  return new cSkinElchiDisplayReplay(ModeOnly);
}

cSkinDisplayVolume *cSkinElchi::DisplayVolume(void)
{
  return new cSkinElchiDisplayVolume;
}


cSkinDisplayTracks *cSkinElchi::DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks)
{
  return new cSkinElchiDisplayTracks(Title, NumTracks, Tracks);
}

cSkinDisplayMessage *cSkinElchi::DisplayMessage(void)
{
  return new cSkinElchiDisplayMessage;
}

// --- cPluginSkinElchi -----------------------------------------------------

class cPluginSkinElchi : public cPlugin {
public:
  cPluginSkinElchi(void);
  virtual ~cPluginSkinElchi();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return ((!SkinElchiSetup.hidemenu || strcasecmp(OSDSKIN, Setup.OSDSkin)) ? NULL :SkinElchiSetup.menuentry); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
};

cPluginSkinElchi::cPluginSkinElchi(void)
{
}

cPluginSkinElchi::~cPluginSkinElchi()
{
#ifdef VIDEOSTATUS
  if (fd_video != -1)
    close(fd_video);
#endif
}

const char *cPluginSkinElchi::CommandLineHelp(void)
{
#ifdef HAVE_IMAGEMAGICK
  return "  -c DIR  --icache=DIR cache converted images in DIR\n"
         "                       (default: '/video/epgimages')\n"
         "                       (if use only show setting in setup)\n";
#else
  return NULL;
#endif
}

bool cPluginSkinElchi::ProcessArgs(int argc, char *argv[])
{
#ifdef HAVE_IMAGEMAGICK
  static struct option long_options[] = {
      { "icache", required_argument, NULL, 'c' },
      { NULL }
    };
  int c, option_index = 0;
  while ((c = getopt_long(argc, argv, "c:", long_options, &option_index)) != -1) {
    switch (c) {
      case 'c': strcpy(SkinElchiSetup.epgimagedir, optarg);
		SkinElchiSetup.commandlinedir = true;
#ifdef DEBUG
    		isyslog("skinelchi: epgimage-Directory '%s'", SkinElchiSetup.epgimagedir);
#endif
    		break;
      default:  
#ifdef DEBUG
    		isyslog("skinelchi: unknown command-line argument");
#endif
    		break;
      }
    }
#endif
  return true;
}

bool cPluginSkinElchi::Initialize(void)
{
  return true;
}

bool cPluginSkinElchi::Start(void)
{
#ifdef VIDEOSTATUS
    fd_video = - 1;
#endif
    RegisterI18n(Phrases);
    Theme.AddColor("clrBackground",         	0x77000066);//  0
    Theme.AddColor("clrButtonRedFg",        	clrWhite);  //  1
    Theme.AddColor("clrButtonRedBg",        	0xCCCC1111);//  2
    Theme.AddColor("clrButtonGreenFg",      	clrBlack);  //  3
    Theme.AddColor("clrButtonGreenBg",      	0xCC22BB22);//  4
    Theme.AddColor("clrButtonYellowFg",     	clrBlack);  //  5
    Theme.AddColor("clrButtonYellowBg",     	0xCCEEEE22);//  6
    Theme.AddColor("clrButtonBlueFg",       	clrWhite);  //  7
    Theme.AddColor("clrButtonBlueBg",       	0xCC2233CC);//  8
    Theme.AddColor("clrMessageStatusFg",    	clrBlack);  //  9
    Theme.AddColor("clrMessageStatusBg",    	0xCC2BA7F1);// 10
    Theme.AddColor("clrMessageInfoFg",      	clrBlack);  // 11
    Theme.AddColor("clrMessageInfoBg",      	0xCC22BB22);// 12
    Theme.AddColor("clrMessageWarningFg",   	clrBlack);  // 13
    Theme.AddColor("clrMessageWarningBg",   	clrYellow); // 14
    Theme.AddColor("clrMessageErrorFg",     	clrWhite);  // 15
    Theme.AddColor("clrMessageErrorBg",     	clrRed);    // 16
    Theme.AddColor("clrVolumePrompt",       	clrWhite);  // 17
    Theme.AddColor("clrVolumeBarUpper",     	0xFFCCBB22);// 18
    Theme.AddColor("clrVolumeBarLower",     	0x77000066);// 19
    Theme.AddColor("clrVolumeSymbolMuteBg", 	clrWhite);  // 20
    Theme.AddColor("clrVolumeSymbolMuteFg", 	0xCCCC1111);// 21
    Theme.AddColor("clrVolumeSymbolVolumeBg",   0x77000066);// 22
    Theme.AddColor("clrVolumeSymbolVolumeFg",   0xFFCCBB22);// 23
    Theme.AddColor("clrChannelNameFg",      	clrWhite);  // 24
    Theme.AddColor("clrChannelNameBg",      	0xCC2BA7F1);// 25
    Theme.AddColor("clrChannelNumberFg",    	clrBlack);  // 26
    Theme.AddColor("clrChannelNumberBg",    	clrWhite);  // 27 - currently not used
    Theme.AddColor("clrChannelDateFg",      	clrBlack);  // 28
    Theme.AddColor("clrChannelDateBg",      	clrWhite);  // 29
    Theme.AddColor("clrChannelEpgTimeFg",   	0xFFDDDDDD);// 30
    Theme.AddColor("clrChannelEpgTimeBg",   	0xAABB0000);// 31
    Theme.AddColor("clrChannelEpgTitle",    	0xFF00FCFC);// 32
    Theme.AddColor("clrChannelEpgTitleBg",  	0x88000000);// 33
    Theme.AddColor("clrChannelEpgShortText",	0xFFCCBB22);// 34
    Theme.AddColor("clrChannelSymbolOn",    	clrYellow); // 35
    Theme.AddColor("clrChannelSymbolOff",   	0x77777777);// 36
    Theme.AddColor("clrChannelSymbolRecFg", 	clrWhite);  // 37
    Theme.AddColor("clrChannelSymbolRecBg", 	clrRed);    // 38
    Theme.AddColor("clrChannelTimebarSeen", 	clrYellow); // 39
    Theme.AddColor("clrChannelTimebarRest", 	clrGray50); // 40
    Theme.AddColor("clrMenuTitleFg",        	clrBlack);  // 41
    Theme.AddColor("clrMenuTitleBg",        	0xCC2BA7F1);// 42
    Theme.AddColor("clrMenuDate",           	clrBlack);  // 43
    Theme.AddColor("clrMenuItemCurrentFg",  	clrBlack);  // 44
    Theme.AddColor("clrMenuItemCurrentBg",  	0xCC2BA7F1);// 45
    Theme.AddColor("clrMenuItemSelectable", 	clrWhite);  // 46
    Theme.AddColor("clrMenuItemNonSelectable",  0xCC2BA7F1);// 47
    Theme.AddColor("clrMenuEventTime",      	clrWhite);  // 48
    Theme.AddColor("clrMenuEventVpsFg",     	clrBlack);  // 49
    Theme.AddColor("clrMenuEventVpsBg",     	clrWhite);  // 50
    Theme.AddColor("clrMenuEventTitle",     	clrYellow); // 51
    Theme.AddColor("clrMenuEventShortText", 	clrWhite);  // 52
    Theme.AddColor("clrMenuEventDescription",   clrYellow); // 53
    Theme.AddColor("clrMenuScrollbarTotal", 	clrWhite);  // 54
    Theme.AddColor("clrMenuScrollbarShown", 	clrYellow); // 55
    Theme.AddColor("clrMenuText",           	clrWhite);  // 56
    Theme.AddColor("clrReplayTitleFg",      	clrBlack);  // 57
    Theme.AddColor("clrReplayTitleBg",      	clrWhite);  // 58
    Theme.AddColor("clrReplayCurrent",      	clrWhite);  // 59
    Theme.AddColor("clrReplayTotal",        	clrWhite);  // 60
    Theme.AddColor("clrReplayModeJump",     	clrWhite);  // 61
    Theme.AddColor("clrReplayProgressSeen", 	0xCC22BB22);// 62
    Theme.AddColor("clrReplayProgressRest", 	clrWhite);  // 63
    Theme.AddColor("clrReplayProgressSelected", 0xCCCC1111);// 64
    Theme.AddColor("clrReplayProgressMark", 	clrBlack);  // 65
    Theme.AddColor("clrReplayProgressCurrent",  0xCCCC1111);// 66
    Theme.AddColor("clrReplaySymbolOn",     	clrYellow); // 67
    Theme.AddColor("clrReplaySymbolOff",    	0x77777777);// 68
    Theme.AddColor("Transparent",	    	0x01000000);// 69
    Theme.AddColor("changing",	    	    	0x00000000);// 70

  char *filename;
  asprintf(&filename, "%s/../../themes/%s%s", cPlugin::ConfigDirectory("skinelchi"), OSDSKIN,"-change.theme");
  Theme.Load(filename);
  free(filename);

  for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++) {
    SkinElchiSetup.ThemeColors[i] = Theme.Color(i);
    }
  new cSkinElchi;
  return true;
}

void cPluginSkinElchi::Housekeeping(void)
{
}

cOsdObject *cPluginSkinElchi::MainMenuAction(void)
{
  if (SkinElchiSetup.hidemenu) {
    if (SkinElchiSetup.hidemenu == 1)
	return new cSkinElchiSetupPage;
    else {
#ifdef HAVE_YAEPG_PATCH
	if (SkinElchiSetup.vidwin) {
	    SkinElchiSetup.vidwin = 0;
	    Skins.Message(mtInfo, tr("Menu without video-window"));
	    }
	else {
	    SkinElchiSetup.vidwin = 1;
	    Skins.Message(mtInfo, tr("Menu with video-window"));
	    }
#endif
	return NULL;
	}
    }
  else
    return new cSkinElchiSetupPage;
}

cMenuSetupPage *cPluginSkinElchi::SetupMenu(void)
{
  return new cSkinElchiSetupPage;
}

bool cPluginSkinElchi::SetupParse(const char *Name, const char *Value)
{
  return SkinElchiSetup.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cPluginSkinElchi); // Don't touch this!

