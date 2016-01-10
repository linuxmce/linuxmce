/*
 * Copyright (C) 2008-2013 Frank Schmirler <vdr@schmirler.de>
 *
 * Major parts copied from VDR's menu.c
 * $Id: menu.c 2.82.1.2 2013/04/27 10:32:28 kls Exp $
 * Copyright (C) 2000, 2003, 2006, 2008 Klaus Schmidinger
 *
 * This file is part of VDR Plugin remotetimers.
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
 */

#include "menu.h"
#include "setup.h"
#include "moverec.h"
#include "i18n.h"
#include "conflict.h"
#include <vdr/menu.h>
#include "menuitems.h"
//#include <ctype.h>
//#include <limits.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <vdr/channels.h>
//#include "config.h"
#include <vdr/cutter.h>
//#include "eitscan.h"
//#include "i18n.h"
#include <vdr/interface.h>
//#include "plugin.h"
#include <vdr/recording.h>
//#include "remote.h"
//#include "shutdown.h"
//#include "sourceparams.h"
//#include "sources.h"
#include <vdr/status.h>
//#include "themes.h"
#include <vdr/timers.h>
//#include "transfer.h"
#include <vdr/videodir.h>

#include <ctype.h>

namespace PluginRemoteTimers {

//#define MAXWAIT4EPGINFO   3 // seconds
//#define MODETIMEOUT       3 // seconds
#define NEWTIMERLIMIT   120 // seconds until the start time of a new timer created from the Schedule menu,
                            // within which it will go directly into the "Edit timer" menu to allow
                            // further parameter settings
//#define DEFERTIMER       60 // seconds by which a timer is deferred in case of problems

//#define MAXRECORDCONTROLS (MAXDEVICES * MAXRECEIVERS)
//#define MAXINSTANTRECTIME (24 * 60 - 1) // 23:59 hours
//#define MAXWAITFORCAMMENU  10 // seconds to wait for the CAM menu to open
//#define CAMMENURETYTIMEOUT  3 // seconds after which opening the CAM menu is retried
//#define CAMRESPONSETIMEOUT  5 // seconds to wait for a response from a CAM
//#define MINFREEDISK       300 // minimum free disk space (in MB) required to start recording
//#define NODISKSPACEDELTA  300 // seconds between "Not enough disk space to start recording!" messages
#define MAXCHNAMWIDTH      16

#define CHNUMWIDTH  (numdigits(Channels.MaxNumber()) + 1)
#define CHNAMWIDTH  (min(MAXCHNAMWIDTH, Channels.MaxShortChannelNameLength() + 1))

#define AUX_STARTTAG "<remotetimers>"
#define AUX_ENDTAG "</remotetimers>"

int ParseUser(const char *p) {
  int user = 0;
  char *buffer = NULL;
  if (p && (p = strstr(p, AUX_STARTTAG))) {
      if (sscanf(p, AUX_STARTTAG "%a[^<]" AUX_ENDTAG, &buffer) == 1) {
          user = cMenuEditUserItem::Parse(buffer);
	  free(buffer);
	  }
      }
  return user;
}

cString UpdateUser(const char *s, int User) {
  if (!s)
      s = "";

  const char *tail = s + strlen(s);
  while (tail > s && isspace(*(tail - 1)))
      tail--;

  const char *start = strstr(s, AUX_STARTTAG);
  const char *end = start ? strstr(start, AUX_ENDTAG) : NULL;

  if (start)
      end = end ? end + strlen(AUX_ENDTAG) : start + strlen(AUX_STARTTAG);
  else
      start = end = tail;

  int lenPrefix = start - s;
  int lenPostfix = tail - end;

  if (User == 0)
      return cString::sprintf("%.*s%.*s", lenPrefix, s, lenPostfix, end);
  else
      return cString::sprintf("%.*s" AUX_STARTTAG "%s" AUX_ENDTAG "%.*s", lenPrefix, s, *cMenuEditUserItem::ToString(User), lenPostfix, end);
}

cTimer* GetBestMatch(const cEvent *Event, int UserMask, eTimerMatch *Match, int *Type, bool *Remote)
{
	cTimer *localTimer = NULL, *remoteTimer = NULL;
	eTimerMatch localMatch = tmNone, remoteMatch = tmNone;
	int localUser = 0, remoteUser = 0;

	for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
		eTimerMatch tm = t->Matches(Event);
		if (tm > localMatch) {
			int user = ParseUser(t->Aux());
			if (UserMask == 0 || user == 0 || (user & UserMask)) {
				localTimer = t;
				localMatch = tm;
				localUser = user;
				if (localMatch == tmFull)
					break;
			}
		}
	}
	for (cRemoteTimer *t = RemoteTimers.First(); t; t = RemoteTimers.Next(t)) {
		eTimerMatch tm = t->Matches(Event);
		if (tm > remoteMatch) {
			int user = ParseUser(t->Aux());
			if (UserMask == 0 || user == 0 || (user & UserMask)) {
				remoteTimer = t;
				remoteMatch = tm;
				remoteUser = user;
				if (remoteMatch == tmFull)
					break;
			}
		}
	}
	if (Match)
		*Match = remoteMatch > localMatch ? remoteMatch : localMatch;
	if (Type)
		*Type = (remoteMatch > localMatch ? remoteUser : localUser) == 0 ? 0 : 1;
	if (Remote)
		*Remote = remoteMatch > localMatch;
	return remoteMatch > localMatch ? remoteTimer : localTimer;
}


// --- cFreeDiskSpace --------------------------------------------------------

#define DISKSPACECHEK     5 // seconds between disk space checks
#define MB_PER_MINUTE 25.75 // this is just an estimate!

class cFreeDiskSpace {
private:
  static time_t lastDiskSpaceCheck;
  static int lastFreeMB;
  static cString freeDiskSpaceString;
  static cString lastPath;
  static int VideoDiskSpace(const char *Dir, int *FreeMB);
  static int DeletedFileSizeMB(const char *Dir);
  static double MBperMinute(const char* Dir);
public:
  static bool HasChanged(const char *SubDir, bool ForceCheck = false);
  static const char *FreeDiskSpaceString() { return freeDiskSpaceString; }
  static cString FreeDiskSpaceString(const cRecording* recording);
  };

time_t cFreeDiskSpace::lastDiskSpaceCheck = 0;
int cFreeDiskSpace::lastFreeMB = 0;
cString cFreeDiskSpace::freeDiskSpaceString;

cFreeDiskSpace FreeDiskSpace;

cString cFreeDiskSpace::lastPath("/");

bool cFreeDiskSpace::HasChanged(const char *SubDir, bool ForceCheck)
{
  cString path(ExchangeChars(strdup(SubDir ? SubDir : ""), true), true);
#if APIVERSNUM > 20101
  path = cString::sprintf("%s/%s", cVideoDirectory::Name(), *path);
#else
  path = cString::sprintf("%s/%s", VideoDirectory, *path);
#endif
  if (ForceCheck || time(NULL) - lastDiskSpaceCheck > DISKSPACECHEK || !EntriesOnSameFileSystem(path, lastPath)) {
     int FreeMB;
     int Percent;
     int MBperMinute = -1;
     lastPath = path;
#if APIVERSNUM > 20101
     if (cVideoDirectory::IsOnVideoDirectoryFileSystem(path)) {
        Percent = cVideoDirectory::VideoDiskSpace(&FreeMB);
#else
     if (IsOnVideoDirectoryFileSystem(path)) {
        Percent = ::VideoDiskSpace(&FreeMB);
#endif
        MBperMinute = Recordings.MBperMinute();
     }
     else {
        Percent = VideoDiskSpace(path, &FreeMB);
        MBperMinute = cFreeDiskSpace::MBperMinute(path);
     }

     lastDiskSpaceCheck = time(NULL);
     if (ForceCheck || FreeMB != lastFreeMB) {
        int Minutes;
        if (MBperMinute > 0)
           Minutes = int(double(FreeMB) / MBperMinute);
        else
           Minutes = int(double(FreeMB) / MB_PER_MINUTE);
        int Hours = Minutes / 60;
        Minutes %= 60;
        freeDiskSpaceString = cString::sprintf("%s %d%% - %2d:%02d %s", tr("Disk"), Percent, Hours, Minutes, tr("free"));
        lastFreeMB = FreeMB;
        return true;
        }
     }
  return false;
}

int cFreeDiskSpace::VideoDiskSpace(const char *Dir, int *FreeMB)
{
  int used = 0;
  int free = FreeDiskSpaceMB(Dir, &used);
  int deleted = DeletedFileSizeMB(Dir);
  if (deleted > used)
     deleted = used;
  free += deleted;
  used -= deleted;
  if (FreeMB)
     *FreeMB = free;
  return (free + used) ? used * 100 / (free + used) : 0;
}

int cFreeDiskSpace::DeletedFileSizeMB(const char *Dir)
{
  int size = 0;
  cThreadLock DeletedRecordingsLock(&DeletedRecordings);
  for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording)) {
      int fileSizeMB = DirSizeMB(recording->FileName());
      if (fileSizeMB > 0 && EntriesOnSameFileSystem(Dir, recording->FileName()))
         size += fileSizeMB;
      }
  return size;
}

double cFreeDiskSpace::MBperMinute(const char* Dir)
{
  int size = 0;
  int length = 0;
  cThreadLock RecordingsLock(&Recordings);
  for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
      if (EntriesOnSameFileSystem(Dir, recording->FileName())) {
         int FileSizeMB = recording->FileSizeMB();
         if (FileSizeMB > 0) {
            int LengthInSeconds = recording->LengthInSeconds();
            if (LengthInSeconds > 0) {
               size += FileSizeMB;
               length += LengthInSeconds;
               }
            }
         }
      }
  return (size && length) ? double(size) * 60 / length : -1;
}

cString cFreeDiskSpace::FreeDiskSpaceString(const cRecording* recording)
{
  int minutes = max(0, (recording->LengthInSeconds() + 30) / 60);
  int sizeMB = max(0, recording->FileSizeMB());
  cString freeString;
  if (minutes > 0 && sizeMB > 0) {
     int freeMB;
     VideoDiskSpace(recording->FileName(), &freeMB);
     int freeMinutes = int(double(freeMB) * minutes / sizeMB);
     freeString = cString::sprintf("%d MB - %d:%02d - %d:%02d %s", sizeMB, minutes / 60, minutes % 60, freeMinutes / 60, freeMinutes % 60, tr("free"));
  }
  else if (sizeMB >= 0)
     freeString = cString::sprintf("%d MB", sizeMB);
  else
     freeString = "";
  return freeString;
}

// --- cMenuEditRemoteTimer --------------------------------------------------------
class cMenuEditRemoteTimer : public cMenuEditTimer {
private:
  cTimer *timer;
  cMenuTimerItem **timerItem;
/*
  cTimer data;
  int channel;
  bool addIfConfirmed;
  cMenuEditDateItem *firstday;
  void SetFirstDayItem(void);
*/
  int remote;
  int tmpRemote;
  int user;
  int tmpUser;
  eOSState CheckState(eRemoteTimersState State);
public:
/*
  cMenuEditTimer(cTimer *Timer, bool New = false);
  virtual ~cMenuEditTimer();
*/
  virtual eOSState ProcessKey(eKeys Key);
  cMenuEditRemoteTimer(cTimer *Timer, bool Remote, bool New = false, cMenuTimerItem **TimerItem = NULL);
  };

cMenuEditRemoteTimer::cMenuEditRemoteTimer(cTimer *Timer, bool Remote, bool New, cMenuTimerItem **TimerItem)
:cMenuEditTimer(Timer, New), timer(Timer), timerItem(TimerItem)
{
  SetCols(15, 4, 12);
  remote = tmpRemote = Remote;
  user = tmpUser = New ? MASK_FROM_SETUP(RemoteTimersSetup.defaultUser) : cMenuTimerItem::ParseUser(timer);
  cOsdItem *item = new cMenuEditBoolItem(trREMOTETIMERS("Location"), &tmpRemote, trREMOTETIMERS("local"), trREMOTETIMERS("remote"));
  if (cSvdrp::GetInstance()->Offline())
      item->SetSelectable(false);
  Add(item, false, First());
  Add(new cMenuEditUserItem(trREMOTETIMERS("User ID"), &tmpUser), false, Get(8));

  if (!New) {
     cTimerConflicts *conflicts = Remote ? (cTimerConflicts*) &RemoteConflicts : (cTimerConflicts*) &LocalConflicts;
     int timerId = Remote ? ((cRemoteTimer *) Timer)->Id() : Timer->Index() + 1;
     for (cTimerConflict *c = conflicts->First(); c; c = conflicts->Next(c)) {
         if (timerId == c->Id()) {
            Add(new cOsdItem(cString::sprintf(trREMOTETIMERS("%.10s\tConflict (Recording %d%%)"),
			*DayDateTime(c->Time()),
			 c->Percent()), osUnknown, false));
            for (const int* with = c->With(); with && *with; ++with) {
                if (*with == timerId)
                   continue;
                const cTimer *t = Remote ? RemoteTimers.GetTimer(*with) : Timers.Get(*with - 1);
                if (t) {
                   Add(new cOsdItem(cString::sprintf("%02d:%02d %02d:%02d %2d\t%d\t%s\t%s",
				t->Start() / 100,
				t->Start() % 100,
				t->Stop() / 100,
				t->Stop() % 100,
				t->Priority(),
				t->Channel()->Number(),
				t->Channel()->ShortName(true),
				t->File())));
                }
            }
         }
     }
  }
}

eOSState cMenuEditRemoteTimer::CheckState(eRemoteTimersState State)
{
  if (State > rtsRefresh) {
     Skins.Message(State == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(State)));
     return osContinue;
     }
  return osBack;
}

eOSState cMenuEditRemoteTimer::ProcessKey(eKeys Key)
{
  int TimerNumber = Timers.Count();
  eOSState state = cMenuEditTimer::ProcessKey(Key);
  if (state == osBack && Key == kOk) {
     // changes have been confirmed
     if (user != tmpUser)
        cMenuTimerItem::UpdateUser(*timer, tmpUser);
     if (TimerNumber == Timers.Count()) {
        // editing existing timer (remote timers are also added to Timers, first)
        if (remote == tmpRemote) {
           // timer was not moved
           if (tmpRemote) {
              state = CheckState(RemoteTimers.Modify((cRemoteTimer*) timer));
              RemoteConflicts.SetNeedsUpdate();
              }
           else
              LocalConflicts.SetNeedsUpdate();
           }
        else if (tmpRemote) {
           // move local to remote
           if (timer->Recording()) {
              Skins.Message(mtError, trREMOTETIMERS("Timer is recording - can't move it to server"));
              state = osContinue;
              }
           else {
              cRemoteTimer *rt = new cRemoteTimer();
              *(cTimer*) rt = *timer;
              if ((state = CheckState(RemoteTimers.New(rt))) == osBack) {
                 Timers.Del(timer);
                 timer = rt;
                 RemoteConflicts.SetNeedsUpdate();
                 LocalConflicts.SetNeedsUpdate();
                 }
              else
                 delete rt;
              }
           }
        else {
           // move remote to local
           cTimer *lt = new cTimer();
           *lt = *(cTimer*) timer;
           if ((state = CheckState(RemoteTimers.Delete((cRemoteTimer*) timer))) == osBack) {
              Timers.Add(lt);
              timer = lt;
              RemoteConflicts.SetNeedsUpdate();
              LocalConflicts.SetNeedsUpdate();
              }
           else
              delete lt;
           }
        if (timerItem && state == osBack)
           (*timerItem)->Update(timer, tmpUser, tmpRemote);
        }
     else {
        // local timer has been added
        if (tmpRemote) {
           // move to remote
           cRemoteTimer *rt = new cRemoteTimer();
           *(cTimer*) rt = *timer;
           if ((state = CheckState(RemoteTimers.New(rt))) == osBack) {
              Timers.Del(timer);
              timer = rt;
              RemoteConflicts.SetNeedsUpdate();
              }
           else {
              delete rt;
              LocalConflicts.SetNeedsUpdate();
              }
           }
        else
           LocalConflicts.SetNeedsUpdate();
        if (timerItem && state == osBack)
           *timerItem = new cMenuTimerItem(timer, tmpUser, tmpRemote);
        }
     }
  return state;
}

// --- cMenuTimerItem --------------------------------------------------------

/*
class cMenuTimerItem : public cOsdItem {
private:
  cTimer *timer;
public:
  cMenuTimerItem(cTimer *Timer);
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  cTimer *Timer(void) { return timer; }
  virtual void SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable);
  };
*/

cMenuTimerItem::cMenuTimerItem(cTimer *Timer, int User, bool Remote)
{
  Update(Timer, User, Remote);
  UpdateConflict();
  Set();
}

void cMenuTimerItem::Update(cTimer *Timer, int User, bool Remote)
{
  timer = Timer;
  user = User;
  remote = Remote;
}

bool cMenuTimerItem::UpdateConflict()
{
  bool conflictOld = conflict;
  conflict = remote ? RemoteConflicts.HasConflict(((cRemoteTimer*) timer)->Id()) : LocalConflicts.HasConflict(timer->Index() + 1);
  return conflict != conflictOld;
}

int cMenuTimerItem::Compare(const cListObject &ListObject) const
{
  return timer->Compare(*((cMenuTimerItem *)&ListObject)->timer);
}

void cMenuTimerItem::Set(void)
{
  cString day, name("");
  if (timer->WeekDays())
     day = timer->PrintDay(0, timer->WeekDays(), false);
  else if (timer->Day() - time(NULL) < 28 * SECSINDAY) {
     day = itoa(timer->GetMDay(timer->Day()));
     name = WeekDayName(timer->Day());
     }
  else {
     struct tm tm_r;
     time_t Day = timer->Day();
     localtime_r(&Day, &tm_r);
     char buffer[16];
     strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
     day = buffer;
     }
  const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(timer->File(), FOLDERDELIMCHAR);
  if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
     File++;
  else
     File = timer->File();
  // TRANSLATORS: Indicator for (R)emote or (L)ocal timer in timers list
  const char *RL = trREMOTETIMERS("RL");
  SetText(cString::sprintf("%c%c\t%c%d\t%s%s%s\t%02d:%02d\t%02d:%02d\t%s",
                    !(timer->HasFlags(tfActive)) ? ' ' : timer->FirstDay() ? '!' : timer->Recording() ? '#' : '>',
                    conflict ? '%' : ' ',
                    RL[remote ? 0 : 1],
                    timer->Channel()->Number(),
                    *name,
                    *name && **name ? " " : "",
                    *day,
                    timer->Start() / 100,
                    timer->Start() % 100,
                    timer->Stop() / 100,
                    timer->Stop() % 100,
                    File));
}

int cMenuTimerItem::ParseUser(const cTimer *Timer) {
  return PluginRemoteTimers::ParseUser(Timer->Aux());
}

void cMenuTimerItem::UpdateUser(cTimer& Timer, int User) {
  cString origTimer = Timer.ToText();
  cString modTimer(PluginRemoteTimers::UpdateUser(*origTimer, User));
  Timer.Parse(*modTimer);
}

void cMenuTimerItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable)
{
  if (!RemoteTimersSetup.skinTimers || !DisplayMenu->SetItemTimer(timer, Index, Current, Selectable))
     DisplayMenu->SetItem(Text(), Index, Current, Selectable);
}

// --- cMenuTimers -----------------------------------------------------------

/*
class cMenuTimers : public cOsdMenu {
private:
  int helpKeys;
  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState OnOff(void);
  eOSState Info(void);
  cTimer *CurrentTimer(void);
  void SetHelpKeys(void);
public:
  cMenuTimers(void);
  virtual ~cMenuTimers();
  virtual eOSState ProcessKey(eKeys Key);
  };
*/

cMenuTimers::cMenuTimers(const char* ServerIp, unsigned short ServerPort)
:cOsdMenu(tr("Timers"), 3, CHNUMWIDTH + 1, 10, 6, 6)
{
  SetMenuCategory(RemoteTimersSetup.skinTimers ? mcTimer : mcPlugin);
  helpKeys = -1;
  userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterTimers);
  currentItem = NULL;
  if (!cSvdrp::GetInstance()->Connect(ServerIp, ServerPort))
      Skins.Message(mtWarning, tr(MSG_UNAVAILABLE));
  Set();
  /*
  for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
      timer->SetEventFromSchedule(); // make sure the event is current
      Add(new cMenuTimerItem(timer));
      }
  Sort();
  SetCurrent(First());
  SetHelpKeys();
  */
  Timers.IncBeingEdited();
}

cMenuTimers::~cMenuTimers()
{
  Timers.DecBeingEdited();
  cSvdrp::GetInstance()->Disconnect();
}

void cMenuTimers::Set(eRemoteTimersState Msg)
{
  cString currentTimerString;
  int currentIndex = Current();
  if (currentIndex >= 0)
     currentTimerString = ((cMenuTimerItem *) Get(currentIndex))->Timer()->ToText();
  Clear();
  LocalConflicts.Update();
  RemoteConflicts.Update();
  eRemoteTimersState state = cSvdrp::GetInstance()->Offline() ? rtsOk : RemoteTimers.Refresh();
  if (state == rtsOk) {
      for (cRemoteTimer *timer = RemoteTimers.First(); timer; timer = RemoteTimers.Next(timer)) {
          int user = cMenuTimerItem::ParseUser(timer);
          if (userFilter == 0 || user == 0 || (user & USER_MASK(userFilter))) {
              timer->SetEventFromSchedule(); // make sure the event is current
              Add(new cMenuTimerItem(timer, user, true));
              }
          }
      }
  for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
      int user = cMenuTimerItem::ParseUser(timer);
      if (userFilter == 0 || user == 0 || (user & USER_MASK(userFilter))) {
          timer->SetEventFromSchedule(); // make sure the event is current
          Add(new cMenuTimerItem(timer, user, false));
          }
      }
  Sort();

  cOsdItem *currentItem = NULL;
  // if we have been editing a timer make it current
  if (*currentTimerString) {
     for (cOsdItem *item = First(); item && !currentItem; item = Next(item)) {
        if (strcmp(*currentTimerString, ((cMenuTimerItem *)item)->Timer()->ToText()) == 0)
           currentItem = item;
        }
     }
  // make the previous position current
  if (!currentItem)
     currentItem = Get(currentIndex < Count() ? currentIndex : Count() - 1);
  // use first
  if (!currentItem)
     currentItem = First();

  SetCurrent(currentItem);
  SetHelpKeys();
  if (userFilter == 0)
     SetTitle(tr("Timers"));
  else
     SetTitle(cString::sprintf("%s - %s %d", tr("Timers"), trREMOTETIMERS("User"), userFilter));

  if (Msg > state)
     state = Msg;
  if (state != rtsOk)
     Skins.Message(state == rtsRefresh ? mtInfo : state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
}

void cMenuTimers::CheckState(eRemoteTimersState State, bool RefreshMsg)
{
  if (State == rtsRefresh && !RefreshMsg)
     Set(rtsOk);
  else if (State != rtsOk)
     Set(State);
}

cMenuTimerItem *cMenuTimers::CurrentItem(void)
{
  return (cMenuTimerItem *)Get(Current());
}

cTimer *cMenuTimers::CurrentTimer(void)
{
  cMenuTimerItem *item = (cMenuTimerItem *)Get(Current());
  return item ? item->Timer() : NULL;
}

bool cMenuTimers::UpdateConflicts(bool Remote)
{
  bool updated = false;
  Remote ? RemoteConflicts.Update() : LocalConflicts.Update();
  for (cOsdItem *item = First(); item; item = Next(item)) {
      cMenuTimerItem *i = (cMenuTimerItem *) item;
      if (i->Remote() == Remote && i->UpdateConflict()) {
         i->Set();
         updated = true;
         }
      }
  return updated;
}

void cMenuTimers::SetHelpKeys(void)
{
  int NewHelpKeys = 0;
  cTimer *timer = CurrentTimer();
  if (timer) {
     if (timer->Event())
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     cMenuTimerItem *item = CurrentItem();
     if (userFilter != 0 && item->User() != 0 && item->User() != USER_MASK(userFilter))
        NewHelpKeys += 2;
     }
  if (NewHelpKeys != helpKeys) {
     helpKeys = NewHelpKeys;
     // TRANSLATORS: Button displayed instead of "Delete" when there are other users who subscribed to the timer
     SetHelp(helpKeys > 0 ? tr("Button$On/Off") : NULL, tr("Button$New"), helpKeys > 0 ? (helpKeys > 2 ? trREMOTETIMERS("Cancel") : tr("Button$Delete")) : NULL, helpKeys == 2 ? tr("Button$Info") : NULL);
     }
}

eOSState cMenuTimers::OnOff(void)
{
  if (HasSubMenu())
     return osContinue;
  cMenuTimerItem *item = CurrentItem();
  cTimer *timer = CurrentTimer();
  if (timer) {
     timer->OnOff();
     timer->SetEventFromSchedule();
     if (item->Remote())
        CheckState(RemoteTimers.Modify((cRemoteTimer*)timer));
     RefreshCurrent();
     DisplayCurrent(true);
     if (timer->FirstDay())
        isyslog("timer %s first day set to %s", *timer->ToDescr(), *timer->PrintFirstDay());
     else
        isyslog("timer %s %sactivated", *timer->ToDescr(), timer->HasFlags(tfActive) ? "" : "de");
     if (item->Remote()) {
        if (UpdateConflicts(true))
           Display();
        }
     else {
        Timers.SetModified();
        if (UpdateConflicts(false))
           Display();
        }
     }
  return osContinue;
}

eOSState cMenuTimers::Edit(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  isyslog("editing timer %s", *CurrentTimer()->ToDescr());
  currentItem = CurrentItem();
  return AddSubMenu(new cMenuEditRemoteTimer(CurrentTimer(), CurrentItem()->Remote(), false, &currentItem));
}

eOSState cMenuTimers::New(void)
{
  if (HasSubMenu())
     return osContinue;
  currentItem = NULL;
  return AddSubMenu(new cMenuEditRemoteTimer(new cTimer, cSvdrp::GetInstance()->Offline() ? false : RemoteTimersSetup.addToRemote, true, &currentItem));
}

eOSState cMenuTimers::Delete(void)
{
  // Check if this timer is active:
  cMenuTimerItem *item = CurrentItem();
  cTimer *ti = CurrentTimer();
  if (ti) {
     if (userFilter != 0 && item->User() != 0 && item->User() != USER_MASK(userFilter)) {
	cMenuTimerItem::UpdateUser(*ti, item->User() ^ USER_MASK(userFilter));
        if (item->Remote())
           CheckState(RemoteTimers.Modify((cRemoteTimer*)ti), false);
        else
           Timers.SetModified();
        cOsdMenu::Del(Current());
        Display();
        }
     else if (Interface->Confirm(tr("Delete timer?"))) {
        if (ti->Recording() && !item->Remote()) {
           if (Interface->Confirm(tr("Timer still recording - really delete?"))) {
              ti->Skip();
              cRecordControls::Process(time(NULL));
              }
           else
              return osContinue;
           }
        isyslog("deleting timer %s", *ti->ToDescr());
        if (item->Remote()) {
           CheckState(RemoteTimers.Delete((cRemoteTimer*)ti), false);
           UpdateConflicts(true);
           }
        else {
           Timers.Del(ti);
           cOsdMenu::Del(Current());
           Timers.SetModified();
           UpdateConflicts(false);
           }
        Display();
        }
     }
  return osContinue;
}

eOSState cMenuTimers::Info(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cTimer *ti = CurrentTimer();
  if (ti && ti->Event())
     return AddSubMenu(new cMenuEvent(ti->Event()));
  return osContinue;
}

eOSState cMenuTimers::ProcessKey(eKeys Key)
{
  //int TimerNumber = HasSubMenu() ? Count() : -1;
  // cMenuTimers::New() sets currentItem to NULL
  int TimerNumber = HasSubMenu() && !currentItem ? Count() : -1;
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     return Edit();
       case kRed:    state = OnOff(); break; // must go through SetHelpKeys()!
       case kGreen:  return New();
       case kYellow: state = Delete(); break;
       case kInfo:
       case kBlue:   return Info();
                     break;
       case k0 ... k9:
                     userFilter = Key - k0;
		     if (RemoteTimersSetup.userFilterTimers < 0 && ::Setup.ResumeID != userFilter) {
		        ::Setup.ResumeID = userFilter;
			Recordings.ResetResume();
			}
                     Set();
                     Display();
                     return osContinue;
       default: break;
       }
     }
  bool display = false;
  if (LocalConflicts.NeedsUpdate()) {
     UpdateConflicts(false);
     display = true;
     }
  if (RemoteConflicts.NeedsUpdate()) {
     UpdateConflicts(true);
     display = true;
     }
  //if (TimerNumber >= 0 && !HasSubMenu() && Timers.Get(TimerNumber)) {
  if (TimerNumber >= 0 && !HasSubMenu() && currentItem) {
     // a newly created timer was confirmed with Ok
     Add(currentItem, true);
     //Add(new cMenuTimerItem(Timers.Get(TimerNumber)), true);
     Set();
     //Display();
     display = true;
     }
  if (display)
     Display();
  if (Key != kNone)
     SetHelpKeys();
  return state;
}

// --- cMenuEvent ------------------------------------------------------------

cMenuEvent::cMenuEvent(const cEvent *Event, bool CanSwitch, bool Buttons)
:cOsdMenu(tr("Event"))
{
  SetMenuCategory(RemoteTimersSetup.skinSchedule ? mcEvent : mcPlugin);
  event = Event;
  if (event) {
     cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true);
     if (channel) {
        SetTitle(channel->Name());
        eTimerMatch TimerMatch = tmNone;
        //Timers.GetMatch(event, &TimerMatch);
	GetBestMatch(event, MASK_FROM_SETUP(RemoteTimersSetup.userFilterSchedule), &TimerMatch, NULL, NULL);
        if (Buttons)
           SetHelp(TimerMatch == tmFull ? tr("Button$Timer") : tr("Button$Record"), NULL, NULL, CanSwitch ? tr("Button$Switch") : NULL);
        }
     }
}

void cMenuEvent::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetEvent(event);
#ifdef USE_GRAPHTFT
  cStatus::MsgOsdSetEvent(event);
#endif

  if (event->Description())
     cStatus::MsgOsdTextItem(event->Description());
}

eOSState cMenuEvent::ProcessKey(eKeys Key)
{
  switch (int(Key)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft);
                  return osContinue;
    case kInfo:   return osBack;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kGreen:
       case kYellow: return osContinue;
       case kOk:     return osBack;
       default: break;
       }
     }
  return state;
}

// --- cMenuScheduleItem -----------------------------------------------------

class cMenuScheduleItem : public cOsdItem {
public:
  enum eScheduleSortMode { ssmAllThis, ssmThisThis, ssmThisAll, ssmAllAll }; // "which event(s) on which channel(s)"
private:
  static eScheduleSortMode sortMode;
public:
  const cEvent *event;
  const cChannel *channel;
  bool withDate;
  bool withBar;
  eTimerMatch timerMatch;
  int timerType;
  cMenuScheduleItem(const cEvent *Event, cChannel *Channel = NULL, bool WithDate = false, bool WithBar = false);
  static void SetSortMode(eScheduleSortMode SortMode) { sortMode = SortMode; }
  static void IncSortMode(void) { sortMode = eScheduleSortMode((sortMode == ssmAllAll) ? ssmAllThis : sortMode + 1); }
  static eScheduleSortMode SortMode(void) { return sortMode; }
  virtual int Compare(const cListObject &ListObject) const;
  bool Update(bool Force = false);
  virtual void SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable);
  };

cMenuScheduleItem::eScheduleSortMode cMenuScheduleItem::sortMode = ssmAllThis;

cMenuScheduleItem::cMenuScheduleItem(const cEvent *Event, cChannel *Channel, bool WithDate, bool WithBar)
{
  event = Event;
  channel = Channel;
  withDate = WithDate;
  withBar = WithBar;
  timerMatch = tmNone;
  timerType = 0;
  Update(true);
}

int cMenuScheduleItem::Compare(const cListObject &ListObject) const
{
  cMenuScheduleItem *p = (cMenuScheduleItem *)&ListObject;
  int r = -1;
  if (sortMode != ssmAllThis)
     r = strcoll(event->Title(), p->event->Title());
  if (sortMode == ssmAllThis || r == 0)
     r = event->StartTime() - p->event->StartTime();
  return r;
}

static const char * const ProgressBar[] =
{
  "[      ]",
  "[|     ]",
  "[||    ]",
  "[|||   ]",
  "[||||  ]",
  "[||||| ]",
  "[||||||]"
};
#define MAXPROGRESS ((int) (sizeof(ProgressBar) / sizeof(char *)))

//static const char *TimerMatchChars = " tT";
// TRANSLATORS: Schedule timer match characters (lower case for partial match): no match (blank), normal (tT)imer, (pP)rivate timer
static const char *TimerMatchChars = trREMOTETIMERS(" tTpP");

bool cMenuScheduleItem::Update(bool Force)
{
  bool result = false;
  eTimerMatch oldTimerMatch = timerMatch;
  //Timers.GetMatch(event, &timerMatch);
  int oldTimerType = timerType;
  GetBestMatch(event, MASK_FROM_SETUP(RemoteTimersSetup.userFilterSchedule), &timerMatch, &timerType, NULL);
  if (Force || timerMatch != oldTimerMatch || timerType != oldTimerType) {
     cString buffer;
     char t = TimerMatchChars[timerMatch + 2 * timerType];
     char v = event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
     char r = event->SeenWithin(30) && event->IsRunning() ? '*' : ' ';
     const char *csn = channel ? channel->ShortName(true) : NULL;
     cString eds = event->GetDateString();
#define CSN_SYMBOLS 999
     if (channel && withDate)
        buffer = cString::sprintf("%d\t%.*s\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, CSN_SYMBOLS), csn, Utf8SymChars(eds, 6), *eds, *event->GetTimeString(), t, v, r, event->Title());
     else if (channel) {
	if (withBar && RemoteTimersSetup.showProgressBar) {
	   int progress = (time(NULL) - event->StartTime()) * MAXPROGRESS / event->Duration();
	   progress = progress < 0 ? 0 : progress >= MAXPROGRESS ? MAXPROGRESS - 1 : progress;
           buffer = cString::sprintf("%d\t%.*s\t%s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, CSN_SYMBOLS), csn, *event->GetTimeString(), ProgressBar[progress], t, v, r, event->Title());
	   }
	else
           buffer = cString::sprintf("%d\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, CSN_SYMBOLS), csn, *event->GetTimeString(), t, v, r, event->Title());
	}
     else
        buffer = cString::sprintf("%.*s\t%s\t%c%c%c\t%s", Utf8SymChars(eds, 6), *eds, *event->GetTimeString(), t, v, r, event->Title());
     SetText(buffer);
     result = true;
     }
  return result;
}

void cMenuScheduleItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable)
{
  if (!RemoteTimersSetup.skinSchedule || !DisplayMenu->SetItemEvent(event, Index, Current, Selectable, channel, withDate, timerMatch))
     DisplayMenu->SetItem(Text(), Index, Current, Selectable);
}

// --- cMenuWhatsOn ----------------------------------------------------------

class cMenuWhatsOn : public cOsdMenu {
private:
  //bool now;
  int whatsOnId;
  int helpKeys;
  int timerState;
  eOSState Record(void);
  eOSState Switch(void);
  static int currentChannel;
  static const cEvent *scheduleEvent;
  bool Update(void);
  void SetHelpKeys(void);
  time_t GetTime(int SecondsFromMidnight);
public:
  cMenuWhatsOn(const cSchedules *Schedules, int WhatsOnId, int CurrentChannelNr);
  static int CurrentChannel(void) { return currentChannel; }
  static void SetCurrentChannel(int ChannelNr) { currentChannel = ChannelNr; }
  static const cEvent *ScheduleEvent(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

int cMenuWhatsOn::currentChannel = 0;
const cEvent *cMenuWhatsOn::scheduleEvent = NULL;

#define NOW  (WhatsOnId == EPGTIME_LENGTH)
#define NEXT (WhatsOnId == EPGTIME_LENGTH + 1)
#define EPGTIMESTR(i) *cString::sprintf("%02d:%02d", RemoteTimersSetup.epgTime[i] / 100, RemoteTimersSetup.epgTime[i] % 100)
#define EPGTIMESEC ((RemoteTimersSetup.epgTime[whatsOnId] / 100) * 3600 + (RemoteTimersSetup.epgTime[whatsOnId] % 100) * 60)

cMenuWhatsOn::cMenuWhatsOn(const cSchedules *Schedules, int WhatsOnId, int CurrentChannelNr)
:cOsdMenu(NOW ? tr("What's on now?") : NEXT ? tr("What's on next?") : *cString::sprintf(trREMOTETIMERS("What's on at %s?"), EPGTIMESTR(WhatsOnId)), CHNUMWIDTH, CHNAMWIDTH, 6, 4, 4)
{
  SetMenuCategory(RemoteTimersSetup.skinSchedule ? (NOW ? mcScheduleNow : mcScheduleNext) : mcPlugin);
  whatsOnId = WhatsOnId;
  helpKeys = -1;
  timerState = 0;
  Timers.Modified(timerState);
  for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
      if (!Channel->GroupSep()) {
         const cSchedule *Schedule = Schedules->GetSchedule(Channel);
         if (Schedule) {
            const cEvent *Event = NOW ? Schedule->GetPresentEvent() : NEXT ? Schedule->GetFollowingEvent() : Schedule->GetEventAround(GetTime(EPGTIMESEC));
            if (Event)
               Add(new cMenuScheduleItem(Event, Channel, false, NOW), Channel->Number() == CurrentChannelNr);
            }
         }
      }
  currentChannel = CurrentChannelNr;
  Display();
  SetHelpKeys();
  int userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterSchedule);
  if (userFilter != 0)
        SetTitle(cString::sprintf("%s %s %d", NOW ? tr("What's on now?") : NEXT ? tr("What's on next?") : *cString::sprintf(trREMOTETIMERS("What's on at %s?"), EPGTIMESTR(whatsOnId)), trREMOTETIMERS("User"), userFilter));
}

time_t cMenuWhatsOn::GetTime(int SecondsFromMidnight)
{
  time_t t = time(NULL);
  struct tm tm;
  localtime_r(&t, &tm);
  if (tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec > SecondsFromMidnight)
     tm.tm_mday++;
  tm.tm_hour = SecondsFromMidnight / 3600;
  tm.tm_min = (SecondsFromMidnight % 3600) / 60;
  tm.tm_sec = SecondsFromMidnight % 60;
  tm.tm_isdst = -1;
  return mktime(&tm);
}

bool cMenuWhatsOn::Update(void)
{
  bool result = false;
  if (Timers.Modified(timerState)) {
     for (cOsdItem *item = First(); item; item = Next(item)) {
         if (((cMenuScheduleItem *)item)->Update())
            result = true;
         }
     }
  return result;
}

void cMenuWhatsOn::SetHelpKeys(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->timerMatch == tmFull)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     static cString GreenTime;
     int nextId = (whatsOnId + 1) % (EPGTIME_LENGTH + 2);
     while (nextId < EPGTIME_LENGTH && !RemoteTimersSetup.epgTime[nextId])
        ++nextId;
     if (nextId < EPGTIME_LENGTH)
        GreenTime = EPGTIMESTR(nextId);
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], nextId == EPGTIME_LENGTH ? tr("Button$Now") : nextId > EPGTIME_LENGTH ? tr("Button$Next") : *GreenTime, tr("Button$Schedule"), RemoteTimersSetup.swapOkBlue ? tr("Button$Info") : tr("Button$Switch"));
     helpKeys = NewHelpKeys;
     }
}

const cEvent *cMenuWhatsOn::ScheduleEvent(void)
{
  const cEvent *ei = scheduleEvent;
  scheduleEvent = NULL;
  return ei;
}

eOSState cMenuWhatsOn::Switch(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     cChannel *channel = Channels.GetByChannelID(item->event->ChannelID(), true);
     if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
        return osEnd;
     }
  Skins.Message(mtError, tr("Can't switch channel!"));
  return osContinue;
}

eOSState cMenuWhatsOn::Record(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     eTimerMatch tm = tmNone;
     bool isRemote = false;
     if (item->timerMatch == tmFull) {
        //cTimer *timer = Timers.GetMatch(item->event, &tm);
        cTimer *timer = GetBestMatch(item->event, MASK_FROM_SETUP(RemoteTimersSetup.userFilterSchedule), &tm, NULL, &isRemote);
        if (timer) {
           isRemote ? RemoteConflicts.Update() : LocalConflicts.Update();
           return AddSubMenu(new cMenuEditRemoteTimer(timer, isRemote));
           }
        }

     tm = tmNone;
     isRemote = true;
     cTimer *timer = new cTimer(item->event);
     cTimer *t = GetBestMatch(item->event, 0, &tm, NULL, &isRemote);
     if (!t || tm != tmFull) {
        isRemote = true;
        t = RemoteTimers.GetTimer(timer);
	}
     if (!t) {
        isRemote = false;
        t = Timers.GetTimer(timer);
        }
     if (t) {
        delete timer;
        timer = t;
	if (tm != tmFull) 
           return AddSubMenu(new cMenuEditRemoteTimer(timer, isRemote));
	else {
           // matching timer, but it was filtered
	   int user = cMenuTimerItem::ParseUser(t);
	   cMenuTimerItem::UpdateUser(*t, user | MASK_FROM_SETUP(RemoteTimersSetup.defaultUser));
	   if (isRemote) {
	      eRemoteTimersState state = RemoteTimers.Modify((cRemoteTimer*)t);
	      if (state > rtsRefresh)
	         Skins.Message(state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
	      }
	   else {
	      Timers.SetModified();
	      }
	   }
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     else {
        isRemote = false;
        cMenuTimerItem::UpdateUser(*timer, MASK_FROM_SETUP(RemoteTimersSetup.defaultUser));
        Timers.Add(timer);
        if (RemoteTimersSetup.addToRemote && !cSvdrp::GetInstance()->Offline()) {
           isRemote = true;
           cRemoteTimer *rt = new cRemoteTimer();
           *(cTimer*) rt = *timer;
           eRemoteTimersState state = RemoteTimers.New(rt);
           if (state <= rtsRefresh) {
              Timers.Del(timer);
              timer = rt;
              }
           else {
              Skins.Message(state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
              Timers.Del(timer);
              delete rt;
              return osContinue;
              }
           }
        Timers.SetModified();
        isyslog("timer %s added (active)", *timer->ToDescr());
        if (timer->Matches(0, false, NEWTIMERLIMIT)) {
           isRemote ? RemoteConflicts.Update() : LocalConflicts.Update();
           return AddSubMenu(new cMenuEditRemoteTimer(timer, isRemote));
        }
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     }
  return osContinue;
}

eOSState cMenuWhatsOn::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRecord:
       case kRed:    return Record();
       case kYellow: state = osBack;
                     // continue with kGreen
       case kGreen:  {
                       cMenuScheduleItem *mi = (cMenuScheduleItem *)Get(Current());
                       if (mi) {
                          scheduleEvent = mi->event;
                          currentChannel = mi->channel->Number();
                          }
                     }
                     break;
       case kBlue:
       case kOk:     if ((Key == kOk && RemoteTimersSetup.swapOkBlue) || (Key == kBlue && !RemoteTimersSetup.swapOkBlue))
                        return Switch();
       case kInfo:   if (Count())
                        return AddSubMenu(new cMenuEvent(((cMenuScheduleItem *)Get(Current()))->event, true, true));
                     break;
       default:      break;
       }
     }
  else if (!HasSubMenu()) {
     if (HadSubMenu && Update())
        Display();
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuSchedule ---------------------------------------------------------

/*
class cMenuSchedule : public cOsdMenu {
private:
  cSchedulesLock schedulesLock;
  const cSchedules *schedules;
  bool now, next;
  int otherChannel;
  int helpKeys;
  int timerState;
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
*/

cMenuSchedule::cMenuSchedule(const char* ServerIp, unsigned short ServerPort)
:cOsdMenu("")
{
  SetMenuCategory(RemoteTimersSetup.skinSchedule ? mcSchedule : mcPlugin);
  //now = next = false;
  whatsOnId = -1;
  otherChannel = 0;
  helpKeys = -1;
  timerState = 0;
  userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterSchedule);
  Timers.Modified(timerState);
  cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
  if (!cSvdrp::GetInstance()->Connect(ServerIp, ServerPort) || RemoteTimers.Refresh() != rtsOk)
     Skins.Message(mtWarning, tr(MSG_UNAVAILABLE));
  cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (channel) {
     cMenuWhatsOn::SetCurrentChannel(channel->Number());
     schedules = cSchedules::Schedules(schedulesLock);
     PrepareScheduleAllThis(NULL, channel);
     SetHelpKeys();
     }
}

cMenuSchedule::~cMenuSchedule()
{
  cMenuWhatsOn::ScheduleEvent(); // makes sure any posted data is cleared
  cSvdrp::GetInstance()->Disconnect();
}

void cMenuSchedule::PrepareScheduleAllThis(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(7, 6, 4);
  cString title(cString::sprintf(tr("Schedule - %s"), Channel->Name()));
  if (userFilter != 0)
     title = cString::sprintf("%s - %s %d", *title, trREMOTETIMERS("User"), userFilter);
  SetTitle(title);
  if (schedules && Channel) {
     const cSchedule *Schedule = schedules->GetSchedule(Channel);
     if (Schedule) {
        const cEvent *PresentEvent = Event ? Event : Schedule->GetPresentEvent();
        time_t now = time(NULL) - Setup.EPGLinger * 60;
        for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
            if (ev->EndTime() > now || ev == PresentEvent)
               Add(new cMenuScheduleItem(ev), ev == PresentEvent);
            }
        }
     }
}

void cMenuSchedule::PrepareScheduleThisThis(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(7, 6, 4);
  cString title(cString::sprintf(tr("This event - %s"), Channel->Name()));
  if (userFilter != 0)
     title = cString::sprintf("%s - %s %d", *title, trREMOTETIMERS("User"), userFilter);
  SetTitle(title);
  if (schedules && Channel && Event) {
     const cSchedule *Schedule = schedules->GetSchedule(Channel);
     if (Schedule) {
        time_t now = time(NULL) - Setup.EPGLinger * 60;
        for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
            if ((ev->EndTime() > now || ev == Event) && !strcmp(ev->Title(), Event->Title()))
               Add(new cMenuScheduleItem(ev), ev == Event);
            }
        }
     }
}

void cMenuSchedule::PrepareScheduleThisAll(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(CHNUMWIDTH, CHNAMWIDTH, 7, 6, 4);
  cString title(tr("This event - all channels"));
  if (userFilter != 0)
     title = cString::sprintf("%s - %s %d", *title, trREMOTETIMERS("User"), userFilter);
  SetTitle(title);
  if (schedules && Event) {
     for (cChannel *ch = Channels.First(); ch; ch = Channels.Next(ch)) {
         const cSchedule *Schedule = schedules->GetSchedule(ch);
         if (Schedule) {
            time_t now = time(NULL) - Setup.EPGLinger * 60;
            for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
                if ((ev->EndTime() > now || ev == Event) && !strcmp(ev->Title(), Event->Title()))
                   Add(new cMenuScheduleItem(ev, ch, true), ev == Event && ch == Channel);
                }
            }
         }
     }
}

void cMenuSchedule::PrepareScheduleAllAll(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(CHNUMWIDTH, CHNAMWIDTH, 7, 6, 4);
  cString title(tr("All events - all channels"));
  if (userFilter != 0)
     title = cString::sprintf("%s - %s %d", *title, trREMOTETIMERS("User"), userFilter);
  SetTitle(title);
  if (schedules) {
     for (cChannel *ch = Channels.First(); ch; ch = Channels.Next(ch)) {
         const cSchedule *Schedule = schedules->GetSchedule(ch);
         if (Schedule) {
            time_t now = time(NULL) - Setup.EPGLinger * 60;
            for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
                if (ev->EndTime() > now || ev == Event)
                   Add(new cMenuScheduleItem(ev, ch, true), ev == Event && ch == Channel);
                }
            }
         }
     }
}

bool cMenuSchedule::Update(void)
{
  bool result = false;
  if (Timers.Modified(timerState)) {
     for (cOsdItem *item = First(); item; item = Next(item)) {
         if (((cMenuScheduleItem *)item)->Update())
            result = true;
         }
     }
  return result;
}

void cMenuSchedule::SetHelpKeys(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->timerMatch == tmFull)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], tr("Button$Now"), tr("Button$Next"));
     helpKeys = NewHelpKeys;
     }
}

eOSState cMenuSchedule::Number(void)
{
  cMenuScheduleItem::IncSortMode();
  cMenuScheduleItem *CurrentItem = (cMenuScheduleItem *)Get(Current());
  const cChannel *Channel = NULL;
  const cEvent *Event = NULL;
  if (CurrentItem) {
     Event = CurrentItem->event;
     Channel = Channels.GetByChannelID(Event->ChannelID(), true);
     }
  else
     Channel = Channels.GetByNumber(cDevice::CurrentChannel());
  switch (cMenuScheduleItem::SortMode()) {
    case cMenuScheduleItem::ssmAllThis:  PrepareScheduleAllThis(Event, Channel); break;
    case cMenuScheduleItem::ssmThisThis: PrepareScheduleThisThis(Event, Channel); break;
    case cMenuScheduleItem::ssmThisAll:  PrepareScheduleThisAll(Event, Channel); break;
    case cMenuScheduleItem::ssmAllAll:   PrepareScheduleAllAll(Event, Channel); break;
    default: esyslog("ERROR: unknown SortMode %d (%s %d)", cMenuScheduleItem::SortMode(), __FUNCTION__, __LINE__);
    }
  CurrentItem = (cMenuScheduleItem *)Get(Current());
  Sort();
  SetCurrent(CurrentItem);
  Display();
  return osContinue;
}

eOSState cMenuSchedule::Record(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     eTimerMatch tm = tmNone;
     bool isRemote = false;
     if (item->timerMatch == tmFull) {
        //cTimer *timer = Timers.GetMatch(item->event, &tm);
        cTimer *timer = GetBestMatch(item->event, MASK_FROM_SETUP(RemoteTimersSetup.userFilterSchedule), &tm, NULL, &isRemote);
        if (timer) {
           isRemote ? RemoteConflicts.Update() : LocalConflicts.Update();
           return AddSubMenu(new cMenuEditRemoteTimer(timer, isRemote));
           }
        }

     tm = tmNone;
     isRemote = true;
     cTimer *timer = new cTimer(item->event);
     cTimer *t = GetBestMatch(item->event, 0, &tm, NULL, &isRemote);
     if (!t || tm != tmFull) {
	isRemote = true;
        t = RemoteTimers.GetTimer(timer);
        }
     if (!t) {
        isRemote = false;
        t = Timers.GetTimer(timer);
        }
     if (t) {
        delete timer;
        timer = t;
	if (tm != tmFull)
           return AddSubMenu(new cMenuEditRemoteTimer(timer, isRemote));
	else {
	   // matching timer, but it was filtered
	   int user = cMenuTimerItem::ParseUser(t);
	   cMenuTimerItem::UpdateUser(*t, user | MASK_FROM_SETUP(RemoteTimersSetup.defaultUser));
	   if (isRemote) {
	      eRemoteTimersState state = RemoteTimers.Modify((cRemoteTimer*)t);
	      if (state > rtsRefresh)
	         Skins.Message(state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
	      }
	   else {
	      Timers.SetModified();
	      }
	   }
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     else {
        isRemote = false;
        cMenuTimerItem::UpdateUser(*timer, MASK_FROM_SETUP(RemoteTimersSetup.defaultUser));
        Timers.Add(timer);
        if (RemoteTimersSetup.addToRemote && !cSvdrp::GetInstance()->Offline()) {
           isRemote = true;
           cRemoteTimer *rt = new cRemoteTimer();
           *(cTimer*) rt = *timer;
           eRemoteTimersState state = RemoteTimers.New(rt);
           if (state <= rtsRefresh) {
              Timers.Del(timer);
              timer = rt;
              }
           else {
              Skins.Message(state == rtsLocked ? mtWarning : mtError, tr(RemoteTimers.GetErrorMessage(state)));
              Timers.Del(timer);
              delete rt;
              return osContinue;
              }
           }
        Timers.SetModified();
        isyslog("timer %s added (active)", *timer->ToDescr());
        if (timer->Matches(0, false, NEWTIMERLIMIT)) {
           isRemote ? RemoteConflicts.Update() : LocalConflicts.Update();
           return AddSubMenu(new cMenuEditRemoteTimer(timer, isRemote));
        }
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     }
  return osContinue;
}

eOSState cMenuSchedule::Switch(void)
{
  if (otherChannel) {
     if (Channels.SwitchTo(otherChannel))
        return osEnd;
     }
  Skins.Message(mtError, tr("Can't switch channel!"));
  return osContinue;
}

eOSState cMenuSchedule::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case k0:      return Number();
       case kRecord:
       case kRed:    return Record();
       case kGreen:  if (schedules) {
                        if (whatsOnId == -1) {
                           int ChannelNr = 0;
                           if (Count()) {
                              cChannel *channel = Channels.GetByChannelID(((cMenuScheduleItem *)Get(Current()))->event->ChannelID(), true);
                              if (channel)
                                 ChannelNr = channel->Number();
                              }
                           whatsOnId = EPGTIME_LENGTH; // now
                           return AddSubMenu(new cMenuWhatsOn(schedules, whatsOnId, ChannelNr));
                           }
                        //now = !now;
                        //next = !next;
                        whatsOnId = (whatsOnId + 1) % (EPGTIME_LENGTH + 2);
                        while (whatsOnId < EPGTIME_LENGTH && !RemoteTimersSetup.epgTime[whatsOnId])
                           ++whatsOnId;
                        return AddSubMenu(new cMenuWhatsOn(schedules, whatsOnId, cMenuWhatsOn::CurrentChannel()));
                        }
       case kYellow: if (schedules)
                        return AddSubMenu(new cMenuWhatsOn(schedules, EPGTIME_LENGTH + 1, cMenuWhatsOn::CurrentChannel()));
                     break;
       case kBlue:   if (Count() && otherChannel)
                        return Switch();
                     break;
       case kInfo:
       case kOk:     if (Count())
                        return AddSubMenu(new cMenuEvent(((cMenuScheduleItem *)Get(Current()))->event, otherChannel, true));
                     break;
       default:      break;
       }
     }
  else if (!HasSubMenu()) {
     // now = next = false;
     whatsOnId = -1;
     const cEvent *ei = cMenuWhatsOn::ScheduleEvent();
     if (ei) {
        cChannel *channel = Channels.GetByChannelID(ei->ChannelID(), true);
        if (channel) {
           cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
           PrepareScheduleAllThis(NULL, channel);
           if (channel->Number() != cDevice::CurrentChannel()) {
              otherChannel = channel->Number();
              SetHelp(Count() ? tr("Button$Record") : NULL, tr("Button$Now"), tr("Button$Next"), tr("Button$Switch"));
              }
           Display();
           }
        }
     else if (HadSubMenu && Update())
        Display();
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuRecording --------------------------------------------------------

class cMenuRecording : public cOsdMenu {
private:
  const cRecording *recording;
  bool withButtons;
public:
  cMenuRecording(const cRecording *Recording, bool WithButtons = false);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuRecording::cMenuRecording(const cRecording *Recording, bool WithButtons)
:cOsdMenu(tr("Recording info"))
{
  SetMenuCategory(RemoteTimersSetup.skinRecordings ? mcRecordingInfo : mcPlugin);
  recording = Recording;
  withButtons = WithButtons;
  if (withButtons)
     SetHelp(tr("Button$Play"), tr("Button$Rewind"));
  SetTitle(cString::sprintf("%s - %s", tr("Recording info"), *cFreeDiskSpace::FreeDiskSpaceString(Recording)));
}

void cMenuRecording::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetRecording(recording);
  if (recording->Info()->Description())
     cStatus::MsgOsdTextItem(recording->Info()->Description());
}

eOSState cMenuRecording::ProcessKey(eKeys Key)
{
  switch (int(Key)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft);
                  return osContinue;
    case kInfo:   return osBack;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:    if (withButtons)
                        Key = kOk; // will play the recording, even if recording commands are defined
       case kGreen:  if (!withButtons)
                        break;
                     cRemote::Put(Key, true);
                     // continue with osBack to close the info menu and process the key
       case kOk:     return osBack;
       default: break;
       }
     }
  return state;
}

// --- cMenuEditRecording --------------------------------------------------------

class cMenuEditRecording : public cOsdMenu {
private:
  int priority;
  int lifetime;
  char name[MaxFileName];
  int user;
  int tmpUser;
  cString fileName;
  cMenuFolder *subMenuFolder;
  cMenuEditStrItem *file;
  eOSState SetFolder(void);
  eOSState Commands(eKeys Key = kNone);
  eOSState Cut(void);
  eOSState Info(void);
  void SetHelpKeys(void);
  bool Rename(const cRecording *Recording, const char *NewName);
  bool UpdatePrioLife(cRecording *Recording);
  bool UpdateName(cRecording *Recording);
  static bool ModifyInfo(cRecording *Recording, const char *buffer);
public:
  static bool UpdateUser(cRecording *Recording, int NewUser);

  cMenuEditRecording(const cRecording *Recording);
  virtual ~cMenuEditRecording();
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuEditRecording::cMenuEditRecording(const cRecording *Recording)
:cOsdMenu(trREMOTETIMERS("Edit recording"), 12)
{
  // there is no mcRecordingEdit
  SetMenuCategory(mcPlugin);
  // Must be locked against Recordings
  priority = Recording->Priority();
  lifetime = Recording->Lifetime();
  strn0cpy(name, Recording->Name(), sizeof(name));
  tmpUser = user = ParseUser(Recording->Info()->Aux());
  fileName = Recording->FileName();
  subMenuFolder = NULL;
  file = NULL;
  SetHelpKeys();
  SetTitle(cString::sprintf("%s - %s", trREMOTETIMERS("Edit recording"), *cFreeDiskSpace::FreeDiskSpaceString(Recording)));
  Add(new cMenuEditIntItem(tr("Priority"), &priority, 0, MAXPRIORITY));
  Add(new cMenuEditIntItem(tr("Lifetime"), &lifetime, 0, MAXLIFETIME));
  Add(new cMenuEditUserItem(trREMOTETIMERS("User ID"), &tmpUser));
  Add(file = new cMenuEditStrItem(tr("File"), name, sizeof(name), tr(FileNameChars)));
  Display();
}

cMenuEditRecording::~cMenuEditRecording()
{
}

eOSState cMenuEditRecording::Commands(eKeys Key)
{
  if (HasSubMenu())
     return osContinue;
  cMenuCommands *menu;
  eOSState state = AddSubMenu(menu = new cMenuCommands(tr("Recording commands"), &RecordingCommands, cString::sprintf("\"%s\"", *strescape(*fileName, "\\\"$"))));
  if (Key != kNone) {
     state = menu->ProcessKey(Key);
     return state;
     }
  return osContinue;
}

eOSState cMenuEditRecording::Cut()
{
  cThreadLock RecordingsLock(&Recordings);
  cRecording *recording = Recordings.GetByName(*fileName);
  if (recording) {
     cMarks Marks;
     if (Marks.Load(recording->FileName(), recording->FramesPerSecond(), recording->IsPesRecording()) && Marks.Count()) {
        const char *name = recording->Name();
        int len = strlen(RemoteTimersSetup.serverDir);
        bool remote = len == 0 || (strstr(name, RemoteTimersSetup.serverDir) == name && name[len] == FOLDERDELIMCHAR);
        if (!remote) {
#if APIVERSNUM > 20101
           if (RecordingsHandler.GetUsage(*fileName) == ruNone) {
              if (RecordingsHandler.Add(ruCut, *fileName))
#else
           if (!cCutter::Active()) {
              if (cCutter::Start(*fileName))
#endif
                 Skins.Message(mtInfo, tr("Editing process started"));
              else
                 Skins.Message(mtError, tr("Can't start editing process!"));
              }
           else
              Skins.Message(mtError, tr("Editing process already active!"));
           }
        else if (cSvdrp::GetInstance()->Connect()) {
           char *date = strdup(recording->Title(' ', false));
           char *p = strchr(date, ' ');
           if (p && (p = strchr(++p, ' ')) && p) {
              *p = 0;
              eRemoteRecordingsState state = RemoteRecordings.Cut(date, name + (len ? len + 1 : 0));
              if (state == rrsOk)
                 Skins.Message(mtInfo, trREMOTETIMERS("Remote editing process started"));
              else
                 Skins.Message(state == rrsLocked ? mtWarning : mtError, tr(RemoteRecordings.GetErrorMessage(state)));
              }
           else
              esyslog("remotetimers: unexpected title format '%s'", date);
           free(date);
           cSvdrp::GetInstance()->Disconnect();
           }
        else {
           Skins.Message(mtError, tr(MSG_UNAVAILABLE));
           cSvdrp::GetInstance()->Disconnect();
           }
        }
     else
        Skins.Message(mtError, tr("No editing marks defined!"));
     }
  else
     Skins.Message(mtError, tr("Error while accessing recording!"));
  return osContinue;
}

eOSState cMenuEditRecording::Info(void)
{
  if (HasSubMenu())
     return osContinue;
  cThreadLock RecordingsLock(&Recordings);
  cRecording *recording = Recordings.GetByName(*fileName);
  if (recording && recording->Info()->Title())
     return AddSubMenu(new cMenuRecording(recording, true));
  return osContinue;
}

#define INFOFILE_PES "info.vdr"
#define INFOFILE_TS "info"
bool cMenuEditRecording::ModifyInfo(cRecording *Recording, const char *Info)
{
  cString InfoFileName = cString::sprintf(Recording->IsPesRecording() ? "%s/" INFOFILE_PES : "%s/" INFOFILE_TS, Recording->FileName());
  FILE *f = fopen(InfoFileName, "a");
  if (f) {
     if (fprintf(f, "%s\n", Info) > 0) {
        if (fclose(f) == 0) {
           cRecording rec(Recording->FileName());
           // as of VDR 1.7.21 WriteInfo() always returns true
           return rec.WriteInfo();
           }
        f = NULL;
        }
     esyslog("remotetimers: Failed to update '%s': %m", *InfoFileName);
     if (f)
        fclose(f);
     }
  else
     esyslog("remotetimers: writing to '%s' failed: %m", *InfoFileName);
  return false;
}

bool cMenuEditRecording::UpdateUser(cRecording *Recording, int NewUser)
{
  cString buffer(PluginRemoteTimers::UpdateUser(Recording->Info()->Aux(), NewUser));
  buffer = cString::sprintf("@ %s", *buffer);
  if (ModifyInfo(Recording, buffer))
     return true;
  Skins.Message(mtError, trREMOTETIMERS("Unable to update user ID"));
  return false;
}

#define PRIO_LIFE_FORMAT ".%02d.%02d.rec"
bool cMenuEditRecording::UpdatePrioLife(cRecording *Recording)
{
  if (!Recording->IsPesRecording()) {
     cString buffer = cString::sprintf("P %d\nL %d", priority, lifetime);
     if (ModifyInfo(Recording, *buffer))
        return true;
     }
  else
  {
     char *newName = strdup(Recording->FileName());
     size_t len = strlen(newName);
     cString freeStr(newName, true);
     cString oldData = cString::sprintf(PRIO_LIFE_FORMAT, Recording->Priority(), Recording->Lifetime());
     cString newData = cString::sprintf(PRIO_LIFE_FORMAT, priority, lifetime);
     size_t lenReplace = strlen(oldData);
     if (lenReplace < len) {
        if (strlen(newData) == lenReplace) {
           char *p = newName + len - lenReplace;
           if (strcmp(p, oldData) == 0) {
              strncpy(p, newData, lenReplace);
              if (Rename(Recording, newName)) {
                 fileName = newName;
                 return true;
                 }
              }
           else
              esyslog("remotetimers: unexpected filename '%s'", *fileName);
           }
        else
           esyslog("remotetimers: invalid priority/lifetime data for '%s'", *fileName);
        }
     else
        esyslog("remotetimers: short filename '%s'", *fileName);
     }
  Skins.Message(mtError, trREMOTETIMERS("Unable to change priority/lifetime"));
  return false;
}

bool cMenuEditRecording::UpdateName(cRecording *Recording)
{
  int len = strlen(name);
  if (!len) {
     // user cleared name - restore previous value
     strn0cpy(name, Recording->Name(), sizeof(name));
     return false;
  }

  const char *p = strrchr(Recording->FileName(), '/');
  if (p) {
     // cMenuEditStrItem strips trailing whitespace characters
     // If name ends with FOLDERDELIMCHAR, assume there was a space
     if (len < MaxFileName - 1 && name[len - 1] == FOLDERDELIMCHAR) {
         name[len++] = ' ';
         name[len] = '\0';
     }
     cString newName(ExchangeChars(strdup(name), true), true);
#if APIVERSNUM > 20101
     newName = cString::sprintf("%s/%s%s", cVideoDirectory::Name(), *newName, p);
#else
     newName = cString::sprintf("%s/%s%s", VideoDirectory, *newName, p);
#endif
     bool wasMoving = cMoveRec::IsMoving();
     if (Rename(Recording, newName)) {
        // keep old name when moving recording in background
        if (cMoveRec::IsMoving() && !wasMoving)
           return false;
	fileName = newName;
        return true;
        }
     }
  Skins.Message(mtError, trREMOTETIMERS("Unable to rename recording"));
  return false;
}

bool cMenuEditRecording::Rename(const cRecording *Recording, const char *NewName) {
  const char *oldName = Recording->FileName();
  if (access(NewName, F_OK) == 0) {
     esyslog("remotetimers: not renaming '%s' to '%s'. File exists", oldName, NewName);
     return false;
     }
  // false makes sure that the actual target directory itself is not created
  if (!MakeDirs(NewName, false))
     return false;
  if (rename(oldName, NewName) != 0) {
     if (errno == EXDEV) {
        if (Interface->Confirm(trREMOTETIMERS("Move to other filesystem in background?"))) {
           return cMoveRec::GetInstance()->Move(Recording, NewName);
           }
        }
     else
        esyslog("remotetimers: error renaming '%s' to '%s': %m", oldName, NewName);
     return false;
     }
  return true;
}

eOSState cMenuEditRecording::SetFolder(void)
{
  cMenuFolder *mf = subMenuFolder;
  if (mf) {
     cString Folder = mf->GetFolder();
     char *p = strrchr(name, FOLDERDELIMCHAR);
     if (p)
        p++;
     else
        p = name;
     if (!isempty(*Folder))
        strn0cpy(name, cString::sprintf("%s%c%s", *Folder, FOLDERDELIMCHAR, p), sizeof(name));
     else if (p != name)
        memmove(name, p, strlen(p) + 1);
     SetCurrent(file);
     Display();
     subMenuFolder = NULL;
     }
  return CloseSubMenu();
}

void cMenuEditRecording::SetHelpKeys()
{
  SetHelp(trREMOTETIMERS("Button$Folder"), trREMOTETIMERS("Cut"), RecordingCommands.Count() ? tr("Commands") : NULL);
}

eOSState cMenuEditRecording::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:    return AddSubMenu(subMenuFolder = new cMenuFolder(trREMOTETIMERS("Select folder"), &Folders, name));
       case kGreen:  return Cut();
       case kYellow: return Commands();
       case kInfo:   return Info();
       case kOk:     {
                        cThreadLock recordingsLock(&Recordings);
                        cRecording *recording = Recordings.GetByName(*fileName);
                        if (recording) {
                           bool replace = false;
                           if (user != tmpUser)
                              replace |= UpdateUser(recording, tmpUser);
                           if (priority != recording->Priority() || lifetime != recording->Lifetime())
                              replace |= UpdatePrioLife(recording);
                           if (replace) {
                              Recordings.Del(recording);
                              Recordings.AddByName(*fileName);
                              recording = Recordings.GetByName(*fileName);
                              }
                           }
                        if (recording) {
                           if (strcmp(name, recording->Name()) != 0)
                              if (UpdateName(recording)) {
                                 Recordings.Del(recording);
                                 Recordings.AddByName(*fileName);
                              }
                           }
                        else
                           Skins.Message(mtError, tr("Error while accessing recording!"));
		        return osBack;
                     }
       default: break;
       }
     }
  else if (state == osEnd && HasSubMenu())
     state = SetFolder();
  else if (Key == kOk || Key == kBack)
     SetHelpKeys();
  return state;
}

// --- cRemoteTimersReplayControl -------------------------------------------------------

class cRemoteTimersReplayControl: public cReplayControl
{
private:
  // was PluginRemoteTimers::cMenuMain open when replay was started?
  bool openMainMenu;
public:
  virtual eOSState ProcessKey(eKeys Key);
  cRemoteTimersReplayControl(bool OpenMainMenu);
};

cRemoteTimersReplayControl::cRemoteTimersReplayControl(bool OpenMainMenu)
{
  openMainMenu = OpenMainMenu;
}

eOSState cRemoteTimersReplayControl::ProcessKey(eKeys Key)
{
  eOSState state = cReplayControl::ProcessKey(Key);
  if (state == osRecordings) {
     cMenuRecordings::SetReplayEnded();
     if (openMainMenu) {
        if (!cRemote::CallPlugin(PLUGIN_NAME_I18N))
           esyslog("remotetimers: Unable to return to recordings menu");
        state = osEnd;
     }
  }
  return state;
}

// --- cMenuRecordingItem ----------------------------------------------------

class cMenuRecordingItem : public cOsdItem {
private:
  cRecording *recording;
  int level;
  char *name;
  int user;
  int totalEntries, newEntries;
public:
  cMenuRecordingItem(cRecording *Recording, int Level, int User);
  ~cMenuRecordingItem();
  void IncrementCounter(bool New);
  const char *Name(void) { return name; }
  cRecording *Recording(void) { return recording; }
  int User(void) { return user; } const
  bool IsDirectory(void) { return name != NULL; }
  virtual void SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable);
  };

cMenuRecordingItem::cMenuRecordingItem(cRecording *Recording, int Level, int User)
{
  recording = Recording;
  level = Level;
  name = NULL;
  user = User;
  totalEntries = newEntries = 0;
  SetText(Recording->Title('\t', true, Level));
  if (*Text() == '\t')
     name = strdup(Text() + 2); // 'Text() + 2' to skip the two '\t'
}

cMenuRecordingItem::~cMenuRecordingItem()
{
  free(name);
}

void cMenuRecordingItem::IncrementCounter(bool New)
{
  totalEntries++;
  if (New)
     newEntries++;
  SetText(cString::sprintf("%d\t\t%d\t%s", totalEntries, newEntries, name));
}

void cMenuRecordingItem::SetMenuItem(cSkinDisplayMenu *DisplayMenu, int Index, bool Current, bool Selectable)
{
  if (!RemoteTimersSetup.skinRecordings || !DisplayMenu->SetItemRecording(recording, Index, Current, Selectable, level, totalEntries, newEntries))
     DisplayMenu->SetItem(Text(), Index, Current, Selectable);
}

// --- cMenuRecordings -------------------------------------------------------

int cMenuRecordings::userFilter = 0;
bool cMenuRecordings::replayEnded = false;

cMenuRecordings::cMenuRecordings(const char *Base, int Level, bool OpenSubMenus)
:cOsdMenu(Base ? Base : tr("Recordings"), 9, 6, 6)
{
  SetMenuCategory(RemoteTimersSetup.skinRecordings ? mcRecording : mcPlugin);
  base = Base ? strdup(Base) : NULL;
  level = ::Setup.RecordingDirs ? Level : -1;
  if (level <= 0 && !replayEnded)
     userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterRecordings);
  Recordings.StateChanged(recordingsState); // just to get the current state
  helpKeys = -1;
  Display(); // this keeps the higher level menus from showing up briefly when pressing 'Back' during replay
  Set();
  SetFreeDiskDisplay(true);
  OpenSubMenus |= replayEnded;
  replayEnded = false;
  if (Current() < 0)
     SetCurrent(First());
  else if (OpenSubMenus && cReplayControl::LastReplayed() && Open(true))
     return;
  Display();
  SetHelpKeys();
}

cMenuRecordings::~cMenuRecordings()
{
  helpKeys = -1;
  free(base);
}

bool cMenuRecordings::SetFreeDiskDisplay(bool Force)
{
  if (FreeDiskSpace.HasChanged(base, Force)) {
     //XXX -> skin function!!!
     if (userFilter == 0)
        SetTitle(cString::sprintf("%s - %s", base ? base : tr("Recordings"), FreeDiskSpace.FreeDiskSpaceString()));
     else
        SetTitle(cString::sprintf("%s - %s - %s %d", base ? base : tr("Recordings"), FreeDiskSpace.FreeDiskSpaceString(), trREMOTETIMERS("User"), userFilter));
     return true;
     }
  return false;
}

#define B_RELEASE 0x100
void cMenuRecordings::SetHelpKeys(void)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  int NewHelpKeys = 0;
  if (ri) {
     if (ri->IsDirectory())
        NewHelpKeys = 1;
     else {
        NewHelpKeys = 2;
        if (ri->Recording()->Info()->Title())
           NewHelpKeys = 3;
        if (userFilter != 0 && ri->User() != 0 && ri->User() != USER_MASK(userFilter))
	   NewHelpKeys |= B_RELEASE;
        }
     }
  if (NewHelpKeys != helpKeys) {
     switch (NewHelpKeys) {
       case 0: SetHelp(NULL); break;
       case 1: SetHelp(tr("Button$Open")); break;
       //case 2:
       //case 3: SetHelp(RecordingCommands.Count() ? tr("Commands") : tr("Button$Play"), tr("Button$Rewind"), tr("Button$Delete"), NewHelpKeys == 3 ? tr("Button$Info") : NULL);
       // TRANSLATORS: Button displayed instead of "Delete" when there are other users who didn't watch the recording yet
       default: SetHelp(tr("Button$Edit"), tr("Button$Rewind"), (NewHelpKeys & B_RELEASE) ? trREMOTETIMERS("Release") : tr("Button$Delete"), (NewHelpKeys & ~B_RELEASE) == 3 ? tr("Button$Info") : NULL);
       }
     helpKeys = NewHelpKeys;
     }
}

void cMenuRecordings::Set(bool Refresh)
{
  const char *CurrentRecording = cReplayControl::LastReplayed();
  cMenuRecordingItem *LastItem = NULL;
  cThreadLock RecordingsLock(&Recordings);
  if (Refresh) {
     if (cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current()))
        CurrentRecording = ri->Recording()->FileName();
     }
  Clear();
  GetRecordingsSortMode(DirectoryName());
  Recordings.Sort();
  for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
      int user = ParseUser(recording->Info()->Aux());
      if (userFilter != 0 && user != 0 && (user & USER_MASK(userFilter)) == 0)
         continue;
      if (!base || (strstr(recording->Name(), base) == recording->Name() && recording->Name()[strlen(base)] == FOLDERDELIMCHAR)) {
         cMenuRecordingItem *Item = new cMenuRecordingItem(recording, level, user);
         cMenuRecordingItem *LastDir = NULL;
         if (Item->IsDirectory()) {
            // Sorting may ignore non-alphanumeric characters, so we need to explicitly handle directories in case they only differ in such characters:
            for (cMenuRecordingItem *p = LastItem; p; p = dynamic_cast<cMenuRecordingItem *>(p->Prev())) {
                if (p->Name() && strcmp(p->Name(), Item->Name()) == 0) {
                   LastDir = p;
                   break;
                   }
                }
            }
         if (*Item->Text() && !LastDir) {
            Add(Item);
            LastItem = Item;
            if (Item->IsDirectory())
               LastDir = Item;
            }
         else
            delete Item;
         if (LastItem || LastDir) {
            if (CurrentRecording && strcmp(CurrentRecording, recording->FileName()) == 0)
               SetCurrent(LastDir ? LastDir : LastItem);
            }
         if (LastDir)
            LastDir->IncrementCounter(recording->IsNew());
         }
      }
  Refresh |= SetFreeDiskDisplay(Refresh);
  if (Refresh)
     Display();
}

cString cMenuRecordings::DirectoryName(void)
{
#if APIVERSNUM > 20101
  cString d(cVideoDirectory::Name());
#else
  cString d(VideoDirectory);
#endif
  if (base) {
     char *s = ExchangeChars(strdup(base), true);
     d = AddDirectory(d, s);
     free(s);
     }
  return d;
}

bool cMenuRecordings::Open(bool OpenSubMenus)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && ri->IsDirectory()) {
     const char *t = ri->Name();
     cString buffer;
     if (base) {
        buffer = cString::sprintf("%s~%s", base, t);
        t = buffer;
        }
     AddSubMenu(new cMenuRecordings(t, level + 1, OpenSubMenus));
     return true;
     }
  return false;
}

eOSState cMenuRecordings::Play(void)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri) {
     if (ri->IsDirectory())
        Open();
     else {
        cReplayControl::SetRecording(ri->Recording()->FileName());
	// use our own replay control which returns to this recordings menu
	cControl::Shutdown();
	cControl::Launch(new cRemoteTimersReplayControl(cMenuMain::IsOpen()));
	return osEnd;
        // return osReplay;
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Rewind(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     cDevice::PrimaryDevice()->StopReplay(); // must do this first to be able to rewind the currently replayed recording
     cResumeFile ResumeFile(ri->Recording()->FileName(), ri->Recording()->IsPesRecording());
     ResumeFile.Delete();
     return Play();
     }
  return osContinue;
}

eOSState cMenuRecordings::Delete(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     if (userFilter != 0 && ri->User() != 0 && ri->User() != USER_MASK(userFilter)) {
        cRecording *recording = ri->Recording();
        cString FileName = recording->FileName();
	if (cMenuEditRecording::UpdateUser(recording, ri->User() ^ USER_MASK(userFilter))) {
           cReplayControl::ClearLastReplayed(FileName);
           Recordings.Del(recording);
           Recordings.AddByName(FileName);
           cOsdMenu::Del(Current());
           SetHelpKeys();
           Display();
           if (!Count())
              return osBack;
	   }
	}
     else if (Interface->Confirm(tr("Delete recording?"))) {
        cRecordControl *rc = cRecordControls::GetRecordControl(ri->Recording()->FileName());
        if (rc) {
           if (Interface->Confirm(tr("Timer still recording - really delete?"))) {
              cTimer *timer = rc->Timer();
              if (timer) {
                 timer->Skip();
                 cRecordControls::Process(time(NULL));
                 if (timer->IsSingleEvent()) {
                    isyslog("deleting timer %s", *timer->ToDescr());
                    Timers.Del(timer);
                    }
                 Timers.SetModified();
                 }
              }
           else
              return osContinue;
           }
        cRecording *recording = ri->Recording();
        cString FileName = recording->FileName();
#if APIVERSNUM > 20101
        if (RecordingsHandler.GetUsage(*FileName) != ruNone) {
#else
        if (cCutter::Active(ri->Recording()->FileName())) {
#endif
           if (Interface->Confirm(tr("Recording is being edited - really delete?"))) {
#if APIVERSNUM > 20101
              RecordingsHandler.Del(*FileName);
#else
              cCutter::Stop();
#endif
              recording = Recordings.GetByName(FileName); // cCutter::Stop() might have deleted it if it was the edited version
              // we continue with the code below even if recording is NULL,
              // in order to have the menu updated etc.
              }
           else
              return osContinue;
           }
        if (cReplayControl::NowReplaying() && strcmp(cReplayControl::NowReplaying(), FileName) == 0)
           cControl::Shutdown();
        if (!recording || recording->Delete()) {
           cReplayControl::ClearLastReplayed(FileName);
           Recordings.DelByName(FileName);
           cOsdMenu::Del(Current());
           SetHelpKeys();
           SetFreeDiskDisplay(true);
           cVideoDiskUsage::ForceCheck();
           Display();
           if (!Count())
              return osBack;
           }
        else
           Skins.Message(mtError, tr("Error while deleting recording!"));
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Info(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory() && ri->Recording()->Info()->Title())
     return AddSubMenu(new cMenuRecording(ri->Recording(), true));
  return osContinue;
}

eOSState cMenuRecordings::Edit(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     cThreadLock RecordingsLock(&Recordings);
     return AddSubMenu(new cMenuEditRecording(ri->Recording()));
     }
  return osContinue;
}

/*
eOSState cMenuRecordings::Commands(eKeys Key)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     cMenuCommands *menu;
     eOSState state = AddSubMenu(menu = new cMenuCommands(tr("Recording commands"), &RecordingCommands, cString::sprintf("\"%s\"", *strescape(ri->Recording()->FileName(), "\\\"$"))));
     if (Key != kNone)
        state = menu->ProcessKey(Key);
     return state;
     }
  return osContinue;
}
*/

eOSState cMenuRecordings::Sort(void)
{
  if (HasSubMenu())
     return osContinue;
  IncRecordingsSortMode(DirectoryName());
  Set(true);
  return osContinue;
}

eOSState cMenuRecordings::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kPlayPause:
       case kPlay:
       case kOk:     return Play();
       //case kRed:    return (helpKeys > 1 && RecordingCommands.Count()) ? Commands() : Play();
       case kRed:    return (helpKeys > 1) ? Edit() : Play();
       case kGreen:  return Rewind();
       case kYellow: return Delete();
       case kInfo:
       case kBlue:   return Info();
       //case k0:      return Sort();
       //case k1...k9: return Commands(Key);
       case k0 ... k9:
                     if (userFilter == Key - k0)
                        return Sort();
                     userFilter = Key - k0;
		     if (RemoteTimersSetup.userFilterRecordings < 0 && ::Setup.ResumeID != Key - k0) {
		        ::Setup.ResumeID = Key - k0;
			Recordings.ResetResume();
			}
                     Set(true);
                     SetHelpKeys();
                     return osContinue;
       case kNone:   if (Recordings.StateChanged(recordingsState))
                        Set(true);
                     break;
       default: break;
       }
     }
  if (Key == kYellow && HadSubMenu && !HasSubMenu()) {
     // the last recording in a subdirectory was deleted, so let's go back up
     cOsdMenu::Del(Current());
     if (!Count())
        return osBack;
     Display();
     }
  if (!HasSubMenu()) {
     if (HadSubMenu)
	Set(true);
     if (!Count())
        return osBack;
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuMain -------------------------------------------------------------

int cMenuMain::count = 0;

cMenuMain::cMenuMain(const char *Title, eOSState State)
:cOsdMenu(Title)
{
  SetMenuCategory(mcMain);
  count++;
  SetHasHotkeys();
  Add(new cOsdItem(hk(tr("Schedule")),   osUser1));
  Add(new cOsdItem(hk(tr("Timers")),     osUser2));
  Add(new cOsdItem(hk(tr("Recordings")), osUser3));
  if (cMoveRec::IsMoving())
     Add(new cOsdItem(hk(trREMOTETIMERS("Abort moving recording")), osUser4));
  if (State == osRecordings)
     AddSubMenu(new cMenuRecordings(NULL, 0, true));
}

cMenuMain::~cMenuMain()
{
  count--;
}

eOSState cMenuMain::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  switch (state) {
    case osUser1:      return AddSubMenu(new cMenuSchedule);
    case osUser2:      return AddSubMenu(new cMenuTimers);
    case osUser3:      return AddSubMenu(new cMenuRecordings);
    case osUser4:      cMoveRec::Abort(-1); return osBack;
    default:           return state;
    }
}

}; // namespace PluginRemoteTimers
