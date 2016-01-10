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
#include "PlutoUtils/ProcessUtils.h"

#include <string>
#include <iostream>
using namespace std;

#ifndef WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif

int main(int argc, char * argv[])
{
	string sOutput, sStdErr;
	const char * args[] = { "/usr/pluto/bin/Network_DisplaySettings.sh", "--orbiter", NULL };
	ProcessUtils::GetCommandOutput(args[0], &args[0], sOutput, sStdErr);
#if 0
	const char * args_default[] = {"(unused)", "/bin/cat", "/proc/cpuinfo", NULL};
	const char ** args = args_default;

	if (argc > 1)
		args = argv;
	
	cout << "Executing " << args[1] << endl;
	string sOutput, sStdErr;
	if (ProcessUtils::GetCommandOutput(args[1], &args[1], sOutput, sStdErr) == 0)
	{
		cout << sOutput;
	}
	else
	{
		cout << "Execution failed" << endl;
	}
#endif
	return 0;
}
