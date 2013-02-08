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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "UpdateEntArea.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif

#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceGroup.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_UserMode.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Icon.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_RoomType.h"
#include "pluto_main/Define_Text.h"

#include "CommandGroupArray.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;


void UpdateEntArea::AddDefaultSecurityScenarios()
{
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( pRow_Room && pRow_Room->FK_RoomType_get()!=ROOMTYPE_Unmanaged_CONST)
			AddDefaultSecurityScenarios(pRow_Room);
	}

	SetDefaultAnnouncementDevices();
	SetDefaultNotifications();
}

void UpdateEntArea::AddDefaultSecurityScenarios(Row_Room *pRow_Room)
{
	CommandGroup *pCommandGroup;
	CommandGroupArray commandGroupArray(pRow_Room,ARRAY_Security_Scenarios_CONST,true);

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Security_Arm_Disarm_CONST,"Security",0,1,0,0,0,TEXT_Security_CONST);
	if( pCommandGroup )
	{
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,
			COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_SecurityPanel_CONST).c_str());
		pCommandGroup->m_pRow_CommandGroup->FK_DesignObj_set(DESIGNOBJ_objHouseStatusIndicator_CONST);
	}

	string sSQL = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory "
		"LEFT JOIN Room ON PK_Room=FK_Room WHERE "
		"(PK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Surveillance_Cameras_CONST) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(DEVICECATEGORY_Surveillance_Cameras_CONST) + 
		") ORDER BY Room.Description";

	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device);
	if( vectRow_Device.size()==0 )
		return;

	if( vectRow_Device.size()>4 )
		AddQuadCameraScenarios(pRow_Room,vectRow_Device,commandGroupArray);
	else
		AddSingleCameraScenarios(pRow_Room,vectRow_Device,commandGroupArray);
}

void UpdateEntArea::AddQuadCameraScenarios(Row_Room *pRow_Room,vector<Row_Device *> &vectRow_Device,CommandGroupArray &commandGroupArray)
{
	CommandGroup *pCommandGroup;
	int iScenarioNum=1,iOrder=1;
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		string sRoom;
		Row_Room *pRow_Room = vectRow_Device[s]->FK_Room_getrow();
		if( pRow_Room )
			sRoom = pRow_Room->Description_get();
		if( sRoom.size()==0 )
			sRoom = "Quad Cams";

		string sDevices;
		for(int iFour=s+4;s<vectRow_Device.size() && s<iFour;++s)
		{
			Row_Device *pRow_Device = vectRow_Device[s];
			sDevices += StringUtils::itos(pRow_Device->PK_Device_get()) + ",";
		}

		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Security_View_Cameras_CONST,
			sRoom,0,iScenarioNum++,0);
		if( pCommandGroup )
			pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,iOrder++,2,
				COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_QuadViewCameras_CONST).c_str(),
				COMMANDPARAMETER_List_PK_Device_CONST, sDevices.c_str());
	}
}

void UpdateEntArea::AddSingleCameraScenarios(Row_Room *pRow_Room,vector<Row_Device *> &vectRow_Device,CommandGroupArray &commandGroupArray)
{
	int iOrder=1;
	CommandGroup *pCommandGroup;
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Security_View_Camera_CONST,
			vectRow_Device[s]->Description_get(),0,s,0);
		if( pCommandGroup )
			pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,iOrder++,2,
				COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_SingleCameraViewOnly_CONST).c_str(),
				COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(vectRow_Device[s]->PK_Device_get()).c_str());
	}
}

void UpdateEntArea::SetDefaultAnnouncementDevices()
{
	Row_Device *pRow_Device_Security_PlugIn=NULL;
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_Security_Plugin_CONST),&vectRow_Device);
	if( vectRow_Device.size()==0 )
		return;
	
	pRow_Device_Security_PlugIn = vectRow_Device[0];
	Row_Device_DeviceData *pRow_Device_DeviceData = GetUnmodifiedDeviceData(pRow_Device_Security_PlugIn->PK_Device_get(),DEVICEDATA_PK_Device_CONST);
	if( !pRow_Device_DeviceData )
		return; // The user has changed this list

	vectRow_Device.clear();
	m_pDatabase_pluto_main->Device_get()->GetRows("WHERE FK_DeviceTemplate in (" + 
		StringUtils::itos(DEVICETEMPLATE_Xine_Player_CONST) + "," +
		StringUtils::itos(DEVICETEMPLATE_SqueezeBox_Player_CONST)
		+ ")"
		,&vectRow_Device);

	string sList;

	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		if( sList.size() )
			sList += ",";
		sList += StringUtils::itos(pRow_Device->PK_Device_get());
	}

	SetDeviceData(pRow_Device_DeviceData->FK_Device_get(),DEVICEDATA_PK_Device_CONST,sList);
}

void UpdateEntArea::SetDefaultNotifications()
{
	Row_Device *pRow_Device_Security_PlugIn=NULL;
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("FK_DeviceTemplate=" + StringUtils::itos(DEVICETEMPLATE_Security_Plugin_CONST),&vectRow_Device);
	if( vectRow_Device.size()==0 )
		return;
	
	pRow_Device_Security_PlugIn = vectRow_Device[0];
	Row_Device_DeviceData *pRow_Device_DeviceData = GetUnmodifiedDeviceData(pRow_Device_Security_PlugIn->PK_Device_get(),DEVICEDATA_Mobile_Orbiter_Notification_CONST);
	if( !pRow_Device_DeviceData )
		return; // The user has changed this list

	vectRow_Device.clear();
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Mobile_Orbiter_CONST)
		,&vectRow_Device);

	string sList="0";

	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		sList += "," + StringUtils::itos(pRow_Device->PK_Device_get()) + ",1,1,1,1,1";
	}

	SetDeviceData(pRow_Device_DeviceData->FK_Device_get(),DEVICEDATA_Mobile_Orbiter_Notification_CONST,sList);
}
