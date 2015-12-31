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
#include "Table_RECORDS.h"



void Database_xxv::CreateTable_RECORDS()
{
	tblRECORDS = new Table_RECORDS(this);
}

void Database_xxv::DeleteTable_RECORDS()
{
	if( tblRECORDS )
		delete tblRECORDS;
}

bool Database_xxv::Commit_RECORDS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblRECORDS->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_RECORDS::~Table_RECORDS()
{
	map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_RECORDS *pRow = (Row_RECORDS *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_RECORDS *pRow = (Row_RECORDS *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_RECORDS::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_RECORDS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_RECORDS *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLong64Key key(pRow->m_eventid);
			map<SingleLong64Key, TableRow*, SingleLong64Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_RECORDS::Reload()
{
	Row_RECORDS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLong64Key key(pRow->m_eventid);
		Row_RECORDS *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_RECORDS::Row_RECORDS(Table_RECORDS *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_RECORDS::SetDefaultValues()
{
	m_eventid = 0;
is_null[0] = false;
m_RecordId = 0;
is_null[1] = false;
m_Path = "";
is_null[2] = false;
m_Prio = 0;
is_null[3] = false;
m_Lifetime = 0;
is_null[4] = false;
m_State = 0;
is_null[5] = false;
is_null[6] = true;
m_Type = "TV";
is_null[7] = false;
m_addtime = "0000-00-00 00:00:00";
is_null[8] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

u_int64_t Row_RECORDS::eventid_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_eventid;}
long int Row_RECORDS::RecordId_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RecordId;}
string Row_RECORDS::Path_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Path;}
short int Row_RECORDS::Prio_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Prio;}
short int Row_RECORDS::Lifetime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Lifetime;}
short int Row_RECORDS::State_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_State;}
string Row_RECORDS::Marks_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Marks;}
string Row_RECORDS::Type_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Type;}
string Row_RECORDS::addtime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_addtime;}

		
void Row_RECORDS::eventid_set(u_int64_t val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_eventid = val; is_modified=true; is_null[0]=false;}
void Row_RECORDS::RecordId_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RecordId = val; is_modified=true; is_null[1]=false;}
void Row_RECORDS::Path_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Path = val; is_modified=true; is_null[2]=false;}
void Row_RECORDS::Prio_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Prio = val; is_modified=true; is_null[3]=false;}
void Row_RECORDS::Lifetime_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Lifetime = val; is_modified=true; is_null[4]=false;}
void Row_RECORDS::State_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_State = val; is_modified=true; is_null[5]=false;}
void Row_RECORDS::Marks_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Marks = val; is_modified=true; is_null[6]=false;}
void Row_RECORDS::Type_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Type = val; is_modified=true; is_null[7]=false;}
void Row_RECORDS::addtime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_addtime = val; is_modified=true; is_null[8]=false;}

		
bool Row_RECORDS::Marks_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_RECORDS::Type_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}

			
void Row_RECORDS::Marks_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_RECORDS::Type_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
	

string Row_RECORDS::eventid_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%I64i", m_eventid);

return buf;
}

string Row_RECORDS::RecordId_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_RecordId);

return buf;
}

string Row_RECORDS::Path_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Path.c_str(), (unsigned long) min((size_t)65535,m_Path.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RECORDS::Prio_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Prio);

return buf;
}

string Row_RECORDS::Lifetime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Lifetime);

return buf;
}

string Row_RECORDS::State_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_State);

return buf;
}

string Row_RECORDS::Marks_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Marks.c_str(), (unsigned long) min((size_t)65535,m_Marks.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RECORDS::Type_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[15];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Type.c_str(), (unsigned long) min((size_t)7,m_Type.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RECORDS::addtime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_addtime.c_str(), (unsigned long) min((size_t)19,m_addtime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_RECORDS::Key::Key(u_int64_t in_eventid)
{
			pk_eventid = in_eventid;
	
}

Table_RECORDS::Key::Key(Row_RECORDS *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_eventid = pRow->m_eventid;
	
}		

bool Table_RECORDS::Key_Less::operator()(const Table_RECORDS::Key &key1, const Table_RECORDS::Key &key2) const
{
			if (key1.pk_eventid!=key2.pk_eventid)
return key1.pk_eventid<key2.pk_eventid;
else
return false;	
}	

bool Table_RECORDS::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_RECORDS *pRow = (Row_RECORDS *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->eventid_asSQL()+", "+pRow->RecordId_asSQL()+", "+pRow->Path_asSQL()+", "+pRow->Prio_asSQL()+", "+pRow->Lifetime_asSQL()+", "+pRow->State_asSQL()+", "+pRow->Marks_asSQL()+", "+pRow->Type_asSQL()+", "+pRow->addtime_asSQL();

	
		string query = "insert into RECORDS (`eventid`, `RecordId`, `Path`, `Prio`, `Lifetime`, `State`, `Marks`, `Type`, `addtime`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			SingleLong64Key key(pRow->m_eventid);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_RECORDS* pRow = (Row_RECORDS*) (*i).second;	
		SingleLong64Key key(pRow->m_eventid);

		char tmp_eventid[32];
sprintf(tmp_eventid, "%I64i", key.pk);


string condition;
condition = condition + "`eventid`=" + tmp_eventid;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`eventid`="+pRow->eventid_asSQL()+", `RecordId`="+pRow->RecordId_asSQL()+", `Path`="+pRow->Path_asSQL()+", `Prio`="+pRow->Prio_asSQL()+", `Lifetime`="+pRow->Lifetime_asSQL()+", `State`="+pRow->State_asSQL()+", `Marks`="+pRow->Marks_asSQL()+", `Type`="+pRow->Type_asSQL()+", `addtime`="+pRow->addtime_asSQL();

	
		string query = "update RECORDS set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_RECORDS* pRow = (Row_RECORDS*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLong64Key key = (*i).first;
		Row_RECORDS* pRow = (Row_RECORDS*) (*i).second;	

		char tmp_eventid[32];
sprintf(tmp_eventid, "%I64i", key.pk);


string condition;
condition = condition + "`eventid`=" + tmp_eventid;

	
		string query = "delete from RECORDS where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_RECORDS*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_RECORDS::GetRows(string where_statement,vector<class Row_RECORDS*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `RECORDS`.* from RECORDS " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `RECORDS`.* from RECORDS where " + where_statement;
	else
		query = "select `RECORDS`.* from RECORDS";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_RECORDS *pRow = new Row_RECORDS(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_eventid = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%I64i", &(pRow->m_eventid));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_RecordId = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_RecordId));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Path = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Prio = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Prio));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Lifetime = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_Lifetime));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_State));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Marks = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Marks = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Type = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Type = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_addtime = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_addtime = string(row[8],lengths[8]);
}



		//checking for duplicates

		SingleLong64Key key(pRow->m_eventid);
		
		map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_RECORDS *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_RECORDS* Table_RECORDS::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_RECORDS *pRow = new Row_RECORDS(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_RECORDS* Table_RECORDS::GetRow(u_int64_t in_eventid)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLong64Key row_key(in_eventid);

	map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_RECORDS*) (*i).second;
	//we have to fetch row
	Row_RECORDS* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_RECORDS* Table_RECORDS::FetchRow(SingleLong64Key &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_eventid[32];
sprintf(tmp_eventid, "%I64i", key.pk);


string condition;
condition = condition + "`eventid`=" + tmp_eventid;


	string query = "select * from RECORDS where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RECORDS::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_RECORDS *pRow = new Row_RECORDS(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_eventid = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%I64i", &(pRow->m_eventid));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_RecordId = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_RecordId));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Path = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Prio = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Prio));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Lifetime = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_Lifetime));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_State));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Marks = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Marks = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Type = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Type = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_addtime = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_addtime = string(row[8],lengths[8]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







