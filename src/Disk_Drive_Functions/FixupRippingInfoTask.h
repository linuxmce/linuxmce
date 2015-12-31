/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#ifndef FixupRippingInfoTask_h
#define FixupRippingInfoTask_h

#include "JobHandler/Task.h"
#include "RipJob.h"

using namespace nsJukeBox;

namespace nsJobHandler
{
	class FixupRippingInfoTask : public Task
	{
	public:
		FixupRippingInfoTask(class Job *pJob,string sName)
			: Task(pJob,sName)
		{
		}

		virtual ~FixupRippingInfoTask() {}
		virtual string GetType() { return "FixupRippingInfoTask"; }

		virtual int Run();  // Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms
	};
};

#endif
