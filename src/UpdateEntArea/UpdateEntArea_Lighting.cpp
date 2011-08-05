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

#include "DCE/Message.h"

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

bool UpdateEntArea::IsLight(int PK_FloorplanType)
{
	return PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_CEILING_LIGHT_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_TABLE_LAMP_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_WALL_SCONCE_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_FLOOR_LAMP_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_CHANDALIER_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_PICTURE_LIGHT_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_ACCENT_LIGHT_CONST ||
		PK_FloorplanType==0;  // Assume it's a light if the user didn't specify the type
}

bool UpdateEntArea::IsBlind(int PK_FloorplanType)
// Determines whether a specific device is a controlling a shading device like blinds or drapes.
{
	return PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST ||
		PK_FloorplanType==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST;
}

bool UpdateEntArea::IsPublicInteriorRoom(int PK_RoomType)
{
	return PK_RoomType==ROOMTYPE_Living_RoomFamily_Room_CONST ||
		PK_RoomType==ROOMTYPE_ClosetStorage_Space_CONST || 
		PK_RoomType==ROOMTYPE_Garage_CONST || 
		PK_RoomType==ROOMTYPE_Kitchen_CONST || 
		PK_RoomType==ROOMTYPE_Dining_Room_CONST || 
		PK_RoomType==ROOMTYPE_Hallway_CONST ||
		PK_RoomType==ROOMTYPE_FoyerEntrance_CONST ||
		PK_RoomType==ROOMTYPE_Home_Theater_CONST;
}

bool UpdateEntArea::IsInteriorRoom(int PK_RoomType)
{
	return PK_RoomType==ROOMTYPE_Living_RoomFamily_Room_CONST ||
		PK_RoomType==ROOMTYPE_ClosetStorage_Space_CONST || 
		PK_RoomType==ROOMTYPE_Garage_CONST || 
		PK_RoomType==ROOMTYPE_Kitchen_CONST || 
		PK_RoomType==ROOMTYPE_Dining_Room_CONST || 
		PK_RoomType==ROOMTYPE_Hallway_CONST ||
		PK_RoomType==ROOMTYPE_Bedroom_Other_CONST ||
		PK_RoomType==ROOMTYPE_Bedroom_Master_CONST ||
		PK_RoomType==ROOMTYPE_FoyerEntrance_CONST ||
		PK_RoomType==ROOMTYPE_Home_Theater_CONST;
}

bool UpdateEntArea::IsExterior(int PK_RoomType)
{
	return PK_RoomType==ROOMTYPE_Patio_CONST ||
		PK_RoomType==ROOMTYPE_Outside_Area_CONST;
}

void UpdateEntArea::AddDefaultLightingScenarios()
{
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( pRow_Room && pRow_Room->FK_RoomType_get()!=ROOMTYPE_Unmanaged_CONST)
			AddDefaultLightingScenarios(pRow_Room);
	}
}

void UpdateEntArea::AddDefaultLightingScenarios(Row_Room *pRow_Room)
{
	CommandGroup *pCommandGroup;
	string sSQL;
	CommandGroupArray commandGroupArray(pRow_Room,ARRAY_Lighting_Scenarios_CONST,true);

	map<int,int> map_Device_Type; // Where type is a floorplan type
	map<int,list<int> > mapType; // For each type a list of the devices 
	GetDevicesTypes(DEVICECATEGORY_Lighting_Device_CONST,pRow_Room->PK_Room_get(),
		&map_Device_Type,&mapType);

	bool bRoomHasLights=false;
	for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
		if( IsLight(it->second) )
			bRoomHasLights=true;

	bool bRoomHasBlinds=false;
	for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
		if( IsBlind(it->second) )
			bRoomHasBlinds=true;

	map<int,int> map_Device_Type_MD; // Where type is a floorplan type
	GetDevicesTypes(DEVICECATEGORY_Media_Director_CONST,pRow_Room->PK_Room_get(),
		&map_Device_Type_MD,NULL);

	int PK_Device_MD = map_Device_Type_MD.size() ? map_Device_Type_MD.begin()->first : 0;


	/* Sort orders:
		Off=1
		On=2
		Showtime=3
		Sleep=4
		Wakeup=5
		HouseToSleep=6
	*/

	int iOrder=3; // Start with 3 so lights on is always 1, off is always 2
	if( bRoomHasLights )
	{
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"On",ICON_Lights_On_CONST,1,0,NULL,1,TEXT_Lights_On_CONST);
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'1',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"100");
		}

		iOrder=1;
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Off",ICON_Lights_Off_CONST,0,0,NULL,2,TEXT_Lights_Off_CONST);
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'2',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,2,0);
		}
	}

	// If there are blinds, we create some open/close/shade scenario buttons
	if( bRoomHasBlinds )
	{
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Open",ICON_Lights_On_CONST,8,0,NULL,8,TEXT_Blinds_Open_CONST);
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'0',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsBlind(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"0");
					                                        
		}

		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Shade",ICON_Lights_On_CONST,9,0,NULL,10,TEXT_Blinds_Shade_CONST);
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'0',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsBlind(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,1,1,COMMANDPARAMETER_Level_CONST,"30");
		}

		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Close",ICON_Lights_Off_CONST,10,0,NULL,10,TEXT_Blinds_Close_CONST);
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'0',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				if( IsBlind(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_On_CONST,2,0);
		}
	}

	// If there are any lights or blinds at all, we'll create a ShowTime scenario if there's also a TV
	if( map_Device_Type.size() )
	{
		map<int,int> map_Device_Type_TV; // Where type is a floorplan type
		GetDevicesTypes(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST,pRow_Room->PK_Room_get(),
			&map_Device_Type_TV,NULL);

		// If there's a TV in the room, we'll add a Showtime scenario
		if( map_Device_Type_TV.size() )
		{
			iOrder=1;
			pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Showtime",ICON_Showtime_CONST,2,0,NULL,3,TEXT_Lights_Showtime_CONST);  // Showtime is parm1=2
			if( pCommandGroup )
			{
				for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
				{
					// Put ceiling lights at 10%, accent lights at 30%, blinds closed, other lights off
					if( it->second==FLOORPLANOBJECTTYPE_LIGHT_CEILING_LIGHT_CONST )
						pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,iOrder++,1,COMMANDPARAMETER_Level_CONST,"10");
					else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_ACCENT_LIGHT_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHT_PICTURE_LIGHT_CONST )
						pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,iOrder++,1,COMMANDPARAMETER_Level_CONST,"30");
					else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST )
						pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);
					else if( IsLight(it->second) )
						pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);
				}
			}
		}
	}

	// For all bedrooms add Sleep, Wakeup etc scenarios, which turn on/off the lights and open/closes the blinds as well as turning off any media.
	if( pRow_Room->FK_RoomType_get()==ROOMTYPE_Bedroom_Other_CONST || pRow_Room->FK_RoomType_get()==ROOMTYPE_Bedroom_Master_CONST )
	{
		iOrder=1;
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Sleep",ICON_Sleep_CONST,3,0,NULL,4,TEXT_Lights_Sleep_CONST);  // Bedtime is parm1=3
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'0',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
			{
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);
				else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_On_CONST,iOrder++,0); // Must be ON as that means it is closed (drapes are 100% out)
			}
			pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Stop_Media_CONST,iOrder++,1,
				COMMANDPARAMETER_Bypass_Event_CONST,"1");
			pCommandGroup->AddCommand(m_dwPK_Device_TelecomPlugIn,COMMAND_Set_User_Mode_CONST,iOrder++,1,
				COMMANDPARAMETER_PK_UserMode_CONST,StringUtils::itos(USERMODE_Sleeping_CONST).c_str());
			if( PK_Device_MD )
				pCommandGroup->AddCommand(PK_Device_MD,COMMAND_Generic_Off_CONST,iOrder++,0);
		}

		iOrder=1;
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Wakeup",ICON_Sleep_CONST,4,0,NULL,5,TEXT_Lights_Wakeup_CONST);  // Wakeup is parm1=4  Sort 4
		if( pCommandGroup )
		{
			AddShortcut(pRow_Room->PK_Room_get(),'3',pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get());
			for(map<int,int>::iterator it=map_Device_Type.begin();it!=map_Device_Type.end();++it)
			{
				if( IsLight(it->second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Set_Level_CONST,iOrder++,1,COMMANDPARAMETER_Level_CONST,"30");
				else if( it->second==FLOORPLANOBJECTTYPE_LIGHT_BLINDS_CONST || it->second==FLOORPLANOBJECTTYPE_LIGHTS_DRAPES_CONST )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);  // Must be OFF as that means it is open (drapes are 0% out)
			}
			pCommandGroup->AddCommand(m_dwPK_Device_TelecomPlugIn,COMMAND_Set_User_Mode_CONST,iOrder++,1,
				COMMANDPARAMETER_PK_UserMode_CONST,StringUtils::itos(USERMODE_At_Home_CONST).c_str());
		}
	}

	// The master bed room has an automatic scenario setup to bring the whole house into sleep mode. This means, all lights are off, and all AV devices are turned off.
	if( pRow_Room->FK_RoomType_get()==ROOMTYPE_Bedroom_Master_CONST )
	{
		iOrder=1;
		pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"House to sleep mode",ICON_House_to_Sleep_Mode_CONST,5,0,NULL,6,TEXT_Lights_House_Sleep_CONST);  // House to sleep mode is parm1=5
		if( pCommandGroup )
		{
			vector<Row_EntertainArea *> vectRow_EntertainArea;
			pRow_Room->Table_Room_get()->Database_pluto_main_get()->EntertainArea_get()->GetRows("1=1 ORDER BY PK_EntertainArea",&vectRow_EntertainArea);
			for(size_t s=0;s<vectRow_EntertainArea.size();++s)
			{
				Row_Room *pRow_Room = vectRow_EntertainArea[s]->FK_Room_getrow();
				if( pRow_Room && IsPublicInteriorRoom(pRow_Room->FK_RoomType_get()) )
					pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Stop_Media_CONST,iOrder++,2,
						COMMANDPARAMETER_PK_EntertainArea_CONST,StringUtils::itos(vectRow_EntertainArea[s]->PK_EntertainArea_get()).c_str(),
						COMMANDPARAMETER_Bypass_Event_CONST,"1");
			}

			map<int,pair<int,int> > map_Device_Type_RoomType;
			GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Lighting_Device_CONST,&map_Device_Type_RoomType);
			for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
			{
				if( IsPublicInteriorRoom(it->second.second) || IsExterior(it->second.second) )
					pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);
			}
			if( PK_Device_MD )
				pCommandGroup->AddCommand(PK_Device_MD,COMMAND_Generic_Off_CONST,iOrder++,0);
		}
	}
}

