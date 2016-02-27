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
#include "Table_Game_GameSystem_Configuration.h"
#include "Table_Game_GameSystem.h"
#include "Table_Configuration.h"



void Database_lmce_game::CreateTable_Game_GameSystem_Configuration()
{
	tblGame_GameSystem_Configuration = new Table_Game_GameSystem_Configuration(this);
}

void Database_lmce_game::DeleteTable_Game_GameSystem_Configuration()
{
	if( tblGame_GameSystem_Configuration )
		delete tblGame_GameSystem_Configuration;
}

bool Database_lmce_game::Commit_Game_GameSystem_Configuration(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblGame_GameSystem_Configuration->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Game_GameSystem_Configuration::~Table_Game_GameSystem_Configuration()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Game_GameSystem_Configuration *pRow = (Row_Game_GameSystem_Configuration *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Game_GameSystem_Configuration *pRow = (Row_Game_GameSystem_Configuration *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Game_GameSystem_Configuration::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Game_GameSystem_Configuration *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Game_GameSystem_Configuration *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Game_GameSystem_Configuration);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Game_GameSystem_Configuration::Reload()
{
	Row_Game_GameSystem_Configuration *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Game_GameSystem_Configuration);
		Row_Game_GameSystem_Configuration *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Game_GameSystem_Configuration::Row_Game_GameSystem_Configuration(Table_Game_GameSystem_Configuration *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Game_GameSystem_Configuration::SetDefaultValues()
{
	m_PK_Game_GameSystem_Configuration = 0;
is_null[0] = false;
m_FK_Game_GameSystem = 0;
is_null[1] = false;
m_FK_Configuration = 0;
is_null[2] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Game_GameSystem_Configuration::PK_Game_GameSystem_Configuration_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Game_GameSystem_Configuration;}
long int Row_Game_GameSystem_Configuration::FK_Game_GameSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Game_GameSystem;}
long int Row_Game_GameSystem_Configuration::FK_Configuration_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Configuration;}

		
void Row_Game_GameSystem_Configuration::PK_Game_GameSystem_Configuration_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Game_GameSystem_Configuration = val; is_modified=true; is_null[0]=false;}
void Row_Game_GameSystem_Configuration::FK_Game_GameSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Game_GameSystem = val; is_modified=true; is_null[1]=false;}
void Row_Game_GameSystem_Configuration::FK_Configuration_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Configuration = val; is_modified=true; is_null[2]=false;}

		

			
	

string Row_Game_GameSystem_Configuration::PK_Game_GameSystem_Configuration_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Game_GameSystem_Configuration);

return buf;
}

string Row_Game_GameSystem_Configuration::FK_Game_GameSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Game_GameSystem);

return buf;
}

string Row_Game_GameSystem_Configuration::FK_Configuration_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Configuration);

return buf;
}




Table_Game_GameSystem_Configuration::Key::Key(long int in_PK_Game_GameSystem_Configuration)
{
			pk_PK_Game_GameSystem_Configuration = in_PK_Game_GameSystem_Configuration;
	
}

Table_Game_GameSystem_Configuration::Key::Key(Row_Game_GameSystem_Configuration *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Game_GameSystem_Configuration = pRow->m_PK_Game_GameSystem_Configuration;
	
}		

bool Table_Game_GameSystem_Configuration::Key_Less::operator()(const Table_Game_GameSystem_Configuration::Key &key1, const Table_Game_GameSystem_Configuration::Key &key2) const
{
			if (key1.pk_PK_Game_GameSystem_Configuration!=key2.pk_PK_Game_GameSystem_Configuration)
return key1.pk_PK_Game_GameSystem_Configuration<key2.pk_PK_Game_GameSystem_Configuration;
else
return false;	
}	

bool Table_Game_GameSystem_Configuration::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Game_GameSystem_Configuration *pRow = (Row_Game_GameSystem_Configuration *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Game_GameSystem_Configuration_asSQL()+", "+pRow->FK_Game_GameSystem_asSQL()+", "+pRow->FK_Configuration_asSQL();

	
		string query = "insert into Game_GameSystem_Configuration (`PK_Game_GameSystem_Configuration`, `FK_Game_GameSystem`, `FK_Configuration`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Game_GameSystem_Configuration=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Game_GameSystem_Configuration is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Game_GameSystem_Configuration);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Game_GameSystem_Configuration* pRow = (Row_Game_GameSystem_Configuration*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Game_GameSystem_Configuration);

		char tmp_PK_Game_GameSystem_Configuration[32];
sprintf(tmp_PK_Game_GameSystem_Configuration, "%li", key.pk);


string condition;
condition = condition + "`PK_Game_GameSystem_Configuration`=" + tmp_PK_Game_GameSystem_Configuration;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Game_GameSystem_Configuration`="+pRow->PK_Game_GameSystem_Configuration_asSQL()+", `FK_Game_GameSystem`="+pRow->FK_Game_GameSystem_asSQL()+", `FK_Configuration`="+pRow->FK_Configuration_asSQL();

	
		string query = "update Game_GameSystem_Configuration set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Game_GameSystem_Configuration* pRow = (Row_Game_GameSystem_Configuration*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Game_GameSystem_Configuration* pRow = (Row_Game_GameSystem_Configuration*) (*i).second;	

		char tmp_PK_Game_GameSystem_Configuration[32];
sprintf(tmp_PK_Game_GameSystem_Configuration, "%li", key.pk);


string condition;
condition = condition + "`PK_Game_GameSystem_Configuration`=" + tmp_PK_Game_GameSystem_Configuration;

	
		string query = "delete from Game_GameSystem_Configuration where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Game_GameSystem_Configuration*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Game_GameSystem_Configuration::GetRows(string where_statement,vector<class Row_Game_GameSystem_Configuration*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Game_GameSystem_Configuration`.* from Game_GameSystem_Configuration " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Game_GameSystem_Configuration`.* from Game_GameSystem_Configuration where " + where_statement;
	else
		query = "select `Game_GameSystem_Configuration`.* from Game_GameSystem_Configuration";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Game_GameSystem_Configuration *pRow = new Row_Game_GameSystem_Configuration(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Game_GameSystem_Configuration = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Game_GameSystem_Configuration));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Game_GameSystem = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Game_GameSystem));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Configuration = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Configuration));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Game_GameSystem_Configuration);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Game_GameSystem_Configuration *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Game_GameSystem_Configuration* Table_Game_GameSystem_Configuration::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Game_GameSystem_Configuration *pRow = new Row_Game_GameSystem_Configuration(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Game_GameSystem_Configuration* Table_Game_GameSystem_Configuration::GetRow(long int in_PK_Game_GameSystem_Configuration)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Game_GameSystem_Configuration);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Game_GameSystem_Configuration*) (*i).second;
	//we have to fetch row
	Row_Game_GameSystem_Configuration* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Game_GameSystem_Configuration* Table_Game_GameSystem_Configuration::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Game_GameSystem_Configuration[32];
sprintf(tmp_PK_Game_GameSystem_Configuration, "%li", key.pk);


string condition;
condition = condition + "`PK_Game_GameSystem_Configuration`=" + tmp_PK_Game_GameSystem_Configuration;


	string query = "select * from Game_GameSystem_Configuration where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem_Configuration::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Game_GameSystem_Configuration *pRow = new Row_Game_GameSystem_Configuration(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Game_GameSystem_Configuration = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Game_GameSystem_Configuration));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Game_GameSystem = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Game_GameSystem));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Configuration = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Configuration));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Game_GameSystem* Row_Game_GameSystem_Configuration::FK_Game_GameSystem_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Game_GameSystem *pTable = table->database->Game_GameSystem_get();
return pTable->GetRow(m_FK_Game_GameSystem);
}
class Row_Configuration* Row_Game_GameSystem_Configuration::FK_Configuration_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Configuration *pTable = table->database->Configuration_get();
return pTable->GetRow(m_FK_Configuration);
}





