/*
 * remotetimers.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
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
	serverPort = 2001;
	swapOkBlue = 0;
	showProgressBar = 0;
	userFilterSchedule = 0;
	userFilterTimers = 0;
	userFilterRecordings = 0;
	defaultUser = 0;
	addToRemote = 1;
	remotePause = 0;
	remoteInstant = 0;
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
	swapOkBlue = Setup.swapOkBlue;
	showProgressBar = Setup.showProgressBar;
	userFilterSchedule = Setup.userFilterSchedule;
	userFilterTimers = Setup.userFilterTimers;
	userFilterRecordings = Setup.userFilterRecordings;
	defaultUser = Setup.defaultUser;
	addToRemote = Setup.addToRemote;
	remotePause = Setup.remotePause;
	remoteInstant = Setup.remoteInstant;
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
	else if (!strcasecmp(Name, "SwapOkBlue"))
		swapOkBlue = atoi(Value);
	else if (!strcasecmp(Name, "ShowProgressBar"))
		showProgressBar = atoi(Value);
	else if (!strcasecmp(Name, "UserFilterSchedule"))
		userFilterSchedule = atoi(Value);
	else if (!strcasecmp(Name, "UserFilterTimers"))
		userFilterTimers = atoi(Value);
	else if (!strcasecmp(Name, "UserFilterRecordings"))
		userFilterRecordings = atoi(Value);
	else if (!strcasecmp(Name, "DefaultUser"))
		defaultUser = PluginRemoteTimers::cMenuEditUserItem::Parse(Value);
	else if (!strcasecmp(Name, "AddToRemote"))
		addToRemote = atoi(Value);
	else if (!strcasecmp(Name, "RemotePause"))
		remotePause = atoi(Value);
	else if (!strcasecmp(Name, "RemoteInstant"))
		remoteInstant = atoi(Value);
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
	SetupStore("SwapOkBlue", setupTmp.swapOkBlue);
	SetupStore("ShowProgressBar", setupTmp.showProgressBar);
	SetupStore("UserFilterSchedule", setupTmp.userFilterSchedule);
	SetupStore("UserFilterTimers", setupTmp.userFilterTimers);
	SetupStore("UserFilterRecordings", setupTmp.userFilterRecordings);
	SetupStore("DefaultUser", PluginRemoteTimers::cMenuEditUserItem::ToString(setupTmp.defaultUser));
	SetupStore("AddToRemote", setupTmp.addToRemote);
	SetupStore("RemotePause", setupTmp.remotePause);
	SetupStore("RemoteInstant", setupTmp.remoteInstant);
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
	Add(new cMenuEditIntItem(trREMOTETIMERS("Server port"), &setupTmp.serverPort, 0, 65535));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Hide mainmenu entry"), &setupTmp.hideMainMenuEntry));
#ifdef MAINMENUHOOKSVERSNUM
	Add(new cMenuEditBoolItem(cString::sprintf(trREMOTETIMERS("Replace mainmenu \"%s\""), tr("Schedule")), &setupTmp.replaceSchedule));
	Add(new cMenuEditBoolItem(cString::sprintf(trREMOTETIMERS("Replace mainmenu \"%s\""), tr("Timers")), &setupTmp.replaceTimers));
	Add(new cMenuEditBoolItem(cString::sprintf(trREMOTETIMERS("Replace mainmenu \"%s\""), tr("Recordings")), &setupTmp.replaceRecordings));
#endif
	
	Add(new cOsdItem(trREMOTETIMERS("Defaults for new timers"), osUnknown, false));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Location"), &setupTmp.addToRemote, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
#ifdef REMOTEINSTANTVERSION
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Instant recordings"), &setupTmp.remoteInstant, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Pause recordings"), &setupTmp.remotePause, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
#endif
	Add(new PluginRemoteTimers::cMenuEditUserItem(trREMOTETIMERS("User ID"), &setupTmp.defaultUser, tr("Setup.Replay$Resume ID")));
	
	Add(new cOsdItem(trREMOTETIMERS("Settings for menu"), osUnknown, false));
	Add(new cMenuEditBoolItem(cString::sprintf(trREMOTETIMERS("Binding of %s/%s in \"What's on\" menus"), tr("Key$Ok"), tr("Key$Blue")), &setupTmp.swapOkBlue, swapOkBlueFalse, swapOkBlueTrue));
	Add(new cMenuEditBoolItem(cString::sprintf(trREMOTETIMERS("Show progress bar in menu \"%s\""), tr("Schedule")), &setupTmp.showProgressBar));
	Add(new cMenuEditIntItem(cString::sprintf(trREMOTETIMERS("User ID filter in menu \"%s\""), tr("Schedule")), &setupTmp.userFilterSchedule, -1, MAX_USER, tr("Setup.Replay$Resume ID")));
	Add(new cMenuEditIntItem(cString::sprintf(trREMOTETIMERS("User ID filter in menu \"%s\""), tr("Timers")), &setupTmp.userFilterTimers, -1, MAX_USER, tr("Setup.Replay$Resume ID")));
	Add(new cMenuEditIntItem(cString::sprintf(trREMOTETIMERS("User ID filter in menu \"%s\""), tr("Recordings")), &setupTmp.userFilterRecordings, -1, MAX_USER, tr("Setup.Replay$Resume ID")));

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
