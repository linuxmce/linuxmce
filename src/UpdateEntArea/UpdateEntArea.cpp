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
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Users.h"
#include "DCE/Message.h"

#include "../include/version.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;

UpdateEntArea::UpdateEntArea()
{
}

UpdateEntArea::~UpdateEntArea()
{
	if(NULL != m_pDatabase_pluto_main)
	{
		delete m_pDatabase_pluto_main;
		m_pDatabase_pluto_main = NULL;
	}
}

void UpdateEntArea::AddDefaultScenarios()
{
	AddDefaultMediaScenarios();
	AddDefaultLightingScenarios();
	AddDefaultClimateScenarios();
	AddDefaultTelecomScenarios();
	AddDefaultSecurityScenarios();
	AddDefaultEventHandlers();
}

void UpdateEntArea::AddShortcut(int PK_Room, char cCharacter, int PK_CommandGroup)
{
	string sMessage = "0 " TOSTRING(DEVICEID_DCEROUTER) " 1 "
		TOSTRING(COMMAND_Execute_Command_Group_CONST) " " TOSTRING(COMMANDPARAMETER_PK_CommandGroup_CONST) " " +
		StringUtils::itos(PK_CommandGroup);
	AddShortcut(PK_Room,cCharacter,sMessage);
}

void UpdateEntArea::AddShortcut(int PK_Room, char cCharacter, string sMessage)
{
	MapShortcuts *pMapShortcuts=NULL;
	map<int, MapShortcuts *>::iterator it=m_mapOrbiter_Shortcuts.find(PK_Room);
	if( it==m_mapOrbiter_Shortcuts.end() )
	{
		pMapShortcuts = new MapShortcuts;
		m_mapOrbiter_Shortcuts[PK_Room]=pMapShortcuts;
	}
	else
		pMapShortcuts = it->second;

	if( pMapShortcuts->find(cCharacter)==pMapShortcuts->end() )  // Don't add shortcuts twice
		(*pMapShortcuts)[cCharacter] = sMessage;
}

void UpdateEntArea::UpdateOrbiterShortcuts()
{
	// Override the DEVICEDATA_Shortcut_CONST to have the shortcuts in our m_mapOrbiter_Shortcuts, unless
	// there's already a shortcut for that orbiter where Automatic=false, meaning the user manually updated it on his own
	vector<Row_Device *> vectRow_Orbiters;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Standard_Orbiter_CONST),
		&vectRow_Orbiters);
	for(vector<Row_Device *>::iterator it=vectRow_Orbiters.begin();it!=vectRow_Orbiters.end();++it)
	{
		Row_Device *pRow_Device = *it;

		// Add shortcuts for the top lighting scenarios
        vector<class Row_CommandGroup_Room *> vectRow_CommandGroup_Room;
        string sql = "JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup WHERE FK_Room=" +
			StringUtils::itos(pRow_Device->FK_Room_get()) + " AND FK_Array=" TOSTRING(ARRAY_Lighting_Scenarios_CONST) " AND Disabled=0 "
			" AND (FK_Template<>" TOSTRING(TEMPLATE_Lighting_Automatic_CONST) " OR (TemplateParm1<>0 AND TemplateParm1<>1 AND TemplateParm1<>3 AND TemplateParm1<>4)) "  // Skip lights on/off/
			" ORDER BY " COMMANDGROUP_ROOM_SORT_FIELD " LIMIT 8";
        m_pDatabase_pluto_main->CommandGroup_Room_get()->GetRows(sql,&vectRow_CommandGroup_Room);
		char cShortcut='4';
		for(vector<class Row_CommandGroup_Room *>::iterator it=vectRow_CommandGroup_Room.begin();it!=vectRow_CommandGroup_Room.end();++it)
		{
			Row_CommandGroup_Room *pRow_CommandGroup_Room = *it;
			Row_CommandGroup *pRow_CommandGroup=pRow_CommandGroup_Room->FK_CommandGroup_getrow();
            if( pRow_CommandGroup )
			{
				string sMessage = "0 " TOSTRING(DEVICEID_DCEROUTER) " 1 "
					TOSTRING(COMMAND_Execute_Command_Group_CONST) " " TOSTRING(COMMANDPARAMETER_PK_CommandGroup_CONST) " " +
					StringUtils::itos(pRow_CommandGroup_Room->FK_CommandGroup_get());
				AddShortcut(pRow_Device->FK_Room_get(),cShortcut++,sMessage);
			}
		}

		vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
		pRow_Device->Device_EntertainArea_FK_Device_getrows(&vectRow_Device_EntertainArea);
		if( vectRow_Device_EntertainArea.size() )
		{
			Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[0];
			AddShortcut(pRow_Device->FK_Room_get(),'R',"0 " TOSTRING(DEVICETEMPLATE_VirtDev_Media_Plugin_CONST) " 1 "
				TOSTRING(COMMAND_Retransmit_AV_Commands_CONST) " " TOSTRING(COMMANDPARAMETER_Text_CONST) " \"VP\" "
				TOSTRING(COMMANDPARAMETER_PK_EntertainArea_CONST) " " + StringUtils::itos(pRow_Device_EntertainArea->FK_EntertainArea_get()) );
			AddShortcut(pRow_Device->FK_Room_get(),'G',"0 " TOSTRING(DEVICETEMPLATE_VirtDev_Media_Plugin_CONST) " 1 "
				TOSTRING(COMMAND_Retransmit_AV_Commands_CONST) " " TOSTRING(COMMANDPARAMETER_Text_CONST) " \"VI\" "
				TOSTRING(COMMANDPARAMETER_PK_EntertainArea_CONST) " " + StringUtils::itos(pRow_Device_EntertainArea->FK_EntertainArea_get()) );
			AddShortcut(pRow_Device->FK_Room_get(),'Y',"0 " TOSTRING(DEVICETEMPLATE_VirtDev_Media_Plugin_CONST) " 1 "
				TOSTRING(COMMAND_Retransmit_AV_Commands_CONST) " " TOSTRING(COMMANDPARAMETER_Text_CONST) " \"AP\" "
				TOSTRING(COMMANDPARAMETER_PK_EntertainArea_CONST) " " + StringUtils::itos(pRow_Device_EntertainArea->FK_EntertainArea_get()) );
			AddShortcut(pRow_Device->FK_Room_get(),'B',"0 " TOSTRING(DEVICETEMPLATE_VirtDev_Media_Plugin_CONST) " 1 "
				TOSTRING(COMMAND_Retransmit_AV_Commands_CONST) " " TOSTRING(COMMANDPARAMETER_Text_CONST) " \"AI\" "
				TOSTRING(COMMANDPARAMETER_PK_EntertainArea_CONST) " " + StringUtils::itos(pRow_Device_EntertainArea->FK_EntertainArea_get()) );
		}

		// Get the shortcuts we have for this room
		map<int, MapShortcuts *>::iterator itMS=m_mapOrbiter_Shortcuts.find(pRow_Device->FK_Room_get());
		MapShortcuts *pMapShortcuts = itMS!=m_mapOrbiter_Shortcuts.end() ? itMS->second : NULL;

		// Get the shortcuts the orbiter already has and put them in here which weren't automatically done
		map< pair<bool,char>, string> mapOrbiterShortcuts; // bool=automatic, char=character
		Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Shortcut_CONST);
		if( pRow_Device_DeviceData )
		{
			vector<string> vectExistingShortcuts;
			string s = pRow_Device_DeviceData->IK_DeviceData_get();
			StringUtils::Tokenize(s,"\r\n",vectExistingShortcuts);
			for(vector<string>::iterator it=vectExistingShortcuts.begin();it!=vectExistingShortcuts.end();++it)
			{
				string::size_type pos=0;
				bool bAutomatic = StringUtils::Tokenize( *it, "\t", pos )=="1";
				string sCharacter = StringUtils::Tokenize( *it, "\t", pos );
				string sMessage = StringUtils::Tokenize( *it, "\t", pos );

				if( sCharacter.empty() )
					continue; // Shouldn't happen

				if( bAutomatic==false )
					mapOrbiterShortcuts[ make_pair<bool,char> (false,sCharacter[0]) ] = sMessage;
			}
		}

		// Now we've added back in all the shortcuts the user did manually.  Add in our own
		if( pMapShortcuts )
		{
			for(map<char,string>::iterator it=pMapShortcuts->begin();it!=pMapShortcuts->end();++it)
			{
				if( mapOrbiterShortcuts.find( make_pair<bool,char> (false,it->first) )==mapOrbiterShortcuts.end() )
				{
					// The user doesn't already have his own manual shortcut.  Use this automatic one
					mapOrbiterShortcuts[ make_pair<bool,char> (true,it->first) ] = it->second;
				}
			}
		}

		// Now we've got all the shortcuts.  Make them a string again and update the database
		string sOutput,sChar=" ";
		for( map< pair<bool,char>, string>::iterator itMS=mapOrbiterShortcuts.begin();itMS!=mapOrbiterShortcuts.end();++itMS )
		{
			sChar[0] = itMS->first.second;  // Doesn't seem to like have a + and a char
			sOutput += (itMS->first.first ? "1\t" : "0\t") + sChar + "\t" + itMS->second + "\n";
		}

		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Shortcut_CONST,sOutput);
	}
}

