/*
 * watcher.c: monitor server's .update file
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/tools.h>
#include <vdr/recording.h>
#include <vdr/videodir.h>
#include "setup.h"
#include "watcher.h"

#define WATCHER_INTERVALL_MS 10000

cUpdateWatcher* cUpdateWatcher::updateWatcher = NULL;

cUpdateWatcher* cUpdateWatcher::GetInstance()
{
	if (!updateWatcher)
		updateWatcher = new cUpdateWatcher();
	return updateWatcher;
}

void cUpdateWatcher::DeleteInstance()
{
	DELETENULL(updateWatcher);
}

cUpdateWatcher::cUpdateWatcher(): cThread("remotetimers update file watcher")
{
	serverUpdateFile = NULL;
	inSubDir = false;
}

cUpdateWatcher::~cUpdateWatcher()
{
	condWait.Signal();
	Cancel(1);
	free((void *) serverUpdateFile);
}

void cUpdateWatcher::Reconfigure()
{
	if (Running())
	{
		condWait.Signal();
		Cancel(1);
	}
	free((void *) serverUpdateFile);
	serverUpdateFile = NULL;
	if (RemoteTimersSetup.watchUpdate)
	{
		inSubDir = RemoteTimersSetup.serverDir[0];
		if (inSubDir)
		{
			char *tmpDir = strdup(RemoteTimersSetup.serverDir);
			tmpDir = ExchangeChars(tmpDir, true);
			serverUpdateFile = strdup(AddDirectory(VideoDirectory, AddDirectory(tmpDir, ".update")));
			free(tmpDir);
		}
		else
		{
			serverUpdateFile = strdup(AddDirectory(VideoDirectory, ".update"));
		}
		dsyslog("remotetimers update file watcher now monitoring '%s'", serverUpdateFile);
		Start();
	}
}

dev_t cUpdateWatcher::DeviceId(const char* FileName)
{
	struct stat st;
	if (stat(FileName, &st) == 0)
		return st.st_dev;
	return 0;
}

void cUpdateWatcher::Action()
{
	SetPriority(19);
	time_t tLast = inSubDir ? LastModifiedTime(serverUpdateFile) : 0;
	dev_t dLast = inSubDir ? 0 : DeviceId(serverUpdateFile);

	while (!condWait.Wait(WATCHER_INTERVALL_MS) && Running())
	{
		if (inSubDir)
		{
			time_t t = LastModifiedTime(serverUpdateFile);
			if (t != tLast)
			{
				Recordings.Update();
				tLast = t;
			}
		}
		else
		{
			dev_t d = DeviceId(serverUpdateFile);
			if (d != dLast)
			{
				Recordings.Update();
				dLast = d;
			}
		}
	}
}
