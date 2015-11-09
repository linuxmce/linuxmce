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
#include "Table_EffectType_Effect_Skin.h"
#include "Table_EffectType.h"
#include "Table_Effect.h"
#include "Table_Skin.h"



void Database_pluto_main::CreateTable_EffectType_Effect_Skin()
{
	tblEffectType_Effect_Skin = new Table_EffectType_Effect_Skin(this);
}

void Database_pluto_main::DeleteTable_EffectType_Effect_Skin()
{
	if( tblEffectType_Effect_Skin )
		delete tblEffectType_Effect_Skin;
}

bool Database_pluto_main::Commit_EffectType_Effect_Skin(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblEffectType_Effect_Skin->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_EffectType_Effect_Skin::~Table_EffectType_Effect_Skin()
{
	map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_EffectType_Effect_Skin *pRow = (Row_EffectType_Effect_Skin *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_EffectType_Effect_Skin *pRow = (Row_EffectType_Effect_Skin *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_EffectType_Effect_Skin::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_EffectType_Effect_Skin *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_EffectType_Effect_Skin *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			TripleLongKey key(pRow->m_FK_EffectType,pRow->m_FK_Effect,pRow->m_FK_Skin);
			map<TripleLongKey, TableRow*, TripleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_EffectType_Effect_Skin::Reload()
{
	Row_EffectType_Effect_Skin *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		TripleLongKey key(pRow->m_FK_EffectType,pRow->m_FK_Effect,pRow->m_FK_Skin);
		Row_EffectType_Effect_Skin *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_EffectType_Effect_Skin::Row_EffectType_Effect_Skin(Table_EffectType_Effect_Skin *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_EffectType_Effect_Skin::SetDefaultValues()
{
	m_FK_EffectType = 0;
is_null[0] = false;
m_FK_Effect = 0;
is_null[1] = false;
m_FK_Skin = 0;
is_null[2] = false;
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

long int Row_EffectType_Effect_Skin::FK_EffectType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_EffectType;}
long int Row_EffectType_Effect_Skin::FK_Effect_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Effect;}
long int Row_EffectType_Effect_Skin::FK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin;}
long int Row_EffectType_Effect_Skin::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_EffectType_Effect_Skin::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_EffectType_Effect_Skin::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_EffectType_Effect_Skin::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_EffectType_Effect_Skin::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_EffectType_Effect_Skin::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_EffectType_Effect_Skin::FK_EffectType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_EffectType = val; is_modified=true; is_null[0]=false;}
void Row_EffectType_Effect_Skin::FK_Effect_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Effect = val; is_modified=true; is_null[1]=false;}
void Row_EffectType_Effect_Skin::FK_Skin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin = val; is_modified=true; is_null[2]=false;}
void Row_EffectType_Effect_Skin::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[3]=false;}
void Row_EffectType_Effect_Skin::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[4]=false;}
void Row_EffectType_Effect_Skin::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[5]=false;}
void Row_EffectType_Effect_Skin::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[6]=false;}
void Row_EffectType_Effect_Skin::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[7]=false;}
void Row_EffectType_Effect_Skin::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[8]=false;}

		
bool Row_EffectType_Effect_Skin::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_EffectType_Effect_Skin::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_EffectType_Effect_Skin::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_EffectType_Effect_Skin::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_EffectType_Effect_Skin::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}

			
void Row_EffectType_Effect_Skin::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_EffectType_Effect_Skin::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_EffectType_Effect_Skin::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_EffectType_Effect_Skin::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_EffectType_Effect_Skin::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
	

string Row_EffectType_Effect_Skin::FK_EffectType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_EffectType);

return buf;
}

string Row_EffectType_Effect_Skin::FK_Effect_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Effect);

return buf;
}

string Row_EffectType_Effect_Skin::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin);

return buf;
}

string Row_EffectType_Effect_Skin::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_EffectType_Effect_Skin::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_EffectType_Effect_Skin::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_EffectType_Effect_Skin::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_EffectType_Effect_Skin::psc_mod_asSQL()
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

string Row_EffectType_Effect_Skin::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_EffectType_Effect_Skin::Key::Key(long int in_FK_EffectType, long int in_FK_Effect, long int in_FK_Skin)
{
			pk_FK_EffectType = in_FK_EffectType;
pk_FK_Effect = in_FK_Effect;
pk_FK_Skin = in_FK_Skin;
	
}

Table_EffectType_Effect_Skin::Key::Key(Row_EffectType_Effect_Skin *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_FK_EffectType = pRow->m_FK_EffectType;
pk_FK_Effect = pRow->m_FK_Effect;
pk_FK_Skin = pRow->m_FK_Skin;
	
}		

bool Table_EffectType_Effect_Skin::Key_Less::operator()(const Table_EffectType_Effect_Skin::Key &key1, const Table_EffectType_Effect_Skin::Key &key2) const
{
			if (key1.pk_FK_EffectType!=key2.pk_FK_EffectType)
return key1.pk_FK_EffectType<key2.pk_FK_EffectType;
else
if (key1.pk_FK_Effect!=key2.pk_FK_Effect)
return key1.pk_FK_Effect<key2.pk_FK_Effect;
else
if (key1.pk_FK_Skin!=key2.pk_FK_Skin)
return key1.pk_FK_Skin<key2.pk_FK_Skin;
else
return false;	
}	

bool Table_EffectType_Effect_Skin::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_EffectType_Effect_Skin *pRow = (Row_EffectType_Effect_Skin *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_EffectType_asSQL()+", "+pRow->FK_Effect_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into EffectType_Effect_Skin (`FK_EffectType`, `FK_Effect`, `FK_Skin`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			TripleLongKey key(pRow->m_FK_EffectType,pRow->m_FK_Effect,pRow->m_FK_Skin);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_EffectType_Effect_Skin* pRow = (Row_EffectType_Effect_Skin*) (*i).second;	
		TripleLongKey key(pRow->m_FK_EffectType,pRow->m_FK_Effect,pRow->m_FK_Skin);

		char tmp_FK_EffectType[32];
sprintf(tmp_FK_EffectType, "%li", key.pk1);

char tmp_FK_Effect[32];
sprintf(tmp_FK_Effect, "%li", key.pk2);

char tmp_FK_Skin[32];
sprintf(tmp_FK_Skin, "%li", key.pk3);


string condition;
condition = condition + "`FK_EffectType`=" + tmp_FK_EffectType+" AND "+"`FK_Effect`=" + tmp_FK_Effect+" AND "+"`FK_Skin`=" + tmp_FK_Skin;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_EffectType`="+pRow->FK_EffectType_asSQL()+", `FK_Effect`="+pRow->FK_Effect_asSQL()+", `FK_Skin`="+pRow->FK_Skin_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update EffectType_Effect_Skin set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_EffectType_Effect_Skin* pRow = (Row_EffectType_Effect_Skin*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		TripleLongKey key = (*i).first;
		Row_EffectType_Effect_Skin* pRow = (Row_EffectType_Effect_Skin*) (*i).second;	

		char tmp_FK_EffectType[32];
sprintf(tmp_FK_EffectType, "%li", key.pk1);

char tmp_FK_Effect[32];
sprintf(tmp_FK_Effect, "%li", key.pk2);

char tmp_FK_Skin[32];
sprintf(tmp_FK_Skin, "%li", key.pk3);


string condition;
condition = condition + "`FK_EffectType`=" + tmp_FK_EffectType+" AND "+"`FK_Effect`=" + tmp_FK_Effect+" AND "+"`FK_Skin`=" + tmp_FK_Skin;

	
		string query = "delete from EffectType_Effect_Skin where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_EffectType_Effect_Skin*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_EffectType_Effect_Skin::GetRows(string where_statement,vector<class Row_EffectType_Effect_Skin*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `EffectType_Effect_Skin`.* from EffectType_Effect_Skin " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `EffectType_Effect_Skin`.* from EffectType_Effect_Skin where " + where_statement;
	else
		query = "select `EffectType_Effect_Skin`.* from EffectType_Effect_Skin";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_EffectType_Effect_Skin *pRow = new Row_EffectType_Effect_Skin(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_EffectType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_EffectType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Effect = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Effect));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Skin));
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

		TripleLongKey key(pRow->m_FK_EffectType,pRow->m_FK_Effect,pRow->m_FK_Skin);
		
		map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_EffectType_Effect_Skin *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_EffectType_Effect_Skin* Table_EffectType_Effect_Skin::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_EffectType_Effect_Skin *pRow = new Row_EffectType_Effect_Skin(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_EffectType_Effect_Skin* Table_EffectType_Effect_Skin::GetRow(long int in_FK_EffectType, long int in_FK_Effect, long int in_FK_Skin)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	TripleLongKey row_key(in_FK_EffectType, in_FK_Effect, in_FK_Skin);

	map<TripleLongKey, class TableRow*, TripleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_EffectType_Effect_Skin*) (*i).second;
	//we have to fetch row
	Row_EffectType_Effect_Skin* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_EffectType_Effect_Skin* Table_EffectType_Effect_Skin::FetchRow(TripleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_FK_EffectType[32];
sprintf(tmp_FK_EffectType, "%li", key.pk1);

char tmp_FK_Effect[32];
sprintf(tmp_FK_Effect, "%li", key.pk2);

char tmp_FK_Skin[32];
sprintf(tmp_FK_Skin, "%li", key.pk3);


string condition;
condition = condition + "`FK_EffectType`=" + tmp_FK_EffectType+" AND "+"`FK_Effect`=" + tmp_FK_Effect+" AND "+"`FK_Skin`=" + tmp_FK_Skin;


	string query = "select * from EffectType_Effect_Skin where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EffectType_Effect_Skin::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_EffectType_Effect_Skin *pRow = new Row_EffectType_Effect_Skin(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_EffectType = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_EffectType));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Effect = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Effect));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Skin));
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


class Row_EffectType* Row_EffectType_Effect_Skin::FK_EffectType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_EffectType *pTable = table->database->EffectType_get();
return pTable->GetRow(m_FK_EffectType);
}
class Row_Effect* Row_EffectType_Effect_Skin::FK_Effect_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Effect *pTable = table->database->Effect_get();
return pTable->GetRow(m_FK_Effect);
}
class Row_Skin* Row_EffectType_Effect_Skin::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}





