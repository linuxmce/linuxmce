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
#include "Table_RepositorySource_URL.h"
#include "Table_RepositorySource.h"
#include "Table_Country.h"

#include "Table_Installation_RepositorySource_URL.h"


void Database_pluto_main::CreateTable_RepositorySource_URL()
{
	tblRepositorySource_URL = new Table_RepositorySource_URL(this);
}

void Database_pluto_main::DeleteTable_RepositorySource_URL()
{
	if( tblRepositorySource_URL )
		delete tblRepositorySource_URL;
}

bool Database_pluto_main::Commit_RepositorySource_URL(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblRepositorySource_URL->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_RepositorySource_URL::~Table_RepositorySource_URL()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_RepositorySource_URL *pRow = (Row_RepositorySource_URL *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_RepositorySource_URL *pRow = (Row_RepositorySource_URL *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_RepositorySource_URL::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_RepositorySource_URL *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_RepositorySource_URL *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_RepositorySource_URL);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_RepositorySource_URL::Reload()
{
	Row_RepositorySource_URL *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_RepositorySource_URL);
		Row_RepositorySource_URL *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_RepositorySource_URL::Row_RepositorySource_URL(Table_RepositorySource_URL *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_RepositorySource_URL::SetDefaultValues()
{
	m_PK_RepositorySource_URL = 0;
is_null[0] = false;
m_FK_RepositorySource = 0;
is_null[1] = false;
m_URL = "";
is_null[2] = false;
is_null[3] = true;
m_FK_Country = 0;
is_null[4] = true;
m_Password = "";
is_null[5] = false;
is_null[6] = true;
is_null[7] = true;
m_psc_id = 0;
is_null[8] = true;
m_psc_batch = 0;
is_null[9] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[10] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[11] = false;
is_null[12] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_RepositorySource_URL::PK_RepositorySource_URL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_RepositorySource_URL;}
long int Row_RepositorySource_URL::FK_RepositorySource_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_RepositorySource;}
string Row_RepositorySource_URL::URL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_URL;}
long int Row_RepositorySource_URL::FK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Country;}
string Row_RepositorySource_URL::Username_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Username;}
string Row_RepositorySource_URL::Password_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password;}
string Row_RepositorySource_URL::Comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Comments;}
long int Row_RepositorySource_URL::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_RepositorySource_URL::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_RepositorySource_URL::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_RepositorySource_URL::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_RepositorySource_URL::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_RepositorySource_URL::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_RepositorySource_URL::PK_RepositorySource_URL_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_RepositorySource_URL = val; is_modified=true; is_null[0]=false;}
void Row_RepositorySource_URL::FK_RepositorySource_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_RepositorySource = val; is_modified=true; is_null[1]=false;}
void Row_RepositorySource_URL::URL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_URL = val; is_modified=true; is_null[2]=false;}
void Row_RepositorySource_URL::FK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Country = val; is_modified=true; is_null[3]=false;}
void Row_RepositorySource_URL::Username_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Username = val; is_modified=true; is_null[4]=false;}
void Row_RepositorySource_URL::Password_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password = val; is_modified=true; is_null[5]=false;}
void Row_RepositorySource_URL::Comments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Comments = val; is_modified=true; is_null[6]=false;}
void Row_RepositorySource_URL::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[7]=false;}
void Row_RepositorySource_URL::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[8]=false;}
void Row_RepositorySource_URL::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[9]=false;}
void Row_RepositorySource_URL::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[10]=false;}
void Row_RepositorySource_URL::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[11]=false;}
void Row_RepositorySource_URL::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[12]=false;}

		
bool Row_RepositorySource_URL::FK_Country_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_RepositorySource_URL::Username_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_RepositorySource_URL::Comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_RepositorySource_URL::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_RepositorySource_URL::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_RepositorySource_URL::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_RepositorySource_URL::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_RepositorySource_URL::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}

			
void Row_RepositorySource_URL::FK_Country_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_RepositorySource_URL::Username_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_RepositorySource_URL::Comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_RepositorySource_URL::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_RepositorySource_URL::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_RepositorySource_URL::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_RepositorySource_URL::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_RepositorySource_URL::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
	

string Row_RepositorySource_URL::PK_RepositorySource_URL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_RepositorySource_URL);

return buf;
}

string Row_RepositorySource_URL::FK_RepositorySource_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RepositorySource);

return buf;
}

string Row_RepositorySource_URL::URL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_URL.c_str(), (unsigned long) min((size_t)255,m_URL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RepositorySource_URL::FK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Country);

return buf;
}

string Row_RepositorySource_URL::Username_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Username.c_str(), (unsigned long) min((size_t)30,m_Username.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RepositorySource_URL::Password_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Password.c_str(), (unsigned long) min((size_t)30,m_Password.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RepositorySource_URL::Comments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Comments.c_str(), (unsigned long) min((size_t)30,m_Comments.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RepositorySource_URL::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_RepositorySource_URL::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_RepositorySource_URL::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_RepositorySource_URL::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_RepositorySource_URL::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_RepositorySource_URL::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_RepositorySource_URL::Key::Key(long int in_PK_RepositorySource_URL)
{
			pk_PK_RepositorySource_URL = in_PK_RepositorySource_URL;
	
}

Table_RepositorySource_URL::Key::Key(Row_RepositorySource_URL *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_RepositorySource_URL = pRow->m_PK_RepositorySource_URL;
	
}		

bool Table_RepositorySource_URL::Key_Less::operator()(const Table_RepositorySource_URL::Key &key1, const Table_RepositorySource_URL::Key &key2) const
{
			if (key1.pk_PK_RepositorySource_URL!=key2.pk_PK_RepositorySource_URL)
return key1.pk_PK_RepositorySource_URL<key2.pk_PK_RepositorySource_URL;
else
return false;	
}	

bool Table_RepositorySource_URL::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_RepositorySource_URL *pRow = (Row_RepositorySource_URL *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_RepositorySource_URL_asSQL()+", "+pRow->FK_RepositorySource_asSQL()+", "+pRow->URL_asSQL()+", "+pRow->FK_Country_asSQL()+", "+pRow->Username_asSQL()+", "+pRow->Password_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into RepositorySource_URL (`PK_RepositorySource_URL`, `FK_RepositorySource`, `URL`, `FK_Country`, `Username`, `Password`, `Comments`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_RepositorySource_URL=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_RepositorySource_URL is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_RepositorySource_URL);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_RepositorySource_URL* pRow = (Row_RepositorySource_URL*) (*i).second;	
		SingleLongKey key(pRow->m_PK_RepositorySource_URL);

		char tmp_PK_RepositorySource_URL[32];
sprintf(tmp_PK_RepositorySource_URL, "%li", key.pk);


string condition;
condition = condition + "`PK_RepositorySource_URL`=" + tmp_PK_RepositorySource_URL;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_RepositorySource_URL`="+pRow->PK_RepositorySource_URL_asSQL()+", `FK_RepositorySource`="+pRow->FK_RepositorySource_asSQL()+", `URL`="+pRow->URL_asSQL()+", `FK_Country`="+pRow->FK_Country_asSQL()+", `Username`="+pRow->Username_asSQL()+", `Password`="+pRow->Password_asSQL()+", `Comments`="+pRow->Comments_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update RepositorySource_URL set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_RepositorySource_URL* pRow = (Row_RepositorySource_URL*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_RepositorySource_URL* pRow = (Row_RepositorySource_URL*) (*i).second;	

		char tmp_PK_RepositorySource_URL[32];
sprintf(tmp_PK_RepositorySource_URL, "%li", key.pk);


string condition;
condition = condition + "`PK_RepositorySource_URL`=" + tmp_PK_RepositorySource_URL;

	
		string query = "delete from RepositorySource_URL where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_RepositorySource_URL*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_RepositorySource_URL::GetRows(string where_statement,vector<class Row_RepositorySource_URL*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `RepositorySource_URL`.* from RepositorySource_URL " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `RepositorySource_URL`.* from RepositorySource_URL where " + where_statement;
	else
		query = "select `RepositorySource_URL`.* from RepositorySource_URL";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_RepositorySource_URL *pRow = new Row_RepositorySource_URL(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RepositorySource_URL = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RepositorySource_URL));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RepositorySource = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RepositorySource));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_URL = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Country));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Username = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Username = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Password = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Password = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Comments = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_id));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_batch));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_user));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_psc_frozen));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_psc_mod = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_RepositorySource_URL);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_RepositorySource_URL *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_RepositorySource_URL* Table_RepositorySource_URL::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_RepositorySource_URL *pRow = new Row_RepositorySource_URL(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_RepositorySource_URL* Table_RepositorySource_URL::GetRow(long int in_PK_RepositorySource_URL)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_RepositorySource_URL);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_RepositorySource_URL*) (*i).second;
	//we have to fetch row
	Row_RepositorySource_URL* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_RepositorySource_URL* Table_RepositorySource_URL::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_RepositorySource_URL[32];
sprintf(tmp_PK_RepositorySource_URL, "%li", key.pk);


string condition;
condition = condition + "`PK_RepositorySource_URL`=" + tmp_PK_RepositorySource_URL;


	string query = "select * from RepositorySource_URL where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_RepositorySource_URL::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_RepositorySource_URL *pRow = new Row_RepositorySource_URL(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_RepositorySource_URL = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_RepositorySource_URL));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_RepositorySource = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_RepositorySource));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_URL = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_URL = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Country));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Username = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Username = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Password = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Password = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Comments = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Comments = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_psc_id));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_psc_batch));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_user));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_psc_frozen));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_psc_mod = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_RepositorySource* Row_RepositorySource_URL::FK_RepositorySource_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RepositorySource *pTable = table->database->RepositorySource_get();
return pTable->GetRow(m_FK_RepositorySource);
}
class Row_Country* Row_RepositorySource_URL::FK_Country_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Country *pTable = table->database->Country_get();
return pTable->GetRow(m_FK_Country);
}


void Row_RepositorySource_URL::Installation_RepositorySource_URL_FK_RepositorySource_URL_getrows(vector <class Row_Installation_RepositorySource_URL*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation_RepositorySource_URL *pTable = table->database->Installation_RepositorySource_URL_get();
pTable->GetRows("`FK_RepositorySource_URL`=" + StringUtils::itos(m_PK_RepositorySource_URL),rows);
}



