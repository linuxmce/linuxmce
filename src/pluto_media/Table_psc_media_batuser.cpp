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
#include "Table_psc_media_batuser.h"
#include "Table_psc_media_bathdr.h"



void Database_pluto_media::CreateTable_psc_media_batuser()
{
	tblpsc_media_batuser = new Table_psc_media_batuser(this);
}

void Database_pluto_media::DeleteTable_psc_media_batuser()
{
	if( tblpsc_media_batuser )
		delete tblpsc_media_batuser;
}

bool Database_pluto_media::Commit_psc_media_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblpsc_media_batuser->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_psc_media_batuser::~Table_psc_media_batuser()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_psc_media_batuser *pRow = (Row_psc_media_batuser *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_psc_media_batuser *pRow = (Row_psc_media_batuser *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_psc_media_batuser::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_psc_media_batuser *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_psc_media_batuser *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_psc_media_batuser);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_psc_media_batuser::Reload()
{
	Row_psc_media_batuser *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_psc_media_batuser);
		Row_psc_media_batuser *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_media_batuser::Row_psc_media_batuser(Table_psc_media_batuser *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_media_batuser::SetDefaultValues()
{
	m_PK_psc_media_batuser = 0;
is_null[0] = false;
m_FK_psc_media_bathdr = 0;
is_null[1] = false;
m_psc_user = 0;
is_null[2] = false;
m_is_sup = 0;
is_null[3] = false;
m_no_pass = 0;
is_null[4] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_media_batuser::PK_psc_media_batuser_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_psc_media_batuser;}
long int Row_psc_media_batuser::FK_psc_media_bathdr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_psc_media_bathdr;}
long int Row_psc_media_batuser::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_psc_media_batuser::is_sup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_is_sup;}
short int Row_psc_media_batuser::no_pass_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_no_pass;}

		
void Row_psc_media_batuser::PK_psc_media_batuser_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_psc_media_batuser = val; is_modified=true; is_null[0]=false;}
void Row_psc_media_batuser::FK_psc_media_bathdr_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_psc_media_bathdr = val; is_modified=true; is_null[1]=false;}
void Row_psc_media_batuser::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[2]=false;}
void Row_psc_media_batuser::is_sup_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_is_sup = val; is_modified=true; is_null[3]=false;}
void Row_psc_media_batuser::no_pass_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_no_pass = val; is_modified=true; is_null[4]=false;}

		

			
	

string Row_psc_media_batuser::PK_psc_media_batuser_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_media_batuser);

return buf;
}

string Row_psc_media_batuser::FK_psc_media_bathdr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_psc_media_bathdr);

return buf;
}

string Row_psc_media_batuser::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_psc_media_batuser::is_sup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_is_sup);

return buf;
}

string Row_psc_media_batuser::no_pass_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_no_pass);

return buf;
}




Table_psc_media_batuser::Key::Key(long int in_PK_psc_media_batuser)
{
			pk_PK_psc_media_batuser = in_PK_psc_media_batuser;
	
}

Table_psc_media_batuser::Key::Key(Row_psc_media_batuser *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_psc_media_batuser = pRow->m_PK_psc_media_batuser;
	
}		

bool Table_psc_media_batuser::Key_Less::operator()(const Table_psc_media_batuser::Key &key1, const Table_psc_media_batuser::Key &key2) const
{
			if (key1.pk_PK_psc_media_batuser!=key2.pk_PK_psc_media_batuser)
return key1.pk_PK_psc_media_batuser<key2.pk_PK_psc_media_batuser;
else
return false;	
}	

bool Table_psc_media_batuser::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_psc_media_batuser *pRow = (Row_psc_media_batuser *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_media_batuser_asSQL()+", "+pRow->FK_psc_media_bathdr_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->is_sup_asSQL()+", "+pRow->no_pass_asSQL();

	
		string query = "insert into psc_media_batuser (`PK_psc_media_batuser`, `FK_psc_media_bathdr`, `psc_user`, `is_sup`, `no_pass`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_psc_media_batuser=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_psc_media_batuser is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_psc_media_batuser);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_psc_media_batuser* pRow = (Row_psc_media_batuser*) (*i).second;	
		SingleLongKey key(pRow->m_PK_psc_media_batuser);

		char tmp_PK_psc_media_batuser[32];
sprintf(tmp_PK_psc_media_batuser, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_batuser`=" + tmp_PK_psc_media_batuser;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_psc_media_batuser`="+pRow->PK_psc_media_batuser_asSQL()+", `FK_psc_media_bathdr`="+pRow->FK_psc_media_bathdr_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `is_sup`="+pRow->is_sup_asSQL()+", `no_pass`="+pRow->no_pass_asSQL();

	
		string query = "update psc_media_batuser set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_psc_media_batuser* pRow = (Row_psc_media_batuser*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_psc_media_batuser* pRow = (Row_psc_media_batuser*) (*i).second;	

		char tmp_PK_psc_media_batuser[32];
sprintf(tmp_PK_psc_media_batuser, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_batuser`=" + tmp_PK_psc_media_batuser;

	
		string query = "delete from psc_media_batuser where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_psc_media_batuser*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_psc_media_batuser::GetRows(string where_statement,vector<class Row_psc_media_batuser*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `psc_media_batuser`.* from psc_media_batuser " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `psc_media_batuser`.* from psc_media_batuser where " + where_statement;
	else
		query = "select `psc_media_batuser`.* from psc_media_batuser";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_psc_media_batuser *pRow = new Row_psc_media_batuser(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_media_batuser = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_media_batuser));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_psc_media_bathdr = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_psc_media_bathdr));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_user));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_is_sup = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_is_sup));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_no_pass = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_no_pass));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_psc_media_batuser);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_psc_media_batuser *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_psc_media_batuser* Table_psc_media_batuser::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_psc_media_batuser *pRow = new Row_psc_media_batuser(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_media_batuser* Table_psc_media_batuser::GetRow(long int in_PK_psc_media_batuser)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_psc_media_batuser);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_psc_media_batuser*) (*i).second;
	//we have to fetch row
	Row_psc_media_batuser* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_media_batuser* Table_psc_media_batuser::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_psc_media_batuser[32];
sprintf(tmp_PK_psc_media_batuser, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_batuser`=" + tmp_PK_psc_media_batuser;


	string query = "select * from psc_media_batuser where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_batuser::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_psc_media_batuser *pRow = new Row_psc_media_batuser(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_media_batuser = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_media_batuser));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_psc_media_bathdr = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_psc_media_bathdr));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_psc_user));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_is_sup = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_is_sup));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_no_pass = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_no_pass));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_psc_media_bathdr* Row_psc_media_batuser::FK_psc_media_bathdr_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_media_bathdr *pTable = table->database->psc_media_bathdr_get();
return pTable->GetRow(m_FK_psc_media_bathdr);
}





