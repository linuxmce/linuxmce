/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef INSTEONJOBEZRAINVALVECONTROL_H_
#define INSTEONJOBEZRAINVALVECONTROL_H_


#include "InsteonJobSendData.h"

/**Turn one of the 8 sprinkler valves on or off */
class InsteonJobEZRainValveControl :
	public InsteonJobSendData
{
	public:
	
		InsteonJobEZRainValveControl(LMCEInsteonAPI * insteonAPI, unsigned char valve, unsigned char valvestate, char* nodeID);
		
		virtual ~InsteonJobEZRainValveControl();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		virtual void timeoutHandler();
		
	private:
	
		class Private;
		Private *d;
};


#endif /*INSTEONJOBEZRAINVALVECONTROL_H_*/
