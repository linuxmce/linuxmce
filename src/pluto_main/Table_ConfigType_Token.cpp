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
#include "Table_ConfigType_Token.h"
#include "Table_ConfigType_Setting.h"
#include "Table_ConfigType_File.h"



void Database_pluto_main::CreateTable_ConfigType_Token()
{
	tblConfigType_Token = new Table_ConfigType_Token(this);
}

void Database_pluto_main::DeleteTable_ConfigType_Token()
{
	if( tblConfigType_Token )
		delete tblConfigType_Token;
}

bool Database_pluto_main::Commit_ConfigType_Token(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblConfigType_Token->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_ConfigType_Token::~Table_ConfigType_Token()
{
	map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_ConfigType_Token *pRow = (Row_ConfigType_Token *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_ConfigType_Token *pRow = (Row_ConfigType_Token *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_ConfigType_Token::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_ConfigType_Token *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_ConfigType_Token *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			Table_ConfigType_Token::Key key(pRow->m_FK_ConfigType_Setting,pRow->m_FK_ConfigType_File,pRow->m_FindStr);
			map<Table_ConfigType_Token::Key, TableRow*, Table_ConfigType_Token::Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_ConfigType_Token::Reload()
{
	Row_ConfigType_Token *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		Table_ConfigType_Token::Key key(pRow->m_FK_ConfigType_Setting,pRow->m_FK_ConfigType_File,pRow->m_FindStr);
		Row_ConfigType_Token *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_ConfigType_Token::Row_ConfigType_Token(Table_ConfigType_Token *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_ConfigType_Token::SetDefaultValues()
{
	m_FK_ConfigType_Setting = 0;
is_null[0] = false;
m_FK_ConfigType_File = 0;
is_null[1] = false;
m_FindStr = "";
is_null[2] = false;
m_ReplaceStr = "";
is_null[3] = false;
is_null[4] = true;
m_psc_id = 0;
is_null[5] = true;
m_psc_batch = 0;
is_null[6] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[7] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[8] = false;
is_null[9] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_ConfigType_Token::FK_ConfigType_Setting_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_ConfigType_Setting;}
long int Row_ConfigType_Token::FK_ConfigType_File_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_ConfigType_File;}
string Row_ConfigType_Token::FindStr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FindStr;}
string Row_ConfigType_Token::ReplaceStr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ReplaceStr;}
long int Row_ConfigType_Token::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_ConfigType_Token::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_ConfigType_Token::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_ConfigType_Token::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_ConfigType_Token::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_ConfigType_Token::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_ConfigType_Token::FK_ConfigType_Setting_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_ConfigType_Setting = val; is_modified=true; is_null[0]=false;}
void Row_ConfigType_Token::FK_ConfigType_File_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_ConfigType_File = val; is_modified=true; is_null[1]=false;}
void Row_ConfigType_Token::FindStr_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FindStr = val; is_modified=true; is_null[2]=false;}
void Row_ConfigType_Token::ReplaceStr_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ReplaceStr = val; is_modified=true; is_null[3]=false;}
void Row_ConfigType_Token::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[4]=false;}
void Row_ConfigType_Token::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[5]=false;}
void Row_ConfigType_Token::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[6]=false;}
void Row_ConfigType_Token::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[7]=false;}
void Row_ConfigType_Token::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[8]=false;}
void Row_ConfigType_Token::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[9]=false;}

		
bool Row_ConfigType_Token::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_ConfigType_Token::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_ConfigType_Token::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_ConfigType_Token::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_ConfigType_Token::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}

			
void Row_ConfigType_Token::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_ConfigType_Token::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_ConfigType_Token::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_ConfigType_Token::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_ConfigType_Token::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
	

string Row_ConfigType_Token::FK_ConfigType_Setting_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_ConfigType_Setting);

return buf;
}

string Row_ConfigType_Token::FK_ConfigType_File_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_ConfigType_File);

return buf;
}

string Row_ConfigType_Token::FindStr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_FindStr.c_str(), (unsigned long) min((size_t)30,m_FindStr.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ConfigType_Token::ReplaceStr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ReplaceStr.c_str(), (unsigned long) min((size_t)65535,m_ReplaceStr.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ConfigType_Token::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_ConfigType_Token::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_ConfigType_Token::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_ConfigType_Token::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_ConfigType_Token::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ConfigType_Token::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_ConfigType_Token::Key::Key(long int in_FK_ConfigType_Setting, long int in_FK_ConfigType_File, string in_FindStr)
{
			pk_FK_ConfigType_Setting = in_FK_ConfigType_Setting;
pk_FK_ConfigType_File = in_FK_ConfigType_File;
pk_FindStr = in_FindStr;
	
}

Table_ConfigType_Token::Key::Key(Row_ConfigType_Token *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_FK_ConfigType_Setting = pRow->m_FK_ConfigType_Setting;
pk_FK_ConfigType_File = pRow->m_FK_ConfigType_File;
pk_FindStr = pRow->m_FindStr;
	
}		

bool Table_ConfigType_Token::Key_Less::operator()(const Table_ConfigType_Token::Key &key1, const Table_ConfigType_Token::Key &key2) const
{
			if (key1.pk_FK_ConfigType_Setting!=key2.pk_FK_ConfigType_Setting)
return key1.pk_FK_ConfigType_Setting<key2.pk_FK_ConfigType_Setting;
else
if (key1.pk_FK_ConfigType_File!=key2.pk_FK_ConfigType_File)
return key1.pk_FK_ConfigType_File<key2.pk_FK_ConfigType_File;
else
if (key1.pk_FindStr!=key2.pk_FindStr)
return key1.pk_FindStr<key2.pk_FindStr;
else
return false;	
}	

bool Table_ConfigType_Token::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_ConfigType_Token *pRow = (Row_ConfigType_Token *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_ConfigType_Setting_asSQL()+", "+pRow->FK_ConfigType_File_asSQL()+", "+pRow->FindStr_asSQL()+", "+pRow->ReplaceStr_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into ConfigType_Token (`FK_ConfigType_Setting`, `FK_ConfigType_File`, `FindStr`, `ReplaceStr`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			Table_ConfigType_Token::Key key(pRow->m_FK_ConfigType_Setting,pRow->m_FK_ConfigType_File,pRow->m_FindStr);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_ConfigType_Token* pRow = (Row_ConfigType_Token*) (*i).second;	
		Table_ConfigType_Token::Key key(pRow->m_FK_ConfigType_Setting,pRow->m_FK_ConfigType_File,pRow->m_FindStr);

		char tmp_FK_ConfigType_Setting[32];
sprintf(tmp_FK_ConfigType_Setting, "%li", key.pk_FK_ConfigType_Setting);

char tmp_FK_ConfigType_File[32];
sprintf(tmp_FK_ConfigType_File, "%li", key.pk_FK_ConfigType_File);

char tmp_FindStr[61];
db_wrapper_real_escape_string(database->m_pDB,tmp_FindStr, key.pk_FindStr.c_str(), (unsigned long) key.pk_FindStr.size());


string condition;
condition = condition + "`FK_ConfigType_Setting`=" + tmp_FK_ConfigType_Setting+" AND "+"`FK_ConfigType_File`=" + tmp_FK_ConfigType_File+" AND "+"`FindStr`=" + "\"" + tmp_FindStr+ "\"";
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_ConfigType_Setting`="+pRow->FK_ConfigType_Setting_asSQL()+", `FK_ConfigType_File`="+pRow->FK_ConfigType_File_asSQL()+", `FindStr`="+pRow->FindStr_asSQL()+", `ReplaceStr`="+pRow->ReplaceStr_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update ConfigType_Token set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_ConfigType_Token* pRow = (Row_ConfigType_Token*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less>::iterator i = deleted_cachedRows.begin();
	
		Table_ConfigType_Token::Key key = (*i).first;
		Row_ConfigType_Token* pRow = (Row_ConfigType_Token*) (*i).second;	

		char tmp_FK_ConfigType_Setting[32];
sprintf(tmp_FK_ConfigType_Setting, "%li", key.pk_FK_ConfigType_Setting);

char tmp_FK_ConfigType_File[32];
sprintf(tmp_FK_ConfigType_File, "%li", key.pk_FK_ConfigType_File);

char tmp_FindStr[61];
db_wrapper_real_escape_string(database->m_pDB,tmp_FindStr, key.pk_FindStr.c_str(), (unsigned long) key.pk_FindStr.size());


string condition;
condition = condition + "`FK_ConfigType_Setting`=" + tmp_FK_ConfigType_Setting+" AND "+"`FK_ConfigType_File`=" + tmp_FK_ConfigType_File+" AND "+"`FindStr`=" + "\"" + tmp_FindStr+ "\"";

	
		string query = "delete from ConfigType_Token where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_ConfigType_Token*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_ConfigType_Token::GetRows(string where_statement,vector<class Row_ConfigType_Token*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `ConfigType_Token`.* from ConfigType_Token " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `ConfigType_Token`.* from ConfigType_Token where " + where_statement;
	else
		query = "select `ConfigType_Token`.* from ConfigType_Token";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_ConfigType_Token *pRow = new Row_ConfigType_Token(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_ConfigType_Setting = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_ConfigType_Setting));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_ConfigType_File = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_ConfigType_File));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FindStr = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_FindStr = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_ReplaceStr = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_ReplaceStr = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_id));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_batch));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_user));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_psc_frozen));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_psc_mod = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		Table_ConfigType_Token::Key key(pRow->m_FK_ConfigType_Setting,pRow->m_FK_ConfigType_File,pRow->m_FindStr);
		
		map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_ConfigType_Token *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_ConfigType_Token* Table_ConfigType_Token::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_ConfigType_Token *pRow = new Row_ConfigType_Token(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_ConfigType_Token* Table_ConfigType_Token::GetRow(long int in_FK_ConfigType_Setting, long int in_FK_ConfigType_File, string in_FindStr)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Table_ConfigType_Token::Key row_key(in_FK_ConfigType_Setting, in_FK_ConfigType_File, in_FindStr);

	map<Table_ConfigType_Token::Key, class TableRow*, Table_ConfigType_Token::Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_ConfigType_Token*) (*i).second;
	//we have to fetch row
	Row_ConfigType_Token* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_ConfigType_Token* Table_ConfigType_Token::FetchRow(Table_ConfigType_Token::Key &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_FK_ConfigType_Setting[32];
sprintf(tmp_FK_ConfigType_Setting, "%li", key.pk_FK_ConfigType_Setting);

char tmp_FK_ConfigType_File[32];
sprintf(tmp_FK_ConfigType_File, "%li", key.pk_FK_ConfigType_File);

char tmp_FindStr[61];
db_wrapper_real_escape_string(database->m_pDB,tmp_FindStr, key.pk_FindStr.c_str(), (unsigned long) key.pk_FindStr.size());


string condition;
condition = condition + "`FK_ConfigType_Setting`=" + tmp_FK_ConfigType_Setting+" AND "+"`FK_ConfigType_File`=" + tmp_FK_ConfigType_File+" AND "+"`FindStr`=" + "\"" + tmp_FindStr+ "\"";


	string query = "select * from ConfigType_Token where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ConfigType_Token::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_ConfigType_Token *pRow = new Row_ConfigType_Token(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_ConfigType_Setting = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_ConfigType_Setting));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_ConfigType_File = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_ConfigType_File));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FindStr = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_FindStr = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_ReplaceStr = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_ReplaceStr = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_id));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_batch));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_psc_user));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_psc_frozen));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_psc_mod = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_ConfigType_Setting* Row_ConfigType_Token::FK_ConfigType_Setting_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_ConfigType_Setting *pTable = table->database->ConfigType_Setting_get();
return pTable->GetRow(m_FK_ConfigType_Setting);
}
class Row_ConfigType_File* Row_ConfigType_Token::FK_ConfigType_File_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_ConfigType_File *pTable = table->database->ConfigType_File_get();
return pTable->GetRow(m_FK_ConfigType_File);
}





