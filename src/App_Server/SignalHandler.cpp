/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 * This file compiles only under Linux. Under Windows it gives an error if included, and it's not useful there anyway
 */
#ifndef __linux__
#error "This file is meant to be compiled under Linux"
#endif /*ifndef __linux__*/

#include "SignalHandler.h"

#include "App_Server.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#include <iostream>

using namespace DCE;
using namespace std;

extern int errno;

static const key_t IPCKEY_APPSERVER = 0xA99516; // h4x0r to english: AppSig
static const struct sembuf c_sops_inc = { sem_num: 0, sem_op: 1, sem_flg: 0, };
static const struct sembuf c_sops_dec = { sem_num: 0, sem_op: -1, sem_flg: 0, };

static App_Server *g_pAppServer = NULL;
static bool g_bQuit = false;
static pthread_t g_SignalHandler_Thread = 0;
static int g_iSemID = -1;

void sh(int i) /* signal handler */
{
	struct sembuf sops;
	int ret;

	// NOTE: cout used instead of PlutoLogger because a Logger->Write can be interrupted by a signal and we'd deadlock
	
	cout << "Linux signal handler: Started" << endl;
	do
	{
		sops = c_sops_inc;
		errno = 0;
		ret = semop(g_iSemID, &sops, 1);
	} while (ret == -1 && errno == EINTR);
	cout << "Linux signal handler: Exited" << endl;
}

void SignalHandler_Action()
{
	if ( g_pAppServer && g_pAppServer->m_bQuit_get() )
		return;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SignalHandler_Action: Started");
	PLUTO_SAFETY_LOCK(ap,g_pAppServer->m_AppMutex);

	int status = 0;
	pid_t pid = 0;

	// wait for all children that exited, not just one of them
	// why: SIGCHLD is a POSIX standard signal, thus if a few children die very fast, we don't get one SIGCHLD per child
	//      we only get the first one, and if a child dies while we're in the handler, only one instance is queued
	//      not doing this, we risk leaving zombies behind in certain high-load situations
	// thanks go to the RLUG mailing list for the pointer
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		int ExitStatus = WEXITSTATUS(status);
		int Signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
		
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SIGCHLD -- PID %d; Return code: %d; Signal: %d", pid, ExitStatus, Signal);

		if (g_pAppServer)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "SignalHandler_Action: Sending 'Application Exited' message to App_Server");
			// Send ourselves a message that calls the ApplicationExited function
			// We don't call it directly to avoid a deadlock if the SIGCHLD signal was received while in ProcessUtils::SpawnApplication
			DCE::CMD_Application_Exited CMD_Application_Exited(g_pAppServer->m_dwPK_Device, g_pAppServer->m_dwPK_Device, pid, ExitStatus);
			g_pAppServer->SendCommand(CMD_Application_Exited);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SignalHandler_Action: g_pAppServer is NULL; can't sent 'Application Exited' message");
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SignalHandler_Action: Exited");
}

void * SignalHandler_Thread(void * Args)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SignalHandler_Thread: Started");
	g_pAppServer = (App_Server *) Args;
	signal(SIGCHLD, sh); /* install signal handler */

	struct sembuf sops;
	int ret;

	while (! g_bQuit)
	{
		do
		{
			sops = c_sops_dec;
			errno = 0;
			ret = semop(g_iSemID, &sops, 1);
		} while (ret == -1 && errno == EINTR);
		if (ret == 0)
			SignalHandler_Action();
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SignalHandler_Thread: Exited");

	return NULL;
}

void SignalHandler_Start(App_Server *pApp_Server)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting signal handler thread");
	g_iSemID = semget(IPCKEY_APPSERVER, 1, IPC_CREAT | 0600);

	if (g_iSemID == -1)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to initialize semaphore. Signal handler not enabled. AppServer will leave lots of zombies behind.");
		return;
	}
	semctl(g_iSemID, 0, SETVAL, (int) 0);
	
	pthread_create(&g_SignalHandler_Thread, NULL, SignalHandler_Thread, (void *) pApp_Server);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Started signal handler thread");
}

void SignalHandler_Stop()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping signal handler thread");
	signal(SIGCHLD, SIG_DFL);
	g_bQuit = true;

	if (g_SignalHandler_Thread != 0)
	{
		semctl(g_iSemID, 0, IPC_RMID);

		pthread_join(g_SignalHandler_Thread, NULL);
		g_SignalHandler_Thread = 0;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopped signal handler thread");
}
