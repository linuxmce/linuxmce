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
#include "Table_Rom_RomAttribute.h"
#include "Table_Rom.h"
#include "Table_RomAttribute.h"



void Database_lmce_game::CreateTable_Rom_RomAttribute()
{
	tblRom_RomAttribute = new Table_Rom_RomAttribute(this);
}

void Database_lmce_game::DeleteTable_Rom_RomAttribute()
{
	if( tblRom_RomAttribute )
		delete tblRom_RomAttribute;
}

bool Database_lmce_game::Commit_Rom_RomAttribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblRom_RomAttribute->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Rom_RomAttribute::~Table_Rom_RomAttribute()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Rom_RomAttribute *pRow = (Row_Rom_RomAttribute *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Rom_RomAttribute *pRow = (Row_Rom_RomAttribute *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Rom_RomAttribute::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Rom_RomAttribute *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Rom_RomAttribute *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_Rom,pRow->m_FK_RomAttribute);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Rom_RomAttribute::Reload()
{
	Row_Rom_RomAttribute *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_Rom,pRow->m_FK_RomAttribute);
		Row_Rom_RomAttribute *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Rom_RomAttribute::Row_Rom_RomAttribute(Table_Rom_RomAttribute *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Rom_RomAttribute::SetDefaultValues()
{
	m_FK_Rom = 0;
is_null[0] = false;
m_FK_RomAttribute = 0;
is_null[1] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Rom_RomAttribute::FK_Rom_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Rom;}
long int Row_Rom_RomAttribute::FK_RomAttribute_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_RomAttribute;}

		
void Row_Rom_RomAttribute::FK_Rom_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Rom = val; is_modified=true; is_null[0]=false;}
void Row_Rom_RomAttribute::FK_RomAttribute_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_RomAttribute = val; is_modified=true; is_null[1]=false;}

		

			
	

string Row_Rom_RomAttribute::FK_Rom_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Rom);

return buf;
}

string Row_Rom_RomAttribute::FK_RomAttribute_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RomAttribute);

return buf;
}




Table_Rom_RomAttribute::Key::Key(long int in_FK_Rom, long int in_FK_RomAttribute)
{
			pk_FK_Rom = in_FK_Rom;
pk_FK_RomAttribute = in_FK_RomAttribute;
	
}

Table_Rom_RomAttribute::Key::Key(Row_Rom_RomAttribute *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_FK_Rom = pRow->m_FK_Rom;
pk_FK_RomAttribute = pRow->m_FK_RomAttribute;
	
}		

bool Table_Rom_RomAttribute::Key_Less::operator()(const Table_Rom_RomAttribute::Key &key1, const Table_Rom_RomAttribute::Key &key2) const
{
			if (key1.pk_FK_Rom!=key2.pk_FK_Rom)
return key1.pk_FK_Rom<key2.pk_FK_Rom;
else
if (key1.pk_FK_RomAttribute!=key2.pk_FK_RomAttribute)
return key1.pk_FK_RomAttribute<key2.pk_FK_RomAttribute;
else
return false;	
}	

bool Table_Rom_RomAttribute::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Rom_RomAttribute *pRow = (Row_Rom_RomAttribute *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_Rom_asSQL()+", "+pRow->FK_RomAttribute_asSQL();

	
		string query = "insert into Rom_RomAttribute (`FK_Rom`, `FK_RomAttribute`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			DoubleLongKey key(pRow->m_FK_Rom,pRow->m_FK_RomAttribute);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Rom_RomAttribute* pRow = (Row_Rom_RomAttribute*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_Rom,pRow->m_FK_RomAttribute);

		char tmp_FK_Rom[32];
sprintf(tmp_FK_Rom, "%li", key.pk1);

char tmp_FK_RomAttribute[32];
sprintf(tmp_FK_RomAttribute, "%li", key.pk2);


string condition;
condition = condition + "`FK_Rom`=" + tmp_FK_Rom+" AND "+"`FK_RomAttribute`=" + tmp_FK_RomAttribute;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_Rom`="+pRow->FK_Rom_asSQL()+", `FK_RomAttribute`="+pRow->FK_RomAttribute_asSQL();

	
		string query = "update Rom_RomAttribute set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Rom_RomAttribute* pRow = (Row_Rom_RomAttribute*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Rom_RomAttribute* pRow = (Row_Rom_RomAttribute*) (*i).second;	

		char tmp_FK_Rom[32];
sprintf(tmp_FK_Rom, "%li", key.pk1);

char tmp_FK_RomAttribute[32];
sprintf(tmp_FK_RomAttribute, "%li", key.pk2);


string condition;
condition = condition + "`FK_Rom`=" + tmp_FK_Rom+" AND "+"`FK_RomAttribute`=" + tmp_FK_RomAttribute;

	
		string query = "delete from Rom_RomAttribute where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Rom_RomAttribute*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Rom_RomAttribute::GetRows(string where_statement,vector<class Row_Rom_RomAttribute*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Rom_RomAttribute`.* from Rom_RomAttribute " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Rom_RomAttribute`.* from Rom_RomAttribute where " + where_statement;
	else
		query = "select `Rom_RomAttribute`.* from Rom_RomAttribute";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Rom_RomAttribute *pRow = new Row_Rom_RomAttribute(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Rom = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Rom));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RomAttribute = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RomAttribute));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_Rom,pRow->m_FK_RomAttribute);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Rom_RomAttribute *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Rom_RomAttribute* Table_Rom_RomAttribute::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Rom_RomAttribute *pRow = new Row_Rom_RomAttribute(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Rom_RomAttribute* Table_Rom_RomAttribute::GetRow(long int in_FK_Rom, long int in_FK_RomAttribute)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	DoubleLongKey row_key(in_FK_Rom, in_FK_RomAttribute);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Rom_RomAttribute*) (*i).second;
	//we have to fetch row
	Row_Rom_RomAttribute* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Rom_RomAttribute* Table_Rom_RomAttribute::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_FK_Rom[32];
sprintf(tmp_FK_Rom, "%li", key.pk1);

char tmp_FK_RomAttribute[32];
sprintf(tmp_FK_RomAttribute, "%li", key.pk2);


string condition;
condition = condition + "`FK_Rom`=" + tmp_FK_Rom+" AND "+"`FK_RomAttribute`=" + tmp_FK_RomAttribute;


	string query = "select * from Rom_RomAttribute where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Rom_RomAttribute::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Rom_RomAttribute *pRow = new Row_Rom_RomAttribute(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_Rom = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_Rom));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RomAttribute = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RomAttribute));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Rom* Row_Rom_RomAttribute::FK_Rom_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Rom *pTable = table->database->Rom_get();
return pTable->GetRow(m_FK_Rom);
}
class Row_RomAttribute* Row_Rom_RomAttribute::FK_RomAttribute_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RomAttribute *pTable = table->database->RomAttribute_get();
return pTable->GetRow(m_FK_RomAttribute);
}





