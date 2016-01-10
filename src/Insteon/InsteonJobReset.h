/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef INSTEONJOBRESET_H_
#define INSTEONJOBRESET_H_



#include "InsteonJob.h"

/**reset job*/
class InsteonJobReset :	public InsteonJob
{
	public :
	
		InsteonJobReset(LMCEInsteonAPI*);
		
		virtual ~InsteonJobReset();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		
	private:
	
		// disable the default constructor
		InsteonJobReset();
		
		class Private;
		Private * d;
};

#endif /*INSTEONJOBRESET_H_*/
