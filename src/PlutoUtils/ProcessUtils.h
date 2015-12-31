/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file ProcessUtils.h
 Header file for the file utils namespace
 */
#ifndef PROCESSUTILS
#define PROCESSUTILS

#include <string>
#include <vector>

/**
@namespace ProcessUtils
For ???.
*/
namespace ProcessUtils
{
	using std::string;
	using std::vector;

	/** Spawn an application and store a reference to it in the g_mapIdentifierToPidData global map using the name given in strAppIdentifier
	 * NOTE: sCmdParams are tab delimited
	 */
	int SpawnApplication(string sCmdExecutable, string sCmdParams, string strAppIdentifier, void *attachedData = NULL, bool bLogOutput = true, bool bDetach = false);

	/** Kill an application that was started with SpawnApplication, using the App Identifier specified*/
	bool KillApplication(string sAppIdentifier, vector<void *> &associatedData);

	/** Function to be called from the signal handler when a child launched with SpawnApplication exited */
	bool ApplicationExited(int pid, string &associatedName, void *&associatedData, bool removeIt = true);

	/** Return if the specified application was started by SpawnApplication and is still in the map */
	bool ApplicationIsLaunchedByMe(string applicationName);

	/** Send a string to the standard input of an application launched by SpawnApplication */
	bool SendKeysToProcess(string sAppIdentifier,string sKeys);

	/** Kill all the applications we have spawned with SpawnApplication */
	void KillAllApplications();

	/** Execute a command with execv and return its stdout. Returns exit code */
	int GetCommandOutput(const char * path, const char *const args[], string & sOutput, string & sStdErr);

	/** Execute a daemon (paenguin?) - i.e. run it in the background and disown it */
	bool SpawnDaemon(const char * path, char * args[], bool bLogOutput = true);
	bool SpawnDaemon(string sPath, string sArguments, bool bLogOutput); // Using \t delimited arguments like SpawnApplication

	// This is used to get elapsed time in milliseconds for quick comparissons.
	// clock is no good because it's only CPU time, not actual time.  timespec
	// is cumbersome.  You must call ResetMsTime to 'start' the stop watch, and
	// then GetMsTime will report the elapsed time in milliseconds.  Because it's a
	// 32-bit integer, the values will roll over after 40 days, so ResetMsTime
	// must be called periodically.  The counter will start at the last 1 second break,
	// so it's not accurate for counting the milliseconds elapsed since Reset, only between
	// subsequent calls to GetMsTime
	extern unsigned long g_SecondsReset;
	void ResetMsTime();
	unsigned long GetMsTime();

	bool RunApplicationAndGetOutput(string sCommandLine, string& sOutput);
};

#endif // PROCESSUTILS
