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

#ifndef DPS_MainJSONData_h
#define DPS_MainJSONData_h

#include "../Data_Provider_Catalog_Plugin/Data_Provider_CallBack.h"

using namespace std;

namespace DCE
{
	class JSON_Provider_InstantiationInfo;

	class DPS_MainJSONData_Devices : public Data_Provider_Source_Base
	{
		JSON_Provider_InstantiationInfo *m_pJSON_Provider_InstantiationInfo;

	public:
		DPS_MainJSONData_Devices(JSON_Provider_InstantiationInfo *pJSON_Provider_InstantiationInfo) : Data_Provider_Source_Base()
		{
			m_pJSON_Provider_InstantiationInfo=pJSON_Provider_InstantiationInfo;
		}

		static const char *DataID() { return "Devices"; }
		static Data_Provider_Source_Base *Instantiate(Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,string sID)
		{ 
			return new DPS_MainJSONData_Devices( (JSON_Provider_InstantiationInfo *) pData_Provider_InstantiationInfo);
		}

		virtual bool Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String);
	};
}

#endif
