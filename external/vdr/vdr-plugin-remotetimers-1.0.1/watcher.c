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
	serverLastModifiedTime = 0;
#if APIVERSNUM > 20101
	clientUpdateFile = AddDirectory(cVideoDirectory::Name(), ".update");
#else
	clientUpdateFile = AddDirectory(VideoDirectory, ".update");
#endif
	clientLastDev = 0;
	inSubDir = false;
}

cUpdateWatcher::~cUpdateWatcher()
{
	condWait.Signal();
	Cancel(1);
}

void cUpdateWatcher::Initialize()
{
	inSubDir = RemoteTimersSetup.serverDir[0];
	if (inSubDir)
	{
		// server recordings in subdir: check mtime of subdir/.update
		char *tmpDir = strdup(RemoteTimersSetup.serverDir);
		tmpDir = ExchangeChars(tmpDir, true);
#if APIVERSNUM > 20101
		serverUpdateFile = AddDirectory(cVideoDirectory::Name(), AddDirectory(tmpDir, ".update"));
#else
		serverUpdateFile = AddDirectory(VideoDirectory, AddDirectory(tmpDir, ".update"));
#endif
	        serverLastModifiedTime = LastModifiedTime(serverUpdateFile);
		free(tmpDir);
	}
	else
	{
		// video dir on network share: check device id of .update
		serverUpdateFile = NULL;
		clientLastDev = DeviceId(clientUpdateFile);
	}
}

void cUpdateWatcher::Reconfigure()
{
	if (Running())
	{
		condWait.Signal();
		Cancel(1);
	}
	Initialize();
	if (RemoteTimersSetup.watchUpdate)
	{
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
	dsyslog("remotetimers update file watcher now monitoring '%s'", inSubDir ? *serverUpdateFile : *clientUpdateFile);

	while (!condWait.Wait(WATCHER_INTERVALL_MS) && Running())
	{
		if (inSubDir)
		{
			time_t t = LastModifiedTime(serverUpdateFile);
			if (t != serverLastModifiedTime)
			{
				// server .update changed. Update recordings
				Recordings.Update();
				serverLastModifiedTime = t;
			}
			if (t < LastModifiedTime(clientUpdateFile))
			{
				// client .update changed. Touch server .update
				TouchFile(serverUpdateFile);
				serverLastModifiedTime = LastModifiedTime(serverUpdateFile);
			}
		}
		else
		{
			dev_t d = DeviceId(clientUpdateFile);
			if (d != clientLastDev)
			{
				Recordings.Update();
				clientLastDev = d;
			}
		}
	}
}
