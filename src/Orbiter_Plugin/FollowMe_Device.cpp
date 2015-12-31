/*
 Orbiter_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "FollowMe_Device.h"
#include "Orbiter_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DeviceData_Router.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"

bool FollowMe_Device::HandleFollowMe( class Message *pMessage )
{
	int iPK_Orbiter = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Orbiter_CONST].c_str());
	int iPK_Device = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
	int iPK_Users = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Users_CONST].c_str());
	int iPK_RoomOrEntArea = atoi(pMessage->m_mapParameters[pMessage->m_dwID==EVENT_Follow_Me_Media_CONST ? EVENTPARAMETER_PK_EntArea_CONST : EVENTPARAMETER_PK_Room_CONST].c_str());
	int iPK_RoomOrEntArea_Left = atoi(pMessage->m_mapParameters[pMessage->m_dwID==EVENT_Follow_Me_Media_CONST ? EVENTPARAMETER_PK_EntArea_Left_CONST : EVENTPARAMETER_PK_Room_Left_CONST].c_str());

LoggerWrapper::GetInstance()->Write(LV_WARNING,"Handle follow me type %d orb %d user %d r/e %d r/e left %d",
					  pMessage->m_dwID,iPK_Orbiter,iPK_Users,iPK_RoomOrEntArea,iPK_RoomOrEntArea_Left);

	if( iPK_RoomOrEntArea_Left )
		FollowMe_LeftRoom(pMessage->m_dwID, iPK_Orbiter, iPK_Device, iPK_Users, iPK_RoomOrEntArea, iPK_RoomOrEntArea_Left);
	if( iPK_RoomOrEntArea )  // Some things, like media, climate, etc. can only have 1 thing going.  Others like Telecom don't have that restriction
		FollowMe_EnteredRoom(pMessage->m_dwID, iPK_Orbiter, iPK_Device, iPK_Users, iPK_RoomOrEntArea, iPK_RoomOrEntArea_Left);

	return false; // Let other devices handle this too
}

void FollowMe_Device::CancelPendingMovesByType(class OH_Orbiter *pOH_Orbiter)
{
}

