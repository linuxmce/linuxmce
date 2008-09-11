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
#include "../include/version.h"

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"

#include "FileNotifier.h"
#include <sys/types.h>
#include <grp.h>

#include <list>
#include <string>

using namespace std;
using namespace DCE;



struct MonitoredDir
{
	int m_iWD;
	string m_sPath;
};

typedef list<MonitoredDir> listOfMonitoredDirs;
typedef list<string> listOfStrings;

void SetAccessRights(const string & sPath)
{
	static group * grent = getgrnam("www-data");
	struct stat st;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting access rights on '%s'", sPath.c_str());
	
	if (grent != NULL)
	{
		chown(sPath.c_str(), (uid_t)-1, grent->gr_gid);
	}

	stat(sPath.c_str(), &st);
	if (S_ISDIR(st.st_mode))
	{
		// it's a directory
		chmod(sPath.c_str(), 0770);
	}
	else
	{
		// it's a file
		chmod(sPath.c_str(), 0660);
	}
}

bool DirIsWatched(listOfMonitoredDirs & listMonitoredDirs, const string & sDirectory)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_sPath == sDirectory)
		{
			listMonitoredDirs.erase(it_lOMD);
			return true;
		}
	}

	return false;
}

listOfMonitoredDirs::iterator FindWatchedDir(listOfMonitoredDirs & listMonitoredDirs, int wd)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_iWD == wd)
		{
			return it_lOMD;
		}
	}
	return listMonitoredDirs.end();
}

void UnwatchDir(listOfMonitoredDirs & listMonitoredDirs, const string & sDirectory)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_sPath == sDirectory)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unwatching directory '%s'", it_lOMD->m_sPath.c_str());
			listMonitoredDirs.erase(it_lOMD);
			break;
		}
	}
}

void UnwatchDir(listOfMonitoredDirs & listMonitoredDirs, int wd)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_iWD == wd)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unwatching directory '%s'", it_lOMD->m_sPath.c_str());
			listMonitoredDirs.erase(it_lOMD);
			break;
		}
	}
}

void WatchDir(listOfMonitoredDirs & listMonitoredDirs, const string & sDirectory, inotify & obj_Inotify)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Watching directory '%s'", sDirectory.c_str());
	SetAccessRights(sDirectory);

	// Add directory watch first
	MonitoredDir struct_MonitoredDir;
	struct_MonitoredDir.m_sPath = sDirectory;
	struct_MonitoredDir.m_iWD = obj_Inotify.watch(sDirectory, IN_CREATE | IN_IGNORED);
	
	listMonitoredDirs.push_back(struct_MonitoredDir);

	// Find files in this directory
	listOfStrings listFiles;
	FileUtils::FindFiles(listFiles, sDirectory, "*.*", false, true);
	for (listOfStrings::iterator it_lOS = listFiles.begin(); it_lOS != listFiles.end(); it_lOS++)
	{
		SetAccessRights(*it_lOS);
	}

	// Find directories in this directory
	listOfStrings listDirectories;
	FileUtils::FindDirectories(listDirectories, sDirectory, false, true, 0, "", NULL);

	for (listOfStrings::iterator it_lOS = listDirectories.begin(); it_lOS != listDirectories.end(); it_lOS++)
	{
		WatchDir(listMonitoredDirs, *it_lOS, obj_Inotify);
	}
}

void OperateFile(listOfMonitoredDirs & listMonitoredDirs, inotify & obj_Inotify, cpp_inotify_event & event)
{
	struct stat st;

	listOfMonitoredDirs::iterator it_lOMD = FindWatchedDir(listMonitoredDirs, event.wd);
	string sPath;
	if (it_lOMD == listMonitoredDirs.end())
	{
		return;
	}
	sPath = it_lOMD->m_sPath + "/" + event.name;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Operating on '%s'", sPath.c_str());
	if (event.mask & IN_IGNORED)
	{
		UnwatchDir(listMonitoredDirs, event.wd);
		return;
	}

	SetAccessRights(sPath);

	stat(sPath.c_str(), &st);
	if (S_ISDIR(st.st_mode))
	{
		WatchDir(listMonitoredDirs, sPath, obj_Inotify);
	}
}

int main(int argc, char *argv[])
{
	// avoid warnings
	(void)argc;
	(void)argv;

	string sDirectory = "/var/spool/asterisk/voicemail";
	inotify obj_Inotify;

	listOfMonitoredDirs listMonitoredDirs;
	listOfStrings listDirectories;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Running Voicemail Monitor");

	// Find directories in starting directory
	// Recursion is done by us, not by the function
	FileUtils::FindDirectories(listDirectories, sDirectory, false, true);

	for (listOfStrings::iterator it_lOS = listDirectories.begin(); it_lOS != listDirectories.end(); it_lOS++)
	{
		WatchDir(listMonitoredDirs, *it_lOS, obj_Inotify);
	}

	while (true)
	{
		cpp_inotify_event event = obj_Inotify.get_event();

		OperateFile(listMonitoredDirs, obj_Inotify, event);
	}

	return 0;
}
