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

#include "Orbiter.h"
#include "WizardLogic.h"
#include "CreateDevice/UserUtils.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Define_Country.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Variable.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/DatabaseUtils.h"

WizardLogic::WizardLogic(Orbiter *pOrbiter)
{
	m_pOrbiter = pOrbiter;
	m_pUserUtils = new UserUtils(this,m_pOrbiter->m_pData->m_dwPK_Installation);
}

WizardLogic::~WizardLogic()
{
	delete m_pUserUtils;
}

bool WizardLogic::Setup()
{
#ifdef WIN32
//	if( !DBConnect("192.168.80.1", "root", "", "pluto_main") )
	if( !DBConnect(m_pOrbiter->m_sIPAddress, "root", "", "pluto_main") )
#else
    const char *pMySqlHost = getenv("MySqlHost");
	string sHost = pMySqlHost && strlen(pMySqlHost) ? pMySqlHost : m_pOrbiter->m_sIPAddress;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"WizardLogic::Setup using host %s (%p)",sHost.c_str(),pMySqlHost);
	if( !DBConnect(sHost, "root", "", "pluto_main") )
#endif
		return false;

	string sSQL;
	DB_ROW row;

	sSQL = "SELECT PK_RoomType,Description FROM RoomType ORDER BY Description";
	{
		PlutoSqlResult result_set_roomtypes;
		if( (result_set_roomtypes.r=db_wrapper_query_result(sSQL)) )
			while ((row = db_wrapper_fetch_row(result_set_roomtypes.r)))
			{
				m_listRoomTypes.push_back( make_pair<int,string> (atoi(row[0]),row[1]) );
				m_mapRoomTypes[atoi(row[0])] = row[1];
			}
	}

	m_nPK_Device_Lighting = m_nPK_Device_AlarmPanel = 0;
	m_nPK_Device_TVProvider_External=m_nPK_Device_TV=m_nPK_Device_Receiver=
		m_nPK_Command_Input_Video_On_TV=0;
	m_bUsingReceiverForVideo=false;

	return true;
}

/*
	GREETING
*/

bool WizardLogic::HouseAlreadySetup()
{
	return AlreadyHasUsers() && AlreadyHasRooms();	
}

int WizardLogic::FindFirstDeviceInCategory(int PK_DeviceCategory,int PK_Device_RelatedTo,string *sDescription)
{
	int PK_Device_PC = PK_Device_RelatedTo ? DatabaseUtils::GetTopMostDevice(this,m_pOrbiter->m_dwPK_Device) : 0;
	string sSQL = "SELECT Device.PK_Device,Device.Description FROM Device "
		"JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory " 
		"LEFT JOIN Device As P1 ON Device.FK_Device_ControlledVia=P1.PK_Device "
		"LEFT JOIN Device As P2 ON P1.FK_Device_ControlledVia=P2.PK_Device " 
		"LEFT JOIN Device As P3 ON P2.FK_Device_ControlledVia=P3.PK_Device "
		"WHERE "
		"(PK_DeviceCategory = " + StringUtils::itos(PK_DeviceCategory) + " OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) + ")";
	
	if( PK_Device_PC )
		sSQL += " AND "
		"(Device.FK_Device_ControlledVia = " + StringUtils::itos(PK_Device_PC) + " " 
		"OR P1.FK_Device_ControlledVia = " + StringUtils::itos(PK_Device_PC) + " " 
		"OR P2.FK_Device_ControlledVia = " + StringUtils::itos(PK_Device_PC) + " "
		"OR P3.FK_Device_ControlledVia = " + StringUtils::itos(PK_Device_PC) + ")";

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) && ((row = db_wrapper_fetch_row(result_set.r))) && row[0] )
	{
		if( sDescription )
			*sDescription = row[1];
		return atoi(row[0]);
	}
	return 0;
}

bool WizardLogic::HasRemoteControl(bool bPopulateListOfOptions)
{
	int PK_Device_Remote = 	FindFirstDeviceInCategory(DEVICECATEGORY_Remote_Controls_CONST,m_pOrbiter->m_dwPK_Device);
	if( !bPopulateListOfOptions || PK_Device_Remote )
		return PK_Device_Remote!=0;

	FindPnpDevices( TOSTRING(DEVICECATEGORY_Infrared_Receivers_CONST) "," TOSTRING(DEVICECATEGORY_Remote_Controls_CONST) );
	return false;
}

void WizardLogic::FindPnpDevices(string sPK_DeviceCategory)
{
	// We don't have a remote, and the user will want to see a list of possibilities
	string sSQL = "SELECT DeviceTemplate.Description,Manufacturer.Description FROM DeviceTemplate "
		"JOIN Manufacturer ON DeviceTemplate.FK_Manufacturer=PK_Manufacturer "
		"JOIN DHCPDevice ON FK_DeviceTemplate=PK_DeviceTemplate "
		"WHERE DeviceTemplate.FK_DeviceCategory IN (" + sPK_DeviceCategory + ")";

	DB_ROW row;
	string sPnpDevices;
	PlutoSqlResult result_set2;
    if( (result_set2.r=db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set2.r)))
			sPnpDevices += (sPnpDevices.size() ? ", " : "") + string(row[1]) + ":" + row[0];

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPnpDevices);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"WizardLogic::FindPnpDevices Category %s devices %s", sPK_DeviceCategory.c_str(), sPnpDevices.c_str());
}

/*
	SETUP USERS
*/

bool WizardLogic::AlreadyHasUsers()
{
	return DatabaseUtils::AlreadyHasUsers(this,m_pOrbiter->m_pData->m_dwPK_Installation);
}

int WizardLogic::AddUser(string sUsername)
{
	return m_pUserUtils->AddUser(sUsername,m_pOrbiter);
}

void WizardLogic::RemoveUser(int PK_User)
{
	m_pUserUtils->RemoveUser(PK_User);
}

bool WizardLogic::AlreadyHasRooms()
{
	return DatabaseUtils::AlreadyHasRooms(this,m_pOrbiter->m_pData->m_dwPK_Installation);
}

// Get a map of RoomTypes to number of Rooms of that Type, and the total number of rooms
int WizardLogic::PreSeedRoomInfo( map<int, int > &mapRooms )
{
	int iNumRooms=0;

  //string sSQL = "SELECT FK_RoomType,count(PK_Room) FROM Room "
	//	"JOIN RoomType ON FK_RoomType=PK_RoomType "
	//	"GROUP BY FK_RoomType";

  // activate all room types (sql from Chris)
  string sSQL =
    "SELECT PK_RoomType, Sum(IF(NOT IFNULL(FK_Installation, 0), 0, 1)) As Count "
    "FROM RoomType LEFT JOIN Room ON FK_RoomType = PK_RoomType "
    "WHERE FK_Installation = " + Installation_get() + " OR FK_Installation IS NULL "
    "GROUP BY PK_RoomType"
    ;

	PlutoSqlResult result_set_room;
	DB_ROW row;
	if( (result_set_room.r=db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set_room.r)))
		{
            LoggerWrapper::GetInstance()->Write(LV_STATUS,"Room type %d : %d", atoi(row[0]), atoi(row[1]));
            
			mapRooms[ atoi(row[0]) ] = atoi(row[1]);
			iNumRooms += atoi(row[1]);
		}

	return iNumRooms;
}

string WizardLogic::GetRoomTypeName(int PK_RoomType)
{
    string sSQL = "SELECT Description FROM RoomType WHERE PK_RoomType=" + StringUtils::ltos(PK_RoomType);

    string sResult;
    PlutoSqlResult result_set_room;
    DB_ROW row;
    if( (result_set_room.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set_room.r)) )
        sResult = row[0];

    return sResult;
}

void WizardLogic::ProcessUpdatedRoomInfo( map<int, int > &mapRooms )
{
	for( map<int, int >::iterator it=mapRooms.begin();it!=mapRooms.end();++it)
	{
		int iNumRooms = 0;  // How many rooms do we already have of this type in the database
		string sSQL = "SELECT count(PK_Room) FROM Room WHERE FK_RoomType=" + StringUtils::itos(it->first);

		PlutoSqlResult result_set_room;
		DB_ROW row;
		if( (result_set_room.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set_room.r)) )
			iNumRooms = atoi(row[0]);

		if( iNumRooms == it->second )
			continue;  // We already have the correct number
		else if( iNumRooms > it->second )
			RemoveRoomsOfType( it->first, iNumRooms, it->second );
		else
			AddRoomsOfType( it->first, iNumRooms, it->second );
	}
}

void WizardLogic::RemoveRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired )
{
	for(int iRoom = NumRoomsCurrent; iRoom > NumRoomsDesired; --iRoom)
	{
		// Find the room of this type that has the least number of devices/command groups
		string sSQL = "SELECT PK_Room,count(PK_Device) as NumDevice,count(FK_CommandGroup) as NumCommandGroup FROM Room "
			"LEFT JOIN Device ON Device.FK_Room=PK_Room "
			"LEFT JOIN CommandGroup_Room ON CommandGroup_Room.FK_Room=PK_Room "
			"WHERE FK_RoomType=" + StringUtils::itos(PK_RoomType) +
			" GROUP BY PK_Room "
			"ORDER BY NumDevice,NumCommandGroup "
			"LIMIT 1;";

		int PK_Room=0;
		PlutoSqlResult result_set_room;
		DB_ROW row;
		if( (result_set_room.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set_room.r)) )
			PK_Room = atoi(row[0]);

		if( !PK_Room )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WizardLogic::RemoveRoomsOfType cannot find a room to delete: type %d current %d desired %d",
                            PK_RoomType, NumRoomsCurrent, NumRoomsDesired);
			continue;
		}

		sSQL = "DELETE FROM Room WHERE PK_Room=" + StringUtils::itos(PK_Room);
		threaded_db_wrapper_query(sSQL);

		sSQL = "UPDATE Device SET FK_Room=NULL WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_db_wrapper_query(sSQL);

		sSQL = "DELETE FROM CommandGroup_Room WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_db_wrapper_query(sSQL);

		sSQL = "DELETE FROM EntertainArea WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_db_wrapper_query(sSQL);

		sSQL = "DELETE FROM Room_Users WHERE FK_Room=" + StringUtils::itos(PK_Room);
		threaded_db_wrapper_query(sSQL);
	}
}

void WizardLogic::AddRoomOfType(int PK_RoomType)
{
	string sSQL = "SELECT Count(*) FROM Room "
		"WHERE FK_RoomType=" + StringUtils::itos(PK_RoomType) +
		" AND FK_Installation = " + Installation_get();

	PlutoSqlResult result_get_rooms;
	DB_ROW row;
	long nNumberOfRooms = 0;
	if((result_get_rooms.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_get_rooms.r)))
		nNumberOfRooms = atoi(row[0]);

	AddRoomsOfType(PK_RoomType, nNumberOfRooms, nNumberOfRooms + 1); //just add a room
}

void WizardLogic::RemoveRoomOfType(int PK_RoomType)
{
	string sSQL = "SELECT Count(*) FROM Room "
		"WHERE FK_RoomType=" + StringUtils::itos(PK_RoomType) +
		" AND FK_Installation = " + Installation_get();

	PlutoSqlResult result_get_rooms;
	DB_ROW row;
	long nNumberOfRooms = 0;
	if((result_get_rooms.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_get_rooms.r)))
		nNumberOfRooms = atoi(row[0]);

	if(nNumberOfRooms)
		RemoveRoomsOfType(PK_RoomType, nNumberOfRooms, nNumberOfRooms - 1);//just remove one room

}

void WizardLogic::AddRoomsOfType( int PK_RoomType, int NumRoomsCurrent, int NumRoomsDesired )
{
	// Simple, just add a room to the database
	for(int iRoom = NumRoomsCurrent; iRoom < NumRoomsDesired; ++iRoom)
	{
		string sDescription = m_mapRoomTypes[PK_RoomType];
		if( NumRoomsDesired > 1 )
			sDescription += " #" + StringUtils::itos(iRoom);

		string sSQL = "INSERT INTO Room(FK_Installation,FK_RoomType,Description) "
			"VALUES(" + Installation_get() + "," +
			StringUtils::itos(PK_RoomType) + ",'" + StringUtils::SQLEscape(sDescription) + "');";
		threaded_db_wrapper_query(sSQL);
	}
}

void WizardLogic::ChangeRoomName(int PK_Room, string sName)
{
	if(PK_Room)
	{
		string sSQL = "UPDATE Room SET Description = '" + sName + "' WHERE PK_Room = " + StringUtils::ltos(PK_Room);
		threaded_db_wrapper_query(sSQL);
	}
}

void WizardLogic::SetExternalDeviceInRoom(int PK_Device, string sPK_Room)
{
	if( sPK_Room=="*" )
	{
		string sSQL = "UPDATE Device SET ManuallyConfigureEA=-1 WHERE PK_Device=" + StringUtils::itos(PK_Device);
		threaded_db_wrapper_query(sSQL);
		return;
	}
	else if( sPK_Room=="0" )
	{
		string sSQL = "UPDATE Device SET ManuallyConfigureEA=0 WHERE PK_Device=" + StringUtils::itos(PK_Device);
		threaded_db_wrapper_query(sSQL);
		return;
	}

	string sSQL = "UPDATE Device SET ManuallyConfigureEA=1 WHERE PK_Device=" + StringUtils::itos(PK_Device);
	threaded_db_wrapper_query(sSQL);

	sSQL = "DELETE FROM Device_EntertainArea WHERE FK_Device=" + StringUtils::itos(PK_Device);
	threaded_db_wrapper_query(sSQL);

	string::size_type pos=0;
	while( true )
	{
		string sOneRoom = StringUtils::Tokenize(sPK_Room,"|",pos);
		if( sOneRoom.empty() )
			break;

		DB_ROW row;

		sSQL = "SELECT PK_EntertainArea FROM EntertainArea WHERE FK_Room=" + sOneRoom;
		PlutoSqlResult result_set_EA;
		if( (result_set_EA.r=db_wrapper_query_result(sSQL))==NULL || result_set_EA.r->row_count==0 )
		{
			sSQL = "INSERT INTO EntertainArea(FK_Room,Description) SELECT PK_Room,Description FROM Room WHERE PK_Room=" + sOneRoom;
			int PK_EntertainArea = threaded_db_wrapper_query_withID(sSQL);
			if( PK_EntertainArea )
				DatabaseUtils::AddDeviceToEntertainArea(this,PK_Device,PK_EntertainArea);
		}
		else
		{
			while ((row = db_wrapper_fetch_row(result_set_EA.r)))
				DatabaseUtils::AddDeviceToEntertainArea(this,PK_Device,atoi(row[0]));
		}
	}
}

int WizardLogic::GetCountry()
{
	string sSQL = "SELECT FK_Country FROM Installation WHERE PK_Installation=" +
		Installation_get();


	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) &&
      (row = db_wrapper_fetch_row(result_set.r)) && row[0] && atoi(row[0]) )
    return atoi(row[0]);

	return COUNTRY_UNITED_STATES_CONST;
}

void WizardLogic::SetCountry(int PK_Country)
{
	string sSQL = "UPDATE Installation SET City=NULL,State=NULL,Zip=NULL,FK_City=NULL,FK_PostalCode=NULL WHERE PK_Installation=" +
		Installation_get() +
		" AND FK_Country<>" + StringUtils::itos(PK_Country);

	threaded_db_wrapper_query(sSQL);

	sSQL = "UPDATE Installation SET FK_Country=" + StringUtils::itos(PK_Country) +
		" WHERE PK_Installation=" +
		Installation_get();

	threaded_db_wrapper_query(sSQL);
}

string WizardLogic::GetCityRegion()
{
	string sSQL = "SELECT City,State FROM Installation WHERE PK_Installation=" +
		Installation_get();

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) &&
      (row = db_wrapper_fetch_row(result_set.r)) && row[0] )
		return string(row[0]) + (row[1] ? ", " + string(row[1]) : string(""));

	return "";
}

int WizardLogic::GetPostalCode()
{
	string sSQL = "SELECT FK_PostalCode FROM Installation";

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0] )
		return atoi(row[0]);
	else
		return 0;
}

bool WizardLogic::GetLocation()
{
	string sSQL = "SELECT FK_City,FK_PostalCode,City,State,Zip,Country.Description FROM Installation JOIN Country ON FK_Country=PK_Country "
		" WHERE PK_Installation=" + StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Installation);

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) && ((row = db_wrapper_fetch_row(result_set.r))) )
	{
		//0=PK_City,1=PK_PostalCode,2=City,3=Region,4=PostalCode,5=country
		if( !row[0] || !row[2] || !row[5] || !row[5] )
			return false;
		
		string sLocation = string(row[2]) + ", " + row[3];
		if( row[4] )
			sLocation += " " + string(row[4]);
		sLocation += string("\n") + row[5];
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,sLocation);
		return true;
	}

	return false;
}

bool WizardLogic::SetLocation(string sLocation)
{
	/*int PK_Country =*/ GetCountry();
	//0=PK_City,1=PK_PostalCode,2=City,3=Region,4=PostalCode,5=Lat,6=Long,7=time zone
	string::size_type pos=0;
	int PK_City = atoi( StringUtils::Tokenize(sLocation,"\t",pos).c_str() );
	int PK_PostalCode = atoi( StringUtils::Tokenize(sLocation,"\t",pos).c_str() );
	string City = StringUtils::Tokenize(sLocation,"\t",pos);
	string Region = StringUtils::Tokenize(sLocation,"\t",pos);
	string PostalCode = StringUtils::Tokenize(sLocation,"\t",pos);
	string Latitude = StringUtils::Tokenize(sLocation,"\t",pos);
	string Longitude = StringUtils::Tokenize(sLocation,"\t",pos);
	string TimeZone = StringUtils::Tokenize(sLocation,"\t",pos);

	if( !PK_City || City.size()==0 || Latitude.size()==0 || Longitude.size()==0 || TimeZone.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WizardLogic::SetLocation City %d %s lat %s long %s timezone %s",PK_City,City.c_str(),Latitude.c_str(),Longitude.c_str(), TimeZone.c_str());
		return false;
	}

	string sSQL = "UPDATE Installation SET City='" + StringUtils::SQLEscape(City) +
		"',State='" + StringUtils::SQLEscape(Region) + "',FK_City=" + StringUtils::itos(PK_City) +
		",FK_PostalCode=" + StringUtils::itos(PK_PostalCode) + ",Zip='" + StringUtils::SQLEscape(PostalCode) + "'"
		" WHERE PK_Installation=" + StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Installation);
	threaded_db_wrapper_query(sSQL);

	SetLongLat(Latitude,Longitude);

	DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
	if( pDevice_Core )
	{
		DeviceData_Base *pDevice_AppServer = 
			pDevice_Core->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
		if( pDevice_AppServer )
		{
			DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
				"/usr/pluto/bin/SetTimeZone.sh","set time zone",TimeZone,"","",false,false,false,true);
			CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Persist;
			m_pOrbiter->SendCommand(CMD_Spawn_Application);
		}
	}

	return true;
}

bool WizardLogic::SetPostalCode(string PostalCode)
{
	int PK_Country = GetCountry();
	string sSQL = "SELECT City,State,FK_City,`Long`,Lat,PK_PostalCode FROM PostalCode WHERE PostalCode='"
		+ StringUtils::SQLEscape(PostalCode) + "' AND FK_Country=" + StringUtils::itos(PK_Country);

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) &&
      (row = db_wrapper_fetch_row(result_set.r)) && row[0] )
	{
		sSQL = "UPDATE Installation SET City='" + StringUtils::SQLEscape(row[0]) +
			"'" +
			(row[1] ? string(",State='") + StringUtils::SQLEscape(row[1]) + "'": string("")) +
			(row[2] ? string(",FK_City='") + StringUtils::SQLEscape(row[2]) + "'" : string("")) +
			",FK_PostalCode=" + row[5];
		threaded_db_wrapper_query(sSQL);

		if( row[3] && row[4] )
			SetLongLat(row[4],row[3]);

		return true;
	}
	return false;
}

void WizardLogic::SetLongLat(string Latitude,string Longitude)
{
	DeviceData_Base *pDevice_Event_Plugin =
		m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Event_Plugins_CONST);
	if( pDevice_Event_Plugin )
	{
		string sSQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData) VALUES(" +
			StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) + "," +
			StringUtils::itos(DEVICEDATA_Longitude_CONST) + ")";
		threaded_db_wrapper_query(sSQL,true);  // Ignore errors, this may already be there

		sSQL = "INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData) VALUES(" +
			StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) + "," +
			StringUtils::itos(DEVICEDATA_Latitude_CONST) + ")";
		threaded_db_wrapper_query(sSQL,true);  // Ignore errors, this may already be there

		sSQL = string("UPDATE Device_DeviceData SET IK_DeviceData='") + Longitude + "'" +
			" WHERE FK_Device=" + StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) +
			" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Longitude_CONST);
		threaded_db_wrapper_query(sSQL);

		sSQL = string("UPDATE Device_DeviceData SET IK_DeviceData='") + Latitude + "'" +
			" WHERE FK_Device=" + StringUtils::itos(pDevice_Event_Plugin->m_dwPK_Device) +
			" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_Latitude_CONST);
		threaded_db_wrapper_query(sSQL);
	}
}
/*	//check if the manufacturerer is already in database

int WizardLogic::AddAVDeviceTemplate()
{	
	string sSQL;

	if( !ExistManufacturer(m_ManufacturerName) )
	{
		sSQL = "INSERT INTO Manufacturer (Description) VALUES('" + m_ManufacturerName + "')";
		m_nPKManufacuter = threaded_db_wrapper_query_withID(sSQL);
	}
	
	sSQL = string("INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer,IRFrequency ,FK_CommMethod) VALUES('") +\
		 m_AVTemplateName + "'," + "77" + "," + StringUtils::itos(m_nPKManufacuter) + "," + "NULL" + "," + "1" + ")"; 
	m_nPKAVTemplate = threaded_db_wrapper_query_withID(sSQL);

	sSQL = string("INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate,IR_PowerDelay,IR_ModeDelay,DigitDelay) VALUES(") + \
		StringUtils::itos(m_nPKAVTemplate) + "," + "2" + "," + "7" + "," + "250" + ")";
	threaded_db_wrapper_query(sSQL);
	return m_nPKAVTemplate;
	return 0;
}

void WizardLogic::UpdateAVTemplateDelays(string IR_PowerDelay,string IR_ModeDelay,string DigitDelay)
{
	string sSQL = "UPDATE DeviceTemplate_AV SET IR_PowerDelay=" + IR_PowerDelay + ",";
	sSQL += "IR_ModeDelay=" + IR_ModeDelay + "," + "DigitDelay=" + DigitDelay + " ";
	sSQL += string("WHERE FK_DeviceTemplate=") + StringUtils::itos(m_nPKAVTemplate);
	threaded_db_wrapper_query(sSQL);
}

void WizardLogic::UpdateAVTemplateSettings()
{
	string sSQL = string("UPDATE DeviceTemplate_AV SET TogglePower=") + 
		(m_bAVTogglePower ? "1" : "0") + "," += "NumericEntry=" + m_AVTemplateNumericEntry + " ";
	sSQL += string("WHERE FK_DeviceTemplate=") + StringUtils::itos(m_nPKAVTemplate);
	threaded_db_wrapper_query(sSQL);
}

//AV Wizard - single
void WizardLogic::AddAVDeviceInput()
{
	string sSQL;
	string sInputs,sMediaType,sConnectorType;
	string embeddedId,insertId;
	string name,aux;
	string::size_type pos=0;
	int nPos = 0;
	
	//temporary
	m_nPKDeviceCategory = 77;

	map<int,string>::iterator it;
	for(it=m_mapAVInputs.begin();it!=m_mapAVInputs.end();it++)
	{
		sInputs = StringUtils::ltos(it->first);
		aux = it->second;
		pos = 0;
		sMediaType = StringUtils::Tokenize( aux, ",", pos );
		sConnectorType = StringUtils::Tokenize( aux, ",", pos );
		name = sInputs + "-" + sMediaType;
		if( sConnectorType == "0" )
			sConnectorType = "NULL";

		aux = "Inputs ,MediaType+ConectorType,name:  ";
		aux += sInputs + "," + sMediaType + "," + sConnectorType + "," + name;
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
		LoggerWrapper::GetInstance()->Write( LV_WARNING, aux.c_str() );

		sSQL =string("INSERT IGNORE INTO DeviceTemplate_Input\
		(FK_DeviceTemplate,FK_Command,FK_ConnectorType) VALUES (") + 
		StringUtils::ltos(m_nPKAVTemplate) + "," + sInputs + "," + StringUtils::ltos(m_nPKConnectorType) + ")";
		threaded_db_wrapper_query(sSQL);
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
		LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

		sSQL = "INSERT IGNORE INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command,FK_ConnectorType,OrderNo)";
		sSQL += " VALUES (";
		sSQL += StringUtils::ltos(m_nPKAVTemplate) + "," + sInputs + "," + sConnectorType + 
			"," + StringUtils::ltos(nPos) + ")"; 
		threaded_db_wrapper_query(sSQL);

		sSQL = "UPDATE DeviceTemplate_Input Set OrderNo=" + StringUtils::ltos(nPos++) + " ";
		sSQL += "WHERE FK_DeviceTemplate='" + StringUtils::ltos(m_nPKAVTemplate) + "' ";
		sSQL += "AND FK_Command='" + sInputs + "'";
		threaded_db_wrapper_query(sSQL);

		LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
		LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

		if( sMediaType != "0" )
		{
			//parentID este device template-ul tau, ala creat initial mediaType este ala selectat pt

			// create embedded device template
			sSQL = "INSERT INTO DeviceTemplate (Description,FK_Manufacturer,FK_DeviceCategory) VALUES(" ;
			sSQL += name + "," + StringUtils::ltos(m_nPKManufacuter) + "," + StringUtils::ltos(m_nPKDeviceCategory) + ")";
			embeddedId = threaded_db_wrapper_query_withID(sSQL);
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

			// link the embedded device with parent device
			sSQL =	"INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia\
				(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)\
				VALUES(";
			sSQL += embeddedId + "," + StringUtils::ltos(m_nPKAVTemplate) + ",1,1)";
			insertId = threaded_db_wrapper_query_withID(sSQL);
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

			//create audio video pipes 2 insert 1&2 pipes
			sSQL + "INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe\
			 (FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)\
			VALUES (";
			sSQL += insertId + ",1," + sInputs + ")";
			threaded_db_wrapper_query(sSQL);	
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

			sSQL + "INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe\
			 (FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)\
			VALUES (";
			sSQL += insertId + ",2," + sInputs + ")";
			threaded_db_wrapper_query(sSQL);	
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

			sSQL = "INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES(" ;
			sSQL += embeddedId + "," + sInputs + ")";
			threaded_db_wrapper_query(sSQL);	
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );

			sSQL = "INSERT INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES(";
			sSQL += embeddedId + "," + sMediaType + ")";
			threaded_db_wrapper_query(sSQL);	
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "AddAVDeviceInput" );
			LoggerWrapper::GetInstance()->Write( LV_WARNING, sSQL.c_str() );
		}
	}
}

//AV Wizard - single
void WizardLogic::AddAVMediaType()
{
	string sSQL;
	sSQL =string("INSERT IGNORE INTO DeviceTemplate_MediaType\
		(FK_DeviceTemplate,PK_DeviceTemplate_MediaType) VALUES (") + 
		StringUtils::ltos(m_nPKAVTemplate) + "," + StringUtils::ltos(m_nPKMediaType) + ")";
	threaded_db_wrapper_query(sSQL);
}

int WizardLogic::GetAVIRCodesType()
{
	string sSQL = "SELECT Description,FK_Command FROM InfraredGroup_Command,InfraredGroup WHERE\
		PK_InfraredGroup=FK_InfraredGroup AND FK_Manufacturer='" + StringUtils::ltos(m_nPKManufacuter) + "'";
	int nCommandId , nReadRow = 0;
	bool bToggle = true;

	PlutoSqlResult result;
	DB_ROW row;

	if( (result.r = db_wrapper_query_result(sSQL))  )
	{
		while( (row = db_wrapper_fetch_row( result.r )) )
		{
			nCommandId = atoi( row[1] );
			if( (nCommandId == 192) || (nCommandId == 193) )
				bToggle = false;
			nReadRow++;
		}
	}

	if( !nReadRow )
		return 0;
	else
	{
		if( bToggle )
			return 1;
		else return 2;
	}
}

bool WizardLogic::IsIRCodes(string commands)
{
	PlutoSqlResult result;
	DB_ROW row;
	string sql = "SELECT Description,OriginalKey FROM InfraredGroup,InfraredGroup_Command WHERE\
		PK_InfraredGroup=FK_InfraredGroup AND FK_DeviceCategory='";
	sql += StringUtils::ltos(m_nPKDeviceCategory) + "'" + "AND FK_Manufacturer='" + 
		StringUtils::ltos(m_nPKAVTemplate) + "'";
	if( !commands.empty() )
		sql += string(" ") + "AND FK_Command IN(" + commands + ")";

	if( (result.r = db_wrapper_query_result(sql))  )
	{
		if( (row = db_wrapper_fetch_row( result.r )) )
			return true;
		else 
			return false;
	}
	return false;
}

// return value 0 unknown,1 toggle,2 discret
int WizardLogic::GetAVInputType()
{
	return 1;
}


int WizardLogic::GetParentDevice()
{
	PlutoSqlResult result_set;
	DB_ROW row;
	m_nPKParentDevice = 0;

	string sql = "SELECT PK_Device,FK_Device_ControlledVia FROM Device WHERE PK_Device='";
	sql += StringUtils::ltos(m_nPKDevice) + "'";

	if( (result_set.r=db_wrapper_query_result(sql)) )
	{
		if( (row = db_wrapper_fetch_row(result_set.r)))
		{
			m_nPKParentDevice = atoi( row[1] );
			return m_nPKParentDevice;
		}
	}	

	return m_nPKParentDevice;
}

void WizardLogic::UpdateAVTemplateToggle()
{
	string sSQL = "UPDATE DeviceTemplate_AV SET ";
	sSQL += string("TogglePower=") + ( m_bAVTogglePower ? "1" : "0" ) + ",";
	sSQL += string("ToggleDSP=") + ( m_bAVToggleDSP ? "1" : "0" ) + ",";
	sSQL += string("ToggleInput=") + ( m_bAVToggleInput ? "1" : "0" ) + " ";
	sSQL += "WHERE FK_DeviceTemplate=" + StringUtils::ltos(m_nPKAVTemplate);
	threaded_db_wrapper_query(sSQL);
}
	*/

void WizardLogic::SetAvPath(int PK_Device_From,int PK_Device_To,int PK_Pipe,int PK_Command_Input)
{
	string sSQL = "DELETE FROM Device_Device_Pipe WHERE FK_Device_From=" + StringUtils::itos(PK_Device_From) +
		" AND FK_Pipe=" + StringUtils::itos(PK_Pipe);
	threaded_db_wrapper_query(sSQL);

	string sCommandInput = PK_Command_Input != 0 ? StringUtils::itos(PK_Command_Input) : "NULL";

	sSQL = "INSERT INTO Device_Device_Pipe(FK_Device_From,FK_Device_To,FK_Pipe,FK_Command_Input)"
		" VALUES(" + StringUtils::itos(PK_Device_From) + "," + StringUtils::itos(PK_Device_To) + ","
		+ StringUtils::itos(PK_Pipe) + "," + sCommandInput + ")";

	threaded_db_wrapper_query(sSQL);
}

bool WizardLogic::GetAvPath(int PK_Device_From,long &PK_Device_To,int PK_Pipe,long &PK_Command_Input,string &sDescription,string &sCommandDescription)
{
	string sSQL = "SELECT PK_Device,Device.Description,FK_Command_Input,Command.Description FROM Device_Device_Pipe JOIN Device ON FK_Device_To=PK_Device "
		" LEFT JOIN Command ON FK_Command_Input=PK_Command WHERE FK_Device_From=" + StringUtils::itos(PK_Device_From) +
		" AND FK_Pipe=" + StringUtils::itos(PK_Pipe);
	DB_ROW row;
	PlutoSqlResult result_set;
	if( (result_set.r=db_wrapper_query_result(sSQL)) && ((row = db_wrapper_fetch_row(result_set.r))) && row[0] )
	{
		PK_Device_To = atoi( row[0] );
		sDescription = row[1];
		PK_Command_Input = row[2] ? atoi(row[2]) : 0;
		sCommandDescription = row[3] ? row[3] : "";
		return true;
	}
	return false;
}

/*
//Insert all dsp modes in database
void WizardLogic::InsertDSPModes()
{
	string sql,sqlFinal;
	sql = string( "INSERT IGNORE INTO DeviceTemplate_DSPMode (FK_DeviceTemplate,FK_Command,OrderNo) VALUES (" );
	list<string>::iterator it;
	int nPos = 0;
	
	for(it=m_listDSPModes.begin();it!=m_listDSPModes.end();it++)
	{
		sqlFinal = sql + StringUtils::ltos(m_nPKAVTemplate) + "," + *it + "," + StringUtils::ltos(nPos) + ")";
		if( m_bAVToggleDSP )
			nPos++;
		threaded_db_wrapper_query(sqlFinal);
	}
}

//change the order for m_firstId m_secondId
void WizardLogic::ChangeDSPOrder()
{
	string sql;
	
	if( m_firstPos.empty() || m_secondPos.empty() )
		return;

	sql = string("UPDATE DeviceTemplate_DSPMode SET OrderNo=") +  m_secondPos + " ";
	sql += string("WHERE FK_DeviceTemplate=") + StringUtils::ltos(m_nPKAVTemplate) + " " + "AND FK_Command=" + 
		m_firstId;
	threaded_db_wrapper_query(sql);

	sql = string("UPDATE DeviceTemplate_DSPMode SET OrderNo=") +  m_firstPos + " ";
	sql += string("WHERE FK_DeviceTemplate=") + StringUtils::ltos(m_nPKAVTemplate) + " " + "AND FK_Command=" + 
		m_secondId;
	threaded_db_wrapper_query(sql);
}

void WizardLogic::ChangeInputOrder()
{
	string sql;
	
	if( m_firstPos.empty() || m_secondPos.empty() )
		return;

	sql = string("UPDATE DeviceTemplate_Input SET OrderNo=") +  m_secondPos + " ";
	sql += string("WHERE FK_DeviceTemplate=") + StringUtils::ltos(m_nPKAVTemplate) + " " + "AND FK_Command=" + 
		m_firstId;
	threaded_db_wrapper_query(sql);

	sql = string("UPDATE DeviceTemplate_Input SET OrderNo=") +  m_firstPos + " ";
	sql += string("WHERE FK_DeviceTemplate=") + StringUtils::ltos(m_nPKAVTemplate) + " " + "AND FK_Command=" + 
		m_secondId;
	threaded_db_wrapper_query(sql);
}

void WizardLogic::CreateIRGroup()
{
	string sql;
	map<int,string>::iterator it;
	sql = "INSERT INTO InfraredGroup (FK_DeviceCategory,FK_Manufacturer,Description,FK_CommMethod) VALUES (";
	sql += StringUtils::ltos(m_nPKDeviceCategory) + "," + StringUtils::ltos(m_nPKManufacuter) + ",";
	sql += StringUtils::ltos(m_nPKAVTemplate) + "," + "1" + ")";
	m_nPKIRGroup = threaded_db_wrapper_query_withID(sql);
  
	for(it=m_mapIRCommands.begin();it!=m_mapIRCommands.end();it++)
	{
		sql = "INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command ,FK_DeviceTemplate,IRData) VALUES("; 
		sql += StringUtils::ltos(m_nPKIRGroup) + "," + StringUtils::ltos(it->first) + ",";
		sql += StringUtils::ltos(m_nPKAVTemplate) + "," + it->second + ")";
		threaded_db_wrapper_query(sql);
	}
}
*/
int WizardLogic::AddDevice(int PK_DeviceTemplate, string sDeviceDataList /* = "" */, long PK_Device_ControlledVia/* = 0*/)
{
	int iPK_Device=0;
	DCE::CMD_Create_Device CMD_Create_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
                                           PK_DeviceTemplate,"",m_pOrbiter->m_pData->m_dwPK_Room,"",sDeviceDataList,0,PK_Device_ControlledVia,
										   "",m_pOrbiter->m_pData->m_dwPK_Device,m_pOrbiter->m_pData->m_dwPK_Device,
                                           &iPK_Device);
	m_pOrbiter->SendCommand(CMD_Create_Device);
	return iPK_Device;
}

void WizardLogic::AddExternalTuner(int PK_Device_Tuner)
{
	DeviceData_Base *pDevice_PVR = m_pOrbiter->m_pData->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_Capture_Cards_CONST);
	if( pDevice_PVR )
	{
		SetAvPath(PK_Device_Tuner,pDevice_PVR->m_dwPK_Device,1,COMMAND_AV_1_CONST);
		SetAvPath(PK_Device_Tuner,pDevice_PVR->m_dwPK_Device,2,COMMAND_AV_1_CONST);
	}
}

string WizardLogic::Installation_get()
{
  return StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Installation);
}

void WizardLogic::DeleteDevicesInThisRoomOfType(int PK_DeviceCategory)
{
	string sSQL = "SELECT PK_Device FROM Device "
		"join DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"join DeviceCategory ON FK_DeviceCategory = PK_DeviceCategory "
		"where (PK_DeviceCategory=" + StringUtils::itos(PK_DeviceCategory) +
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(PK_DeviceCategory) +
		") AND FK_Room=" + StringUtils::itos(m_pOrbiter->m_pData->m_dwPK_Room);

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			string sResponse;  // Make sure the device is deleted before we continue
			DCE::CMD_Delete_Device CMD_Delete_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,atoi(row[0]));
			m_pOrbiter->SendCommand(CMD_Delete_Device,&sResponse);
		}
}

int WizardLogic::FindManufacturer(string sName)
{
	sName = StringUtils::SQLEscape(sName);
	string sSQL = "SELECT PK_Manufacturer FROM Manufacturer WHERE Description='" + sName + "'";
	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0] )
		return atoi(row[0]);

	sSQL = "INSERT INTO Manufacturer(Description) VALUES('" + sName + "')";
	return threaded_db_wrapper_query_withID(sSQL);
}

int WizardLogic::FindModel(int PK_DeviceCategory,string sModel)
{
	if( !m_dwPK_Manufacturer )
		return 0; // Shouldn't happen

	sModel = StringUtils::SQLEscape(sModel);
	string sSQL = "SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE FK_Manufacturer = " + StringUtils::itos(m_dwPK_Manufacturer) + " AND Description='" + sModel + "'";
	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0] )
		return atoi(row[0]);

	sSQL = "INSERT INTO DeviceTemplate(Description,FK_DeviceCategory,FK_Manufacturer,FK_CommMethod) VALUES('" + sModel + "'," + StringUtils::itos(PK_DeviceCategory) + 
		"," + StringUtils::itos(m_dwPK_Manufacturer) + "," TOSTRING(COMMMETHOD_Infrared_CONST) ")";
	return threaded_db_wrapper_query_withID(sSQL);
}

string WizardLogic::GetDeviceStatus(long nPK_Device)
{
	string sSQL = "SELECT Status FROM Device WHERE PK_Device = " + StringUtils::ltos(nPK_Device);

	PlutoSqlResult result_set;
	DB_ROW row;
	if((result_set.r = db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0])
		return row[0];

	return "";
}

int WizardLogic::GetNumLights()
{
	m_dequeNumLights.clear();
	string sSQL = "SELECT PK_Device,FK_Room,IK_DeviceData FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory = PK_DeviceCategory "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) +
		" WHERE PK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST) +
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST);

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			if( !row[1] || !atoi(row[1]) )
				m_dequeNumLights.push_back(make_pair<int,string> (atoi(row[0]),row[2] ? row[2] : ""));
		}

	return (int) result_set.r->row_count;
}

int WizardLogic::GetNumSensors()
{
	m_dequeNumSensors.clear();
	string sSQL = "SELECT PK_Device,FK_Room,IK_DeviceData,FK_DeviceTemplate FROM Device "
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory = PK_DeviceCategory "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) +
		" WHERE PK_DeviceCategory=" + StringUtils::itos(DEVICECATEGORY_Security_Device_CONST) +
		" OR FK_DeviceCategory_Parent=" + StringUtils::itos(DEVICECATEGORY_Security_Device_CONST);

	PlutoSqlResult result_set;
	DB_ROW row;
	if( (result_set.r=db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			if( !row[1] || !atoi(row[1]) || atoi(row[3])==DEVICETEMPLATE_Generic_Sensor_CONST )
				m_dequeNumSensors.push_back(make_pair<int,string> (atoi(row[0]),row[2] ? row[2] : ""));
		}

	return (int) result_set.r->row_count;
}

string WizardLogic::GetDeviceName(string sPK_Device)
{
	string sSQL = "SELECT Description FROM Device WHERE PK_Device = " + sPK_Device;

	PlutoSqlResult result_set;
	DB_ROW row;
	if((result_set.r = db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0])
		return row[0];

	return "";
}

void WizardLogic::SetDeviceName(string sPK_Device, string sName)
{
	string sSQL = "UPDATE Device SET Description = '" + sName + "' WHERE PK_Device = " + sPK_Device;
	threaded_db_wrapper_query(sSQL);
}

long WizardLogic::GetDeviceTemplateForDevice(string sPK_Device)
{
	string sSQL = "SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = " + sPK_Device;

	PlutoSqlResult result_set;
	DB_ROW row;
	if((result_set.r = db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0])
		return atoi(row[0]);

	return 0;
}

void WizardLogic::ChangeDeviceTemplateForDevice(string sPK_Device, string sFK_DeviceTemplate)
{
	string sSQL = "UPDATE Device SET FK_DeviceTemplate = " + sFK_DeviceTemplate + " WHERE PK_Device = " + sPK_Device;
	threaded_db_wrapper_query(sSQL);
}

long WizardLogic::GetRoomForDevice(string sPK_Device)
{
	string sSQL = "SELECT PK_Room FROM Device LEFT JOIN Room ON FK_Room=PK_Room AND Device.FK_Installation=Room.FK_Installation WHERE PK_Device = " + sPK_Device;  // Do a join so we know the room is valid

	PlutoSqlResult result_set;
	DB_ROW row;
	if((result_set.r = db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)) && row[0])
		return row[0] != NULL ? atoi(row[0]) : 0;

	return 0;
}

void WizardLogic::SetRoomForDevice(string sPK_Device, string sFK_Room)
{
	DCE::CMD_Set_Room_For_Device CMD_Set_Room_For_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,atoi(sPK_Device.c_str()),"",atoi(sFK_Room.c_str()));
	string sResponse;
	if( !m_pOrbiter->SendCommand(CMD_Set_Room_For_Device,&sResponse) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WizardLogic::SetRoomForDevice failed");
		string sSQL = "UPDATE Device SET FK_Room = " + sFK_Room + " WHERE PK_Device = " + sPK_Device;
		threaded_db_wrapper_query(sSQL);
	}
}

int WizardLogic::GetTopMostDevice(int PK_Device)
{
	return DatabaseUtils::GetTopMostDevice(this, PK_Device);
}

int WizardLogic::WhatRoomIsThisDeviceIn(int PK_Device)
{
	return GetRoomForDevice(StringUtils::ltos(DatabaseUtils::GetTopMostDevice(this, PK_Device)));
}

void WizardLogic::SetPVRSoftware(char PVRSoftware)
{
	int PK_DeviceTemplate_RemovePlugin = PVRSoftware=='V' ? DEVICETEMPLATE_MythTV_PlugIn_CONST : DEVICETEMPLATE_VDRPlugin_CONST,
		PK_DeviceTemplate_RemovePlayer = PVRSoftware=='V' ? DEVICETEMPLATE_MythTV_Player_CONST : DEVICETEMPLATE_VDR_CONST,
		PK_DeviceTemplate_AddPlugin = PVRSoftware=='V' ? DEVICETEMPLATE_VDRPlugin_CONST : DEVICETEMPLATE_MythTV_PlugIn_CONST,
		PK_DeviceTemplate_AddPlayer = PVRSoftware=='V' ? DEVICETEMPLATE_VDR_CONST : DEVICETEMPLATE_MythTV_Player_CONST;

	char s[2]=" ";
	s[0]=PVRSoftware;
	DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
	if( pDevice_Core )
		DatabaseUtils::SetDeviceData(this,pDevice_Core->m_dwPK_Device,DEVICEDATA_TV_Standard_CONST,s);

	string sSQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN (" + StringUtils::itos(PK_DeviceTemplate_RemovePlugin) + "," + StringUtils::itos(PK_DeviceTemplate_RemovePlayer) + ")";

	PlutoSqlResult result_set;
	DB_ROW row;
	if((result_set.r = db_wrapper_query_result(sSQL)) )
	{
		while (NULL != (row = db_wrapper_fetch_row(result_set.r)))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Deleting the old pvr device %s", row[0]);

			DCE::CMD_Delete_Device CMD_Delete_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,atoi(row[0]));
			m_pOrbiter->SendCommand(CMD_Delete_Device);
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating the pvr plugin...");

	int PK_Device;
	DCE::CMD_Create_Device CMD_Create_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,PK_DeviceTemplate_AddPlugin,"",0,"","",0,0,"",0,0,&PK_Device);
	m_pOrbiter->SendCommand(CMD_Create_Device);

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating the pvr players...");
	sSQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=" TOSTRING(DEVICETEMPLATE_OnScreen_Orbiter_CONST);
	PlutoSqlResult result_set_osd;
	if((result_set_osd.r = db_wrapper_query_result(sSQL)) )
	{
		while (NULL != (row = db_wrapper_fetch_row(result_set_osd.r)))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating pvr players child of %d", m_pOrbiter->m_dwPK_Device);

			DCE::CMD_Create_Device CMD_Create_Device(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,PK_DeviceTemplate_AddPlayer,"",0,"","",0,atoi(row[0]),"",0,0,&PK_Device);
			m_pOrbiter->SendCommand(CMD_Create_Device);
		}
	}

	//make sure the packages to be remove can be reinstalled later; mark them as uninstalled
	sSQL = "DELETE Package_Device.* FROM Package_Device JOIN DeviceTemplate ON DeviceTemplate.FK_Package = Package_Device.FK_Package "
		"WHERE PK_DeviceTemplate IN (" + StringUtils::itos(PK_DeviceTemplate_RemovePlugin) + "," + StringUtils::itos(PK_DeviceTemplate_RemovePlayer) + ")";
	threaded_db_wrapper_query(sSQL);

	//what to remove ?
	string sParms = PVRSoftware!='V' ? "vdr" : "mythtv";

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Spawning remove pvr packages, parms %s", sParms.c_str());

	//remove undeeded packages from all machines
	DCE::CMD_Spawn_Application_DT cmd_Spawn_Application_DT(
		m_pOrbiter->m_dwPK_Device, DEVICETEMPLATE_App_Server_CONST, BL_SameHouse,
		"/usr/pluto/bin/remove_pvr_packages.sh","remove_pvr_packages",sParms,"","",false,false,false,true);
	cmd_Spawn_Application_DT.m_pMessage->m_eRetry = MR_Retry;
	m_pOrbiter->SendCommand(cmd_Spawn_Application_DT);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//TEMPORARY CODE - WARNING!!!
	//HACK TO USE VDR vs MYTHTV

	if(PVRSoftware == 'V')
	{
		//to associate LiveTV with VDR:
		sSQL = "UPDATE MediaType_DesignObj SET FK_Screen=117, FK_Screen_OSD=278, "
			"FK_DesignObj_Popup=5576 WHERE psc_id=3";
	}
	else
	{
		//to associate LiveTV with Myth:
		sSQL = "UPDATE MediaType_DesignObj SET FK_Screen=48, FK_Screen_OSD=128, "
			"FK_DesignObj_Popup=4898 WHERE psc_id=3 ";
	}
	
	threaded_db_wrapper_query(sSQL);

	// end of hack
	////////////////////////////////////////////////////////////////////////////////////////////////////
}

bool WizardLogic::AnyPVRSoftwareInstalled()
{
	string sSQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN (" + 
		StringUtils::itos(DEVICETEMPLATE_MythTV_PlugIn_CONST) + "," + StringUtils::itos(DEVICETEMPLATE_VDRPlugin_CONST) + ")";

	PlutoSqlResult result_set;
	if((result_set.r = db_wrapper_query_result(sSQL)) )
	{
		//got any pvr plugin ?
		return NULL != db_wrapper_fetch_row(result_set.r);
	}

	return true;
}
