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
#include "Table_psc_media_tables.h"



void Database_pluto_media::CreateTable_psc_media_tables()
{
	tblpsc_media_tables = new Table_psc_media_tables(this);
}

void Database_pluto_media::DeleteTable_psc_media_tables()
{
	if( tblpsc_media_tables )
		delete tblpsc_media_tables;
}

bool Database_pluto_media::Commit_psc_media_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblpsc_media_tables->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_psc_media_tables::~Table_psc_media_tables()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_psc_media_tables *pRow = (Row_psc_media_tables *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_psc_media_tables *pRow = (Row_psc_media_tables *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_psc_media_tables::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_psc_media_tables *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_psc_media_tables *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_psc_media_tables);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_psc_media_tables::Reload()
{
	Row_psc_media_tables *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_psc_media_tables);
		Row_psc_media_tables *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_media_tables::Row_psc_media_tables(Table_psc_media_tables *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_media_tables::SetDefaultValues()
{
	m_PK_psc_media_tables = 0;
is_null[0] = false;
m_Tablename = "";
is_null[1] = false;
is_null[2] = true;
m_frozen = 0;
is_null[3] = false;
m_anonymous = 0;
is_null[4] = false;
m_last_psc_id = 0;
is_null[5] = false;
m_last_psc_batch = 0;
is_null[6] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_media_tables::PK_psc_media_tables_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_psc_media_tables;}
string Row_psc_media_tables::Tablename_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Tablename;}
string Row_psc_media_tables::filter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_filter;}
short int Row_psc_media_tables::frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_frozen;}
short int Row_psc_media_tables::anonymous_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_anonymous;}
long int Row_psc_media_tables::last_psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_last_psc_id;}
long int Row_psc_media_tables::last_psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_last_psc_batch;}

		
void Row_psc_media_tables::PK_psc_media_tables_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_psc_media_tables = val; is_modified=true; is_null[0]=false;}
void Row_psc_media_tables::Tablename_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Tablename = val; is_modified=true; is_null[1]=false;}
void Row_psc_media_tables::filter_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_filter = val; is_modified=true; is_null[2]=false;}
void Row_psc_media_tables::frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_frozen = val; is_modified=true; is_null[3]=false;}
void Row_psc_media_tables::anonymous_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_anonymous = val; is_modified=true; is_null[4]=false;}
void Row_psc_media_tables::last_psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_last_psc_id = val; is_modified=true; is_null[5]=false;}
void Row_psc_media_tables::last_psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_last_psc_batch = val; is_modified=true; is_null[6]=false;}

		
bool Row_psc_media_tables::filter_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}

			
void Row_psc_media_tables::filter_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
	

string Row_psc_media_tables::PK_psc_media_tables_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_media_tables);

return buf;
}

string Row_psc_media_tables::Tablename_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Tablename.c_str(), (unsigned long) min((size_t)60,m_Tablename.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_psc_media_tables::filter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_filter.c_str(), (unsigned long) min((size_t)255,m_filter.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_psc_media_tables::frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_frozen);

return buf;
}

string Row_psc_media_tables::anonymous_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_anonymous);

return buf;
}

string Row_psc_media_tables::last_psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_last_psc_id);

return buf;
}

string Row_psc_media_tables::last_psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_last_psc_batch);

return buf;
}




Table_psc_media_tables::Key::Key(long int in_PK_psc_media_tables)
{
			pk_PK_psc_media_tables = in_PK_psc_media_tables;
	
}

Table_psc_media_tables::Key::Key(Row_psc_media_tables *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_psc_media_tables = pRow->m_PK_psc_media_tables;
	
}		

bool Table_psc_media_tables::Key_Less::operator()(const Table_psc_media_tables::Key &key1, const Table_psc_media_tables::Key &key2) const
{
			if (key1.pk_PK_psc_media_tables!=key2.pk_PK_psc_media_tables)
return key1.pk_PK_psc_media_tables<key2.pk_PK_psc_media_tables;
else
return false;	
}	

bool Table_psc_media_tables::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_psc_media_tables *pRow = (Row_psc_media_tables *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_media_tables_asSQL()+", "+pRow->Tablename_asSQL()+", "+pRow->filter_asSQL()+", "+pRow->frozen_asSQL()+", "+pRow->anonymous_asSQL()+", "+pRow->last_psc_id_asSQL()+", "+pRow->last_psc_batch_asSQL();

	
		string query = "insert into psc_media_tables (`PK_psc_media_tables`, `Tablename`, `filter`, `frozen`, `anonymous`, `last_psc_id`, `last_psc_batch`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_psc_media_tables=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_psc_media_tables is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_psc_media_tables);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_psc_media_tables* pRow = (Row_psc_media_tables*) (*i).second;	
		SingleLongKey key(pRow->m_PK_psc_media_tables);

		char tmp_PK_psc_media_tables[32];
sprintf(tmp_PK_psc_media_tables, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_tables`=" + tmp_PK_psc_media_tables;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_psc_media_tables`="+pRow->PK_psc_media_tables_asSQL()+", `Tablename`="+pRow->Tablename_asSQL()+", `filter`="+pRow->filter_asSQL()+", `frozen`="+pRow->frozen_asSQL()+", `anonymous`="+pRow->anonymous_asSQL()+", `last_psc_id`="+pRow->last_psc_id_asSQL()+", `last_psc_batch`="+pRow->last_psc_batch_asSQL();

	
		string query = "update psc_media_tables set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_psc_media_tables* pRow = (Row_psc_media_tables*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_psc_media_tables* pRow = (Row_psc_media_tables*) (*i).second;	

		char tmp_PK_psc_media_tables[32];
sprintf(tmp_PK_psc_media_tables, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_tables`=" + tmp_PK_psc_media_tables;

	
		string query = "delete from psc_media_tables where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_psc_media_tables*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_psc_media_tables::GetRows(string where_statement,vector<class Row_psc_media_tables*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `psc_media_tables`.* from psc_media_tables " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `psc_media_tables`.* from psc_media_tables where " + where_statement;
	else
		query = "select `psc_media_tables`.* from psc_media_tables";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_psc_media_tables *pRow = new Row_psc_media_tables(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_media_tables = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_media_tables));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Tablename = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Tablename = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_filter = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_filter = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_frozen = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_frozen));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_anonymous = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_anonymous));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_last_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_last_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_last_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_last_psc_batch));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_psc_media_tables);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_psc_media_tables *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_psc_media_tables* Table_psc_media_tables::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_psc_media_tables *pRow = new Row_psc_media_tables(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_media_tables* Table_psc_media_tables::GetRow(long int in_PK_psc_media_tables)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_psc_media_tables);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_psc_media_tables*) (*i).second;
	//we have to fetch row
	Row_psc_media_tables* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_media_tables* Table_psc_media_tables::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_psc_media_tables[32];
sprintf(tmp_PK_psc_media_tables, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_tables`=" + tmp_PK_psc_media_tables;


	string query = "select * from psc_media_tables where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_tables::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_psc_media_tables *pRow = new Row_psc_media_tables(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_media_tables = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_media_tables));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Tablename = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Tablename = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_filter = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_filter = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_frozen = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_frozen));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_anonymous = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_anonymous));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_last_psc_id = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_last_psc_id));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_last_psc_batch = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_last_psc_batch));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







