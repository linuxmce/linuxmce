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
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Room.h"

#include <iostream>
using namespace std;
using namespace DCE;

XML_Data_InstantiationInfo_Basic::XML_Data_InstantiationInfo_Basic(Basic_XML_Data_Source_Plugin *pBasic_XML_Data_Source_Plugin)
{
	m_pBasic_XML_Data_Source_Plugin=pBasic_XML_Data_Source_Plugin;
	m_pDatabase_pluto_main=m_pBasic_XML_Data_Source_Plugin->m_pDatabase_pluto_main_get();
}

/*virtual*/ int DeviceList::Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root)
{
	// As we go through the devices, keep track of the rooms, device templates, and device categories we reference so we can include them
	map<int,Row_Room *> m_mapRow_Room;
	map<int,Row_DeviceTemplate *> m_mapRow_DeviceTemplate;
	map<int,Row_DeviceCategory *> m_mapRow_DeviceCategory;

	xmlNodePtr xmlNodePtr_device_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "DeviceList", BAD_CAST "");

	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("1=1",&vectRow_Device);
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		Row_Device *pRow_Device = *it;
		xmlNodePtr xmlNodePtr_device = xmlNewTextChild (xmlNodePtr_device_list, NULL, BAD_CAST "Device", BAD_CAST pRow_Device->Description_get().c_str());
		xmlNewProp (xmlNodePtr_device, BAD_CAST "PK_Device", BAD_CAST StringUtils::itos(pRow_Device->PK_Device_get()).c_str());	
		Row_Room *pRow_Room = pRow_Device->FK_Room_getrow();
		if( pRow_Room )
		{
			m_mapRow_Room[ pRow_Room->PK_Room_get() ] = pRow_Room;
			xmlNewProp (xmlNodePtr_device, BAD_CAST "PK_Room", BAD_CAST StringUtils::itos(pRow_Room->PK_Room_get()).c_str());	
		}

		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
		if( pRow_DeviceTemplate )
		{
			m_mapRow_DeviceTemplate[ pRow_DeviceTemplate->PK_DeviceTemplate_get() ] = pRow_DeviceTemplate;
			xmlNewProp (xmlNodePtr_device, BAD_CAST "PK_DeviceTemplate", BAD_CAST StringUtils::itos(pRow_DeviceTemplate->PK_DeviceTemplate_get()).c_str());	
			Row_DeviceCategory *pRow_DeviceCategory = pRow_DeviceTemplate->FK_DeviceCategory_getrow();
			if( pRow_DeviceCategory )
			{
				m_mapRow_DeviceCategory[ pRow_DeviceCategory->PK_DeviceCategory_get() ] = pRow_DeviceCategory;
				xmlNewProp (xmlNodePtr_device, BAD_CAST "PK_DeviceCategory", BAD_CAST StringUtils::itos(pRow_DeviceCategory->PK_DeviceCategory_get()).c_str());	
			}
		}
	}

	xmlNodePtr xmlNodePtr_room_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "RoomList", BAD_CAST "");
	for(map<int,Row_Room *>::iterator it=m_mapRow_Room.begin();it!=m_mapRow_Room.end();++it)
	{
		Row_Room *pRow_Room = it->second;
		xmlNodePtr xmlNodePtr_room = xmlNewTextChild (xmlNodePtr_room_list, NULL, BAD_CAST "Room", BAD_CAST pRow_Room->Description_get().c_str());
		xmlNewProp (xmlNodePtr_room, BAD_CAST "PK_Room", BAD_CAST StringUtils::itos(pRow_Room->PK_Room_get()).c_str());	
	}

	xmlNodePtr xmlNodePtr_DeviceTemplate_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "DeviceTemplateList", BAD_CAST "");
	for(map<int,Row_DeviceTemplate *>::iterator it=m_mapRow_DeviceTemplate.begin();it!=m_mapRow_DeviceTemplate.end();++it)
	{
		Row_DeviceTemplate *pRow_DeviceTemplate = it->second;
		xmlNodePtr xmlNodePtr_DeviceTemplate = xmlNewTextChild (xmlNodePtr_DeviceTemplate_list, NULL, BAD_CAST "DeviceTemplate", BAD_CAST pRow_DeviceTemplate->Description_get().c_str());
		xmlNewProp (xmlNodePtr_DeviceTemplate, BAD_CAST "PK_DeviceTemplate", BAD_CAST StringUtils::itos(pRow_DeviceTemplate->PK_DeviceTemplate_get()).c_str());	
	}

	xmlNodePtr xmlNodePtr_DeviceCategory_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "DeviceCategoryList", BAD_CAST "");
	for(map<int,Row_DeviceCategory *>::iterator it=m_mapRow_DeviceCategory.begin();it!=m_mapRow_DeviceCategory.end();++it)
	{
		Row_DeviceCategory *pRow_DeviceCategory = it->second;
		xmlNodePtr xmlNodePtr_DeviceCategory = xmlNewTextChild (xmlNodePtr_DeviceCategory_list, NULL, BAD_CAST "DeviceCategory", BAD_CAST pRow_DeviceCategory->Description_get().c_str());
		xmlNewProp (xmlNodePtr_DeviceCategory, BAD_CAST "PK_DeviceCategory", BAD_CAST StringUtils::itos(pRow_DeviceCategory->PK_DeviceCategory_get()).c_str());	
	}

	return 0;
}

/*virtual*/ int CommandGroupList::Populate(xmlDocPtr xmlDocPtr_Parms_In,xmlDocPtr xmlDocPtr_Data,xmlNodePtr xmlNodePtr_Root)
{
	vector<Row_Device *> vectRow_Moxi;
	m_pDatabase_pluto_main->Device_get()->GetRows("FK_DeviceTemplate=1870",&vectRow_Moxi);
	Row_Device *pRow_Device = NULL;
	if( vectRow_Moxi.empty()==false )
		pRow_Device = vectRow_Moxi[0];

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CommandGroupList::Populate delete this hack device %d room %d",
	pRow_Device ? pRow_Device->PK_Device_get() : 0,pRow_Device ? pRow_Device->FK_Room_get() : 0);
	// As we go through the devices, keep track of the rooms, device templates, and device categories we reference so we can include them
	map<int,Row_Room *> m_mapRow_Room;
	map<int,Row_Array *> m_mapRow_Array;

	xmlNodePtr xmlNodePtr_commandgroup_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "CommandGroupList", BAD_CAST "");

	vector<Row_CommandGroup *> vectRow_CommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows("FK_Array IN (1,2,3,4)",&vectRow_CommandGroup);
	for(vector<Row_CommandGroup *>::iterator it=vectRow_CommandGroup.begin();it!=vectRow_CommandGroup.end();++it)
	{
		Row_CommandGroup *pRow_CommandGroup = *it;

if( pRow_Device )
{
	Row_CommandGroup_Room *pRow_CommandGroup_Room = this->m_pDatabase_pluto_main->CommandGroup_Room_get()->GetRow(pRow_CommandGroup->PK_CommandGroup_get(),
		pRow_Device->FK_Room_get());
	if( !pRow_CommandGroup_Room )
	{
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CommandGroupList::Populate skipping %d",pRow_CommandGroup->PK_CommandGroup_get()); 
continue;
	}
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CommandGroupList::Populate letting through %d",pRow_CommandGroup->PK_CommandGroup_get()); 
}


		xmlNodePtr xmlNodePtr_commandgroup = xmlNewTextChild (xmlNodePtr_commandgroup_list, NULL, BAD_CAST "CommandGroup", BAD_CAST pRow_CommandGroup->Description_get().c_str());
		xmlNewProp (xmlNodePtr_commandgroup, BAD_CAST "PK_CommandGroup", BAD_CAST StringUtils::itos(pRow_CommandGroup->PK_CommandGroup_get()).c_str());
		
		vector<Row_CommandGroup_Room *> vectRow_CommandGroup_Room;
		pRow_CommandGroup->CommandGroup_Room_FK_CommandGroup_getrows(&vectRow_CommandGroup_Room);
		for(vector<Row_CommandGroup_Room *>::iterator itRoom=vectRow_CommandGroup_Room.begin();itRoom!=vectRow_CommandGroup_Room.end();++itRoom)
		{
			Row_CommandGroup_Room *pRow_CommandGroup_Room = *itRoom;
			Row_Room *pRow_Room = pRow_CommandGroup_Room->FK_Room_getrow();
			if( pRow_Room )
			{
				xmlNodePtr xmlNodePtr_room = xmlNewTextChild (xmlNodePtr_commandgroup, NULL, BAD_CAST "Room", BAD_CAST "");
				xmlNewProp (xmlNodePtr_room, BAD_CAST "PK_Room", BAD_CAST StringUtils::itos(pRow_Room->PK_Room_get()).c_str());
				m_mapRow_Room[ pRow_Room->PK_Room_get() ] = pRow_Room;
			}
		}

		Row_Array *pRow_Array = pRow_CommandGroup->FK_Array_getrow();
		if( pRow_Array )
		{
			m_mapRow_Array[ pRow_Array->PK_Array_get() ] = pRow_Array;
			xmlNewProp (xmlNodePtr_commandgroup, BAD_CAST "PK_Array", BAD_CAST StringUtils::itos(pRow_Array->PK_Array_get()).c_str());	
		}
	}

	xmlNodePtr xmlNodePtr_room_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "RoomList", BAD_CAST "");
	for(map<int,Row_Room *>::iterator it=m_mapRow_Room.begin();it!=m_mapRow_Room.end();++it)
	{
		Row_Room *pRow_Room = it->second;
		xmlNodePtr xmlNodePtr_room = xmlNewTextChild (xmlNodePtr_room_list, NULL, BAD_CAST "Room", BAD_CAST pRow_Room->Description_get().c_str());
		xmlNewProp (xmlNodePtr_room, BAD_CAST "PK_Room", BAD_CAST StringUtils::itos(pRow_Room->PK_Room_get()).c_str());	
	}

	xmlNodePtr xmlNodePtr_Array_list = xmlNewTextChild (xmlNodePtr_Root, NULL, BAD_CAST "ArrayList", BAD_CAST "");
	for(map<int,Row_Array *>::iterator it=m_mapRow_Array.begin();it!=m_mapRow_Array.end();++it)
	{
		Row_Array *pRow_Array = it->second;
		xmlNodePtr xmlNodePtr_Array = xmlNewTextChild (xmlNodePtr_Array_list, NULL, BAD_CAST "Array", BAD_CAST pRow_Array->Description_get().c_str());
		xmlNewProp (xmlNodePtr_Array, BAD_CAST "PK_Array", BAD_CAST StringUtils::itos(pRow_Array->PK_Array_get()).c_str());	
	}

	return 0;
}
