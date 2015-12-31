/*
 MediaHandlerInfo

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "EntertainArea.h"
#include "Media_Plugin.h"

#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

void BoundRemote::UpdateOrbiter( MediaStream *pMediaStream, bool bRefreshScreen, Message **p_pMessage )
{
    // Log a big warning on the log file. This should not crash the router but it is an error to call UpdateOrbiter with a NULL media Stream.
    if ( pMediaStream == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "BoundRemote::UpdateOrbiter was called with a NULL pointer!. Ignoring request!");
        return;
    }

	LoggerWrapper::GetInstance()->Write(LV_WARNING,
			"BoundRemote::UpdateOrbiter(): Updating object: %s on device %d with size of %d",
				m_sPK_DesignObj_GraphicImage.c_str(),
				m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
				pMediaStream->m_iPictureSize_get() );

    // TODO -- Figure out the media information, like track, timecode, picture, etc. For now just update the text object. Also need to update the pictures
//  size_t size; char *pPic = FileUtils::ReadFileIntoBuffer( "/image.jpg", size );
	Message *pMessage_Out=NULL;
	if( m_sPK_DesignObj_GraphicImage.empty()==false )
	{
		DCE::CMD_Update_Object_Image CMD_Update_Object_Image( 0, m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, m_sPK_DesignObj_GraphicImage, "jpg", pMediaStream->m_pPictureData_get(), pMediaStream->m_iPictureSize_get(), "0" );
		pMessage_Out = CMD_Update_Object_Image.m_pMessage;
	}

    if( m_iPK_Text_Synopsis )
    {
        DCE::CMD_Set_Text CMD_Set_Text( m_pMedia_Plugin->m_dwPK_Device, m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, "",
            ( pMediaStream ? pMediaStream->m_sMediaSynopsis : "* no media *" ), m_iPK_Text_Synopsis );

		if( pMessage_Out )
			pMessage_Out->m_vectExtraMessages.push_back( CMD_Set_Text.m_pMessage );
		else
			pMessage_Out = CMD_Set_Text.m_pMessage;
    }

	pMediaStream->SetNowPlaying( m_pOH_Orbiter, bRefreshScreen, false, pMessage_Out );

	if( p_pMessage )
		*p_pMessage = pMessage_Out;
	else if( pMessage_Out )
		m_pMedia_Plugin->QueueMessageToRouter( pMessage_Out );
}
