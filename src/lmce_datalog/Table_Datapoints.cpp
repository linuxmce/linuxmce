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
#include "Table_Datapoints.h"
#include "Table_Unit.h"



void Database_lmce_datalog::CreateTable_Datapoints()
{
	tblDatapoints = new Table_Datapoints(this);
}

void Database_lmce_datalog::DeleteTable_Datapoints()
{
	if( tblDatapoints )
		delete tblDatapoints;
}

bool Database_lmce_datalog::Commit_Datapoints(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDatapoints->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Datapoints::~Table_Datapoints()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Datapoints *pRow = (Row_Datapoints *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Datapoints *pRow = (Row_Datapoints *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Datapoints::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Datapoints *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Datapoints *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Datapoints);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Datapoints::Reload()
{
	Row_Datapoints *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Datapoints);
		Row_Datapoints *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Datapoints::Row_Datapoints(Table_Datapoints *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Datapoints::SetDefaultValues()
{
	m_PK_Datapoints = 0;
is_null[0] = false;
is_null[1] = true;
m_Datapoint = 0;
m_FK_Unit = 0;
is_null[2] = false;
m_EK_Device = 0;
is_null[3] = false;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Datapoints::PK_Datapoints_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Datapoints;}
float Row_Datapoints::Datapoint_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Datapoint;}
long int Row_Datapoints::FK_Unit_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Unit;}
long int Row_Datapoints::EK_Device_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Device;}
string Row_Datapoints::timestamp_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_timestamp;}

		
void Row_Datapoints::PK_Datapoints_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Datapoints = val; is_modified=true; is_null[0]=false;}
void Row_Datapoints::Datapoint_set(float val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Datapoint = val; is_modified=true; is_null[1]=false;}
void Row_Datapoints::FK_Unit_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Unit = val; is_modified=true; is_null[2]=false;}
void Row_Datapoints::EK_Device_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Device = val; is_modified=true; is_null[3]=false;}
void Row_Datapoints::timestamp_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_timestamp = val; is_modified=true; is_null[4]=false;}

		
bool Row_Datapoints::Datapoint_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_Datapoints::timestamp_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}

			
void Row_Datapoints::Datapoint_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_Datapoints::timestamp_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
	

string Row_Datapoints::PK_Datapoints_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Datapoints);

return buf;
}

string Row_Datapoints::Datapoint_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%f", m_Datapoint);

return buf;
}

string Row_Datapoints::FK_Unit_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Unit);

return buf;
}

string Row_Datapoints::EK_Device_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Device);

return buf;
}

string Row_Datapoints::timestamp_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_timestamp.c_str(), (unsigned long) min((size_t)19,m_timestamp.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_Datapoints::Key::Key(long int in_PK_Datapoints)
{
			pk_PK_Datapoints = in_PK_Datapoints;
	
}

Table_Datapoints::Key::Key(Row_Datapoints *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Datapoints = pRow->m_PK_Datapoints;
	
}		

bool Table_Datapoints::Key_Less::operator()(const Table_Datapoints::Key &key1, const Table_Datapoints::Key &key2) const
{
			if (key1.pk_PK_Datapoints!=key2.pk_PK_Datapoints)
return key1.pk_PK_Datapoints<key2.pk_PK_Datapoints;
else
return false;	
}	

bool Table_Datapoints::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Datapoints *pRow = (Row_Datapoints *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Datapoints_asSQL()+", "+pRow->Datapoint_asSQL()+", "+pRow->FK_Unit_asSQL()+", "+pRow->EK_Device_asSQL()+", "+pRow->timestamp_asSQL();

	
		string query = "insert into Datapoints (`PK_Datapoints`, `Datapoint`, `FK_Unit`, `EK_Device`, `timestamp`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Datapoints=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Datapoints is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Datapoints);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Datapoints* pRow = (Row_Datapoints*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Datapoints);

		char tmp_PK_Datapoints[32];
sprintf(tmp_PK_Datapoints, "%li", key.pk);


string condition;
condition = condition + "`PK_Datapoints`=" + tmp_PK_Datapoints;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Datapoints`="+pRow->PK_Datapoints_asSQL()+", `Datapoint`="+pRow->Datapoint_asSQL()+", `FK_Unit`="+pRow->FK_Unit_asSQL()+", `EK_Device`="+pRow->EK_Device_asSQL()+", `timestamp`="+pRow->timestamp_asSQL();

	
		string query = "update Datapoints set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Datapoints* pRow = (Row_Datapoints*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Datapoints* pRow = (Row_Datapoints*) (*i).second;	

		char tmp_PK_Datapoints[32];
sprintf(tmp_PK_Datapoints, "%li", key.pk);


string condition;
condition = condition + "`PK_Datapoints`=" + tmp_PK_Datapoints;

	
		string query = "delete from Datapoints where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Datapoints*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Datapoints::GetRows(string where_statement,vector<class Row_Datapoints*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Datapoints`.* from Datapoints " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Datapoints`.* from Datapoints where " + where_statement;
	else
		query = "select `Datapoints`.* from Datapoints";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Datapoints *pRow = new Row_Datapoints(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Datapoints = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Datapoints));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Datapoint = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%f", &(pRow->m_Datapoint));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Unit = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Unit));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_EK_Device));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_timestamp = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_timestamp = string(row[4],lengths[4]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Datapoints);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Datapoints *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Datapoints* Table_Datapoints::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Datapoints *pRow = new Row_Datapoints(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Datapoints* Table_Datapoints::GetRow(long int in_PK_Datapoints)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Datapoints);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Datapoints*) (*i).second;
	//we have to fetch row
	Row_Datapoints* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Datapoints* Table_Datapoints::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Datapoints[32];
sprintf(tmp_PK_Datapoints, "%li", key.pk);


string condition;
condition = condition + "`PK_Datapoints`=" + tmp_PK_Datapoints;


	string query = "select * from Datapoints where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Datapoints::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Datapoints *pRow = new Row_Datapoints(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Datapoints = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Datapoints));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Datapoint = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%f", &(pRow->m_Datapoint));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Unit = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Unit));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_EK_Device = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_EK_Device));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_timestamp = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_timestamp = string(row[4],lengths[4]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Unit* Row_Datapoints::FK_Unit_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Unit *pTable = table->database->Unit_get();
return pTable->GetRow(m_FK_Unit);
}





