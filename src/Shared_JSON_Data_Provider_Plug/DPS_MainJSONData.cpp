/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "DPS_MainJSONData.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/DeviceData_Router.h"
#include "Shared_JSON_Data_Provider_Plug.h"
#include "DCE/DCERouter.h"

#include <iostream>
using namespace std;
using namespace DCE;

bool DPS_MainJSONData_Devices::Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String)
{
	for(
		map<int, DeviceData_Router *>::iterator it_device = m_pJSON_Provider_InstantiationInfo->m_pRouter->DataLayer()->Devices().begin(); 
		it_device != m_pJSON_Provider_InstantiationInfo->m_pRouter->DataLayer()->Devices().end(); ++it_device
    )
	{
		DeviceData_Router *pDeviceData_Router = it_device->second;

		*sData_String += pDeviceData_Router->m_sDescription + "\t";
	}

	return true;
}
