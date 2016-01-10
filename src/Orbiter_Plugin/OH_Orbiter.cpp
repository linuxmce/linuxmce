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
#include "DeviceData_Router.h"
#include "OH_Orbiter.h"

using namespace DCE;

OH_Orbiter::~OH_Orbiter()
{
	for(map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.begin(),
		end = m_mapFloorplanObjectVector.end(); it != end; ++it)
	{
		FloorplanObjectVectorMap *pFloorplanObjectVectorMap = it->second;
		if(NULL == pFloorplanObjectVectorMap)
			continue;

		for(map<int,FloorplanObjectVector *>::iterator itm = pFloorplanObjectVectorMap->begin(), 
			endm = pFloorplanObjectVectorMap->end(); itm != endm; ++itm)
		{
			FloorplanObjectVector *pFloorplanObjectVector = itm->second;
			if(NULL == pFloorplanObjectVector)
				continue;

			for(vector<FloorplanObject *>::iterator itv = pFloorplanObjectVector->begin(),
				endv = pFloorplanObjectVector->end(); itv != endv; ++itv)
			{
				delete *itv;
			}
			pFloorplanObjectVector->clear();
			delete pFloorplanObjectVector;
		}

		pFloorplanObjectVectorMap->clear();
		delete pFloorplanObjectVectorMap;
	}
	m_mapFloorplanObjectVector.clear();
}

bool OH_Orbiter::NeedVMC()
{
    m_pDeviceData_Router->m_pRow_Device->Reload();
    return string::npos != m_pDeviceData_Router->m_sState_get().find(NEED_VMC_TOKEN);
}

void OH_Orbiter::NeedVMC(bool bTrue)
{
    if(NeedVMC() != bTrue) //need to change state
        SetState(bTrue, NeedApp());
}

bool OH_Orbiter::NeedApp()
{
    m_pDeviceData_Router->m_pRow_Device->Reload();
    return string::npos != m_pDeviceData_Router->m_sState_get().find(NEED_APP_TOKEN);
}

void OH_Orbiter::NeedApp(bool bTrue)
{
    if(NeedApp() != bTrue) //need to change state
        SetState(NeedVMC(), bTrue);
}

void OH_Orbiter::SetState(bool bNeedVmc, bool bNeedApp)
{
    m_pDeviceData_Router->m_sState_set(
        (bNeedVmc ? NEED_VMC_TOKEN + string(" | ") : string()) +
        (bNeedApp ? NEED_APP_TOKEN + string(" | ") : string())
    );
}