/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __FILE_NOTIFIER_H__
#define __FILE_NOTIFIER_H__
//-----------------------------------------------------------------------------------------------------
#include <pthread.h>
#include <memory>
#include <list>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
#include "PlutoUtils/MultiThreadIncludes.h"
#include "inotify/inotify_class.h"
//-----------------------------------------------------------------------------------------------------
typedef void (*FileNotifierCallback)(list<string> &listFiles);
//-----------------------------------------------------------------------------------------------------
//   Class FileNotifier (wrapper for inotifier):
//     - cross-platform
//     - recursive
//     - event driven (using callbacks registered by the user class)
//-----------------------------------------------------------------------------------------------------
class FileNotifier 
{
private:
    FileNotifierCallback m_pfOnCreate;
    FileNotifierCallback m_pfOnDelete;

    bool m_bCallbacksRegistered;
    pthread_t m_INotifyWorkerThreadID;
	pthread_t m_BackgroundWorkerThreadID;
    map<int, string> m_mapWatchedFiles;

public:
    FileNotifier(class Database_pluto_media *pDatabase_pluto_media = NULL); 
    ~FileNotifier(void);

    void RegisterCallbacks(FileNotifierCallback pfOnCreate, FileNotifierCallback pfOnDelete);
    void Watch(string sDirectory);
	void Run();

    bool m_bCancelThread;
    inotify m_inotify;
	class Database_pluto_media *m_pDatabase_pluto_media;
	
	int m_wdRootFolder;
	string m_sRootFolder;
	void ResetWatches();
	static string GetLastModifiedDate(string sFolder);

    pluto_pthread_mutex_t m_WatchedFilesMutex;
    string m_mapWatchedFiles_Find(int wd)	
    { 
        map<int, string>::iterator it = m_mapWatchedFiles.find(wd); 
        return it == m_mapWatchedFiles.end() ? "" : (*it).second; 
    }

	void FireOnCreate(list<string> &listFiles);
	void FireOnDelete(list<string> &listFiles);
};
//-----------------------------------------------------------------------------------------------------
#endif //__FILE_NOTIFIER_H__
