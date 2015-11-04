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
#include "pluto_main/Table_EventHandler.h"
#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_CriteriaParmNesting.h"
#include "pluto_main/Table_CriteriaParm.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_UserMode.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_CriteriaParmList.h"
#include "pluto_main/Define_CriteriaList.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_HouseMode.h"
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

#include "CommandGroupArray.h"
#include "Criteria.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;



void UpdateEntArea::AddDefaultEventHandlers()
{
	CommandGroup *pCommandGroup;
	Row_EventHandler *pRow_EventHandler;

	CommandGroupArray commandGroupArray(m_pDatabase_pluto_main,m_iPK_Installation,ARRAY_Scenarios_for_Event_Handlers_CONST,true);

	pCommandGroup=CreateLeaveHomeCommandGroup(commandGroupArray);
	pRow_EventHandler=CreateLeaveHomeEventHandler(commandGroupArray);
	if( pRow_EventHandler )  // The user didn't change it, so go ahead and confirm it's current
	{
		if( pCommandGroup )
			pRow_EventHandler->FK_CommandGroup_set( pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get() );
		SetLeaveHomeCriteria(pRow_EventHandler);
		pRow_EventHandler->Table_EventHandler_get()->Commit();
		ResetEventHandler_psc_mod(pRow_EventHandler);
	}

	pCommandGroup=CreateSunsetCommandGroup(commandGroupArray);
	pRow_EventHandler=CreateSunsetEventHandler(commandGroupArray);
	if( pRow_EventHandler )  // The user didn't change it, so go ahead and confirm it's current
	{
		if( pCommandGroup )
			pRow_EventHandler->FK_CommandGroup_set( pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get() );
		pRow_EventHandler->Table_EventHandler_get()->Commit();
		ResetEventHandler_psc_mod(pRow_EventHandler);
	}

	pCommandGroup=CreateSunriseCommandGroup(commandGroupArray);
	pRow_EventHandler=CreateSunriseEventHandler(commandGroupArray);
	if( pRow_EventHandler )  // The user didn't change it, so go ahead and confirm it's current
	{
		if( pCommandGroup )
			pRow_EventHandler->FK_CommandGroup_set( pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get() );
		pRow_EventHandler->Table_EventHandler_get()->Commit();
		ResetEventHandler_psc_mod(pRow_EventHandler);
	}

	// Create a handler for all the security breaches
	int iSecurityAlerts[] = {EVENT_Security_Breach_CONST,EVENT_Fire_Alarm_CONST,EVENT_Reset_Alarm_CONST};
	for(int iCount=0;iCount<3;iCount++)
	{
		pCommandGroup=CreateSecurityAlertCommandGroup(commandGroupArray,iSecurityAlerts[iCount]);
		pRow_EventHandler=CreateGenericEventHandler(commandGroupArray,TEMPLATE_Security_Events_CONST,2,iSecurityAlerts[iCount],"Security Event " + StringUtils::itos(iSecurityAlerts[iCount]),iSecurityAlerts[iCount]);
		if( pRow_EventHandler )  // The user didn't change it, so go ahead and confirm it's current
		{
			if( pCommandGroup )
				pRow_EventHandler->FK_CommandGroup_set( pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get() );
			pRow_EventHandler->Table_EventHandler_get()->Commit();
			ResetEventHandler_psc_mod(pRow_EventHandler);
		}
	}

	m_pDatabase_pluto_main->EventHandler_get()->Commit();
	m_pDatabase_pluto_main->Criteria_get()->Commit();
	m_pDatabase_pluto_main->CriteriaParmNesting_get()->Commit();
	m_pDatabase_pluto_main->CriteriaParm_get()->Commit();

	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( pRow_Room && pRow_Room->FK_RoomType_get()!=ROOMTYPE_Unmanaged_CONST)
			AddDefaultEventHandlers(pRow_Room);
	}
}

void UpdateEntArea::ResetEventHandler_psc_mod(Row_EventHandler *pRow_EventHandler)
{
		m_pDatabase_pluto_main->threaded_db_wrapper_query(
			"UPDATE EventHandler set psc_mod=0 WHERE PK_EventHandler=" + 
			StringUtils::itos(pRow_EventHandler->PK_EventHandler_get()));
}

CommandGroup *UpdateEntArea::CreateLeaveHomeCommandGroup(CommandGroupArray &commandGroupArray)
{
/*
	interior lights off
	media off
	all users to away from home
	arm any security interfaces
*/
	int iOrder=1;
    CommandGroup *pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Security_Events_CONST,"Leave House",0,1,1); // 1,1 = leave house
	if( !pCommandGroup )
		return NULL;

	map<int,pair<int,int> > map_Device_Type_RoomType;
	GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Lighting_Device_CONST,&map_Device_Type_RoomType);

	for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
	{
		if( IsInteriorRoom(it->second.second) )
			pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);
	}

	map_Device_Type_RoomType.clear();
	GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Media_Director_CONST,&map_Device_Type_RoomType);

	for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
		pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);

	iOrder=1000;
	vector<Row_EntertainArea *> vectRow_EntertainArea;
	m_pDatabase_pluto_main->EntertainArea_get()->GetRows("1=1 ORDER BY PK_EntertainArea",&vectRow_EntertainArea);
	for(size_t s=0;s<vectRow_EntertainArea.size();++s)
		pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Stop_Media_CONST,iOrder++,1,
			COMMANDPARAMETER_PK_EntertainArea_CONST,StringUtils::itos(vectRow_EntertainArea[s]->PK_EntertainArea_get()).c_str());

	iOrder=2000;
	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("1=1 ORDER BY PK_Users",&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
	{
		Row_Users *pRow_Users = vectRow_Users[s];
		pCommandGroup->AddCommand(m_dwPK_Device_TelecomPlugIn,COMMAND_Set_User_Mode_CONST,iOrder++,2,
			COMMANDPARAMETER_PK_Users_CONST,StringUtils::itos(pRow_Users->PK_Users_get()).c_str(),
			COMMANDPARAMETER_PK_UserMode_CONST,StringUtils::itos(USERMODE_Away_CONST).c_str());
	}

	return pCommandGroup;
}

Row_EventHandler *UpdateEntArea::CreateLeaveHomeEventHandler(CommandGroupArray &commandGroupArray)
{
	vector<Row_EventHandler *> vectRow_EventHandler;
	string sSQL = "FK_Template=" + StringUtils::itos(TEMPLATE_Security_Events_CONST) + 
		" AND TemplateParm1=1 AND TemplateParm2=1";
	
	// There should only be 1
	m_pDatabase_pluto_main->EventHandler_get()->GetRows(sSQL,&vectRow_EventHandler);
	for(size_t s=1;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())==0 )
			pRow_EventHandler->Delete();
	}

	Row_EventHandler *pRow_EventHandler;
	if( vectRow_EventHandler.size() )
	{
		pRow_EventHandler = vectRow_EventHandler[0];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())!=0 )
			return NULL;
	}
	else	// There is no such event handler.  Create it
		pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->AddRow();

	pRow_EventHandler->Description_set("Leave house");
	pRow_EventHandler->FK_Event_set(EVENT_House_Mode_Changed_CONST);
	pRow_EventHandler->FK_Installation_set(m_iPK_Installation);
	pRow_EventHandler->FK_Template_set(TEMPLATE_Security_Events_CONST);
	pRow_EventHandler->TemplateParm1_set(1);
	pRow_EventHandler->TemplateParm2_set(1);
	pRow_EventHandler->UserCreated_set(0);

	return pRow_EventHandler;
}

Row_Criteria *UpdateEntArea::SetLeaveHomeCriteria(Row_EventHandler *pRow_EventHandler)
{
	// (				// NewMode
	//		NewHouseMode (#39 EVENTPARAMETER_PK_HouseMode_CONST) = HOUSEMODE_Armed_away_CONST
	//	||	NewHouseMode (#39 EVENTPARAMETER_PK_HouseMode_CONST) = HOUSEMODE_Armed_Extended_away_CONST
	// )
	// AND
	// (				// Existing Mode
	//		HouseMode = HOUSEMODE_Unarmed_at_home_CONST
	//	||	HouseMode = HOUSEMODE_Armed_at_home_CONST
	//	||	HouseMode = HOUSEMODE_Sleeping_CONST
	//	||	HouseMode = HOUSEMODE_Entertaining_CONST
	// )

	CriteriaParm *pCriteriaParm;

	Row_Criteria *pRow_Criteria = pRow_EventHandler->FK_Criteria_getrow();
	Row_CriteriaParmNesting *pRow_CriteriaParmNesting = NULL;
	if( pRow_Criteria )
		if( atoi(pRow_Criteria->psc_mod_get().c_str()) )
			return NULL; // User made changes
		else
			pRow_CriteriaParmNesting = pRow_Criteria->FK_CriteriaParmNesting_getrow();

	if( !pRow_CriteriaParmNesting )
		pRow_CriteriaParmNesting = m_pDatabase_pluto_main->CriteriaParmNesting_get()->AddRow();
	else if( atoi( pRow_CriteriaParmNesting->psc_mod_get().c_str() ) )
		return NULL;

	CriteriaParmNesting *p_criteriaParmNesting_Top = 
		new CriteriaParmNesting(true,pRow_CriteriaParmNesting);  // The And

	// The first nested
	CriteriaParmNesting *p_criteriaParmNesting_NewMode =
		new CriteriaParmNesting(false,p_criteriaParmNesting_Top->m_vectRow_CriteriaParmNesting_get(0));
	p_criteriaParmNesting_Top->m_dequeCriteriaParmNesting.push_back(p_criteriaParmNesting_NewMode);

	pCriteriaParm = p_criteriaParmNesting_NewMode->new_CriteriaParm(CRITERIAPARMLIST_PK_EventParameter_CONST,
		StringUtils::itos(EVENTPARAMETER_PK_HouseMode_CONST),operatorEquals,
		StringUtils::itos(HOUSEMODE_Armed_away_CONST));

	pCriteriaParm = p_criteriaParmNesting_NewMode->new_CriteriaParm(CRITERIAPARMLIST_PK_EventParameter_CONST,
		StringUtils::itos(EVENTPARAMETER_PK_HouseMode_CONST),operatorEquals,
		StringUtils::itos(HOUSEMODE_Armed_Extended_away_CONST));

	// The second nested
	CriteriaParmNesting *p_criteriaParmNesting_CurrentMode =
		new CriteriaParmNesting(false,p_criteriaParmNesting_Top->m_vectRow_CriteriaParmNesting_get(1));
	p_criteriaParmNesting_Top->m_dequeCriteriaParmNesting.push_back(p_criteriaParmNesting_CurrentMode);

	pCriteriaParm = p_criteriaParmNesting_CurrentMode->new_CriteriaParm(CRITERIAPARMLIST_House_Mode_CONST,
		"",operatorEquals,
		StringUtils::itos(HOUSEMODE_Unarmed_at_home_CONST));

	pCriteriaParm = p_criteriaParmNesting_CurrentMode->new_CriteriaParm(CRITERIAPARMLIST_House_Mode_CONST,
		"",operatorEquals,
		StringUtils::itos(HOUSEMODE_Armed_at_home_CONST));

	pCriteriaParm = p_criteriaParmNesting_CurrentMode->new_CriteriaParm(CRITERIAPARMLIST_House_Mode_CONST,
		"",operatorEquals,
		StringUtils::itos(HOUSEMODE_Sleeping_CONST));

	pCriteriaParm = p_criteriaParmNesting_CurrentMode->new_CriteriaParm(CRITERIAPARMLIST_House_Mode_CONST,
		"",operatorEquals,
		StringUtils::itos(HOUSEMODE_Entertaining_CONST));

	if( p_criteriaParmNesting_Top->Commit(NULL) )  // Returns false if the user made any changes
	{
		if( !pRow_Criteria )
			pRow_Criteria = m_pDatabase_pluto_main->Criteria_get()->AddRow();
		pRow_Criteria->FK_CriteriaParmNesting_set(p_criteriaParmNesting_Top->m_pRow_CriteriaParmNesting->PK_CriteriaParmNesting_get());
		pRow_Criteria->FK_CriteriaList_set(CRITERIALIST_Events_CONST);
		pRow_Criteria->FK_Installation_set(m_iPK_Installation);
		pRow_Criteria->Description_set("Leave Home");
		pRow_Criteria->Define_set("");
		m_pDatabase_pluto_main->Criteria_get()->Commit();

		m_pDatabase_pluto_main->threaded_db_wrapper_query(
			"UPDATE Criteria set psc_mod=0 WHERE PK_Criteria=" + 
			StringUtils::itos(pRow_Criteria->PK_Criteria_get()));

		pRow_EventHandler->FK_Criteria_set( pRow_Criteria->PK_Criteria_get() );
		return pRow_Criteria;
	}
	else
		return NULL;
}

CommandGroup *UpdateEntArea::CreateSunsetCommandGroup(CommandGroupArray &commandGroupArray)
{
	CommandGroup *pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Sunset",0,0,0);  // Sunset event
	if( pCommandGroup )
	{
		map<int,pair<int,int> > map_Device_Type_RoomType;
		GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Lighting_Device_CONST,&map_Device_Type_RoomType);
		int iOrder=1;
		for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
		{
			if( IsExterior(it->second.second) )
				pCommandGroup->AddCommand(it->first,COMMAND_Generic_On_CONST,iOrder++,0);
		}
	}
	return pCommandGroup;
}

Row_EventHandler *UpdateEntArea::CreateSunsetEventHandler(CommandGroupArray &commandGroupArray)
{
	vector<Row_EventHandler *> vectRow_EventHandler;
	string sSQL = "FK_Template=" + StringUtils::itos(TEMPLATE_Lighting_Wizard_CONST) + 
		" AND TemplateParm1=0 AND TemplateParm2=0";
	
	// There should only be 1
	m_pDatabase_pluto_main->EventHandler_get()->GetRows(sSQL,&vectRow_EventHandler);
	for(size_t s=1;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())==0 )
			pRow_EventHandler->Delete();
	}

	Row_EventHandler *pRow_EventHandler;
	if( vectRow_EventHandler.size() )
	{
		pRow_EventHandler = vectRow_EventHandler[0];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())!=0 )
			return NULL;
	}
	else	// There is no such event handler.  Create it
		pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->AddRow();

	pRow_EventHandler->Description_set("Sunset");
	pRow_EventHandler->FK_Event_set(EVENT_Sunset_CONST);
	pRow_EventHandler->FK_Installation_set(m_iPK_Installation);
	pRow_EventHandler->FK_Template_set(TEMPLATE_Lighting_Wizard_CONST);
	pRow_EventHandler->TemplateParm1_set(0);
	pRow_EventHandler->TemplateParm2_set(0);
	pRow_EventHandler->UserCreated_set(0);

	return pRow_EventHandler;
}

CommandGroup *UpdateEntArea::CreateSunriseCommandGroup(CommandGroupArray &commandGroupArray)
{
	CommandGroup *pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Lighting_Automatic_CONST,"Sunrise",0,1,0);  // Sunrise event
	if( pCommandGroup )
	{
		map<int,pair<int,int> > map_Device_Type_RoomType;
		GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Lighting_Device_CONST,&map_Device_Type_RoomType);
		int iOrder=1;
		for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
		{
			if( IsExterior(it->second.second) )
				pCommandGroup->AddCommand(it->first,COMMAND_Generic_Off_CONST,iOrder++,0);
		}
	}
	return pCommandGroup;
}

Row_EventHandler *UpdateEntArea::CreateSunriseEventHandler(CommandGroupArray &commandGroupArray)
{
	vector<Row_EventHandler *> vectRow_EventHandler;
	string sSQL = "FK_Template=" + StringUtils::itos(TEMPLATE_Lighting_Wizard_CONST) + 
		" AND TemplateParm1=1 AND TemplateParm2=0";
	
	// There should only be 1
	m_pDatabase_pluto_main->EventHandler_get()->GetRows(sSQL,&vectRow_EventHandler);
	for(size_t s=1;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())==0 )
			pRow_EventHandler->Delete();
	}

	Row_EventHandler *pRow_EventHandler;
	if( vectRow_EventHandler.size() )
	{
		pRow_EventHandler = vectRow_EventHandler[0];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())!=0 )
			return NULL;
	}
	else	// There is no such event handler.  Create it
		pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->AddRow();

	pRow_EventHandler->Description_set("Sunrise");
	pRow_EventHandler->FK_Event_set(EVENT_Sunrise_CONST);
	pRow_EventHandler->FK_Installation_set(m_iPK_Installation);
	pRow_EventHandler->FK_Template_set(TEMPLATE_Lighting_Wizard_CONST);
	pRow_EventHandler->TemplateParm1_set(1);
	pRow_EventHandler->TemplateParm2_set(0);
	pRow_EventHandler->UserCreated_set(0);

	return pRow_EventHandler;
}

void UpdateEntArea::AddDefaultEventHandlers(Row_Room *pRow_Room)
{
	CommandGroup *pCommandGroup;
	Row_EventHandler *pRow_EventHandler;
	CommandGroupArray commandGroupArray(pRow_Room,ARRAY_Scenarios_for_Event_Handlers_CONST,true);

	map<int,int> map_Device_Type_TV; // Where type is a floorplan type
	GetDevicesTypes(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST,pRow_Room->PK_Room_get(),
		&map_Device_Type_TV,NULL);

	if( map_Device_Type_TV.size() )
	{
		// Watching media
		pCommandGroup=CreateWatchingMediaCommandGroup(commandGroupArray,pRow_Room,1);
		pRow_EventHandler=CreateWatchingMediaEventHandler(commandGroupArray,pRow_Room,1);
		if( pRow_EventHandler )  // The user didn't change it, so go ahead and confirm it's current
		{
			if( pCommandGroup )
				pRow_EventHandler->FK_CommandGroup_set( pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get() );
			SetWatchingMediaCriteria(pRow_EventHandler, pRow_Room->FK_RoomType_get());
			pRow_EventHandler->Table_EventHandler_get()->Commit();
			ResetEventHandler_psc_mod(pRow_EventHandler);
		}

		// Stop Watching
		pCommandGroup=CreateWatchingMediaCommandGroup(commandGroupArray,pRow_Room,0);
		pRow_EventHandler=CreateWatchingMediaEventHandler(commandGroupArray,pRow_Room,0);
		if( pRow_EventHandler )  // The user didn't change it, so go ahead and confirm it's current
		{
			if( pCommandGroup )
				pRow_EventHandler->FK_CommandGroup_set( pCommandGroup->m_pRow_CommandGroup->PK_CommandGroup_get() );
			SetWatchingMediaCriteria(pRow_EventHandler, pRow_Room->FK_RoomType_get());
			pRow_EventHandler->Table_EventHandler_get()->Commit();
			ResetEventHandler_psc_mod(pRow_EventHandler);
		}
	}
}

CommandGroup *UpdateEntArea::CreateWatchingMediaCommandGroup(CommandGroupArray &commandGroupArray,Row_Room *pRow_Room,int iIsWatching)
{
	string sDescription;
	if( iIsWatching )
		sDescription = "Watching - " + pRow_Room->Description_get();
	else
		sDescription = "Not Watching - " + pRow_Room->Description_get();

    CommandGroup *pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Media_Events_CONST,sDescription,0,pRow_Room->PK_Room_get(),iIsWatching); 
	if( !pCommandGroup )
		return NULL;

	Row_CommandGroup *pRow_CommandGroup_Execute;
	if( iIsWatching )
		pRow_CommandGroup_Execute = commandGroupArray.FindCommandGroupByTemplate(pRow_Room,ARRAY_Lighting_Scenarios_CONST,TEMPLATE_Lighting_Automatic_CONST,2,0);  // Showtime is parm1=2
	else
		pRow_CommandGroup_Execute = commandGroupArray.FindCommandGroupByTemplate(pRow_Room,ARRAY_Lighting_Scenarios_CONST,TEMPLATE_Lighting_Automatic_CONST,1,0);

	if( pRow_CommandGroup_Execute )
		pCommandGroup->AddCommand(m_dwPK_Device_DCERouter,COMMAND_Execute_Command_Group_CONST,0,1,
			COMMANDPARAMETER_PK_CommandGroup_CONST,StringUtils::itos(pRow_CommandGroup_Execute->PK_CommandGroup_get()).c_str());

	return pCommandGroup;
}

Row_EventHandler *UpdateEntArea::CreateWatchingMediaEventHandler(CommandGroupArray &commandGroupArray,Row_Room *pRow_Room,int iIsWatching)
{
	vector<Row_EventHandler *> vectRow_EventHandler;
	string sSQL = "FK_Template=" + StringUtils::itos(TEMPLATE_Media_Events_CONST) + 
		" AND TemplateParm1=" + StringUtils::itos(pRow_Room->PK_Room_get()) + " AND TemplateParm2=" +
		StringUtils::itos(iIsWatching);
	
	// There should only be 1
	m_pDatabase_pluto_main->EventHandler_get()->GetRows(sSQL,&vectRow_EventHandler);
	for(size_t s=1;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())==0 )
			pRow_EventHandler->Delete();
	}

	Row_EventHandler *pRow_EventHandler;
	if( vectRow_EventHandler.size() )
	{
		pRow_EventHandler = vectRow_EventHandler[0];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())!=0 )
			return NULL;
	}
	else	// There is no such event handler.  Create it
		pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->AddRow();

	string sDescription;
	if( iIsWatching )
		sDescription = "Watching - " + pRow_Room->Description_get();
	else
		sDescription = "Not Watching - " + pRow_Room->Description_get();

	pRow_EventHandler->Description_set(sDescription);
	pRow_EventHandler->FK_Event_set(iIsWatching ? EVENT_Watching_Media_CONST : EVENT_Stopped_Watching_Media_CONST);
	pRow_EventHandler->FK_Installation_set(m_iPK_Installation);
	pRow_EventHandler->FK_Template_set(TEMPLATE_Media_Events_CONST);
	pRow_EventHandler->TemplateParm1_set(pRow_Room->PK_Room_get());
	pRow_EventHandler->TemplateParm2_set(iIsWatching);
	pRow_EventHandler->UserCreated_set(0);

	return pRow_EventHandler;
}

Row_Criteria *UpdateEntArea::SetWatchingMediaCriteria(Row_EventHandler *pRow_EventHandler, long int FK_RoomType)
{
	// (				// NewMode
	//		Room (#39 EVENTPARAMETER_PK_Room_CONST) = Room
	// )

	CriteriaParm *pCriteriaParm;

	Row_Criteria *pRow_Criteria = pRow_EventHandler->FK_Criteria_getrow();
	Row_CriteriaParmNesting *pRow_CriteriaParmNesting = NULL;
	if( pRow_Criteria )
		if( atoi(pRow_Criteria->psc_mod_get().c_str()) )
			return NULL; // User made changes
		else
			pRow_CriteriaParmNesting = pRow_Criteria->FK_CriteriaParmNesting_getrow();

	if( !pRow_CriteriaParmNesting )
		pRow_CriteriaParmNesting = m_pDatabase_pluto_main->CriteriaParmNesting_get()->AddRow();
	else if( atoi( pRow_CriteriaParmNesting->psc_mod_get().c_str() ) )
		return NULL;

	CriteriaParmNesting *p_criteriaParmNesting_Top = 
		new CriteriaParmNesting(true,pRow_CriteriaParmNesting);  // The And

	pCriteriaParm = p_criteriaParmNesting_Top->new_CriteriaParm(CRITERIAPARMLIST_PK_EventParameter_CONST,
		StringUtils::itos(EVENTPARAMETER_PK_Room_CONST),operatorEquals,
		StringUtils::itos(pRow_EventHandler->TemplateParm1_get()));

	// For all room types except home theatre, only turn on and off the lights if it is night time
	if (FK_RoomType != ROOMTYPE_Home_Theater_CONST) {
		pCriteriaParm = p_criteriaParmNesting_Top->new_CriteriaParm(CRITERIAPARMLIST_Time_of_day_CONST,
									    "",operatorEquals, "NIGHT");
	}

	if( p_criteriaParmNesting_Top->Commit(NULL) )  // Returns false if the user made any changes
	{
		if( !pRow_Criteria )
			pRow_Criteria = m_pDatabase_pluto_main->Criteria_get()->AddRow();
		pRow_Criteria->FK_CriteriaParmNesting_set(p_criteriaParmNesting_Top->m_pRow_CriteriaParmNesting->PK_CriteriaParmNesting_get());
		pRow_Criteria->FK_CriteriaList_set(CRITERIALIST_Events_CONST);
		pRow_Criteria->FK_Installation_set(m_iPK_Installation);
		pRow_Criteria->Define_set("");
		if( pRow_EventHandler->FK_Event_get()==EVENT_Watching_Media_CONST )
			pRow_Criteria->Description_set("Watching Media");
		else
			pRow_Criteria->Description_set("Stop Watching");
		m_pDatabase_pluto_main->Criteria_get()->Commit();

		m_pDatabase_pluto_main->threaded_db_wrapper_query(
			"UPDATE Criteria set psc_mod=0 WHERE PK_Criteria=" + 
			StringUtils::itos(pRow_Criteria->PK_Criteria_get()));

		pRow_EventHandler->FK_Criteria_set( pRow_Criteria->PK_Criteria_get() );
		return pRow_Criteria;
	}
	else
		return NULL;
}

CommandGroup *UpdateEntArea::CreateSecurityAlertCommandGroup(CommandGroupArray &commandGroupArray,int PK_Event)
{
	int iOrder=1;
	CommandGroup *pCommandGroup = commandGroupArray.FindCommandGroupByTemplate(TEMPLATE_Security_Events_CONST,"Security Event " + StringUtils::itos(PK_Event),0,2,PK_Event); // 2=security events
	if( !pCommandGroup )
		return NULL;

	for(map<int, LevelOfMedia >::iterator it=m_mapEnt_Area_Auto_Media.begin();it!=m_mapEnt_Area_Auto_Media.end();++it)
	{
		string sFile;

		if( PK_Event==EVENT_Security_Breach_CONST && (it->second ==lomContainsMD || it->second ==lomContainsOtherVideo) )
			sFile="/home/public/data/samples/security/security.mpg";
		else if( PK_Event==EVENT_Security_Breach_CONST )
			sFile="/home/public/data/samples/security/security.mp3";
		else if( PK_Event==EVENT_Fire_Alarm_CONST && (it->second ==lomContainsMD || it->second ==lomContainsOtherVideo) )
			sFile="/home/public/data/samples/security/fire.mpg";
		else if( PK_Event==EVENT_Fire_Alarm_CONST )
			sFile="/home/public/data/samples/security/fire.mp3";

		//always stop the media before playing an alert
		pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Stop_Media_CONST,iOrder++,1,
			COMMANDPARAMETER_PK_EntertainArea_CONST,StringUtils::itos(it->first).c_str());

		pCommandGroup->AddCommand(m_dwPK_Device_MediaPlugIn,COMMAND_MH_Play_Media_CONST,iOrder++,3,
			COMMANDPARAMETER_Bypass_Event_CONST,"1",
			COMMANDPARAMETER_PK_EntertainArea_CONST,StringUtils::itos(it->first).c_str(),
			COMMANDPARAMETER_Filename_CONST,sFile.c_str());
	}

	// Turn on all lights if this is not just resetting the alarm
	if( PK_Event!=EVENT_Reset_Alarm_CONST )
	{
		map<int,pair<int,int> > map_Device_Type_RoomType;
		GetDevicesTypesAndRoomTypes(DEVICECATEGORY_Lighting_Device_CONST,&map_Device_Type_RoomType);
		for(map<int,pair<int,int> >::iterator it=map_Device_Type_RoomType.begin();it!=map_Device_Type_RoomType.end();++it)
			pCommandGroup->AddCommand(it->first,COMMAND_Generic_On_CONST,iOrder++,0);
	}

	return pCommandGroup;
}

Row_EventHandler *UpdateEntArea::CreateGenericEventHandler(CommandGroupArray &commandGroupArray,int PK_Template,int Parm1,int Parm2,string sDescription,int PK_Event)
{
	vector<Row_EventHandler *> vectRow_EventHandler;
	string sSQL = "FK_Template=" + StringUtils::itos(PK_Template) + 
		" AND TemplateParm1=" + StringUtils::itos(Parm1) + " AND TemplateParm2=" + StringUtils::itos(Parm2) + "";
	
	// There should only be 1
	m_pDatabase_pluto_main->EventHandler_get()->GetRows(sSQL,&vectRow_EventHandler);
	for(size_t s=1;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())==0 )
			pRow_EventHandler->Delete();
	}

	Row_EventHandler *pRow_EventHandler;
	if( vectRow_EventHandler.size() )
	{
		pRow_EventHandler = vectRow_EventHandler[0];
		if( atoi(pRow_EventHandler->psc_mod_get().c_str())!=0 )
			return NULL;
	}
	else	// There is no such event handler.  Create it
		pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->AddRow();

	pRow_EventHandler->Description_set(sDescription);
	pRow_EventHandler->FK_Event_set(PK_Event);
	pRow_EventHandler->FK_Installation_set(m_iPK_Installation);
	pRow_EventHandler->FK_Template_set(PK_Template);
	pRow_EventHandler->TemplateParm1_set(Parm1);
	pRow_EventHandler->TemplateParm2_set(Parm2);
	pRow_EventHandler->UserCreated_set(0);

	return pRow_EventHandler;
}

