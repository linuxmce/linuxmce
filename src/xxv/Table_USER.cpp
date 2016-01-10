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
#include "Table_USER.h"



void Database_xxv::CreateTable_USER()
{
	tblUSER = new Table_USER(this);
}

void Database_xxv::DeleteTable_USER()
{
	if( tblUSER )
		delete tblUSER;
}

bool Database_xxv::Commit_USER(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblUSER->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_USER::~Table_USER()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_USER *pRow = (Row_USER *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_USER *pRow = (Row_USER *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_USER::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_USER *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_USER *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_Id);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_USER::Reload()
{
	Row_USER *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_Id);
		Row_USER *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_USER::Row_USER(Table_USER *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_USER::SetDefaultValues()
{
	m_Id = 0;
is_null[0] = false;
m_Name = "";
is_null[1] = false;
m_Password = "";
is_null[2] = false;
m_Level = "";
is_null[3] = false;
m_Prefs = "";
is_null[4] = false;
m_UserPrefs = "";
is_null[5] = false;
is_null[6] = true;
m_MaxLifeTime = 0;
is_null[7] = false;
m_MaxPriority = 0;
is_null[8] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_USER::Id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Id;}
string Row_USER::Name_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Name;}
string Row_USER::Password_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password;}
string Row_USER::Level_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Level;}
string Row_USER::Prefs_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Prefs;}
string Row_USER::UserPrefs_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UserPrefs;}
string Row_USER::Deny_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Deny;}
short int Row_USER::MaxLifeTime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MaxLifeTime;}
short int Row_USER::MaxPriority_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MaxPriority;}

		
void Row_USER::Id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Id = val; is_modified=true; is_null[0]=false;}
void Row_USER::Name_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Name = val; is_modified=true; is_null[1]=false;}
void Row_USER::Password_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password = val; is_modified=true; is_null[2]=false;}
void Row_USER::Level_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Level = val; is_modified=true; is_null[3]=false;}
void Row_USER::Prefs_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Prefs = val; is_modified=true; is_null[4]=false;}
void Row_USER::UserPrefs_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UserPrefs = val; is_modified=true; is_null[5]=false;}
void Row_USER::Deny_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Deny = val; is_modified=true; is_null[6]=false;}
void Row_USER::MaxLifeTime_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MaxLifeTime = val; is_modified=true; is_null[7]=false;}
void Row_USER::MaxPriority_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MaxPriority = val; is_modified=true; is_null[8]=false;}

		
bool Row_USER::Prefs_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_USER::UserPrefs_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_USER::Deny_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_USER::MaxLifeTime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_USER::MaxPriority_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}

			
void Row_USER::Prefs_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_USER::UserPrefs_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_USER::Deny_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_USER::MaxLifeTime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_USER::MaxPriority_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
	

string Row_USER::Id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Id);

return buf;
}

string Row_USER::Name_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Name.c_str(), (unsigned long) min((size_t)100,m_Name.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_USER::Password_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Password.c_str(), (unsigned long) min((size_t)100,m_Password.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_USER::Level_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[33];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Level.c_str(), (unsigned long) min((size_t)16,m_Level.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_USER::Prefs_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Prefs.c_str(), (unsigned long) min((size_t)100,m_Prefs.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_USER::UserPrefs_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_UserPrefs.c_str(), (unsigned long) min((size_t)100,m_UserPrefs.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_USER::Deny_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[123];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Deny.c_str(), (unsigned long) min((size_t)61,m_Deny.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_USER::MaxLifeTime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MaxLifeTime);

return buf;
}

string Row_USER::MaxPriority_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MaxPriority);

return buf;
}




Table_USER::Key::Key(long int in_Id)
{
			pk_Id = in_Id;
	
}

Table_USER::Key::Key(Row_USER *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_Id = pRow->m_Id;
	
}		

bool Table_USER::Key_Less::operator()(const Table_USER::Key &key1, const Table_USER::Key &key2) const
{
			if (key1.pk_Id!=key2.pk_Id)
return key1.pk_Id<key2.pk_Id;
else
return false;	
}	

bool Table_USER::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_USER *pRow = (Row_USER *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->Id_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->Password_asSQL()+", "+pRow->Level_asSQL()+", "+pRow->Prefs_asSQL()+", "+pRow->UserPrefs_asSQL()+", "+pRow->Deny_asSQL()+", "+pRow->MaxLifeTime_asSQL()+", "+pRow->MaxPriority_asSQL();

	
		string query = "insert into USER (`Id`, `Name`, `Password`, `Level`, `Prefs`, `UserPrefs`, `Deny`, `MaxLifeTime`, `MaxPriority`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_Id=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Id is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_Id);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_USER* pRow = (Row_USER*) (*i).second;	
		SingleLongKey key(pRow->m_Id);

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`Id`="+pRow->Id_asSQL()+", `Name`="+pRow->Name_asSQL()+", `Password`="+pRow->Password_asSQL()+", `Level`="+pRow->Level_asSQL()+", `Prefs`="+pRow->Prefs_asSQL()+", `UserPrefs`="+pRow->UserPrefs_asSQL()+", `Deny`="+pRow->Deny_asSQL()+", `MaxLifeTime`="+pRow->MaxLifeTime_asSQL()+", `MaxPriority`="+pRow->MaxPriority_asSQL();

	
		string query = "update USER set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_USER* pRow = (Row_USER*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_USER* pRow = (Row_USER*) (*i).second;	

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;

	
		string query = "delete from USER where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_USER*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_USER::GetRows(string where_statement,vector<class Row_USER*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `USER`.* from USER " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `USER`.* from USER where " + where_statement;
	else
		query = "select `USER`.* from USER";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_USER *pRow = new Row_USER(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_Id = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_Id));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Name = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Password = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Password = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Level = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Level = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Prefs = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Prefs = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_UserPrefs = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_UserPrefs = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Deny = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Deny = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_MaxLifeTime = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_MaxLifeTime));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_MaxPriority = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_MaxPriority));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_Id);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_USER *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_USER* Table_USER::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_USER *pRow = new Row_USER(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_USER* Table_USER::GetRow(long int in_Id)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_Id);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_USER*) (*i).second;
	//we have to fetch row
	Row_USER* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_USER* Table_USER::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;


	string query = "select * from USER where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_USER::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_USER *pRow = new Row_USER(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_Id = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_Id));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Name = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Name = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Password = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Password = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Level = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Level = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Prefs = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Prefs = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_UserPrefs = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_UserPrefs = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Deny = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Deny = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_MaxLifeTime = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_MaxLifeTime));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_MaxPriority = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_MaxPriority));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







