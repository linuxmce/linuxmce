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

#include "FixupRippingInfoTask.h"
#include "RipJob.h"
#include "Disk_Drive_Functions.h"

using namespace nsJobHandler;

int FixupRippingInfoTask::Run()
{
	if( m_pJob->GetType()=="RipJob" )  // always true
	{
		RipJob *pRipJob = (RipJob *) m_pJob;
		if( pRipJob->m_pDisk_Drive_Functions )
		{
			if (pRipJob->m_pDisk_Drive_Functions->FixupRippingInfo(pRipJob->m_pDisk_Drive_Functions,pRipJob->m_iPK_MediaType,pRipJob->m_sFileName,pRipJob->m_sTracks,pRipJob->m_iEK_Disc,pRipJob->m_sDirectory))
			    m_eTaskStatus_set(TASK_COMPLETED);
			else
			    m_eTaskStatus_set(TASK_FAILED_CONTINUE);
		}
		else
		    m_eTaskStatus_set(TASK_COMPLETED);

	}
	else
		m_eTaskStatus_set(TASK_COMPLETED);
	return 0;
}
