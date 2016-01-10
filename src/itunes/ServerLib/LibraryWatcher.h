/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:52 (local)
    --------------------------------------------------------------
    Purpose:    Library watcher
**************************************************************************/

#include "Library.h"
#include "ITunes.h"
#include "api.h"

#ifndef __HEADER_INCLUDED_LIBRARYWATCHER__
#define __HEADER_INCLUDED_LIBRARYWATCHER__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

/// Library watcher
class PITMCB_API LibraryWatcher {

public:

	/// Parameters for the watching thread
	struct PITMCB_API WatchingThreadParams {
		/// Shutdown event for the watching thread
		HANDLE shutdownEvent;
		/// Thread which watching over the iTunes-library changes
		HANDLE thread;
		/// Thread ID
		unsigned int threadId;
		/// Service control code: update DB
		DWORD dbUpdateServiceControlCode;
		/// Current service name
		std::wstring serviceName;
		
		WatchingThreadParams()
		:	shutdownEvent(NULL)
		,	thread(NULL) {
			/*...*/
		}
	};

public:

	LibraryWatcher(void);

	virtual ~LibraryWatcher(void);

	/**
	 * Starts a watching over the iTunes-library changes and updates 
	 * local library. If local library has been updated, it should
	 * send dbUpdateServiceControlCode code to the service with
	 * name internalServiceName.
	 */
	void watchOverITunesLibraryChanges(	const DWORD dbUpdateServiceControlCode,
										const LPCWSTR internalServiceName);


	/// Stops a watching over the iTunes-library changes.
	void stopWatchOverITunesLibraryChanges();

private:

	/// Parameters for the watching thread
	WatchingThreadParams _watchingThread;

};

	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_LIBRARYWATCHER__
