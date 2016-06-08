#include "FileStatusObserver.h"
#include "inotify/FileNotifier.h"
#include "PlutoUtils/ProcessUtils.h"
#include "FileUtils/file_utils.h"
#include <algorithm>
using namespace std;

//------------------------------------------------------------------------------------------
#define CHECK_OBSERVED_FILES_STATUS 1
#define OBSERVER_SCANNING_INTERVAL  3 //seconds
//------------------------------------------------------------------------------------------
FileStatusObserver FileStatusObserver::m_Instance;
//------------------------------------------------------------------------------------------
FileStatusObserver::FileStatusObserver() :
	m_ObservedFilesMutex("observed files"), m_pAlarmManager(NULL), m_pFileNotifier(NULL)
{
    m_ObservedFilesMutex.Init(NULL);

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(1); 
}
//------------------------------------------------------------------------------------------
FileStatusObserver::~FileStatusObserver()
{
	Finalize();
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::Finalize()
{
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}
//------------------------------------------------------------------------------------------
FileStatusObserver &FileStatusObserver::Instance()
{
	return m_Instance;
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::SetFileNotifier(FileNotifier *pFileNotifier)
{
	PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "OBSERVER: using file notifier %p", pFileNotifier);
	m_pFileNotifier = pFileNotifier;
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::UnsetFileNotifier()
{
	PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "OBSERVER: not using any file notifier");
	m_pFileNotifier = NULL;
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::Observe(string sFilename)
{
	PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
	list<string>::iterator it = find(m_listFileToObserve.begin(), m_listFileToObserve.end(), sFilename);
	if(it == m_listFileToObserve.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "OBSERVER: File added to list with files to be observed: %s", sFilename.c_str());
		m_listFileToObserve.push_back(sFilename);

		//start alarm if not already started
		if(-1 == m_pAlarmManager->FindAlarmByType(CHECK_OBSERVED_FILES_STATUS))
			m_pAlarmManager->AddRelativeAlarm(3, this, CHECK_OBSERVED_FILES_STATUS, NULL);
	}
}
//------------------------------------------------------------------------------------------
void FileStatusObserver::AlarmCallback(int id, void* /*param*/)
{
	if(id == CHECK_OBSERVED_FILES_STATUS)
	{
		PLUTO_SAFETY_LOCK(ofm, m_ObservedFilesMutex);
		for(list<string>::iterator it = m_listFileToObserve.begin(); it != m_listFileToObserve.end();)
		{
			string sFilename = *it;

			if(!FileUtils::FileExists(sFilename))
			{
				LoggerWrapper::GetInstance()->Write(LV_MEDIA, "OBSERVER: The file was delete: %s", sFilename.c_str());
				m_listFileToObserve.erase(it++);
			}
			else if(!IsFileOpen(sFilename) && NULL != m_pFileNotifier)
			{
				LoggerWrapper::GetInstance()->Write(LV_MEDIA, "OBSERVER: File not opened anymore: %s", sFilename.c_str());

				list<string> listFiles;
				listFiles.push_back(sFilename);
				m_pFileNotifier->FireOnCreate(listFiles);

				m_listFileToObserve.erase(it++);
			}
			else
			{
				++it;
			}
		}

		//start alarm again only if needed
		if(!m_listFileToObserve.empty())
			m_pAlarmManager->AddRelativeAlarm(60, this, CHECK_OBSERVED_FILES_STATUS, NULL);
	}
}

bool FileStatusObserver::IsFileOpen(string sFilename)
{
#ifdef WIN32

	HANDLE hFile = CreateFile(
		sFilename.c_str(),     // file to open
        GENERIC_WRITE,         // open for writing
        0,					   // deny reading or writing
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);   

	if(NULL != hFile && INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		return false;
	}

	if(ERROR_SHARING_VIOLATION == GetLastError())
		return true;

	return false;

#else

	//HACK HACK HACK! LINUX DOESN'T HAVE A WORKING C FUNCTION TO DETECT IF A FILE IS OPENED! :(
	/*
	open function man:

	       O_EXCL When used with O_CREAT, if the file already exists it is an error and the open() will fail. In this context, a symbolic link exists, regardâ
              less  of  where  it  points to.  O_EXCL is broken on NFS file systems; programs which rely on it for performing locking tasks will contain a
              race condition.  The solution for performing atomic file locking using a lockfile is to create a unique file on the same file system  (e.g.,
              incorporating  hostname  and  pid), use link(2) to make a link to the lockfile. If link() returns 0, the lock is successful.  Otherwise, use
              stat(2) on the unique file to check if its link count has increased to 2, in which case the lock is also successful.
	*/

	if(time(NULL) - UpdateMediaFileUtils::ModificationDateForFile(sFilename.c_str()) > 120)
		return false;

	string sCommandLine = "fuser -a \"" + sFilename + "\" ";
	string sOutput;

	if(ProcessUtils::RunApplicationAndGetOutput(sCommandLine, sOutput))
	{
		int nPID = atoi(sOutput.c_str());
		if(nPID != 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_MEDIA, "OBSERVER: File %s is used by process %d", 
				sFilename.c_str(), nPID);
			return true;
		}
	}

	return false;

#endif
}
//------------------------------------------------------------------------------------------

