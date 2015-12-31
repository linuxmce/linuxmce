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
#ifndef OH_Orbiter_h
#define OH_Orbiter_h

#include "OH_User.h"
#include "Orbiter/Floorplan.h"
#include "DCE/DeviceData_Router.h"
#include "pluto_main/Define_DeviceData.h"

#define NEED_VMC_TOKEN "NEED VMC"
#define NEED_APP_TOKEN "NEED APP"

namespace DCE
{
	// Store an orbiter list within the handler
	class OH_Orbiter
	{
	public:
		map<int,FloorplanObjectVectorMap *> m_mapFloorplanObjectVector;
		FloorplanObjectVectorMap *m_mapFloorplanObjectVector_Find(int Page) { map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.find(Page); return it==m_mapFloorplanObjectVector.end() ? NULL : (*it).second; }

		class DeviceData_Router *m_pDeviceData_Router;
		class DeviceData_Router *m_pDevice_CurrentDetected;

		OH_User *m_pOH_User;  // The current user
		class EntertainArea *m_pEntertainArea,*m_pEntertainArea_Locked;  // The current entertain area
		int m_dwPK_Room,m_dwPK_Room_Locked;  // The current room
		int m_dwPK_Skin;
		int m_dwPK_UI;
		int m_dwPK_Device_CurrentRemote;

		int m_iLastSignalStrength;
		int m_iFailedToConnectCount;
		bool m_bDisplayOn,m_bDontAutoShowRemote,m_bSendPopups,
			m_bFirstRegen; // True if the Orbiter hasn't completed setup yet
		time_t m_tRegistered;
        string m_sUpdateVMCFile;
        string m_sConfigFile;
		string m_sVersion;

		time_t m_tRegenTime,m_tSendAppTime;

		int PK_Users_get() { return m_pOH_User ? m_pOH_User->m_iPK_Users : 0; }  // Lots of places just want a 0 when there is no user

		OH_Orbiter(class DeviceData_Router *pDeviceData_Router)
		{
			m_bSendPopups=true;
			m_pDeviceData_Router = pDeviceData_Router;
			m_dwPK_Room = m_dwPK_Room_Locked = 0;
			m_pEntertainArea = m_pEntertainArea_Locked = NULL;
			m_iLastSignalStrength = 0;
			m_pDevice_CurrentDetected = NULL;
			m_iFailedToConnectCount = 0;
			m_bDisplayOn=false;
			m_tRegistered=0;
			m_tSendAppTime=m_tRegenTime=0;
			m_pOH_User=NULL;
			m_bDontAutoShowRemote=false;
			m_bFirstRegen=false;
			m_dwPK_Skin = atoi(m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PK_Skin_CONST).c_str());
			m_dwPK_UI = atoi(m_pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PK_UI_CONST).c_str());
			m_dwPK_Device_CurrentRemote=0;
		}

		~OH_Orbiter();

        bool NeedVMC();
        void NeedVMC(bool bTrue);
        bool NeedApp();
        void NeedApp(bool bTrue);

    protected:
        void SetState(bool bNeedVmc, bool bNeedApp);
	};
}

#endif
