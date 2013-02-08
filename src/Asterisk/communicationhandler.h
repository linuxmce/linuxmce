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
// C++ Interface: ringdetectstatemachine
//
// Description: 
//
//
// Author: Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __COMMUNICATION_HANDLER_H__
#define __COMMUNICATION_HANDLER_H__

#include "tokenmanager.h"

#include <string>
#include <map>

namespace ASTERISK {

/**
@author 
*/
class CommunicationHandler : public TokenPool {

public:
	CommunicationHandler();
	~CommunicationHandler();

protected:
	int handleToken(Token* ptoken, bool& bIsResponseToken);
	
private:

	//extension for the one called <-> the channel of the one who makes to call
	std::map<std::string,std::string> map_ringext;

	//channel <-> callerid
	std::map<std::string,std::string> map_callerid;

	int handleNewextenEvent(Token* ptoken);
	int handleHangupEvent(Token* ptoken);
	int handleLinkEvent(Token* ptoken);
	int handleNewStateEvent(Token* ptoken);
	int handleNewChannelEvent(Token* ptoken);
	int handlePeerStatusChanged(Token* ptoken);
	int handleExtensionStatusChanged(Token* ptoken);
	int handleDial(Token* ptoken);
};

}; 

#endif //__COMMUNICATION_HANDLER_H__
