/*
 * setup.h: IPTV plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __IPTV_SETUP_H
#define __IPTV_SETUP_H

#include <vdr/menuitems.h>
#include <vdr/sourceparams.h>
#include "common.h"

class cIptvTransponderParameters
{
  friend class cIptvSourceParam;
private:
  int sidscan;
  int pidscan;
  int protocol;
  char address[MaxFileName];
  int parameter;
public:
  cIptvTransponderParameters(const char *Parameters = NULL);
  int SidScan(void) const { return sidscan; }
  int PidScan(void) const { return pidscan; }
  int Protocol(void) const { return protocol; }
  const char *Address(void) const { return address; }
  int Parameter(void) const { return parameter; }
  void SetSidScan(int SidScan) { sidscan = SidScan; }
  void SetPidScan(int PidScan) { pidscan = PidScan; }
  void SetProtocol(int Protocol) { protocol = Protocol; }
  void SetAddress(const char *Address) { strncpy(address, Address, sizeof(address)); }
  void SetParameter(int Parameter) { parameter = Parameter; }
  cString ToString(char Type) const;
  bool Parse(const char *s);
};

class cIptvSourceParam : public cSourceParam
{
private:
  enum {
    eProtocolUDP,
    eProtocolHTTP,
    eProtocolFILE,
    eProtocolEXT,
    eProtocolCount
  };
  int param;
  cChannel data;
  cIptvTransponderParameters itp;
  const char *protocols[eProtocolCount];
public:
  cIptvSourceParam(char Source, const char *Description);
  virtual void SetData(cChannel *Channel);
  virtual void GetData(cChannel *Channel);
  virtual cOsdItem *GetOsdItem(void);
};

class cIptvPluginSetup : public cMenuSetupPage
{
private:
  int tsBufferSize;
  int tsBufferPrefill;
  int extProtocolBasePort;
  int sectionFiltering;
  int numDisabledFilters;
  int disabledFilterIndexes[SECTION_FILTER_TABLE_SIZE];
  const char *disabledFilterNames[SECTION_FILTER_TABLE_SIZE];
  cVector<const char*> help;

  eOSState ShowInfo(void);
  void Setup(void);
  void StoreFilters(const char *Name, int *Values);

protected:
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Store(void);

public:
  cIptvPluginSetup();
};

#endif // __IPTV_SETUP_H
