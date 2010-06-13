/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONOSD_H
#define __FEMONOSD_H

#include <linux/dvb/frontend.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <vdr/osd.h>
#include <vdr/thread.h>
#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/channels.h>
#include <vdr/transfer.h>
#include <vdr/tools.h>
#include "svdrpservice.h"

#define MAX_BM_NUMBER 8

class cFemonOsd : public cOsdObject, public cThread, public cStatus {
private:
  static cFemonOsd *pInstance;

  cOsd             *m_Osd;
  cFemonReceiver   *m_Receiver;
  int               m_Frontend;
  int               m_SvdrpFrontend;
  double            m_SvdrpVideoBitrate;
  double            m_SvdrpAudioBitrate;
  SvdrpConnection_v1_0 m_SvdrpConnection;
  cPlugin          *m_SvdrpPlugin;
  int               m_Number;
  int               m_OldNumber;
  uint16_t          m_SNR;
  bool              m_SNRValid;
  uint16_t          m_Signal;
  bool              m_SignalValid;
  uint32_t          m_BER;
  bool              m_BERValid;
  uint32_t          m_UNC;
  bool              m_UNCValid;
  fe_status_t       m_FrontendStatus;
  bool              m_FrontendStatusValid;
  dvb_frontend_info m_FrontendInfo;
  int               m_DisplayMode;
  int               m_OsdWidth;
  int               m_OsdHeight;
  int               m_OsdLeft;
  int               m_OsdTop;
  cFont            *m_Font;
  cTimeMs           m_InputTime;
  cCondWait         m_Sleep;
  cMutex            m_Mutex;

  void DrawStatusWindow(void);
  void DrawInfoWindow(void);
  bool SvdrpConnect(void);
  bool SvdrpTune(void);

protected:
  cFemonOsd();
  cFemonOsd(const cFemonOsd&);
  cFemonOsd& operator= (const cFemonOsd&);
  virtual void Action(void);
  virtual void ChannelSwitch(const cDevice * device, int channelNumber);
  virtual void SetAudioTrack(int Index, const char * const *Tracks);

public:
  static cFemonOsd *Instance(bool create = false);
  ~cFemonOsd();

  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);

  bool    DeviceSwitch(int direction);
  double  GetVideoBitrate(void);
  double  GetAudioBitrate(void);
  double  GetDolbyBitrate(void);
};

#endif //__FEMONOSD_H

