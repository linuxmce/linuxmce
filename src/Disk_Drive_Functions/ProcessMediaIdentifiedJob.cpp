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

#include "ProcessMediaIdentifiedJob.h"
#include "JukeBox.h"
#include "DCE/Logger.h"
#include "DCE/Command_Impl.h"
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"

using namespace nsJobHandler;
using namespace DCE;

#include "../Gen_Devices/AllCommandsRequests.h"
#include "../pluto_main/Define_DeviceTemplate.h"

ProcessMediaIdentifiedJob::ProcessMediaIdentifiedJob(JobHandler *pJobHandler,int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc, JukeBox *pJukeBox, Disk_Drive_Functions *pDisk_Drive_Functions)
	: Job(pJobHandler,"ProcessMediaIdentifiedJob",0,pDisk_Drive_Functions ? pDisk_Drive_Functions->m_pCommand_Impl_get() : NULL)
{
	m_iPK_Device=iPK_Device;
	m_sValue_To_Assign=sValue_To_Assign;
	m_sID=sID;
	m_pData = NULL;
	m_iData_Size = 0;
	if( pData && iData_Size )
	{
		m_iData_Size=iData_Size;
		m_pData = new char[iData_Size];
		memcpy(m_pData,pData,m_iData_Size);
	}
	m_sFormat=sFormat;
	m_iPK_MediaType=iPK_MediaType;
	m_sMediaURL=sMediaURL;
	m_sURL=sURL;
	m_iEK_Disc=*iEK_Disc;
	m_pJukeBox=pJukeBox;
	m_pDisk_Drive_Functions=pDisk_Drive_Functions;
}

void ProcessMediaIdentifiedJob::Run()
{
	if( m_pJukeBox )
		m_pJukeBox->Media_Identified(m_iPK_Device,m_sValue_To_Assign,m_sID,m_pData,m_iData_Size,m_sFormat,m_iPK_MediaType,m_sMediaURL,m_sURL,&m_iEK_Disc);
	else if( m_pCommand_Impl )
	{
		DCE::CMD_Media_Identified_DT CMD_Media_Identified_DT(m_pCommand_Impl->m_dwPK_Device,DEVICETEMPLATE_Media_Plugin_CONST,
			BL_SameHouse,m_pCommand_Impl->m_dwPK_Device,m_sValue_To_Assign,m_sID,m_pData,m_iData_Size,m_sFormat,m_iPK_MediaType,m_sMediaURL,m_sURL,&m_iEK_Disc);
		m_pCommand_Impl->SendCommand(CMD_Media_Identified_DT);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessMediaIdentifiedJob::Run disc is %d",m_iEK_Disc);
		if( m_iEK_Disc )
		{
			char cMediaType='M'; // The default
			if( m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
				cMediaType='c';
			else if( m_iPK_MediaType==MEDIATYPE_pluto_DVD_CONST )
				cMediaType='d';
			m_pDisk_Drive_Functions->UpdateDiscLocation(cMediaType,m_iEK_Disc);
		}
	}
	m_eJobStatus=job_Done;
	m_pJobHandler->BroadcastCond();
}
