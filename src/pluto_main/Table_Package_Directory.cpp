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
#include "Table_Package_Directory.h"
#include "Table_Package.h"
#include "Table_Directory.h"
#include "Table_OperatingSystem.h"
#include "Table_Distro.h"

#include "Table_Package_Directory_File.h"


void Database_pluto_main::CreateTable_Package_Directory()
{
	tblPackage_Directory = new Table_Package_Directory(this);
}

void Database_pluto_main::DeleteTable_Package_Directory()
{
	if( tblPackage_Directory )
		delete tblPackage_Directory;
}

bool Database_pluto_main::Commit_Package_Directory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblPackage_Directory->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Package_Directory::~Table_Package_Directory()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Package_Directory *pRow = (Row_Package_Directory *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Package_Directory *pRow = (Row_Package_Directory *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Package_Directory::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Package_Directory *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Package_Directory *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Package_Directory);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Package_Directory::Reload()
{
	Row_Package_Directory *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Package_Directory);
		Row_Package_Directory *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_Directory::Row_Package_Directory(Table_Package_Directory *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_Directory::SetDefaultValues()
{
	m_PK_Package_Directory = 0;
is_null[0] = false;
m_FK_Package = 0;
is_null[1] = false;
m_FK_Directory = 0;
is_null[2] = false;
is_null[3] = true;
m_FK_OperatingSystem = 0;
is_null[4] = true;
m_FK_Distro = 0;
m_Path = "";
is_null[5] = false;
is_null[6] = true;
m_FlipSource = 0;
is_null[7] = false;
m_GenerateDoxygen = 0;
is_null[8] = false;
is_null[9] = true;
m_psc_id = 0;
is_null[10] = true;
m_psc_batch = 0;
is_null[11] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[12] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[13] = false;
is_null[14] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Package_Directory::PK_Package_Directory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Package_Directory;}
long int Row_Package_Directory::FK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Package;}
long int Row_Package_Directory::FK_Directory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Directory;}
long int Row_Package_Directory::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_OperatingSystem;}
long int Row_Package_Directory::FK_Distro_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Distro;}
string Row_Package_Directory::Path_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Path;}
string Row_Package_Directory::InputPath_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_InputPath;}
short int Row_Package_Directory::FlipSource_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FlipSource;}
short int Row_Package_Directory::GenerateDoxygen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_GenerateDoxygen;}
long int Row_Package_Directory::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Package_Directory::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Package_Directory::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Package_Directory::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Package_Directory::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Package_Directory::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Package_Directory::PK_Package_Directory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Package_Directory = val; is_modified=true; is_null[0]=false;}
void Row_Package_Directory::FK_Package_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Package = val; is_modified=true; is_null[1]=false;}
void Row_Package_Directory::FK_Directory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Directory = val; is_modified=true; is_null[2]=false;}
void Row_Package_Directory::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[3]=false;}
void Row_Package_Directory::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Distro = val; is_modified=true; is_null[4]=false;}
void Row_Package_Directory::Path_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Path = val; is_modified=true; is_null[5]=false;}
void Row_Package_Directory::InputPath_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_InputPath = val; is_modified=true; is_null[6]=false;}
void Row_Package_Directory::FlipSource_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FlipSource = val; is_modified=true; is_null[7]=false;}
void Row_Package_Directory::GenerateDoxygen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_GenerateDoxygen = val; is_modified=true; is_null[8]=false;}
void Row_Package_Directory::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[9]=false;}
void Row_Package_Directory::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[10]=false;}
void Row_Package_Directory::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[11]=false;}
void Row_Package_Directory::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[12]=false;}
void Row_Package_Directory::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[13]=false;}
void Row_Package_Directory::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[14]=false;}

		
bool Row_Package_Directory::FK_OperatingSystem_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Package_Directory::FK_Distro_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Package_Directory::InputPath_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Package_Directory::GenerateDoxygen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Package_Directory::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Package_Directory::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Package_Directory::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Package_Directory::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Package_Directory::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}

			
void Row_Package_Directory::FK_OperatingSystem_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Package_Directory::FK_Distro_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Package_Directory::InputPath_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Package_Directory::GenerateDoxygen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Package_Directory::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Package_Directory::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Package_Directory::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Package_Directory::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Package_Directory::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
	

string Row_Package_Directory::PK_Package_Directory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Package_Directory);

return buf;
}

string Row_Package_Directory::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_Package_Directory::FK_Directory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Directory);

return buf;
}

string Row_Package_Directory::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}

string Row_Package_Directory::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_Package_Directory::Path_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Path.c_str(), (unsigned long) min((size_t)765,m_Path.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Package_Directory::InputPath_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_InputPath.c_str(), (unsigned long) min((size_t)765,m_InputPath.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Package_Directory::FlipSource_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FlipSource);

return buf;
}

string Row_Package_Directory::GenerateDoxygen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_GenerateDoxygen);

return buf;
}

string Row_Package_Directory::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Package_Directory::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Package_Directory::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Package_Directory::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Package_Directory::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Package_Directory::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Package_Directory::Key::Key(long int in_PK_Package_Directory)
{
			pk_PK_Package_Directory = in_PK_Package_Directory;
	
}

Table_Package_Directory::Key::Key(Row_Package_Directory *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Package_Directory = pRow->m_PK_Package_Directory;
	
}		

bool Table_Package_Directory::Key_Less::operator()(const Table_Package_Directory::Key &key1, const Table_Package_Directory::Key &key2) const
{
			if (key1.pk_PK_Package_Directory!=key2.pk_PK_Package_Directory)
return key1.pk_PK_Package_Directory<key2.pk_PK_Package_Directory;
else
return false;	
}	

bool Table_Package_Directory::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Package_Directory *pRow = (Row_Package_Directory *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Package_Directory_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->FK_Directory_asSQL()+", "+pRow->FK_OperatingSystem_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->Path_asSQL()+", "+pRow->InputPath_asSQL()+", "+pRow->FlipSource_asSQL()+", "+pRow->GenerateDoxygen_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Package_Directory (`PK_Package_Directory`, `FK_Package`, `FK_Directory`, `FK_OperatingSystem`, `FK_Distro`, `Path`, `InputPath`, `FlipSource`, `GenerateDoxygen`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Package_Directory=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Package_Directory is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Package_Directory);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Package_Directory* pRow = (Row_Package_Directory*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Package_Directory);

		char tmp_PK_Package_Directory[32];
sprintf(tmp_PK_Package_Directory, "%li", key.pk);


string condition;
condition = condition + "`PK_Package_Directory`=" + tmp_PK_Package_Directory;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Package_Directory`="+pRow->PK_Package_Directory_asSQL()+", `FK_Package`="+pRow->FK_Package_asSQL()+", `FK_Directory`="+pRow->FK_Directory_asSQL()+", `FK_OperatingSystem`="+pRow->FK_OperatingSystem_asSQL()+", `FK_Distro`="+pRow->FK_Distro_asSQL()+", `Path`="+pRow->Path_asSQL()+", `InputPath`="+pRow->InputPath_asSQL()+", `FlipSource`="+pRow->FlipSource_asSQL()+", `GenerateDoxygen`="+pRow->GenerateDoxygen_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Package_Directory set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Package_Directory* pRow = (Row_Package_Directory*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Package_Directory* pRow = (Row_Package_Directory*) (*i).second;	

		char tmp_PK_Package_Directory[32];
sprintf(tmp_PK_Package_Directory, "%li", key.pk);


string condition;
condition = condition + "`PK_Package_Directory`=" + tmp_PK_Package_Directory;

	
		string query = "delete from Package_Directory where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Package_Directory*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Package_Directory::GetRows(string where_statement,vector<class Row_Package_Directory*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Package_Directory`.* from Package_Directory " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Package_Directory`.* from Package_Directory where " + where_statement;
	else
		query = "select `Package_Directory`.* from Package_Directory";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Package_Directory *pRow = new Row_Package_Directory(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Package_Directory = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Package_Directory));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Directory = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Directory));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Distro));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Path = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_InputPath = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_InputPath = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FlipSource = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FlipSource));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_GenerateDoxygen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_GenerateDoxygen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_id));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_batch));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_user));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_psc_frozen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_psc_mod = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Package_Directory);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Package_Directory *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Package_Directory* Table_Package_Directory::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Package_Directory *pRow = new Row_Package_Directory(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_Directory* Table_Package_Directory::GetRow(long int in_PK_Package_Directory)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Package_Directory);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Package_Directory*) (*i).second;
	//we have to fetch row
	Row_Package_Directory* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_Directory* Table_Package_Directory::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Package_Directory[32];
sprintf(tmp_PK_Package_Directory, "%li", key.pk);


string condition;
condition = condition + "`PK_Package_Directory`=" + tmp_PK_Package_Directory;


	string query = "select * from Package_Directory where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_Directory::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Package_Directory *pRow = new Row_Package_Directory(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Package_Directory = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Package_Directory));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Package));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Directory = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Directory));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Distro));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Path = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Path = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_InputPath = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_InputPath = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FlipSource = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FlipSource));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_GenerateDoxygen = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_GenerateDoxygen));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_psc_id));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_batch));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_user));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_psc_frozen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_psc_mod = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Package* Row_Package_Directory::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Directory* Row_Package_Directory::FK_Directory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Directory *pTable = table->database->Directory_get();
return pTable->GetRow(m_FK_Directory);
}
class Row_OperatingSystem* Row_Package_Directory::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}
class Row_Distro* Row_Package_Directory::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}


void Row_Package_Directory::Package_Directory_File_FK_Package_Directory_getrows(vector <class Row_Package_Directory_File*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package_Directory_File *pTable = table->database->Package_Directory_File_get();
pTable->GetRows("`FK_Package_Directory`=" + StringUtils::itos(m_PK_Package_Directory),rows);
}



