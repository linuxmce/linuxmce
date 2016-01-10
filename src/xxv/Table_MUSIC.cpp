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
#include "Table_MUSIC.h"



void Database_xxv::CreateTable_MUSIC()
{
	tblMUSIC = new Table_MUSIC(this);
}

void Database_xxv::DeleteTable_MUSIC()
{
	if( tblMUSIC )
		delete tblMUSIC;
}

bool Database_xxv::Commit_MUSIC(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblMUSIC->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_MUSIC::~Table_MUSIC()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_MUSIC *pRow = (Row_MUSIC *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_MUSIC *pRow = (Row_MUSIC *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_MUSIC::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_MUSIC *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_MUSIC *) *i != this); i++);
			
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

void Row_MUSIC::Reload()
{
	Row_MUSIC *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_Id);
		Row_MUSIC *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MUSIC::Row_MUSIC(Table_MUSIC *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MUSIC::SetDefaultValues()
{
	m_Id = 0;
is_null[0] = false;
m_FILE = "";
is_null[1] = false;
m_ARTIST = "unknown";
is_null[2] = false;
m_ALBUM = "unknown";
is_null[3] = false;
m_TITLE = "unknown";
is_null[4] = false;
is_null[5] = true;
m_TRACKNUM = "0";
is_null[6] = false;
is_null[7] = true;
m_YEAR = 0;
is_null[8] = true;
is_null[9] = true;
m_BITRATE = 0;
is_null[10] = true;
m_SECS = 0;
is_null[11] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MUSIC::Id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Id;}
string Row_MUSIC::FILE_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FILE;}
string Row_MUSIC::ARTIST_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ARTIST;}
string Row_MUSIC::ALBUM_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ALBUM;}
string Row_MUSIC::TITLE_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TITLE;}
string Row_MUSIC::COMMENT_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_COMMENT;}
string Row_MUSIC::TRACKNUM_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TRACKNUM;}
short int Row_MUSIC::YEAR_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_YEAR;}
string Row_MUSIC::GENRE_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_GENRE;}
short int Row_MUSIC::BITRATE_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_BITRATE;}
string Row_MUSIC::FREQUENCY_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FREQUENCY;}
long int Row_MUSIC::SECS_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SECS;}

		
void Row_MUSIC::Id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Id = val; is_modified=true; is_null[0]=false;}
void Row_MUSIC::FILE_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FILE = val; is_modified=true; is_null[1]=false;}
void Row_MUSIC::ARTIST_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ARTIST = val; is_modified=true; is_null[2]=false;}
void Row_MUSIC::ALBUM_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ALBUM = val; is_modified=true; is_null[3]=false;}
void Row_MUSIC::TITLE_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TITLE = val; is_modified=true; is_null[4]=false;}
void Row_MUSIC::COMMENT_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_COMMENT = val; is_modified=true; is_null[5]=false;}
void Row_MUSIC::TRACKNUM_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TRACKNUM = val; is_modified=true; is_null[6]=false;}
void Row_MUSIC::YEAR_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_YEAR = val; is_modified=true; is_null[7]=false;}
void Row_MUSIC::GENRE_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_GENRE = val; is_modified=true; is_null[8]=false;}
void Row_MUSIC::BITRATE_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_BITRATE = val; is_modified=true; is_null[9]=false;}
void Row_MUSIC::FREQUENCY_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FREQUENCY = val; is_modified=true; is_null[10]=false;}
void Row_MUSIC::SECS_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SECS = val; is_modified=true; is_null[11]=false;}

		
bool Row_MUSIC::ARTIST_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_MUSIC::ALBUM_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_MUSIC::TITLE_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_MUSIC::COMMENT_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_MUSIC::TRACKNUM_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_MUSIC::YEAR_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_MUSIC::GENRE_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_MUSIC::BITRATE_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_MUSIC::FREQUENCY_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}

			
void Row_MUSIC::ARTIST_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_MUSIC::ALBUM_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_MUSIC::TITLE_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_MUSIC::COMMENT_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_MUSIC::TRACKNUM_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_MUSIC::YEAR_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_MUSIC::GENRE_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_MUSIC::BITRATE_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_MUSIC::FREQUENCY_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
	

string Row_MUSIC::Id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Id);

return buf;
}

string Row_MUSIC::FILE_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_FILE.c_str(), (unsigned long) min((size_t)65535,m_FILE.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::ARTIST_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[257];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ARTIST.c_str(), (unsigned long) min((size_t)128,m_ARTIST.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::ALBUM_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[257];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ALBUM.c_str(), (unsigned long) min((size_t)128,m_ALBUM.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::TITLE_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[257];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_TITLE.c_str(), (unsigned long) min((size_t)128,m_TITLE.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::COMMENT_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[257];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_COMMENT.c_str(), (unsigned long) min((size_t)128,m_COMMENT.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::TRACKNUM_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_TRACKNUM.c_str(), (unsigned long) min((size_t)10,m_TRACKNUM.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::YEAR_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_YEAR);

return buf;
}

string Row_MUSIC::GENRE_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[257];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_GENRE.c_str(), (unsigned long) min((size_t)128,m_GENRE.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::BITRATE_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_BITRATE);

return buf;
}

string Row_MUSIC::FREQUENCY_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[9];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_FREQUENCY.c_str(), (unsigned long) min((size_t)4,m_FREQUENCY.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MUSIC::SECS_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_SECS);

return buf;
}




Table_MUSIC::Key::Key(long int in_Id)
{
			pk_Id = in_Id;
	
}

Table_MUSIC::Key::Key(Row_MUSIC *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_Id = pRow->m_Id;
	
}		

bool Table_MUSIC::Key_Less::operator()(const Table_MUSIC::Key &key1, const Table_MUSIC::Key &key2) const
{
			if (key1.pk_Id!=key2.pk_Id)
return key1.pk_Id<key2.pk_Id;
else
return false;	
}	

bool Table_MUSIC::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_MUSIC *pRow = (Row_MUSIC *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->Id_asSQL()+", "+pRow->FILE_asSQL()+", "+pRow->ARTIST_asSQL()+", "+pRow->ALBUM_asSQL()+", "+pRow->TITLE_asSQL()+", "+pRow->COMMENT_asSQL()+", "+pRow->TRACKNUM_asSQL()+", "+pRow->YEAR_asSQL()+", "+pRow->GENRE_asSQL()+", "+pRow->BITRATE_asSQL()+", "+pRow->FREQUENCY_asSQL()+", "+pRow->SECS_asSQL();

	
		string query = "insert into MUSIC (`Id`, `FILE`, `ARTIST`, `ALBUM`, `TITLE`, `COMMENT`, `TRACKNUM`, `YEAR`, `GENRE`, `BITRATE`, `FREQUENCY`, `SECS`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_MUSIC* pRow = (Row_MUSIC*) (*i).second;	
		SingleLongKey key(pRow->m_Id);

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`Id`="+pRow->Id_asSQL()+", `FILE`="+pRow->FILE_asSQL()+", `ARTIST`="+pRow->ARTIST_asSQL()+", `ALBUM`="+pRow->ALBUM_asSQL()+", `TITLE`="+pRow->TITLE_asSQL()+", `COMMENT`="+pRow->COMMENT_asSQL()+", `TRACKNUM`="+pRow->TRACKNUM_asSQL()+", `YEAR`="+pRow->YEAR_asSQL()+", `GENRE`="+pRow->GENRE_asSQL()+", `BITRATE`="+pRow->BITRATE_asSQL()+", `FREQUENCY`="+pRow->FREQUENCY_asSQL()+", `SECS`="+pRow->SECS_asSQL();

	
		string query = "update MUSIC set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_MUSIC* pRow = (Row_MUSIC*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_MUSIC* pRow = (Row_MUSIC*) (*i).second;	

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;

	
		string query = "delete from MUSIC where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_MUSIC*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_MUSIC::GetRows(string where_statement,vector<class Row_MUSIC*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `MUSIC`.* from MUSIC " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `MUSIC`.* from MUSIC where " + where_statement;
	else
		query = "select `MUSIC`.* from MUSIC";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_MUSIC *pRow = new Row_MUSIC(this);
		
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
pRow->m_FILE = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_FILE = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_ARTIST = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_ARTIST = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_ALBUM = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_ALBUM = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_TITLE = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_TITLE = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_COMMENT = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_COMMENT = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_TRACKNUM = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_TRACKNUM = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_YEAR = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_YEAR));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_GENRE = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_GENRE = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_BITRATE = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_BITRATE));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FREQUENCY = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_FREQUENCY = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_SECS = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_SECS));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_Id);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_MUSIC *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_MUSIC* Table_MUSIC::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_MUSIC *pRow = new Row_MUSIC(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MUSIC* Table_MUSIC::GetRow(long int in_Id)
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
		return (Row_MUSIC*) (*i).second;
	//we have to fetch row
	Row_MUSIC* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MUSIC* Table_MUSIC::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;


	string query = "select * from MUSIC where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MUSIC::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_MUSIC *pRow = new Row_MUSIC(this);
		
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
pRow->m_FILE = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_FILE = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_ARTIST = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_ARTIST = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_ALBUM = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_ALBUM = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_TITLE = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_TITLE = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_COMMENT = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_COMMENT = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_TRACKNUM = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_TRACKNUM = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_YEAR = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_YEAR));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_GENRE = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_GENRE = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_BITRATE = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_BITRATE));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FREQUENCY = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_FREQUENCY = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_SECS = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_SECS));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







