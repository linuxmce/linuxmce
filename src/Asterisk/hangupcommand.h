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
// C++ Interface: originatestatemachine
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKHANGUPSTATEMACHINE_H
#define ASTERISKHANGUPSTATEMACHINE_H

#include "asteriskcommand.h"

#include <string>

namespace ASTERISK {

/**
@author 
*/
class HangupCommand : public IAsteriskCommand
{
public:
	HangupCommand();
	~HangupCommand();

public:
	void setChannel(std::string channel);

	void handleStartup();
	void handleTerminate();
	bool handleResponse(Token &token);

private:
	std::string channel;

private:
};

};

#endif
