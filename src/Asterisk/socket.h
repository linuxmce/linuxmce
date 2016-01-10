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
// C++ Interface: socket
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKSOCKET_H
#define ASTERISKSOCKET_H


#include <string>
#include "token.h"

namespace ASTERISK {

	
/**
@author 
*/
class Socket{
public:
    Socket();
    ~Socket();

public:
	int Connect();
	int Disconnect();
	bool isConnected();


	int sendToken(const Token& token);
	int recvToken(Token &token);
	bool isReceivable();

private:
	int recvGreeting();
	std::string recvLine();

private:
	int socket_fd;
	std::string recvtail;
};

};

#endif
