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
// C++ Implementation: socket
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "token.h"
#include "asteriskconsts.h"

using namespace std;

namespace ASTERISK {

Token::Token() {
}


Token::~Token()
{
}

void Token::setKey(std::string key, std::string value) {
	tokmap[key] = value;
}

string& Token::getKey(std::string key) {
	return tokmap[key];
}

bool Token::hasKey(std::string key) {
	return tokmap.find(key) != tokmap.end();
}
	    
std::string Token::serialize() const {
	string ret;
	
	map<string, string>::const_iterator it = tokmap.begin();
	while(it != tokmap.end()) {

		ret += (*it).first;
		ret += ": ";
		ret += (*it).second;
		ret += "\x0D\x0A";
			
		it++;
	}
	ret += "\x0D\x0A";
	

	return ret;
}

void Token::removeAll() {
	tokmap.clear();
}

int Token::unserialize(std::string str) {
	removeAll();
	
	string key, value;
	bool bResponseFollows = false;
	bool bPriviledgeCommand = false;
	int keyend, valueend = 0;
	while(1) 
	{
		keyend = (int)str.find(": ", valueend);
		if(keyend != -1 && !(bResponseFollows && bPriviledgeCommand) )
		{
			keyend += 2;
			key = str.substr(valueend, keyend - valueend -2 /*dd*/);
			valueend = (int)str.find("\r\n", keyend);
			if(valueend != -1) {
				valueend += 2;
				value = str.substr(keyend, valueend - keyend  - 2/*eol*/);

				setKey(key, value);

				if(key == TOKEN_RESPONSE && value == TOKEN_FOLLOWS)
					bResponseFollows = true;
				else if(key == TOKEN_PRIVILEGE && value == TOKEN_COMMAND)
					bPriviledgeCommand = true;
			} 
			else 
			{
				break;
			}
		} 
		else 
		{
			setKey(TOKEN_DATA, str.substr(valueend));
			break;
		}
	}
			
	return 0;	
}

}
