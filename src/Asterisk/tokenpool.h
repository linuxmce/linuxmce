/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//
// C++ Interface: TokenPool 
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ASTERISKTOKENPOOL_H
#define ASTERISKTOKENPOOL_H

#include "socket.h"
#include "thread.h"
#include "mutex.h"
#include "token.h"

#include <list>

#include "asteriskcommand.h"

namespace ASTERISK { 

/**
@author 
*/
class TokenPool : public Thread {
public:
    TokenPool();
    virtual ~TokenPool();

	int sendCommand(IAsteriskCommand* pCommand);

protected:
    virtual void* _Run();
	
	virtual int handleToken(Token* ptoken, bool& bIsResponseToken);
	virtual int handleConnect(Socket *psock);
	virtual void handleDisconnect();
	virtual void handleError(int errcode);

private:
    Mutex sm; /*socket access mutex*/
    Socket sock;

    Mutex sqm; /*send queue access mutex*/
    std::list<IAsteriskCommand *> sendqueue;
};

};

#endif
