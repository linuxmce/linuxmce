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
#include "ScreenMap.h"
#include "PlutoUtils/CommonIncludes.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Screen.h"
#include "pluto_main/Table_Screen_DesignObj.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_Skin.h"
#include "pluto_main/Table_DeviceTemplate.h"

void PopulateScreenMap(Database_pluto_main *pDatabase_pluto_main, map<int,int>& mapDesignObj,
					   Row_UI *pRow_UI, Row_Skin *pRow_Skin, Row_Device *pRow_Device,
					   bool bMatchingSkin)
{
	string SQL="(FK_UI=" + StringUtils::itos(pRow_UI->PK_UI_get()) + " OR FK_UI IS NULL) AND "
		"(FK_Skin=" + StringUtils::itos(pRow_Skin->PK_Skin_get()) + " OR FK_Skin IS NULL) AND "
		"(FK_DeviceTemplate IN ("
			"SELECT PK_DeviceTemplate FROM DeviceTemplate_DesignObj "
			"JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_DesignObj.FK_DeviceTemplate "
			"JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
			"WHERE PK_Devicetemplate=" + StringUtils::itos(pRow_Device->FK_DeviceTemplate_get()) + " OR "
			"PK_DeviceTemplate<>" + StringUtils::itos(DEVICETEMPLATE_Orbiter_CONST) + ") "
		"OR FK_DeviceTemplate IS NULL) "
		"ORDER BY FK_Screen,FK_DeviceTemplate DESC, FK_UI DESC, FK_Skin DESC";

	vector<Row_Screen_DesignObj *> vectRow_Screen_DesignObj;
	pDatabase_pluto_main->Screen_DesignObj_get()->GetRows(SQL,&vectRow_Screen_DesignObj);

	int PK_Screen_Last=0;  // Keep track of the last one, we only 1 want design obj per screen, the first one
	for(size_t s=0;s<vectRow_Screen_DesignObj.size();++s)
	{
		Row_Screen_DesignObj *pRow_Screen_DesignObj = vectRow_Screen_DesignObj[s];
		if( pRow_Screen_DesignObj->FK_Screen_get()==PK_Screen_Last )
			continue;

		if( bMatchingSkin && pRow_Skin->PK_Skin_get()!=pRow_Screen_DesignObj->FK_Skin_get() )
			continue;

		PK_Screen_Last=pRow_Screen_DesignObj->FK_Screen_get();
		mapDesignObj[PK_Screen_Last] = pRow_Screen_DesignObj->FK_DesignObj_get();
	}
}
