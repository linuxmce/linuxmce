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
#include "Table_RomAttribute.h"
#include "Table_RomAttributeType.h"
#include "Table_GameSystem.h"

#include "Table_Rom_RomAttribute.h"


void Database_lmce_game::CreateTable_RomAttribute()
{
	tblRomAttribute = new Table_RomAttribute(this);
}

void Database_lmce_game::DeleteTable_RomAttribute()
{
	if( tblRomAttribute )
		delete tblRomAttribute;
}

bool Database_lmce_game::Commit_RomAttribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblRomAttribute->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_RomAttribute::~Table_RomAttribute()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_RomAttribute *pRow = (Row_RomAttribute *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_RomAttribute *pRow = (Row_RomAttribute *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_RomAttribute::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_RomAttribute *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_RomAttribute *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_RomAttribute);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_RomAttribute::Reload()
{
	Row_RomAttribute *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_RomAttribute);
		Row_RomAttribute *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_RomAttribute::Row_RomAttribute(Table_RomAttribute *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_RomAttribute::SetDefaultValues()
{
	m_PK_RomAttribute = 0;
is_null[0] = false;
m_FK_RomAttributeType = 0;
is_null[1] = false;
m_FK_GameSystem = 0;
is_null[2] = false;
is_null[3] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_RomAttribute::PK_RomAttribute_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_RomAttribute;}
long int Row_RomAttribute::FK_RomAttributeType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_RomAttributeType;}
long int Row_RomAttribute::FK_GameSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_GameSystem;}
string Row_RomAttribute::Name_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Name;}

		
void Row_RomAttribute::PK_RomAttribute_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_RomAttribute = val; is_modified=true; is_null[0]=false;}
void Row_RomAttribute::FK_RomAttributeType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_RomAttributeType = val; is_modified=true; is_null[1]=false;}
void Row_RomAttribute::FK_GameSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_GameSystem = val; is_modified=true; is_null[2]=false;}
void Row_RomAttribute::Name_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Name = val; is_modified=true; is_null[3]=false;}

		
bool Row_RomAttribute::Name_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}

			
void Row_RomAttribute::Name_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
	

string Row_RomAttribute::PK_RomAttribute_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_RomAttribute);

return buf;
}

string Row_RomAttribute::FK_RomAttributeType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RomAttributeType);

return buf;
}

string Row_RomAttribute::FK_GameSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_GameSystem);

return buf;
}

string Row_RomAttribute::Name_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[5000000];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Name.c_str(), (unsigned long) min((size_t)-1,m_Name.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_RomAttribute::Key::Key(long int in_PK_RomAttribute)
{
			pk_PK_RomAttribute = in_PK_RomAttribute;
	
}

Table_RomAttribute::Key::Key(Row_RomAttribute *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_RomAttribute = pRow->m_PK_RomAttribute;
	
}		

bool Table_RomAttribute::Key_Less::operator()(const Table_RomAttribute::Key &key1, const Table_RomAttribute::Key &key2) const
{
			if (key1.pk_PK_RomAttribute!=key2.pk_PK_RomAttribute)
return key1.pk_PK_RomAttribute<key2.pk_PK_RomAttribute;
else
return false;	
}	

bool Table_RomAttribute::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_RomAttribute *pRow = (Row_RomAttribute *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_RomAttribute_asSQL()+", "+pRow->FK_RomAttributeType_asSQL()+", "+pRow->FK_GameSystem_asSQL()+", "+pRow->Name_asSQL();

	
		string query = "insert into RomAttribute (`PK_RomAttribute`, `FK_RomAttributeType`, `FK_GameSystem`, `Name`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_RomAttribute=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_RomAttribute is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_RomAttribute);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_RomAttribute* pRow = (Row_RomAttribute*) (*i).second;	
		SingleLongKey key(pRow->m_PK_RomAttribute);

		char tmp_PK_RomAttribute[32];
sprintf(tmp_PK_RomAttribute, "%li", key.pk);


string condition;
condition = condition + "`PK_RomAttribute`=" + tmp_PK_RomAttribute;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_RomAttribute`="+pRow->PK_RomAttribute_asSQL()+", `FK_RomAttributeType`="+pRow->FK_RomAttributeType_asSQL()+", `FK_GameSystem`="+pRow->FK_GameSystem_asSQL()+", `Name`="+pRow->Name_asSQL();

	
		string query = "update RomAttribute set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_RomAttribute* pRow = (Row_RomAttribute*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_RomAttribute* pRow = (Row_RomAttribute*) (*i).second;	

		char tmp_PK_RomAttribute[32];
sprintf(tmp_PK_RomAttribute, "%li", key.pk);


string condition;
condition = condition + "`PK_RomAttribute`=" + tmp_PK_RomAttribute;

	
		string query = "delete from RomAttribute where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_RomAttribute*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_RomAttribute::GetRows(string where_statement,vector<class Row_RomAttribute*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `RomAttribute`.* from RomAttribute " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `RomAttribute`.* from RomAttribute where " + where_statement;
	else
		query = "select `RomAttribute`.* from RomAttribute";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_RomAttribute *pRow = new Row_RomAttribute(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RomAttribute = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RomAttribute));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RomAttributeType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RomAttributeType));
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
pRow->m_Name = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Name = string(row[3],lengths[3]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_RomAttribute);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_RomAttribute *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_RomAttribute* Table_RomAttribute::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_RomAttribute *pRow = new Row_RomAttribute(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_RomAttribute* Table_RomAttribute::GetRow(long int in_PK_RomAttribute)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_RomAttribute);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_RomAttribute*) (*i).second;
	//we have to fetch row
	Row_RomAttribute* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_RomAttribute* Table_RomAttribute::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_RomAttribute[32];
sprintf(tmp_PK_RomAttribute, "%li", key.pk);


string condition;
condition = condition + "`PK_RomAttribute`=" + tmp_PK_RomAttribute;


	string query = "select * from RomAttribute where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RomAttribute::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_RomAttribute *pRow = new Row_RomAttribute(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RomAttribute = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RomAttribute));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RomAttributeType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RomAttributeType));
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
pRow->m_Name = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Name = string(row[3],lengths[3]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_RomAttributeType* Row_RomAttribute::FK_RomAttributeType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RomAttributeType *pTable = table->database->RomAttributeType_get();
return pTable->GetRow(m_FK_RomAttributeType);
}
class Row_GameSystem* Row_RomAttribute::FK_GameSystem_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_GameSystem *pTable = table->database->GameSystem_get();
return pTable->GetRow(m_FK_GameSystem);
}


void Row_RomAttribute::Rom_RomAttribute_FK_RomAttribute_getrows(vector <class Row_Rom_RomAttribute*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Rom_RomAttribute *pTable = table->database->Rom_RomAttribute_get();
pTable->GetRows("`FK_RomAttribute`=" + StringUtils::itos(m_PK_RomAttribute),rows);
}



