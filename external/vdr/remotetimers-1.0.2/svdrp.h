/*
 * Copyright (C) 2008-2013 Frank Schmirler <vdr@schmirler.de>
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

#ifndef _REMOTETIMERS_SVDRP__H
#define _REMOTETIMERS_SVDRP__H

#include <stdlib.h>
#include "svdrpservice.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>

class cSvdrp {
	private:
		static cSvdrp        *svdrp;

		SvdrpConnection_v1_0 conn;
		cPlugin              *service;
		int                  refcount;
	public:
		static cSvdrp* GetInstance();
		static void DeleteInstance();

		cSvdrp(cPlugin *Service);
		~cSvdrp();
		bool Connect(const char* ServerIp = NULL, unsigned short ServerPort = 0);
		void Disconnect();
		bool Offline() { return conn.handle == -1; }
		unsigned short Send(SvdrpCommand_v1_0 *Cmd);
};

class cRemoteTimer: public cTimer {
	private:
		int id;
		char *orig;
	public:
		cRemoteTimer();
		cRemoteTimer(const cEvent* &Event);
		virtual ~cRemoteTimer();
		cRemoteTimer& operator= (const cRemoteTimer &Timer);
		int Id() const { return id; }
		const char* Orig() const { return orig; }
		bool Parse(const char *s);
};

enum eRemoteTimersState { rtsOk, rtsRefresh, rtsLocked, rtsRecording, rtsRejected, rtsUpdated, rtsUnexpected, rtsConnError };

class cRemoteTimers: public cList<cRemoteTimer> {
	private:
		unsigned short CmdDELT(cRemoteTimer *Timer);
		unsigned short CmdLSTT(int Number, char*& TimerString);
		unsigned short CmdMODT(cRemoteTimer *Timer);
		unsigned short CmdNEWT(cRemoteTimer *Timer, int& Number);

		unsigned int GetFlags(const char *TimerString, const char*& Tail);
		eRemoteTimersState Verify(cRemoteTimer *Timer);
		eRemoteTimersState List(int Number, char*& TimerString);
	public:
		cString GetErrorMessage(eRemoteTimersState state);
		eRemoteTimersState Refresh();
		eRemoteTimersState Delete(cRemoteTimer *Timer);
		eRemoteTimersState Modify(cRemoteTimer *Timer);
		eRemoteTimersState New(cRemoteTimer *Timer);
		cRemoteTimer* GetMatch(const cEvent *Event, int *Match);
		cRemoteTimer* GetTimer(cTimer *Timer);
		cRemoteTimer* GetTimer(int Id);
};

enum eRemoteRecordingsState { rrsOk, rrsLocked, rrsNotFound, rrsUnexpected, rrsConnError };

class cRemoteRecordings {
	private:
		unsigned short CmdLSTR(const char *Date, const char *Title, unsigned int &Number);
		unsigned short CmdEDIT(unsigned int Number);
	public:
		cString GetErrorMessage(eRemoteRecordingsState state);
		eRemoteRecordingsState Cut(const char *Date, const char *Title);
};

extern cRemoteTimers RemoteTimers;
extern cRemoteRecordings RemoteRecordings;

#endif //_REMOTETIMERS_SVDRP__H
