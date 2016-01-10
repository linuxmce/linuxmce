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
#include "Table_EPG.h"



void Database_xxv::CreateTable_EPG()
{
	tblEPG = new Table_EPG(this);
}

void Database_xxv::DeleteTable_EPG()
{
	if( tblEPG )
		delete tblEPG;
}

bool Database_xxv::Commit_EPG(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblEPG->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_EPG::~Table_EPG()
{
	map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_EPG *pRow = (Row_EPG *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_EPG *pRow = (Row_EPG *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_EPG::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_EPG *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_EPG *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLong64Key key(pRow->m_eventid);
			map<SingleLong64Key, TableRow*, SingleLong64Key_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_EPG::Reload()
{
	Row_EPG *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLong64Key key(pRow->m_eventid);
		Row_EPG *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_EPG::Row_EPG(Table_EPG *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_EPG::SetDefaultValues()
{
	m_eventid = 0;
is_null[0] = false;
m_title = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
m_channel_id = "";
is_null[4] = false;
m_starttime = "0000-00-00 00:00:00";
is_null[5] = false;
m_duration = 0;
is_null[6] = false;
m_tableid = 0;
is_null[7] = false;
is_null[8] = true;
m_version = 0;
is_null[9] = false;
m_video = "";
is_null[10] = false;
m_audio = "";
is_null[11] = false;
m_addtime = "0000-00-00 00:00:00";
is_null[12] = false;
m_vpstime = "0000-00-00 00:00:00";
is_null[13] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

u_int64_t Row_EPG::eventid_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_eventid;}
string Row_EPG::title_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_title;}
string Row_EPG::subtitle_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_subtitle;}
string Row_EPG::description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_description;}
string Row_EPG::channel_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_channel_id;}
string Row_EPG::starttime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_starttime;}
long int Row_EPG::duration_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_duration;}
short int Row_EPG::tableid_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_tableid;}
string Row_EPG::image_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_image;}
short int Row_EPG::version_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_version;}
string Row_EPG::video_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_video;}
string Row_EPG::audio_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_audio;}
string Row_EPG::addtime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_addtime;}
string Row_EPG::vpstime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_vpstime;}

		
void Row_EPG::eventid_set(u_int64_t val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_eventid = val; is_modified=true; is_null[0]=false;}
void Row_EPG::title_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_title = val; is_modified=true; is_null[1]=false;}
void Row_EPG::subtitle_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_subtitle = val; is_modified=true; is_null[2]=false;}
void Row_EPG::description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_description = val; is_modified=true; is_null[3]=false;}
void Row_EPG::channel_id_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_channel_id = val; is_modified=true; is_null[4]=false;}
void Row_EPG::starttime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_starttime = val; is_modified=true; is_null[5]=false;}
void Row_EPG::duration_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_duration = val; is_modified=true; is_null[6]=false;}
void Row_EPG::tableid_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_tableid = val; is_modified=true; is_null[7]=false;}
void Row_EPG::image_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_image = val; is_modified=true; is_null[8]=false;}
void Row_EPG::version_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_version = val; is_modified=true; is_null[9]=false;}
void Row_EPG::video_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_video = val; is_modified=true; is_null[10]=false;}
void Row_EPG::audio_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_audio = val; is_modified=true; is_null[11]=false;}
void Row_EPG::addtime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_addtime = val; is_modified=true; is_null[12]=false;}
void Row_EPG::vpstime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_vpstime = val; is_modified=true; is_null[13]=false;}

		
bool Row_EPG::subtitle_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_EPG::description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_EPG::tableid_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_EPG::image_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_EPG::version_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_EPG::video_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_EPG::audio_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_EPG::vpstime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}

			
void Row_EPG::subtitle_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_EPG::description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_EPG::tableid_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_EPG::image_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_EPG::version_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_EPG::video_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_EPG::audio_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_EPG::vpstime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
	

string Row_EPG::eventid_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%I64i", m_eventid);

return buf;
}

string Row_EPG::title_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_title.c_str(), (unsigned long) min((size_t)65535,m_title.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::subtitle_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_subtitle.c_str(), (unsigned long) min((size_t)65535,m_subtitle.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_description.c_str(), (unsigned long) min((size_t)65535,m_description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::channel_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_channel_id.c_str(), (unsigned long) min((size_t)100,m_channel_id.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::starttime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_starttime.c_str(), (unsigned long) min((size_t)19,m_starttime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::duration_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_duration);

return buf;
}

string Row_EPG::tableid_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_tableid);

return buf;
}

string Row_EPG::image_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_image.c_str(), (unsigned long) min((size_t)65535,m_image.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::version_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_version);

return buf;
}

string Row_EPG::video_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_video.c_str(), (unsigned long) min((size_t)100,m_video.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::audio_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_audio.c_str(), (unsigned long) min((size_t)255,m_audio.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::addtime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_addtime.c_str(), (unsigned long) min((size_t)19,m_addtime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_EPG::vpstime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_vpstime.c_str(), (unsigned long) min((size_t)19,m_vpstime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_EPG::Key::Key(u_int64_t in_eventid)
{
			pk_eventid = in_eventid;
	
}

Table_EPG::Key::Key(Row_EPG *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_eventid = pRow->m_eventid;
	
}		

bool Table_EPG::Key_Less::operator()(const Table_EPG::Key &key1, const Table_EPG::Key &key2) const
{
			if (key1.pk_eventid!=key2.pk_eventid)
return key1.pk_eventid<key2.pk_eventid;
else
return false;	
}	

bool Table_EPG::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_EPG *pRow = (Row_EPG *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->eventid_asSQL()+", "+pRow->title_asSQL()+", "+pRow->subtitle_asSQL()+", "+pRow->description_asSQL()+", "+pRow->channel_id_asSQL()+", "+pRow->starttime_asSQL()+", "+pRow->duration_asSQL()+", "+pRow->tableid_asSQL()+", "+pRow->image_asSQL()+", "+pRow->version_asSQL()+", "+pRow->video_asSQL()+", "+pRow->audio_asSQL()+", "+pRow->addtime_asSQL()+", "+pRow->vpstime_asSQL();

	
		string query = "insert into EPG (`eventid`, `title`, `subtitle`, `description`, `channel_id`, `starttime`, `duration`, `tableid`, `image`, `version`, `video`, `audio`, `addtime`, `vpstime`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			SingleLong64Key key(pRow->m_eventid);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_EPG* pRow = (Row_EPG*) (*i).second;	
		SingleLong64Key key(pRow->m_eventid);

		char tmp_eventid[32];
sprintf(tmp_eventid, "%I64i", key.pk);


string condition;
condition = condition + "`eventid`=" + tmp_eventid;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`eventid`="+pRow->eventid_asSQL()+", `title`="+pRow->title_asSQL()+", `subtitle`="+pRow->subtitle_asSQL()+", `description`="+pRow->description_asSQL()+", `channel_id`="+pRow->channel_id_asSQL()+", `starttime`="+pRow->starttime_asSQL()+", `duration`="+pRow->duration_asSQL()+", `tableid`="+pRow->tableid_asSQL()+", `image`="+pRow->image_asSQL()+", `version`="+pRow->version_asSQL()+", `video`="+pRow->video_asSQL()+", `audio`="+pRow->audio_asSQL()+", `addtime`="+pRow->addtime_asSQL()+", `vpstime`="+pRow->vpstime_asSQL();

	
		string query = "update EPG set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_EPG* pRow = (Row_EPG*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLong64Key key = (*i).first;
		Row_EPG* pRow = (Row_EPG*) (*i).second;	

		char tmp_eventid[32];
sprintf(tmp_eventid, "%I64i", key.pk);


string condition;
condition = condition + "`eventid`=" + tmp_eventid;

	
		string query = "delete from EPG where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_EPG*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_EPG::GetRows(string where_statement,vector<class Row_EPG*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `EPG`.* from EPG " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `EPG`.* from EPG where " + where_statement;
	else
		query = "select `EPG`.* from EPG";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_EPG *pRow = new Row_EPG(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_eventid = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%I64i", &(pRow->m_eventid));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_title = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_title = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_subtitle = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_subtitle = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_channel_id = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_channel_id = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_starttime = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_starttime = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_duration = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_duration));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_tableid = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_tableid));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_image = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_image = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_version = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_version));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_video = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_video = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_audio = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_audio = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_addtime = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_addtime = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_vpstime = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_vpstime = string(row[13],lengths[13]);
}



		//checking for duplicates

		SingleLong64Key key(pRow->m_eventid);
		
		map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_EPG *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_EPG* Table_EPG::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_EPG *pRow = new Row_EPG(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_EPG* Table_EPG::GetRow(u_int64_t in_eventid)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLong64Key row_key(in_eventid);

	map<SingleLong64Key, class TableRow*, SingleLong64Key_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_EPG*) (*i).second;
	//we have to fetch row
	Row_EPG* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_EPG* Table_EPG::FetchRow(SingleLong64Key &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_eventid[32];
sprintf(tmp_eventid, "%I64i", key.pk);


string condition;
condition = condition + "`eventid`=" + tmp_eventid;


	string query = "select * from EPG where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_EPG::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_EPG *pRow = new Row_EPG(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_eventid = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%I64i", &(pRow->m_eventid));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_title = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_title = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_subtitle = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_subtitle = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_channel_id = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_channel_id = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_starttime = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_starttime = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_duration = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_duration));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_tableid = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_tableid));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_image = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_image = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_version = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_version));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_video = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_video = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_audio = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_audio = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_addtime = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_addtime = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_vpstime = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_vpstime = string(row[13],lengths[13]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







