/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:51 (local)
    --------------------------------------------------------------
    Purpose:    Exceptions for work with library
**************************************************************************/

#include "Log.h"

#ifndef __HEADER_INCLUDED_EXCEPTIONS_LIBRARY__
#define __HEADER_INCLUDED_EXCEPTIONS_LIBRARY__

/// Exceptions for work with library
namespace PlutoITunesMediaConnector {
	namespace Broadcaster {
		namespace Exceptions {

/// Common exception for library
class Library : public std::exception {
public:
	Library() {
		REGISTER_RUNTIME_POINT("Exceptions::Library", __FILE__, __LINE__);
	}
};

/// Exception for work with XML COM
class LibraryCom : public Library {

public:

	LibraryCom(HRESULT operationResult)
	:	_operationResult(operationResult) {
		REGISTER_RUNTIME_POINT("Exceptions::LibraryCom", __FILE__, __LINE__);
		REGISTER_COM_ERROR(operationResult, __FILE__, __LINE__);
	}

private:

	HRESULT _operationResult;

};

		}// namespace Exceptions
	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_EXCEPTIONS_LIBRARY__
