/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef WIN32
	#include <sys/ioctl.h>
	#include <errno.h>
	#include "inotify/inotify.h"
	#include "inotify/inotify-syscalls.h"
#else
	#include <sys/types.h> 
	#include <sys/stat.h>
#endif

#include "FileNotifier.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include "PlutoUtils/Other.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_File.h"
using namespace DCE;
//-----------------------------------------------------------------------------------------------------
#ifdef WIN32
	#define IN_ALL_EVENTS 0xFFFFFFFF
	#define IN_CREATE		0x00000100
	#define IN_MOVED_TO		0x00000080
	#define IN_MOVED_FROM	0x00000040
	#define IN_DELETE		0x00000200
	#define IN_DELETE_SELF	0x00000400
	#define IN_ISDIR		0x40000000
#endif
//-----------------------------------------------------------------------------------------------------
void *INotifyWorkerThread(void *);
void *BackgroundWorkerThread(void *);
//-----------------------------------------------------------------------------------------------------
bool operator > (string s1, string s2)
{
	return s1.compare(0, s1.size(),  s2) > 0;
}
//-----------------------------------------------------------------------------------------------------
FileNotifier::FileNotifier(class Database_pluto_media *pDatabase_pluto_media) : 
	m_WatchedFilesMutex("watched files")
{
    m_bCallbacksRegistered = false;
    m_bCancelThread = false;

    m_pfOnCreate = NULL;
	m_pfOnDelete = NULL;

	m_wdRootFolder = 0;
    m_WatchedFilesMutex.Init(NULL);

	m_pDatabase_pluto_media = pDatabase_pluto_media;
}
//-----------------------------------------------------------------------------------------------------
FileNotifier::~FileNotifier(void)
{
    m_bCancelThread = true;
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::Watch(string sDirectory)
{
#ifdef WIN32	
	if(!m_wdRootFolder)
	{
		m_wdRootFolder = 1;
		m_sRootFolder = sDirectory;
	}
#endif

	//FIXME: check and do not register against external shares (nfs)
	// currently inotify watches are put on nfs files, and they do not get notifications
	// as nfs shares do not support inotify behaviour
	// only changes made from the machine that creates the watch will be notified
	// changes made to the share from other systems will not provide notification

	list<string> listFilesOnDisk;
	FileUtils::FindDirectories(listFilesOnDisk,sDirectory,true,false,0,sDirectory + "/");  
	listFilesOnDisk.push_back(sDirectory);

	PLUTO_SAFETY_LOCK(wfm, m_WatchedFilesMutex);
	for(list<string>::iterator it = listFilesOnDisk.begin(); it != listFilesOnDisk.end(); it++)
	{
		string sItem = *it;

		try
		{
			int wd = m_inotify.watch(*it, IN_ALL_EVENTS);

			if(!m_wdRootFolder)
			{
				m_wdRootFolder = wd; //root folder
				m_sRootFolder = sDirectory;
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding file to watch map: filename %s", sItem.c_str());
			m_mapWatchedFiles[wd] = *it; 
		}
		catch(...)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to add notifier for file %s", sItem.c_str());
		}
    }
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::ResetWatches()
{
	m_sRootFolder = "";
	m_wdRootFolder = 0;
	m_mapWatchedFiles.clear();
}
//-----------------------------------------------------------------------------------------------------
void *BackgroundWorkerThread(void *p)
{
	FileNotifier *pFileNotifier = (FileNotifier *)p;

	while(!pFileNotifier->m_bCancelThread)
	{
		if(!pFileNotifier->m_wdRootFolder)
		{
			//we don't know yet what is the root folder
			Sleep(1000);
			continue;
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Background thread waking up...");

		string sRootFolder = pFileNotifier->m_sRootFolder;
		sRootFolder = FileUtils::ExcludeTrailingSlash(sRootFolder);
		
		list<string> listFiles;
		listFiles.push_back(sRootFolder);
		pFileNotifier->FireOnCreate(listFiles);

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Background thread going to sleep...");
		Sleep(120 * 1000); //2 minutes
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------
void *INotifyWorkerThread(void *p)
{
    FileNotifier *pFileNotifier = (FileNotifier *)p;

    while(!pFileNotifier->m_bCancelThread)
    {
		while(!pFileNotifier->m_inotify.pending_events()) 
			Sleep(100); 
		
        cpp_inotify_event event = pFileNotifier->m_inotify.get_event();

		//LoggerWrapper::GetInstance()->Write(LV_STATUS, "We got something: mask %d, name %s, wd %d, tostring %s",
		//	event.mask, event.name.c_str(), event.wd, event.tostring().c_str());

		bool bCreateEvent = event.mask & IN_CREATE || event.mask & IN_MOVED_TO;
		bool bDeleteEvent = event.mask & IN_DELETE || event.mask & IN_MOVED_FROM;

		if(event.mask & IN_DELETE_SELF)
		{
			if(pFileNotifier->m_wdRootFolder == event.wd)
			{
				string sRootFolder = pFileNotifier->m_sRootFolder;
				pFileNotifier->ResetWatches(); 
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Voodoo with the root folder; someone reseted our watches. Reinitializing.");
				pFileNotifier->Watch(sRootFolder);
			}
		}
		
		if(bCreateEvent || bDeleteEvent)
        {
            PLUTO_SAFETY_LOCK(wfm, pFileNotifier->m_WatchedFilesMutex);
            string sFilename = pFileNotifier->m_mapWatchedFiles_Find(event.wd) + "/" + event.name;
            wfm.Release();

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "inotify: New event for %s", sFilename.c_str());

            bool bIsDir = (event.mask & IN_ISDIR) != 0;
			list<string> listFiles;

            if(bIsDir && bCreateEvent)
				pFileNotifier->Watch(sFilename);
			
			listFiles.push_back(sFilename);

			if(bCreateEvent)
				pFileNotifier->FireOnCreate(listFiles);
			else
				pFileNotifier->FireOnDelete(listFiles);
        }
    }
        
    return NULL;
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::RegisterCallbacks(FileNotifierCallback pfOnCreate, FileNotifierCallback pfOnDelete)
{
    if(!m_bCallbacksRegistered)
    {
        m_pfOnCreate = pfOnCreate;
        m_pfOnDelete = pfOnDelete;
		m_bCallbacksRegistered = true;

	    m_bCancelThread = false;

	    pthread_create(&m_INotifyWorkerThreadID, NULL, INotifyWorkerThread, (void *)this);

		if(m_pDatabase_pluto_media)
			pthread_create(&m_BackgroundWorkerThreadID, NULL, BackgroundWorkerThread, (void *)this);
    }
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::FireOnCreate(list<string> &listFiles)
{
	if(m_bCallbacksRegistered && m_pfOnCreate)
		m_pfOnCreate(listFiles);
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::FireOnDelete(list<string> &listFiles)
{
	if(m_bCallbacksRegistered && m_pfOnDelete)
		m_pfOnDelete(listFiles);
}
//-----------------------------------------------------------------------------------------------------
void FileNotifier::Run()
{
	//waiting for threads to finish
	pthread_join(m_INotifyWorkerThreadID, NULL);
	pthread_join(m_BackgroundWorkerThreadID, NULL);
}
//-----------------------------------------------------------------------------------------------------

