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
// C++ Interface: utils
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ASTERISKUTILS_H
#define ASTERISKUTILS_H

#include <string>

namespace ASTERISK {

/**
@author 
*/
class Utils{
public:
	static int ParseChannel(const std::string channel,
		std::string* psphone, std::string* psphonetype = 0,	std::string* psid = 0);
	static int ParseCallerID(const std::string callerid,
		std::string* psphonenume, std::string* pscallername = 0);
	static int ParseParty(const std::string party, std::string* extension, std::string* string = 0);
};

};

#endif
