/*
 * wirbelscan_services.h
 *
 * Copyright (C) 2010 Winfried Koehler 
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
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * The author can be reached at: handygewinnspiel AT gmx DOT de
 *
 * The project's page is http://wirbel.htpc-forum.de/wirbelscan/index2.html
 */

#ifndef __WIRBELSCAN_SERVICES_H__
#define __WIRBELSCAN_SERVICES_H__


/********************************************************************
 *
 * wirbelscans plugin service interface
 *
 * see http://wirbel.htpc-forum.de/wirbelscan/vdr-servdemo-0.0.1.tgz
 * for example on usage.
 *
 *******************************************************************/

namespace WIRBELSCAN_SERVICE { 
/* begin of namespace. to use this header file:
 * #include "../wirbelscan/wirbelscan_services.h"
 * using namespace WIRBELSCAN_SERVICE;
 */

/* --- service(s) version ----------------------------------------------------
 */

static const char *SPlugin    = "wirbelscan_";   // prefix
static const char *SInfo      = "GetVersion";    // plugin version and service api
static const char *SCommand   = "DoCmd#0001";    // command api 0001
static const char *SStatus    = "Status#0001";   // query status
static const char *SSetup     = "Setup#0001";    // get/put setup, GetSetup#XXXX/SetSetup#XXXX
static const char *SCountry   = "Country#0001";  // get list of country IDs and Names
static const char *SSat       = "Sat#0001";      // get list of satellite IDs and Names

/* --- wirbelscan_GetVersion -------------------------------------------------
 * Query wirbelscans versions, will fail only if plugin version doesnt support service at all.
 */

typedef struct {
  const char * PluginVersion;                    // plugin version
  const char * CommandVersion;                   // commands service version
  const char * StatusVersion;                    // status service version
  const char * SetupVersion;                     // get/put setup service version
  const char * CountryVersion;                   // country ID list version
  const char * SatVersion;                       // satellite ID list version
  const char * reserved1;                        // reserved, dont use.
  const char * reserved2;                        // reserved, dont use.
  const char * reserved3;                        // reserved, dont use.
} cWirbelscanInfo;

/* --- wirbelscan_DoCmd ------------------------------------------------------
 * Execute commands.
 */

typedef enum {
  CmdStartScan = 0,                              // start scanning
  CmdStopScan  = 1,                              // stop scanning
  CmdStore     = 2,                              // store current setup
} s_cmd;

typedef struct {
  s_cmd cmd;                                     // see above.
  bool replycode;                                // false, if unsuccessful.
} cWirbelscanCmd;

/* --- wirbelscan_Status -----------------------------------------------------
 * Query Status. Use this to build up your osd information displayed to user.
 */

typedef enum {
  StatusUnknown  = 0,                            // no status information available, try again later.
  StatusScanning = 1,                            // scan in progress.
  StatusStopped  = 2,                            // no scan in progress (not started, finished or stopped).
  StatusBusy     = 3,                            // plugin is busy, try again later.
} cStatus;

typedef struct {
  cStatus status;                                // see above.
  char curr_device[256];                         // name of current device. meaningless, if (status != StatusScanning)
  uint16_t progress;                             // progress by means of "percent of predefined transponders". NOTE: will differ in terms of time. meaningless, if (status != StatusScanning)
  uint16_t strength;                             // current signal strength as reported from device. NOTE: updated only after switching to new transponder. meaningless, if (status != StatusScanning)
  char transponder[256];                         // current transponder. meaningless, if (status != StatusScanning)
  uint16_t numChannels;                          // current number of (all) channels, including those which are new. meaningless, if (status != StatusScanning)
  uint16_t newChannels;                          // number of channels found during this scan. meaningless, if (status != StatusScanning)
  uint16_t reserved1;                            // reserved, dont use.
  uint16_t reserved2;                            // reserved, dont use.
  uint16_t reserved3;                            // reserved, dont use.
} cWirbelscanStatus;

/* --- wirbelscan_GetSetup, wirbelscan_SetSetup ------------------------------
 * Get/Set Setup. Use this to build up your setup osd displayed to user.
 */

typedef struct {
  uint16_t  verbosity;                           // 0 (errors only) .. 5 (extended debug); default = 3 (messages)
  uint16_t  logFile;                             // 0 = off, 1 = stdout, 2 = syslog
  uint16_t DVB_Type;                             // DVB-T = 0, DVB-C = 1, DVB-S/S2 = 2, PVRINPUT = 3, PVRINPUT(FM Radio) = 4, ATSC = 5
  uint16_t DVBT_Inversion;                       // AUTO/OFF = 0, AUTO/ON = 1
  uint16_t DVBC_Inversion;                       // AUTO/OFF = 0, AUTO/ON = 1
  uint16_t DVBC_Symbolrate;                      // careful here - may change. AUTO = 0, 6900 = 1, 6875 = 2  (...)  14 = 5483, 15 = ALL
  uint16_t DVBC_QAM;                             // AUTO = 0,QAM64 = 1, QAM128 = 2, QAM256 = 3, ALL = 4
  uint16_t CountryId;                            // the id according to country, found in country list,   see wirbelscan_GetCountry
  uint16_t SatId;                                // the id according to satellite, found in list,         see wirbelscan_GetSat
  uint32_t scanflags;                            // bitwise flag of wanted channels: TV = (1 << 0), RADIO = (1 << 1), FTA = (1 << 2), SCRAMBLED = (1 << 4), HDTV = (1 << 5)
  uint16_t ATSC_type;                            // VSB = 0, QAM = 1, both VSB+QAM = 2
  uint16_t stuffing[6];                          // dont use.
} cWirbelscanScanSetup;

/* --- wirbelscan_GetCountry, wirbelscan_GetSat ------------------------------
 * Use this to build up your setup OSD - user needs to choose satellite and
 * country by name and assign correct IDs to setup, see
 *    * cWirbelscanScanSetup::CountryId
 *    * cWirbelscanScanSetup::SatId.
 *
 * 1) Query needed buffer size with cPreAllocBuffer::size == 0
 * 2) allocate memory in your plugin, to be at least: cPreAllocBuffer::size * sizeof(SListItem)
 * 3) second call fills buffer with count * sizeof(SListItem) bytes.
 * 4) access to items as SListItem*
 */

typedef struct {
  int id;
  char short_name[8];
  char full_name[64];
} SListItem;

typedef struct {
  uint32_t size;
  uint32_t count;
  SListItem * buffer;
} cPreAllocBuffer;

} /* end of namespace, dont touch */
#endif
