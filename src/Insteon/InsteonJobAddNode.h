/*
     Copyright (C) 2007 Peter Kalogiannis

     www.linuxmce.com
     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef INSTEONJOBADDNODE_H_
#define INSTEONJOBADDNODE_H_


#include "InsteonJob.h"


class InsteonJobAddNode : public InsteonJob
{
	public:
	
		InsteonJobAddNode(LMCEInsteonAPI*, char* nodeID);
	
		virtual ~InsteonJobAddNode();
	
		/** It is called to perform the job.*/
		virtual bool run();
		
	private:
	
		// disable the default constructor
		InsteonJobAddNode();
		
		class Private;
		Private * d;

};
#endif /*INSTEONJOBADDNODE_H_*/

