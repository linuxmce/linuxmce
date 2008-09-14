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
#include "Table_Country.h"

#include "Table_City.h"
#include "Table_Installation.h"
#include "Table_PostalCode.h"
#include "Table_Region.h"
#include "Table_RepositorySource_URL.h"


void Database_pluto_main::CreateTable_Country()
{
	tblCountry = new Table_Country(this);
}

void Database_pluto_main::DeleteTable_Country()
{
	if( tblCountry )
		delete tblCountry;
}

bool Database_pluto_main::Commit_Country(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblCountry->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Country::~Table_Country()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Country *pRow = (Row_Country *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Country *pRow = (Row_Country *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Country::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Country *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Country *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Country);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_Country::Reload()
{
	Row_Country *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Country);
		Row_Country *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Country::Row_Country(Table_Country *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Country::SetDefaultValues()
{
	m_PK_Country = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
m_Define = "";
is_null[2] = false;
m_UsePostalCode = 0;
is_null[3] = false;
is_null[4] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Country::PK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Country;}
string Row_Country::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_Country::Define_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Define;}
short int Row_Country::UsePostalCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UsePostalCode;}
string Row_Country::country_code_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_country_code;}

		
void Row_Country::PK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Country = val; is_modified=true; is_null[0]=false;}
void Row_Country::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Country::Define_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_Country::UsePostalCode_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UsePostalCode = val; is_modified=true; is_null[3]=false;}
void Row_Country::country_code_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_country_code = val; is_modified=true; is_null[4]=false;}

		
bool Row_Country::country_code_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}

			
void Row_Country::country_code_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
	

string Row_Country::PK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Country);

return buf;
}

string Row_Country::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)30,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Country::Define_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Define.c_str(), (unsigned long) min((size_t)30,m_Define.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Country::UsePostalCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_UsePostalCode);

return buf;
}

string Row_Country::country_code_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[11];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_country_code.c_str(), (unsigned long) min((size_t)5,m_country_code.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_Country::Key::Key(long int in_PK_Country)
{
			pk_PK_Country = in_PK_Country;
	
}

Table_Country::Key::Key(Row_Country *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Country = pRow->m_PK_Country;
	
}		

bool Table_Country::Key_Less::operator()(const Table_Country::Key &key1, const Table_Country::Key &key2) const
{
			if (key1.pk_PK_Country!=key2.pk_PK_Country)
return key1.pk_PK_Country<key2.pk_PK_Country;
else
return false;	
}	

bool Table_Country::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Country *pRow = (Row_Country *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Country_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->UsePostalCode_asSQL()+", "+pRow->country_code_asSQL();

	
		string query = "insert into Country (`PK_Country`, `Description`, `Define`, `UsePostalCode`, `country_code`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Country=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Country is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Country);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Country* pRow = (Row_Country*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Country);

		char tmp_PK_Country[32];
sprintf(tmp_PK_Country, "%li", key.pk);


string condition;
condition = condition + "`PK_Country`=" + tmp_PK_Country;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Country`="+pRow->PK_Country_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `UsePostalCode`="+pRow->UsePostalCode_asSQL()+", `country_code`="+pRow->country_code_asSQL();

	
		string query = "update Country set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Country* pRow = (Row_Country*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Country* pRow = (Row_Country*) (*i).second;	

		char tmp_PK_Country[32];
sprintf(tmp_PK_Country, "%li", key.pk);


string condition;
condition = condition + "`PK_Country`=" + tmp_PK_Country;

	
		string query = "delete from Country where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Country*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Country::GetRows(string where_statement,vector<class Row_Country*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Country`.* from Country " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Country`.* from Country where " + where_statement;
	else
		query = "select `Country`.* from Country";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Country *pRow = new Row_Country(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Country = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Country));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Description = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_UsePostalCode = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_UsePostalCode));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_country_code = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_country_code = string(row[4],lengths[4]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Country);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Country *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Country* Table_Country::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Country *pRow = new Row_Country(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Country* Table_Country::GetRow(long int in_PK_Country)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Country);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Country*) (*i).second;
	//we have to fetch row
	Row_Country* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Country* Table_Country::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Country[32];
sprintf(tmp_PK_Country, "%li", key.pk);


string condition;
condition = condition + "`PK_Country`=" + tmp_PK_Country;


	string query = "select * from Country where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Country::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Country *pRow = new Row_Country(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Country = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Country));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Description = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_UsePostalCode = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_UsePostalCode));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_country_code = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_country_code = string(row[4],lengths[4]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}




void Row_Country::City_FK_Country_getrows(vector <class Row_City*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_City *pTable = table->database->City_get();
pTable->GetRows("`FK_Country`=" + StringUtils::itos(m_PK_Country),rows);
}
void Row_Country::Installation_FK_Country_getrows(vector <class Row_Installation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation *pTable = table->database->Installation_get();
pTable->GetRows("`FK_Country`=" + StringUtils::itos(m_PK_Country),rows);
}
void Row_Country::PostalCode_FK_Country_getrows(vector <class Row_PostalCode*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PostalCode *pTable = table->database->PostalCode_get();
pTable->GetRows("`FK_Country`=" + StringUtils::itos(m_PK_Country),rows);
}
void Row_Country::Region_FK_Country_getrows(vector <class Row_Region*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Region *pTable = table->database->Region_get();
pTable->GetRows("`FK_Country`=" + StringUtils::itos(m_PK_Country),rows);
}
void Row_Country::RepositorySource_URL_FK_Country_getrows(vector <class Row_RepositorySource_URL*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RepositorySource_URL *pTable = table->database->RepositorySource_URL_get();
pTable->GetRows("`FK_Country`=" + StringUtils::itos(m_PK_Country),rows);
}



