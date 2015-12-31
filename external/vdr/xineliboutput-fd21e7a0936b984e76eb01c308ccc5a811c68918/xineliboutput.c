/*
 * vdr-xineliboutput: xine-lib based output device plugin for VDR
 *
 * Copyright (C) 2003-2008 Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 *
 * xineliboutput.c: VDR Plugin interface
 *
 * $Id: xineliboutput.c,v 1.61 2014-01-31 08:58:30 rofafor Exp $
 *
 */

#include "features.h"

#include <vdr/plugin.h>
#include <vdr/i18n.h>

#include "logdefs.h"
#include "config.h"
#include "device.h"
#include "setup_menu.h"
#include "menu.h"
#include "media_player.h"

#if defined(APIVERSNUM) && (APIVERSNUM < 10733)
# error VDR API versions < 1.7.33 are not supported !
#endif

//---------------------------------plugin-------------------------------------

static const char *VERSION        = "2.0.0-cvs";
static const char *DESCRIPTION    = trNOOP("X11/xine-lib output plugin");
static const char *MAINMENUENTRY  = trNOOP("Media Player");

class cPluginXinelibOutput : public cPlugin
{
  private:
    // Add any member variables or functions you may need here.
    cXinelibDevice *m_Dev;
    int             m_MakePrimary;

  public:
    cPluginXinelibOutput(void);
    virtual ~cPluginXinelibOutput();

    virtual const char *Version(void) { return VERSION; }
    virtual const char *Description(void) { return tr(DESCRIPTION); }
    virtual const char *CommandLineHelp(void);

    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    //virtual void Housekeeping(void);
    virtual void MainThreadHook();
    //virtual cString Active(void);
    //virtual time_t WakeupTime(void);

    virtual const char *MainMenuEntry(void) { return xc.hide_main_menu ? NULL : tr(MAINMENUENTRY); }
    virtual cOsdObject *MainMenuAction(void);

    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);

    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginXinelibOutput::cPluginXinelibOutput(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!

  m_Dev = NULL;
  m_MakePrimary = 0;
}

cPluginXinelibOutput::~cPluginXinelibOutput()
{
  // Clean up after yourself!

  if (m_Dev) {
    cXinelibDevice::Dispose();
  }
}


const char cmdLineHelp[] =
"  -l NAME   --local=NAME   Use local frontend NAME\n"
"                           Supported frontends:\n"
"                           sxfe    (X11)\n"
"                           fbfe    (framebuffer)\n"
"                           none    (only remote frontends)\n"
"  -r PORT   --remote=PORT  Listen PORT for remote clients\n"
"                           (default "LISTEN_PORT_S")\n"
"                           none or 0 disables remote mode\n"
"                           Also local interface address can be specified:\n"
"                           --remote=<ip>:<port>  (default is all interfaces)\n"
"  -A NAME   --audio=NAME   Use audio driver NAME for local frontend\n"
"                           Supported values:\n"
"                           auto, alsa, oss, esound, none\n"
"  -V NAME   --video=NAME   Use video driver NAME for local frontend\n"
"                           Supported values:\n"
"                           for sxfe: auto, x11, xshm, xv, xvmc, xxmc,\n"
#ifdef HAVE_VDPAU
"                                     vdpau, "
#endif
"                                     vaapi, vidix, sdl, opengl, none\n"
"                           for fbfe: auto, fb, DirectFB, vidixfb,\n"
"                                     sdl, dxr3, aadxr3, none\n"
#if 0
"  -m M      --modeline=M   Use modeline M for local frontend\n"
"                           (example: )\n" 
#endif
"  -f        --fullscreen   Fullscreen mode (X11)\n"
#if defined(HAVE_XRENDER) || defined(HAVE_OPENGL)
"  -D        --hud[=flag[,flag]]\n"
"                           Head Up Display OSD (X11)\n"
"                           flags:\n"
#endif
#if defined(HAVE_XRENDER) && defined(HAVE_XSHAPE)
"                           xshape  Use XShape instead of compositing\n"
#endif
#ifdef HAVE_OPENGL
"                           opengl  Use OpenGL instead of compositing\n"
"  -O        --opengl       Use OpenGL for video and Head Up Display OSD\n"
#endif
"  -w        --width=x      Window width\n"
"  -h        --height=x     Window width\n"
"  -g,       --geometry=WxH[+X+Y] Set output window geometry (X style)\n"
"  -d DISP   --display=DISP Use X11 display DISP\n"
"                           (or framebuffer device name)\n"
"  -W ID     --wid=ID       Use existing X11 window\n"
"                           Special ID for root window: --wid=root\n"
#ifdef HAVE_XRANDR
"  -m        --modeswitch   Enable video mode switching\n"
#endif
"  -P NAME   --post=NAME    Use xine post plugin NAME\n"
"                           format: pluginname[:arg=val[,arg=val]][,...]\n"
"                           example: \n"
"                           --post=upmix;tvtime:enabled=1,cheap_mode=1\n"
"  -p        --primary      Force xineliboutput to be primary device when\n"
"                           there are active frontend(s)\n"
"  -c        --exit-on-close  Exit vdr when local frontend window is closed\n"
"  -C        --config=file  Use xine-lib config file\n"
;

const char *cPluginXinelibOutput::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return cmdLineHelp;
}

bool cPluginXinelibOutput::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return xc.ProcessArgs(argc, argv);
}

bool cPluginXinelibOutput::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  TRACEF("cPluginXinelibOutput::Initialize");

  m_Dev = &(cXinelibDevice::Instance());

  return m_Dev ? m_Dev->InitDevice() : false;
}

bool cPluginXinelibOutput::Start(void)
{
  // Start any background activities the plugin shall perform.
  TRACEF("cPluginXinelibOutput::Start");

  return m_Dev ? m_Dev->StartDevice() : false;
}

void cPluginXinelibOutput::MainThreadHook(void)
{
  TRACEF("cPluginXinelibOutput::MainThreadHook");

  if (m_MakePrimary) {
     LOGDBG("Switching primary device to %d", m_MakePrimary);
     cDevice::SetPrimaryDevice(m_MakePrimary);
     m_MakePrimary = 0;
     }

  if (m_Dev) {
    m_Dev->MainThreadHook();
  }
}

void cPluginXinelibOutput::Stop(void)
{
  // Start any background activities the plugin shall perform.
  TRACEF("cPluginXinelibOutput::Stop");

  if (m_Dev) {
    m_Dev->StopDevice();
  }
}

cOsdObject *cPluginXinelibOutput::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  TRACEF("cPluginXinelibOutput::MainMenuAction");

  if (!m_Dev) {
    return NULL;
  }
  if (xc.main_menu_mode == CloseOsd) {
    xc.main_menu_mode = ShowMenu;
    return NULL;
  }

  if (xc.pending_menu_action) {
    cOsdObject *tmp = xc.pending_menu_action;
    xc.pending_menu_action = NULL;
    return tmp;
  }

  if (xc.hide_main_menu)
    return NULL;

  return m_Dev ? new cMenuXinelib(m_Dev) : NULL;
}

cMenuSetupPage *cPluginXinelibOutput::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  TRACEF("cPluginXinelibOutput::SetupMenu");

  return m_Dev ? new cMenuSetupXinelib(m_Dev) : NULL;
}

bool cPluginXinelibOutput::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return xc.SetupParse(Name, Value);
}

bool cPluginXinelibOutput::Service(const char *Id, void *Data)
{
  if(Id) {
    char *CData = (char*)Data;

    if(!strcmp(Id, "MediaPlayer-1.0")) {
      if(CData && *CData) {
        LOGMSG("Service(%s, %s)", Id, CData);
        cPlayerFactory::Launch(m_Dev, pmAudioVideo, CData);
        return true;
      }
      LOGMSG("Service(%s) -> true", Id);
      return true;
    }

    else if(!strcmp(Id, "MusicPlayer-1.0")) {
      if(CData && *CData) {
        LOGMSG("Service(%s, %s)", Id, CData);
        cPlayerFactory::Launch(m_Dev, pmAudioOnly, CData);
        return true;
      }
      LOGMSG("Service(%s) -> true", Id);
      return true;
    }

    else if(!strcmp(Id, "DvdPlayer-1.0")) {
      if(Data && *CData) {
        LOGMSG("Service(%s, %s)", Id, CData);
        cPlayerFactory::Launch(m_Dev, pmNone, CData);
        return true;
      }
      LOGMSG("Service(%s) -> true", Id);
      return true;
    }

    else if(!strcmp(Id, "ImagePlayer-1.0")) {
      if(CData && *CData) {
        LOGMSG("Service(%s, %s)", Id, CData);
        cPlayerFactory::Launch(m_Dev, pmVideoOnly, CData);
        return true;
      }
      LOGMSG("Service(%s) -> true", Id);
      return true;
    }

    else if(!strcmp(Id, "StartFrontend-1.0")) {
      if(CData && *CData) {
        LOGMSG("Service(%s, %s)", Id, CData);
        int local_frontend = strstra(CData, xc.s_frontends, -1);
        if (local_frontend >= 0 && local_frontend < FRONTEND_count && strcmp(CData, xc.local_frontend)) {
          strn0cpy(xc.local_frontend, xc.s_frontends[local_frontend], sizeof(xc.local_frontend));
          m_Dev->ConfigureWindow(
               xc.fullscreen, xc.width, xc.height, xc.modeswitch, xc.modeline,
               xc.display_aspect, xc.scale_video);
        }
        return true;
      }
      LOGMSG("Service(%s) -> true", Id);
      return true;
    }

  }
  return false;
}

const char **cPluginXinelibOutput::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "PMDA <file>\n"
    "    Play media file.",
    "PDVD <file>\n"
    "    Play DVD disc.",
    "PMSC <file>\n"
    "    Play music file.",
    "PIMG <file>\n"
    "    Play/show image file.",
    "QMSC <file>\n"
    "    Queue music file to playlist.",
    "LFRO <frontend>\n"
    "    Start/stop local frontend. <frontend> can be none, sxfe or fbfe.",
    "PRIM <n>\n"
    "    Make <n> the primary device. If <n> is missing,\n"
    "    xineliboutput will become the primary device.",
    NULL
    };
  return HelpPages;
}

cString cPluginXinelibOutput::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if(strcasecmp(Command, "PMDA") == 0) {
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      cPlayerFactory::Launch(m_Dev, pmAudioVideo, Option);
      return cString("Playing video file");
    } else {
      ReplyCode = 550; // Requested action not taken
      return cString("File name missing");
    }
  }

  else if(strcasecmp(Command, "PDVD") == 0) {
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      cPlayerFactory::Launch(m_Dev, pmNone, Option);
    } else {
      cPlayerFactory::Launch(m_Dev, pmNone, "dvd:/");
    }
    return cString("Playing DVD disc");
  }

  else if(strcasecmp(Command, "PMSC") == 0) {
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      cPlayerFactory::Launch(m_Dev, pmAudioOnly, Option);
      return cString("Playing music file");
    } else {
      ReplyCode = 550; // Requested action not taken
      return cString("Music file name missing");
    }
  }

  else if(strcasecmp(Command, "PIMG") == 0) {
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      cPlayerFactory::Launch(m_Dev, pmVideoOnly, Option);
      return cString("Showing image file");
    } else {
      ReplyCode = 550; // Requested action not taken
      return cString("Image file name missing");
    }
  }

  else if(strcasecmp(Command, "QMSC") == 0) {
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      cPlayerFactory::Queue(m_Dev, Option);
      return cString("Queueing music file");
    } else {
      ReplyCode = 550; // Requested action not taken
      return cString("Music file name missing");
    }
  }

  else if(strcasecmp(Command, "LFRO") == 0) {
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      Service("StartFrontend-1.0", (void*)Option);
      return cString::sprintf("Local frontend: %s", xc.local_frontend);
    } else {
      ReplyCode = 550; // Requested action not taken
      return cString("Local frontend name missing");
    }
  }

  else if(strcasecmp(Command, "PRIM") == 0) {
    int primary = 0;
    if(*Option) {
      LOGMSG("SVDRP(%s, %s)", Command, Option);
      primary = strtol(Option, NULL, 0);
    } else {
      LOGMSG("SVDRP(%s)", Command);
    }
    if(!primary && m_Dev)
        primary = m_Dev->DeviceNumber() + 1;
    m_MakePrimary = primary;
    return cString::sprintf("Switching primary device to %d", primary);
  }

  return NULL;
}

extern "C"
void *VDRPluginCreator(void) __attribute__((visibility("default")));

VDRPLUGINCREATOR(cPluginXinelibOutput); // Don't touch this!
