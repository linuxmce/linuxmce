/*
 * setup.h: Settings
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _REMOTETIMERS_SETUP__H
#define _REMOTETIMERS_SETUP__H

#include <vdr/config.h>
#include "menuitems.h"

#define MAX_IP_LENGTH 16

struct cRemoteTimersSetup {
	int hideMainMenuEntry;
	int replaceSchedule;
	int replaceTimers;
	int replaceRecordings;
	char serverIp[MAX_IP_LENGTH];
	int serverPort;
	int swapOkBlue;
	int showProgressBar;
	int userFilterSchedule;
	int userFilterTimers;
	int userFilterRecordings;
	int defaultUser;
	int addToRemote;
	int remotePause;
	int remoteInstant;
	char serverDir[MaxFileName];
	int watchUpdate;

	bool Parse(const char *Name, const char *Value);
	cRemoteTimersSetup& operator=(const cRemoteTimersSetup &Setup);
	cRemoteTimersSetup();
};

extern cRemoteTimersSetup RemoteTimersSetup;

class cRemoteTimersMenuSetup: public cMenuSetupPage {
	private:
		cRemoteTimersSetup setupTmp;
		const char *lastServerDir;
		const char *swapOkBlueFalse;
		const char *swapOkBlueTrue;
		void Set();
	protected:
		virtual void Store(void);
	public:
		virtual eOSState ProcessKey(eKeys Key);
		cRemoteTimersMenuSetup();
		virtual ~cRemoteTimersMenuSetup();
};

#endif //_REMOTETIMERS_SETUP__H
