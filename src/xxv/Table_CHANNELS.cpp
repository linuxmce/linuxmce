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
#include "Table_CHANNELS.h"



void Database_xxv::CreateTable_CHANNELS()
{
	tblCHANNELS = new Table_CHANNELS(this);
}

void Database_xxv::DeleteTable_CHANNELS()
{
	if( tblCHANNELS )
		delete tblCHANNELS;
}

bool Database_xxv::Commit_CHANNELS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblCHANNELS->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_CHANNELS::~Table_CHANNELS()
{
	map<SingleStringKey, class TableRow*, SingleStringKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_CHANNELS *pRow = (Row_CHANNELS *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_CHANNELS *pRow = (Row_CHANNELS *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_CHANNELS::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_CHANNELS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_CHANNELS *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleStringKey key(pRow->m_Id);
			map<SingleStringKey, TableRow*, SingleStringKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_CHANNELS::Reload()
{
	Row_CHANNELS *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleStringKey key(pRow->m_Id);
		Row_CHANNELS *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_CHANNELS::Row_CHANNELS(Table_CHANNELS *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_CHANNELS::SetDefaultValues()
{
	m_Id = "";
is_null[0] = false;
m_Name = "";
is_null[1] = false;
m_Frequency = 0;
is_null[2] = false;
m_Parameters = "";
is_null[3] = false;
is_null[4] = true;
m_Srate = 0;
is_null[5] = false;
m_VPID = "";
is_null[6] = false;
m_APID = "";
is_null[7] = false;
m_TPID = "";
is_null[8] = false;
m_CA = "";
is_null[9] = false;
m_SID = 0;
is_null[10] = false;
m_NID = 0;
is_null[11] = false;
m_TID = 0;
is_null[12] = false;
m_RID = 0;
is_null[13] = false;
m_GRP = 0;
is_null[14] = false;
m_POS = 0;
is_null[15] = false;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

string Row_CHANNELS::Id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Id;}
string Row_CHANNELS::Name_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Name;}
long int Row_CHANNELS::Frequency_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Frequency;}
string Row_CHANNELS::Parameters_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Parameters;}
string Row_CHANNELS::Source_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Source;}
long int Row_CHANNELS::Srate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Srate;}
string Row_CHANNELS::VPID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VPID;}
string Row_CHANNELS::APID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_APID;}
string Row_CHANNELS::TPID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TPID;}
string Row_CHANNELS::CA_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CA;}
long int Row_CHANNELS::SID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SID;}
long int Row_CHANNELS::NID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_NID;}
long int Row_CHANNELS::TID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TID;}
long int Row_CHANNELS::RID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RID;}
long int Row_CHANNELS::GRP_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_GRP;}
long int Row_CHANNELS::POS_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_POS;}

		
void Row_CHANNELS::Id_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Id = val; is_modified=true; is_null[0]=false;}
void Row_CHANNELS::Name_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Name = val; is_modified=true; is_null[1]=false;}
void Row_CHANNELS::Frequency_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Frequency = val; is_modified=true; is_null[2]=false;}
void Row_CHANNELS::Parameters_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Parameters = val; is_modified=true; is_null[3]=false;}
void Row_CHANNELS::Source_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Source = val; is_modified=true; is_null[4]=false;}
void Row_CHANNELS::Srate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Srate = val; is_modified=true; is_null[5]=false;}
void Row_CHANNELS::VPID_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VPID = val; is_modified=true; is_null[6]=false;}
void Row_CHANNELS::APID_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_APID = val; is_modified=true; is_null[7]=false;}
void Row_CHANNELS::TPID_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TPID = val; is_modified=true; is_null[8]=false;}
void Row_CHANNELS::CA_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CA = val; is_modified=true; is_null[9]=false;}
void Row_CHANNELS::SID_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SID = val; is_modified=true; is_null[10]=false;}
void Row_CHANNELS::NID_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_NID = val; is_modified=true; is_null[11]=false;}
void Row_CHANNELS::TID_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TID = val; is_modified=true; is_null[12]=false;}
void Row_CHANNELS::RID_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RID = val; is_modified=true; is_null[13]=false;}
void Row_CHANNELS::GRP_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_GRP = val; is_modified=true; is_null[14]=false;}
void Row_CHANNELS::POS_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_POS = val; is_modified=true; is_null[15]=false;}

		
bool Row_CHANNELS::Parameters_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_CHANNELS::Source_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_CHANNELS::Srate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_CHANNELS::VPID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_CHANNELS::APID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_CHANNELS::TPID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_CHANNELS::CA_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_CHANNELS::SID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_CHANNELS::NID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_CHANNELS::TID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_CHANNELS::RID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_CHANNELS::GRP_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}

			
void Row_CHANNELS::Parameters_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_CHANNELS::Source_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_CHANNELS::Srate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_CHANNELS::VPID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_CHANNELS::APID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_CHANNELS::TPID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_CHANNELS::CA_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_CHANNELS::SID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_CHANNELS::NID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_CHANNELS::TID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_CHANNELS::RID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_CHANNELS::GRP_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
	

string Row_CHANNELS::Id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Id.c_str(), (unsigned long) min((size_t)100,m_Id.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::Name_asSQL()
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

string Row_CHANNELS::Frequency_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Frequency);

return buf;
}

string Row_CHANNELS::Parameters_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Parameters.c_str(), (unsigned long) min((size_t)100,m_Parameters.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::Source_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Source.c_str(), (unsigned long) min((size_t)100,m_Source.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::Srate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Srate);

return buf;
}

string Row_CHANNELS::VPID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_VPID.c_str(), (unsigned long) min((size_t)100,m_VPID.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::APID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_APID.c_str(), (unsigned long) min((size_t)100,m_APID.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::TPID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_TPID.c_str(), (unsigned long) min((size_t)100,m_TPID.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::CA_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_CA.c_str(), (unsigned long) min((size_t)100,m_CA.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_CHANNELS::SID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_SID);

return buf;
}

string Row_CHANNELS::NID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_NID);

return buf;
}

string Row_CHANNELS::TID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_TID);

return buf;
}

string Row_CHANNELS::RID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_RID);

return buf;
}

string Row_CHANNELS::GRP_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_GRP);

return buf;
}

string Row_CHANNELS::POS_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_POS);

return buf;
}




Table_CHANNELS::Key::Key(string in_Id)
{
			pk_Id = in_Id;
	
}

Table_CHANNELS::Key::Key(Row_CHANNELS *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_Id = pRow->m_Id;
	
}		

bool Table_CHANNELS::Key_Less::operator()(const Table_CHANNELS::Key &key1, const Table_CHANNELS::Key &key2) const
{
			if (key1.pk_Id!=key2.pk_Id)
return key1.pk_Id<key2.pk_Id;
else
return false;	
}	

bool Table_CHANNELS::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_CHANNELS *pRow = (Row_CHANNELS *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->Id_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->Frequency_asSQL()+", "+pRow->Parameters_asSQL()+", "+pRow->Source_asSQL()+", "+pRow->Srate_asSQL()+", "+pRow->VPID_asSQL()+", "+pRow->APID_asSQL()+", "+pRow->TPID_asSQL()+", "+pRow->CA_asSQL()+", "+pRow->SID_asSQL()+", "+pRow->NID_asSQL()+", "+pRow->TID_asSQL()+", "+pRow->RID_asSQL()+", "+pRow->GRP_asSQL()+", "+pRow->POS_asSQL();

	
		string query = "insert into CHANNELS (`Id`, `Name`, `Frequency`, `Parameters`, `Source`, `Srate`, `VPID`, `APID`, `TPID`, `CA`, `SID`, `NID`, `TID`, `RID`, `GRP`, `POS`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			SingleStringKey key(pRow->m_Id);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleStringKey, class TableRow*, SingleStringKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_CHANNELS* pRow = (Row_CHANNELS*) (*i).second;	
		SingleStringKey key(pRow->m_Id);

		char tmp_Id[201];
db_wrapper_real_escape_string(database->m_pDB,tmp_Id, key.pk.c_str(), (unsigned long) key.pk.size());


string condition;
condition = condition + "`Id`=" + "\"" + tmp_Id+ "\"";
	
			
		
string update_values_list;
update_values_list = update_values_list + "`Id`="+pRow->Id_asSQL()+", `Name`="+pRow->Name_asSQL()+", `Frequency`="+pRow->Frequency_asSQL()+", `Parameters`="+pRow->Parameters_asSQL()+", `Source`="+pRow->Source_asSQL()+", `Srate`="+pRow->Srate_asSQL()+", `VPID`="+pRow->VPID_asSQL()+", `APID`="+pRow->APID_asSQL()+", `TPID`="+pRow->TPID_asSQL()+", `CA`="+pRow->CA_asSQL()+", `SID`="+pRow->SID_asSQL()+", `NID`="+pRow->NID_asSQL()+", `TID`="+pRow->TID_asSQL()+", `RID`="+pRow->RID_asSQL()+", `GRP`="+pRow->GRP_asSQL()+", `POS`="+pRow->POS_asSQL();

	
		string query = "update CHANNELS set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_CHANNELS* pRow = (Row_CHANNELS*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleStringKey, class TableRow*, SingleStringKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleStringKey key = (*i).first;
		Row_CHANNELS* pRow = (Row_CHANNELS*) (*i).second;	

		char tmp_Id[201];
db_wrapper_real_escape_string(database->m_pDB,tmp_Id, key.pk.c_str(), (unsigned long) key.pk.size());


string condition;
condition = condition + "`Id`=" + "\"" + tmp_Id+ "\"";

	
		string query = "delete from CHANNELS where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_CHANNELS*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_CHANNELS::GetRows(string where_statement,vector<class Row_CHANNELS*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `CHANNELS`.* from CHANNELS " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `CHANNELS`.* from CHANNELS where " + where_statement;
	else
		query = "select `CHANNELS`.* from CHANNELS";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_CHANNELS *pRow = new Row_CHANNELS(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_Id = "";
}
else
{
pRow->is_null[0]=false;
pRow->m_Id = string(row[0],lengths[0]);
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
pRow->m_Frequency = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Frequency));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Parameters = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Parameters = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Source = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Source = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Srate = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Srate));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_VPID = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_VPID = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_APID = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_APID = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_TPID = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_TPID = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_CA = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_CA = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_SID = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_SID));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_NID = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_NID));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_TID = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_TID));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_RID = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_RID));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_GRP = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_GRP));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_POS = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_POS));
}



		//checking for duplicates

		SingleStringKey key(pRow->m_Id);
		
		map<SingleStringKey, class TableRow*, SingleStringKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_CHANNELS *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_CHANNELS* Table_CHANNELS::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_CHANNELS *pRow = new Row_CHANNELS(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_CHANNELS* Table_CHANNELS::GetRow(string in_Id)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleStringKey row_key(in_Id);

	map<SingleStringKey, class TableRow*, SingleStringKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_CHANNELS*) (*i).second;
	//we have to fetch row
	Row_CHANNELS* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_CHANNELS* Table_CHANNELS::FetchRow(SingleStringKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_Id[201];
db_wrapper_real_escape_string(database->m_pDB,tmp_Id, key.pk.c_str(), (unsigned long) key.pk.size());


string condition;
condition = condition + "`Id`=" + "\"" + tmp_Id+ "\"";


	string query = "select * from CHANNELS where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_CHANNELS::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_CHANNELS *pRow = new Row_CHANNELS(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_Id = "";
}
else
{
pRow->is_null[0]=false;
pRow->m_Id = string(row[0],lengths[0]);
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
pRow->m_Frequency = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_Frequency));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Parameters = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Parameters = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Source = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Source = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Srate = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_Srate));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_VPID = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_VPID = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_APID = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_APID = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_TPID = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_TPID = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_CA = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_CA = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_SID = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_SID));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_NID = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_NID));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_TID = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_TID));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_RID = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_RID));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_GRP = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_GRP));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_POS = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_POS));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}







