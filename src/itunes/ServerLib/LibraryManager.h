/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:52 (local)
    --------------------------------------------------------------
    Purpose:    Local library manager
**************************************************************************/

#include "Library.h"
#include "api.h"

#ifndef __HEADER_INCLUDED_LIBRARYMANAGER__
#define __HEADER_INCLUDED_LIBRARYMANAGER__

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {

/// Local library manager
class PITMCB_API LibraryManager {

public:

	LibraryManager(void);

	virtual ~LibraryManager(void);

};

	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_LIBRARYMANAGER__
