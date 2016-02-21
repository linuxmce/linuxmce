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
#include "Table_Game_GameSystem.h"
#include "Table_Game.h"
#include "Table_GameSystem.h"
#include "Table_Manufacturer.h"
#include "Table_Genre.h"



void Database_lmce_game::CreateTable_Game_GameSystem()
{
	tblGame_GameSystem = new Table_Game_GameSystem(this);
}

void Database_lmce_game::DeleteTable_Game_GameSystem()
{
	if( tblGame_GameSystem )
		delete tblGame_GameSystem;
}

bool Database_lmce_game::Commit_Game_GameSystem(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblGame_GameSystem->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Game_GameSystem::~Table_Game_GameSystem()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Game_GameSystem *pRow = (Row_Game_GameSystem *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Game_GameSystem *pRow = (Row_Game_GameSystem *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Game_GameSystem::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Game_GameSystem *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Game_GameSystem *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Game_GameSystem);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Game_GameSystem::Reload()
{
	Row_Game_GameSystem *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Game_GameSystem);
		Row_Game_GameSystem *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Game_GameSystem::Row_Game_GameSystem(Table_Game_GameSystem *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Game_GameSystem::SetDefaultValues()
{
	m_PK_Game_GameSystem = 0;
is_null[0] = false;
m_FK_Game = 0;
is_null[1] = false;
m_FK_GameSystem = 0;
is_null[2] = false;
m_FK_Manufacturer = -1;
is_null[3] = false;
m_FK_Genre = -1;
is_null[4] = false;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_Year = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Game_GameSystem::PK_Game_GameSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Game_GameSystem;}
long int Row_Game_GameSystem::FK_Game_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Game;}
long int Row_Game_GameSystem::FK_GameSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_GameSystem;}
long int Row_Game_GameSystem::FK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Manufacturer;}
long int Row_Game_GameSystem::FK_Genre_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Genre;}
string Row_Game_GameSystem::Name_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Name;}
string Row_Game_GameSystem::Subtitle_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Subtitle;}
long int Row_Game_GameSystem::Year_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Year;}

		
void Row_Game_GameSystem::PK_Game_GameSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Game_GameSystem = val; is_modified=true; is_null[0]=false;}
void Row_Game_GameSystem::FK_Game_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Game = val; is_modified=true; is_null[1]=false;}
void Row_Game_GameSystem::FK_GameSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_GameSystem = val; is_modified=true; is_null[2]=false;}
void Row_Game_GameSystem::FK_Manufacturer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Manufacturer = val; is_modified=true; is_null[3]=false;}
void Row_Game_GameSystem::FK_Genre_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Genre = val; is_modified=true; is_null[4]=false;}
void Row_Game_GameSystem::Name_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Name = val; is_modified=true; is_null[5]=false;}
void Row_Game_GameSystem::Subtitle_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Subtitle = val; is_modified=true; is_null[6]=false;}
void Row_Game_GameSystem::Year_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Year = val; is_modified=true; is_null[7]=false;}

		
bool Row_Game_GameSystem::Name_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Game_GameSystem::Subtitle_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Game_GameSystem::Year_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}

			
void Row_Game_GameSystem::Name_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Game_GameSystem::Subtitle_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Game_GameSystem::Year_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
	

string Row_Game_GameSystem::PK_Game_GameSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Game_GameSystem);

return buf;
}

string Row_Game_GameSystem::FK_Game_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Game);

return buf;
}

string Row_Game_GameSystem::FK_GameSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_GameSystem);

return buf;
}

string Row_Game_GameSystem::FK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Manufacturer);

return buf;
}

string Row_Game_GameSystem::FK_Genre_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Genre);

return buf;
}

string Row_Game_GameSystem::Name_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[769];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Name.c_str(), (unsigned long) min((size_t)384,m_Name.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Game_GameSystem::Subtitle_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[769];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Subtitle.c_str(), (unsigned long) min((size_t)384,m_Subtitle.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Game_GameSystem::Year_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Year);

return buf;
}




Table_Game_GameSystem::Key::Key(long int in_PK_Game_GameSystem)
{
			pk_PK_Game_GameSystem = in_PK_Game_GameSystem;
	
}

Table_Game_GameSystem::Key::Key(Row_Game_GameSystem *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Game_GameSystem = pRow->m_PK_Game_GameSystem;
	
}		

bool Table_Game_GameSystem::Key_Less::operator()(const Table_Game_GameSystem::Key &key1, const Table_Game_GameSystem::Key &key2) const
{
			if (key1.pk_PK_Game_GameSystem!=key2.pk_PK_Game_GameSystem)
return key1.pk_PK_Game_GameSystem<key2.pk_PK_Game_GameSystem;
else
return false;	
}	

bool Table_Game_GameSystem::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Game_GameSystem *pRow = (Row_Game_GameSystem *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Game_GameSystem_asSQL()+", "+pRow->FK_Game_asSQL()+", "+pRow->FK_GameSystem_asSQL()+", "+pRow->FK_Manufacturer_asSQL()+", "+pRow->FK_Genre_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->Subtitle_asSQL()+", "+pRow->Year_asSQL();

	
		string query = "insert into Game_GameSystem (`PK_Game_GameSystem`, `FK_Game`, `FK_GameSystem`, `FK_Manufacturer`, `FK_Genre`, `Name`, `Subtitle`, `Year`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Game_GameSystem=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Game_GameSystem is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Game_GameSystem);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Game_GameSystem* pRow = (Row_Game_GameSystem*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Game_GameSystem);

		char tmp_PK_Game_GameSystem[32];
sprintf(tmp_PK_Game_GameSystem, "%li", key.pk);


string condition;
condition = condition + "`PK_Game_GameSystem`=" + tmp_PK_Game_GameSystem;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Game_GameSystem`="+pRow->PK_Game_GameSystem_asSQL()+", `FK_Game`="+pRow->FK_Game_asSQL()+", `FK_GameSystem`="+pRow->FK_GameSystem_asSQL()+", `FK_Manufacturer`="+pRow->FK_Manufacturer_asSQL()+", `FK_Genre`="+pRow->FK_Genre_asSQL()+", `Name`="+pRow->Name_asSQL()+", `Subtitle`="+pRow->Subtitle_asSQL()+", `Year`="+pRow->Year_asSQL();

	
		string query = "update Game_GameSystem set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Game_GameSystem* pRow = (Row_Game_GameSystem*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Game_GameSystem* pRow = (Row_Game_GameSystem*) (*i).second;	

		char tmp_PK_Game_GameSystem[32];
sprintf(tmp_PK_Game_GameSystem, "%li", key.pk);


string condition;
condition = condition + "`PK_Game_GameSystem`=" + tmp_PK_Game_GameSystem;

	
		string query = "delete from Game_GameSystem where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Game_GameSystem*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Game_GameSystem::GetRows(string where_statement,vector<class Row_Game_GameSystem*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Game_GameSystem`.* from Game_GameSystem " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Game_GameSystem`.* from Game_GameSystem where " + where_statement;
	else
		query = "select `Game_GameSystem`.* from Game_GameSystem";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Game_GameSystem *pRow = new Row_Game_GameSystem(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Game_GameSystem = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Game_GameSystem));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Game = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Game));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_GameSystem = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_GameSystem));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Manufacturer));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Genre = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Genre));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Name = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Subtitle = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Subtitle = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Year = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Year));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Game_GameSystem);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Game_GameSystem *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Game_GameSystem* Table_Game_GameSystem::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Game_GameSystem *pRow = new Row_Game_GameSystem(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Game_GameSystem* Table_Game_GameSystem::GetRow(long int in_PK_Game_GameSystem)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Game_GameSystem);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Game_GameSystem*) (*i).second;
	//we have to fetch row
	Row_Game_GameSystem* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Game_GameSystem* Table_Game_GameSystem::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Game_GameSystem[32];
sprintf(tmp_PK_Game_GameSystem, "%li", key.pk);


string condition;
condition = condition + "`PK_Game_GameSystem`=" + tmp_PK_Game_GameSystem;


	string query = "select * from Game_GameSystem where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Game_GameSystem::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Game_GameSystem *pRow = new Row_Game_GameSystem(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Game_GameSystem = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Game_GameSystem));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Game = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Game));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_GameSystem = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_GameSystem));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Manufacturer));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Genre = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Genre));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Name = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Subtitle = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Subtitle = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Year = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Year));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Game* Row_Game_GameSystem::FK_Game_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Game *pTable = table->database->Game_get();
return pTable->GetRow(m_FK_Game);
}
class Row_GameSystem* Row_Game_GameSystem::FK_GameSystem_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_GameSystem *pTable = table->database->GameSystem_get();
return pTable->GetRow(m_FK_GameSystem);
}
class Row_Manufacturer* Row_Game_GameSystem::FK_Manufacturer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Manufacturer *pTable = table->database->Manufacturer_get();
return pTable->GetRow(m_FK_Manufacturer);
}
class Row_Genre* Row_Game_GameSystem::FK_Genre_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Genre *pTable = table->database->Genre_get();
return pTable->GetRow(m_FK_Genre);
}





