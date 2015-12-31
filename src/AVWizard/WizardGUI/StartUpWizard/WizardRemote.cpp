/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "WizardRemote.h"

#include "signal.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
using namespace std;

RemoteProcess::RemoteProcess()
{
	pid = 0;
	m_sCmd = "";
}

RemoteProcess::~RemoteProcess()
{
#ifndef WIN32
	if (pid > 0)
		kill(pid, SIGTERM);
#endif
}

void RemoteProcess::Start(string sCmd)
{
	cout << "RemoteProcess::Start: sCmd='" << sCmd << "'" << endl;
	if (pid > 0)
		throw "Attempted to start another process";
	if (sCmd.size() == 0)
		return; // don't start anything; remote support will be disabled

	m_sCmd = sCmd;

#ifndef WIN32
	pid = fork();
	switch (pid)
	{
		case 0: /* child */
			chdir("/usr/pluto/bin");
			system(m_sCmd.c_str()); // too lazy to split and execv :)
			_exit(0);
			break;

		case -1: /* error */
			throw "Failed to fork remote control process";
			break;

		default: /* parent */
			break;
	}
#endif
}
