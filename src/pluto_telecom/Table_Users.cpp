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
#include "Table_Users.h"



void Database_pluto_telecom::CreateTable_Users()
{
	tblUsers = new Table_Users(this);
}

void Database_pluto_telecom::DeleteTable_Users()
{
	if( tblUsers )
		delete tblUsers;
}

bool Database_pluto_telecom::Commit_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblUsers->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Users::~Table_Users()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Users *pRow = (Row_Users *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Users *pRow = (Row_Users *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Users::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Users *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Users *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_EK_Users);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Users::Reload()
{
	Row_Users *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_EK_Users);
		Row_Users *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Users::Row_Users(Table_Users *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Users::SetDefaultValues()
{
	m_EK_Users = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_psc_id = 0;
is_null[3] = true;
m_psc_batch = 0;
is_null[4] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[5] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[6] = false;
is_null[7] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Users::EK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Users;}
string Row_Users::SyncSettings_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SyncSettings;}
long int Row_Users::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Users::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Users::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Users::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Users::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Users::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Users::EK_Users_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Users = val; is_modified=true; is_null[0]=false;}
void Row_Users::SyncSettings_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SyncSettings = val; is_modified=true; is_null[1]=false;}
void Row_Users::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[2]=false;}
void Row_Users::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[3]=false;}
void Row_Users::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[4]=false;}
void Row_Users::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[5]=false;}
void Row_Users::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[6]=false;}
void Row_Users::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[7]=false;}

		
bool Row_Users::SyncSettings_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_Users::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Users::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Users::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Users::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Users::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}

			
void Row_Users::SyncSettings_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_Users::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Users::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Users::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Users::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Users::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
	

string Row_Users::EK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Users);

return buf;
}

string Row_Users::SyncSettings_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_SyncSettings.c_str(), (unsigned long) min((size_t)65535,m_SyncSettings.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Users::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Users::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Users::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Users::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Users::Key::Key(long int in_EK_Users)
{
			pk_EK_Users = in_EK_Users;
	
}

Table_Users::Key::Key(Row_Users *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_EK_Users = pRow->m_EK_Users;
	
}		

bool Table_Users::Key_Less::operator()(const Table_Users::Key &key1, const Table_Users::Key &key2) const
{
			if (key1.pk_EK_Users!=key2.pk_EK_Users)
return key1.pk_EK_Users<key2.pk_EK_Users;
else
return false;	
}	

bool Table_Users::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Users *pRow = (Row_Users *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->EK_Users_asSQL()+", "+pRow->SyncSettings_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Users (`EK_Users`, `SyncSettings`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		
				
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_EK_Users);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Users* pRow = (Row_Users*) (*i).second;	
		SingleLongKey key(pRow->m_EK_Users);

		char tmp_EK_Users[32];
sprintf(tmp_EK_Users, "%li", key.pk);


string condition;
condition = condition + "`EK_Users`=" + tmp_EK_Users;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`EK_Users`="+pRow->EK_Users_asSQL()+", `SyncSettings`="+pRow->SyncSettings_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Users set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Users* pRow = (Row_Users*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Users* pRow = (Row_Users*) (*i).second;	

		char tmp_EK_Users[32];
sprintf(tmp_EK_Users, "%li", key.pk);


string condition;
condition = condition + "`EK_Users`=" + tmp_EK_Users;

	
		string query = "delete from Users where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Users*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Users::GetRows(string where_statement,vector<class Row_Users*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Users`.* from Users " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Users`.* from Users where " + where_statement;
	else
		query = "select `Users`.* from Users";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Users *pRow = new Row_Users(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_EK_Users));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_SyncSettings = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_SyncSettings = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_id));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_batch));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_user));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_psc_frozen));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_psc_mod = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_EK_Users);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Users *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Users* Table_Users::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Users *pRow = new Row_Users(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Users* Table_Users::GetRow(long int in_EK_Users)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_EK_Users);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Users*) (*i).second;
	//we have to fetch row
	Row_Users* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Users* Table_Users::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_EK_Users[32];
sprintf(tmp_EK_Users, "%li", key.pk);


string condition;
condition = condition + "`EK_Users`=" + tmp_EK_Users;


	string query = "select * from Users where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Users *pRow = new Row_Users(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_EK_Users = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_EK_Users));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_SyncSettings = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_SyncSettings = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_id));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_batch));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_user));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_psc_frozen));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_psc_mod = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







