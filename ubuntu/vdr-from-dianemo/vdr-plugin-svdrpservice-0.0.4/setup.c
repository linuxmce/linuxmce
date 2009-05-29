/*
 * setup.c: Svdrpservice setup
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/menuitems.h>
#include "i18n.h"
#include "setup.h"

cSvdrpServiceSetup SvdrpServiceSetup;

const char* cSvdrpServiceSetup::opt_serverIp = NULL;
const char* cSvdrpServiceSetup::opt_serverPort = NULL;

cSvdrpServiceSetup::cSvdrpServiceSetup() {
	serverIp[0] = 0;
	serverPort = 2001;
	connectTimeout = 2;
	readTimeout = 5;
}

cSvdrpServiceSetup& cSvdrpServiceSetup::operator=(const cSvdrpServiceSetup &Setup) {
	strn0cpy(serverIp, Setup.serverIp, sizeof(serverIp));
	serverPort = Setup.serverPort;
	connectTimeout = Setup.connectTimeout;
	readTimeout = Setup.readTimeout;
	return *this;
}

bool cSvdrpServiceSetup::Parse(const char *Name, const char *Value) {
	if (!strcasecmp(Name, "ServerIp"))
		strn0cpy(serverIp, opt_serverIp ? opt_serverIp : Value, sizeof(serverIp));
	else if (!strcasecmp(Name, "ServerPort"))
		serverPort = opt_serverIp ? (opt_serverPort ? atoi(opt_serverPort) : 2001) :  atoi(Value);
	else if (!strcasecmp(Name, "ConnectTimeout"))
		connectTimeout = atoi(Value);
	else if (!strcasecmp(Name, "ReadTimeout"))
		readTimeout = atoi(Value);
	else
		return false;
	return true;
}

void cSvdrpServiceMenuSetup::Store() {
	SetupStore("ServerIp", setupTmp.serverIp);
	SetupStore("ServerPort", setupTmp.serverPort);
	SetupStore("ConnectTimeout", setupTmp.connectTimeout);
	SetupStore("ReadTimeout", setupTmp.readTimeout);
	SvdrpServiceSetup = setupTmp;
}

cSvdrpServiceMenuSetup::cSvdrpServiceMenuSetup() {
	setupTmp = SvdrpServiceSetup;
	Add(new cMenuEditStrItem(tr("Default server IP"), setupTmp.serverIp, 15, ".1234567890"));
	Add(new cMenuEditIntItem(tr("Default server port"), &setupTmp.serverPort, 1, 65535));
	if (cSvdrpServiceSetup::opt_serverIp)
	{
		First()->SetSelectable(false);
		Next(First())->SetSelectable(false);
	}
	Add(new cMenuEditIntItem(tr("Connect timeout (s)"), &setupTmp.connectTimeout, 1));
	Add(new cMenuEditIntItem(tr("Command timeout (s)"), &setupTmp.readTimeout, 1));
}

cSvdrpServiceMenuSetup::~cSvdrpServiceMenuSetup() {
}
