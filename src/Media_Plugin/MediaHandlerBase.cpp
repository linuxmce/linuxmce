/*
 Media_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "MediaHandlerBase.h"
#include "Media_Plugin.h"

#include <iostream>
using namespace std;
using namespace DCE;

void MediaHandlerBase::GetRenderDevices(EntertainArea *pEntertainArea, map<int,MediaDevice *> *pmapMediaDevice)
{
    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Getting Render Devices with ea %p map %p",pEntertainArea, pmapMediaDevice);
	if( pEntertainArea->m_pMediaDevice_ActiveDest && pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, " Setting the device data source: to %d (%s)",  pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device, pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_sDescription.c_str());
		(*pmapMediaDevice)[pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device] = pEntertainArea->m_pMediaDevice_ActiveDest;
	}
}

void MediaHandlerBase:: GetRenderDevices(map<int, MediaDevice *> *pmapMediaDevices)
{
	for(map<int, class EntertainArea *>::iterator it=m_pMedia_Plugin->m_mapEntertainAreas.begin();it!=m_pMedia_Plugin->m_mapEntertainAreas.end();++it)
	{
		EntertainArea *pEntertainArea = it->second;
		if( pEntertainArea->m_pMediaStream && 
				((void *) pEntertainArea->m_pMediaStream->m_pMediaHandlerInfo->m_pCommand_Impl)== ((void *) this) && 
				pEntertainArea->m_pMediaDevice_ActiveDest )
			(*pmapMediaDevices)[ pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router->m_dwPK_Device ] = pEntertainArea->m_pMediaDevice_ActiveDest;
	}
}

bool MediaHandlerBase::StartMedia(class MediaStream *pMediaStream,string &sError)
{
	return true;
}

bool MediaHandlerBase::StopMedia(class MediaStream *pMediaStream)
{
	return true;
}

