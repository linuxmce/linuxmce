/**************************************************************************
    Author:     Evgueni V. Palchukovsky
    E-mail:     coder@nutteno.com
    URL:        http://www.nutteno.com
    --------------------------------------------------------------
    Created:    2006/09/30 21:50 (local)
    --------------------------------------------------------------
    Purpose:    Exceptions for work with iTunes
**************************************************************************/

#include "Log.h"

#ifndef __HEADER_INCLUDED_EXCEPTIONS_ITUNES__
#define __HEADER_INCLUDED_EXCEPTIONS_ITUNES__

/// Exceptions for work with iTunes
namespace PlutoITunesMediaConnector {
	namespace Broadcaster {
		namespace Exceptions {

/// Common exception for iTunes
class ITunes : public std::exception {
public:
	ITunes() {
		REGISTER_RUNTIME_POINT("Exceptions::ITunes", __FILE__, __LINE__);
	}
};

/// Exception for work with iTunes COM
class ITunesCom : public ITunes {

public:

	ITunesCom(HRESULT operationResult)
	:	_operationResult(operationResult) {
		REGISTER_RUNTIME_POINT("Exceptions::ITunesCom", __FILE__, __LINE__);
		REGISTER_COM_ERROR(operationResult, __FILE__, __LINE__);
	}

private:

	HRESULT _operationResult;

};

		} // namespace Exceptions
	} // namespace Broadcaster 
} // namespace PlutoITunesMediaConnector

#endif // __HEADER_INCLUDED_EXCEPTIONS_ITUNES__
