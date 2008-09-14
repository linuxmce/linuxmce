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
#include "Table_PostalCode.h"
#include "Table_City.h"
#include "Table_Country.h"

#include "Table_Installation.h"


void Database_pluto_main::CreateTable_PostalCode()
{
	tblPostalCode = new Table_PostalCode(this);
}

void Database_pluto_main::DeleteTable_PostalCode()
{
	if( tblPostalCode )
		delete tblPostalCode;
}

bool Database_pluto_main::Commit_PostalCode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblPostalCode->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_PostalCode::~Table_PostalCode()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_PostalCode *pRow = (Row_PostalCode *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_PostalCode *pRow = (Row_PostalCode *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_PostalCode::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_PostalCode *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_PostalCode *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_PostalCode);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_PostalCode::Reload()
{
	Row_PostalCode *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_PostalCode);
		Row_PostalCode *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_PostalCode::Row_PostalCode(Table_PostalCode *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_PostalCode::SetDefaultValues()
{
	m_PK_PostalCode = 0;
is_null[0] = false;
m_PostalCode = "";
is_null[1] = false;
m_Long = "";
is_null[2] = false;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_FK_City = 0;
m_FK_Country = 0;
is_null[8] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_PostalCode::PK_PostalCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_PostalCode;}
string Row_PostalCode::PostalCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PostalCode;}
string Row_PostalCode::Long_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Long;}
string Row_PostalCode::Lat_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Lat;}
string Row_PostalCode::City_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_City;}
string Row_PostalCode::State_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_State;}
string Row_PostalCode::County_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_County;}
long int Row_PostalCode::FK_City_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_City;}
long int Row_PostalCode::FK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Country;}

		
void Row_PostalCode::PK_PostalCode_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_PostalCode = val; is_modified=true; is_null[0]=false;}
void Row_PostalCode::PostalCode_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PostalCode = val; is_modified=true; is_null[1]=false;}
void Row_PostalCode::Long_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Long = val; is_modified=true; is_null[2]=false;}
void Row_PostalCode::Lat_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Lat = val; is_modified=true; is_null[3]=false;}
void Row_PostalCode::City_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_City = val; is_modified=true; is_null[4]=false;}
void Row_PostalCode::State_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_State = val; is_modified=true; is_null[5]=false;}
void Row_PostalCode::County_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_County = val; is_modified=true; is_null[6]=false;}
void Row_PostalCode::FK_City_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_City = val; is_modified=true; is_null[7]=false;}
void Row_PostalCode::FK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Country = val; is_modified=true; is_null[8]=false;}

		
bool Row_PostalCode::Lat_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_PostalCode::City_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_PostalCode::State_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_PostalCode::County_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_PostalCode::FK_City_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}

			
void Row_PostalCode::Lat_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_PostalCode::City_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_PostalCode::State_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_PostalCode::County_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_PostalCode::FK_City_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
	

string Row_PostalCode::PK_PostalCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_PostalCode);

return buf;
}

string Row_PostalCode::PostalCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_PostalCode.c_str(), (unsigned long) min((size_t)10,m_PostalCode.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PostalCode::Long_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Long.c_str(), (unsigned long) min((size_t)10,m_Long.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PostalCode::Lat_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Lat.c_str(), (unsigned long) min((size_t)10,m_Lat.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PostalCode::City_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_City.c_str(), (unsigned long) min((size_t)60,m_City.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PostalCode::State_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[5];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_State.c_str(), (unsigned long) min((size_t)2,m_State.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PostalCode::County_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_County.c_str(), (unsigned long) min((size_t)60,m_County.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_PostalCode::FK_City_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_City);

return buf;
}

string Row_PostalCode::FK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Country);

return buf;
}




Table_PostalCode::Key::Key(long int in_PK_PostalCode)
{
			pk_PK_PostalCode = in_PK_PostalCode;
	
}

Table_PostalCode::Key::Key(Row_PostalCode *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_PostalCode = pRow->m_PK_PostalCode;
	
}		

bool Table_PostalCode::Key_Less::operator()(const Table_PostalCode::Key &key1, const Table_PostalCode::Key &key2) const
{
			if (key1.pk_PK_PostalCode!=key2.pk_PK_PostalCode)
return key1.pk_PK_PostalCode<key2.pk_PK_PostalCode;
else
return false;	
}	

bool Table_PostalCode::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_PostalCode *pRow = (Row_PostalCode *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_PostalCode_asSQL()+", "+pRow->PostalCode_asSQL()+", "+pRow->Long_asSQL()+", "+pRow->Lat_asSQL()+", "+pRow->City_asSQL()+", "+pRow->State_asSQL()+", "+pRow->County_asSQL()+", "+pRow->FK_City_asSQL()+", "+pRow->FK_Country_asSQL();

	
		string query = "insert into PostalCode (`PK_PostalCode`, `PostalCode`, `Long`, `Lat`, `City`, `State`, `County`, `FK_City`, `FK_Country`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_PostalCode=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_PostalCode is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_PostalCode);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_PostalCode* pRow = (Row_PostalCode*) (*i).second;	
		SingleLongKey key(pRow->m_PK_PostalCode);

		char tmp_PK_PostalCode[32];
sprintf(tmp_PK_PostalCode, "%li", key.pk);


string condition;
condition = condition + "`PK_PostalCode`=" + tmp_PK_PostalCode;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_PostalCode`="+pRow->PK_PostalCode_asSQL()+", `PostalCode`="+pRow->PostalCode_asSQL()+", `Long`="+pRow->Long_asSQL()+", `Lat`="+pRow->Lat_asSQL()+", `City`="+pRow->City_asSQL()+", `State`="+pRow->State_asSQL()+", `County`="+pRow->County_asSQL()+", `FK_City`="+pRow->FK_City_asSQL()+", `FK_Country`="+pRow->FK_Country_asSQL();

	
		string query = "update PostalCode set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_PostalCode* pRow = (Row_PostalCode*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_PostalCode* pRow = (Row_PostalCode*) (*i).second;	

		char tmp_PK_PostalCode[32];
sprintf(tmp_PK_PostalCode, "%li", key.pk);


string condition;
condition = condition + "`PK_PostalCode`=" + tmp_PK_PostalCode;

	
		string query = "delete from PostalCode where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_PostalCode*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_PostalCode::GetRows(string where_statement,vector<class Row_PostalCode*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `PostalCode`.* from PostalCode " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `PostalCode`.* from PostalCode where " + where_statement;
	else
		query = "select `PostalCode`.* from PostalCode";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_PostalCode *pRow = new Row_PostalCode(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PostalCode = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PostalCode));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_PostalCode = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_PostalCode = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Long = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Long = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Lat = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Lat = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_City = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_City = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_State = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_County = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_County = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_City = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_City));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Country));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_PostalCode);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_PostalCode *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_PostalCode* Table_PostalCode::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_PostalCode *pRow = new Row_PostalCode(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_PostalCode* Table_PostalCode::GetRow(long int in_PK_PostalCode)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_PostalCode);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_PostalCode*) (*i).second;
	//we have to fetch row
	Row_PostalCode* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_PostalCode* Table_PostalCode::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_PostalCode[32];
sprintf(tmp_PK_PostalCode, "%li", key.pk);


string condition;
condition = condition + "`PK_PostalCode`=" + tmp_PK_PostalCode;


	string query = "select * from PostalCode where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_PostalCode::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_PostalCode *pRow = new Row_PostalCode(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_PostalCode = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_PostalCode));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_PostalCode = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_PostalCode = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Long = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Long = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Lat = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Lat = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_City = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_City = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_State = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_County = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_County = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_City = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_City));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Country));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_City* Row_PostalCode::FK_City_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_City *pTable = table->database->City_get();
return pTable->GetRow(m_FK_City);
}
class Row_Country* Row_PostalCode::FK_Country_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Country *pTable = table->database->Country_get();
return pTable->GetRow(m_FK_Country);
}


void Row_PostalCode::Installation_FK_PostalCode_getrows(vector <class Row_Installation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation *pTable = table->database->Installation_get();
pTable->GetRows("`FK_PostalCode`=" + StringUtils::itos(m_PK_PostalCode),rows);
}



