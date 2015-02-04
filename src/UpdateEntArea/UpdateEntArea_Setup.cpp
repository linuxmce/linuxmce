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
#include "PlutoUtils/DatabaseUtils.h"
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

#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_Device_DeviceData.h"

#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_DeviceData.h"

using namespace std;
using namespace DCE;
using namespace DefaultScenarios;

bool UpdateEntArea::Connect(int PK_Installation,string host, string user, string pass, string db_name, int port)
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Starting UpdateEnt Areas" );
	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if( !m_pDatabase_pluto_main->Connect( host, user, pass, db_name, port ) )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
		return false;
	}
	m_iPK_Installation=PK_Installation;

	m_pCreateDevice = new CreateDevice(m_iPK_Installation,host,user,pass,db_name,port);

	// ******** Media Plugin ******************
	vector<Row_Device *> vectRow_Device;
	string sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Media_Plugins_CONST);
	if( m_iPK_Installation!=-1 )
		sql += " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);
	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a media plugin" );
		return false; // It's useless without one of these
	}
	m_dwPK_Device_MediaPlugIn=vectRow_Device[0]->PK_Device_get();
	m_iPK_Installation = vectRow_Device[0]->FK_Installation_get();

	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get()->GetRow(m_iPK_Installation);
	if( !pRow_Installation )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Got Media Plugin %d but installation %d is invalid",
			m_dwPK_Device_MediaPlugIn,m_iPK_Installation);
		return false;
	}
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Using Media Plugin %d Installation %d",
			m_dwPK_Device_MediaPlugIn,m_iPK_Installation);

	// ******** Orbiter Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Orbiter_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a Orbiter plugin" );
		return false; // It's useless without one of these
	}
	else
		m_dwPK_Device_OrbiterPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Lighting Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Lighting_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a lighting plugin" );
		m_dwPK_Device_LightingPlugIn=0;
	}
	else
		m_dwPK_Device_LightingPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Climate Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Climate_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a Climate plugin" );
		m_dwPK_Device_ClimatePlugIn=0;
	}
	else
		m_dwPK_Device_ClimatePlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Security Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Security_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a Security plugin" );
		m_dwPK_Device_SecurityPlugIn=0;
	}
	else
		m_dwPK_Device_SecurityPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** Telecom Plugin ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_Telecom_Plugins_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a Telecom plugin" );
		m_dwPK_Device_TelecomPlugIn=0;
	}
	else
		m_dwPK_Device_TelecomPlugIn=vectRow_Device[0]->PK_Device_get();

	// ******** DCERouter ******************
	vectRow_Device.clear();
	sql = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		StringUtils::itos(DEVICECATEGORY_DCE_Router_CONST) + " AND FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	m_pDatabase_pluto_main->Device_get()->GetRows(sql,&vectRow_Device);
	if( vectRow_Device.size()!=1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find a DCERouter" );
		m_dwPK_Device_DCERouter=0;
	}
	else
		m_dwPK_Device_DCERouter=vectRow_Device[0]->PK_Device_get();
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Connect/setup ok" );
	return true;
}

void UpdateEntArea::FixMissingAutoCreateDevices()
{
	// Find any auto create children specified in DeviceTemplate_DeviceTemplate_ControlledVia that aren't present
	string sSQL="SELECT DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate,RerouteMessagesToParent,PK_DeviceTemplate_DeviceTemplate_ControlledVia,Parent.PK_Device FROM "
		"DeviceTemplate_DeviceTemplate_ControlledVia "
		"JOIN Device As Parent ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate_ControlledVia=Parent.FK_DeviceTemplate "
		"LEFT JOIN Device As Child ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate = Child.FK_DeviceTemplate AND Child.FK_Device_ControlledVia=Parent.PK_Device "
		"WHERE Child.PK_Device IS NULL AND AutoCreateChildren=1";

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
			m_pCreateDevice->CreateAutoCreateChildDevice(atoi(row[3]),atoi(row[0]),row[1] && atoi(row[1]),atoi(row[2]),0);

	// Find any auto create children specified in DeviceTemplate_DeviceCategory_ControlledVia that aren't present
	sSQL="SELECT DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceTemplate,RerouteMessagesToParent,PK_DeviceTemplate_DeviceCategory_ControlledVia,Parent.PK_Device FROM "
		"DeviceTemplate_DeviceCategory_ControlledVia "
		"JOIN DeviceCategory AS C1 ON C1.PK_DeviceCategory=DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceCategory "
		"LEFT JOIN DeviceCategory AS C2 ON C2.FK_DeviceCategory_Parent=C1.PK_DeviceCategory "
		"LEFT JOIN DeviceCategory AS C3 ON C3.FK_DeviceCategory_Parent=C2.PK_DeviceCategory "
		"JOIN DeviceTemplate ON DeviceTemplate.FK_DeviceCategory=C1.PK_DeviceCategory OR DeviceTemplate.FK_DeviceCategory=C2.PK_DeviceCategory OR DeviceTemplate.FK_DeviceCategory=C3.PK_DeviceCategory OR DeviceTemplate.FK_DeviceCategory=C3.FK_DeviceCategory_Parent "
		"JOIN Device As Parent ON DeviceTemplate.PK_DeviceTemplate=Parent.FK_DeviceTemplate "
		"LEFT JOIN Device As Child ON DeviceTemplate_DeviceCategory_ControlledVia.FK_DeviceTemplate = Child.FK_DeviceTemplate AND Child.FK_Device_ControlledVia=Parent.PK_Device "
		"WHERE Child.PK_Device IS NULL AND AutoCreateChildren=1";

	PlutoSqlResult result_set2;
	if( (result_set2.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set2.r)))
			m_pCreateDevice->CreateAutoCreateChildDevice(atoi(row[3]),atoi(row[0]),row[1] && atoi(row[1]),0,atoi(row[2]));
}

void UpdateEntArea::GetMediaAndRooms()
{
	// Be sure child devices are in the same installation as the parent
	string sSQL="UPDATE Device "
		" JOIN Device AS Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device"
		" SET Device.FK_Installation=" + StringUtils::itos(m_iPK_Installation) +
		" WHERE Parent.FK_Installation=" + StringUtils::itos(m_iPK_Installation);

	// Keep doing this in case any nested rows missing the installation
	while( m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL)>0 );

	// Be sure hybrid media director's are in the same room as the core
	sSQL = "UPDATE Device "
		" JOIN Device AS Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia "
		" JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate "
		" JOIN DeviceTemplate AS DeviceTemplate_Parent ON Parent.FK_DeviceTemplate=DeviceTemplate_Parent.PK_DeviceTemplate "
		" SET Device.FK_Room = Parent.FK_Room "
		" WHERE DeviceTemplate.FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Media_Director_CONST) +
		" AND DeviceTemplate_Parent.FK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Core_CONST);
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);

	// Be sure devices with auto assign to parent's room are in the same room as the parents
	sSQL = "UPDATE Device "
		"JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate AND FK_DeviceData=" TOSTRING(DEVICEDATA_Autoassign_to_parents_room_CONST) " "
		"LEFT JOIN Device As Parent ON Parent.PK_Device=Device.FK_Device_ControlledVia "
		"SET Device.FK_Room=Parent.FK_Room "
		"WHERE IK_DeviceData=1";
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);

	// Get all the entertainment areas and populate them with all the devices in those areas
	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get( )->GetRow( m_iPK_Installation );
	m_mapRoom_Media.clear();

	if( !pRow_Installation )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"UpdateEntArea::GetMediaAndRooms cannot continue with invalid installation %d", m_iPK_Installation);
		return;
	}

	vector<Row_Room *> vectRow_Room; // Ent Areas are specified by room. Get all the rooms first
	pRow_Installation->Room_FK_Installation_getrows( &vectRow_Room );
	for( size_t iRoom=0;iRoom<vectRow_Room.size( );++iRoom )
	{
		Row_Room *pRow_Room=vectRow_Room[iRoom];
		LevelOfMedia levelOfMedia=lomNone;

		vector<Row_Device *> vectRow_Device; 
		pRow_Room->Device_FK_Room_getrows(&vectRow_Device);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Room: %d %s is auto configure",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());

		// We'll want to configure this room's entertainment area automatically.  See if there's a media director in the room
		for(size_t s=0;s<vectRow_Device.size();++s)
		{
			Row_Device *pRow_Device = vectRow_Device[s];

			// Check up 3 generations of DeviceCategories to see if this is a media director
			if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Director_CONST ||
				pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST ||
				(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
				pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Media_Director_CONST) )
			{
				levelOfMedia = lomContainsMD;
				if( !pRow_Room->ManuallyConfigureEA_get() )
					PutMDsChildrenInRoom(pRow_Device);
			}

                        // Check up 3 generations of DeviceCategories to see if this is a mobile internet device
                        if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Mobile_Internet_Devices_CONST ||
                                pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Mobile_Internet_Devices_CONST ||
                                (pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
                                pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Mobile_Internet_Devices_CONST) )
                        {
                                levelOfMedia = lomContainsMD;
                                if( !pRow_Room->ManuallyConfigureEA_get() )
                                        PutMDsChildrenInRoom(pRow_Device);
                        }

                        if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Workstations_CONST ||
                                pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Workstations_CONST ||
                                (pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
                                pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_Workstations_CONST) )
                        {
                                levelOfMedia = lomContainsWorkstation;
                                if( !pRow_Room->ManuallyConfigureEA_get() )
                                        PutMDsChildrenInRoom(pRow_Device);
                        }

		}

		if( levelOfMedia==lomNone )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Room: %d %s has no md",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
			// See if there is some other video device

			for(size_t s=0;s<vectRow_Device.size();++s)
			{
				Row_Device *pRow_Device = vectRow_Device[s];

				// Check up 3 generations of DeviceCategories to see if this is a tv
				if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST ||
					pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST ||
					(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
					pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST) )
				{
					levelOfMedia=lomContainsOtherVideo;
					break;
				}
			}
			if( levelOfMedia==lomNone )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Room: %d %s has no video",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
				for(size_t s=0;s<vectRow_Device.size();++s)
				{
					Row_Device *pRow_Device = vectRow_Device[s];
					// Check up 3 generations of DeviceCategories to see if this is some other audio device
					if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_AV_CONST ||
						pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_AV_CONST ||
						(pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_get() &&
						pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_getrow()->FK_DeviceCategory_Parent_getrow()->FK_DeviceCategory_Parent_get()==DEVICECATEGORY_AV_CONST) )
					{
						levelOfMedia=lomContainsAudio;
						break;
					}
				}
			}
		}
		m_mapRoom_Media[pRow_Room->PK_Room_get()] = make_pair<LevelOfMedia, bool> (levelOfMedia,pRow_Room->ManuallyConfigureEA_get()==1);
	}
}

void UpdateEntArea::SetEAInRooms()
{
	for(map<int, pair<LevelOfMedia, bool> >::iterator it=m_mapRoom_Media.begin();it!=m_mapRoom_Media.end();++it)
	{
		Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(it->first);
		if( it->second.second )
			continue;  // This is a manually configure.  So do nothing

		vector<Row_EntertainArea *> vectEntertainArea;
		m_pDatabase_pluto_main->EntertainArea_get()->GetRows("FK_Room=" + StringUtils::itos(it->first),&vectEntertainArea);

		if( ( it->second.first!=lomContainsWorkstation || it->second.first!=lomContainsMD ) && vectEntertainArea.size() )
		{
			Row_EntertainArea *pRow_EntertainArea = vectEntertainArea[0];

			// Delete any 'put this device in all rooms with a md' if there's no m/d
			string sSQL = "DELETE Device_EntertainArea FROM Device_EntertainArea JOIN Device ON FK_Device=PK_Device WHERE ManuallyConfigureEA=-1 AND FK_EntertainArea=" + StringUtils::itos(pRow_EntertainArea->PK_EntertainArea_get());
			m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
		}

		if( it->second.first==lomNone )
		{
			continue;  // The user may have manually put a device in here
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Room: %d %s has media ea %d",pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str(),(int) vectEntertainArea.size());

			// There is media in this room.  Add an entertainment area and any a/v equipment to it
			// First, see if there are already too many ent areas (maybe the user changed from manual to automatic)
			for(size_t s=1;s<vectEntertainArea.size();++s)
				DeleteEntertainArea(vectEntertainArea[s]);

			Row_EntertainArea *pRow_EntertainArea = vectEntertainArea.size() ? vectEntertainArea[0] : NULL;

			if( !pRow_EntertainArea )
			{
				pRow_EntertainArea = m_pDatabase_pluto_main->EntertainArea_get()->AddRow();
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"set ent area %d %s to Room: %d %s creating default commands",pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str(),pRow_Room->PK_Room_get(),pRow_Room->Description_get().c_str());
				pRow_EntertainArea->FK_Room_set(pRow_Room->PK_Room_get());
				pRow_EntertainArea->Description_set(pRow_Room->Description_get());
				m_pDatabase_pluto_main->EntertainArea_get()->Commit();
			}
			m_mapEnt_Area_Auto_Media[pRow_EntertainArea->PK_EntertainArea_get()] = it->second.first;

			pRow_EntertainArea->Description_set(pRow_Room->Description_get());
			if( it->second.first==lomContainsMD )
				pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_PLUTO_MEDIA_DIR_CONST);
			else if( it->second.first==lomContainsOtherVideo || it->second.first==lomContainsWorkstation )
				pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_NONPLUTO_TV_CONST);
			else if( it->second.first==lomContainsAudio )
				pRow_EntertainArea->FK_FloorplanObjectType_set(FLOORPLANOBJECTTYPE_ENTERTAINMENT_AUDIO_ZONE_CONST);

			AddAVDevicesToEntArea(pRow_EntertainArea);
			AddMDsDevicesToEntArea(pRow_EntertainArea);
			AddWSsDevicesToEntArea(pRow_EntertainArea);

			// Be sure all the devices in this entertainment area are only in the one entertainment area
			vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
			pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows(&vectRow_Device_EntertainArea);
			for(size_t s=0;s<vectRow_Device_EntertainArea.size();++s)
			{
				Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s];
				if( pRow_Device_EntertainArea )
				{
					Row_Device *pRow_Device = pRow_Device_EntertainArea->FK_Device_getrow();
					if( !pRow_Device || pRow_Device->ManuallyConfigureEA_get()!=0 )
						continue;
					vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea2;
					m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRows("WHERE FK_EntertainArea<>" +
						StringUtils::itos(pRow_Device_EntertainArea->FK_EntertainArea_get()) + " AND FK_Device=" + StringUtils::itos(pRow_Device_EntertainArea->FK_Device_get()),
						&vectRow_Device_EntertainArea2);
					for(size_t s2=0;s2<vectRow_Device_EntertainArea2.size();++s2)
						vectRow_Device_EntertainArea2[s2]->Delete();
				}
			}
			m_pDatabase_pluto_main->EntertainArea_get()->Commit();
			m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
			m_pDatabase_pluto_main->Commit();
		}
	}
}

void UpdateEntArea::PutMDsChildrenInRoom(Row_Device *pRow_Device)
{
	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device_Child = vectRow_Device[s];

		// If this is not an embedded device (ie route to is null) and it's parent is an interface device, then don't 
		// put it in the same room since the children are things like lights or security sensors which may
		// be in different rooms from the interface
		if( pRow_Device_Child->FK_Device_RouteTo_isNull()==true && DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICECATEGORY_Interfaces_CONST)  )
			continue;

		// Output zones may be in different EA's from the parents
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_Child->FK_DeviceTemplate_getrow();
		if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Output_Zone_CONST )
			continue;

		pRow_Device_Child->FK_Room_set(pRow_Device->FK_Room_get());
		pRow_Device_Child->Table_Device_get()->Commit();

		PutMDsChildrenInRoom(pRow_Device_Child);
	}
}

void UpdateEntArea::DeleteEntertainArea(Row_EntertainArea *pRow_EntertainArea)
{
	vector<Row_CommandGroup_EntertainArea *> vectRow_CommandGroup_EntertainArea;
	pRow_EntertainArea->CommandGroup_EntertainArea_FK_EntertainArea_getrows(&vectRow_CommandGroup_EntertainArea);
	for(size_t s=0;s<vectRow_CommandGroup_EntertainArea.size();++s)
	{
		Row_CommandGroup_EntertainArea *pRow_CommandGroup_EntertainArea = vectRow_CommandGroup_EntertainArea[s];
		pRow_CommandGroup_EntertainArea->Delete();
	}

	vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
	pRow_EntertainArea->Device_EntertainArea_FK_EntertainArea_getrows(&vectRow_Device_EntertainArea);
	for(size_t s=0;s<vectRow_Device_EntertainArea.size();++s)
	{
		Row_Device_EntertainArea *pRow_Device_EntertainArea = vectRow_Device_EntertainArea[s];
		pRow_Device_EntertainArea->Delete();
	}

	pRow_EntertainArea->Delete();

	m_pDatabase_pluto_main->EntertainArea_get()->Commit();
	m_pDatabase_pluto_main->CommandGroup_EntertainArea_get()->Commit();
	m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
}

void UpdateEntArea::AddAVDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea)
{
	vector<Row_Device *> vectRow_Device; 

	// First fix up any embedded devices
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		vector<Row_Device *> vectRow_Device_Embedded; 
		pRow_Device->Device_FK_Device_RouteTo_getrows(&vectRow_Device_Embedded);
		for(size_t s2=0;s2<vectRow_Device_Embedded.size();++s2)
		{
			Row_Device *pRow_Device_Child = vectRow_Device_Embedded[s2];

			// Output zones may be in different EA's from the parents
			Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_Child->FK_DeviceTemplate_getrow();
			if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Output_Zone_CONST )
				continue;

			if( pRow_Device_Child->FK_Room_get()!=pRow_Device->FK_Room_get() )
			{
				pRow_Device_Child->FK_Room_set(pRow_Device->FK_Room_get());
				pRow_Device_Child->Table_Device_get()->Commit();
			}
		}
	}

	vectRow_Device.clear();
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		if( pRow_Device->ManuallyConfigureEA_get()!=0 )
			continue;

		// Check up 3 generations of DeviceCategories to see if this is av equipment
		if( DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICECATEGORY_AV_CONST) || pRow_Device->FK_DeviceTemplate_get()==DEVICETEMPLATE_App_Server_CONST )
		{
			Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
			if( !pRow_Device_EntertainArea )
			{
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "adding device %d %s to ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),
					pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
				AddDevicesToEntArea(pRow_Device,pRow_EntertainArea);
			}
		}
	}
}

void UpdateEntArea::AddMDsDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea)
{
	bool bHasMd=false;
	vector<Row_Device *> vectRow_Device; 
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		// Is this a media director?
		if( DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICECATEGORY_Media_Director_CONST) )
		{
			bHasMd=true;
			// If this is a hybrid, the m/d may be a child of the Core.  Get the parent then, so children of the core, like pvr cards
			// will also be in the same ent area
			Row_Device *pRow_Device_Parent = pRow_Device->FK_Device_ControlledVia_getrow();
			if( pRow_Device_Parent )
				pRow_Device = pRow_Device_Parent;
			AddMDsDevicesToEntArea(pRow_Device,pRow_EntertainArea); // This will recurse
		}
	}

	vectRow_Device.clear();

	if( bHasMd )
	{
		m_pDatabase_pluto_main->Device_get()->GetRows("ManuallyConfigureEA=-1",&vectRow_Device);
		for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
		{
			Row_Device *pRow_Device = *it;
			Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
			if( !pRow_Device_EntertainArea )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "adding device %d %s to all ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
				AddDevicesToEntArea(pRow_Device,pRow_EntertainArea,false);
			}
		}
	}
}

void UpdateEntArea::AddWSsDevicesToEntArea(Row_EntertainArea *pRow_EntertainArea)
{
	bool bHasWS=false;
	vector<Row_Device *> vectRow_Device; 
	pRow_EntertainArea->FK_Room_getrow()->Device_FK_Room_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		// Is this a workstation?
		if( DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICECATEGORY_Workstations_CONST) )
		{
			bHasWS=true;
			// If this is a hybrid, the m/d may be a child of the Core.  Get the parent then, so children of the core, like pvr cards
			// will also be in the same ent area
			Row_Device *pRow_Device_Parent = pRow_Device->FK_Device_ControlledVia_getrow();
			if( pRow_Device_Parent )
				pRow_Device = pRow_Device_Parent;
			AddWSsDevicesToEntArea(pRow_Device,pRow_EntertainArea); // This will recurse
		}
	}

	vectRow_Device.clear();

	if( bHasWS )
	{
		m_pDatabase_pluto_main->Device_get()->GetRows("ManuallyConfigureEA=-1",&vectRow_Device);
		for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
		{
			Row_Device *pRow_Device = *it;
			Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
			if( !pRow_Device_EntertainArea )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "adding device %d %s to all ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
				AddDevicesToEntArea(pRow_Device,pRow_EntertainArea,false);
			}
		}
	}
}

void UpdateEntArea::AddMDsDevicesToEntArea(Row_Device *pRow_Device,Row_EntertainArea *pRow_EntertainArea)
{
	if( pRow_Device->ManuallyConfigureEA_get()!=0 )  // The user is manually specifying this
		return;

	// If it's not an embedded device (ie route to is null), and the parent is an interface, then this is something like a light switch or sensor which doesn't inherit the parent's room
	if( pRow_Device->FK_Device_ControlledVia_get() && pRow_Device->FK_Device_RouteTo_isNull()==true && DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->FK_Device_ControlledVia_get(),DEVICECATEGORY_Interfaces_CONST) )
		return;

	// Output zones may be in different EA's from the parents
	Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
	if( pRow_DeviceTemplate && (pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Output_Zone_CONST || pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Network_Audio_Player_CONST || pRow_DeviceTemplate->PK_DeviceTemplate_get()==DEVICETEMPLATE_Generic_Proxy_Orbiter_CONST) )
		return;

	Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
	if( !pRow_Device_EntertainArea )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "adding device %d %s to ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
		AddDevicesToEntArea(pRow_Device,pRow_EntertainArea);
	}

	vector<Row_Device *> vectRow_Device_Child;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device_Child);
	for(vector<Row_Device *>::iterator it=vectRow_Device_Child.begin();it!=vectRow_Device_Child.end();++it)
		AddMDsDevicesToEntArea( *it, pRow_EntertainArea );
}

void UpdateEntArea::AddWSsDevicesToEntArea(Row_Device *pRow_Device,Row_EntertainArea *pRow_EntertainArea)
{
	if( pRow_Device->ManuallyConfigureEA_get()!=0 )  // The user is manually specifying this
		return;

	// If it's not an embedded device (ie route to is null), and the parent is an interface, then this is something like a light switch or sensor which doesn't inherit the parent's room
	if( pRow_Device->FK_Device_ControlledVia_get() && pRow_Device->FK_Device_RouteTo_isNull()==true && DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->FK_Device_ControlledVia_get(),DEVICECATEGORY_Interfaces_CONST) )
		return;

	// Output zones may be in different EA's from the parents
	Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device->FK_DeviceTemplate_getrow();
	if( pRow_DeviceTemplate && (pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Output_Zone_CONST || pRow_DeviceTemplate->FK_DeviceCategory_get()==DEVICECATEGORY_Network_Audio_Player_CONST || pRow_DeviceTemplate->PK_DeviceTemplate_get()==DEVICETEMPLATE_Generic_Proxy_Orbiter_CONST) )
		return;

	Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
	if( !pRow_Device_EntertainArea )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "adding device %d %s to ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());
		AddDevicesToEntArea(pRow_Device,pRow_EntertainArea);
	}

	vector<Row_Device *> vectRow_Device_Child;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device_Child);
	for(vector<Row_Device *>::iterator it=vectRow_Device_Child.begin();it!=vectRow_Device_Child.end();++it)
		AddMDsDevicesToEntArea( *it, pRow_EntertainArea );
}


void UpdateEntArea::GetDevicesTypes(int PK_DeviceCategory,int PK_Room,map<int,int> *p_map_Device_Type,map<int,list<int> > *p_mapType)
{
	string sSQL = "SELECT PK_Device,IK_DeviceData "
		"FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) +
		" WHERE (PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) +
		")";

	if( PK_Room )
		sSQL += " AND FK_Room=" + StringUtils::itos(PK_Room);
    
	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
			(*p_map_Device_Type)[ atoi(row[0]) ] = row[1] ? atoi(row[1]) : 0;

	if( p_mapType )
		for(map<int,int>::iterator it=p_map_Device_Type->begin();it!=p_map_Device_Type->end();++it)
			(*p_mapType)[it->first].push_back(it->second);
}

void UpdateEntArea::GetDevicesTypesAndRoomTypes(int PK_DeviceCategory,map<int,pair<int,int> > *p_map_Device_Type_RoomType)
{
	string sSQL = "SELECT PK_Device,IK_DeviceData,FK_RoomType "
		"FROM Device "
		"LEFT JOIN Room ON FK_Room=PK_Room "
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) +
		" WHERE (PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) + 
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) +
		")";
	
	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
			(*p_map_Device_Type_RoomType)[ atoi(row[0]) ] = make_pair<int,int> (row[1] ? atoi(row[1]) : 0,row[2] ? atoi(row[2]) : 0);
}



Row_Device_DeviceData *UpdateEntArea::GetUnmodifiedDeviceData(int PK_Device,int PK_DeviceData)
{
    Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(PK_Device,PK_DeviceData);
	if( pRow_Device_DeviceData && atoi(pRow_Device_DeviceData->psc_mod_get().c_str())!=0 && pRow_Device_DeviceData->IK_DeviceData_get()!="auto" )
		return NULL; // User made his own changes

	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(PK_Device);
		pRow_Device_DeviceData->FK_DeviceData_set(PK_DeviceData);
		m_pDatabase_pluto_main->Device_DeviceData_get()->Commit();
	}

	return pRow_Device_DeviceData;
}

void UpdateEntArea::SetDeviceData(int PK_Device,int PK_DeviceData,string sValue)
{
	string sSQL = "UPDATE Device_DeviceData SET IK_DeviceData='" + StringUtils::SQLEscape(sValue) + "',psc_mod=0" +
		" WHERE FK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + StringUtils::itos(PK_DeviceData);
	m_pDatabase_pluto_main->threaded_db_wrapper_query(sSQL);
}

void UpdateEntArea::AddDevicesToEntArea(Row_Device *pRow_Device,Row_EntertainArea *pRow_EntertainArea,bool bOneEAOnly)
{
	Row_Device_EntertainArea *pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->GetRow(pRow_Device->PK_Device_get(),pRow_EntertainArea->PK_EntertainArea_get());
	if( !pRow_Device_EntertainArea )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, " UpdateEntArea::AddDevicesToEntArea adding device %d %s to ent area %d %s",pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str(),
			pRow_EntertainArea->PK_EntertainArea_get(),pRow_EntertainArea->Description_get().c_str());

		// Before adding a new row, see if there's already one we should just change
		vector<Row_Device_EntertainArea *> vectRow_Device_EntertainArea;
		if( bOneEAOnly==true )
			pRow_Device->Device_EntertainArea_FK_Device_getrows(&vectRow_Device_EntertainArea);
		
		for(vector<Row_Device_EntertainArea *>::iterator it=vectRow_Device_EntertainArea.begin();it!=vectRow_Device_EntertainArea.end();++it)
		{
			pRow_Device_EntertainArea = *it;
			pRow_Device_EntertainArea->Delete();
			m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
		}

		pRow_Device_EntertainArea = m_pDatabase_pluto_main->Device_EntertainArea_get()->AddRow();

		pRow_Device_EntertainArea->FK_Device_set(pRow_Device->PK_Device_get());
		pRow_Device_EntertainArea->FK_EntertainArea_set(pRow_EntertainArea->PK_EntertainArea_get());
		m_pDatabase_pluto_main->Device_EntertainArea_get()->Commit();
	}
}
