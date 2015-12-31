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

/**
*
* @file Generic_NonPluto_Media.h
* @brief source file for the Generic_NonPluto_Media class
* @author
*
*/

#include "Generic_NonPluto_Media.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "Gen_Devices/AllCommandsRequests.h"

Generic_NonPluto_Media::Generic_NonPluto_Media(Media_Plugin *pMedia_Plugin)
{
	m_pMedia_Plugin=pMedia_Plugin;
}

Generic_NonPluto_Media::~Generic_NonPluto_Media()
{

}


class MediaStream *Generic_NonPluto_Media::CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	if ( pMediaDevice == NULL  && vectEntertainArea.size() )
	{
		// Find a source device in this area
		EntertainArea *pEntertainArea = vectEntertainArea[0];
        for( map<int, class MediaDevice *>::iterator itDevice=pEntertainArea->m_mapMediaDevice.begin( );itDevice!=pEntertainArea->m_mapMediaDevice.end( );++itDevice )
        {
            class MediaDevice *pMediaDevice2 = ( *itDevice ).second;
			if( pMediaDevice2->m_mapMediaType.find( pMediaHandlerInfo->m_PK_MediaType )!=pMediaDevice2->m_mapMediaType.end() &&
				pMediaDevice2->m_iPK_MediaProvider==iPK_MediaProvider )
			{
				pMediaDevice=pMediaDevice2;
				break;
			}
		}
	}
	if ( pMediaDevice == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
		return NULL;
	}
/*
	if ( ! pMediaDevice && (pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea)) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
		return NULL;
	}
*/
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Selected device (%d) as playback device!", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device);

	MediaStream *pMediaStream
		= new MediaStream( pMediaHandlerInfo, iPK_MediaProvider,
							pMediaDevice, 
							iPK_Users, st_RemovableMedia, StreamID );

	return pMediaStream;
}

bool Generic_NonPluto_Media::StartMedia( class MediaStream *pMediaStream,string &sError )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Starting media stream playback--sending command, waiting for response" );

	return MediaHandlerBase::StartMedia(pMediaStream,sError);
}

bool Generic_NonPluto_Media::StopMedia( class MediaStream *pMediaStream )
{
	PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in Generic_NonPluto_Media!");

	return MediaHandlerBase::StopMedia(pMediaStream);
}

