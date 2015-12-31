#ifndef __FILE_STATUS_OBSERVER_H__
#define __FILE_STATUS_OBSERVER_H__
//------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
#include "AlarmManager.h"
//------------------------------------------------------------------------------------------
using namespace DCE;
//------------------------------------------------------------------------------------------
class FileNotifier;
//------------------------------------------------------------------------------------------
class FileStatusObserver : public AlarmEvent
{
private:

	FileStatusObserver();
	static FileStatusObserver m_Instance;
	list<string> m_listFileToObserve;
	pluto_pthread_mutex_t m_ObservedFilesMutex;
	AlarmManager *m_pAlarmManager;
	FileNotifier *m_pFileNotifier;

	void AlarmCallback(int id, void* param);

public:

    static FileStatusObserver &Instance();

	~FileStatusObserver();

	void SetFileNotifier(FileNotifier *pFileNotifier);
	void UnsetFileNotifier();
	void Finalize();

	void Observe(string sFilename);

	//utils
	static bool IsFileOpen(string sFilename);
};
//------------------------------------------------------------------------------------------
#endif // __FILE_STATUS_OBSERVER_H__
