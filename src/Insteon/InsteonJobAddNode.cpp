//
// C++ Implementation: InsteonAddNode
//
//   Copyright (C) 2007 Peter Kalogiannis, <pkalogiannis@gmail.com>
//
//   This program is free software; you can redistribute it and/or modify it
//   under the terms of the GNU General Public License as published by the Free
//   Software Foundation; either version 2 of the License, or (at your option)
//   any later version.
//
//   This program is distributed in the hope that it will be useful, but
//   WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License along
//   with this program; if not, write to the Free Software Foundation, Inc.,
//   59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//

#include "InsteonJobAddNode.h"
#include "LMCEInsteonAPI.h"
#include "main.h"


#include <stdio.h>
#include <string.h>


// ----------------------------------

class InsteonJobAddNode::Private
{
	public:
	
		Private(unsigned char* nID);
		~Private();
	
		JobsDeque jobsQueue;
		InsteonJob * currentJob;
		unsigned char* nodeID;
		
	private:
};

InsteonJobAddNode::Private::Private(unsigned char* nID)
	: currentJob(NULL),
	  nodeID(nID)
{
}

InsteonJobAddNode::Private::~Private()
{
	delete currentJob;
	currentJob = NULL;
	
	InsteonJob * job = NULL;
	for(JobsDequeIterator it=jobsQueue.begin(); it!=jobsQueue.end(); ++it)
	{
		job = (*it);
		delete job;
		job = NULL;
	}
}

// ----------------------------------

// ----------------------------------

InsteonJobAddNode::InsteonJobAddNode(LMCEInsteonAPI * lmceAPI, char* nodeID)
	: InsteonJob(lmceAPI)
{
	unsigned char* conv_string = str_id_to_hex(nodeID);
	d = new Private(conv_string);
	setType(InsteonJob::ADD_NODE);
}

InsteonJobAddNode::~InsteonJobAddNode()
{
	delete d;
	d = NULL;
}

bool InsteonJobAddNode::run()
{
	handler()->insertNode(d->nodeID, 1);
}


