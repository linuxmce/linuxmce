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
#include "Table_Language.h"
#include "Table_Language.h"

#include "Table_DesignObjVariation_DesignObjParameter.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_Text_Skin_Language.h"
#include "Table_Language.h"
#include "Table_Text_LS.h"
#include "Table_Text_LS_AltVersions.h"
#include "Table_Users.h"


void Database_pluto_main::CreateTable_Language()
{
	tblLanguage = new Table_Language(this);
}

void Database_pluto_main::DeleteTable_Language()
{
	if( tblLanguage )
		delete tblLanguage;
}

bool Database_pluto_main::Commit_Language(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblLanguage->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Language::~Table_Language()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Language *pRow = (Row_Language *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Language *pRow = (Row_Language *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Language::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Language *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Language *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Language);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Language::Reload()
{
	Row_Language *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Language);
		Row_Language *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Language::Row_Language(Table_Language *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Language::SetDefaultValues()
{
	m_PK_Language = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
m_FK_Language_TextPlacement = 0;
is_null[3] = true;
m_psc_id = 0;
is_null[4] = true;
m_psc_batch = 0;
is_null[5] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[6] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[7] = false;
is_null[8] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Language::PK_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Language;}
string Row_Language::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_Language::FK_Language_TextPlacement_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Language_TextPlacement;}
long int Row_Language::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Language::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Language::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Language::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Language::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Language::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Language::PK_Language_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Language = val; is_modified=true; is_null[0]=false;}
void Row_Language::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Language::FK_Language_TextPlacement_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Language_TextPlacement = val; is_modified=true; is_null[2]=false;}
void Row_Language::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_Language::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_Language::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_Language::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_Language::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}
void Row_Language::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[8]=false;}

		
bool Row_Language::FK_Language_TextPlacement_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Language::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Language::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Language::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Language::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Language::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}

			
void Row_Language::FK_Language_TextPlacement_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Language::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Language::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Language::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Language::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Language::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
	

string Row_Language::PK_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Language);

return buf;
}

string Row_Language::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[181];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)90,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Language::FK_Language_TextPlacement_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Language_TextPlacement);

return buf;
}

string Row_Language::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Language::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Language::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Language::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Language::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Language::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Language::Key::Key(long int in_PK_Language)
{
			pk_PK_Language = in_PK_Language;
	
}

Table_Language::Key::Key(Row_Language *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Language = pRow->m_PK_Language;
	
}		

bool Table_Language::Key_Less::operator()(const Table_Language::Key &key1, const Table_Language::Key &key2) const
{
			if (key1.pk_PK_Language!=key2.pk_PK_Language)
return key1.pk_PK_Language<key2.pk_PK_Language;
else
return false;	
}	

bool Table_Language::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Language *pRow = (Row_Language *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Language_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Language_TextPlacement_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Language (`PK_Language`, `Description`, `FK_Language_TextPlacement`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Language=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Language is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Language);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Language* pRow = (Row_Language*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Language);

		char tmp_PK_Language[32];
sprintf(tmp_PK_Language, "%li", key.pk);


string condition;
condition = condition + "`PK_Language`=" + tmp_PK_Language;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Language`="+pRow->PK_Language_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_Language_TextPlacement`="+pRow->FK_Language_TextPlacement_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Language set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Language* pRow = (Row_Language*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Language* pRow = (Row_Language*) (*i).second;	

		char tmp_PK_Language[32];
sprintf(tmp_PK_Language, "%li", key.pk);


string condition;
condition = condition + "`PK_Language`=" + tmp_PK_Language;

	
		string query = "delete from Language where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Language*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Language::GetRows(string where_statement,vector<class Row_Language*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Language`.* from Language " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Language`.* from Language where " + where_statement;
	else
		query = "select `Language`.* from Language";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Language *pRow = new Row_Language(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Language = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Language));
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
pRow->m_FK_Language_TextPlacement = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Language_TextPlacement));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Language);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Language *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Language* Table_Language::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Language *pRow = new Row_Language(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Language* Table_Language::GetRow(long int in_PK_Language)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Language);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Language*) (*i).second;
	//we have to fetch row
	Row_Language* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Language* Table_Language::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Language[32];
sprintf(tmp_PK_Language, "%li", key.pk);


string condition;
condition = condition + "`PK_Language`=" + tmp_PK_Language;


	string query = "select * from Language where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Language::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Language *pRow = new Row_Language(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Language = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Language));
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
pRow->m_FK_Language_TextPlacement = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Language_TextPlacement));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_psc_id));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_psc_batch));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_psc_user));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_psc_frozen));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_psc_mod = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Language* Row_Language::FK_Language_TextPlacement_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language_TextPlacement);
}


void Row_Language::DesignObjVariation_DesignObjParameter_FK_Language_getrows(vector <class Row_DesignObjVariation_DesignObjParameter*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObjParameter *pTable = table->database->DesignObjVariation_DesignObjParameter_get();
pTable->GetRows("`FK_Language`=" + StringUtils::itos(m_PK_Language),rows);
}
void Row_Language::DesignObjVariation_DesignObj_Skin_Language_FK_Language_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_Language`=" + StringUtils::itos(m_PK_Language),rows);
}
void Row_Language::DesignObjVariation_Text_Skin_Language_FK_Language_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_Text_Skin_Language *pTable = table->database->DesignObjVariation_Text_Skin_Language_get();
pTable->GetRows("`FK_Language`=" + StringUtils::itos(m_PK_Language),rows);
}
void Row_Language::Language_FK_Language_TextPlacement_getrows(vector <class Row_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Language *pTable = table->database->Language_get();
pTable->GetRows("`FK_Language_TextPlacement`=" + StringUtils::itos(m_PK_Language),rows);
}
void Row_Language::Text_LS_FK_Language_getrows(vector <class Row_Text_LS*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Text_LS *pTable = table->database->Text_LS_get();
pTable->GetRows("`FK_Language`=" + StringUtils::itos(m_PK_Language),rows);
}
void Row_Language::Text_LS_AltVersions_FK_Language_getrows(vector <class Row_Text_LS_AltVersions*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Text_LS_AltVersions *pTable = table->database->Text_LS_AltVersions_get();
pTable->GetRows("`FK_Language`=" + StringUtils::itos(m_PK_Language),rows);
}
void Row_Language::Users_FK_Language_getrows(vector <class Row_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Users *pTable = table->database->Users_get();
pTable->GetRows("`FK_Language`=" + StringUtils::itos(m_PK_Language),rows);
}



