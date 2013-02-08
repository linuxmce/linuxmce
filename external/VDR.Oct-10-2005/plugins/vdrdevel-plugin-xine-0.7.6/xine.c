/*
 * xine.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "xineCommon.h"

#include <vdr/plugin.h>

#include "xineDevice.h"
#include "xineSettings.h"
#include "xineSetupPage.h"
#include "xineI18n.h"



static const char *VERSION        = "0.7.6";
static const char *DESCRIPTION    = "Software based playback using xine";
//static const char *MAINMENUENTRY  = "xine - Toggle prebuffer setting";



class cPluginXine : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  PluginXine::cXineSettings m_settings;

  PluginXine::cXineRemote *m_remote;
  bool m_remoteOn;
  
public:
  PluginXine::cXineLib *m_xineLib;
  int m_instanceNo;
  
  cPluginXine(void);
  virtual ~cPluginXine();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);// { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginXine::cPluginXine(void)
  : cPlugin()
  , m_remote(0)
  , m_remoteOn(false)
  , m_xineLib(0)
  , m_instanceNo(-1)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginXine::~cPluginXine()
{
  // Clean up after yourself!
}

const char *cPluginXine::CommandLineHelp(void)
{
  //Return a string that describes all known command line options.
  //"  -         --             x                                                   \n"
  return
    "  -i N                     instance number to append to FIFO directory\n"
    "  -q                       turn off debug messages on console\n"
    "  -r                       turn on remote (pressing keys in xine controls VDR)\n"
    "  -s                       switch to curses skin, while xine is disconnected\n"
    ;
}

bool cPluginXine::ProcessArgs(int argc, char *argv[])
{
  ::optind = 0;
  ::opterr = 0;

  for (int r = -1; (r = ::getopt(argc, argv, "i:qrs")) >= 0; )
  {
    switch (r)
    {
    case 'i':
      {
        const int no = ::atoi(::optarg);
        if (no < 0)
          return false;
        
        m_instanceNo = no;
      }
      break;
      
    case 'r':
      m_remoteOn = true;
      break;
      
    case 's':
      m_settings.SetSwitchSkin(true);
      break;

    case 'q':
      m_settings.SetBeQuiet(true);
      break;
      
    default:
      return false;
    }
  }
  
  return true;
}

bool cPluginXine::Initialize(void)
{
  RegisterI18n(PluginXine::Phrases);
  
  // Initialize any background activities the plugin shall perform.
  m_remote = new PluginXine::cXineRemote(m_remoteOn);
  if (!m_remote)
    return false;
  
  if (!PluginXine::cXineDevice::Create(this, m_settings, m_remote))
    return false;

  return true;
}

bool cPluginXine::Start(void)
{
  // Start any background activities the plugin shall perform.
  if (!PluginXine::cXineDevice::Open())
    return false;

  return true;
}

void cPluginXine::Stop(void)
{
  PluginXine::cXineDevice::Stop();
}

void cPluginXine::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginXine::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  m_settings.TogglePrebufferMode();
      
  return NULL;
}

cMenuSetupPage *cPluginXine::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new PluginXine::cXineSetupPage(m_xineLib, m_settings);
}

bool cPluginXine::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return m_settings.SetupParse(Name, Value);
}

const char *cPluginXine::MainMenuEntry(void)
{
//  return m_settings.GetMainMenuEntry();
  return 0;
}

namespace PluginXine
{
  int GetInstanceNo(cPlugin *const plugin)
  {
    return ((cPluginXine *)plugin)->m_instanceNo;
  }

  cXineLib *&GetXineLib(cPlugin *const plugin)
  {
    return ((cPluginXine *)plugin)->m_xineLib;
  }
}



VDRPLUGINCREATOR(cPluginXine); // Don't touch this!
