/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/menu.h>
#include <vdr/remote.h>
#include <vdr/menu.h>
#include "femoncfg.h"
#include "femonreceiver.h"
#include "femonosd.h"
#include "femonservice.h"
#include "femontools.h"

#if defined(APIVERSNUM) && APIVERSNUM < 10713
#error "VDR-1.7.13 API version or greater is required!"
#endif

static const char VERSION[]       = "1.7.7";
static const char DESCRIPTION[]   = trNOOP("DVB Signal Information Monitor (OSD)");
static const char MAINMENUENTRY[] = trNOOP("Signal Information");

class cPluginFemon : public cPlugin {
public:
  cPluginFemon(void);
  virtual ~cPluginFemon();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void) {}
  virtual cString Active(void) { return NULL; }
  virtual const char *MainMenuEntry(void) { return (femonConfig.hidemenu ? NULL : tr(MAINMENUENTRY)); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginFemon::cPluginFemon()
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  debug("%s()\n", __PRETTY_FUNCTION__);
}

cPluginFemon::~cPluginFemon()
{
  // Clean up after yourself!
  debug("%s()\n", __PRETTY_FUNCTION__);
}

const char *cPluginFemon::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginFemon::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginFemon::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginFemon::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginFemon::Stop(void)
{
  // Stop the background activities.
}

void cPluginFemon::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginFemon::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  debug("%s()\n", __PRETTY_FUNCTION__);
  if (cReplayControl::NowReplaying() || (Channels.Count() <= 0))
     Skins.Message(mtInfo, tr("Femon not available"));
  else
     return cFemonOsd::Instance(true);
  return NULL;
}

bool cPluginFemon::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "HideMenu"))       femonConfig.hidemenu       = atoi(Value);
  else if (!strcasecmp(Name, "DisplayMode"))    femonConfig.displaymode    = atoi(Value);
  else if (!strcasecmp(Name, "Position"))       femonConfig.position       = atoi(Value);
  else if (!strcasecmp(Name, "Skin"))           femonConfig.skin           = atoi(Value);
  else if (!strcasecmp(Name, "Theme"))          femonConfig.theme          = atoi(Value);
  else if (!strcasecmp(Name, "Downscale"))      femonConfig.downscale      = atoi(Value);
  else if (!strcasecmp(Name, "RedLimit"))       femonConfig.redlimit       = atoi(Value);
  else if (!strcasecmp(Name, "GreenLimit"))     femonConfig.greenlimit     = atoi(Value);
  else if (!strcasecmp(Name, "UpdateInterval")) femonConfig.updateinterval = atoi(Value);
  else if (!strcasecmp(Name, "AnalStream"))     femonConfig.analyzestream  = atoi(Value);
  else if (!strcasecmp(Name, "CalcInterval"))   femonConfig.calcinterval   = atoi(Value);
  else if (!strcasecmp(Name, "UseSvdrp"))       femonConfig.usesvdrp       = atoi(Value);
  else if (!strcasecmp(Name, "ServerPort"))     femonConfig.svdrpport      = atoi(Value);
  else if (!strcasecmp(Name, "ServerIp"))       strn0cpy(femonConfig.svdrpip, Value, sizeof(femonConfig.svdrpip));
  else
    return false;
  if (femonConfig.displaymode < 0 || femonConfig.displaymode >= eFemonModeMaxNumber) femonConfig.displaymode = 0;

  return true;
}

bool cPluginFemon::Service(const char *Id, void *Data)
{
  if (strcmp(Id,"FemonService-v1.0") == 0) {
     if (Data) {
        FemonService_v1_0 *data = (FemonService_v1_0*)Data;
        int ndx = cDevice::ActualDevice()->CardIndex();
        data->fe_name = getFrontendName(ndx);
        data->fe_status = getFrontendStatus(ndx);
        data->fe_snr = getSNR(ndx);
        data->fe_signal = getSignal(ndx);
        data->fe_ber = getBER(ndx);
        data->fe_unc = getUNC(ndx);
        data->video_bitrate = cFemonOsd::Instance() ? cFemonOsd::Instance()->GetVideoBitrate() : 0.0;
        data->audio_bitrate = cFemonOsd::Instance() ? cFemonOsd::Instance()->GetAudioBitrate() : 0.0;
        data->dolby_bitrate = cFemonOsd::Instance() ? cFemonOsd::Instance()->GetDolbyBitrate() : 0.0;
        }
     return true;
     }

  return false;
}

const char **cPluginFemon::SVDRPHelpPages(void)
{ 
  static const char *HelpPages[] = {
    "OPEN\n"
    "    Open femon plugin.",
    "QUIT\n"
    "    Close femon plugin.",
    "NEXT\n"
    "    Switch to next possible device.",
    "PREV\n"
    "    Switch to previous possible device.",
    "INFO\n"
    "    Print the current frontend information.",
    "NAME\n"
    "    Print the current frontend name.",
    "STAT\n"
    "    Print the current frontend status.",
    "SGNL\n"
    "    Print the current signal strength.",
    "SNRA\n"
    "    Print the current signal-to-noise ratio.",
    "BERA\n"
    "    Print the current bit error rate.",
    "UNCB\n"
    "    Print the current uncorrected blocks rate.",
    "VIBR\n"
    "    Print the actual device and current video bitrate [Mbit/s].",
    "AUBR\n"
    "    Print the actual device and current audio bitrate [kbit/s].",
    "DDBR\n"
    "    Print the actual device and current dolby bitrate [kbit/s].",
    NULL
    };
  return HelpPages;
}

cString cPluginFemon::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if (strcasecmp(Command, "OPEN") == 0) {
     if (cReplayControl::NowReplaying()) {
        ReplyCode = 550; // Requested action not taken
        return cString("Cannot open femon plugin while replaying");
        }
     if (!cFemonOsd::Instance())
        cRemote::CallPlugin("femon");
     return cString("Opening femon plugin");
     }
  else if (strcasecmp(Command, "QUIT") == 0) {
     if (cFemonOsd::Instance())
        cRemote::Put(kBack);
     return cString("Closing femon plugin");
     }
  else if (strcasecmp(Command, "NEXT") == 0) {
     if (cFemonOsd::Instance())
        return cString::sprintf("Switching to next device: %s", cFemonOsd::Instance()->DeviceSwitch(1) ? "ok" : "failed");
     else
        return cString("Cannot switch device");
     }
  else if (strcasecmp(Command, "PREV") == 0) {
     if (cFemonOsd::Instance())
        return cString::sprintf("Switching to previous device: %s", cFemonOsd::Instance()->DeviceSwitch(-1) ? "ok" : "failed");
     else
        return cString("Cannot switch device");
     }
  else if (strcasecmp(Command, "INFO") == 0) {
     return getFrontendInfo(cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "NAME") == 0) {
     return getFrontendName(cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "STAT") == 0) {
     return getFrontendStatus(cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "SGNL") == 0) {
     int value = getSignal(cDevice::ActualDevice()->CardIndex());
     return cString::sprintf("%04X (%02d%%) on device #%d", value, value / 655, cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "SNRA") == 0) {
     int value = getSNR(cDevice::ActualDevice()->CardIndex());
     return cString::sprintf("%04X (%02d%%) on device #%d", value, value / 655, cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "BERA") == 0) {
     return cString::sprintf("%08X on device #%d", getBER(cDevice::ActualDevice()->CardIndex()), cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "UNCB") == 0) {
     return cString::sprintf("%08X on device #%d", getUNC(cDevice::ActualDevice()->CardIndex()), cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "VIBR") == 0) {
     if (cFemonOsd::Instance())
        return cString::sprintf("%s on device #%d", *getBitrateMbits(cFemonOsd::Instance()->GetVideoBitrate()), cDevice::ActualDevice()->CardIndex());
     else
        return cString::sprintf("--- Mbit/s on device #%d", cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "AUBR") == 0) {
     if (cFemonOsd::Instance())
        return cString::sprintf("%s on device #%d", *getBitrateKbits(cFemonOsd::Instance()->GetAudioBitrate()), cDevice::ActualDevice()->CardIndex());
     else
        return cString::sprintf("--- kbit/s on device #%d", cDevice::ActualDevice()->CardIndex());
     }
  else if (strcasecmp(Command, "DDBR") == 0) {
     if (cFemonOsd::Instance())
        return cString::sprintf("%s on device #%d", *getBitrateKbits(cFemonOsd::Instance()->GetDolbyBitrate()), cDevice::ActualDevice()->CardIndex());
     else
        return cString::sprintf("--- kbit/s on device #%d", cDevice::ActualDevice()->CardIndex());
     }
  return NULL;
}

class cMenuFemonSetup : public cMenuSetupPage {
private:
  const char *dispmodes[eFemonModeMaxNumber];
  const char *skins[eFemonSkinMaxNumber];
  const char *themes[eFemonThemeMaxNumber];
  cFemonConfig data;
  cVector<const char*> help;
  void Setup(void);
protected:
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Store(void);
public:
  cMenuFemonSetup(void);
  };

cMenuFemonSetup::cMenuFemonSetup(void)
{
  debug("%s()\n", __PRETTY_FUNCTION__);
  dispmodes[eFemonModeBasic]       = tr("basic");
  dispmodes[eFemonModeTransponder] = tr("transponder");
  dispmodes[eFemonModeStream]      = tr("stream");
  dispmodes[eFemonModeAC3]         = tr("AC-3");

  skins[eFemonSkinClassic]         = tr("Classic");
  skins[eFemonSkinElchi]           = tr("Elchi");

  themes[eFemonThemeClassic]       = tr("Classic");
  themes[eFemonThemeElchi]         = tr("Elchi");
  themes[eFemonThemeSTTNG]         = tr("ST:TNG");
  themes[eFemonThemeDeepBlue]      = tr("DeepBlue");
  themes[eFemonThemeMoronimo]      = tr("Moronimo");
  themes[eFemonThemeEnigma]        = tr("Enigma");
  themes[eFemonThemeEgalsTry]      = tr("EgalsTry");
  themes[eFemonThemeDuotone]       = tr("Duotone");
  themes[eFemonThemeSilverGreen]   = tr("SilverGreen");

  data = femonConfig;
  Setup();
}

void cMenuFemonSetup::Setup(void)
{
  int current = Current();

  Clear();
  help.Clear();

  Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &data.hidemenu));
  help.Append(tr("Define whether the main menu entry is hidden."));

  Add(new cMenuEditStraItem(tr("Default display mode"), &data.displaymode, eFemonModeMaxNumber, dispmodes));
  help.Append(tr("Define the default display mode at startup."));

  Add(new cMenuEditStraItem(trVDR("Setup.OSD$Skin"), &data.skin, eFemonSkinMaxNumber, skins));
  help.Append(tr("Define the used OSD skin."));

  Add(new cMenuEditStraItem(trVDR("Setup.OSD$Theme"), &data.theme, eFemonThemeMaxNumber,themes));
  help.Append(tr("Define the used OSD theme."));

  Add(new cMenuEditBoolItem(tr("Position"), &data.position, trVDR("bottom"), trVDR("top")));
  help.Append(tr("Define the position of OSD."));

  Add(new cMenuEditIntItem(tr("Downscale OSD size [%]"), &data.downscale, 0, 20));
  help.Append(tr("Define the downscale ratio for OSD size."));

  Add(new cMenuEditIntItem(tr("Red limit [%]"), &data.redlimit, 1, 50));
  help.Append(tr("Define a limit for red bar, which is used to indicate a bad signal."));

  Add(new cMenuEditIntItem(tr("Green limit [%]"), &data.greenlimit, 51, 100));
  help.Append(tr("Define a limit for green bar, which is used to indicate a good signal."));

  Add(new cMenuEditIntItem(tr("OSD update interval [0.1s]"), &data.updateinterval, 1, 100));
  help.Append(tr("Define an interval for OSD updates. The smaller interval generates higher CPU load."));

  Add(new cMenuEditBoolItem(tr("Analyze stream"), &data.analyzestream));
  help.Append(tr("Define whether the DVB stream is analyzed and bitrates calculated."));

  if (femonConfig.analyzestream) {
     Add(new cMenuEditIntItem(tr("Calculation interval [0.1s]"), &data.calcinterval, 1, 100));
     help.Append(tr("Define an interval for calculation. The bigger interval generates more stable values."));
     }

  Add(new cMenuEditBoolItem(tr("Use SVDRP service"), &data.usesvdrp));
  help.Append(tr("Define whether the SVDRP service is used in client/server setups."));

  if (data.usesvdrp) {
     Add(new cMenuEditIntItem(tr("SVDRP service port"), &data.svdrpport, 1, 65535));
     help.Append(tr("Define the port number of SVDRP service."));

     Add(new cMenuEditStrItem(tr("SVDRP service IP"), data.svdrpip, MaxSvdrpIp, ".1234567890"));
     help.Append(tr("Define the IP address of SVDRP service."));
     }

  SetCurrent(Get(current));
  Display();
}

void cMenuFemonSetup::Store(void)
{
  debug("%s()\n", __PRETTY_FUNCTION__);
  femonConfig = data;
  SetupStore("HideMenu",       femonConfig.hidemenu);
  SetupStore("DisplayMode",    femonConfig.displaymode);
  SetupStore("Skin",           femonConfig.skin);
  SetupStore("Theme",          femonConfig.theme);
  SetupStore("Position",       femonConfig.position);
  SetupStore("Downscale",      femonConfig.downscale);
  SetupStore("RedLimit",       femonConfig.redlimit);
  SetupStore("GreenLimit",     femonConfig.greenlimit);
  SetupStore("UpdateInterval", femonConfig.updateinterval);
  SetupStore("AnalStream",     femonConfig.analyzestream);
  SetupStore("CalcInterval",   femonConfig.calcinterval);
  SetupStore("UseSvdrp",       femonConfig.usesvdrp);
  SetupStore("ServerPort",     femonConfig.svdrpport);
  SetupStore("ServerIp",       femonConfig.svdrpip);
}

eOSState cMenuFemonSetup::ProcessKey(eKeys Key)
{
  int oldUsesvdrp = data.usesvdrp;
  int oldAnalyzestream = data.analyzestream;

  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (Key != kNone && (data.analyzestream != oldAnalyzestream || data.usesvdrp != oldUsesvdrp))
     Setup();

  if ((Key == kInfo) && (state == osUnknown) && (Current() < help.Size()))
     return AddSubMenu(new cMenuText(cString::sprintf("%s - %s '%s'", tr("Help"), trVDR("Plugin"), PLUGIN_NAME_I18N), help[Current()]));

  return state;
}

cMenuSetupPage *cPluginFemon::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuFemonSetup;
}

VDRPLUGINCREATOR(cPluginFemon); // Don't touch this!
