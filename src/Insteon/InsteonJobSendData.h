/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/


#ifndef INSTEONJOBSENDDATA_H_
#define INSTEONJOBSENDDATA_H_


#include "InsteonJob.h"
#include <string>


class InsteonJobSendData : public InsteonJob
{
	public:
	
		InsteonJobSendData(LMCEInsteonAPI*);
	
		virtual ~InsteonJobSendData();
	
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char* buffer, size_t length);
		
	protected:
	
		std::string logID_;
		unsigned char callbackID_;
		bool onlyShortAnswer_;
		
	private:
	
		// disable the default constructor
		InsteonJobSendData();
};

#endif /*INSTEONJOBSENDDATA_H_*/
