/*
 * Copyright (C) 2008-2011 Frank Schmirler <vdr@schmirler.de>
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

#include <stdlib.h>
#include <ctype.h>
#include "svdrp.h"
#include "setup.h"
#include "i18n.h"

// cSvdrp -------------------------------------------------

cSvdrp* cSvdrp::svdrp = NULL;

cSvdrp* cSvdrp::GetInstance() {
	if (!svdrp)
		svdrp = new cSvdrp(cPluginManager::GetPlugin("svdrpservice"));
	return svdrp;
}

void cSvdrp::DeleteInstance() {
	DELETENULL(svdrp);
}

cSvdrp::cSvdrp(cPlugin *Service) {
	service = Service;
	refcount = 0;
	conn.handle = -1;
}

cSvdrp::~cSvdrp() {
	if (conn.handle >= 0)
		service->Service("SvdrpConnection-v1.0", &conn);
}

bool cSvdrp::Connect(const char* ServerIp, unsigned short ServerPort) {
	refcount++;
	if (!service)
		esyslog("remotetimers: Plugin svdrpservice not available.");
	else if (conn.handle < 0) {
		conn.serverIp = (ServerIp && *ServerIp) ? ServerIp : RemoteTimersSetup.serverIp;
		conn.serverPort = ServerPort != 0 ? ServerPort : RemoteTimersSetup.serverPort;
		conn.shared = true;
		service->Service("SvdrpConnection-v1.0", &conn);
		if (conn.handle < 0)
			RemoteTimers.Clear();
	}
	return conn.handle >= 0;
}

void cSvdrp::Disconnect() {
	refcount--;
	if (!refcount && conn.handle >= 0) {
		service->Service("SvdrpConnection-v1.0", &conn);
		conn.handle = -1;
	}
}

unsigned short cSvdrp::Send(SvdrpCommand_v1_0 *Cmd) {
	Cmd->handle = conn.handle;
	service->Service("SvdrpCommand-v1.0", Cmd);
	return Cmd->responseCode;
}

cRemoteTimers RemoteTimers;

// cRemoteTimer -------------------------------------------------

cRemoteTimer::cRemoteTimer(): id(-1), orig(NULL) {
}

cRemoteTimer::cRemoteTimer(const cEvent* &Event): cTimer(Event), id(-1), orig(NULL) {
}

cRemoteTimer::~cRemoteTimer() {
	free(orig);
}

cRemoteTimer& cRemoteTimer::operator= (const cRemoteTimer &Timer) {
	if (this != &Timer) {
		cTimer::operator=(Timer);
		id = Timer.id;
		orig = Timer.orig ? strdup(Timer.orig) : NULL;
	}
	return *this;
}

bool cRemoteTimer::Parse(const char *s) {
	bool result = false;

	free(orig);
	orig = NULL;
	id = -1;
	if (s) {
		orig = strdup(s);
		id = atoi(s);
		s = strchr(s, ' ');
	}
	if (s && id > 0) {
		result = cTimer::Parse(s);
		// Parse clears the recording flag
		if (result && (atoi(s) & tfRecording))
			SetRecording(true);
	}
	return result;
}

// cRemoteTimers -------------------------------------------------

cString cRemoteTimers::GetErrorMessage(eRemoteTimersState state) {
	switch (state) {
		case rtsRefresh:
			return cString(trNOOP("Updated remote timers list"));
		case rtsLocked:
			return cString(trNOOP("Timers are being edited remotely - please try again"));
		case rtsRecording:
			return cString(trNOOP("Timer is recording - please deactivate it first"));
		case rtsRejected:
			return cString(trNOOP("Timer already defined"));
		case rtsUpdated:
			return cString(trNOOP("Timers modified remotely - please check remote timers"));
		case rtsUnexpected:
			return cString(trNOOP("Unexpected error - please check remote timers"));
		case rtsConnError:
			return cString(trNOOP("Lost SVDRP connection - please check remote timers"));
		default:
			return cString();
	}
}

unsigned int cRemoteTimers::GetFlags(const char *TimerString, const char*& Tail) {
	int count;
	unsigned int flags;
	Tail = TimerString;
	if (sscanf(TimerString, " %*u %u :%n", &flags, &count) > 0)
		Tail += count;
	// mask recording flag
	return flags & ~tfRecording;
}

eRemoteTimersState cRemoteTimers::Refresh() {
	SvdrpCommand_v1_0 cmd;
	eRemoteTimersState state;

	cmd.command = RemoteTimersSetup.useChannelId ? "LSTT id\r\n" : "LSTT\r\n";
	cSvdrp::GetInstance()->Send(&cmd);

	Clear();
	if (cmd.responseCode == 250) {
		for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line)) {
			cRemoteTimer* timer = new cRemoteTimer();
			if (timer->Parse(line->Text()))
				Add(timer);
			else {
				esyslog("remotetimers: Error parsing timer %s", line->Text());
				delete timer;
			}
		}
		state = rtsOk;
	}
	else if (cmd.responseCode == 550)
		state = rtsOk;	//no timers defined
	else if (cmd.responseCode < 100)
		state = rtsConnError;
	else
		state = rtsUnexpected;
	return state;
}

eRemoteTimersState cRemoteTimers::New(cRemoteTimer *Timer) {
	char *s;
	int num;
	unsigned short result;
	eRemoteTimersState state;

	result = CmdNEWT(Timer, num);

	if (result == 250 && num > 0) {
		state = List(num, s);
		if (state == rtsOk && s) {
			if (Timer->Parse(s)) {
				Add(Timer);
				if (num != Count())
					state = rtsRefresh;
			}
			else
				state = rtsUnexpected;
		}
		free(s);
	}
	else if (result < 100)
		state = rtsConnError;
	else if (result == 550)
		state = rtsRejected;
	else
		state = rtsUnexpected;
	return state;
}

eRemoteTimersState cRemoteTimers::Delete(cRemoteTimer *Timer) {
	unsigned short result;
	eRemoteTimersState state;
	
	state = Verify(Timer);
	if (state == rtsOk) {
		result = CmdDELT(Timer);
		if (result == 250) {
			Del(Timer);
			// DELT changes timer numbers
			state = rtsRefresh;
		}
		else if (result < 100)
			state = rtsConnError;
		else if (result == 550)
			state = Timer->Recording() ? rtsRecording : rtsLocked;
		else
			state = rtsUnexpected;
	}
	return state;
}

eRemoteTimersState cRemoteTimers::Modify(cRemoteTimer *Timer) {
	unsigned short result;
	eRemoteTimersState state;
	
	state = Verify(Timer);
	if (state == rtsOk) {
		result = CmdMODT(Timer);
		if (result == 250) {
			char *s = NULL;
			state = List(Timer->Id(), s);
			if (state == rtsOk && s) {
				if (!Timer->Parse(s))
					result = rtsUnexpected;
			}
			free(s);
		}
		else if (result < 100)
			state = rtsConnError;
		else if (result == 550)
			state = rtsLocked;
		else
			state = rtsUnexpected;
	}
	return state;
}

eRemoteTimersState cRemoteTimers::Verify(cRemoteTimer *Timer) {
	unsigned short result;
	eRemoteTimersState state;
	
	char *s = NULL;
	result = CmdLSTT(Timer->Id(), s);
	if (result == 250) {
		if (s) {
			const char *orig, *curr;
			// masks the recording flag
			unsigned int origFlags = GetFlags(Timer->Orig(), orig);
			unsigned int currFlags = GetFlags(s, curr);
			// compare flags and the rest
			if (origFlags == currFlags && strcmp(orig, curr) == 0)
				state = rtsOk;
			else
				state = rtsUpdated;
		}
		else
			state = rtsUnexpected;
	}
	else
		state = (result < 100) ? rtsConnError : rtsUpdated;
	free(s);
	return state;
}

eRemoteTimersState cRemoteTimers::List(int Number, char*& TimerString) {
	unsigned short result;
	eRemoteTimersState state;
	
	result = CmdLSTT(Number, TimerString);
	if (result == 250) {
		if (TimerString)
			state = rtsOk;
		else
			state = rtsUnexpected;
	}
	else
		state = (result < 100) ? rtsConnError : rtsUpdated;
	return state;
}

cRemoteTimer *cRemoteTimers::GetTimer(cTimer *Timer)
{
	for (cRemoteTimer *ti = First(); ti; ti = Next(ti)) {
		if (ti->Channel() == Timer->Channel() &&
				((ti->WeekDays() && ti->WeekDays() == Timer->WeekDays()) || (!ti->WeekDays() && ti->Day() == Timer->Day())) &&
				ti->Start() == Timer->Start() &&
				ti->Stop() == Timer->Stop())
			return ti;
	}
	return NULL;
}

cRemoteTimer *cRemoteTimers::GetTimer(int Id)
{
	for (cRemoteTimer *ti = First(); ti; ti = Next(ti)) {
		if (Id == ti->Id())
			return ti;
	}
	return NULL;
}

cRemoteTimer *cRemoteTimers::GetMatch(const cEvent *Event, int *Match)
{
	cRemoteTimer *t = NULL;
	int m = tmNone;
	for (cRemoteTimer *ti = First(); ti; ti = Next(ti)) {
		int tm = ti->Matches(Event);
		if (tm > m) {
			t = ti;
			m = tm;
			if (m == tmFull)
				break;
		}
	}
	if (Match)
		*Match = m;
	return t;
}

unsigned short cRemoteTimers::CmdDELT(cRemoteTimer *Timer) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("DELT %d\r\n", Timer->Id());
	return cSvdrp::GetInstance()->Send(&cmd);
}

unsigned short cRemoteTimers::CmdMODT(cRemoteTimer *Timer) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("MODT %d %s", Timer->Id(), *Timer->ToText(RemoteTimersSetup.useChannelId));
	return cSvdrp::GetInstance()->Send(&cmd);
}

unsigned short cRemoteTimers::CmdNEWT(cRemoteTimer *Timer, int &Number) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("NEWT %s", *Timer->ToText(RemoteTimersSetup.useChannelId));
	cSvdrp::GetInstance()->Send(&cmd);

	Number = (cmd.responseCode == 250) ? atoi(cmd.reply.First()->Text()) : -1;
	return cmd.responseCode;
}

unsigned short cRemoteTimers::CmdLSTT(int Number, char*& s) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("LSTT %s%d\r\n", RemoteTimersSetup.useChannelId ? "id " : "", Number);
	cSvdrp::GetInstance()->Send(&cmd);

	s = (cmd.responseCode == 250) ? strdup(cmd.reply.First()->Text()) : NULL;
	return cmd.responseCode;
}
// cRemoteRecordings -------------------------------------------------

cRemoteRecordings RemoteRecordings;

cString cRemoteRecordings::GetErrorMessage(eRemoteRecordingsState state) {
	switch (state) {
		case rrsLocked:
			return cString(trNOOP("Remote cutter already active or no marks defined"));
		case rrsNotFound:
			return cString(trNOOP("Recording not found on remote VDR - use local cutter"));
		case rrsUnexpected:
			return cString(trNOOP("Unexpected error - unable to start remote cutter"));
		case rrsConnError:
			return cString(trNOOP("Lost SVDRP connection - unable to start remote cutter"));
		default:
			return cString();
	}
}
eRemoteRecordingsState cRemoteRecordings::Cut(const char *Date, const char *Title) {
	unsigned int Number = 0;
	unsigned short result;
	eRemoteRecordingsState state;

	result = CmdLSTR(Date, Title, Number);
	if (result == 250) {
		if (Number > 0) {
			result = CmdEDIT(Number);
			if (result == 250)
				state = rrsOk;
			else if (result == 554)
				state = rrsLocked;
			else if (result < 100)
				state = rrsConnError;
			else
				state = rrsUnexpected;
		}
		else
			state = rrsNotFound;
	}
	else
		state = (result < 100) ? rrsConnError : rrsNotFound;
	return state;
}

unsigned short cRemoteRecordings::CmdLSTR(const char *Date, const char *Title, unsigned int &Number) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("LSTR\r\n");
	cSvdrp::GetInstance()->Send(&cmd);

	Number = 0;
	if (cmd.responseCode == 250) {
		for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line)) {
			unsigned int recnum;
			char *rectext;
			if (sscanf(line->Text(), " %u %a[^\n]", &recnum, &rectext) >= 2) {
				if (strstr(rectext, Date) == rectext)
				{
					char *p = rectext + strlen(Date);

					int dummy = 0;
					int len = 0;
					// strip recording length (VDR 1.7.21+)
					if (isspace(*p) && !isspace(*(p+1)) && sscanf(p, " %*d:%2d%n", &dummy, &len) > 0)
						p += len;
					// new indicator
					if (*p == '*')
						p++;
					p = skipspace(p);
					if (strcmp(Title, p) == 0)
						Number = recnum;
				}
				free(rectext);
			}
		}
	}
	return cmd.responseCode;
}

unsigned short cRemoteRecordings::CmdEDIT(unsigned int Number) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("EDIT %d\r\n", Number);
	return cSvdrp::GetInstance()->Send(&cmd);
}

