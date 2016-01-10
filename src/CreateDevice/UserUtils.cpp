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

#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Command_Impl.h"
#include "PlutoUtils/DBHelper.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "UserUtils.h"
using namespace DCE;

UserUtils::UserUtils(DBHelper *pDBHelper, int PK_Installation)
{
	m_pDBHelper=pDBHelper;
	m_PK_Installation=PK_Installation;
}

bool UserUtils::AlreadyHasMasterUsers()
{
	string sSQL = "SELECT PK_Users FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE userCanModifyInstallation=1 and FK_Installation="
		+ StringUtils::itos(m_PK_Installation) + " LIMIT 1";

	PlutoSqlResult result_set_room;
	if( (result_set_room.r=m_pDBHelper->db_wrapper_query_result(sSQL)) && result_set_room.r->row_count )
		return true;
	return false;
}

string UserUtils::GetGoodExtension(map<int,bool> &mapUsedExtensions) 
{
	// Don't put a condition.  If we use up more than the allowed extensions (shouldn't happen)
	// we'll just keep going anyway
	for(int i=FIRST_USER_EXTENSION;;++i)
	{
		if( mapUsedExtensions[i]==false )
		{
			mapUsedExtensions[i]=true;
			return StringUtils::itos(i);
		}
	}
}

void UserUtils::CheckExtensions(  )
{
	// First mark off which valid extensions are already used
	map<int,bool> mapUsedExtensions;
	string sSQL = "select Extension FROM Users JOIN Installation_Users ON FK_Users=PK_Users "
		"WHERE Extension IS NOT NULL AND Extension>=" + StringUtils::itos(FIRST_USER_EXTENSION) +
		" AND Extension<=" + StringUtils::itos(LAST_USER_EXTENSION) + 
		" ORDER BY Extension";

	PlutoSqlResult result_set_valid_ext;
	DB_ROW row;
	if( (result_set_valid_ext.r=m_pDBHelper->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set_valid_ext.r)))
			mapUsedExtensions[ atoi(row[0]) ] = true;

	sSQL = "select PK_Users FROM Users JOIN Installation_Users ON FK_Users=PK_Users "
		"WHERE Extension IS NULL OR Extension<" + StringUtils::itos(FIRST_USER_EXTENSION) +
		" OR Extension>" + StringUtils::itos(LAST_USER_EXTENSION) + 
		" ORDER BY UserName";

	PlutoSqlResult result_set_bad_ext;
	if( (result_set_bad_ext.r=m_pDBHelper->db_wrapper_query_result(sSQL)) )
		while ((row = db_wrapper_fetch_row(result_set_bad_ext.r)))
		{
			sSQL = "UPDATE Users SET Extension=" + GetGoodExtension(mapUsedExtensions) 
				+ " WHERE PK_Users=" + row[0];
			m_pDBHelper->threaded_db_wrapper_query(sSQL);

		}
}

int UserUtils::AddUser(string sUsername,Command_Impl *pCommand_Impl)
{
	StringUtils::TrimSpaces(sUsername);
	bool bExistingUsers=AlreadyHasMasterUsers();
	string sSQL;
	
	DB_ROW row;
	string sOriginalUsername=sUsername;
	// Be sure the username is unique, up to 99 users of the same name should be more than enough
	for(int i=1;i<99;++i)
	{
		if( i>1 )
			sUsername = sOriginalUsername + "_" + StringUtils::itos(i);
			
		sSQL = "SELECT count(PK_Users) from Users WHERE UserName='" + StringUtils::SQLEscape(sUsername) + "';";
		PlutoSqlResult result_set;
		if( (result_set.r=m_pDBHelper->db_wrapper_query_result(sSQL))==NULL || 
			(row = db_wrapper_fetch_row(result_set.r))==NULL || atoi(row[0])==0 )
				break;  // This username is unique
	}
	sSQL = "INSERT INTO Users(UserName,Password,PINCode,HasMailbox,AccessGeneralMailbox) "
		"VALUES('" + StringUtils::SQLEscape(sUsername) + "',md5('" + StringUtils::SQLEscape(sUsername) + "'),md5('1234')" +
		(bExistingUsers ? ",1,0)" : ",1,1)");

	int PK_Users = m_pDBHelper->threaded_db_wrapper_query_withID(sSQL);
	sSQL = "INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation,userCanChangeHouseMode) "
		"VALUES(" + StringUtils::itos(m_PK_Installation) + "," + StringUtils::itos(PK_Users) + 
		(bExistingUsers ? ",0,0)" : ",1,1)");

	m_pDBHelper->threaded_db_wrapper_query(sSQL);

	CheckExtensions();

	if( pCommand_Impl )
	{
		DeviceData_Base *pDevice_Core = pCommand_Impl->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
		if( pDevice_Core )
		{
			DeviceData_Base *pDevice_AppServer = 
				pDevice_Core->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
			if( pDevice_AppServer )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(pCommand_Impl->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
					"/usr/pluto/bin/SetPasswords.sh","set passwords",StringUtils::itos(PK_Users) + "\t" + sUsername,"","",false,false,false,true);
				CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Persist;
				pCommand_Impl->SendCommand(CMD_Spawn_Application);
			}
		}
	}
	else
	{
		string sCmd = "/usr/pluto/bin/SetPasswords.sh " + StringUtils::itos(PK_Users) + " \"" + sUsername + "\"";
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Executing %s",sCmd.c_str());
		system(sCmd.c_str());
	}

	DBHelper mySqlHelper_telecom(m_pDBHelper->m_sDBHost,m_pDBHelper->m_sDBUser,m_pDBHelper->m_sDBPass,"pluto_telecom");

	string sPK_Users = StringUtils::itos(PK_Users);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",1,0,0,1,'ring,');";
	mySqlHelper_telecom.threaded_db_wrapper_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",2,0,0,1,'voicemail,0');";
	mySqlHelper_telecom.threaded_db_wrapper_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",4,0,0,1,'voicemail,0');";
	mySqlHelper_telecom.threaded_db_wrapper_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",4,1,1,1,'ring,');";
	mySqlHelper_telecom.threaded_db_wrapper_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",3,0,0,1,'voicemail,0');";
	mySqlHelper_telecom.threaded_db_wrapper_query(sSQL);
	sSQL = "INSERT INTO `UserRouting` (`EK_Users`, `EK_UserMode`, `SendStatusFirst`, `IsPriorityCaller`, `StepOrder`, `Routing`) VALUES (" + sPK_Users + ",3,1,1,1,'ring,');";
	mySqlHelper_telecom.threaded_db_wrapper_query(sSQL);

	//NOTE: don't call setup users if we don't have a command impl
	//webadmin will run the script manually

	ConfirmMasterUserExists();

	if(NULL != pCommand_Impl)
	{
		DeviceData_Base *pDevice_Core = pCommand_Impl->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
		if(NULL != pDevice_Core)
		{
			DeviceData_Base *pDevice_AppServer = 
				pDevice_Core->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
			if(NULL != pDevice_AppServer)
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(pCommand_Impl->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
					"/usr/pluto/bin/SetupUsers.sh","setup users","","","",false,false,false,true);
				CMD_Spawn_Application.m_pMessage->m_eRetry=MR_Persist;
				pCommand_Impl->SendCommand(CMD_Spawn_Application);
			}
		}
	}

	return PK_Users;
}

void UserUtils::RemoveUser(int PK_User)
{
	string sSQL = "DELETE FROM Users WHERE PK_Users=" + StringUtils::itos(PK_User);
	m_pDBHelper->threaded_db_wrapper_query(sSQL);

	sSQL = "DELETE FROM Installation_Users WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pDBHelper->threaded_db_wrapper_query(sSQL);

	sSQL = "DELETE FROM Device_Users WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pDBHelper->threaded_db_wrapper_query(sSQL);

	sSQL = "DELETE FROM Orbiter_Users_PasswordReq WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pDBHelper->threaded_db_wrapper_query(sSQL);

	sSQL = "DELETE FROM Room_Users WHERE FK_Users=" + StringUtils::itos(PK_User);
	m_pDBHelper->threaded_db_wrapper_query(sSQL);

	ConfirmMasterUserExists();
}

void UserUtils::ConfirmMasterUserExists()
{
	if( !AlreadyHasMasterUsers() )
	{
		string sSQL = "SELECT min(FK_Users) FROM Users JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation=" + StringUtils::itos(m_PK_Installation);
		PlutoSqlResult result_set_user;
		DB_ROW row;
		if( (result_set_user.r=m_pDBHelper->db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set_user.r))!=NULL )
		{
			sSQL = "UPDATE Installation_Users set userCanModifyInstallation=1,userCanChangeHouseMode=1 WHERE FK_Users=" + string(row[0]);
			m_pDBHelper->threaded_db_wrapper_query(sSQL);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"User removed master user.  New master user is: %s", row[0]);
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No users to set as master");
	}
}
