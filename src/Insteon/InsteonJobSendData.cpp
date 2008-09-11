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

#include "InsteonJobSendData.h"
#include "LMCEInsteonAPI.h"
#include "main.h"
#include "InsteonJob.h"


InsteonJobSendData::InsteonJobSendData(LMCEInsteonAPI* insteonAPI)
	: InsteonJob(insteonAPI),
	  logID_("InsteonJobSendData: "),
	  callbackID_(0),
	  onlyShortAnswer_(false)
{
	setType(InsteonJob::SEND_DATA);
}

InsteonJobSendData::~InsteonJobSendData()
{
}

bool InsteonJobSendData::processData(const char* buffer, size_t length)
{
	
	//This needs to be filled in to perform data send
	switch(state())
	{
		default:
		case InsteonJob::IDLE:
		case InsteonJob::STOPPED:
			return true;		
	}
	
	//return handler()->processData(buffer, length);
	return true;
}
