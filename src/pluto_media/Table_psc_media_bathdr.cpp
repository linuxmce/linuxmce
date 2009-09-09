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
#include "Table_psc_media_bathdr.h"

#include "Table_psc_media_batdet.h"
#include "Table_psc_media_batdet.h"
#include "Table_psc_media_batdet.h"
#include "Table_psc_media_batdet.h"
#include "Table_psc_media_batuser.h"


void Database_pluto_media::CreateTable_psc_media_bathdr()
{
	tblpsc_media_bathdr = new Table_psc_media_bathdr(this);
}

void Database_pluto_media::DeleteTable_psc_media_bathdr()
{
	if( tblpsc_media_bathdr )
		delete tblpsc_media_bathdr;
}

bool Database_pluto_media::Commit_psc_media_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblpsc_media_bathdr->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_psc_media_bathdr::~Table_psc_media_bathdr()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_psc_media_bathdr *pRow = (Row_psc_media_bathdr *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_psc_media_bathdr *pRow = (Row_psc_media_bathdr *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_psc_media_bathdr::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_psc_media_bathdr *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_psc_media_bathdr *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_psc_media_bathdr);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_psc_media_bathdr::Reload()
{
	Row_psc_media_bathdr *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_psc_media_bathdr);
		Row_psc_media_bathdr *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_psc_media_bathdr::Row_psc_media_bathdr(Table_psc_media_bathdr *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_psc_media_bathdr::SetDefaultValues()
{
	m_PK_psc_media_bathdr = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_psc_media_bathdr::PK_psc_media_bathdr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_psc_media_bathdr;}
string Row_psc_media_bathdr::IPAddress_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IPAddress;}
string Row_psc_media_bathdr::date_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_date;}
string Row_psc_media_bathdr::comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_comments;}

		
void Row_psc_media_bathdr::PK_psc_media_bathdr_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_psc_media_bathdr = val; is_modified=true; is_null[0]=false;}
void Row_psc_media_bathdr::IPAddress_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IPAddress = val; is_modified=true; is_null[1]=false;}
void Row_psc_media_bathdr::date_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_date = val; is_modified=true; is_null[2]=false;}
void Row_psc_media_bathdr::comments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_comments = val; is_modified=true; is_null[3]=false;}

		
bool Row_psc_media_bathdr::IPAddress_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_psc_media_bathdr::date_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_psc_media_bathdr::comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}

			
void Row_psc_media_bathdr::IPAddress_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_psc_media_bathdr::date_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_psc_media_bathdr::comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
	

string Row_psc_media_bathdr::PK_psc_media_bathdr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_psc_media_bathdr);

return buf;
}

string Row_psc_media_bathdr::IPAddress_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[33];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_IPAddress.c_str(), (unsigned long) min((size_t)16,m_IPAddress.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_psc_media_bathdr::date_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_date.c_str(), (unsigned long) min((size_t)19,m_date.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_psc_media_bathdr::comments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_comments.c_str(), (unsigned long) min((size_t)65535,m_comments.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_psc_media_bathdr::Key::Key(long int in_PK_psc_media_bathdr)
{
			pk_PK_psc_media_bathdr = in_PK_psc_media_bathdr;
	
}

Table_psc_media_bathdr::Key::Key(Row_psc_media_bathdr *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_psc_media_bathdr = pRow->m_PK_psc_media_bathdr;
	
}		

bool Table_psc_media_bathdr::Key_Less::operator()(const Table_psc_media_bathdr::Key &key1, const Table_psc_media_bathdr::Key &key2) const
{
			if (key1.pk_PK_psc_media_bathdr!=key2.pk_PK_psc_media_bathdr)
return key1.pk_PK_psc_media_bathdr<key2.pk_PK_psc_media_bathdr;
else
return false;	
}	

bool Table_psc_media_bathdr::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_psc_media_bathdr *pRow = (Row_psc_media_bathdr *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_psc_media_bathdr_asSQL()+", "+pRow->IPAddress_asSQL()+", "+pRow->date_asSQL()+", "+pRow->comments_asSQL();

	
		string query = "insert into psc_media_bathdr (`PK_psc_media_bathdr`, `IPAddress`, `date`, `comments`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_psc_media_bathdr=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_psc_media_bathdr is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_psc_media_bathdr);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_psc_media_bathdr* pRow = (Row_psc_media_bathdr*) (*i).second;	
		SingleLongKey key(pRow->m_PK_psc_media_bathdr);

		char tmp_PK_psc_media_bathdr[32];
sprintf(tmp_PK_psc_media_bathdr, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_bathdr`=" + tmp_PK_psc_media_bathdr;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_psc_media_bathdr`="+pRow->PK_psc_media_bathdr_asSQL()+", `IPAddress`="+pRow->IPAddress_asSQL()+", `date`="+pRow->date_asSQL()+", `comments`="+pRow->comments_asSQL();

	
		string query = "update psc_media_bathdr set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_psc_media_bathdr* pRow = (Row_psc_media_bathdr*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_psc_media_bathdr* pRow = (Row_psc_media_bathdr*) (*i).second;	

		char tmp_PK_psc_media_bathdr[32];
sprintf(tmp_PK_psc_media_bathdr, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_bathdr`=" + tmp_PK_psc_media_bathdr;

	
		string query = "delete from psc_media_bathdr where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_psc_media_bathdr*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_psc_media_bathdr::GetRows(string where_statement,vector<class Row_psc_media_bathdr*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `psc_media_bathdr`.* from psc_media_bathdr " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `psc_media_bathdr`.* from psc_media_bathdr where " + where_statement;
	else
		query = "select `psc_media_bathdr`.* from psc_media_bathdr";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_psc_media_bathdr *pRow = new Row_psc_media_bathdr(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_media_bathdr = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_media_bathdr));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_IPAddress = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_IPAddress = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_date = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_date = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_comments = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_comments = string(row[3],lengths[3]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_psc_media_bathdr);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_psc_media_bathdr *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_psc_media_bathdr* Table_psc_media_bathdr::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_psc_media_bathdr *pRow = new Row_psc_media_bathdr(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_psc_media_bathdr* Table_psc_media_bathdr::GetRow(long int in_PK_psc_media_bathdr)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_psc_media_bathdr);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_psc_media_bathdr*) (*i).second;
	//we have to fetch row
	Row_psc_media_bathdr* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_psc_media_bathdr* Table_psc_media_bathdr::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_psc_media_bathdr[32];
sprintf(tmp_PK_psc_media_bathdr, "%li", key.pk);


string condition;
condition = condition + "`PK_psc_media_bathdr`=" + tmp_PK_psc_media_bathdr;


	string query = "select * from psc_media_bathdr where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_psc_media_bathdr::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_psc_media_bathdr *pRow = new Row_psc_media_bathdr(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_psc_media_bathdr = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_psc_media_bathdr));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_IPAddress = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_IPAddress = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_date = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_date = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_comments = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_comments = string(row[3],lengths[3]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}




void Row_psc_media_bathdr::psc_media_batdet_FK_psc_media_bathdr_getrows(vector <class Row_psc_media_batdet*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_media_batdet *pTable = table->database->psc_media_batdet_get();
pTable->GetRows("`FK_psc_media_bathdr`=" + StringUtils::itos(m_PK_psc_media_bathdr),rows);
}
void Row_psc_media_bathdr::psc_media_batdet_FK_psc_media_bathdr_orig_getrows(vector <class Row_psc_media_batdet*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_media_batdet *pTable = table->database->psc_media_batdet_get();
pTable->GetRows("`FK_psc_media_bathdr_orig`=" + StringUtils::itos(m_PK_psc_media_bathdr),rows);
}
void Row_psc_media_bathdr::psc_media_batdet_FK_psc_media_bathdr_auth_getrows(vector <class Row_psc_media_batdet*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_media_batdet *pTable = table->database->psc_media_batdet_get();
pTable->GetRows("`FK_psc_media_bathdr_auth`=" + StringUtils::itos(m_PK_psc_media_bathdr),rows);
}
void Row_psc_media_bathdr::psc_media_batdet_FK_psc_media_bathdr_unauth_getrows(vector <class Row_psc_media_batdet*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_media_batdet *pTable = table->database->psc_media_batdet_get();
pTable->GetRows("`FK_psc_media_bathdr_unauth`=" + StringUtils::itos(m_PK_psc_media_bathdr),rows);
}
void Row_psc_media_bathdr::psc_media_batuser_FK_psc_media_bathdr_getrows(vector <class Row_psc_media_batuser*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_psc_media_batuser *pTable = table->database->psc_media_batuser_get();
pTable->GetRows("`FK_psc_media_bathdr`=" + StringUtils::itos(m_PK_psc_media_bathdr),rows);
}



