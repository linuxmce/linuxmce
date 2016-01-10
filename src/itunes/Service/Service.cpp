/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/10/01 15:39 (local)
    --------------------------------------------------------------
    Purpose:    Service
**************************************************************************/

#include "stdafx.h"
#include "..\ServerLib\LibraryWatcher.h"
#include "..\ServerLib\Server.h"
#include "..\ServerLib\ServerConsts.h"
#include "..\ServerLib\Log.h"

/// Service name
#define BROADCAST_SERVICE_NAME_PUBLIC L"Pluto iTunes Connector Service"
/// Internal service name
#define BROADCAST_SERVICE_NAME_INTERNAL L"PlutoITunesMediaConnector_Broadcaster"

/// Watcher name
#define BROADCAST_WATCHER_NAME_PUBLIC L"Pluto iTunes Watcher"

/// Shell command: service start
#define SHELL_COMMAND_BROADCAST_SERVICE_START L"service"
/// Shell command: service installation
#define SHELL_COMMAND_BROADCAST_SERVICE_INSTALL L"install"
/// Shell command: service uninstallation
#define SHELL_COMMAND_BROADCAST_SERVICE_UNINSTALL L"uninstall"
/// Shell command: watch
#define SHELL_COMMAND_BROADCAST_WATCH_START L"watch"
/// Shell command: stop watch
#define SHELL_COMMAND_BROADCAST_WATCH_STOP L"notwatch"

/// Server connections number in pool
#define SERVER_CONNECTIONS_POOL_SIZE 5

/// Watcher window class name
#define WATCHER_WND_CLASS "PlutoITunesMediaConnector_Broadcaster_Watcher_WndClass"


/// Customized event for LibraryManager object: the local library has been updated
#define SERVICE_DEFINED_CONTROL_UPDATE_LOCAL_DB 200

/// Shell commands
enum BROADCAST_SERVICE_SHELL_COMMANDS {
	BSSC_UNKNOWN = 0,		/// Unknown command
	BSSC_SERVICE_START,		/// Start service
	BSSC_SERVICE_INSTALL,	/// Install service
	BSSC_SERVICE_UNINSTALL,	/// Uninstall service
	BSSC_SERVICE_WATCH_START, /// Launch watcher
	BSSC_SERVICE_WATCH_STOP /// to not watch
};

///////////////////////////////////////////////////////////////////////////

using namespace std;

SERVICE_STATUS __broadcastServiceStatus;
SERVICE_STATUS_HANDLE __broadcastServiceStatusHandle;
HANDLE __broadcastServiceShutdownEvent = NULL;
HANDLE __localLibraryUpdateEvent = NULL;

/// Controler of the broadcasting service
void WINAPI BroadcastServiceCtrlHandler(DWORD opcode) {
	REGISTER_RUNTIME_POINT("BroadcastServiceCtrlHandler", __FILE__, __LINE__);
	switch (opcode) { 
		//case SERVICE_CONTROL_PAUSE: 
		//	_serviceStatus.dwCurrentState = SERVICE_PAUSED; 
		//	break; 
		//case SERVICE_CONTROL_CONTINUE: 
		//	_serviceStatus.dwCurrentState = SERVICE_RUNNING; 
		//	break; 
		case SERVICE_CONTROL_STOP: 
			__broadcastServiceStatus.dwWin32ExitCode	= 0; 
			__broadcastServiceStatus.dwCurrentState		= SERVICE_STOPPED; 
			__broadcastServiceStatus.dwCheckPoint		= 0; 
			__broadcastServiceStatus.dwWaitHint			= 0; 
			SetServiceStatus(__broadcastServiceStatusHandle, &__broadcastServiceStatus);
			SetEvent(__broadcastServiceShutdownEvent);
			REGISTER_RUNTIME_POINT("BroadcastServiceCtrlHandler", __FILE__, __LINE__);
			break;
		//case SERVICE_CONTROL_INTERROGATE: 
		//	break; 
		case SERVICE_DEFINED_CONTROL_UPDATE_LOCAL_DB:
			SetEvent(__localLibraryUpdateEvent);
			REGISTER_RUNTIME_POINT("BroadcastServiceCtrlHandler", __FILE__, __LINE__);
			break;
	}
}


#pragma warning(disable: 4100)

/// Main function of the broadcasting service 
void WINAPI BroadcastServiceMain(DWORD argc, LPWSTR *argv) {

	REGISTER_RUNTIME_POINT("BroadcastServiceMain", __FILE__, __LINE__);

	__broadcastServiceStatus.dwServiceType				= SERVICE_WIN32_OWN_PROCESS; 
	__broadcastServiceStatus.dwCurrentState				= SERVICE_START_PENDING; 
	__broadcastServiceStatus.dwControlsAccepted			= SERVICE_ACCEPT_STOP; 
	__broadcastServiceStatus.dwWin32ExitCode			= 0; 
	__broadcastServiceStatus.dwServiceSpecificExitCode	= 0; 
	__broadcastServiceStatus.dwCheckPoint				= 0; 
	__broadcastServiceStatus.dwWaitHint					= 0; 
	
	__broadcastServiceStatusHandle = RegisterServiceCtrlHandlerW(
		BROADCAST_SERVICE_NAME_INTERNAL, &BroadcastServiceCtrlHandler); 
	if (!__broadcastServiceStatusHandle) {
		REGISTER_RUNTIME_POINT("BroadcastServiceMain", __FILE__, __LINE__);
		return;
	}

	__broadcastServiceStatus.dwCurrentState	= SERVICE_RUNNING; 
	__broadcastServiceStatus.dwCheckPoint	= 0; 
	__broadcastServiceStatus.dwWaitHint		= 0;  

	__broadcastServiceShutdownEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	ResetEvent(__broadcastServiceShutdownEvent);
	__localLibraryUpdateEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	ResetEvent(__localLibraryUpdateEvent);

	SetServiceStatus(__broadcastServiceStatusHandle, &__broadcastServiceStatus);

	///////////////////////////////////////////////////////////////////////

	using namespace PlutoITunesMediaConnector::Broadcaster;
	Server &server = Server::getInstance();
	server.start(SERVER_PORT, SERVER_CONNECTIONS_POOL_SIZE, __localLibraryUpdateEvent);
	WaitForSingleObject(__broadcastServiceShutdownEvent, INFINITE);
	server.stop();

	///////////////////////////////////////////////////////////////////////
	
	CloseHandle(__broadcastServiceShutdownEvent);
	CloseHandle(__localLibraryUpdateEvent);
	
	__broadcastServiceStatus.dwWin32ExitCode	= 0; 
	__broadcastServiceStatus.dwCurrentState		= SERVICE_STOPPED; 
	__broadcastServiceStatus.dwCheckPoint		= 0; 
	__broadcastServiceStatus.dwWaitHint			= 0; 
	SetServiceStatus(__broadcastServiceStatusHandle, &__broadcastServiceStatus);

}

#pragma warning(default: 4100)


/// Returns a shall-command code
BROADCAST_SERVICE_SHELL_COMMANDS GetShellCommand() {

	REGISTER_RUNTIME_POINT("GetShellCommand", __FILE__, __LINE__);
	
	BROADCAST_SERVICE_SHELL_COMMANDS command = BSSC_UNKNOWN;

	int argc;
	const LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	
	if (argc == 2) {
		const LPCWSTR cmd = argv[1];
		if (!StrCmpLogicalW(cmd, SHELL_COMMAND_BROADCAST_SERVICE_START)) {
			command = BSSC_SERVICE_START;
		} else if (!StrCmpLogicalW(cmd, SHELL_COMMAND_BROADCAST_SERVICE_INSTALL)) {
			command = BSSC_SERVICE_INSTALL;
		} else if (!StrCmpLogicalW(cmd, SHELL_COMMAND_BROADCAST_SERVICE_UNINSTALL)) {
			command = BSSC_SERVICE_UNINSTALL;
		} else if (!StrCmpLogicalW(cmd, SHELL_COMMAND_BROADCAST_WATCH_START)) {
			command = BSSC_SERVICE_WATCH_START;
		} else if (!StrCmpLogicalW(cmd, SHELL_COMMAND_BROADCAST_WATCH_STOP)) {
			command = BSSC_SERVICE_WATCH_STOP;
		}
	}

	return command;

}


/// Starts a broadcasting service
void StartBroadcastService() {
	REGISTER_RUNTIME_POINT("StartBroadcastService", __FILE__, __LINE__);
	SERVICE_TABLE_ENTRYW dispatchTable[] = {
		{BROADCAST_SERVICE_NAME_INTERNAL, &BroadcastServiceMain},
		{NULL, NULL}
	};
	StartServiceCtrlDispatcherW(dispatchTable);
}


/// Stops a broadcasting service
DWORD StopBroadcastService(	SC_HANDLE hSCM,
							SC_HANDLE hService, 
							BOOL fStopDependencies,
							DWORD dwTimeout) {

	REGISTER_RUNTIME_POINT("StopBroadcastService", __FILE__, __LINE__);

	SERVICE_STATUS ss;
	DWORD dwStartTime = GetTickCount();

	// Make sure the service is not already stopped
	if (!QueryServiceStatus(hService, &ss)) {
		REGISTER_RUNTIME_POINT("StopBroadcastService", __FILE__, __LINE__);
		return GetLastError();
	}

	if (ss.dwCurrentState == SERVICE_STOPPED)  {
		return ERROR_SUCCESS;
	}

	// If a stop is pending, just wait for it
	while (ss.dwCurrentState == SERVICE_STOP_PENDING) {
		Sleep(ss.dwWaitHint);
		if (!QueryServiceStatus(hService, &ss)) {
			return GetLastError();
		}
		if (ss.dwCurrentState == SERVICE_STOPPED) {
			return ERROR_SUCCESS;
		}
		if (GetTickCount() - dwStartTime > dwTimeout) {
			return ERROR_TIMEOUT;
		}
	}

	// If the service is running, dependencies must be stopped first
	if (fStopDependencies) {
		DWORD i;
		DWORD dwBytesNeeded;
		DWORD dwCount;

		LPENUM_SERVICE_STATUS	lpDependencies = NULL;
		ENUM_SERVICE_STATUS		ess;
		SC_HANDLE				hDepService;

		// Pass a zero-length buffer to get the required buffer size
		if (EnumDependentServices(hService, SERVICE_ACTIVE, lpDependencies, 0, &dwBytesNeeded, &dwCount)) {
			// If the Enum call succeeds, then there are no dependent
			// services so do nothing
		} else {
			if (GetLastError() != ERROR_MORE_DATA) {
				return GetLastError(); // Unexpected error
			}
			// Allocate a buffer for the dependencies
			lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc( 
				GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded );
			if (!lpDependencies) {
				return GetLastError();
			}
			__try {
				// Enumerate the dependencies
				if (!EnumDependentServices(hService, SERVICE_ACTIVE,
						lpDependencies, dwBytesNeeded, &dwBytesNeeded, &dwCount)) {
					return GetLastError();
				}
				for (i = 0; i < dwCount; i++) {
					ess = *(lpDependencies + i);
					// Open the service
					hDepService = OpenService( hSCM, ess.lpServiceName, 
						SERVICE_STOP | SERVICE_QUERY_STATUS );
					if (!hDepService) {
						return GetLastError();
					}
					__try {
						// Send a stop code
						if (!ControlService( hDepService, 
							SERVICE_CONTROL_STOP, &ss)) {
							return GetLastError();
						}
						// Wait for the service to stop
						while (ss.dwCurrentState != SERVICE_STOPPED) {
							Sleep(ss.dwWaitHint);
							if (!QueryServiceStatus(hDepService, &ss)) {
								return GetLastError();
							}
							if (ss.dwCurrentState == SERVICE_STOPPED) {
								break;
							}
							if (GetTickCount() - dwStartTime > dwTimeout) {
								return ERROR_TIMEOUT;
							}
						}
					} __finally {
						// Always release the service handle
						CloseServiceHandle(hDepService);
					}
				}
			} __finally  {
				// Always free the enumeration buffer
				HeapFree(GetProcessHeap(), 0, lpDependencies);
			}
		}
	}

	// Send a stop code to the main service
	if (!ControlService(hService, SERVICE_CONTROL_STOP, &ss)) {
		return GetLastError();
	}

	// Wait for the service to stop
	while (ss.dwCurrentState != SERVICE_STOPPED) {
		Sleep(ss.dwWaitHint);
		if (!QueryServiceStatus(hService, &ss)) {
			return GetLastError();
		}
		if (ss.dwCurrentState == SERVICE_STOPPED) {
			break;
		}
		if (GetTickCount() - dwStartTime > dwTimeout) {
			return ERROR_TIMEOUT;
		}
	}
	
	// Return success
	return ERROR_SUCCESS;
}


BOOL disableFirewall() {

	REGISTER_RUNTIME_POINT("disableFirewall", __FILE__, __LINE__);

	BOOL result = TRUE;

	INetFwMgr* fwMgr = NULL;
	INetFwProfile* fwProfile = NULL;
	INetFwPolicy* fwPolicy = NULL;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplication* fwAuthApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	BSTR fwBstrProcessImageFileName = SysAllocString(argv[0]);
	BSTR fwBstrProcessAppName = SysAllocString(BROADCAST_SERVICE_NAME_PUBLIC);
	
	try {

		HRESULT hr = CoCreateInstance(
			__uuidof(NetFwMgr),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(INetFwMgr),
			(void**)&fwMgr);
		if (FAILED(hr)) {
			throw exception();
		}

		hr = fwMgr->get_LocalPolicy(&fwPolicy);
		if (FAILED(hr)) {
			throw exception();
		}
		hr = fwPolicy->get_CurrentProfile(&fwProfile);
		if (FAILED(hr)) {
			throw exception();
		}

		hr = fwProfile->get_AuthorizedApplications(&fwApps);
		if (FAILED(hr)) {
			throw exception();
		}
		
		VARIANT_BOOL fwEnabled;
		hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
		if (SUCCEEDED(hr)) {
			hr = fwApp->get_Enabled(&fwEnabled);
			if (FAILED(hr)) {
				throw exception();
			}
		} else {
			fwEnabled = VARIANT_FALSE;
		}
			
		if (fwEnabled == VARIANT_FALSE) {

			hr = CoCreateInstance(
					__uuidof(NetFwAuthorizedApplication),
					NULL,
					CLSCTX_INPROC_SERVER,
					__uuidof(INetFwAuthorizedApplication),
					(void**)&fwAuthApp);
			if (FAILED(hr)) {
				throw exception();
			}

			hr = fwAuthApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
			if (FAILED(hr)) {
				throw exception();
			}

			hr = fwAuthApp->put_Name(fwBstrProcessAppName);
			if (FAILED(hr)) {
				throw exception();
			}

			hr = fwApps->Add(fwAuthApp);
			if (FAILED(hr)) {
				throw exception();
			}

		}

	} catch (...) {
		result = FALSE;
	}

	if (fwApp) {
		fwApp->Release();
	}
	if (fwApps) {
		fwApps->Release();
	}
	if (fwAuthApp) {
		fwAuthApp->Release();
	}
	if (fwMgr) {
		fwMgr->Release();
	}
	if (fwPolicy) {
		fwPolicy->Release();
	}
	if (fwProfile) {
		fwProfile->Release();
	}
    SysFreeString(fwBstrProcessImageFileName);
	SysFreeString(fwBstrProcessAppName);

	return result;

}


/// Installs a broadcasting service
BOOL InstallBroadcastService() {
	REGISTER_RUNTIME_POINT("InstallBroadcastService", __FILE__, __LINE__);
	disableFirewall();
	return TRUE;
}


/// Uninstalls a broadcasting service
BOOL UninstallBroadcastService() {
	REGISTER_RUNTIME_POINT("UninstallBroadcastService", __FILE__, __LINE__);
	return TRUE;
}


#pragma warning(disable: 4100)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_QUERYENDSESSION || message == WM_DESTROY) {
		PostQuitMessage(0);
		return TRUE;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

#pragma warning(default: 4100)


/**
 * iTunes watcher
 *
 */
int Watch(HINSTANCE hInstance) {

	REGISTER_RUNTIME_POINT("Watch", __FILE__, __LINE__);

	WNDCLASS wc; 
	wc.style			= 0;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0; 
	wc.cbWndExtra		= 0; 
	wc.hInstance		= hInstance; 
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName	= WATCHER_WND_CLASS; 
	wc.lpszMenuName		= NULL;
	RegisterClass(&wc);
	CreateWindow(
		WATCHER_WND_CLASS, "",	WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	////

	MSG msg;

	PlutoITunesMediaConnector::Broadcaster::LibraryWatcher watcher;
	watcher.watchOverITunesLibraryChanges(
		SERVICE_DEFINED_CONTROL_UPDATE_LOCAL_DB, BROADCAST_SERVICE_NAME_INTERNAL);
	
	while (GetMessage(&msg, NULL, 0, 0)) { 
		REGISTER_RUNTIME_POINT("Watch", __FILE__, __LINE__);
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	
	watcher.stopWatchOverITunesLibraryChanges();
	
	return (int)msg.wParam;

}

#pragma warning(disable: 4100)


void NotWatch() {
	REGISTER_RUNTIME_POINT("NotWatch", __FILE__, __LINE__);
	HWND windowHandle = FindWindow(WATCHER_WND_CLASS, "");
	if (windowHandle) {
		REGISTER_RUNTIME_POINT("NotWatch", __FILE__, __LINE__);
		SendMessage(windowHandle, WM_DESTROY, NULL, NULL);
	}
}


int __stdcall WinMain(	HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPSTR lpCmdLine,
						int nCmdShow) {
	
	REGISTER_RUNTIME_POINT("WinMain", __FILE__, __LINE__);

	int result = 0;
	const BROADCAST_SERVICE_SHELL_COMMANDS cmd = GetShellCommand();

	switch (cmd) {
		case BSSC_SERVICE_START:
			StartBroadcastService();
			break;
		case BSSC_SERVICE_UNINSTALL:
			UninstallBroadcastService();
		case BSSC_SERVICE_WATCH_STOP:
			NotWatch();
			break;
		case BSSC_SERVICE_INSTALL:
			CoInitialize(0);
			InstallBroadcastService();
			CoUninitialize();
			break;
		case BSSC_SERVICE_WATCH_START:
			result = Watch(hInstance);
			break;
    }
 
	return result;

}

#pragma warning(default: 4100)
