#ifndef __ASTERISK_COMMAND_H__
#define __ASTERISK_COMMAND_H__

#include "token.h"


namespace ASTERISK {

class IAsteriskCommand
{
protected:
	Token m_token;

public:

	IAsteriskCommand() {}
	virtual ~IAsteriskCommand() {}

	virtual void handleStartup() = 0;
	virtual void handleTerminate() = 0;
	virtual bool handleResponse(Token &token) = 0;

	Token& GetToken() { return m_token; }
};

};

#endif //__ASTERISK_COMMAND_H__

