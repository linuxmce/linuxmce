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
#include "Table_Alert.h"
#include "Table_AlertType.h"

#include "Table_Alert_Device.h"
#include "Table_Notification.h"


void Database_pluto_security::CreateTable_Alert()
{
	tblAlert = new Table_Alert(this);
}

void Database_pluto_security::DeleteTable_Alert()
{
	if( tblAlert )
		delete tblAlert;
}

bool Database_pluto_security::Commit_Alert(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblAlert->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Alert::~Table_Alert()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Alert *pRow = (Row_Alert *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Alert *pRow = (Row_Alert *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Alert::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Alert *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Alert *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Alert);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Alert::Reload()
{
	Row_Alert *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Alert);
		Row_Alert *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Alert::Row_Alert(Table_Alert *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Alert::SetDefaultValues()
{
	m_PK_Alert = 0;
is_null[0] = false;
is_null[1] = true;
m_FK_AlertType = 0;
m_EK_Device = 0;
is_null[2] = false;
m_DetectionTime = "0000-00-00 00:00:00";
is_null[3] = false;
is_null[4] = true;
is_null[5] = true;
m_ResetBeforeExpiration = 0;
is_null[6] = true;
m_Benign = 0;
is_null[7] = true;
is_null[8] = true;
m_EK_Users = 0;
m_AnnouncementOnly = 0;
is_null[9] = false;
m_PhotoOnly = 0;
is_null[10] = false;
m_Notification = 0;
is_null[11] = false;
is_null[12] = true;
m_psc_id = 0;
is_null[13] = true;
m_psc_batch = 0;
is_null[14] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[15] = false;
is_null[16] = true;
is_null[17] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Alert::PK_Alert_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Alert;}
long int Row_Alert::FK_AlertType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_AlertType;}
long int Row_Alert::EK_Device_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Device;}
string Row_Alert::DetectionTime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DetectionTime;}
string Row_Alert::ExpirationTime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ExpirationTime;}
short int Row_Alert::ResetBeforeExpiration_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ResetBeforeExpiration;}
short int Row_Alert::Benign_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Benign;}
string Row_Alert::ResetTime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ResetTime;}
long int Row_Alert::EK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Users;}
short int Row_Alert::AnnouncementOnly_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AnnouncementOnly;}
short int Row_Alert::PhotoOnly_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PhotoOnly;}
short int Row_Alert::Notification_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Notification;}
long int Row_Alert::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Alert::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Alert::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Alert::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Alert::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Alert::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Alert::PK_Alert_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Alert = val; is_modified=true; is_null[0]=false;}
void Row_Alert::FK_AlertType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_AlertType = val; is_modified=true; is_null[1]=false;}
void Row_Alert::EK_Device_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Device = val; is_modified=true; is_null[2]=false;}
void Row_Alert::DetectionTime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DetectionTime = val; is_modified=true; is_null[3]=false;}
void Row_Alert::ExpirationTime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ExpirationTime = val; is_modified=true; is_null[4]=false;}
void Row_Alert::ResetBeforeExpiration_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ResetBeforeExpiration = val; is_modified=true; is_null[5]=false;}
void Row_Alert::Benign_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Benign = val; is_modified=true; is_null[6]=false;}
void Row_Alert::ResetTime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ResetTime = val; is_modified=true; is_null[7]=false;}
void Row_Alert::EK_Users_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Users = val; is_modified=true; is_null[8]=false;}
void Row_Alert::AnnouncementOnly_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AnnouncementOnly = val; is_modified=true; is_null[9]=false;}
void Row_Alert::PhotoOnly_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PhotoOnly = val; is_modified=true; is_null[10]=false;}
void Row_Alert::Notification_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Notification = val; is_modified=true; is_null[11]=false;}
void Row_Alert::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[12]=false;}
void Row_Alert::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[13]=false;}
void Row_Alert::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[14]=false;}
void Row_Alert::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[15]=false;}
void Row_Alert::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[16]=false;}
void Row_Alert::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[17]=false;}

		
bool Row_Alert::FK_AlertType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_Alert::ExpirationTime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Alert::ResetBeforeExpiration_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Alert::Benign_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Alert::ResetTime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Alert::EK_Users_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Alert::AnnouncementOnly_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Alert::PhotoOnly_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Alert::Notification_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Alert::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Alert::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_Alert::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_Alert::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Alert::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_Alert::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}

			
void Row_Alert::FK_AlertType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_Alert::ExpirationTime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Alert::ResetBeforeExpiration_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Alert::Benign_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Alert::ResetTime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Alert::EK_Users_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Alert::AnnouncementOnly_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Alert::PhotoOnly_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Alert::Notification_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Alert::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Alert::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Alert::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Alert::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Alert::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_Alert::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
	

string Row_Alert::PK_Alert_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Alert);

return buf;
}

string Row_Alert::FK_AlertType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_AlertType);

return buf;
}

string Row_Alert::EK_Device_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Device);

return buf;
}

string Row_Alert::DetectionTime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DetectionTime.c_str(), (unsigned long) min((size_t)19,m_DetectionTime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Alert::ExpirationTime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ExpirationTime.c_str(), (unsigned long) min((size_t)19,m_ExpirationTime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Alert::ResetBeforeExpiration_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ResetBeforeExpiration);

return buf;
}

string Row_Alert::Benign_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Benign);

return buf;
}

string Row_Alert::ResetTime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ResetTime.c_str(), (unsigned long) min((size_t)19,m_ResetTime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Alert::EK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Users);

return buf;
}

string Row_Alert::AnnouncementOnly_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AnnouncementOnly);

return buf;
}

string Row_Alert::PhotoOnly_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PhotoOnly);

return buf;
}

string Row_Alert::Notification_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Notification);

return buf;
}

string Row_Alert::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Alert::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Alert::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Alert::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Alert::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Alert::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Alert::Key::Key(long int in_PK_Alert)
{
			pk_PK_Alert = in_PK_Alert;
	
}

Table_Alert::Key::Key(Row_Alert *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Alert = pRow->m_PK_Alert;
	
}		

bool Table_Alert::Key_Less::operator()(const Table_Alert::Key &key1, const Table_Alert::Key &key2) const
{
			if (key1.pk_PK_Alert!=key2.pk_PK_Alert)
return key1.pk_PK_Alert<key2.pk_PK_Alert;
else
return false;	
}	

bool Table_Alert::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Alert *pRow = (Row_Alert *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Alert_asSQL()+", "+pRow->FK_AlertType_asSQL()+", "+pRow->EK_Device_asSQL()+", "+pRow->DetectionTime_asSQL()+", "+pRow->ExpirationTime_asSQL()+", "+pRow->ResetBeforeExpiration_asSQL()+", "+pRow->Benign_asSQL()+", "+pRow->ResetTime_asSQL()+", "+pRow->EK_Users_asSQL()+", "+pRow->AnnouncementOnly_asSQL()+", "+pRow->PhotoOnly_asSQL()+", "+pRow->Notification_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Alert (`PK_Alert`, `FK_AlertType`, `EK_Device`, `DetectionTime`, `ExpirationTime`, `ResetBeforeExpiration`, `Benign`, `ResetTime`, `EK_Users`, `AnnouncementOnly`, `PhotoOnly`, `Notification`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Alert=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Alert is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Alert);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Alert* pRow = (Row_Alert*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Alert);

		char tmp_PK_Alert[32];
sprintf(tmp_PK_Alert, "%li", key.pk);


string condition;
condition = condition + "`PK_Alert`=" + tmp_PK_Alert;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Alert`="+pRow->PK_Alert_asSQL()+", `FK_AlertType`="+pRow->FK_AlertType_asSQL()+", `EK_Device`="+pRow->EK_Device_asSQL()+", `DetectionTime`="+pRow->DetectionTime_asSQL()+", `ExpirationTime`="+pRow->ExpirationTime_asSQL()+", `ResetBeforeExpiration`="+pRow->ResetBeforeExpiration_asSQL()+", `Benign`="+pRow->Benign_asSQL()+", `ResetTime`="+pRow->ResetTime_asSQL()+", `EK_Users`="+pRow->EK_Users_asSQL()+", `AnnouncementOnly`="+pRow->AnnouncementOnly_asSQL()+", `PhotoOnly`="+pRow->PhotoOnly_asSQL()+", `Notification`="+pRow->Notification_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Alert set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Alert* pRow = (Row_Alert*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Alert* pRow = (Row_Alert*) (*i).second;	

		char tmp_PK_Alert[32];
sprintf(tmp_PK_Alert, "%li", key.pk);


string condition;
condition = condition + "`PK_Alert`=" + tmp_PK_Alert;

	
		string query = "delete from Alert where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Alert*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Alert::GetRows(string where_statement,vector<class Row_Alert*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Alert`.* from Alert " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Alert`.* from Alert where " + where_statement;
	else
		query = "select `Alert`.* from Alert";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Alert *pRow = new Row_Alert(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Alert = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Alert));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AlertType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AlertType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Device));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_DetectionTime = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_DetectionTime = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ExpirationTime = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ExpirationTime = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ResetBeforeExpiration = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_ResetBeforeExpiration));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Benign = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Benign));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ResetTime = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ResetTime = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_EK_Users));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_AnnouncementOnly = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_AnnouncementOnly));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_PhotoOnly = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_PhotoOnly));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Notification = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_Notification));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_id));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_batch));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_user));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_psc_frozen));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_psc_mod = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Alert);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Alert *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Alert* Table_Alert::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Alert *pRow = new Row_Alert(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Alert* Table_Alert::GetRow(long int in_PK_Alert)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Alert);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Alert*) (*i).second;
	//we have to fetch row
	Row_Alert* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Alert* Table_Alert::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Alert[32];
sprintf(tmp_PK_Alert, "%li", key.pk);


string condition;
condition = condition + "`PK_Alert`=" + tmp_PK_Alert;


	string query = "select * from Alert where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Alert::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Alert *pRow = new Row_Alert(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Alert = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Alert));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_AlertType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_AlertType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_EK_Device));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_DetectionTime = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_DetectionTime = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ExpirationTime = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_ExpirationTime = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ResetBeforeExpiration = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_ResetBeforeExpiration));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Benign = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Benign));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_ResetTime = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_ResetTime = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_EK_Users));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_AnnouncementOnly = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_AnnouncementOnly));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_PhotoOnly = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_PhotoOnly));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Notification = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_Notification));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_id));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_batch));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_user));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_psc_frozen));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_psc_mod = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_AlertType* Row_Alert::FK_AlertType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_AlertType *pTable = table->database->AlertType_get();
return pTable->GetRow(m_FK_AlertType);
}


void Row_Alert::Alert_Device_FK_Alert_getrows(vector <class Row_Alert_Device*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Alert_Device *pTable = table->database->Alert_Device_get();
pTable->GetRows("`FK_Alert`=" + StringUtils::itos(m_PK_Alert),rows);
}
void Row_Alert::Notification_FK_Alert_getrows(vector <class Row_Notification*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Notification *pTable = table->database->Notification_get();
pTable->GetRows("`FK_Alert`=" + StringUtils::itos(m_PK_Alert),rows);
}



