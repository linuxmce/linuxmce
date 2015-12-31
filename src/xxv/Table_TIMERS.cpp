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
#include "Table_TIMERS.h"



void Database_xxv::CreateTable_TIMERS()
{
	tblTIMERS = new Table_TIMERS(this);
}

void Database_xxv::DeleteTable_TIMERS()
{
	if( tblTIMERS )
		delete tblTIMERS;
}

bool Database_xxv::Commit_TIMERS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblTIMERS->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_TIMERS::~Table_TIMERS()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_TIMERS *pRow = (Row_TIMERS *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_TIMERS *pRow = (Row_TIMERS *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_TIMERS::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_TIMERS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_TIMERS *) *i != this); i++);
			
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

void Row_TIMERS::Reload()
{
	Row_TIMERS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_Id);
		Row_TIMERS *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_TIMERS::Row_TIMERS(Table_TIMERS *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_TIMERS::SetDefaultValues()
{
	m_Id = 0;
is_null[0] = false;
m_Status = "1";
is_null[1] = false;
m_ChannelID = "";
is_null[2] = false;
m_Day = "-------";
is_null[3] = false;
is_null[4] = true;
m_Start = 0;
is_null[5] = true;
m_Stop = 0;
is_null[6] = true;
m_Priority = 0;
is_null[7] = true;
m_Lifetime = 0;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
is_null[11] = true;
m_Collision = "0";
is_null[12] = false;
m_NextEpgId = 0;
is_null[13] = false;
m_AutotimerId = 0;
is_null[14] = false;
m_addtime = "0000-00-00 00:00:00";
is_null[15] = false;
m_Checked = "0";
is_null[16] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_TIMERS::Id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Id;}
string Row_TIMERS::Status_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Status;}
string Row_TIMERS::ChannelID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ChannelID;}
string Row_TIMERS::Day_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Day;}
long int Row_TIMERS::Start_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Start;}
long int Row_TIMERS::Stop_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Stop;}
short int Row_TIMERS::Priority_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Priority;}
short int Row_TIMERS::Lifetime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Lifetime;}
string Row_TIMERS::File_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_File;}
string Row_TIMERS::Summary_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Summary;}
string Row_TIMERS::NextStartTime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_NextStartTime;}
string Row_TIMERS::NextStopTime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_NextStopTime;}
string Row_TIMERS::Collision_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Collision;}
u_int64_t Row_TIMERS::NextEpgId_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_NextEpgId;}
long int Row_TIMERS::AutotimerId_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AutotimerId;}
string Row_TIMERS::addtime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_addtime;}
string Row_TIMERS::Checked_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Checked;}

		
void Row_TIMERS::Id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Id = val; is_modified=true; is_null[0]=false;}
void Row_TIMERS::Status_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Status = val; is_modified=true; is_null[1]=false;}
void Row_TIMERS::ChannelID_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ChannelID = val; is_modified=true; is_null[2]=false;}
void Row_TIMERS::Day_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Day = val; is_modified=true; is_null[3]=false;}
void Row_TIMERS::Start_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Start = val; is_modified=true; is_null[4]=false;}
void Row_TIMERS::Stop_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Stop = val; is_modified=true; is_null[5]=false;}
void Row_TIMERS::Priority_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Priority = val; is_modified=true; is_null[6]=false;}
void Row_TIMERS::Lifetime_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Lifetime = val; is_modified=true; is_null[7]=false;}
void Row_TIMERS::File_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_File = val; is_modified=true; is_null[8]=false;}
void Row_TIMERS::Summary_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Summary = val; is_modified=true; is_null[9]=false;}
void Row_TIMERS::NextStartTime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_NextStartTime = val; is_modified=true; is_null[10]=false;}
void Row_TIMERS::NextStopTime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_NextStopTime = val; is_modified=true; is_null[11]=false;}
void Row_TIMERS::Collision_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Collision = val; is_modified=true; is_null[12]=false;}
void Row_TIMERS::NextEpgId_set(u_int64_t val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_NextEpgId = val; is_modified=true; is_null[13]=false;}
void Row_TIMERS::AutotimerId_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AutotimerId = val; is_modified=true; is_null[14]=false;}
void Row_TIMERS::addtime_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_addtime = val; is_modified=true; is_null[15]=false;}
void Row_TIMERS::Checked_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Checked = val; is_modified=true; is_null[16]=false;}

		
bool Row_TIMERS::Status_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_TIMERS::Day_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_TIMERS::Start_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_TIMERS::Stop_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_TIMERS::Priority_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_TIMERS::Lifetime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_TIMERS::File_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_TIMERS::Summary_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_TIMERS::NextStartTime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_TIMERS::NextStopTime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_TIMERS::Collision_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_TIMERS::NextEpgId_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_TIMERS::AutotimerId_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_TIMERS::Checked_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}

			
void Row_TIMERS::Status_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_TIMERS::Day_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_TIMERS::Start_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_TIMERS::Stop_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_TIMERS::Priority_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_TIMERS::Lifetime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_TIMERS::File_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_TIMERS::Summary_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_TIMERS::NextStartTime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_TIMERS::NextStopTime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_TIMERS::Collision_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_TIMERS::NextEpgId_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_TIMERS::AutotimerId_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_TIMERS::Checked_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
	

string Row_TIMERS::Id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Id);

return buf;
}

string Row_TIMERS::Status_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Status.c_str(), (unsigned long) min((size_t)1,m_Status.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::ChannelID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ChannelID.c_str(), (unsigned long) min((size_t)100,m_ChannelID.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::Day_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Day.c_str(), (unsigned long) min((size_t)20,m_Day.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::Start_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Start);

return buf;
}

string Row_TIMERS::Stop_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Stop);

return buf;
}

string Row_TIMERS::Priority_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Priority);

return buf;
}

string Row_TIMERS::Lifetime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Lifetime);

return buf;
}

string Row_TIMERS::File_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_File.c_str(), (unsigned long) min((size_t)65535,m_File.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::Summary_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Summary.c_str(), (unsigned long) min((size_t)65535,m_Summary.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::NextStartTime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_NextStartTime.c_str(), (unsigned long) min((size_t)19,m_NextStartTime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::NextStopTime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_NextStopTime.c_str(), (unsigned long) min((size_t)19,m_NextStopTime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::Collision_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Collision.c_str(), (unsigned long) min((size_t)100,m_Collision.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::NextEpgId_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%I64i", m_NextEpgId);

return buf;
}

string Row_TIMERS::AutotimerId_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_AutotimerId);

return buf;
}

string Row_TIMERS::addtime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_addtime.c_str(), (unsigned long) min((size_t)19,m_addtime.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_TIMERS::Checked_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Checked.c_str(), (unsigned long) min((size_t)1,m_Checked.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}




Table_TIMERS::Key::Key(long int in_Id)
{
			pk_Id = in_Id;
	
}

Table_TIMERS::Key::Key(Row_TIMERS *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_Id = pRow->m_Id;
	
}		

bool Table_TIMERS::Key_Less::operator()(const Table_TIMERS::Key &key1, const Table_TIMERS::Key &key2) const
{
			if (key1.pk_Id!=key2.pk_Id)
return key1.pk_Id<key2.pk_Id;
else
return false;	
}	

bool Table_TIMERS::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_TIMERS *pRow = (Row_TIMERS *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->Id_asSQL()+", "+pRow->Status_asSQL()+", "+pRow->ChannelID_asSQL()+", "+pRow->Day_asSQL()+", "+pRow->Start_asSQL()+", "+pRow->Stop_asSQL()+", "+pRow->Priority_asSQL()+", "+pRow->Lifetime_asSQL()+", "+pRow->File_asSQL()+", "+pRow->Summary_asSQL()+", "+pRow->NextStartTime_asSQL()+", "+pRow->NextStopTime_asSQL()+", "+pRow->Collision_asSQL()+", "+pRow->NextEpgId_asSQL()+", "+pRow->AutotimerId_asSQL()+", "+pRow->addtime_asSQL()+", "+pRow->Checked_asSQL();

	
		string query = "insert into TIMERS (`Id`, `Status`, `ChannelID`, `Day`, `Start`, `Stop`, `Priority`, `Lifetime`, `File`, `Summary`, `NextStartTime`, `NextStopTime`, `Collision`, `NextEpgId`, `AutotimerId`, `addtime`, `Checked`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_TIMERS* pRow = (Row_TIMERS*) (*i).second;	
		SingleLongKey key(pRow->m_Id);

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`Id`="+pRow->Id_asSQL()+", `Status`="+pRow->Status_asSQL()+", `ChannelID`="+pRow->ChannelID_asSQL()+", `Day`="+pRow->Day_asSQL()+", `Start`="+pRow->Start_asSQL()+", `Stop`="+pRow->Stop_asSQL()+", `Priority`="+pRow->Priority_asSQL()+", `Lifetime`="+pRow->Lifetime_asSQL()+", `File`="+pRow->File_asSQL()+", `Summary`="+pRow->Summary_asSQL()+", `NextStartTime`="+pRow->NextStartTime_asSQL()+", `NextStopTime`="+pRow->NextStopTime_asSQL()+", `Collision`="+pRow->Collision_asSQL()+", `NextEpgId`="+pRow->NextEpgId_asSQL()+", `AutotimerId`="+pRow->AutotimerId_asSQL()+", `addtime`="+pRow->addtime_asSQL()+", `Checked`="+pRow->Checked_asSQL();

	
		string query = "update TIMERS set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_TIMERS* pRow = (Row_TIMERS*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_TIMERS* pRow = (Row_TIMERS*) (*i).second;	

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;

	
		string query = "delete from TIMERS where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_TIMERS*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_TIMERS::GetRows(string where_statement,vector<class Row_TIMERS*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `TIMERS`.* from TIMERS " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `TIMERS`.* from TIMERS where " + where_statement;
	else
		query = "select `TIMERS`.* from TIMERS";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_TIMERS *pRow = new Row_TIMERS(this);
		
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
pRow->m_Status = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Status = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_ChannelID = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_ChannelID = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Day = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Day = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Start = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Start));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Stop = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Stop));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Priority = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Priority));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Lifetime = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Lifetime));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_File = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_File = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Summary = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Summary = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_NextStartTime = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_NextStartTime = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_NextStopTime = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_NextStopTime = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Collision = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Collision = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_NextEpgId = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%I64i", &(pRow->m_NextEpgId));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_AutotimerId = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_AutotimerId));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_addtime = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_addtime = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Checked = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_Checked = string(row[16],lengths[16]);
}



		//checking for duplicates

		SingleLongKey key(pRow->m_Id);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_TIMERS *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_TIMERS* Table_TIMERS::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_TIMERS *pRow = new Row_TIMERS(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_TIMERS* Table_TIMERS::GetRow(long int in_Id)
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
		return (Row_TIMERS*) (*i).second;
	//we have to fetch row
	Row_TIMERS* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_TIMERS* Table_TIMERS::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;


	string query = "select * from TIMERS where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_TIMERS::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_TIMERS *pRow = new Row_TIMERS(this);
		
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
pRow->m_Status = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Status = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_ChannelID = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_ChannelID = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Day = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Day = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Start = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_Start));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Stop = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Stop));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Priority = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Priority));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Lifetime = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Lifetime));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_File = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_File = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Summary = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Summary = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_NextStartTime = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_NextStartTime = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_NextStopTime = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_NextStopTime = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Collision = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Collision = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_NextEpgId = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%I64i", &(pRow->m_NextEpgId));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_AutotimerId = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_AutotimerId));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_addtime = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_addtime = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Checked = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_Checked = string(row[16],lengths[16]);
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







