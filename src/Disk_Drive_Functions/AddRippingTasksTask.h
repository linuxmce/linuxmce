/*
 Main

 Copyright (C) 2008 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#ifndef AddRippingTasksTask_h
#define AddRippingTasksTask_h

#include "JobHandler/Task.h"
#include "RipJob.h"

using namespace nsJukeBox;

namespace nsJobHandler
{
	class AddRippingTasksTask : public Task
	{
		Drive *m_pDrive;
	public:
		AddRippingTasksTask(class Job *pJob,Drive *pDrive,string sName)
			: Task(pJob,sName), m_pDrive(pDrive)
		{
		}

		virtual ~AddRippingTasksTask() {}
		virtual string GetType() { return "AddRippingTasksTask"; }

		virtual int Run();  // Return 0 if the task is done, or a number of milliseconds if you want Run to be called again in that many ms
	};
};

#endif
