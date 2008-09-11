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

