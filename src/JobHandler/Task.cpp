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
#include "Task.h"
#include "Job.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

using namespace nsJobHandler;
using namespace DCE;

Task::Task(Job *pJob,string sName)
{
	m_sName = sName;
	m_pJob=pJob;
	m_eTaskStatus=TASK_NOT_STARTED;
	m_iID=m_pJob->m_NextTaskID++;
	m_tStarted=0;
	m_iPercent=0;
}

bool Task::Abort()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Task::Abort abort task %s", m_sName.c_str());
#endif
	if( m_eTaskStatus_get()==TASK_IN_PROGRESS || m_eTaskStatus_get()==TASK_NOT_STARTED)
		m_eTaskStatus_set(TASK_CANCELED);
	return true;
}

void Task::m_eTaskStatus_set(TaskStatus taskStatus) 
{
	if( m_eTaskStatus==TASK_NOT_STARTED && taskStatus==TASK_IN_PROGRESS )
		m_tStarted=time(NULL);

	m_pJob->RefreshOrbiter();

	m_eTaskStatus=taskStatus;
}
