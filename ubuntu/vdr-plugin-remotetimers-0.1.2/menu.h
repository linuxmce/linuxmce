/*
 * menu.h: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menu.h 1.85 2006/03/25 12:15:19 kls Exp $
 */

#ifndef _REMOTETIMERS_MENU__H
#define _REMOTETIMERS_MENU__H

#include "svdrp.h"
//#include "ci.h"
//#include "device.h"
#include <vdr/epg.h>
#include <vdr/osdbase.h>
//#include "dvbplayer.h"
#include <vdr/menuitems.h>
#include <vdr/recorder.h>
#include <vdr/skins.h>

namespace PluginRemoteTimers {

class cMenuTimerItem : public cOsdItem {
private:
  cTimer *timer;
  bool remote;
  int user;
public:
  cMenuTimerItem(cTimer *Timer, int User, bool Remote = false);
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  int User() { return user; }
  bool Remote() { return remote; }
  cTimer *Timer(void) { return timer; }

  static int ParseUser(const cTimer *Timer);
  static void UpdateUser(cTimer &Timer, int User);
  };

class cMenuTimers : public cOsdMenu {
private:
  int helpKeys;
  int userFilter;
  cString currentTimerString;

  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState OnOff(void);
  eOSState Info(void);
  cMenuTimerItem *CurrentItem(void);
  cTimer *CurrentTimer(void);
  void SetHelpKeys(void);
  void Set(eRemoteTimersState State = rtsOk);
  void CheckState(eRemoteTimersState State, bool RefreshMsg = true);
public:
  cMenuTimers(void);
  virtual ~cMenuTimers();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuSchedule : public cOsdMenu {
private:
  cSchedulesLock schedulesLock;
  const cSchedules *schedules;
  bool now, next;
  int otherChannel;
  int helpKeys;
  int timerState;
  int userFilter;
  eOSState Number(void);
  eOSState Record(void);
  eOSState Switch(void);
  void PrepareScheduleAllThis(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleThisThis(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleThisAll(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleAllAll(const cEvent *Event, const cChannel *Channel);
  bool Update(void);
  void SetHelpKeys(void);
public:
  cMenuSchedule(void);
  virtual ~cMenuSchedule();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuEvent : public cOsdMenu {
private:
  const cEvent *event;
public:
  cMenuEvent(const cEvent *Event, bool CanSwitch = false, bool Buttons = false);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuRecordingItem;

class cMenuRecordings : public cOsdMenu {
private:
  static int userFilter;
  static bool replayEnded;

  char *base;
  int level;
  int recordingsState;
  int helpKeys;
  bool SetFreeDiskDisplay(bool Force = false);
  void SetHelpKeys(void);
  void Set(bool Refresh = false);
  bool Open(bool OpenSubMenus = false);
  eOSState Play(void);
  eOSState Rewind(void);
  eOSState Delete(void);
  eOSState Info(void);
  eOSState Edit(void);
  //eOSState Commands(eKeys Key = kNone);
protected:
  cRecording *GetRecording(cMenuRecordingItem *Item);
public:
  static void SetReplayEnded() { replayEnded = true; }
  static bool ReplayEnded() { return replayEnded; }

  cMenuRecordings(const char *Base = NULL, int Level = 0, bool OpenSubMenus = false);
  ~cMenuRecordings();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuMain : public cOsdMenu {
private:
  static int count;
public:
  static bool IsOpen() { return count; }

  cMenuMain(const char *Title, eOSState State);
  virtual ~cMenuMain();
  virtual eOSState ProcessKey(eKeys Key);
  };

}; // namespace PluginRemoteTimers

#endif //_REMOTETIMERS_MENU__H
