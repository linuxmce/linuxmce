
#include "stdafx.h"
#include "Watcher.h"
#include "..\ServerLib\ITunes.h"
#include "..\ServerLib\Log.h"
#include "..\ServerLib\ServerConsts.h"
#include "..\CrashRpt\include\CrashRpt.h"

#define ITUNES_MODULE_NAME "iTunes.exe"
#define ITUNES_MODULE_NAME_LEN 10
#define MODULE_NAME_BUFFER_LEN 1024

#define SERVIE_NAME_BUFFER_LEN 255

#pragma data_seg(".Shared") 

/// Hook handle
HHOOK __shellHookHandle = NULL;
/// Customized event handle for service
DWORD __dbUpdateServiceControlCode = 0;
///  Service-owner name
WCHAR __serviceName[SERVIE_NAME_BUFFER_LEN + 1] = L"";

#pragma data_seg() 
#pragma comment(linker, "/SECTION:.Shared,RWS")

/// Updating thread handle
HANDLE __localDatabaseUpdateThread = NULL;

/// Module handle
HANDLE __moduleHandle = NULL;

///////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace PlutoITunesMediaConnector::Broadcaster::Watcher;


#pragma warning(disable: 4100)

BOOL APIENTRY DllMain(	HANDLE hModule,
						DWORD  ul_reason_for_call, 
						LPVOID lpReserved) {
	REGISTER_RUNTIME_POINT("DllMain", __FILE__, __LINE__);
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		REGISTER_RUNTIME_POINT("DllMain", __FILE__, __LINE__);
		__moduleHandle = hModule;
	}
	return TRUE;
}

#pragma warning(default: 4100)


#pragma warning(disable: 4100)

LRESULT CALLBACK PlutoITunesMediaConnector::Broadcaster::Watcher::ITunesLaunchWaiting(
	int code, WPARAM wParam, LPARAM lParam) {

	REGISTER_RUNTIME_POINT("ITunesLaunchWaiting", __FILE__, __LINE__);

	if (code == HSHELL_WINDOWCREATED) {

		REGISTER_RUNTIME_POINT("ITunesLaunchWaiting", __FILE__, __LINE__);

		TCHAR moduleName[MODULE_NAME_BUFFER_LEN];
		const UINT moduleNameLen = GetWindowModuleFileName(
			(HWND)wParam, moduleName, MODULE_NAME_BUFFER_LEN);

		if (	moduleNameLen > ITUNES_MODULE_NAME_LEN
			&&	!StrCmpI(moduleName + (moduleNameLen - ITUNES_MODULE_NAME_LEN), ITUNES_MODULE_NAME)) {
			
			REGISTER_RUNTIME_POINT("ITunesLaunchWaiting", __FILE__, __LINE__);
			startLocalDatabaseUpdateThread();

		}

	}
	
	return CallNextHookEx(__shellHookHandle, code, wParam, lParam);

}

#pragma warning(default: 4100)


void PlutoITunesMediaConnector::Broadcaster::Watcher::SetITunesLaunchWaitingHook(
	const DWORD dbUpdateServiceControlCode, const wstring& serviceName) {

	REGISTER_RUNTIME_POINT("SetITunesLaunchWaitingHook", __FILE__, __LINE__);

	if (!__shellHookHandle) {
		REGISTER_RUNTIME_POINT("SetITunesLaunchWaitingHook", __FILE__, __LINE__);
		__shellHookHandle = SetWindowsHookEx(
			WH_SHELL,
			&PlutoITunesMediaConnector::Broadcaster::Watcher::ITunesLaunchWaiting,
			(HINSTANCE)__moduleHandle,
			0);
	}
	__dbUpdateServiceControlCode = dbUpdateServiceControlCode;
	StringCbCopyNW(
		__serviceName, sizeof(WCHAR) * (SERVIE_NAME_BUFFER_LEN - 1),
		serviceName.c_str(), SERVIE_NAME_BUFFER_LEN);

}


void PlutoITunesMediaConnector::Broadcaster::Watcher::RemoveITunesLaunchWaitingHook() {
	REGISTER_RUNTIME_POINT("RemoveITunesLaunchWaitingHook", __FILE__, __LINE__);
	if (__shellHookHandle) {
		REGISTER_RUNTIME_POINT("RemoveITunesLaunchWaitingHook", __FILE__, __LINE__);
		UnhookWindowsHookEx(__shellHookHandle);
		__shellHookHandle = NULL;
	}
	__dbUpdateServiceControlCode = 0;
	stopLocalDatabaseUpdateThread();
}


#pragma warning(disable: 4100)

unsigned __stdcall PlutoITunesMediaConnector::Broadcaster::Watcher::localDatabaseUpdateThread(void * __args) {

	REGISTER_RUNTIME_POINT("localDatabaseUpdateThread", __FILE__, __LINE__);
	
	using namespace PlutoITunesMediaConnector::Broadcaster;

	wstring sid;

	WCHAR userName[UNLEN + 1];
	DWORD userNameLen = UNLEN;
	GetUserNameW(userName, &userNameLen);
	
	DWORD sidLen	= 0;
	DWORD domenLen	= 0;
	SID_NAME_USE sidNameUse;
	LookupAccountNameW(
		NULL, userName, NULL, &sidLen,
		NULL, &domenLen, &sidNameUse);

	PSID psid = static_cast<PSID>(new char[sidLen]);
	WCHAR* domenName = new WCHAR[domenLen];
	const BOOL lookupResult = LookupAccountNameW(
		NULL, userName, psid, &sidLen,
		domenName, &domenLen, &sidNameUse);
	if (lookupResult) {
		REGISTER_RUNTIME_POINT("localDatabaseUpdateThread", __FILE__, __LINE__);
		LPWSTR strSid;
		if (ConvertSidToStringSidW(psid, &strSid))  {
			sid = strSid;
			LocalFree(strSid);
		}
	}

	delete[] psid;
	delete[] domenName;

	HANDLE iTunesQuitEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	ResetEvent(iTunesQuitEvent);

	ITunes iTunes;
	iTunes.attach(iTunesQuitEvent);
	HANDLE iTunesDbUpdateEvent = iTunes.watchOverITunesLibraryChanges(sid);

	HANDLE events[2] = {
		iTunesQuitEvent,
		iTunesDbUpdateEvent
	};
	for (;;) {
		REGISTER_RUNTIME_POINT("localDatabaseUpdateThread", __FILE__, __LINE__);
		const DWORD object = WaitForMultipleObjects(2, events, FALSE, INFINITE);
		const DWORD code = object - WAIT_OBJECT_0;
		if (code == 1) {
			sendDbUpdateCmdToService();
		} else { // thread stop or iTunes quit
			break;
		}
	}

	iTunes.detach();

	_endthreadex(0);
	return 0;
}

#pragma warning(default: 4100)



static BOOL CALLBACK PlutoITunesMediaConnector::Broadcaster::Watcher::emergencyShutdownOnCrash(LPVOID) {
	REGISTER_RUNTIME_POINT("emergencyShutdownOnCrash", __FILE__, __LINE__);
	return TRUE;
}


void PlutoITunesMediaConnector::Broadcaster::Watcher::startLocalDatabaseUpdateThread() {

	REGISTER_RUNTIME_POINT("startLocalDatabaseUpdateThread", __FILE__, __LINE__);

	stopLocalDatabaseUpdateThread();
	
	// Install crash report handler
	LPVOID crashRptState = CrashReport::Install(
		&PlutoITunesMediaConnector::Broadcaster::Watcher::emergencyShutdownOnCrash,
		CRASHREPORT_EMAIL_ADDR, CRASHREPORT_EMAIL_SUBJ);
#	ifdef _DEBUG
		CrashReport::AddFile(crashRptState, "\\PlutoITunesMediaConnector.log", "debugLog");
#	endif // _DEBUG

	unsigned int threadId;
	__localDatabaseUpdateThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		&PlutoITunesMediaConnector::Broadcaster::Watcher::localDatabaseUpdateThread,
		NULL,
		0,
		&threadId);
	
	if (!__localDatabaseUpdateThread) {
		REGISTER_RUNTIME_POINT("startLocalDatabaseUpdateThread", __FILE__, __LINE__);
		// silent error
		stopLocalDatabaseUpdateThread();
	}

}


void PlutoITunesMediaConnector::Broadcaster::Watcher::stopLocalDatabaseUpdateThread() {
	REGISTER_RUNTIME_POINT("stopLocalDatabaseUpdateThread", __FILE__, __LINE__);
	if (__localDatabaseUpdateThread) {
		REGISTER_RUNTIME_POINT("stopLocalDatabaseUpdateThread", __FILE__, __LINE__);
		WaitForSingleObject(__localDatabaseUpdateThread, INFINITE);
	}
	__localDatabaseUpdateThread = NULL;
}


BOOL PlutoITunesMediaConnector::Broadcaster::Watcher::sendDbUpdateCmdToService() {
	
	REGISTER_RUNTIME_POINT("sendDbUpdateCmdToService", __FILE__, __LINE__);

	if (!__dbUpdateServiceControlCode) {
		REGISTER_RUNTIME_POINT("sendDbUpdateCmdToService", __FILE__, __LINE__);
		return FALSE;
	}

	BOOL result = FALSE;

	SC_HANDLE scManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
	if (scManager) {
		REGISTER_RUNTIME_POINT("sendDbUpdateCmdToService", __FILE__, __LINE__);
		SC_HANDLE service = OpenServiceW(
			scManager, __serviceName, SERVICE_USER_DEFINED_CONTROL);
		if (service) {
			REGISTER_RUNTIME_POINT("sendDbUpdateCmdToService", __FILE__, __LINE__);
			SERVICE_STATUS status;
			ControlService(service, __dbUpdateServiceControlCode, &status);
			CloseServiceHandle(service);
			result = TRUE;
		}
		CloseServiceHandle(scManager);
	}

	return result;

}
