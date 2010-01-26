/*
 * svdrp.h: SVDRP interface for remote timers
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 */

#ifndef _REMOTETIMERS_SVDRP__H
#define _REMOTETIMERS_SVDRP__H

#include <stdlib.h>
#include "../svdrpservice/svdrpservice.h"
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
		bool Connect();
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
