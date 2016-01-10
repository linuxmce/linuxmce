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


void UpdateEntArea::AddDefaultTelecomScenarios()
{
        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Staring to update telecom areas ...");
	
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
               	if( !pRow_Room )
                {
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Room %d!", it->first);
                        continue;
                }
		if( pRow_Room && pRow_Room->FK_RoomType_get()!=ROOMTYPE_Unmanaged_CONST)
			AddDefaultTelecomScenarios(pRow_Room);
	}
}

void UpdateEntArea::AddDefaultTelecomScenarios(Row_Room *pRow_Room)
{
	CommandGroup *pCommandGroup;
	CommandGroupArray commandGroupArray(pRow_Room,ARRAY_Communication_Scenarios_CONST,true);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting to add telecom scenarios ...");

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Telecom_Scenarios_CONST,"Active Calls",ICON_Phone_CONST,1,0,0,0,TEXT_Active_Calls_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,
		COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_Active_Calls_CONST).c_str());

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Telecom_Scenarios_CONST,"Speed Dial",ICON_Phone_CONST,2,0,0,0,TEXT_SPEED_DIAL_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,
			COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_MakeCallFavorites_CONST).c_str());

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Telecom_Scenarios_CONST,"Dial Direct",ICON_Phone_CONST,3,0,0,0,TEXT_CALL_DNUM_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,
			COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_MakeCallDialNumber_CONST).c_str());

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Telecom_Scenarios_CONST,"Phone Book",ICON_Phone_CONST,4,0,0,0,TEXT_CALL_PB_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,
			COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_MakeCallPhonebook_CONST).c_str());

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Telecom_Scenarios_CONST,"Intercom",ICON_Phone_CONST,5,0,0,0,TEXT_CALL_INTERCOM_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,
			COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_MakeCallIntercom_CONST).c_str());

	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
	{
		Row_Users *pRow_Users = vectRow_Users[s];

		//Voicemail Button.
		//Use Nickname when available. 
		string sName = "";						
		if( pRow_Users->Nickname_get().size() && !(pRow_Users->Nickname_isNull())) {
			//There is a Nickname, use it!
			sName = pRow_Users->Nickname_get();
		} else {
			//There is no Nickname, so use the UserName		
			sName = pRow_Users->UserName_get();
		}

		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Telecom_Scenarios_CONST,sName,0,1,pRow_Users->PK_Users_get());
		if( pCommandGroup )
		{
			pCommandGroup->m_pRow_CommandGroup->FK_DesignObj_set(DESIGNOBJ_butUserStatus_CONST);
			pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Set_Variable_CONST,1,2,
				COMMANDPARAMETER_PK_Variable_CONST,StringUtils::itos(VARIABLE_PK_Users_CONST).c_str(),
				COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(pRow_Users->PK_Users_get()).c_str());
			pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,2,1,
				COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_UserStatus_CONST).c_str());
		}
	}
}

