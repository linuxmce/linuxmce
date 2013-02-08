/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/


//
// C++ Interface: LMCEInsteonAPI
//
//


#ifndef INSTEONJOBSWITCHCHANGELEVEL_H_
#define INSTEONJOBSWITCHCHANGELEVEL_H_



#include "InsteonJobSendData.h"

/**change the switch level (the same for dimmer)*/
class InsteonJobSwitchChangeLevel :
	public InsteonJobSendData
{
	public:
	
		InsteonJobSwitchChangeLevel(LMCEInsteonAPI * insteonAPI, unsigned char level, char* nodeID);
		
		virtual ~InsteonJobSwitchChangeLevel();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		virtual void timeoutHandler();
		
	private:
	
		class Private;
		Private *d;
};

#endif /*INSTEONJOBSWITCHCHANGELEVEL_H_*/
