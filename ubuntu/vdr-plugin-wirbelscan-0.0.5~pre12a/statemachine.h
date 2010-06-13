/*
 * statemachine.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * wirbelscan-0.0.5
 *
 * $Id$ v20080915
 */

#ifndef __WIRBELSCAN_STATEMACHINE_H_
#define __WIRBELSCAN_STATEMACHINE_H_

#include <vdr/tools.h>
#include <vdr/device.h>

/* wirbelscan-0.0.5
 */

class cStateMachine : public cThread {
private:
  enum eState {
    eStart           = 0,   // init, add next_from_list to NewTransponders      (NextTransponder)
    eStop            = 1,   // cleanup and leave loop                           ()
    eTune            = 2,   // tune, check for lock                             (AttachReceiver, NextTransponder)
    eNextTransponder = 3,   // next unsearched transponder from NewTransponders (Tune, Stop)
    eAttachReceiver  = 4,   // attach some dummy receiver                       (ScanPat, Stop)
    eDetachReceiver  = 5,   // detach all receivers                             (NextTransponder)
    eScanPat         = 6,   // pat/pmt scan                                     (ScanNit)
    eScanNit         = 7,   // nit scan                                         (ScanNitOther)
    eScanSdt         = 8,   // sdt scan                                         (ScanSdtOther)
    eScanEit         = 9,   // eit scan                                         (DetachReceiver)
    eUnknown         = 10,  // oops                                             (Stop)
    eAddChannels     = 11,  // adding results
    };

  eState     state, lastState;
  cChannel * initial;
  cDevice *  dev;
  bool       stop;
  cCondWait  cWait;

protected:
  virtual void Action(void);
  virtual void Report(eState State);
public:
  cStateMachine(cDevice * Dev, cChannel * InitialTransponder);
  virtual ~cStateMachine(void);
  void DoStop() {
    stop = true;
    }
  };

#endif
