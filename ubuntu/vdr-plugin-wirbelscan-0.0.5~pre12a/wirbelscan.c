/*
 * wirbelscan.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "menusetup.h"
#if VDRVERSNUM < 10507
#include "i18n.h"
#endif

#include "wirbelscan.h"
#include "wirbelscan_services.h"
#include "countries.h"
#include "satellites.h"

static const char *VERSION        = "0.0.5-pre12a";
static const char *DESCRIPTION    = "DVB and pvrinput channel scan for VDR";
static const char *MAINMENUENTRY  = NULL; /* don't like a main menu for stupid plugins.. */

const char * extVer  = VERSION;
const char * extDesc = DESCRIPTION;

cPluginWirbelscan * thisPlugin;

const char *cPluginWirbelscan::Version(void)
{
  return VERSION;
}

const char *cPluginWirbelscan::Description(void)
{
  return tr(DESCRIPTION);
}

const char *cPluginWirbelscan::MainMenuEntry(void)
{
  return MAINMENUENTRY;
}

cPluginWirbelscan::cPluginWirbelscan(void)
{
  thisPlugin = this;
}

cPluginWirbelscan::~cPluginWirbelscan()
{
  // Clean up after yourself!
}

const char *cPluginWirbelscan::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginWirbelscan::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginWirbelscan::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
  RegisterI18n(wirbelscan_Phrases); /* initialize phrases for internationalization */
#endif
  return true;
}

bool cPluginWirbelscan::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginWirbelscan::Stop(void)
{
  // Stop any background activities the plugin shall perform.
  stopScanners();
}

void cPluginWirbelscan::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginWirbelscan::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginWirbelscan::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginWirbelscan::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginWirbelscan::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuScanning();
}

bool cPluginWirbelscan::SetupParse(const char *Name, const char *Value)
{
  if      (!strcasecmp(Name, "verbosity"))       Wirbelscan.verbosity=atoi(Value);
  else if (!strcasecmp(Name, "logFile"))         Wirbelscan.logFile=atoi(Value);
  else if (!strcasecmp(Name, "DVB_Type"))        Wirbelscan.DVB_Type=(scantype_t) atoi(Value);
  else if (!strcasecmp(Name, "DVBT_Inversion"))  Wirbelscan.DVBT_Inversion=atoi(Value);
  else if (!strcasecmp(Name, "DVBC_Inversion"))  Wirbelscan.DVBC_Inversion=atoi(Value);
  else if (!strcasecmp(Name, "DVBC_Symbolrate")) Wirbelscan.DVBC_Symbolrate=atoi(Value);
  else if (!strcasecmp(Name, "DVBC_QAM"))        Wirbelscan.DVBC_QAM=atoi(Value);
  else if (!strcasecmp(Name, "CountryIndex"))    Wirbelscan.CountryIndex=atoi(Value);
  else if (!strcasecmp(Name, "SatIndex"))        Wirbelscan.SatIndex=atoi(Value);
  else if (!strcasecmp(Name, "enable_s2"))       Wirbelscan.enable_s2=atoi(Value);
  else if (!strcasecmp(Name, "ATSC_type"))       Wirbelscan.ATSC_type=atoi(Value);
  else if (!strcasecmp(Name, "scanflags"))       Wirbelscan.scanflags=atoi(Value);
  else return false;                                              
  return true;
}

void cPluginWirbelscan::StoreSetup(void)
{
  SetupStore("verbosity",       Wirbelscan.verbosity);
  SetupStore("logFile",         Wirbelscan.logFile);
  SetupStore("DVB_Type",        Wirbelscan.DVB_Type);
  SetupStore("DVBT_Inversion",  Wirbelscan.DVBT_Inversion);
  SetupStore("DVBC_Inversion",  Wirbelscan.DVBC_Inversion);
  SetupStore("DVBC_Symbolrate", Wirbelscan.DVBC_Symbolrate);
  SetupStore("DVBC_QAM",        Wirbelscan.DVBC_QAM);
  SetupStore("CountryIndex",    Wirbelscan.CountryIndex);
  SetupStore("SatIndex",        Wirbelscan.SatIndex);
  SetupStore("enable_s2",       Wirbelscan.enable_s2);
  SetupStore("ATSC_type",       Wirbelscan.ATSC_type);
  SetupStore("scanflags",       Wirbelscan.scanflags);
  cCondWait::SleepMs(500);
  Setup.Save();
}

using namespace WIRBELSCAN_SERVICE;

bool cPluginWirbelscan::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  char * service;

  asprintf(&service, "%s%s", SPlugin, SInfo);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cWirbelscanInfo * info = (cWirbelscanInfo*) Data;
     info->PluginVersion  = VERSION;
     info->CommandVersion = SCommand;
     info->StatusVersion  = SStatus;
     info->SetupVersion   = SSetup;
     info->CountryVersion = SCountry;
     info->SatVersion     = SSat;
     info->reserved1      = VERSION; // may change later.
     info->reserved2      = VERSION; // may change later.
     info->reserved3      = VERSION; // may change later.
     return true;      
     }

  free(service);  
  asprintf(&service, "%sGet%s", SPlugin, SStatus);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cWirbelscanStatus * s = (cWirbelscanStatus*)Data;
     if (Scanner)
        s->status = StatusScanning;
     else
        s->status = StatusStopped;
     memset(s->curr_device, 0, 256);
     strcpy(s->curr_device, *deviceName? *deviceName:"none");
     memset(s->transponder, 0, 256);
     strcpy(s->transponder, *lTransponder? *lTransponder:"none");
     s->progress = s->status == StatusScanning?lProgress:0;
     s->strength = s->status == StatusScanning?lStrength:0;
     s->numChannels = Channels.Count();
     s->newChannels = ((Channels.Count() - channelcount) > 0) && channelcount?Channels.Count() - channelcount:0;
     return true;
     }

  free(service);  
  asprintf(&service, "%s%s", SPlugin, SCommand);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cWirbelscanCmd * request = (cWirbelscanCmd*) Data;

     switch (request->cmd) {
        case  CmdStartScan:
                  DoScan(Wirbelscan.DVB_Type);
                  request->replycode = true;
                  break;       
        case  CmdStopScan:
                  DoStop();
                  request->replycode = true;
                  break;
        case  CmdStore:
                  StoreSetup();
                  request->replycode = true;
                  break;
        default:
                  request->replycode = false;
                  return false;
        }
     return true;
     }

  free(service);  
  asprintf(&service, "%sGet%s", SPlugin, SSetup);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cWirbelscanScanSetup * d = (cWirbelscanScanSetup*)Data;
     d->verbosity       = Wirbelscan.verbosity;
     d->logFile         = Wirbelscan.logFile;
     d->DVB_Type        = (int) Wirbelscan.DVB_Type;
     d->DVBT_Inversion  = Wirbelscan.DVBT_Inversion;
     d->DVBC_Inversion  = Wirbelscan.DVBC_Inversion;
     d->DVBC_Symbolrate = Wirbelscan.DVBC_Symbolrate;
     d->DVBC_QAM        = Wirbelscan.DVBC_QAM;
     d->CountryId       = Wirbelscan.CountryIndex;
     d->SatId           = Wirbelscan.SatIndex;
     d->scanflags       = Wirbelscan.scanflags;
     d->ATSC_type       = Wirbelscan.ATSC_type;
     return true;
     }

  free(service);  
  asprintf(&service, "%sSet%s", SPlugin,SSetup);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cWirbelscanScanSetup * d = (cWirbelscanScanSetup*)Data;
     Wirbelscan.verbosity       = d->verbosity;
     Wirbelscan.logFile         = d->logFile;
     Wirbelscan.DVB_Type        = (scantype_t)d->DVB_Type;
     Wirbelscan.DVBT_Inversion  = d->DVBT_Inversion;
     Wirbelscan.DVBC_Inversion  = d->DVBC_Inversion;
     Wirbelscan.DVBC_Symbolrate = d->DVBC_Symbolrate;
     Wirbelscan.DVBC_QAM        = d->DVBC_QAM;
     Wirbelscan.CountryIndex    = d->CountryId;
     Wirbelscan.SatIndex        = d->SatId;
     Wirbelscan.scanflags       = d->scanflags;
     Wirbelscan.ATSC_type       = d->ATSC_type;
     return true;
     }

  free(service);  
  asprintf(&service, "%sGet%s", SPlugin,SSat);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cPreAllocBuffer * b = (cPreAllocBuffer *)Data;
     SListItem * l = b->buffer;
     b->count = 0;
     if (b->size < (uint) sat_count()) {
        b->size = sat_count();
        return true;
        }
     for (int i = 0; i < sat_count(); i++) {
        memset(&l[i], 0, sizeof(SListItem));
        l[i].id = sat_list[i].id;
        strcpy(l[i].short_name, sat_list[i].short_name);
        strcpy(l[i].full_name, sat_list[i].full_name);
        b->count++;
        }
     return true;
     }

  free(service);  
  asprintf(&service, "%sGet%s", SPlugin,SCountry);
  if (strcmp(Id, service) == 0) {
     free(service);
     if (! Data) return true; // check for support.
     cPreAllocBuffer * b = (cPreAllocBuffer *)Data;
     SListItem * l = b->buffer;
     b->count = 0;
     if (b->size < (uint) COUNTRY::country_count()) {
        b->size = COUNTRY::country_count();
        return true;
        }
     for (int i = 0; i < COUNTRY::country_count(); i++) {
        memset(&l[i], 0, sizeof(SListItem));
        l[i].id = COUNTRY::country_list[i].id;
        strcpy(l[i].short_name, COUNTRY::country_list[i].short_name);
        strcpy(l[i].full_name, COUNTRY::country_list[i].full_name);
        b->count++;
        }
     return true;
     }

  free(service);
  return false;
}

const char **cPluginWirbelscan::SVDRPHelpPages(void)
{
  static const char * SVDRHelp[] = {
    "S_START\n"
    "    Start scan",
    "S_STOP\n"
    "    Stop scan(s) (if any)",
    "S_TERR\n"
    "    Start DVB-T scan",
    "S_CABL\n"
    "    Start DVB-C scan",
    "S_SAT\n"
    "    Start DVB-S/S2 scan",
    "SETUP <verb:log:type:inv_t:inv_c:srate:qam:cidx:sidx:s2:atsc:flags>\n"
    "    verb   verbostity (0..5)\n"
    "    log    logfile (0=OFF, 1=stdout, 2=syslog)\n"
    "    type   scan type\n"
    "           (0=DVB-T, 1=DVB-C, 2=DVB-S/S2, 3=pvrinput, 4=pvrinput FM, 5=ATSC)\n"
    "    inv_t  DVB-T inversion\n"
    "           (0=AUTO/OFF, 1=AUTO/ON)\n"
    "    inv_c  DVB-C inversion\n"
    "           (0=AUTO/OFF, 1=AUTO/ON)\n"
    "    srate  DVB-C Symbolrate (0..15)\n"
    "    qam    DVB-C modulation\n"
    "           (0=AUTO, 1=QAM64, 2=QAM128, 3=QAM256, 4=ALL)\n"
    "    cidx   country list index\n"
    "    sidx   satellite list index\n"
    "    s2     enable DVB-S2 (0=OFF, 1=ON)\n"
    "    atsc   ATSC scan type\n"
    "           (0=VSB, 1=QAM, 2=VSB+QAM)\n"
    "    flags  bitwise flag of\n"
    "           TV=1, RADIO=2, FTA=4, SCRAMBLED=8, HDTV=16",
    "STORE\n"
    "    Store current setup",
    "LSTC\n"
    "    list countries",
    "LSTS\n"
    "    list satellites",
    "QUERY\n"
    "    return plugin version, current setup and service versions",
    NULL
    };
  return SVDRHelp;
}

#define cmd(x) (strcasecmp(Command, x) == 0)

cString cPluginWirbelscan::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if      cmd("S_TERR"  ) { return DoScan(Wirbelscan.DVB_Type = DVB_TERR)   ? "DVB-T scan started"     : "Could not start DVB-T scan.";    }
  else if cmd("S_CABL"  ) { return DoScan(Wirbelscan.DVB_Type = DVB_CABLE)  ? "DVB-C scan started"     : "Could not start DVB-C scan.";    }
  else if cmd("S_SAT"   ) { return DoScan(Wirbelscan.DVB_Type = DVB_SAT)    ? "DVB-S scan started"     : "Could not start DVB-S scan.";    }
  else if cmd("S_PVR"   ) { return DoScan(Wirbelscan.DVB_Type = PVRINPUT)   ? "PVRx50 scan started"    : "Could not start PVRx50 scan.";   }
  else if cmd("S_PVR_FM") { return DoScan(Wirbelscan.DVB_Type = PVRINPUT_FM)? "PVRx50 FM scan started" : "Could not start PVRx50 FM scan.";}
  else if cmd("S_START" ) { return DoScan(Wirbelscan.DVB_Type)              ? "starting scan"          : "Could not start scan.";          }
  else if cmd("S_STOP"  ) { DoStop();       return "stopping scan(s)";  }
  else if cmd("STORE"   ) { StoreSetup();   return "setup stored.";     }
  else if cmd("SETUP"   ) {
    cWirbelscan d;

    if (12 != sscanf(Option, "%i:%i:%i:%i:%i:%i:%i:%i:%i:%i:%i:%i",
              &d.verbosity, &d.logFile,
              (int *) &d.DVB_Type,
              &d.DVBT_Inversion, &d.DVBC_Inversion,
              &d.DVBC_Symbolrate, &d.DVBC_QAM,
              &d.CountryIndex, &d.SatIndex,
              &d.enable_s2, &d.ATSC_type, &d.scanflags)) {

        ReplyCode = 501;
        return "couldnt parse setup string.";
        }
    Wirbelscan = d;
    return cString::sprintf("changed setup to %d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
                            Wirbelscan.verbosity,
                            Wirbelscan.logFile,
                            Wirbelscan.DVB_Type,
                            Wirbelscan.DVBT_Inversion,
                            Wirbelscan.DVBC_Inversion,
                            Wirbelscan.DVBC_Symbolrate,
                            Wirbelscan.DVBC_QAM,
                            Wirbelscan.CountryIndex,
                            Wirbelscan.SatIndex,
                            Wirbelscan.enable_s2,
                            Wirbelscan.ATSC_type,
                            Wirbelscan.scanflags);
    }
  else if cmd("QUERY") {
    return cString::sprintf("plugin version: %s\n"
                            "current setup:  %d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d\n"
                            "commands api:   %s\n"
                            "status api:     %s\n"
                            "setup api:      %s\n"
                            "country api:    %s\n"
                            "sat api:        %s",
                            VERSION,
                            Wirbelscan.verbosity,
                            Wirbelscan.logFile,
                            Wirbelscan.DVB_Type,
                            Wirbelscan.DVBT_Inversion,
                            Wirbelscan.DVBC_Inversion,
                            Wirbelscan.DVBC_Symbolrate,
                            Wirbelscan.DVBC_QAM,
                            Wirbelscan.CountryIndex,
                            Wirbelscan.SatIndex,
                            Wirbelscan.enable_s2,
                            Wirbelscan.ATSC_type,
                            Wirbelscan.scanflags,
                            SCommand,
                            SStatus,
                            SSetup,
                            SCountry,
                            SSat);
    }
  else if cmd("LSTC") {
    cString s = "";
    for (int i = 0; i < COUNTRY::country_count(); i++)
         s = cString::sprintf("%s%d:%s:%s\n", *s,
                         COUNTRY::country_list[i].id,
                         COUNTRY::country_list[i].short_name,
                         COUNTRY::country_list[i].full_name);
    return s;
    }
  else if cmd("LSTS") {
    cString s = "";
    for (int i = 0; i < sat_count(); i++)
         s = cString::sprintf("%s%d:%s:%s\n", *s,
                         sat_list[i].id,
                         sat_list[i].short_name,
                         sat_list[i].full_name);
    return s;
    }
  return NULL;
}

VDRPLUGINCREATOR(cPluginWirbelscan); // Don't touch this!
