
#include "stdafx.h"
#include "LibraryWatcher.h"
#include "..\Watcher\Watcher.h"
#include "Log.h"

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster;

///////////////////////////////////////////////////////////////////////////
// Threads:
///////////////////////////////////////////////////////////////////////////


namespace PlutoITunesMediaConnector {
	namespace Broadcaster {
		/// Thread which watching over the iTunes-library changes
		unsigned __stdcall WatchingThread(void * __args);
	}
}

unsigned __stdcall PlutoITunesMediaConnector::Broadcaster::WatchingThread(void * __args) {

	REGISTER_RUNTIME_POINT("WatchingThread", __FILE__, __LINE__);

	using namespace PlutoITunesMediaConnector::Broadcaster::Watcher;

	LibraryWatcher::WatchingThreadParams *const params
			= (LibraryWatcher::WatchingThreadParams*)__args;

	SetITunesLaunchWaitingHook(params->dbUpdateServiceControlCode, params->serviceName);
	WaitForSingleObject(params->shutdownEvent, INFINITE);
	RemoveITunesLaunchWaitingHook();

	_endthreadex(0);
	return 0;
}


///////////////////////////////////////////////////////////////////////////
// Class:
///////////////////////////////////////////////////////////////////////////


LibraryWatcher::LibraryWatcher(void) {
	REGISTER_RUNTIME_POINT("LibraryWatcher::LibraryWatcher", __FILE__, __LINE__);
}

LibraryWatcher::~LibraryWatcher(void) {
	REGISTER_RUNTIME_POINT("LibraryWatcher::~LibraryWatcher", __FILE__, __LINE__);
	stopWatchOverITunesLibraryChanges();
}

void LibraryWatcher::watchOverITunesLibraryChanges(const DWORD dbUpdateServiceControlCode,
												   const LPCWSTR internalServiceName) {

	REGISTER_RUNTIME_POINT("LibraryWatcher::watchOverITunesLibraryChanges", __FILE__, __LINE__);

	if (_watchingThread.thread) {
		REGISTER_RUNTIME_POINT("LibraryWatcher::watchOverITunesLibraryChanges", __FILE__, __LINE__);
		if (internalServiceName != _watchingThread.serviceName) {
			REGISTER_RUNTIME_POINT("LibraryWatcher::watchOverITunesLibraryChanges", __FILE__, __LINE__);
			stopWatchOverITunesLibraryChanges();
			watchOverITunesLibraryChanges(dbUpdateServiceControlCode, internalServiceName);
		}
		return;
	}
	
	_watchingThread.dbUpdateServiceControlCode = dbUpdateServiceControlCode;
	_watchingThread.serviceName = internalServiceName;

	_watchingThread.shutdownEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	ResetEvent(_watchingThread.shutdownEvent);

	_watchingThread.thread = (HANDLE)_beginthreadex(
		NULL, 0, &WatchingThread, &_watchingThread, 0, &_watchingThread.threadId);
	if (!_watchingThread.thread) {
		REGISTER_RUNTIME_POINT("LibraryWatcher::watchOverITunesLibraryChanges", __FILE__, __LINE__);
		// silent error
		stopWatchOverITunesLibraryChanges();
	}
	
}

void LibraryWatcher::stopWatchOverITunesLibraryChanges() {
	REGISTER_RUNTIME_POINT("LibraryWatcher::stopWatchOverITunesLibraryChanges", __FILE__, __LINE__);
	if (_watchingThread.thread) {
		REGISTER_RUNTIME_POINT("LibraryWatcher::stopWatchOverITunesLibraryChanges", __FILE__, __LINE__);
		SetEvent(_watchingThread.shutdownEvent);
		WaitForSingleObject(_watchingThread.thread, INFINITE);
		_watchingThread.thread = NULL;
	}
	if (_watchingThread.shutdownEvent) {
		REGISTER_RUNTIME_POINT("LibraryWatcher::stopWatchOverITunesLibraryChanges", __FILE__, __LINE__);
		CloseHandle(_watchingThread.shutdownEvent);
		_watchingThread.shutdownEvent = NULL;
	}
}

