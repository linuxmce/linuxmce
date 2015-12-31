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
#include "Table_AUTOTIMER.h"



void Database_xxv::CreateTable_AUTOTIMER()
{
	tblAUTOTIMER = new Table_AUTOTIMER(this);
}

void Database_xxv::DeleteTable_AUTOTIMER()
{
	if( tblAUTOTIMER )
		delete tblAUTOTIMER;
}

bool Database_xxv::Commit_AUTOTIMER(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblAUTOTIMER->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_AUTOTIMER::~Table_AUTOTIMER()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_AUTOTIMER *pRow = (Row_AUTOTIMER *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_AUTOTIMER *pRow = (Row_AUTOTIMER *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_AUTOTIMER::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_AUTOTIMER *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_AUTOTIMER *) *i != this); i++);
			
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

void Row_AUTOTIMER::Reload()
{
	Row_AUTOTIMER *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_Id);
		Row_AUTOTIMER *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_AUTOTIMER::Row_AUTOTIMER(Table_AUTOTIMER *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_AUTOTIMER::SetDefaultValues()
{
	m_Id = 0;
is_null[0] = false;
m_Activ = "y";
is_null[1] = false;
m_Done = "timer";
is_null[2] = false;
m_Search = "";
is_null[3] = false;
m_InFields = "";
is_null[4] = false;
is_null[5] = true;
m_Start = "0000";
is_null[6] = false;
m_Stop = "0000";
is_null[7] = false;
is_null[8] = true;
m_MinLength = 0;
is_null[9] = true;
m_Priority = 0;
is_null[10] = true;
m_Lifetime = 0;
is_null[11] = true;
m_VPS = "n";
is_null[12] = false;
is_null[13] = true;
m_prevminutes = 0;
is_null[14] = true;
m_afterminutes = 0;
is_null[15] = true;
is_null[16] = true;
is_null[17] = true;
is_null[18] = true;
m_count = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_AUTOTIMER::Id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Id;}
string Row_AUTOTIMER::Activ_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Activ;}
string Row_AUTOTIMER::Done_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Done;}
string Row_AUTOTIMER::Search_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Search;}
string Row_AUTOTIMER::InFields_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_InFields;}
string Row_AUTOTIMER::Channels_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Channels;}
string Row_AUTOTIMER::Start_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Start;}
string Row_AUTOTIMER::Stop_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Stop;}
short int Row_AUTOTIMER::MinLength_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MinLength;}
short int Row_AUTOTIMER::Priority_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Priority;}
short int Row_AUTOTIMER::Lifetime_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Lifetime;}
string Row_AUTOTIMER::Dir_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Dir;}
string Row_AUTOTIMER::VPS_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VPS;}
short int Row_AUTOTIMER::prevminutes_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_prevminutes;}
short int Row_AUTOTIMER::afterminutes_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_afterminutes;}
string Row_AUTOTIMER::Weekdays_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Weekdays;}
string Row_AUTOTIMER::startdate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_startdate;}
string Row_AUTOTIMER::stopdate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_stopdate;}
long int Row_AUTOTIMER::count_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_count;}

		
void Row_AUTOTIMER::Id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Id = val; is_modified=true; is_null[0]=false;}
void Row_AUTOTIMER::Activ_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Activ = val; is_modified=true; is_null[1]=false;}
void Row_AUTOTIMER::Done_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Done = val; is_modified=true; is_null[2]=false;}
void Row_AUTOTIMER::Search_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Search = val; is_modified=true; is_null[3]=false;}
void Row_AUTOTIMER::InFields_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_InFields = val; is_modified=true; is_null[4]=false;}
void Row_AUTOTIMER::Channels_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Channels = val; is_modified=true; is_null[5]=false;}
void Row_AUTOTIMER::Start_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Start = val; is_modified=true; is_null[6]=false;}
void Row_AUTOTIMER::Stop_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Stop = val; is_modified=true; is_null[7]=false;}
void Row_AUTOTIMER::MinLength_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MinLength = val; is_modified=true; is_null[8]=false;}
void Row_AUTOTIMER::Priority_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Priority = val; is_modified=true; is_null[9]=false;}
void Row_AUTOTIMER::Lifetime_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Lifetime = val; is_modified=true; is_null[10]=false;}
void Row_AUTOTIMER::Dir_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Dir = val; is_modified=true; is_null[11]=false;}
void Row_AUTOTIMER::VPS_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VPS = val; is_modified=true; is_null[12]=false;}
void Row_AUTOTIMER::prevminutes_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_prevminutes = val; is_modified=true; is_null[13]=false;}
void Row_AUTOTIMER::afterminutes_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_afterminutes = val; is_modified=true; is_null[14]=false;}
void Row_AUTOTIMER::Weekdays_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Weekdays = val; is_modified=true; is_null[15]=false;}
void Row_AUTOTIMER::startdate_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_startdate = val; is_modified=true; is_null[16]=false;}
void Row_AUTOTIMER::stopdate_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_stopdate = val; is_modified=true; is_null[17]=false;}
void Row_AUTOTIMER::count_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_count = val; is_modified=true; is_null[18]=false;}

		
bool Row_AUTOTIMER::Activ_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_AUTOTIMER::Channels_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_AUTOTIMER::Start_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_AUTOTIMER::Stop_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_AUTOTIMER::MinLength_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_AUTOTIMER::Priority_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_AUTOTIMER::Lifetime_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_AUTOTIMER::Dir_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_AUTOTIMER::VPS_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_AUTOTIMER::prevminutes_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_AUTOTIMER::afterminutes_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_AUTOTIMER::Weekdays_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_AUTOTIMER::startdate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_AUTOTIMER::stopdate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_AUTOTIMER::count_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}

			
void Row_AUTOTIMER::Activ_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_AUTOTIMER::Channels_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_AUTOTIMER::Start_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_AUTOTIMER::Stop_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_AUTOTIMER::MinLength_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_AUTOTIMER::Priority_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_AUTOTIMER::Lifetime_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_AUTOTIMER::Dir_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_AUTOTIMER::VPS_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_AUTOTIMER::prevminutes_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_AUTOTIMER::afterminutes_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_AUTOTIMER::Weekdays_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_AUTOTIMER::startdate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_AUTOTIMER::stopdate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_AUTOTIMER::count_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
	

string Row_AUTOTIMER::Id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Id);

return buf;
}

string Row_AUTOTIMER::Activ_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Activ.c_str(), (unsigned long) min((size_t)1,m_Activ.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::Done_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[51];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Done.c_str(), (unsigned long) min((size_t)25,m_Done.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::Search_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Search.c_str(), (unsigned long) min((size_t)65535,m_Search.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::InFields_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[53];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_InFields.c_str(), (unsigned long) min((size_t)26,m_InFields.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::Channels_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Channels.c_str(), (unsigned long) min((size_t)65535,m_Channels.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::Start_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[9];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Start.c_str(), (unsigned long) min((size_t)4,m_Start.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::Stop_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[9];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Stop.c_str(), (unsigned long) min((size_t)4,m_Stop.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::MinLength_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MinLength);

return buf;
}

string Row_AUTOTIMER::Priority_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Priority);

return buf;
}

string Row_AUTOTIMER::Lifetime_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Lifetime);

return buf;
}

string Row_AUTOTIMER::Dir_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Dir.c_str(), (unsigned long) min((size_t)65535,m_Dir.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::VPS_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_VPS.c_str(), (unsigned long) min((size_t)1,m_VPS.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::prevminutes_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_prevminutes);

return buf;
}

string Row_AUTOTIMER::afterminutes_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_afterminutes);

return buf;
}

string Row_AUTOTIMER::Weekdays_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[55];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Weekdays.c_str(), (unsigned long) min((size_t)27,m_Weekdays.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::startdate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_startdate.c_str(), (unsigned long) min((size_t)19,m_startdate.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::stopdate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_stopdate.c_str(), (unsigned long) min((size_t)19,m_stopdate.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_AUTOTIMER::count_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_count);

return buf;
}




Table_AUTOTIMER::Key::Key(long int in_Id)
{
			pk_Id = in_Id;
	
}

Table_AUTOTIMER::Key::Key(Row_AUTOTIMER *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_Id = pRow->m_Id;
	
}		

bool Table_AUTOTIMER::Key_Less::operator()(const Table_AUTOTIMER::Key &key1, const Table_AUTOTIMER::Key &key2) const
{
			if (key1.pk_Id!=key2.pk_Id)
return key1.pk_Id<key2.pk_Id;
else
return false;	
}	

bool Table_AUTOTIMER::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_AUTOTIMER *pRow = (Row_AUTOTIMER *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->Id_asSQL()+", "+pRow->Activ_asSQL()+", "+pRow->Done_asSQL()+", "+pRow->Search_asSQL()+", "+pRow->InFields_asSQL()+", "+pRow->Channels_asSQL()+", "+pRow->Start_asSQL()+", "+pRow->Stop_asSQL()+", "+pRow->MinLength_asSQL()+", "+pRow->Priority_asSQL()+", "+pRow->Lifetime_asSQL()+", "+pRow->Dir_asSQL()+", "+pRow->VPS_asSQL()+", "+pRow->prevminutes_asSQL()+", "+pRow->afterminutes_asSQL()+", "+pRow->Weekdays_asSQL()+", "+pRow->startdate_asSQL()+", "+pRow->stopdate_asSQL()+", "+pRow->count_asSQL();

	
		string query = "insert into AUTOTIMER (`Id`, `Activ`, `Done`, `Search`, `InFields`, `Channels`, `Start`, `Stop`, `MinLength`, `Priority`, `Lifetime`, `Dir`, `VPS`, `prevminutes`, `afterminutes`, `Weekdays`, `startdate`, `stopdate`, `count`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_AUTOTIMER* pRow = (Row_AUTOTIMER*) (*i).second;	
		SingleLongKey key(pRow->m_Id);

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`Id`="+pRow->Id_asSQL()+", `Activ`="+pRow->Activ_asSQL()+", `Done`="+pRow->Done_asSQL()+", `Search`="+pRow->Search_asSQL()+", `InFields`="+pRow->InFields_asSQL()+", `Channels`="+pRow->Channels_asSQL()+", `Start`="+pRow->Start_asSQL()+", `Stop`="+pRow->Stop_asSQL()+", `MinLength`="+pRow->MinLength_asSQL()+", `Priority`="+pRow->Priority_asSQL()+", `Lifetime`="+pRow->Lifetime_asSQL()+", `Dir`="+pRow->Dir_asSQL()+", `VPS`="+pRow->VPS_asSQL()+", `prevminutes`="+pRow->prevminutes_asSQL()+", `afterminutes`="+pRow->afterminutes_asSQL()+", `Weekdays`="+pRow->Weekdays_asSQL()+", `startdate`="+pRow->startdate_asSQL()+", `stopdate`="+pRow->stopdate_asSQL()+", `count`="+pRow->count_asSQL();

	
		string query = "update AUTOTIMER set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_AUTOTIMER* pRow = (Row_AUTOTIMER*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_AUTOTIMER* pRow = (Row_AUTOTIMER*) (*i).second;	

		char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;

	
		string query = "delete from AUTOTIMER where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_AUTOTIMER*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_AUTOTIMER::GetRows(string where_statement,vector<class Row_AUTOTIMER*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `AUTOTIMER`.* from AUTOTIMER " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `AUTOTIMER`.* from AUTOTIMER where " + where_statement;
	else
		query = "select `AUTOTIMER`.* from AUTOTIMER";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_AUTOTIMER *pRow = new Row_AUTOTIMER(this);
		
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
pRow->m_Activ = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Activ = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Done = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Done = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Search = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Search = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_InFields = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_InFields = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Channels = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Channels = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Start = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Start = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Stop = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Stop = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_MinLength = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_MinLength));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Priority = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Priority));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Lifetime = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_Lifetime));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Dir = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_Dir = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_VPS = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_VPS = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_prevminutes = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_prevminutes));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_afterminutes = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_afterminutes));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Weekdays = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Weekdays = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_startdate = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_startdate = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_stopdate = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_stopdate = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_count = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_count));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_Id);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_AUTOTIMER *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_AUTOTIMER* Table_AUTOTIMER::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_AUTOTIMER *pRow = new Row_AUTOTIMER(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_AUTOTIMER* Table_AUTOTIMER::GetRow(long int in_Id)
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
		return (Row_AUTOTIMER*) (*i).second;
	//we have to fetch row
	Row_AUTOTIMER* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_AUTOTIMER* Table_AUTOTIMER::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_Id[32];
sprintf(tmp_Id, "%li", key.pk);


string condition;
condition = condition + "`Id`=" + tmp_Id;


	string query = "select * from AUTOTIMER where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_AUTOTIMER::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_AUTOTIMER *pRow = new Row_AUTOTIMER(this);
		
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
pRow->m_Activ = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_Activ = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Done = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Done = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Search = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Search = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_InFields = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_InFields = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Channels = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Channels = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Start = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Start = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Stop = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Stop = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_MinLength = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_MinLength));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Priority = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Priority));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Lifetime = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_Lifetime));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_Dir = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_Dir = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_VPS = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_VPS = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_prevminutes = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_prevminutes));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_afterminutes = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_afterminutes));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Weekdays = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Weekdays = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_startdate = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_startdate = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_stopdate = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_stopdate = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_count = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_count));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







