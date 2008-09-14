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
#include "Table_City.h"
#include "Table_Country.h"
#include "Table_Region.h"
#include "Table_TimeZone.h"

#include "Table_Installation.h"
#include "Table_PostalCode.h"


void Database_pluto_main::CreateTable_City()
{
	tblCity = new Table_City(this);
}

void Database_pluto_main::DeleteTable_City()
{
	if( tblCity )
		delete tblCity;
}

bool Database_pluto_main::Commit_City(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblCity->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_City::~Table_City()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_City *pRow = (Row_City *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_City *pRow = (Row_City *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_City::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_City *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_City *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_City);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_City::Reload()
{
	Row_City *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_City);
		Row_City *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_City::Row_City(Table_City *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_City::SetDefaultValues()
{
	m_PK_City = 0;
is_null[0] = false;
m_FK_Country = 0;
is_null[1] = false;
is_null[2] = true;
m_FK_Region = 0;
is_null[3] = true;
is_null[4] = true;
m_Latitude = 0;
is_null[5] = true;
m_Longitude = 0;
is_null[6] = true;
m_DmaId = 0;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
m_FK_TimeZone = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_City::PK_City_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_City;}
long int Row_City::FK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Country;}
long int Row_City::FK_Region_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Region;}
string Row_City::City_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_City;}
float Row_City::Latitude_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Latitude;}
float Row_City::Longitude_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Longitude;}
short int Row_City::DmaId_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DmaId;}
string Row_City::County_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_County;}
string Row_City::Code_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Code;}
long int Row_City::FK_TimeZone_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_TimeZone;}

		
void Row_City::PK_City_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_City = val; is_modified=true; is_null[0]=false;}
void Row_City::FK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Country = val; is_modified=true; is_null[1]=false;}
void Row_City::FK_Region_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Region = val; is_modified=true; is_null[2]=false;}
void Row_City::City_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_City = val; is_modified=true; is_null[3]=false;}
void Row_City::Latitude_set(float val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Latitude = val; is_modified=true; is_null[4]=false;}
void Row_City::Longitude_set(float val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Longitude = val; is_modified=true; is_null[5]=false;}
void Row_City::DmaId_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DmaId = val; is_modified=true; is_null[6]=false;}
void Row_City::County_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_County = val; is_modified=true; is_null[7]=false;}
void Row_City::Code_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Code = val; is_modified=true; is_null[8]=false;}
void Row_City::FK_TimeZone_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_TimeZone = val; is_modified=true; is_null[9]=false;}

		
bool Row_City::FK_Region_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_City::City_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_City::Latitude_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_City::Longitude_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_City::DmaId_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_City::County_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_City::Code_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_City::FK_TimeZone_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}

			
void Row_City::FK_Region_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_City::City_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_City::Latitude_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_City::Longitude_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_City::DmaId_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_City::County_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_City::Code_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_City::FK_TimeZone_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
	

string Row_City::PK_City_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_City);

return buf;
}

string Row_City::FK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Country);

return buf;
}

string Row_City::FK_Region_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Region);

return buf;
}

string Row_City::City_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[91];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_City.c_str(), (unsigned long) min((size_t)45,m_City.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_City::Latitude_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%f", m_Latitude);

return buf;
}

string Row_City::Longitude_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%f", m_Longitude);

return buf;
}

string Row_City::DmaId_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DmaId);

return buf;
}

string Row_City::County_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[51];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_County.c_str(), (unsigned long) min((size_t)25,m_County.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_City::Code_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[9];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Code.c_str(), (unsigned long) min((size_t)4,m_Code.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_City::FK_TimeZone_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_TimeZone);

return buf;
}




Table_City::Key::Key(long int in_PK_City)
{
			pk_PK_City = in_PK_City;
	
}

Table_City::Key::Key(Row_City *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_City = pRow->m_PK_City;
	
}		

bool Table_City::Key_Less::operator()(const Table_City::Key &key1, const Table_City::Key &key2) const
{
			if (key1.pk_PK_City!=key2.pk_PK_City)
return key1.pk_PK_City<key2.pk_PK_City;
else
return false;	
}	

bool Table_City::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_City *pRow = (Row_City *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_City_asSQL()+", "+pRow->FK_Country_asSQL()+", "+pRow->FK_Region_asSQL()+", "+pRow->City_asSQL()+", "+pRow->Latitude_asSQL()+", "+pRow->Longitude_asSQL()+", "+pRow->DmaId_asSQL()+", "+pRow->County_asSQL()+", "+pRow->Code_asSQL()+", "+pRow->FK_TimeZone_asSQL();

	
		string query = "insert into City (`PK_City`, `FK_Country`, `FK_Region`, `City`, `Latitude`, `Longitude`, `DmaId`, `County`, `Code`, `FK_TimeZone`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_City=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_City is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_City);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_City* pRow = (Row_City*) (*i).second;	
		SingleLongKey key(pRow->m_PK_City);

		char tmp_PK_City[32];
sprintf(tmp_PK_City, "%li", key.pk);


string condition;
condition = condition + "`PK_City`=" + tmp_PK_City;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_City`="+pRow->PK_City_asSQL()+", `FK_Country`="+pRow->FK_Country_asSQL()+", `FK_Region`="+pRow->FK_Region_asSQL()+", `City`="+pRow->City_asSQL()+", `Latitude`="+pRow->Latitude_asSQL()+", `Longitude`="+pRow->Longitude_asSQL()+", `DmaId`="+pRow->DmaId_asSQL()+", `County`="+pRow->County_asSQL()+", `Code`="+pRow->Code_asSQL()+", `FK_TimeZone`="+pRow->FK_TimeZone_asSQL();

	
		string query = "update City set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_City* pRow = (Row_City*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_City* pRow = (Row_City*) (*i).second;	

		char tmp_PK_City[32];
sprintf(tmp_PK_City, "%li", key.pk);


string condition;
condition = condition + "`PK_City`=" + tmp_PK_City;

	
		string query = "delete from City where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_City*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_City::GetRows(string where_statement,vector<class Row_City*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `City`.* from City " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `City`.* from City where " + where_statement;
	else
		query = "select `City`.* from City";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_City *pRow = new Row_City(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_City = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_City));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Country));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Region = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Region));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_City = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_City = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Latitude = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%f", &(pRow->m_Latitude));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Longitude = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%f", &(pRow->m_Longitude));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_DmaId = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_DmaId));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_County = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_County = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Code = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Code = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_TimeZone = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_TimeZone));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_City);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_City *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_City* Table_City::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_City *pRow = new Row_City(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_City* Table_City::GetRow(long int in_PK_City)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_City);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_City*) (*i).second;
	//we have to fetch row
	Row_City* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_City* Table_City::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_City[32];
sprintf(tmp_PK_City, "%li", key.pk);


string condition;
condition = condition + "`PK_City`=" + tmp_PK_City;


	string query = "select * from City where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_City::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_City *pRow = new Row_City(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_City = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_City));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Country));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Region = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Region));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_City = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_City = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Latitude = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%f", &(pRow->m_Latitude));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Longitude = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%f", &(pRow->m_Longitude));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_DmaId = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_DmaId));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_County = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_County = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Code = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Code = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_TimeZone = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_TimeZone));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Country* Row_City::FK_Country_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Country *pTable = table->database->Country_get();
return pTable->GetRow(m_FK_Country);
}
class Row_Region* Row_City::FK_Region_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Region *pTable = table->database->Region_get();
return pTable->GetRow(m_FK_Region);
}
class Row_TimeZone* Row_City::FK_TimeZone_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_TimeZone *pTable = table->database->TimeZone_get();
return pTable->GetRow(m_FK_TimeZone);
}


void Row_City::Installation_FK_City_getrows(vector <class Row_Installation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation *pTable = table->database->Installation_get();
pTable->GetRows("`FK_City`=" + StringUtils::itos(m_PK_City),rows);
}
void Row_City::PostalCode_FK_City_getrows(vector <class Row_PostalCode*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PostalCode *pTable = table->database->PostalCode_get();
pTable->GetRows("`FK_City`=" + StringUtils::itos(m_PK_City),rows);
}



