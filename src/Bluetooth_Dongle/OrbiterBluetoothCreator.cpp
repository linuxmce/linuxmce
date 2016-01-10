/*
 StartOrbiterSDLBluetooth
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include <string>
using namespace std;

#include "OrbiterBluetoothCreator.h"
#include "OrbiterBluetooth.h"
extern Command_Impl *g_pCommand_Impl;

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
OrbiterBluetooth *CreateOrbiterBluetooth(
	class BDCommandProcessor *pBDCommandProcessor,
	int PK_Device, 
	int PK_DeviceTemplate,
	string sRouter_IP, 
	string sLocalDirectory, 
	bool bLocalMode, 
	int Width,
	int Height,
    pluto_pthread_mutex_t *pExternalScreenMutex,
	int PK_Room,
	int PK_EntertainArea
)
{
    OrbiterBluetooth * pOrbiterBluetooth = NULL;

    try
    {
	    pOrbiterBluetooth = new OrbiterBluetooth(
				    pBDCommandProcessor,
				    PK_Device, PK_DeviceTemplate, sRouter_IP, 
				    sLocalDirectory, bLocalMode, Width, Height, pExternalScreenMutex);

		g_pCommand_Impl = pOrbiterBluetooth;  

	    if (bLocalMode || (pOrbiterBluetooth->GetConfig() && pOrbiterBluetooth->Connect(0))) // Don't validate the device template
	    {
		    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
		    pOrbiterBluetooth->Initialize(gtSDLGraphic,PK_Room,PK_EntertainArea);
		    LoggerWrapper::GetInstance()->Write(LV_STATUS, "SDLGraphic initialized");
    		
		    if( !bLocalMode  )
			    pOrbiterBluetooth->CreateChildren();

	    }
    }
    catch(string s)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,s.c_str());
        pOrbiterBluetooth = NULL;
    }
    catch(const char *s)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, s);
        pOrbiterBluetooth = NULL;
    }
    catch(...)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter threw an unknown exception.");
        pOrbiterBluetooth = NULL;
    }

	return pOrbiterBluetooth;
}
//-----------------------------------------------------------------------------------------------------
