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

#ifndef _REMOTETIMERS_SETUP__H
#define _REMOTETIMERS_SETUP__H

#include <vdr/config.h>
#include "menuitems.h"

#define MAX_IP_LENGTH 16
#define EPGTIME_LENGTH 4

struct cRemoteTimersSetup {
	int hideMainMenuEntry;
	int replaceSchedule;
	int replaceTimers;
	int replaceRecordings;
	char serverIp[MAX_IP_LENGTH];
	int serverPort;
	int useChannelId;
	int swapOkBlue;
	int showProgressBar;
	int epgTime[EPGTIME_LENGTH];
	int userFilterSchedule;
	int userFilterTimers;
	int userFilterRecordings;
	int skinSchedule;
	int skinTimers;
	int skinRecordings;
	int defaultUser;
	int addToRemote;
	int remotePause;
	int remoteInstant;
	int moveBandwidth;
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
