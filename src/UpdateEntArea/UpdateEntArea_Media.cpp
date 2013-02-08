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
#include "PlutoUtils/DatabaseUtils.h"
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
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Icon.h"
#include "pluto_main/Define_RoomType.h"

#include "CommandGroupArray.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;

void UpdateEntArea::AddDefaultMediaScenarios()
{
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Staring to update media areas ...");
//	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Staring to update media areas [%d]: from %d to %d!", m_mapEnt_Area_Auto_Media.size(), 
//														m_mapEnt_Area_Auto_Media.begin(), m_mapEnt_Area_Auto_Media.end());
	
	for(map<int, LevelOfMedia >::iterator it=m_mapEnt_Area_Auto_Media.begin();it!=m_mapEnt_Area_Auto_Media.end();++it)
	{
		Row_EntertainArea *pRow_EntertainArea = m_pDatabase_pluto_main->EntertainArea_get()->GetRow(it->first);
		if( !pRow_EntertainArea )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find EntertainArea %d!", it->first);
			continue;
		}

		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(pRow_EntertainArea->FK_Room_get());
		if( !pRow_Room )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Room %d!", pRow_EntertainArea->FK_Room_get());
			continue;
		}

		if( pRow_Room->FK_RoomType_get()!= ROOMTYPE_Unmanaged_CONST)
			AddDefaultMediaScenarios(pRow_EntertainArea);
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Nothing to do with EntertainArea!");
	}
}

void UpdateEntArea::AddDefaultMediaScenarios(Row_EntertainArea *pRow_EntertainArea)
{
	CommandGroup *pCommandGroup;
	string sSQL;
	CommandGroupArray commandGroupArray(pRow_EntertainArea,ARRAY_Media_Scenarios_CONST,true);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting to add media scenarios ...");
	
	// See what kind of Pluto TV we have (VDR/Myth/etc. and add the button(s)
	sSQL="SELECT PK_Device,Device.Description FROM Device_EntertainArea "
		"JOIN Device ON FK_Device=PK_Device "
		"JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceTemplate_MediaType ON Device.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate "
		"WHERE FK_MediaType=" + StringUtils::itos(MEDIATYPE_pluto_LiveTV_CONST) + " AND FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get()) + 
		" AND FK_DeviceCategory NOT IN (" 
		TOSTRING(DEVICECATEGORY_Capture_Card_Ports_CONST) "," TOSTRING(DEVICECATEGORY_Mobile_Orbiter_CONST) "," TOSTRING(DEVICECATEGORY_Standard_Orbiter_CONST) "," TOSTRING(DEVICECATEGORY_Orbiter_CONST)
		") "
		" ORDER BY PK_Device";

	int iOrder=1;
	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
	{
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( atoi(row[0]) );
			string sDesc = "TV";
			if( result_set.r->row_count>1 )
				sDesc += string("\n") + row[1];
			pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_Pluto_Sources_CONST,sDesc,ICON_TV_CONST,atoi(row[0]),MEDIATYPE_pluto_LiveTV_CONST,NULL,iOrder, 0);
			if( pCommandGroup )
			{
				if( pRow_Device && pRow_Device->FK_DeviceTemplate_get()==DEVICETEMPLATE_MythTV_Player_CONST && 
					pRow_Device->FK_Device_ControlledVia_getrow() && pRow_Device->FK_Device_ControlledVia_getrow()->FK_Device_ControlledVia_getrow() &&
					pRow_Device->FK_Device_ControlledVia_getrow()->FK_Device_ControlledVia_getrow()->FK_DeviceTemplate_get()==DEVICETEMPLATE_Fiire_Station_1_CONST
					)
				{
					pCommandGroup->AddCommand(m_dwPK_Device_OrbiterPlugIn,COMMAND_Display_Message_CONST,iOrder++,2,
						COMMANDPARAMETER_List_PK_Device_CONST,StringUtils::itos(pRow_Device->FK_Device_ControlledVia_get()).c_str(),
						COMMANDPARAMETER_Text_CONST,"Sorry.  MythTV doesn't currently run on this platform.");
				}
				else
					pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Play_Media_CONST,iOrder++,2,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_LiveTV_CONST).c_str(),COMMANDPARAMETER_PK_Device_CONST,row[0]);
			}
		}
	}
	
	//
	// Adding PK_Text if it exists to have proper captions for foreign languages --nite_man
	//
	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Video",ICON_Video_CONST,0,MEDIATYPE_pluto_StoredVideo_CONST,NULL,10,TEXT_MediaType_Video_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_StoredVideo_CONST).c_str());

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Audio",ICON_Music_CONST,0,MEDIATYPE_pluto_StoredAudio_CONST,NULL,11,TEXT_MediaType_Audio_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_StoredAudio_CONST).c_str());

        pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Radio",ICON_Music_CONST,0,MEDIATYPE_lmce_StreamedAudio_CONST,NULL,13,TEXT_MediaType_StreamedAudio_CONST);
        if( pCommandGroup )
	  pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_lmce_StreamedAudio_CONST).c_str());

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Playlists",ICON_Playlist_CONST,0,MEDIATYPE_misc_Playlist_CONST,NULL,15,TEXT_Playlists_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_misc_Playlist_CONST).c_str());

        {
		// Add a Games button if a Game Player exists..
		// TODO: Make this a category when the next emulator is supported.
		string sSQL = "SELECT PK_Device FROM Device_EntertainArea "
			"JOIN Device ON FK_Device=PK_Device "
			"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
			"WHERE FK_DeviceTemplate = " + StringUtils::itos(DEVICETEMPLATE_Game_Player_CONST) + " AND FK_EntertainArea=" +
			StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get());
		PlutoSqlResult result_set;
		if ( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) && result_set.r->row_count>0 )
		{
		      pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Games",ICON_Playlist_CONST,NULL,12);
		      if (pCommandGroup)
			  pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_lmce_Game_CONST).c_str());
		}
	}

	{
		//Manage Drives Scenario: only add this button if there's at least one disk drive somewhere in the house...
		string sSQL = "SELECT PK_Device from Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Disc_Drives_CONST);
		PlutoSqlResult result_set;
		if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) && result_set.r->row_count>0 )
		{
			pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Manage Drives",ICON_Start_Disc_CONST,0,0,NULL,16,TEXT_Manage_Drives_CONST);
			if( pCommandGroup )
				pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Goto_Screen_CONST,1,1,COMMANDPARAMETER_PK_Screen_CONST,StringUtils::itos(SCREEN_Manage_Drives_CONST).c_str());
		}
	}

	pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Pictures",ICON_Picture_CONST,0,MEDIATYPE_pluto_Pictures_CONST,NULL,13,TEXT_MediaType_Pictures_CONST);
	if( pCommandGroup )
		pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_pluto_Pictures_CONST).c_str());

	// Temporarily removed useless Docs media type, for now. It will be back -tschak

	// pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_FileDisc_CONST,"Docs",ICON_Document_CONST,0,MEDIATYPE_misc_DocViewer_CONST,NULL,14);
	// if( pCommandGroup )
	//	pCommandGroup->AddCommand(DEVICETEMPLATE_This_Orbiter_CONST,COMMAND_Show_File_List_CONST,1,1,COMMANDPARAMETER_PK_MediaType_CONST,StringUtils::itos(MEDIATYPE_misc_DocViewer_CONST).c_str());

	{
		// Find all non-pluto media
		string sSQL="SELECT PK_Device,FK_MediaType,MediaType.Description as MediaType "
			"FROM Device_EntertainArea "
			"JOIN Device ON FK_Device=PK_Device "
			"JOIN DeviceTemplate_MediaType ON Device.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate "
			"JOIN MediaType ON FK_MediaType=PK_MediaType "
			"WHERE FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get()) + " AND FK_MediaType NOT IN (1,2,3,4,5,6,7,20,21,22,23,24,27,28,29,30,32,33,34,35,36,37,38,39,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64) ORDER BY PK_Device";

		iOrder=1;
		PlutoSqlResult result_set;
		DB_ROW row;
		if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
		{
			while ((row = db_wrapper_fetch_row(result_set.r)))
			{
				Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(atoi(row[0]));
				if( !pRow_Device->FK_Device_RouteTo_isNull() )
					pRow_Device = pRow_Device->FK_Device_RouteTo_getrow();

				if( !pRow_Device )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Error device %s cannot be located",row[0]);
					continue;
				}

				string sDesc = row[2] + string("\n") + pRow_Device->Description_get();
				pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Wiz_NP_Sources_CONST,sDesc,0,atoi(row[0]),atoi(row[1]),NULL,iOrder+30);
				if( pCommandGroup )
					pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Play_Media_CONST,iOrder++,3,
						COMMANDPARAMETER_PK_MediaType_CONST,row[1],
						COMMANDPARAMETER_PK_Device_CONST,row[0],
						COMMANDPARAMETER_PK_EntertainArea_CONST,StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get()).c_str());
			}
		}
	}
}
