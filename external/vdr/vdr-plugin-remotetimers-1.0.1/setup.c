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

#include <vdr/config.h>
#include "setup.h"
#include "menuitems.h"
#include "watcher.h"
#include "i18n.h"

cRemoteTimersSetup RemoteTimersSetup;

char* copyFilename(char *dest, const char *src, size_t n) {
	while (*src == FOLDERDELIMCHAR)
		src++;
	strn0cpy(dest, src, n);
	for (int i = strlen(dest) - 1; i >= 0 && dest[i] == FOLDERDELIMCHAR; i--)
		dest[i] = '\0';
	return dest;
}

cRemoteTimersSetup::cRemoteTimersSetup() {
	hideMainMenuEntry = 0;
	replaceSchedule = 0;
	replaceTimers = 0;
	replaceRecordings = 0;
	serverIp[0] = 0;
	serverPort = 0;
	useChannelId = 1;
	swapOkBlue = 0;
	showProgressBar = 0;
	for (int i = 0; i < EPGTIME_LENGTH; ++i)
		epgTime[i] = 0;
	userFilterSchedule = 0;
	userFilterTimers = 0;
	userFilterRecordings = 0;
	skinSchedule = 0;
	skinTimers = 0;
	skinRecordings = 0;
	defaultUser = 0;
	addToRemote = 1;
	remotePause = 0;
	remoteInstant = 0;
	moveBandwidth = 0;
	serverDir[0] = 0;
	watchUpdate = 1;
}

cRemoteTimersSetup& cRemoteTimersSetup::operator=(const cRemoteTimersSetup &Setup) {
	hideMainMenuEntry = Setup.hideMainMenuEntry;
	replaceSchedule = Setup.replaceSchedule;
	replaceTimers = Setup.replaceTimers;
	replaceRecordings = Setup.replaceRecordings;
	strn0cpy(serverIp, Setup.serverIp, sizeof(serverIp));
	serverPort = Setup.serverPort;
	useChannelId = Setup.useChannelId;
	swapOkBlue = Setup.swapOkBlue;
	showProgressBar = Setup.showProgressBar;
	for (int i = 0; i < EPGTIME_LENGTH; ++i)
		epgTime[i] = Setup.epgTime[i];
	userFilterSchedule = Setup.userFilterSchedule;
	userFilterTimers = Setup.userFilterTimers;
	userFilterRecordings = Setup.userFilterRecordings;
	skinSchedule = Setup.skinSchedule;
	skinTimers = Setup.skinTimers;
	skinRecordings = Setup.skinRecordings;
	defaultUser = Setup.defaultUser;
	addToRemote = Setup.addToRemote;
	remotePause = Setup.remotePause;
	remoteInstant = Setup.remoteInstant;
	moveBandwidth = Setup.moveBandwidth;
	copyFilename(serverDir, Setup.serverDir, sizeof(serverDir));
	watchUpdate = Setup.watchUpdate;
	return *this;
}

bool cRemoteTimersSetup::Parse(const char *Name, const char *Value) {
	if (!strcasecmp(Name, "HideMainMenuEntry"))
		hideMainMenuEntry = atoi(Value);
	else if (!strcasecmp(Name, "ReplaceSchedule"))
		replaceSchedule = atoi(Value);
	else if (!strcasecmp(Name, "ReplaceTimers"))
		replaceTimers = atoi(Value);
	else if (!strcasecmp(Name, "ReplaceRecordings"))
		replaceRecordings = atoi(Value);
	else if (!strcasecmp(Name, "ServerIp"))
		strn0cpy(serverIp, Value, sizeof(serverIp));
	else if (!strcasecmp(Name, "ServerPort"))
		serverPort = atoi(Value);
	else if (!strcasecmp(Name, "UseChannelId"))
		useChannelId = atoi(Value);
	else if (!strcasecmp(Name, "SwapOkBlue"))
		swapOkBlue = atoi(Value);
	else if (!strcasecmp(Name, "ShowProgressBar"))
		showProgressBar = atoi(Value);
	else if (!strncasecmp(Name, "EpgTime", 7)) {
		int i = atoi(Name + 7);
		if (0 <= i && i < EPGTIME_LENGTH)
			epgTime[i] = atoi(Value);
	}
	else if (!strcasecmp(Name, "UserFilterSchedule"))
		userFilterSchedule = atoi(Value);
	else if (!strcasecmp(Name, "UserFilterTimers"))
		userFilterTimers = atoi(Value);
	else if (!strcasecmp(Name, "UserFilterRecordings"))
		userFilterRecordings = atoi(Value);
	else if (!strcasecmp(Name, "SkinSchedule"))
		skinSchedule = atoi(Value);
	else if (!strcasecmp(Name, "SkinTimers"))
		skinTimers = atoi(Value);
	else if (!strcasecmp(Name, "SkinRecordings"))
		skinRecordings = atoi(Value);
	else if (!strcasecmp(Name, "DefaultUser"))
		defaultUser = PluginRemoteTimers::cMenuEditUserItem::Parse(Value);
	else if (!strcasecmp(Name, "AddToRemote"))
		addToRemote = atoi(Value);
	else if (!strcasecmp(Name, "RemotePause"))
		remotePause = atoi(Value);
	else if (!strcasecmp(Name, "RemoteInstant"))
		remoteInstant = atoi(Value);
	else if (!strcasecmp(Name, "MoveBandwidth"))
		moveBandwidth = atoi(Value);
	else if (!strcasecmp(Name, "serverDir"))
		copyFilename(serverDir, Value, sizeof(serverDir));
	else if (!strcasecmp(Name, "WatchUpdate"))
		watchUpdate = atoi(Value);
	else
		return false;
	return true;
}

void cRemoteTimersMenuSetup::Store() {
	SetupStore("HideMainMenuEntry", setupTmp.hideMainMenuEntry);
	SetupStore("ReplaceSchedule", setupTmp.replaceSchedule);
	SetupStore("ReplaceTimers", setupTmp.replaceTimers);
	SetupStore("ReplaceRecordings", setupTmp.replaceRecordings);
	SetupStore("ServerIp", setupTmp.serverIp);
	SetupStore("ServerPort", setupTmp.serverPort);
	SetupStore("UseChannelId", setupTmp.useChannelId);
	SetupStore("SwapOkBlue", setupTmp.swapOkBlue);
	SetupStore("ShowProgressBar", setupTmp.showProgressBar);
	for (int i = 0; i < EPGTIME_LENGTH; ++i)
		SetupStore(cString::sprintf("EpgTime%d", i), setupTmp.epgTime[i]);
	SetupStore("UserFilterSchedule", setupTmp.userFilterSchedule);
	SetupStore("UserFilterTimers", setupTmp.userFilterTimers);
	SetupStore("UserFilterRecordings", setupTmp.userFilterRecordings);
	SetupStore("SkinSchedule", setupTmp.skinSchedule);
	SetupStore("SkinTimers", setupTmp.skinTimers);
	SetupStore("SkinRecordings", setupTmp.skinRecordings);
	SetupStore("DefaultUser", PluginRemoteTimers::cMenuEditUserItem::ToString(setupTmp.defaultUser));
	SetupStore("AddToRemote", setupTmp.addToRemote);
	SetupStore("RemotePause", setupTmp.remotePause);
	SetupStore("RemoteInstant", setupTmp.remoteInstant);
	SetupStore("MoveBandwidth", setupTmp.moveBandwidth);
	SetupStore("ServerDir", setupTmp.serverDir);
	SetupStore("WatchUpdate", setupTmp.watchUpdate);
	bool serverDirUpdated = RemoteTimersSetup.watchUpdate != setupTmp.watchUpdate ||
			strcmp(RemoteTimersSetup.serverDir, setupTmp.serverDir) != 0;
	RemoteTimersSetup = setupTmp;
	if (serverDirUpdated)
		cUpdateWatcher::GetInstance()->Reconfigure();
		
}

cRemoteTimersMenuSetup::cRemoteTimersMenuSetup() {
	setupTmp = RemoteTimersSetup;
	lastServerDir = strdup(setupTmp.serverDir);
	swapOkBlueFalse = strdup(cString::sprintf("%s/%s", tr("Button$Info"), tr("Button$Switch")));
	swapOkBlueTrue = strdup(cString::sprintf("%s/%s", tr("Button$Switch"), tr("Button$Info")));
	Set();
}

cRemoteTimersMenuSetup::~cRemoteTimersMenuSetup() {
	free((void *) lastServerDir);
	free((void *) swapOkBlueFalse);
	free((void *) swapOkBlueTrue);
}

void cRemoteTimersMenuSetup::Set() {
	int current = Current();
	Clear();
	Add(new cMenuEditStrItem(trREMOTETIMERS("Server IP"), setupTmp.serverIp, sizeof(setupTmp.serverIp), ".1234567890"));
	Add(new cMenuEditIntItem(trREMOTETIMERS("Server port"), &setupTmp.serverPort, 0, 65535, trREMOTETIMERS("from svdrpservice")));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Map channels using"), &setupTmp.useChannelId, trREMOTETIMERS("channel number"), trREMOTETIMERS("channel ID")));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Hide mainmenu entry"), &setupTmp.hideMainMenuEntry));
	
	Add(new cOsdItem(trREMOTETIMERS("Defaults for new timers"), osUnknown, false));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Location"), &setupTmp.addToRemote, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
#ifdef REMOTEINSTANTVERSION
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Instant recordings"), &setupTmp.remoteInstant, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Pause recordings"), &setupTmp.remotePause, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
#endif
	Add(new PluginRemoteTimers::cMenuEditUserItem(trREMOTETIMERS("User ID"), &setupTmp.defaultUser, tr("Setup.Replay$Resume ID")));
	
	Add(new cOsdItem(cString::sprintf(trREMOTETIMERS("Settings for menu \"%s\""), tr("Schedule")), osUnknown, false));
#ifdef MAINMENUHOOKSVERSNUM
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Replace mainmenu"), &setupTmp.replaceSchedule));
#endif
	Add(new cMenuEditBoolItem(trREMOTETIMERS("List style"), &setupTmp.skinSchedule, tr("Plugin"), tr("Setup.OSD$Skin")));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Show progress bar"), &setupTmp.showProgressBar));
	for (int i = 0; i < EPGTIME_LENGTH; ++i)
		Add(new cMenuEditTimeItem(cString::sprintf("%s %d", *cString::sprintf(trREMOTETIMERS("What's on at %s?"), "..."), i + 1), &setupTmp.epgTime[i]));
	Add(new cMenuEditBoolItem(cString::sprintf(trREMOTETIMERS("Key binding of %s/%s"), tr("Key$Ok"), tr("Key$Blue")), &setupTmp.swapOkBlue, swapOkBlueFalse, swapOkBlueTrue));
	Add(new cMenuEditIntItem(trREMOTETIMERS("User ID filter"), &setupTmp.userFilterSchedule, -1, MAX_USER, tr("Setup.Replay$Resume ID")));

	Add(new cOsdItem(cString::sprintf(trREMOTETIMERS("Settings for menu \"%s\""), tr("Timers")), osUnknown, false));
#ifdef MAINMENUHOOKSVERSNUM
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Replace mainmenu"), &setupTmp.replaceTimers));
#endif
	Add(new cMenuEditBoolItem(trREMOTETIMERS("List style"), &setupTmp.skinTimers, tr("Plugin"), tr("Setup.OSD$Skin")));
	Add(new cMenuEditIntItem(trREMOTETIMERS("User ID filter"), &setupTmp.userFilterTimers, -1, MAX_USER, tr("Setup.Replay$Resume ID")));

	Add(new cOsdItem(cString::sprintf(trREMOTETIMERS("Settings for menu \"%s\""), tr("Recordings")), osUnknown, false));
#ifdef MAINMENUHOOKSVERSNUM
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Replace mainmenu"), &setupTmp.replaceRecordings));
#endif
	Add(new cMenuEditBoolItem(trREMOTETIMERS("List style"), &setupTmp.skinRecordings, tr("Plugin"), tr("Setup.OSD$Skin")));
	Add(new cMenuEditIntItem(trREMOTETIMERS("User ID filter"), &setupTmp.userFilterRecordings, -1, MAX_USER, tr("Setup.Replay$Resume ID")));
	Add(new cMenuEditIntItem(trREMOTETIMERS("Move recording bandwidth limit (Mbit/s)"), &setupTmp.moveBandwidth, 0, INT_MAX, trREMOTETIMERS("unlimited")));

	Add(new cOsdItem(trREMOTETIMERS("Remote recordings"), osUnknown, false));
	Add(new cMenuEditStrItem(trREMOTETIMERS("Mounted on subdirectory"), setupTmp.serverDir, sizeof(setupTmp.serverDir), tr(FileNameChars)));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Monitor update file"), &setupTmp.watchUpdate));
	SetCurrent(Get(current));
	Display();
}

eOSState cRemoteTimersMenuSetup::ProcessKey(eKeys Key) {
	eOSState state = cMenuSetupPage::ProcessKey(Key);
	if (Key == kOk && strcmp(lastServerDir, setupTmp.serverDir) != 0)
	{
		free((void *) lastServerDir);
		lastServerDir = strdup(setupTmp.serverDir);
	}
	return state;
}
