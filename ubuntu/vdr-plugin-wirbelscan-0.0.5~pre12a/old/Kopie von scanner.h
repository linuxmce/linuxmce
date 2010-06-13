/*
 * scanner.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __WIRBELSCAN_SCANNER_H_
#define __WIRBELSCAN_SCANNER_H_

#include <linux/dvb/frontend.h>
#include <vdr/thread.h>
#include <linux/types.h>

#include "dvb_wrapper.h"
#include "scanfilter.h"

///!-------------terr scanning stuff---------------------------------
///!  tested, works with cinergy T² cards..
///!-----------------------------------------------------------------

class cTerrScanner : public cThread {
private:
  int                    fd_frontend;
  int                    detachReceivers;
  eCableTerrInversions   inversion;
  eTerrBandwidths        bandwidth;
  eTerrConstellations    constellation;
  eTerrCodeRates         coderateh, coderatel;
  eTerrGuardIntervals    guardinterval;
  eTerrTransmissionModes transmissionmode;
  eTerrHierarchies       hierarchy;
  cChannel *             aChannel;
  cDevice *              dev;
  int                    freq, fIndex, cIndex, vnorm, flIndex, fCount;
  int *                  percent;
  int *                  isLiving;
  bool                   shouldstop;
  char *                 buf;
protected:
  virtual void Action(void);
public:
  cTerrScanner(const char * Description, int * Percent);
  virtual      ~cTerrScanner(void);

  virtual void SetShouldstop(bool On);
  virtual bool ActionAllowed(void) {
    return (Running() && !shouldstop);
    };
  };

///!-------------cable scanning stuff--------------------------------
///!  tested, seems to work perfectly with TT FF cards and cinergy1200/knc one..
///!-----------------------------------------------------------------

class cCableScanner : public cThread {
private:
  int                    fd_frontend;
  int                    detachReceivers;
  eCableTerrInversions   inversion;
  eCableSatCodeRates     coderateh;
  eCableModulations      modulation;
  int                    symbolrate;
  cChannel *             aChannel;
  cDevice *              dev;
  int                    freq, fIndex, cIndex, vnorm, flIndex, fCount;
  int *                  percent;
  int *                  isLiving;
  bool                   shouldstop;
  char *                 buf;
protected:
  virtual void Action(void);
public:
  cCableScanner(const char * Description, int * Percent);
  virtual      ~cCableScanner(void);

  virtual void SetShouldstop(bool On);
  virtual bool ActionAllowed(void) {
    return (Running() && !shouldstop);
    };
  };



/*
class cCableScanner : public cThread {
private:
  int         fd_frontend;
  int         detachReceivers;
  int         cardnum;
  int         inversion, qam, coderateh, symbolrate;
  int         freq, fIndex, cIndex, vnorm, flIndex, fCount, cnt;
  int         pa, pb, pc, pd;
  int *       percent;
  bool        LiveView;
  bool        shouldstop;
  char *      buf;
  struct dvb_frontend_info fe_info;
  fe_status_t frontend_status;
  cCondWait   CWait;
  cChannel *  ScanChannel;
protected:
  virtual void Action(void);
  virtual bool CheckTransponder(int frequency, int srate, int mod, int coderateh, int maxtunetrials, int maxlocktrials);
  virtual bool HasLock(void) {
    GetStatus();
    return (frontend_status & FE_HAS_LOCK);
    };
  virtual bool HasSomeSignal(void) {
    GetStatus();
    return ((frontend_status & FE_HAS_SIGNAL) && (frontend_status & FE_HAS_CARRIER));
    };
  virtual void GetStatus(void);
public:
  cCableScanner(const char * Description, int * Percent);
  virtual      ~cCableScanner(void);
  virtual void StopWait(void) {
    CWait.Signal();
    };
  virtual void SetShouldstop(bool On) {
    shouldstop = On;
    };
  virtual bool ActionAllowed(void);
  };
*/

///!-------------sat scanning stuff----------------------------------
///!  trying some sat scanning, not complete tested..
///!-----------------------------------------------------------------

class cSatScanner : public cThread {
private:
  int         fd_frontend;
  int         detachReceivers;
  int         cardnum;
  int         polarisation, coderateh, symbolrate, modulation_system,
              rolloff, modulation_type;
  int         freq, fIndex, cIndex, vnorm, flIndex, fCount, cnt;
  int *       percent;
  bool        LiveView;
  bool        shouldstop;
  char *      buf;
  struct dvb_frontend_info fe_info;
  fe_status_t frontend_status;
  cCondWait   CWait;
  cChannel *  ScanChannel;
protected:
  virtual void Action(void);
  virtual void SetTransponderData(void);
  virtual void SwitchTransponder(void);
  virtual bool HasLock(void) {
    GetStatus();
    return (frontend_status & FE_HAS_LOCK);
    };
  virtual bool HasSomeSignal(void) {
    GetStatus();
    return ((frontend_status & FE_HAS_SIGNAL) && (frontend_status & FE_HAS_CARRIER));
    };
  virtual void GetStatus(void);
public:
  cSatScanner(const char * Description, int * Percent);
  virtual      ~cSatScanner(void);
  virtual void StopWait(void) {
    CWait.Signal();
    };
  virtual void SetShouldstop(bool On) {
    shouldstop = On;
    };
  virtual bool ActionAllowed(void);
  };

///!-------------analog scanning stuff-------------------------------
///!
///!-----------------------------------------------------------------

class cAnalogScanner : public cThread {
private:
  int       fd_videodev;
  int       detachReceivers;
  int       cardnum;
  int       freq, fIndex, cIndex, vnorm, flIndex, fCount, cnt;
  int *     percent;
  int       channelSyntax;
  bool      LiveView;
  bool      shouldstop;
  char *    buf;
  cCondWait CWait;
  cChannel  ScanChannel;
protected:
  virtual void Action(void);
  virtual bool IsPvr(int dev);
  virtual bool EnumInputs(struct v4l2_input * vin);
  virtual bool SetInput(int input);
  virtual bool SetFrequency(int frequency);
  virtual int  GetSignalStatus(void);
public:
  cAnalogScanner(const char * Description, int * Percent);
  virtual      ~cAnalogScanner(void);
  virtual void StopWait(void) {
    CWait.Signal();
    };
  virtual void SetShouldstop(bool On) {
    shouldstop = On;
    };
  virtual bool ActionAllowed(void);
  };

#endif
