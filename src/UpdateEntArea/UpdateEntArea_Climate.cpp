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
#include "pluto_main/Define_Text.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_RoomType.h"

#include "CommandGroupArray.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;


void UpdateEntArea::AddDefaultClimateScenarios()
{
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( pRow_Room && pRow_Room->FK_RoomType_get()!=ROOMTYPE_Unmanaged_CONST)
			AddDefaultClimateScenarios(pRow_Room);
	}
}

void UpdateEntArea::AddDefaultClimateScenarios(Row_Room *pRow_Room)
{
  CommandGroup *pCommandGroup;
  string sSQL;
  CommandGroupArray commandGroupArray(pRow_Room,ARRAY_Climate_Scenarios_CONST,true);
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Starting to add climate scenarios ...");

  // Add a weather button, if the Weather Plugin exists.
  sSQL="SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="+StringUtils::itos(DEVICECATEGORY_Weather_PlugIns_CONST);
  PlutoSqlResult result_set;
  if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) && result_set.r->row_count>0 )
    {
      pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Weather_Scenarios_CONST,"Weather",0,0,0,0,0,TEXT_Weather_CONST);
      if (pCommandGroup)
	{
	  pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_Weather_Now_CONST).c_str());
	  pCommandGroup->m_pRow_CommandGroup->FK_DesignObj_set(DESIGNOBJ_butWeatherMain_CONST);
	}
    }

}
