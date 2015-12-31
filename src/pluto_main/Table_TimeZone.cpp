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
#include "Table_TimeZone.h"

#include "Table_City.h"


void Database_pluto_main::CreateTable_TimeZone()
{
	tblTimeZone = new Table_TimeZone(this);
}

void Database_pluto_main::DeleteTable_TimeZone()
{
	if( tblTimeZone )
		delete tblTimeZone;
}

bool Database_pluto_main::Commit_TimeZone(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblTimeZone->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_TimeZone::~Table_TimeZone()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_TimeZone *pRow = (Row_TimeZone *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_TimeZone *pRow = (Row_TimeZone *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_TimeZone::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_TimeZone *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_TimeZone *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_TimeZone);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_TimeZone::Reload()
{
	Row_TimeZone *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_TimeZone);
		Row_TimeZone *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_TimeZone::Row_TimeZone(Table_TimeZone *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_TimeZone::SetDefaultValues()
{
	m_PK_TimeZone = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
m_TimeZoneOffset = 0;
is_null[4] = true;
m_SummerTimeOffset = 0;
is_null[5] = true;
is_null[6] = true;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_TimeZone::PK_TimeZone_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_TimeZone;}
string Row_TimeZone::ZoneName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ZoneName;}
string Row_TimeZone::Coordinate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Coordinate;}
double Row_TimeZone::TimeZoneOffset_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TimeZoneOffset;}
double Row_TimeZone::SummerTimeOffset_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SummerTimeOffset;}
string Row_TimeZone::Comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Comments;}
string Row_TimeZone::CountryCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CountryCode;}

		
void Row_TimeZone::PK_TimeZone_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_TimeZone = val; is_modified=true; is_null[0]=false;}
void Row_TimeZone::ZoneName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ZoneName = val; is_modified=true; is_null[1]=false;}
void Row_TimeZone::Coordinate_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Coordinate = val; is_modified=true; is_null[2]=false;}
void Row_TimeZone::TimeZoneOffset_set(double val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TimeZoneOffset = val; is_modified=true; is_null[3]=false;}
void Row_TimeZone::SummerTimeOffset_set(double val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SummerTimeOffset = val; is_modified=true; is_null[4]=false;}
void Row_TimeZone::Comments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Comments = val; is_modified=true; is_null[5]=false;}
void Row_TimeZone::CountryCode_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CountryCode = val; is_modified=true; is_null[6]=false;}

		
bool Row_TimeZone::ZoneName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_TimeZone::Coordinate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_TimeZone::TimeZoneOffset_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_TimeZone::SummerTimeOffset_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_TimeZone::Comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_TimeZone::CountryCode_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}

			
void Row_TimeZone::ZoneName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_TimeZone::Coordinate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_TimeZone::TimeZoneOffset_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_TimeZone::SummerTimeOffset_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_TimeZone::Comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_TimeZone::CountryCode_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
	

string Row_TimeZone::PK_TimeZone_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_TimeZone);

return buf;
}

string Row_TimeZone::ZoneName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ZoneName.c_str(), (unsigned long) min((size_t)50,m_ZoneName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TimeZone::Coordinate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[31];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Coordinate.c_str(), (unsigned long) min((size_t)15,m_Coordinate.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TimeZone::TimeZoneOffset_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%lf", m_TimeZoneOffset);

return buf;
}

string Row_TimeZone::SummerTimeOffset_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%lf", m_SummerTimeOffset);

return buf;
}

string Row_TimeZone::Comments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Comments.c_str(), (unsigned long) min((size_t)100,m_Comments.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TimeZone::CountryCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[11];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_CountryCode.c_str(), (unsigned long) min((size_t)5,m_CountryCode.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_TimeZone::Key::Key(long int in_PK_TimeZone)
{
			pk_PK_TimeZone = in_PK_TimeZone;
	
}

Table_TimeZone::Key::Key(Row_TimeZone *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_TimeZone = pRow->m_PK_TimeZone;
	
}		

bool Table_TimeZone::Key_Less::operator()(const Table_TimeZone::Key &key1, const Table_TimeZone::Key &key2) const
{
			if (key1.pk_PK_TimeZone!=key2.pk_PK_TimeZone)
return key1.pk_PK_TimeZone<key2.pk_PK_TimeZone;
else
return false;	
}	

bool Table_TimeZone::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_TimeZone *pRow = (Row_TimeZone *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_TimeZone_asSQL()+", "+pRow->ZoneName_asSQL()+", "+pRow->Coordinate_asSQL()+", "+pRow->TimeZoneOffset_asSQL()+", "+pRow->SummerTimeOffset_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->CountryCode_asSQL();

	
		string query = "insert into TimeZone (`PK_TimeZone`, `ZoneName`, `Coordinate`, `TimeZoneOffset`, `SummerTimeOffset`, `Comments`, `CountryCode`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_TimeZone=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_TimeZone is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_TimeZone);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_TimeZone* pRow = (Row_TimeZone*) (*i).second;	
		SingleLongKey key(pRow->m_PK_TimeZone);

		char tmp_PK_TimeZone[32];
sprintf(tmp_PK_TimeZone, "%li", key.pk);


string condition;
condition = condition + "`PK_TimeZone`=" + tmp_PK_TimeZone;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_TimeZone`="+pRow->PK_TimeZone_asSQL()+", `ZoneName`="+pRow->ZoneName_asSQL()+", `Coordinate`="+pRow->Coordinate_asSQL()+", `TimeZoneOffset`="+pRow->TimeZoneOffset_asSQL()+", `SummerTimeOffset`="+pRow->SummerTimeOffset_asSQL()+", `Comments`="+pRow->Comments_asSQL()+", `CountryCode`="+pRow->CountryCode_asSQL();

	
		string query = "update TimeZone set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_TimeZone* pRow = (Row_TimeZone*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_TimeZone* pRow = (Row_TimeZone*) (*i).second;	

		char tmp_PK_TimeZone[32];
sprintf(tmp_PK_TimeZone, "%li", key.pk);


string condition;
condition = condition + "`PK_TimeZone`=" + tmp_PK_TimeZone;

	
		string query = "delete from TimeZone where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_TimeZone*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_TimeZone::GetRows(string where_statement,vector<class Row_TimeZone*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `TimeZone`.* from TimeZone " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `TimeZone`.* from TimeZone where " + where_statement;
	else
		query = "select `TimeZone`.* from TimeZone";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_TimeZone *pRow = new Row_TimeZone(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_TimeZone = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_TimeZone));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_ZoneName = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_ZoneName = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Coordinate = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Coordinate = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_TimeZoneOffset = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%lf", &(pRow->m_TimeZoneOffset));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_SummerTimeOffset = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%lf", &(pRow->m_SummerTimeOffset));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Comments = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_CountryCode = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_CountryCode = string(row[6],lengths[6]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_TimeZone);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_TimeZone *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_TimeZone* Table_TimeZone::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_TimeZone *pRow = new Row_TimeZone(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_TimeZone* Table_TimeZone::GetRow(long int in_PK_TimeZone)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_TimeZone);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_TimeZone*) (*i).second;
	//we have to fetch row
	Row_TimeZone* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_TimeZone* Table_TimeZone::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_TimeZone[32];
sprintf(tmp_PK_TimeZone, "%li", key.pk);


string condition;
condition = condition + "`PK_TimeZone`=" + tmp_PK_TimeZone;


	string query = "select * from TimeZone where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TimeZone::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_TimeZone *pRow = new Row_TimeZone(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_TimeZone = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_TimeZone));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_ZoneName = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_ZoneName = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Coordinate = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Coordinate = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_TimeZoneOffset = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%lf", &(pRow->m_TimeZoneOffset));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_SummerTimeOffset = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%lf", &(pRow->m_SummerTimeOffset));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Comments = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_CountryCode = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_CountryCode = string(row[6],lengths[6]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}




void Row_TimeZone::City_FK_TimeZone_getrows(vector <class Row_City*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_City *pTable = table->database->City_get();
pTable->GetRows("`FK_TimeZone`=" + StringUtils::itos(m_PK_TimeZone),rows);
}



