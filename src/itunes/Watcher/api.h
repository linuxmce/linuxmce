/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 22:13 (local)
    --------------------------------------------------------------
    Purpose:    Watcher API
**************************************************************************/

#ifndef __HEADER_INCLUDED_WATCHER_API__
#define __HEADER_INCLUDED_WATCHER_API__

#ifndef WATCHER_EXPORTS
#	ifdef _DEBUG
#		define WATCHER_EXPORTS_LIB_FILE "..\\_Debug\\bin\\watcher_dbg.lib"
#	else // _DEBUG
#		define WATCHER_EXPORTS_LIB_FILE "..\\_Release\\bin\\watcher.lib"
#	endif // _DEBUG
#	define WATCHER_EXPORTS_LIB WATCHER_EXPORTS_LIB_FILE
#	pragma comment(lib, WATCHER_EXPORTS_LIB)
#	define WATCHER_API __declspec(dllimport)
#else // WATCHER_EXPORTS
#	define WATCHER_API __declspec(dllexport)
#endif // WATCHER_EXPORTS

#endif // __HEADER_INCLUDED_WATCHER_API__
