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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
x#include "EPGGrid.h"

#include <iostream>
using namespace std;
using namespace DCE;
using namespace VDREPG;
#include "EPG.h"
#include "VDRMediaStream.h"
#include "VDRPlugin.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Text.h"

int EpgGrid::GetRows()
{
	if( !m_pEPG )
		return 0;
	return m_pEPG->m_listChannel.size();
}

int EpgGrid::GetCols()
{
	if( !m_pEPG )
		return 0;
	return m_pEPG->m_tTime_Last / 60 / m_iGridResolution;
}

void EpgGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EpgGrid::ToData");
	ClearData();
	PLUTO_SAFETY_LOCK(vm,m_pVDRPlugin->m_VDRMutex);
	m_pEPG = m_pVDRPlugin->m_mapEPG_Find(m_pVDRMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
	if( !m_pEPG )
	{
		Size=0; Data=NULL;
		return;  // Nothing we can do
	}
	if( !(*ColStart) || (m_pVDRMediaStream && !m_pVDRMediaStream->m_mapOrbiter_HasInitialPosition[m_dwPK_Device_Orbiter]) )
	{
		m_pVDRMediaStream->m_mapOrbiter_HasInitialPosition[m_dwPK_Device_Orbiter]=true;
		*ColStart = GetCurrentColumn();
	}

	// Start with the the current time at the top of the hour
	time_t tNow = *ColStart * 60 * m_iGridResolution;
	struct tm *tmptr = localtime(&tNow);
	tmptr->tm_min=0;
	time_t tStartOfHour = mktime(tmptr);
	tNow = time(NULL);
	tmptr = localtime(&tNow);
	struct tm tmNow = *tmptr;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EpgGrid::ToData COLSTART %d tStartOfHour %d (%d %d)",*ColStart,tStartOfHour,m_pEPG->m_listChannel.size(),m_pEPG->m_mapEvent.size());

	// Put a new column heading ever 30 minutes
	for(int iColHeading = tStartOfHour / 60 / m_iGridResolution; iColHeading<*ColStart + ColCount;iColHeading += 30 / m_iGridResolution )
	{
		time_t tTime_Column = iColHeading * 60 * m_iGridResolution;
		tmptr = localtime(&tTime_Column);
		string sDesc;
		if( tmNow.tm_mon != tmptr->tm_mon || tmNow.tm_mday != tmptr->tm_mday )
			sDesc = StringUtils::itos(tmptr->tm_mon+1) + "/" + StringUtils::itos(tmptr->tm_mday) + " ";
		sDesc += StringUtils::itos(tmptr->tm_hour) + ":" + (tmptr->tm_min<10 ? "0" : "") +
			StringUtils::itos(tmptr->tm_min);
		DataGridCell *pCell = new DataGridCell(sDesc);
		pCell->m_Colspan = 30 / m_iGridResolution;
		SetData(iColHeading,0,pCell);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EpgGrid::ToData RowStart %d RowCount %d",*RowStart,RowCount);

	for(int iRow=*RowStart;iRow<*RowStart + RowCount;++iRow)
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"in loop irow %d < %d size %d",iRow,(int) (*RowStart + RowCount), (int) m_pEPG->m_vectChannel.size());
		if( iRow>=0 && iRow<m_pEPG->m_vectChannel.size() )
		{
			Channel *pChannel = m_pEPG->m_vectChannel[iRow];
			// Use iRow+1 since the first row we added already for the header with the times
			PopulateRow(pChannel,iRow+1,*ColStart * 60 * m_iGridResolution,(*ColStart + ColCount) * 60 * m_iGridResolution );
			DataGridCell *pCell = new DataGridCell(pChannel->m_sChannelName);
			if( pChannel->m_pImage && pChannel->m_sizeImage )
			{
				char *pBuffer = new char[pChannel->m_sizeImage];
				memcpy(pBuffer,pChannel->m_pImage,pChannel->m_sizeImage);
				pCell->m_pGraphicData = pChannel->m_pImage;
				pCell->m_GraphicFormat = GR_JPG;
				pCell->m_GraphicLength = pChannel->m_sizeImage;
			}
			SetData(0,iRow+1,pCell);
		}
	}
	m_TotalRows=GetRows();
	m_TotalColumns=GetCols();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EpgGrid::ToData m_TotalRows %d m_TotalColumns %d",m_TotalRows,m_TotalColumns);

	DataGridTable::ToData(GridID, Size, Data, ColStart, RowStart, ColCount, RowCount);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EpgGrid::ToData done");
	m_pEPG=NULL;  // Once the mutex goes out of scope it's no longer valid
}

void EpgGrid::PopulateRow(Channel *pChannel,int iRow,int StartTime,int StopTime)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"EpgGrid::PopulateRow %s",pChannel->m_sChannelName.c_str());
	Event *pEvent = pChannel->m_pEvent_First,*pEvent_Prior=NULL;
	while(pEvent && pEvent->m_tStartTime<StartTime)
	{
		pEvent_Prior = pEvent;
		pEvent = pEvent->m_pEvent_Next;
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"pevent %p p event prior %p",pEvent,pEvent_Prior);

	// The prior show is partially in this grid
	if( pEvent_Prior && pEvent_Prior->m_tStopTime > StartTime )
	{
		DataGridCell *pCell = new DataGridCell("<" + pEvent_Prior->GetProgram(),"E" + StringUtils::itos(pEvent_Prior->m_EventID));
		pCell->m_Colspan = (pEvent_Prior->m_tStopTime - StartTime) / 60 / m_iGridResolution;
		if( pEvent_Prior->NowPlaying() )
			pCell->m_AltColor = -15126452;
		SetData(StartTime / 60 / m_iGridResolution,iRow,pCell);
	}

	// Add all current shows
	while(pEvent && pEvent->m_tStartTime<StopTime)
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"pevent %p p event prior %p  st %d  stop %d",pEvent,pEvent_Prior,(int) pEvent->m_tStartTime,(int) StopTime);
		string sDesc = pEvent->GetProgram();
		if( pEvent->m_tStopTime > StopTime )
			sDesc += ">";
		DataGridCell *pCell = new DataGridCell(sDesc,"E" + StringUtils::itos(pEvent->m_EventID));
		int Column = pEvent->m_tStartTime / 60 / m_iGridResolution;
		// When shows don't start/stop at even times, rounding times cause gaps.  So if this show
		// stops at the same time as the next starts, calculate the column width that way we there are no gaps
		if( pEvent->m_pEvent_Next && pEvent->m_pEvent_Next->m_tStartTime == pEvent->m_tStopTime )
			pCell->m_Colspan = pEvent->m_pEvent_Next->m_tStartTime / 60 / m_iGridResolution - Column;
		else
			pCell->m_Colspan = (pEvent->m_tStopTime - pEvent->m_tStartTime) / 60 / m_iGridResolution;
		if( pEvent->NowPlaying() )
			pCell->m_AltColor = -15126452;

		SetData(Column,iRow,pCell);
		if( pEvent->m_pEvent_Next && pEvent->m_pEvent_Next->m_tStartTime < pEvent->m_tStartTime )  // We're looping at the end
		{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"hit the end");
			break;

		}
		pEvent = pEvent->m_pEvent_Next;
	}
}

int EpgGrid::GetCurrentColumn()
{
	return (time(NULL)-900) / 60 / m_iGridResolution; // Start 15 mins in the past so it's easier to see what shows are ending
}
