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
#include <winsock.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "Table_CHRONICLE.h"



void Database_xxv::CreateTable_CHRONICLE()
{
	tblCHRONICLE = new Table_CHRONICLE(this);
}

void Database_xxv::DeleteTable_CHRONICLE()
{
	if( tblCHRONICLE )
		delete tblCHRONICLE;
}

bool Database_xxv::Commit_CHRONICLE(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblCHRONICLE->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_CHRONICLE::~Table_CHRONICLE()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_CHRONICLE *pRow = (Row_CHRONICLE *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_CHRONICLE *pRow = (Row_CHRONICLE *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_CHRONICLE::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_CHRONICLE *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_CHRONICLE *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_id);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CHRONICLE::Reload()
{
	Row_CHRONICLE *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_id);
		Row_CHRONICLE *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CHRONICLE::Row_CHRONICLE(Table_CHRONICLE *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CHRONICLE::SetDefaultValues()
{
	m_id = 0;
is_null[0] = false;
m_hash = "";
is_null[1] = false;
m_title = "";
is_null[2] = false;
m_channel_id = "";
is_null[3] = false;
m_starttime = "0000-00-00 00:00:00";
is_null[4] = false;
m_duration = 0;
is_null[5] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_CHRONICLE::id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_id;}
string Row_CHRONICLE::hash_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_hash;}
string Row_CHRONICLE::title_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_title;}
string Row_CHRONICLE::channel_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_channel_id;}
string Row_CHRONICLE::starttime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_starttime;}
long int Row_CHRONICLE::duration_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_duration;}

		
void Row_CHRONICLE::id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_id = val; is_modified=true; is_null[0]=false;}
void Row_CHRONICLE::hash_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_hash = val; is_modified=true; is_null[1]=false;}
void Row_CHRONICLE::title_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_title = val; is_modified=true; is_null[2]=false;}
void Row_CHRONICLE::channel_id_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_channel_id = val; is_modified=true; is_null[3]=false;}
void Row_CHRONICLE::starttime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_starttime = val; is_modified=true; is_null[4]=false;}
void Row_CHRONICLE::duration_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_duration = val; is_modified=true; is_null[5]=false;}

		

			
	

string Row_CHRONICLE::id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_id);

return buf;
}

string Row_CHRONICLE::hash_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[33];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_hash.c_str(), (unsigned long) min((size_t)16,m_hash.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHRONICLE::title_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_title.c_str(), (unsigned long) min((size_t)65535,m_title.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHRONICLE::channel_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_channel_id.c_str(), (unsigned long) min((size_t)100,m_channel_id.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHRONICLE::starttime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_starttime.c_str(), (unsigned long) min((size_t)19,m_starttime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHRONICLE::duration_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_duration);

return buf;
}




Table_CHRONICLE::Key::Key(long int in_id)
{
			pk_id = in_id;
	
}

Table_CHRONICLE::Key::Key(Row_CHRONICLE *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_id = pRow->m_id;
	
}		

bool Table_CHRONICLE::Key_Less::operator()(const Table_CHRONICLE::Key &key1, const Table_CHRONICLE::Key &key2) const
{
			if (key1.pk_id!=key2.pk_id)
return key1.pk_id<key2.pk_id;
else
return false;	
}	

bool Table_CHRONICLE::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_CHRONICLE *pRow = (Row_CHRONICLE *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->id_asSQL()+", "+pRow->hash_asSQL()+", "+pRow->title_asSQL()+", "+pRow->channel_id_asSQL()+", "+pRow->starttime_asSQL()+", "+pRow->duration_asSQL();

	
		string query = "insert into CHRONICLE (`id`, `hash`, `title`, `channel_id`, `starttime`, `duration`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_id=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"id is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_id);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_CHRONICLE* pRow = (Row_CHRONICLE*) (*i).second;	
		SingleLongKey key(pRow->m_id);

		char tmp_id[32];
sprintf(tmp_id, "%li", key.pk);


string condition;
condition = condition + "`id`=" + tmp_id;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`id`="+pRow->id_asSQL()+", `hash`="+pRow->hash_asSQL()+", `title`="+pRow->title_asSQL()+", `channel_id`="+pRow->channel_id_asSQL()+", `starttime`="+pRow->starttime_asSQL()+", `duration`="+pRow->duration_asSQL();

	
		string query = "update CHRONICLE set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_CHRONICLE* pRow = (Row_CHRONICLE*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_CHRONICLE* pRow = (Row_CHRONICLE*) (*i).second;	

		char tmp_id[32];
sprintf(tmp_id, "%li", key.pk);


string condition;
condition = condition + "`id`=" + tmp_id;

	
		string query = "delete from CHRONICLE where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_CHRONICLE*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_CHRONICLE::GetRows(string where_statement,vector<class Row_CHRONICLE*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `CHRONICLE`.* from CHRONICLE " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `CHRONICLE`.* from CHRONICLE where " + where_statement;
	else
		query = "select `CHRONICLE`.* from CHRONICLE";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_CHRONICLE *pRow = new Row_CHRONICLE(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_id = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_id));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_hash = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_hash = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_title = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_title = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_channel_id = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_channel_id = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_starttime = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_starttime = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_duration = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_duration));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_id);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_CHRONICLE *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_CHRONICLE* Table_CHRONICLE::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_CHRONICLE *pRow = new Row_CHRONICLE(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CHRONICLE* Table_CHRONICLE::GetRow(long int in_id)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_id);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_CHRONICLE*) (*i).second;
	//we have to fetch row
	Row_CHRONICLE* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CHRONICLE* Table_CHRONICLE::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_id[32];
sprintf(tmp_id, "%li", key.pk);


string condition;
condition = condition + "`id`=" + tmp_id;


	string query = "select * from CHRONICLE where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHRONICLE::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_CHRONICLE *pRow = new Row_CHRONICLE(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_id = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_id));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_hash = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_hash = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_title = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_title = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_channel_id = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_channel_id = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_starttime = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_starttime = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_duration = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_duration));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







