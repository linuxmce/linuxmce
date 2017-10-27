/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

// If using the thread logger, these generated classes create lots of activity
#ifdef NO_SQL_THREAD_LOG
#undef THREAD_LOG
#endif

#ifdef WIN32
#include <WinSock2.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_Room.h"
#include "Table_Installation.h"
#include "Table_RoomType.h"
#include "Table_Icon.h"
#include "Table_FloorplanObjectType.h"

#include "Table_CommandGroup_Room.h"
#include "Table_Device.h"
#include "Table_EntertainArea.h"
#include "Table_Room_Users.h"


void Database_pluto_main::CreateTable_Room()
{
	tblRoom = new Table_Room(this);
}

void Database_pluto_main::DeleteTable_Room()
{
	if( tblRoom )
		delete tblRoom;
}

bool Database_pluto_main::Commit_Room(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblRoom->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Room::~Table_Room()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Room *pRow = (Row_Room *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Room *pRow = (Row_Room *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Room::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Room *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Room *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Room);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Room::Reload()
{
	Row_Room *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Room);
		Row_Room *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Room::Row_Room(Table_Room *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Room::SetDefaultValues()
{
	m_PK_Room = 0;
is_null[0] = false;
m_FK_Installation = 0;
is_null[1] = false;
is_null[2] = true;
m_FK_RoomType = 0;
m_Description = "";
is_null[3] = false;
is_null[4] = true;
m_FK_Icon = 0;
m_ManuallyConfigureEA = 0;
is_null[5] = false;
m_HideFromOrbiter = 0;
is_null[6] = false;
is_null[7] = true;
m_FK_FloorplanObjectType = 0;
is_null[8] = true;
is_null[9] = true;
m_psc_id = 0;
is_null[10] = true;
m_psc_batch = 0;
is_null[11] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[12] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[13] = false;
is_null[14] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Room::PK_Room_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Room;}
long int Row_Room::FK_Installation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Installation;}
long int Row_Room::FK_RoomType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_RoomType;}
string Row_Room::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_Room::FK_Icon_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Icon;}
short int Row_Room::ManuallyConfigureEA_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ManuallyConfigureEA;}
short int Row_Room::HideFromOrbiter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HideFromOrbiter;}
long int Row_Room::FK_FloorplanObjectType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_FloorplanObjectType;}
string Row_Room::FloorplanInfo_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FloorplanInfo;}
long int Row_Room::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Room::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Room::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Room::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Room::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Room::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Room::PK_Room_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Room = val; is_modified=true; is_null[0]=false;}
void Row_Room::FK_Installation_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Installation = val; is_modified=true; is_null[1]=false;}
void Row_Room::FK_RoomType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_RoomType = val; is_modified=true; is_null[2]=false;}
void Row_Room::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_Room::FK_Icon_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Icon = val; is_modified=true; is_null[4]=false;}
void Row_Room::ManuallyConfigureEA_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ManuallyConfigureEA = val; is_modified=true; is_null[5]=false;}
void Row_Room::HideFromOrbiter_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HideFromOrbiter = val; is_modified=true; is_null[6]=false;}
void Row_Room::FK_FloorplanObjectType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_FloorplanObjectType = val; is_modified=true; is_null[7]=false;}
void Row_Room::FloorplanInfo_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FloorplanInfo = val; is_modified=true; is_null[8]=false;}
void Row_Room::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[9]=false;}
void Row_Room::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[10]=false;}
void Row_Room::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[11]=false;}
void Row_Room::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[12]=false;}
void Row_Room::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[13]=false;}
void Row_Room::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[14]=false;}

		
bool Row_Room::FK_RoomType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Room::FK_Icon_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Room::ManuallyConfigureEA_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Room::FK_FloorplanObjectType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Room::FloorplanInfo_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Room::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Room::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Room::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Room::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Room::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}

			
void Row_Room::FK_RoomType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Room::FK_Icon_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Room::ManuallyConfigureEA_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Room::FK_FloorplanObjectType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Room::FloorplanInfo_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Room::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Room::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Room::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Room::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Room::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
	

string Row_Room::PK_Room_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Room);

return buf;
}

string Row_Room::FK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation);

return buf;
}

string Row_Room::FK_RoomType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RoomType);

return buf;
}

string Row_Room::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[181];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)90,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Room::FK_Icon_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Icon);

return buf;
}

string Row_Room::ManuallyConfigureEA_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ManuallyConfigureEA);

return buf;
}

string Row_Room::HideFromOrbiter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HideFromOrbiter);

return buf;
}

string Row_Room::FK_FloorplanObjectType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_FloorplanObjectType);

return buf;
}

string Row_Room::FloorplanInfo_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[393211];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_FloorplanInfo.c_str(), (unsigned long) min((size_t)196605,m_FloorplanInfo.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Room::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Room::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Room::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Room::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Room::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Room::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Room::Key::Key(long int in_PK_Room)
{
			pk_PK_Room = in_PK_Room;
	
}

Table_Room::Key::Key(Row_Room *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Room = pRow->m_PK_Room;
	
}		

bool Table_Room::Key_Less::operator()(const Table_Room::Key &key1, const Table_Room::Key &key2) const
{
			if (key1.pk_PK_Room!=key2.pk_PK_Room)
return key1.pk_PK_Room<key2.pk_PK_Room;
else
return false;	
}	

bool Table_Room::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Room *pRow = (Row_Room *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Room_asSQL()+", "+pRow->FK_Installation_asSQL()+", "+pRow->FK_RoomType_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Icon_asSQL()+", "+pRow->ManuallyConfigureEA_asSQL()+", "+pRow->HideFromOrbiter_asSQL()+", "+pRow->FK_FloorplanObjectType_asSQL()+", "+pRow->FloorplanInfo_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Room (`PK_Room`, `FK_Installation`, `FK_RoomType`, `Description`, `FK_Icon`, `ManuallyConfigureEA`, `HideFromOrbiter`, `FK_FloorplanObjectType`, `FloorplanInfo`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedInsertRow )
				{
					addedRows.erase(i);
					delete pRow;
				}
				break;   // Go ahead and continue to do the updates
			}
		}
	
		if (db_wrapper_affected_rows(database->m_pDB)!=0)
		{
			
			
			long int id = (long int) db_wrapper_insert_id(database->m_pDB);
		
			if (id!=0)
		pRow->m_PK_Room=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Room is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Room);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Room* pRow = (Row_Room*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Room);

		char tmp_PK_Room[32];
sprintf(tmp_PK_Room, "%li", key.pk);


string condition;
condition = condition + "`PK_Room`=" + tmp_PK_Room;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Room`="+pRow->PK_Room_asSQL()+", `FK_Installation`="+pRow->FK_Installation_asSQL()+", `FK_RoomType`="+pRow->FK_RoomType_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_Icon`="+pRow->FK_Icon_asSQL()+", `ManuallyConfigureEA`="+pRow->ManuallyConfigureEA_asSQL()+", `HideFromOrbiter`="+pRow->HideFromOrbiter_asSQL()+", `FK_FloorplanObjectType`="+pRow->FK_FloorplanObjectType_asSQL()+", `FloorplanInfo`="+pRow->FloorplanInfo_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Room set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
			{
				if( bDeleteFailedModifiedRow )
				{
					cachedRows.erase(i);
					delete pRow;
				}
				break;  // Go ahead and do the deletes
			}
		}
	
		pRow->is_modified = false;	
	}	
	

//delete deleted added
	while (!deleted_addedRows.empty())
	{	
		vector<TableRow*>::iterator i = deleted_addedRows.begin();
		Row_Room* pRow = (Row_Room*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Room* pRow = (Row_Room*) (*i).second;	

		char tmp_PK_Room[32];
sprintf(tmp_PK_Room, "%li", key.pk);


string condition;
condition = condition + "`PK_Room`=" + tmp_PK_Room;

	
		string query = "delete from Room where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Room*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Room::GetRows(string where_statement,vector<class Row_Room*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Room`.* from Room " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Room`.* from Room where " + where_statement;
	else
		query = "select `Room`.* from Room";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Room *pRow = new Row_Room(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Room = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Room));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Installation));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_RoomType = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_RoomType));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Icon = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Icon));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ManuallyConfigureEA = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_ManuallyConfigureEA));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_HideFromOrbiter = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_HideFromOrbiter));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_FloorplanObjectType = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_FloorplanObjectType));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FloorplanInfo = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_FloorplanInfo = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_id));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_batch));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_user));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_psc_frozen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_psc_mod = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Room);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Room *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Room* Table_Room::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Room *pRow = new Row_Room(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Room* Table_Room::GetRow(long int in_PK_Room)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Room);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Room*) (*i).second;
	//we have to fetch row
	Row_Room* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Room* Table_Room::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Room[32];
sprintf(tmp_PK_Room, "%li", key.pk);


string condition;
condition = condition + "`PK_Room`=" + tmp_PK_Room;


	string query = "select * from Room where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Room::FetchRow db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return NULL;
	}	
	
	DB_ROW row = db_wrapper_fetch_row(res);
	
	if (!row)
	{
		//dataset is empty
		db_wrapper_free_result(res);			
		return NULL;		
	}	
						
	unsigned long *lengths = db_wrapper_fetch_lengths(res);

	Row_Room *pRow = new Row_Room(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Room = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Room));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Installation = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Installation));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_RoomType = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_RoomType));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Icon = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Icon));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ManuallyConfigureEA = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_ManuallyConfigureEA));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_HideFromOrbiter = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_HideFromOrbiter));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_FloorplanObjectType = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_FloorplanObjectType));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FloorplanInfo = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_FloorplanInfo = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_id));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_batch));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_user));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_psc_frozen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_psc_mod = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Installation* Row_Room::FK_Installation_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation);
}
class Row_RoomType* Row_Room::FK_RoomType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RoomType *pTable = table->database->RoomType_get();
return pTable->GetRow(m_FK_RoomType);
}
class Row_Icon* Row_Room::FK_Icon_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Icon *pTable = table->database->Icon_get();
return pTable->GetRow(m_FK_Icon);
}
class Row_FloorplanObjectType* Row_Room::FK_FloorplanObjectType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_FloorplanObjectType *pTable = table->database->FloorplanObjectType_get();
return pTable->GetRow(m_FK_FloorplanObjectType);
}


void Row_Room::CommandGroup_Room_FK_Room_getrows(vector <class Row_CommandGroup_Room*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_Room *pTable = table->database->CommandGroup_Room_get();
pTable->GetRows("`FK_Room`=" + StringUtils::itos(m_PK_Room),rows);
}
void Row_Room::Device_FK_Room_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("`FK_Room`=" + StringUtils::itos(m_PK_Room),rows);
}
void Row_Room::EntertainArea_FK_Room_getrows(vector <class Row_EntertainArea*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_EntertainArea *pTable = table->database->EntertainArea_get();
pTable->GetRows("`FK_Room`=" + StringUtils::itos(m_PK_Room),rows);
}
void Row_Room::Room_Users_FK_Room_getrows(vector <class Row_Room_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Room_Users *pTable = table->database->Room_Users_get();
pTable->GetRows("`FK_Room`=" + StringUtils::itos(m_PK_Room),rows);
}



