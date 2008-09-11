/*
     Copyright (C) 2007 Peter Kalogiannis, <pkalogiannis@gmail.com>

     This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the Free
     Software Foundation; either version 2 of the License, or (at your option)
     any later version.

     This program is distributed in the hope that it will be useful, but
     WITHOUT ANY WARRANTY; without even the implied warranty of 
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <stdio.h>
#include "InsteonJobSwitchChangeLevel.h"
#include "LMCEInsteonAPI.h"
#include "main.h"

#define DATA_LEN 3

class InsteonJobSwitchChangeLevel::Private
{
public:

	/**it is possible to change the level for a list of nodes*/
	Private(unsigned char level, unsigned char* nodeID);
	unsigned char level;
	unsigned char* nodeID;
	unsigned char callbackID;
};

InsteonJobSwitchChangeLevel::Private::Private(unsigned char l, unsigned char* nID)
	: level(l),
	  nodeID(nID),
	  callbackID(0)
{
}

InsteonJobSwitchChangeLevel::InsteonJobSwitchChangeLevel(LMCEInsteonAPI * insteonAPI, unsigned char level, char* nodeID)
	: InsteonJobSendData(insteonAPI)
{
	unsigned char* conv_string = str_id_to_hex(nodeID);
	
	d = new Private(level, conv_string);
	setType(InsteonJob::SET_SWITCH_LEVEL);
//	setReceivingTimeout( 3 );
	logID_ = "InsteonJobSwitchChangeLevel: ";
}

InsteonJobSwitchChangeLevel::~InsteonJobSwitchChangeLevel(void)
{
	delete d;
	d = NULL;
}

bool InsteonJobSwitchChangeLevel::run()
{
	/*ZWaveNode * node = handler()->getNode(d->nodeID);
	if( node == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "ZWJobSwitchChangeLevel : node null" );
		return false;
	}
	NodeType type = node->type();
	*/
	time_t currentTime = time(NULL);
	if( !again() )
	{
//		setStartTime( currentTime );
	}
	else
	{
		// retry but it will be enough to get the short answer
		// ask Eugen before uncommenting the next line
//		onlyShortAnswer_ = true;
	}
//	setAnswerTime( currentTime );
	
	setState(InsteonJob::RUNNING);
	
	size_t len = 9;
	unsigned char data[9];
	
	data[0] = 0x11;
	data[1] = d->level;
	data[2] = d->nodeID[0];
	data[3] = d->nodeID[1];
	data[4] = d->nodeID[2];
	
	
	return handler()->sendData(data, len);
}

void InsteonJobSwitchChangeLevel::timeoutHandler()
{
}
