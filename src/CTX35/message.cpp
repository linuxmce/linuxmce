/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: message
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "message.h"
#include <cstdlib>
#include <stdio.h>

namespace CTX35DEV {

unsigned char Message::s_MapHouseDeviceCodes[] =  {
	0x06, //A
	0x0E, //B
	0x02, //C
	0x0A, //D
	0x01, //E
	0x09, //F
	0x05, //G
	0x0D, //H
	0x07, //I
	0x0F, //J
	0x03, //K
	0x0B, //L
	0x00, //M
	0x08, //N
	0x04, //O
	0x0C  //P
};

Message::Message() 
	: houseCode_(0), deviceCode_(0), functionCode_(0), dimmLevel_(0)
{
}

Message::~Message() {
}


void 
Message::setAddress(std::string address) {// adress in the form of A1, B2,...
	int len = address.length();
	if(len < 2 || len > 3) {
		return;
	}
	
	if(address[0] >= 'A' && address[0] <= 'P') {
		houseCode_ = s_MapHouseDeviceCodes[address[0] - 'A'];
	}
	
	int devcode = atoi(address.substr(1, len - 1).c_str());
	if(devcode >= 1 && devcode <= 16) {
		deviceCode_ = s_MapHouseDeviceCodes[devcode - 1];
	}
}

char* Message::getHouseCode(unsigned char x10addr)
{
	char house_code=(x10addr >> 4) & 0x0F;
	int i;
	static char buffer[2];

	for(i=0;i<16;i++)
	{
		if(house_code == s_MapHouseDeviceCodes[i])
		{
			sprintf(buffer, "%c", 'A'+i);
			break;
		}
	}
	return buffer;
}

char* Message::getAddress(unsigned char x10addr)
{
	char house_code=(x10addr >> 4) & 0x0F;
	char unit_code=x10addr & 0x0F;
	static char buffer[4];
	int i;
	for(i=0;i<16;i++)
	{
		if(house_code == s_MapHouseDeviceCodes[i])
		{
			house_code='A'+i;
			break;
		}
	}
	for(i=0;i<16;i++)
	{
		if(unit_code == s_MapHouseDeviceCodes[i])
		{
			unit_code=1+i;
			break;
		}
	}
	sprintf(buffer,"%c%d",house_code,unit_code);
	return buffer;
}

};
