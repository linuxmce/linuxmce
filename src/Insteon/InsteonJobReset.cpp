/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "InsteonJobReset.h"
#include "LMCEInsteonAPI.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class InsteonJobReset::Private
{
	public:
	
		Private();
		~Private();
	
		unsigned char callbackID;
		
	private:
};

InsteonJobReset::Private::Private()
	: callbackID(0)
{
}

InsteonJobReset::Private::~Private()
{
}

// ----------------------------------

InsteonJobReset::InsteonJobReset(LMCEInsteonAPI * lmceAPI)
	: InsteonJob(lmceAPI)
{
	d = new Private();
	setType(InsteonJob::RESET);
}

InsteonJobReset::~InsteonJobReset()
{
	delete d;
	d = NULL;
}

bool InsteonJobReset::run()
{
	handler()->RemoveLinks();
}

