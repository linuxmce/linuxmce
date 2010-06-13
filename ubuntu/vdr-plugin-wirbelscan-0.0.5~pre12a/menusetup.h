/*
 * menusetup.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __WIRBELSCAN_MENUSETUP_H_
#define __WIRBELSCAN_MENUSETUP_H_

#include <vdr/menuitems.h>
#include <vdr/osdbase.h>
#include <vdr/thread.h>
#include "scanner.h"
#include "common.h"
 
#define STDOUT                  1
#define SYSLOG                  2

#define ADAPTER_AUTO            0

#define DVBC_INVERSION_AUTO     0

#define DVBC_QAM_AUTO           0
#define DVBC_QAM_64             1
#define DVBC_QAM_128            2
#define DVBC_QAM_256            3

#define CHANNEL_SYNTAX_PVRINPUT      0
#define CHANNEL_SYNTAX_PVRINPUT_IPTV 1

#define MAXSIGNALSTRENGTH       65535
#define MINSIGNALSTRENGTH       16383


#define SCAN_TV        ( 1 << 0 )
#define SCAN_RADIO     ( 1 << 1 )
#define SCAN_FTA       ( 1 << 2 )
#define SCAN_SCRAMBLED ( 1 << 3 )
#define SCAN_HD        ( 1 << 4 )

extern const char * extVer;
extern const char * extDesc;
extern cScanner   * Scanner;
extern cString      deviceName;
extern cString      lTransponder;
extern int lProgress;
extern int lStrength;
extern int channelcount;

void stopScanners(void);
bool DoScan (int DVB_Type);
void DoStop (void);

class cWirbelscan {
 private:
 public:
  int verbosity;
  int logFile;
  scantype_t DVB_Type;
  int DVBT_Inversion;
  int DVBC_Inversion;
  int DVBC_Symbolrate;
  int DVBC_QAM;
  int CountryIndex;
  int SatIndex;
  int enable_s2;
  int ATSC_type;
  uint32_t scanflags;
  bool update;
  cWirbelscan(void);
};
extern cWirbelscan Wirbelscan;

class cMenuScanning : public cMenuSetupPage {
 private:
    bool needs_update;
 protected:
    virtual bool StartScan(void);
    virtual bool StopScan(void);
    virtual void AddCategory(const char * category);
 public:
    cMenuScanning(void);
    ~cMenuScanning(void);
    virtual void Store(void);
    virtual eOSState ProcessKey(eKeys Key);
    void SetStatus(int status);
    void SetProgress(const int progress, scantype_t type, int left);
    void SetTransponder(const cChannel * transponder); 
    void SetStr(uint strength, bool locked);
    void SetChan(int count);
    void SetDeviceInfo(cString Info, bool update = true);
    void SetChanAdd(uint32_t flags);
    void AddLogMsg(const char * Msg);
};
extern cMenuScanning * MenuScanning;

#endif
