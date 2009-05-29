/*
 * menu.c: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menu.c 1.477 2008/02/16 13:53:26 kls Exp $
 */

#include "menu.h"
#include "setup.h"
#include "i18n.h"
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
//#include "recording.h"
//#include "remote.h"
//#include "shutdown.h"
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
#define DISKSPACECHEK     5 // seconds between disk space checks
#define NEWTIMERLIMIT   120 // seconds until the start time of a new timer created from the Schedule menu,
                            // within which it will go directly into the "Edit timer" menu to allow
                            // further parameter settings

//#define MAXRECORDCONTROLS (MAXDEVICES * MAXRECEIVERS)
//#define MAXINSTANTRECTIME (24 * 60 - 1) // 23:59 hours
//#define MAXWAITFORCAMMENU  10 // seconds to wait for the CAM menu to open
//#define CAMMENURETYTIMEOUT  3 // seconds after which opening the CAM menu is retried
//#define CAMRESPONSETIMEOUT  5 // seconds to wait for a response from a CAM
//#define MINFREEDISK       300 // minimum free disk space (in MB) required to start recording
//#define NODISKSPACEDELTA  300 // seconds between "Not enough disk space to start recording!" messages

#define CHNUMWIDTH  (numdigits(Channels.MaxNumber()) + 1)

#define MSG_UNAVAILABLE trNOOP("Remote timers not available")

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

cTimer* GetBestMatch(const cEvent *Event, int UserMask, int *Match, int *Type, bool *Remote)
{
	cTimer *localTimer = NULL, *remoteTimer = NULL;
	int localMatch = tmNone, remoteMatch = tmNone;
	int localUser = 0, remoteUser = 0;

	for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
		int tm = t->Matches(Event);
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
		int tm = t->Matches(Event);
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

#define MB_PER_MINUTE 25.75 // this is just an estimate!

class cFreeDiskSpace {
private:
  static time_t lastDiskSpaceCheck;
  static int lastFreeMB;
  static cString freeDiskSpaceString;
public:
  static bool HasChanged(bool ForceCheck = false);
  static const char *FreeDiskSpaceString(void) { HasChanged(); return freeDiskSpaceString; }
  };

time_t cFreeDiskSpace::lastDiskSpaceCheck = 0;
int cFreeDiskSpace::lastFreeMB = 0;
cString cFreeDiskSpace::freeDiskSpaceString;

cFreeDiskSpace FreeDiskSpace;

bool cFreeDiskSpace::HasChanged(bool ForceCheck)
{
  if (ForceCheck || time(NULL) - lastDiskSpaceCheck > DISKSPACECHEK) {
     int FreeMB;
     int Percent = VideoDiskSpace(&FreeMB);
     lastDiskSpaceCheck = time(NULL);
     if (ForceCheck || FreeMB != lastFreeMB) {
        int Minutes = int(double(FreeMB) / MB_PER_MINUTE);
        int Hours = Minutes / 60;
        Minutes %= 60;
        freeDiskSpaceString = cString::sprintf("%s %d%% - %2d:%02d %s", tr("Disk"), Percent, Hours, Minutes, tr("free"));
        lastFreeMB = FreeMB;
        return true;
        }
     }
  return false;
}

// --- cMenuEditRemoteTimer --------------------------------------------------------
class cMenuEditRemoteTimer : public cMenuEditTimer {
private:
  cTimer *timer;
  cString *timerString;
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
  cMenuEditRemoteTimer(cTimer *Timer, cString *TimerString = NULL, bool New = false, bool Remote = false);
  };

cMenuEditRemoteTimer::cMenuEditRemoteTimer(cTimer *Timer, cString *TimerString, bool New, bool Remote)
:cMenuEditTimer(Timer, New), timer(Timer), timerString(TimerString)
{
  remote = tmpRemote = Remote;
  user = tmpUser = New ? MASK_FROM_SETUP(RemoteTimersSetup.defaultUser) : cMenuTimerItem::ParseUser(timer);
  cOsdItem *item = new cMenuEditBoolItem(trREMOTETIMERS("Location"), &tmpRemote, trREMOTETIMERS("local"), trREMOTETIMERS("remote"));
  if (cSvdrp::GetInstance()->Offline())
      item->SetSelectable(false);
  Add(item, false, First());
  Add(new cMenuEditUserItem(trREMOTETIMERS("User ID"), &tmpUser), false, Get(8));
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
           if (tmpRemote)
              state = CheckState(RemoteTimers.Modify((cRemoteTimer*) timer));
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
                 }
              else
                 delete rt;
              }
           }
        else {
           // move remote to local
           cTimer *lt = new cTimer();
#if VDRVERSNUM < 10403
           if (lt->Parse(timer->ToText())) {
#else
           *lt = *(cTimer*) timer;
#endif
           if ((state = CheckState(RemoteTimers.Delete((cRemoteTimer*) timer))) == osBack) {
              Timers.Add(lt);
              timer = lt;
              }
           else
              delete lt;
#if VDRVERSNUM < 10403
           }
           else
              delete lt;
#endif
           }
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
              }
           else
              delete rt;
           }
        }
     }
  // store timer string so we can highlight the correct item after a refresh
  if (state == osBack && timerString)
     *timerString = timer->ToText();
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
  };
*/

cMenuTimerItem::cMenuTimerItem(cTimer *Timer, int User, bool Remote)
{
  timer = Timer;
  user = User;
  remote = Remote;
  Set();
}

int cMenuTimerItem::Compare(const cListObject &ListObject) const
{
  return timer->Compare(*((cMenuTimerItem *)&ListObject)->timer);
}

void cMenuTimerItem::Set(void)
{
  cString day, name("");
  if (timer->WeekDays())
#if VDRVERSNUM < 10503
     day = timer->PrintDay(0, timer->WeekDays());
#else
     day = timer->PrintDay(0, timer->WeekDays(), false);
#endif
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
  // TRANSLATORS: Indicator for (R)emote or (L)ocal timer in timers list
  const char *RL = trREMOTETIMERS("RL");
  SetText(cString::sprintf("%c\t%c%d\t%s%s%s\t%02d:%02d\t%02d:%02d\t%s",
                    !(timer->HasFlags(tfActive)) ? ' ' : timer->FirstDay() ? '!' : timer->Recording() ? '#' : '>',
                    RL[remote ? 0 : 1],
                    timer->Channel()->Number(),
                    *name,
                    *name && **name ? " " : "",
                    *day,
                    timer->Start() / 100,
                    timer->Start() % 100,
                    timer->Stop() / 100,
                    timer->Stop() % 100,
                    timer->File()));
}

int cMenuTimerItem::ParseUser(const cTimer *Timer) {
  return PluginRemoteTimers::ParseUser(Timer->Aux());
}

void cMenuTimerItem::UpdateUser(cTimer& Timer, int User) {
  cString origTimer = Timer.ToText();
  cString modTimer(PluginRemoteTimers::UpdateUser(*origTimer, User));
  Timer.Parse(*modTimer);
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

cMenuTimers::cMenuTimers(void)
:cOsdMenu(tr("Timers"), 2, CHNUMWIDTH + 1, 10, 6, 6)
{
  helpKeys = -1;
  userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterTimers);
  if (!cSvdrp::GetInstance()->Connect())
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
  int currentIndex = Current();
  Clear();
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
          Add(new cMenuTimerItem(timer, user));
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
     currentTimerString = NULL;
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
     if (!item->Remote())
        Timers.SetModified();
     }
  return osContinue;
}

eOSState cMenuTimers::Edit(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  isyslog("editing timer %s", *CurrentTimer()->ToDescr());
  return AddSubMenu(new cMenuEditRemoteTimer(CurrentTimer(), &currentTimerString, false, CurrentItem()->Remote()));
}

eOSState cMenuTimers::New(void)
{
  if (HasSubMenu())
     return osContinue;
  return AddSubMenu(new cMenuEditRemoteTimer(new cTimer, &currentTimerString, true, cSvdrp::GetInstance()->Offline() ? false : RemoteTimersSetup.addToRemote));
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
        if (item->Remote())
           CheckState(RemoteTimers.Delete((cRemoteTimer*)ti), false);
        else {
           Timers.Del(ti);
           cOsdMenu::Del(Current());
           Timers.SetModified();
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
  int TimerNumber = HasSubMenu() ? Count() : -1;
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
  if (TimerNumber >= 0 && !HasSubMenu()) {
  //if (TimerNumber >= 0 && !HasSubMenu() && Timers.Get(TimerNumber)) {
     // a newly created timer was confirmed with Ok
     //Add(new cMenuTimerItem(Timers.Get(TimerNumber)), true);
     Set();
     Display();
     }
  if (Key != kNone)
     SetHelpKeys();
  return state;
}

// --- cMenuEvent ------------------------------------------------------------

cMenuEvent::cMenuEvent(const cEvent *Event, bool CanSwitch, bool Buttons)
:cOsdMenu(tr("Event"))
{
  event = Event;
  if (event) {
     cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true);
     if (channel) {
        SetTitle(channel->Name());
        int TimerMatch = tmNone;
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
  if (event->Description())
     cStatus::MsgOsdTextItem(event->Description());
}

eOSState cMenuEvent::ProcessKey(eKeys Key)
{
  switch (Key) {
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
  int timerMatch;
  int timerType;
  cMenuScheduleItem(const cEvent *Event, cChannel *Channel = NULL, bool WithDate = false, bool WithBar = false);
  static void SetSortMode(eScheduleSortMode SortMode) { sortMode = SortMode; }
  static void IncSortMode(void) { sortMode = eScheduleSortMode((sortMode == ssmAllAll) ? ssmAllThis : sortMode + 1); }
  static eScheduleSortMode SortMode(void) { return sortMode; }
  virtual int Compare(const cListObject &ListObject) const;
  bool Update(bool Force = false);
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
  int oldTimerMatch = timerMatch;
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
#if VDRVERSNUM < 10504
#define Utf8SymChars(a,b) b
#endif
     if (channel && withDate)
        buffer = cString::sprintf("%d\t%.*s\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, 6), csn, Utf8SymChars(csn, 6), *eds, *event->GetTimeString(), t, v, r, event->Title());
     else if (channel) {
	if (withBar && RemoteTimersSetup.showProgressBar) {
	   int progress = (time(NULL) - event->StartTime()) * MAXPROGRESS / event->Duration();
	   progress = progress < 0 ? 0 : progress >= MAXPROGRESS ? MAXPROGRESS - 1 : progress;
           buffer = cString::sprintf("%d\t%.*s\t%s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, 6), csn, *event->GetTimeString(), ProgressBar[progress], t, v, r, event->Title());
	   }
	else
           buffer = cString::sprintf("%d\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), Utf8SymChars(csn, 6), csn, *event->GetTimeString(), t, v, r, event->Title());
	}
     else
        buffer = cString::sprintf("%.*s\t%s\t%c%c%c\t%s", Utf8SymChars(eds, 6), *eds, *event->GetTimeString(), t, v, r, event->Title());
     SetText(buffer);
     result = true;
     }
  return result;
}

// --- cMenuWhatsOn ----------------------------------------------------------

class cMenuWhatsOn : public cOsdMenu {
private:
  bool now;
  int helpKeys;
  int timerState;
  eOSState Record(void);
  eOSState Switch(void);
  static int currentChannel;
  static const cEvent *scheduleEvent;
  bool Update(void);
  void SetHelpKeys(void);
public:
  cMenuWhatsOn(const cSchedules *Schedules, bool Now, int CurrentChannelNr);
  static int CurrentChannel(void) { return currentChannel; }
  static void SetCurrentChannel(int ChannelNr) { currentChannel = ChannelNr; }
  static const cEvent *ScheduleEvent(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

int cMenuWhatsOn::currentChannel = 0;
const cEvent *cMenuWhatsOn::scheduleEvent = NULL;

cMenuWhatsOn::cMenuWhatsOn(const cSchedules *Schedules, bool Now, int CurrentChannelNr)
:cOsdMenu(Now ? tr("What's on now?") : tr("What's on next?"), CHNUMWIDTH, 7, 6, 4, 4)
{
  now = Now;
  helpKeys = -1;
  timerState = 0;
  Timers.Modified(timerState);
  for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
      if (!Channel->GroupSep()) {
         const cSchedule *Schedule = Schedules->GetSchedule(Channel);
         if (Schedule) {
            const cEvent *Event = Now ? Schedule->GetPresentEvent() : Schedule->GetFollowingEvent();
            if (Event)
               Add(new cMenuScheduleItem(Event, Channel, false, Now), Channel->Number() == CurrentChannelNr);
            }
         }
      }
  currentChannel = CurrentChannelNr;
  Display();
  SetHelpKeys();
  int userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterSchedule);
  if (userFilter != 0)
     SetTitle(cString::sprintf("%s %s %d", Now ? tr("What's on now?") : tr("What's on next?"), trREMOTETIMERS("User"), userFilter));
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
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], now ? tr("Button$Next") : tr("Button$Now"), tr("Button$Schedule"), RemoteTimersSetup.swapOkBlue ? tr("Button$Info") : tr("Button$Switch"));
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
     int tm = tmNone;
     bool isRemote = false;
     if (item->timerMatch == tmFull) {
        //cTimer *timer = Timers.GetMatch(item->event, &tm);
        cTimer *timer = GetBestMatch(item->event, MASK_FROM_SETUP(RemoteTimersSetup.userFilterSchedule), &tm, NULL, &isRemote);
        if (timer)
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
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
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
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
        if (timer->Matches(0, false, NEWTIMERLIMIT))
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
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
       case kOk:     if (Key == kOk && RemoteTimersSetup.swapOkBlue || Key == kBlue && !RemoteTimersSetup.swapOkBlue)
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

cMenuSchedule::cMenuSchedule(void)
:cOsdMenu("")
{
  now = next = false;
  otherChannel = 0;
  helpKeys = -1;
  timerState = 0;
  userFilter = USER_FROM_SETUP(RemoteTimersSetup.userFilterSchedule);
  Timers.Modified(timerState);
  cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
  cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (channel) {
     cMenuWhatsOn::SetCurrentChannel(channel->Number());
     schedules = cSchedules::Schedules(schedulesLock);
     PrepareScheduleAllThis(NULL, channel);
     SetHelpKeys();
     }
  if (!cSvdrp::GetInstance()->Connect() || RemoteTimers.Refresh() != rtsOk)
     Skins.Message(mtWarning, tr(MSG_UNAVAILABLE));
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
  SetCols(CHNUMWIDTH, 7, 7, 6, 4);
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
  SetCols(CHNUMWIDTH, 7, 7, 6, 4);
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
     int tm = tmNone;
     bool isRemote = false;
     if (item->timerMatch == tmFull) {
        //cTimer *timer = Timers.GetMatch(item->event, &tm);
        cTimer *timer = GetBestMatch(item->event, MASK_FROM_SETUP(RemoteTimersSetup.userFilterSchedule), &tm, NULL, &isRemote);
        if (timer)
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
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
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
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
        if (timer->Matches(0, false, NEWTIMERLIMIT))
           return AddSubMenu(new cMenuEditRemoteTimer(timer, NULL, false, isRemote));
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
                        if (!now && !next) {
                           int ChannelNr = 0;
                           if (Count()) {
                              cChannel *channel = Channels.GetByChannelID(((cMenuScheduleItem *)Get(Current()))->event->ChannelID(), true);
                              if (channel)
                                 ChannelNr = channel->Number();
                              }
                           now = true;
                           return AddSubMenu(new cMenuWhatsOn(schedules, true, ChannelNr));
                           }
                        now = !now;
                        next = !next;
                        return AddSubMenu(new cMenuWhatsOn(schedules, now, cMenuWhatsOn::CurrentChannel()));
                        }
       case kYellow: if (schedules)
                        return AddSubMenu(new cMenuWhatsOn(schedules, false, cMenuWhatsOn::CurrentChannel()));
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
     now = next = false;
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

// --- cMenuCommands ---------------------------------------------------------

class cMenuCommands : public cOsdMenu {
private:
  cCommands *commands;
  char *parameters;
  eOSState Execute(void);
public:
  cMenuCommands(const char *Title, cCommands *Commands, const char *Parameters = NULL);
  virtual ~cMenuCommands();
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuCommands::cMenuCommands(const char *Title, cCommands *Commands, const char *Parameters)
:cOsdMenu(Title)
{
  SetHasHotkeys();
  commands = Commands;
  parameters = Parameters ? strdup(Parameters) : NULL;
  for (cCommand *command = commands->First(); command; command = commands->Next(command))
      Add(new cOsdItem(hk(command->Title())));
}

cMenuCommands::~cMenuCommands()
{
  free(parameters);
}

eOSState cMenuCommands::Execute(void)
{
  cCommand *command = commands->Get(Current());
  if (command) {
     bool confirmed = true;
     if (command->Confirm())
        confirmed = Interface->Confirm(cString::sprintf("%s?", command->Title()));
     if (confirmed) {
        Skins.Message(mtStatus, cString::sprintf("%s...", command->Title()));
        const char *Result = command->Execute(parameters);
        Skins.Message(mtStatus, NULL);
        if (Result)
           return AddSubMenu(new cMenuText(command->Title(), Result, fontFix));
        return osEnd;
        }
     }
  return osContinue;
}

eOSState cMenuCommands::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;
       case kOk:     return Execute();
       default:      break;
       }
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
  recording = Recording;
  withButtons = WithButtons;
  if (withButtons)
     SetHelp(tr("Button$Play"), tr("Button$Rewind"));
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
  switch (Key) {
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
  char *fileName;
  eOSState Commands(eKeys Key = kNone);
  eOSState Cut(void);
  eOSState Info(void);
  void SetHelpKeys(void);
  bool Rename(const char *Old, const char *New);
  bool UpdateData(cRecording *Recording);
  bool UpdateName(cRecording *Recording);
public:
  static bool UpdateUser(cRecording *Recording, int NewUser);

  cMenuEditRecording(const cRecording *Recording);
  virtual ~cMenuEditRecording();
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuEditRecording::cMenuEditRecording(const cRecording *Recording)
:cOsdMenu(trREMOTETIMERS("Edit recording"), 12)
{
  // Must be locked agains Recordings
  priority = Recording->priority;
  lifetime = Recording->lifetime;
  strn0cpy(name, Recording->Name(), sizeof(name));
  tmpUser = user = ParseUser(Recording->Info()->Aux());
  fileName = strdup(Recording->FileName());
  SetHelpKeys();
  Add(new cMenuEditIntItem(tr("Priority"), &priority, 0, MAXPRIORITY));
  Add(new cMenuEditIntItem(tr("Lifetime"), &lifetime, 0, MAXLIFETIME));
  Add(new cMenuEditUserItem(trREMOTETIMERS("User ID"), &tmpUser));
  Add(new cMenuEditPathItem(tr("File"), name, sizeof(name), tr(FileNameChars)));
  Display();
}

cMenuEditRecording::~cMenuEditRecording()
{
  free(fileName);
}

eOSState cMenuEditRecording::Commands(eKeys Key)
{
  if (HasSubMenu())
     return osContinue;
  cMenuCommands *menu;
  eOSState state = AddSubMenu(menu = new cMenuCommands(tr("Recording commands"), &RecordingCommands, cString::sprintf("\"%s\"", *strescape(fileName, "\\\"$"))));
  if (Key != kNone) {
     state = menu->ProcessKey(Key);
     return state;
     }
  return osContinue;
}

eOSState cMenuEditRecording::Cut()
{
  cThreadLock RecordingsLock(&Recordings);
  cRecording *recording = Recordings.GetByName(fileName);
  if (recording) {
     cMarks Marks;
     if (Marks.Load(recording->FileName()) && Marks.Count()) {
        const char *name = recording->Name();
	int len = strlen(RemoteTimersSetup.serverDir);
        bool remote = len == 0 || (strstr(name, RemoteTimersSetup.serverDir) == name && name[len] == '~');
        if (!remote) {
           if (!cCutter::Active()) {
              if (cCutter::Start(fileName))
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
              cSvdrp::GetInstance()->Disconnect();
	      }
           else
              esyslog("remotetimers: unexpected title format '%s'", date);
           free(date);
           }
        else
           Skins.Message(mtError, tr(MSG_UNAVAILABLE));
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
  cRecording *recording = Recordings.GetByName(fileName);
  if (recording && recording->Info()->Title())
     return AddSubMenu(new cMenuRecording(recording, true));
  return osContinue;
}

bool cMenuEditRecording::UpdateUser(cRecording *Recording, int NewUser)
{
  bool tainted = false;

  // check for write access as cRecording::WriteInfo() always returns true
  // TODO: writing may still fail as access() doesn't use the effective UID
  cString InfoFileName = cString::sprintf("%s/info.vdr", Recording->FileName());
  if (access(InfoFileName, W_OK) == 0) {
     cString buffer(PluginRemoteTimers::UpdateUser(Recording->Info()->Aux(), NewUser));
     buffer = cString::sprintf("@ %s", *buffer);
     FILE *f = fmemopen((void *) *buffer, strlen(*buffer) * sizeof(char), "r");
     if (f) {
        // Casting const away is nasty, but what the heck?
        // The Recordings thread is locked and the object is going to be deleted anyway.
        if (((cRecordingInfo *)Recording->Info())->Read(f) && Recording->WriteInfo())
           return true;
        tainted = true;
        esyslog("remotetimers: error in aux string '%s'", *buffer);
	}
     else
        esyslog("remotetimers: error in fmemopen: %m");
     }
  else
     esyslog("remotetimers: '%s' not writeable: %m", *InfoFileName);
  Skins.Message(mtError, trREMOTETIMERS("Unable to update user ID"));
  return tainted;
}

#define PRIO_LIFE_FORMAT ".%02d.%02d.rec"
bool cMenuEditRecording::UpdateData(cRecording *Recording)
{
  bool renamed = false;

  char *newName = strdup(Recording->FileName());
  size_t len = strlen(newName);
  cString oldData = cString::sprintf(PRIO_LIFE_FORMAT, Recording->priority, Recording->lifetime);
  cString newData = cString::sprintf(PRIO_LIFE_FORMAT, priority, lifetime);
  size_t lenReplace = strlen(oldData);
  if (lenReplace < len) {
     if (strlen(newData) == lenReplace) {
        char *p = newName + len - lenReplace;
        if (strcmp(p, oldData) == 0) {
           strncpy(p, newData, lenReplace);
           renamed = Rename(Recording->FileName(), newName);
           }
        else
           esyslog("remotetimers: unexpected filename '%s'", fileName);
        }
     else
        esyslog("remotetimers: invalid priority/lifetime data for '%s'", fileName);
     }
  else
     esyslog("remotetimers: short filename '%s'", fileName);
  
  if (renamed) {
     free(fileName);
     fileName = newName;
     }
  else {
     Skins.Message(mtError, trREMOTETIMERS("Unable to change priority/lifetime"));
     free(newName);
     }
  return renamed;
}

bool cMenuEditRecording::UpdateName(cRecording *Recording)
{
  bool renamed = false;

  char *oldFileName = strdup(Recording->FileName());
  char *p = strrchr(oldFileName, '/');
  if (p) {
     cString newFileName(ExchangeChars(strdup(name), true), true);
     newFileName = cString::sprintf("%s/%s%s", VideoDirectory, *newFileName, p);
     renamed = Rename(oldFileName, newFileName);
     if (renamed) {
	free(fileName);
	fileName = strdup(newFileName);
        }
     }
  if (!renamed)
     Skins.Message(mtError, trREMOTETIMERS("Unable to rename recording"));
  free(oldFileName);
  return renamed;
}

bool cMenuEditRecording::Rename(const char *Old, const char *New) {
  if (access(New, F_OK) == 0) {
     esyslog("remotetimers: not renaming '%s' to '%s'. File exists", Old, New);
     return false;
     }
  // false makes sure that the actual target directory itself is not created
  if (!MakeDirs(New, false))
     return false;
  if (rename(Old, New) != 0) {
     esyslog("remotetimers: error renaming '%s' to '%s': %m", Old, New);
     return false;
     }
  return true;
}

void cMenuEditRecording::SetHelpKeys()
{
  SetHelp(RecordingCommands.Count() ? tr("Commands") : NULL, trREMOTETIMERS("Cut"));
}

eOSState cMenuEditRecording::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:    return Commands();
       case kGreen:  return Cut();
       case kInfo:   return Info();
       case kOk:     {
                        cThreadLock recordingsLock(&Recordings);
                        cRecording *recording = Recordings.GetByName(fileName);
                        if (recording) {
                           bool replace = false;
                           if (user != tmpUser)
                              replace |= UpdateUser(recording, tmpUser);
                           if (priority != recording->priority || lifetime != recording->lifetime)
                              replace |= UpdateData(recording);
                           if (strcmp(name, recording->Name()) != 0)
                              replace |= UpdateName(recording);
                           if (replace) {
                              Recordings.Del(recording);
                              Recordings.AddByName(fileName);
                              }
                           }
                        else
                           Skins.Message(mtError, tr("Error while accessing recording!"));
		        return osBack;
                     }
       default: break;
       }
     }
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
  char *fileName;
  char *name;
  int user;
  int totalEntries, newEntries;
public:
  cMenuRecordingItem(cRecording *Recording, int Level, int User);
  ~cMenuRecordingItem();
  void IncrementCounter(bool New);
  const char *Name(void) { return name; }
  const char *FileName(void) { return fileName; }
  int User(void) { return user; } const
  bool IsDirectory(void) { return name != NULL; }
  };

cMenuRecordingItem::cMenuRecordingItem(cRecording *Recording, int Level, int User)
{
  fileName = strdup(Recording->FileName());
  name = NULL;
  user = User;
  totalEntries = newEntries = 0;
  SetText(Recording->Title('\t', true, Level));
  if (*Text() == '\t')
     name = strdup(Text() + 2); // 'Text() + 2' to skip the two '\t'
}

cMenuRecordingItem::~cMenuRecordingItem()
{
  free(fileName);
  free(name);
}

void cMenuRecordingItem::IncrementCounter(bool New)
{
  totalEntries++;
  if (New)
     newEntries++;
  SetText(cString::sprintf("%d\t%d\t%s", totalEntries, newEntries, name));
}

// --- cMenuRecordings -------------------------------------------------------

int cMenuRecordings::userFilter = 0;
bool cMenuRecordings::replayEnded = false;

cMenuRecordings::cMenuRecordings(const char *Base, int Level, bool OpenSubMenus)
:cOsdMenu(Base ? Base : tr("Recordings"), 9, 7)
{
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
  if (FreeDiskSpace.HasChanged(Force)) {
     //XXX -> skin function!!!
     if (userFilter == 0)
        SetTitle(cString::sprintf("%s - %s", base ? base : tr("Recordings"), FreeDiskSpace.FreeDiskSpaceString()));
     else
        SetTitle(cString::sprintf("%s - %s - %s %d", base ? base : tr("Recordings"), FreeDiskSpace.FreeDiskSpaceString(), trREMOTETIMERS("User"), userFilter));
     return true;
     }
  return false;
}

void cMenuRecordings::SetHelpKeys(void)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  int NewHelpKeys = 0;
  if (ri) {
     if (ri->IsDirectory())
        NewHelpKeys = 1;
     else {
        NewHelpKeys = 2;
        cRecording *recording = GetRecording(ri);
        if (recording && recording->Info()->Title())
           NewHelpKeys = 3;
        }
        if (userFilter != 0 && ri->User() != 0 && ri->User() != USER_MASK(userFilter))
	   NewHelpKeys += 2;
     }
  if (NewHelpKeys != helpKeys) {
     switch (NewHelpKeys) {
       case 0: SetHelp(NULL); break;
       case 1: SetHelp(tr("Button$Open")); break;
       case 2:
       case 3: //SetHelp(RecordingCommands.Count() ? tr("Commands") : tr("Button$Play"), tr("Button$Rewind"), tr("Button$Delete"), NewHelpKeys == 3 ? tr("Button$Info") : NULL);
       case 4:
       // TRANSLATORS: Button displayed instead of "Delete" when there are other users who didn't watch the recording yet
       case 5: SetHelp(tr("Button$Edit"), tr("Button$Rewind"), NewHelpKeys > 3 ? trREMOTETIMERS("Release") : tr("Button$Delete"), NewHelpKeys == 3 ? tr("Button$Info") : NULL);
       }
     helpKeys = NewHelpKeys;
     }
}

void cMenuRecordings::Set(bool Refresh)
{
  const char *CurrentRecording = cReplayControl::LastReplayed();
  cMenuRecordingItem *LastItem = NULL;
  char *LastItemText = NULL;
  cThreadLock RecordingsLock(&Recordings);
  if (Refresh) {
     cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
     if (ri) {
        cRecording *Recording = Recordings.GetByName(ri->FileName());
        if (Recording)
           CurrentRecording = Recording->FileName();
        }
     }
  Clear();
  Recordings.Sort();
  for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
      int user = ParseUser(recording->Info()->Aux());
      if (userFilter != 0 && user != 0 && (user & USER_MASK(userFilter)) == 0)
         continue;
      if (!base || (strstr(recording->Name(), base) == recording->Name() && recording->Name()[strlen(base)] == '~')) {
         cMenuRecordingItem *Item = new cMenuRecordingItem(recording, level, user);
         if (*Item->Text() && (!LastItem || strcmp(Item->Text(), LastItemText) != 0)) {
            Add(Item);
            LastItem = Item;
            free(LastItemText);
            LastItemText = strdup(LastItem->Text()); // must use a copy because of the counters!
            }
         else
            delete Item;
         if (LastItem) {
            if (CurrentRecording && strcmp(CurrentRecording, recording->FileName()) == 0)
               SetCurrent(LastItem);
            if (LastItem->IsDirectory())
               LastItem->IncrementCounter(recording->IsNew());
            }
         }
      }
  free(LastItemText);
  Refresh |= SetFreeDiskDisplay(Refresh);
  if (Refresh)
     Display();
}

cRecording *cMenuRecordings::GetRecording(cMenuRecordingItem *Item)
{
  cRecording *recording = Recordings.GetByName(Item->FileName());
  if (!recording)
     Skins.Message(mtError, tr("Error while accessing recording!"));
  return recording;
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
        cRecording *recording = GetRecording(ri);
        if (recording) {
           cReplayControl::SetRecording(recording->FileName(), recording->Title());
	   // use our own replay control which returns to this recordings menu
	   cControl::Shutdown();
	   cControl::Launch(new cRemoteTimersReplayControl(cMenuMain::IsOpen()));
	   return osEnd;
           // return osReplay;
           }
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
     cResumeFile ResumeFile(ri->FileName());
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
        cRecording *recording = Recordings.GetByName(ri->FileName());
        if (recording) {
	   if (cMenuEditRecording::UpdateUser(recording, ri->User() ^ USER_MASK(userFilter))) {
              cReplayControl::ClearLastReplayed(ri->FileName());
              cOsdMenu::Del(Current());
              SetHelpKeys();
              Display();
              if (!Count())
                 return osBack;
	      }
	   }
	else
           Skins.Message(mtError, tr("Error while accessing recording!"));
	}
     else if (Interface->Confirm(tr("Delete recording?"))) {
        cRecordControl *rc = cRecordControls::GetRecordControl(ri->FileName());
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
        cRecording *recording = GetRecording(ri);
        if (recording) {
#if VDRVERSNUM >= 10515
           if (cReplayControl::NowReplaying() && strcmp(cReplayControl::NowReplaying(), ri->FileName()) == 0)
              cControl::Shutdown();
#endif
           if (recording->Delete()) {
              cReplayControl::ClearLastReplayed(ri->FileName());
              Recordings.DelByName(ri->FileName());
              cOsdMenu::Del(Current());
              SetHelpKeys();
              SetFreeDiskDisplay(true);
              Display();
              if (!Count())
                 return osBack;
              }
           else
              Skins.Message(mtError, tr("Error while deleting recording!"));
           }
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Info(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     cRecording *recording = GetRecording(ri);
     if (recording && recording->Info()->Title())
        return AddSubMenu(new cMenuRecording(recording, true));
     }
  return osContinue;
}

eOSState cMenuRecordings::Edit(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     cRecording *recording = GetRecording(ri);
     if (recording) {
        cThreadLock RecordingsLock(&Recordings);
        return AddSubMenu(new cMenuEditRecording(recording));
        }
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
     cRecording *recording = GetRecording(ri);
     if (recording) {
        cMenuCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuCommands(tr("Recording commands"), &RecordingCommands, cString::sprintf("\"%s\"", *strescape(recording->FileName(), "\\\"$"))));
        if (Key != kNone)
           state = menu->ProcessKey(Key);
        return state;
        }
     }
  return osContinue;
}
*/

eOSState cMenuRecordings::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kPlay:
       case kOk:     return Play();
       //case kRed:    return (helpKeys > 1 && RecordingCommands.Count()) ? Commands() : Play();
       case kRed:    return Edit();
       case kGreen:  return Rewind();
       case kYellow: return Delete();
       case kInfo:
       case kBlue:   return Info();
       //case k1...k9: return Commands(Key);
       case k0 ... k9:
                     userFilter = Key - k0;
		     if (RemoteTimersSetup.userFilterRecordings < 0 && ::Setup.ResumeID != Key - k0) {
		        ::Setup.ResumeID = Key - k0;
			Recordings.ResetResume();
			}
                     Set(true);
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
        // SetFreeDiskDisplay();
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
  count++;
  SetHasHotkeys();
  Add(new cOsdItem(hk(tr("Schedule")),   osUser1));
  Add(new cOsdItem(hk(tr("Timers")),     osUser2));
  Add(new cOsdItem(hk(tr("Recordings")), osUser3));
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
    default:           return state;
    }
}

}; // namespace PluginRemoteTimers
