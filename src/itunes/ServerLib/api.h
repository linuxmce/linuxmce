/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:49 (local)
    --------------------------------------------------------------
    Purpose:    Library API
**************************************************************************/

#ifndef __HEADER_INCLUDED_API__
#define __HEADER_INCLUDED_API__

#ifndef PITMCB_EXPORTS
#	ifdef _DEBUG
#		define PITMCB_EXPORTS_LIB_FILE "..\\_Debug\\bin\\pitmcb_dbg.lib"
#	else // _DEBUG
#		define PITMCB_EXPORTS_LIB_FILE "..\\_Release\\bin\\pitmcb.lib"
#	endif // _DEBUG
#	define PITMCB_EXPORTS_LIB PITMCB_EXPORTS_LIB_FILE
#	// pragma comment(lib, PITMCB_EXPORTS_LIB)
#	define PITMCB_API __declspec(dllimport)
#else // PITMCB_EXPORTS
#	define PITMCB_API __declspec(dllexport)
#endif // PITMCB_EXPORTS

#endif // __HEADER_INCLUDED_API__
