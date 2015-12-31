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
// C++ Implementation: utils
//
// Description: 
//
//
// Authors:  <igor@dexx>, Cristian Miron, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "utils.h"

namespace ASTERISK {

int 
Utils::ParseChannel(const std::string channel,
		std::string* psphone, std::string* psphonetype,	std::string* psid) 
{
	int pos, oldpos = 0;
	
	pos = (int)channel.find('/');
	if(pos < 0) {
		return -1;
	}
	if(psphonetype) {
		*psphonetype = channel.substr(0, pos);
	}
	
	oldpos = pos + 1;
	pos = (int)channel.find('@',oldpos);
	if(pos < 0) {
		pos = (int)channel.find('-',oldpos);
	}
	if(pos < 0) {
		pos = (int)channel.find('/',oldpos);
	}
	if(pos < 0) {
		pos = (int)channel.find('&',oldpos);
	}
	if(pos < 0) {
		pos = (int)channel.find(',',oldpos);
	}
	if(pos < 0) {
		pos = (int)channel.find('|',oldpos);
	}
	if(pos < 0) {
		return -1;
	}
	
	if(psphone) {
		*psphone = channel.substr(oldpos, pos - oldpos);
	}
	
	if(psid) {
		*psid = channel.substr(pos, channel.length() - pos);
	}
	
	return 0;
}

int 
Utils::ParseCallerID(const std::string callerid,
		std::string* psphonenum, std::string* pscallername)
{
	int pos, oldpos = 0;
	
	pos = (int)callerid.find('<');
	if(pos < 0) {
		return -1;
	}
	if(pscallername) {
		*pscallername = callerid.substr(0, pos - 1);
	}
	
	oldpos = pos;
	pos = (int)callerid.find('>', oldpos);
	if(pos < 0) {
		return -1;
	}

	if(psphonenum) {
		*psphonenum = callerid.substr(oldpos + 1, pos - oldpos - 1);
	}
	
	return 0;
}

int 
Utils::ParseParty(const std::string party, std::string* extension, std::string *rest) 
{
	int pos, oldpos = 0;
	
	pos = (int)party.find('/');
	if(pos < 0) {
		return -1;
	}
	
	oldpos = pos + 1;
	pos = (int)party.find('@',oldpos);	
	if(pos < 0) {
		pos = (int)party.find('&',oldpos);	
	}
	if(pos < 0) {
		pos = (int)party.find(',',oldpos);	
	}
	if(pos < 0) {
		pos = (int)party.find('|',oldpos);
	}
	if(pos < 0) {
		pos = (int)party.find('/',oldpos);
	}
	if(pos < 0) {
		pos = (int)party.length();
	}
	
	if(extension) {
		*extension = party.substr(oldpos, pos - oldpos);
	}
	if(rest)
	{
		pos = (int)party.find('&',oldpos);
		if(pos > 0)
		{
			*rest = party.substr(pos+1,party.length());
		}
		else
		{
			*rest = "";
		}
	}
	return 0;
}


};
