/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "ProcessUtils.h"
#include "CommonIncludes.h"
#include "MultiThreadIncludes.h"
#include "DCE/Logger.h"

#include <vector>
#include <map>

#ifndef WINCE
#include <errno.h>
#include <signal.h>
#endif

#ifdef WIN32
	#include <pthread.h>
	#include <windows.h>
#else
	#include <fcntl.h>
	#include <sys/ipc.h>

#ifndef ANDROID
	#include <sys/sem.h>
    extern int errno;
#endif
	#include <unistd.h>
	#include <sys/wait.h>
	#include <errno.h>
	#include <sys/select.h>


#endif
using namespace std;
using namespace DCE;

#include "StringUtils.h"
#include "FileUtils.h"
#include "PlutoDefs.h"

namespace ProcessUtils
{
	using std::map;
	using std::vector;

	class PidData
	{
	public:
		void *m_pData;
		int in; // pipe to child's stdin
		bool bDetached; // should the process it to be treated as detached or not
	};

	typedef map<int, PidData *> MapPidToData;
	typedef map<string, MapPidToData> MapIdentifierToPidData;

	MapIdentifierToPidData g_mapIdentifierToPidData;
}

pluto_pthread_mutex_t g_ProcessUtilsMutex("process utils",true);

int ProcessUtils::SpawnApplication(string sCmdExecutable, string sCmdParams, string sAppIdentifier, void *attachedData, bool bLogOutput, bool bDetach)
{
    if ( sAppIdentifier == "" )
        sAppIdentifier = "not named";

    if (sCmdExecutable == "")
    {
		LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication() Received empty Executable and Parameters for '%s'", sAppIdentifier.c_str());
        return 0;
    }

#ifndef ANDROID
#ifndef WIN32
    //parse the args
    const int MaxArgs = 32;
    char * args[MaxArgs];

	string::size_type pos = 0;
    int i = 0;

	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication() sCMDExec %s parms: %s size: %d",
		sCmdExecutable.c_str(),sCmdParams.c_str(),(int) sCmdParams.size());

	// this looks to complicated but i don;t have time to make it cleaner :-( mtoader@gmail.com)
    args[0] = (char *) strdup(sCmdExecutable.c_str());
LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"dupped exec %s",args[0]);
	while ( pos!=string::npos && pos<sCmdParams.size() && i < MaxArgs - 1)
{
string s=StringUtils::Tokenize(sCmdParams,"\t",pos);
const char *ps=strdup(s.c_str());
LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"dupped arg %d %s",i,ps);

		args[++i] = strdup(s.c_str());
}

	args[++i] = 0;
	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication() Found %d arguments", i);

    for (int x = 0 ; x < i; x++)
		LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication() Argument %d: %s", x, args[x]);

	int in[2];
	pipe(in);

	struct sembuf sops;
	int semSpawnApplication = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
	semctl(semSpawnApplication, 0, SETVAL, 1);

	PLUTO_SAFETY_LOCK(sl,g_ProcessUtilsMutex);
    pid_t pid = fork();

    switch (pid)
    {
        case 0: //child
        {
			// setenv("DISPLAY", ":0", 1);
            //now, exec the process
            LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication(): Spawning");
			setpgrp();

			string sLogFile;
			if (bLogOutput)
			{
				sLogFile = string("") + "/var/log/pluto/Spawn_" + sAppIdentifier + "_" + StringUtils::itos(getpid()) + ".log";
			}
			else
			{
				sLogFile = "/dev/null";
			}
			int fd = open(sLogFile.c_str(), O_WRONLY | O_CREAT,S_IRWXU); // Creating a file needs access rights defined.
			dup2(fd, 1);
			dup2(fd, 2);
			close(fd);
			
			close(in[1]);
			dup2(in[0], 0);
			
			int ret;
			do
			{
				sops.sem_num = 0;
				sops.sem_op = 0;
				sops.sem_flg = 0;
				errno = 0;
				ret = semop(semSpawnApplication, &sops, 1);
			} while (ret == -1 && errno == EINTR);
			
            if (execvp(args[0], args) == -1)
			{
                _exit(99);
			}
        }

        case -1:
            LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication() Error starting %s, err: %s", sCmdExecutable.c_str(), strerror(errno));
            return 0;

		default:
			close(in[0]);
			for(int i=0;i<i;++i)
				free(args[i]);

			LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"Freed args");

			LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnApplication() adding this %d pid to the spawned list for %s", pid, sAppIdentifier.c_str());

			if ( g_mapIdentifierToPidData.find(sAppIdentifier) == g_mapIdentifierToPidData.end() )
                g_mapIdentifierToPidData[sAppIdentifier] = map<int, PidData *>();

			PidData *pPidData = new PidData();
			pPidData->m_pData = attachedData;
			pPidData->in = in[1];
			pPidData->bDetached = bDetach;
			g_mapIdentifierToPidData[sAppIdentifier][pid] = pPidData;

			int ret;
			do
			{
				sops.sem_num = 0;
				sops.sem_op = -1;
				sops.sem_flg = 0;
				errno = 0;
				ret = semop(semSpawnApplication, &sops, 1);
			} while (ret == -1 && errno == EINTR);
			semctl(semSpawnApplication, 0, IPC_RMID);
			
            return pid;
    }
#else
//TO BE IMPLEMENTED
/*
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
    CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
*/
	return 1;
#endif
#endif

}

bool ProcessUtils::KillApplication(string sAppIdentifier, vector<void *> &associatedData)
{
	PLUTO_SAFETY_LOCK(sl,g_ProcessUtilsMutex);

	vector<int> pidsArray;
	MapIdentifierToPidData::iterator element = g_mapIdentifierToPidData.find(sAppIdentifier);

	if (element == g_mapIdentifierToPidData.end())
    {
        LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::KillApplication() No application '%s' found. None killed", sAppIdentifier.c_str());
		return false;
    }

	MapPidToData &mapPidsToData = element->second;
	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::KillApplication(): Found %d '%s' applications.", mapPidsToData.size(), sAppIdentifier.c_str());

	associatedData.clear();
	MapPidToData::const_iterator itPidsToData = mapPidsToData.begin();
	while (itPidsToData != mapPidsToData.end())
	{
		if (! itPidsToData->second->bDetached)
		{
			pidsArray.push_back(itPidsToData->first);
			associatedData.push_back(itPidsToData->second->m_pData);
		}
		itPidsToData++;
	}

	g_mapIdentifierToPidData.erase(element);
	sl.Release();

	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::KillApplication(): Mutex unlocked Sending kill signal.");
	// we are doing the killing without the lock taken (since this killing might trigger another call into one of our functions which might cause a deadlock).
	vector<int>::const_iterator itPids = pidsArray.begin();
	while ( itPids != pidsArray.end() )
	{
#ifndef WIN32
		kill(-*itPids, SIGTERM);
#endif
		itPids++;
	}

	return true;
}

bool ProcessUtils::ApplicationExited(int pid, string &associatedName, void *&associatedData, bool removeIt)
{
	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::ApplicationExited() pid exited: %d", pid);

	PLUTO_SAFETY_LOCK(sl,g_ProcessUtilsMutex);

	MapIdentifierToPidData::iterator applicationElement = g_mapIdentifierToPidData.begin();
	while ( applicationElement != g_mapIdentifierToPidData.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::ApplicationExited() Looking at: %s", applicationElement->first.c_str());

		MapPidToData &mapPidsToData = applicationElement->second;
		MapPidToData::iterator itPidsToData = mapPidsToData.begin();
		while ( itPidsToData != mapPidsToData.end() && itPidsToData->first != pid )
		{
			LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::ApplicationExited() checking pid %d", itPidsToData->first);
			itPidsToData++;
		}

		if ( itPidsToData != mapPidsToData.end() )
		{
			associatedName = applicationElement->first;
			PidData *pPidData = itPidsToData->second;
			associatedData = pPidData->m_pData;

			if ( removeIt )
				mapPidsToData.erase(itPidsToData);

			return true;
		}
		applicationElement++;
	}

	return false;
}

bool ProcessUtils::ApplicationIsLaunchedByMe(string applicationName)
{
	PLUTO_SAFETY_LOCK(sl,g_ProcessUtilsMutex);

	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ApplicationIsLaunchedByMe %s size: %d",applicationName.c_str(),(int) g_mapIdentifierToPidData.size());
	if ( g_mapIdentifierToPidData.find(applicationName) == g_mapIdentifierToPidData.end() )
	{
		return false;
	}

	return true;
}

bool ProcessUtils::SendKeysToProcess(string sAppIdentifier,string sKeys)
{
#ifndef WINCE
	PLUTO_SAFETY_LOCK(sl,g_ProcessUtilsMutex);

	vector<int> pidsArray;
	MapIdentifierToPidData::iterator element = g_mapIdentifierToPidData.find(sAppIdentifier);

	if (element == g_mapIdentifierToPidData.end())
    {
        LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::KillApplication() No application '%s' found. None to send keys to", sAppIdentifier.c_str());
		return false;
    }

	MapPidToData &mapPidsToData = element->second;
	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SendKeysToProcess(): Found %d '%s' applications, sending %s.", mapPidsToData.size(), sAppIdentifier.c_str(), sKeys.c_str());

	MapPidToData::const_iterator itPidsToData = mapPidsToData.begin();
	while ( itPidsToData != mapPidsToData.end() )
	{
		PidData *pPidData = itPidsToData->second;
		write(pPidData->in, sKeys.c_str(), (unsigned int)(sKeys.length()));
		itPidsToData++;
	}
#endif

	return true;
}

void ProcessUtils::KillAllApplications()
{
	list<string> listOpenProcesses;
	PLUTO_SAFETY_LOCK(sl,g_ProcessUtilsMutex);
	for(MapIdentifierToPidData::iterator it=g_mapIdentifierToPidData.begin();it!=g_mapIdentifierToPidData.end();++it)
		listOpenProcesses.push_back( it->first );
	sl.Release();
	for(list<string>::iterator itx=listOpenProcesses.begin();itx!=listOpenProcesses.end();++itx)
	{
		vector<void *> messagesToSend;
		KillApplication(*itx, messagesToSend);
	}
}

int ProcessUtils::GetCommandOutput(const char * path, const char *const args[], string & sOutput, string & sStdErr)
{
	string sArgs;
	for(int i=0;;++i)
	{
		const char *ptr = args[i];
		if( !ptr )
			break;
		sArgs += string("\t") + ptr;
	}

	LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::GetCommandOutput %s%s", path,sArgs.c_str());

#ifdef WIN32
	return 0;
#else
	int pid;
	int status = 253;
	int output[2];
	int errput[2];

	pipe(output);
	pipe(errput);

	struct sigaction act, oldact;
	act.sa_handler = SIG_DFL;
	act.sa_flags = SA_NOCLDSTOP;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCHLD, &act, &oldact);

	switch (pid = fork())
	{
		case 0: /* child */
			// stdout pipe
			close(output[0]);
			dup2(output[1], 1);
			close(output[1]);

			// stderr pipe
			close (errput[0]);
			dup2(errput[1], 2);
			close(errput[1]);
			
			execv(path, (char * const *) args);
			_exit(254);
			/* Rationale for _exit instead of exit:
			 * exit calls atexit-registered functions and X registers some stuff
			 * this causes the Orbiter to lose the connection to X
			 * but this child doesn't register any atexit functions, nor does it make any output if exec fails
			 */
			break;
		case -1: /* error */
			return false;
			break;
		default: /* parent */
			close(output[1]);
			close(errput[1]);
			
			fd_set fdset;

			int maxfd = max(output[0], errput[0]) + 1;
			
			char buffer[4096];
			memset(buffer, 0, sizeof(buffer));
			
			while (waitpid(pid, &status, WNOHANG) == 0)
			{
				FD_ZERO(&fdset);
				FD_SET(output[0], &fdset);
				FD_SET(errput[0], &fdset);

				int bytes = 0;
				
				select(maxfd, &fdset, NULL, NULL, NULL);

				if (FD_ISSET(output[0], &fdset))
				{
					bytes = read(output[0], buffer, sizeof(buffer) - 1);
					sOutput += buffer;
					memset(buffer, 0, sizeof(buffer));
				}
				if (FD_ISSET(errput[0], &fdset))
				{
					bytes = read(errput[0], buffer, sizeof(buffer) - 1);
					sStdErr += buffer;
					memset(buffer, 0, sizeof(buffer));
				}

				// if one of the streams returned 0 bytes, or read error,
				// the child application has probably terminated
				// (otherwise, it will get a 'broken pipe' signal and terminate anyway)
				// so we finish the loop and return the output to the user
			}
			status = WEXITSTATUS(status);
			
			// catch unflushed buffers
			// 1. stdout
			memset(buffer, 0, sizeof(buffer));
			read(output[0], buffer, sizeof(buffer) - 1);
			sOutput += buffer;
			// 2. stderr
			memset(buffer, 0, sizeof(buffer));
			read(errput[0], buffer, sizeof(buffer) - 1);
			sStdErr += buffer;
			
			close(output[0]);
			close(errput[0]);
	}

	sigaction(SIGCHLD, &oldact, NULL);

	return status;
#endif
}

// SpawnPaenguin
bool ProcessUtils::SpawnDaemon(string sPath, string sArguments, bool bLogOutput)
{
	vector<string> vectArguments;
	StringUtils::Tokenize(sArguments,"\t",vectArguments);
	size_t iSize = vectArguments.size();
	char **pArgs = new char*[iSize + 2]; // enough to include 1 path, n arguments and a NULL pointer at the end (space for n+2 elements numbered 0..n+1)
	
	// args[0]
	pArgs[0] = (char *) sPath.c_str();

	// args[1..n], n == vectArguments.size() == iSize
	for (size_t i = 0; i < iSize; i++)
		pArgs[i + 1] = (char *) vectArguments[i].c_str();
	pArgs[iSize + 1] = NULL; // NULL pointer in the last element, args[n+1]

	bool bResult = SpawnDaemon(pArgs[0], pArgs, bLogOutput);
	delete [] pArgs; // Not PLUTO_SAFE_DELETE_ARRAY_OF_ARRAYS because the strings are c_str and not allocated on the heap
	return bResult;
}

// SpawnPaenguin
bool ProcessUtils::SpawnDaemon(const char * path, char * args[], bool bLogOutput)
{
#ifndef WIN32
	int pid;

	{
		string sArgs;
		for (int i = 0; args[i] != NULL; i++)
			sArgs += string("") + args[i] + ",";
		LoggerWrapper::GetInstance()->Write(LV_PROCESSUTILS,"ProcessUtils::SpawnDaemon: path=%s; args=%s", path, sArgs.c_str());
	}

	switch (pid = fork())
	{
		case 0: /* child */
			// separate us from the original parent
			switch (pid = fork())
			{
				case 0: /* daemon */
				{ // start of variable scope limitation
					setpgrp();
					string sAppIdentifier = FileUtils::FilenameWithoutPath(path);
					string sLogFile;
					if (bLogOutput)
					{
						sLogFile = string("") + "/var/log/pluto/SpawnDaemon_" + sAppIdentifier + "_" + StringUtils::itos(getpid()) + ".log";
					}
					else
					{
						sLogFile = "/dev/null";
					}

					// NEVER dup(open(...))!!! You lose the filedescriptor from open and that may cause problems in the child
					
					// redirect process output to log file, as selected above
					int fd = open(sLogFile.c_str(), O_WRONLY | O_CREAT,S_IRWXU); // Creating a file needs access rights defined.
					dup2(fd, 1);
					dup2(fd, 2);
					close(fd);

					// get process input from /dev/null, to properly create a daemon
					fd = open("/dev/null", O_RDONLY);
					dup2(fd, 0);
					close(fd);

					execv(path, args);
					_exit(254);
				} // end of variable scope limitation
					break;
				case -1: /* failed to spawn daemon */
					_exit(254);
					break;
				default: /* daemon's parent */
					_exit(0); // daemon successfully started; what it does from here is none of our business
			}
			_exit(254); // if fork() fails

			break;
		case -1: /* error */
			return false;
			break;
		default: /* parent */
			int status;

			waitpid(pid, &status, 0); // wait for daemon spawner to start
			status = WEXITSTATUS(status);

			if (status == 254)
				return false;
			break;
	}
#endif
	return true;
}

unsigned long ProcessUtils::g_SecondsReset=0;

void ProcessUtils::ResetMsTime()
{
	timespec ts;
	gettimeofday( &ts, NULL );
	g_SecondsReset = ts.tv_sec;
}

unsigned long ProcessUtils::GetMsTime()
{
	timespec ts;
	gettimeofday( &ts, NULL );
	return (ts.tv_sec-g_SecondsReset)*1000 + (ts.tv_nsec/1000000);
}

bool ProcessUtils::RunApplicationAndGetOutput(string sCommandLine, string& sOutput)
{
	bool bResult = false;

#ifndef WINCE

	char *pFilename = tempnam("/tmp", "pluto-temp-");

	string sOutputFilename = NULL != pFilename ? pFilename : "pluto_temp";
	sOutputFilename += ".out";

	if(NULL != pFilename)
	{
		free(pFilename);
    	pFilename = NULL;
	}

	sCommandLine = sCommandLine + " > " + sOutputFilename;

	if(-1 != system(sCommandLine.c_str()))
		bResult = FileUtils::ReadTextFile(sOutputFilename, sOutput);

	if(FileUtils::FileExists(sOutputFilename))
		FileUtils::DelFile(sOutputFilename);

#endif

	return bResult;
}

