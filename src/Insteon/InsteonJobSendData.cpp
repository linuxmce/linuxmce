/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

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
