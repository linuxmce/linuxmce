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
#include "Table_OutgoingLog.h"
#include "Table_PhoneNumber.h"



void Database_pluto_telecom::CreateTable_OutgoingLog()
{
	tblOutgoingLog = new Table_OutgoingLog(this);
}

void Database_pluto_telecom::DeleteTable_OutgoingLog()
{
	if( tblOutgoingLog )
		delete tblOutgoingLog;
}

bool Database_pluto_telecom::Commit_OutgoingLog(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblOutgoingLog->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_OutgoingLog::~Table_OutgoingLog()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_OutgoingLog *pRow = (Row_OutgoingLog *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_OutgoingLog *pRow = (Row_OutgoingLog *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_OutgoingLog::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_OutgoingLog *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_OutgoingLog *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_OutgoingLog);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_OutgoingLog::Reload()
{
	Row_OutgoingLog *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_OutgoingLog);
		Row_OutgoingLog *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_OutgoingLog::Row_OutgoingLog(Table_OutgoingLog *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_OutgoingLog::SetDefaultValues()
{
	m_PK_OutgoingLog = 0;
is_null[0] = false;
is_null[1] = true;
m_EK_Users = 0;
is_null[2] = true;
m_FK_PhoneNumber = 0;
is_null[3] = true;
is_null[4] = true;
m_Duration = 0;
m_Answered = 0;
is_null[5] = false;
is_null[6] = true;
m_psc_id = 0;
is_null[7] = true;
m_psc_batch = 0;
is_null[8] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[9] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[10] = false;
is_null[11] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_OutgoingLog::PK_OutgoingLog_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_OutgoingLog;}
long int Row_OutgoingLog::EK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Users;}
long int Row_OutgoingLog::FK_PhoneNumber_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PhoneNumber;}
string Row_OutgoingLog::PhoneNumber_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PhoneNumber;}
long int Row_OutgoingLog::Duration_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Duration;}
short int Row_OutgoingLog::Answered_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Answered;}
long int Row_OutgoingLog::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_OutgoingLog::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_OutgoingLog::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_OutgoingLog::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_OutgoingLog::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_OutgoingLog::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_OutgoingLog::PK_OutgoingLog_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_OutgoingLog = val; is_modified=true; is_null[0]=false;}
void Row_OutgoingLog::EK_Users_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Users = val; is_modified=true; is_null[1]=false;}
void Row_OutgoingLog::FK_PhoneNumber_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PhoneNumber = val; is_modified=true; is_null[2]=false;}
void Row_OutgoingLog::PhoneNumber_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PhoneNumber = val; is_modified=true; is_null[3]=false;}
void Row_OutgoingLog::Duration_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Duration = val; is_modified=true; is_null[4]=false;}
void Row_OutgoingLog::Answered_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Answered = val; is_modified=true; is_null[5]=false;}
void Row_OutgoingLog::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[6]=false;}
void Row_OutgoingLog::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[7]=false;}
void Row_OutgoingLog::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[8]=false;}
void Row_OutgoingLog::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[9]=false;}
void Row_OutgoingLog::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[10]=false;}
void Row_OutgoingLog::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[11]=false;}

		
bool Row_OutgoingLog::EK_Users_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_OutgoingLog::FK_PhoneNumber_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_OutgoingLog::PhoneNumber_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_OutgoingLog::Duration_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_OutgoingLog::Answered_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_OutgoingLog::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_OutgoingLog::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_OutgoingLog::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_OutgoingLog::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_OutgoingLog::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}

			
void Row_OutgoingLog::EK_Users_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_OutgoingLog::FK_PhoneNumber_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_OutgoingLog::PhoneNumber_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_OutgoingLog::Duration_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_OutgoingLog::Answered_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_OutgoingLog::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_OutgoingLog::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_OutgoingLog::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_OutgoingLog::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_OutgoingLog::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
	

string Row_OutgoingLog::PK_OutgoingLog_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_OutgoingLog);

return buf;
}

string Row_OutgoingLog::EK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Users);

return buf;
}

string Row_OutgoingLog::FK_PhoneNumber_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_PhoneNumber);

return buf;
}

string Row_OutgoingLog::PhoneNumber_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_PhoneNumber.c_str(), (unsigned long) min((size_t)30,m_PhoneNumber.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_OutgoingLog::Duration_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Duration);

return buf;
}

string Row_OutgoingLog::Answered_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Answered);

return buf;
}

string Row_OutgoingLog::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_OutgoingLog::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_OutgoingLog::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_OutgoingLog::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_OutgoingLog::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_OutgoingLog::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_OutgoingLog::Key::Key(long int in_PK_OutgoingLog)
{
			pk_PK_OutgoingLog = in_PK_OutgoingLog;
	
}

Table_OutgoingLog::Key::Key(Row_OutgoingLog *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_OutgoingLog = pRow->m_PK_OutgoingLog;
	
}		

bool Table_OutgoingLog::Key_Less::operator()(const Table_OutgoingLog::Key &key1, const Table_OutgoingLog::Key &key2) const
{
			if (key1.pk_PK_OutgoingLog!=key2.pk_PK_OutgoingLog)
return key1.pk_PK_OutgoingLog<key2.pk_PK_OutgoingLog;
else
return false;	
}	

bool Table_OutgoingLog::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_OutgoingLog *pRow = (Row_OutgoingLog *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_OutgoingLog_asSQL()+", "+pRow->EK_Users_asSQL()+", "+pRow->FK_PhoneNumber_asSQL()+", "+pRow->PhoneNumber_asSQL()+", "+pRow->Duration_asSQL()+", "+pRow->Answered_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into OutgoingLog (`PK_OutgoingLog`, `EK_Users`, `FK_PhoneNumber`, `PhoneNumber`, `Duration`, `Answered`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_OutgoingLog=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_OutgoingLog is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_OutgoingLog);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_OutgoingLog* pRow = (Row_OutgoingLog*) (*i).second;	
		SingleLongKey key(pRow->m_PK_OutgoingLog);

		char tmp_PK_OutgoingLog[32];
sprintf(tmp_PK_OutgoingLog, "%li", key.pk);


string condition;
condition = condition + "`PK_OutgoingLog`=" + tmp_PK_OutgoingLog;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_OutgoingLog`="+pRow->PK_OutgoingLog_asSQL()+", `EK_Users`="+pRow->EK_Users_asSQL()+", `FK_PhoneNumber`="+pRow->FK_PhoneNumber_asSQL()+", `PhoneNumber`="+pRow->PhoneNumber_asSQL()+", `Duration`="+pRow->Duration_asSQL()+", `Answered`="+pRow->Answered_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update OutgoingLog set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_OutgoingLog* pRow = (Row_OutgoingLog*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_OutgoingLog* pRow = (Row_OutgoingLog*) (*i).second;	

		char tmp_PK_OutgoingLog[32];
sprintf(tmp_PK_OutgoingLog, "%li", key.pk);


string condition;
condition = condition + "`PK_OutgoingLog`=" + tmp_PK_OutgoingLog;

	
		string query = "delete from OutgoingLog where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_OutgoingLog*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_OutgoingLog::GetRows(string where_statement,vector<class Row_OutgoingLog*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `OutgoingLog`.* from OutgoingLog " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `OutgoingLog`.* from OutgoingLog where " + where_statement;
	else
		query = "select `OutgoingLog`.* from OutgoingLog";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_OutgoingLog *pRow = new Row_OutgoingLog(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_OutgoingLog = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_OutgoingLog));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_EK_Users));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_PhoneNumber = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_PhoneNumber));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_PhoneNumber = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_PhoneNumber = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Duration = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Duration));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Answered = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_Answered));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_OutgoingLog);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_OutgoingLog *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_OutgoingLog* Table_OutgoingLog::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_OutgoingLog *pRow = new Row_OutgoingLog(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_OutgoingLog* Table_OutgoingLog::GetRow(long int in_PK_OutgoingLog)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_OutgoingLog);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_OutgoingLog*) (*i).second;
	//we have to fetch row
	Row_OutgoingLog* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_OutgoingLog* Table_OutgoingLog::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_OutgoingLog[32];
sprintf(tmp_PK_OutgoingLog, "%li", key.pk);


string condition;
condition = condition + "`PK_OutgoingLog`=" + tmp_PK_OutgoingLog;


	string query = "select * from OutgoingLog where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_OutgoingLog::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_OutgoingLog *pRow = new Row_OutgoingLog(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_OutgoingLog = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_OutgoingLog));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_EK_Users));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_PhoneNumber = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_PhoneNumber));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_PhoneNumber = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_PhoneNumber = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Duration = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Duration));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Answered = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_Answered));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_id));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_batch));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_user));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_psc_frozen));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_psc_mod = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_PhoneNumber* Row_OutgoingLog::FK_PhoneNumber_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PhoneNumber *pTable = table->database->PhoneNumber_get();
return pTable->GetRow(m_FK_PhoneNumber);
}





