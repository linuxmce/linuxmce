/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "XML_Data_Source.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Basic_XML_Data_Source_Plugin.h"
#include "DCERouter.h"
#include "DCERouter/IDataLayer.h"

using namespace DCE;
//#include "pluto_main/Table_Device.h"
//#include "pluto_main/Table_DeviceTemplate.h"
//#include "pluto_main/Table_DeviceCategory.h"
//#include "pluto_main/Table_Room.h"
//#include "pluto_main/Table_Array.h"
//#include "pluto_main/Table_CommandGroup.h"
//#include "pluto_main/Table_CommandGroup_Room.h"

#include <iostream>
using namespace std;
using namespace DCE;

XML_Data_InstantiationInfo_Basic::XML_Data_InstantiationInfo_Basic(Basic_XML_Data_Source_Plugin *pBasic_XML_Data_Source_Plugin)
{
	m_pBasic_XML_Data_Source_Plugin=pBasic_XML_Data_Source_Plugin;
	//m_pDatabase_pluto_main=m_pBasic_XML_Data_Source_Plugin->m_pDatabase_pluto_main_get();
}

/*virtual*/ int DeviceList::Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root)
{
	//TODO: implement me later if needed

	return 0;
}

/*virtual*/ int CommandGroupList::Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root)
{
	IDataLayer *pDataLayer = m_pXML_Data_InstantiationInfo_Basic->m_pBasic_XML_Data_Source_Plugin->m_pRouter->DataLayer();

	xmlNodePtr xmlNodePtr_commandgroup_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "CommandGroupList", BAD_CAST "");

	for(std::map<int, Scene_Data>::const_iterator it = pDataLayer->Scenes().begin(); it != pDataLayer->Scenes().end(); ++it)
	{
		const Scene_Data& scene = it->second;

		xmlNodePtr xmlNodePtr_commandgroup = xmlNewTextChild (xmlNodePtr_commandgroup_list, NULL, BAD_CAST "CommandGroup", BAD_CAST scene.Description().c_str());
		xmlNewProp (xmlNodePtr_commandgroup, BAD_CAST "PK_CommandGroup", BAD_CAST StringUtils::itos(scene.SceneID()).c_str());
		xmlNewProp (xmlNodePtr_commandgroup, BAD_CAST "PK_Array", BAD_CAST "1"); //WARNING, HARDCODED ARRAY ID

		xmlNodePtr xmlNodePtr_room = xmlNewTextChild (xmlNodePtr_commandgroup, NULL, BAD_CAST "Room", BAD_CAST "");
		xmlNewProp (xmlNodePtr_room, BAD_CAST "PK_Room", BAD_CAST StringUtils::ltos(scene.RoomID()).c_str());
	}

	xmlNodePtr xmlNodePtr_Array_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "ArrayList", BAD_CAST "");
	xmlNodePtr xmlNodePtr_Array = xmlNewTextChild (xmlNodePtr_Array_list, NULL, BAD_CAST "Array", BAD_CAST "Scenarios"); //WARNING, HARDCODED ARRAY DESCRIPTION
	xmlNewProp (xmlNodePtr_Array, BAD_CAST "PK_Array", BAD_CAST "1"); //WARNING, HARDCODED ARRAY ID

	return 0;
}
