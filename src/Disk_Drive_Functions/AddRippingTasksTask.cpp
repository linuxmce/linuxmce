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

#include "AddRippingTasksTask.h"
#include "RipJob.h"
#include "Disk_Drive_Functions.h"

using namespace nsJobHandler;

int AddRippingTasksTask::Run()
{
	if( m_pJob->GetType()=="RipJob" )  // always true
	{
		RipJob *pRipJob = (RipJob *) m_pJob;
		pRipJob->AddRippingTasks(m_pDrive, RipJob::position_TasklistBegin);
		m_eTaskStatus_set(TASK_COMPLETED);
	}
	else
		m_eTaskStatus_set(TASK_COMPLETED);
	return 0;
}
