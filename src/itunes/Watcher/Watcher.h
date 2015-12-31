/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/10/08 13:53 (local)
	ID:         $id$
    --------------------------------------------------------------
    Purpose:    iTunes watcher lib
**************************************************************************/

#include "api.h"

#ifndef __HEADER_INCLUDED_WATCHER__
#define __HEADER_INCLUDED_WATCHER__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {
		
		/// iTunes watcher lib
		namespace Watcher {
		
			/**
			 * Sets a hook which will wait the iTunes launching
			 *
			 */
			WATCHER_API void SetITunesLaunchWaitingHook(
				const DWORD serviceControlCodeForCustomizedEvent,
				const std::wstring& serviceName);
		
			/**
			 * Removes a hook which will wait the iTunes launching
			 *
			 */
			WATCHER_API void RemoveITunesLaunchWaitingHook();

			/**
			* Thread starts with iTunes and watching for iTunes database.
			*/
			unsigned __stdcall localDatabaseUpdateThread(void * __args);

			/**
			* Starts the thread "localDatabaseUpdateThread".
			* Can be only one iTunes for all windows users. So, we always stop
			* previous thread before launch new.
			*/
			void startLocalDatabaseUpdateThread();
			
			/**
			* Stops the thread "localDatabaseUpdateThread"
			*/
			void stopLocalDatabaseUpdateThread();

			/**
			* Sends the "update-local-db" control code to the server
			*/
			BOOL sendDbUpdateCmdToService();

			/**
			 * Hook procedure that receives notifications useful to shell applications.
			 * Waiting for iTunes launching and sends message to the service.
			 */
			LRESULT CALLBACK ITunesLaunchWaiting(	int code,
													WPARAM wParam,
													LPARAM lParam);

			static BOOL CALLBACK emergencyShutdownOnCrash(LPVOID);

		}
	}
}

#endif // __HEADER_INCLUDED_WATCHER__
