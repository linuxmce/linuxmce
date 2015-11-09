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
#include "Table_UnknownDevices.h"
#include "Table_Device.h"



void Database_pluto_main::CreateTable_UnknownDevices()
{
	tblUnknownDevices = new Table_UnknownDevices(this);
}

void Database_pluto_main::DeleteTable_UnknownDevices()
{
	if( tblUnknownDevices )
		delete tblUnknownDevices;
}

bool Database_pluto_main::Commit_UnknownDevices(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblUnknownDevices->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_UnknownDevices::~Table_UnknownDevices()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_UnknownDevices *pRow = (Row_UnknownDevices *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_UnknownDevices *pRow = (Row_UnknownDevices *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_UnknownDevices::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_UnknownDevices *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_UnknownDevices *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_UnknownDevices);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_UnknownDevices::Reload()
{
	Row_UnknownDevices *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_UnknownDevices);
		Row_UnknownDevices *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_UnknownDevices::Row_UnknownDevices(Table_UnknownDevices *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_UnknownDevices::SetDefaultValues()
{
	m_PK_UnknownDevices = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
m_FK_Device_PC = 0;
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

long int Row_UnknownDevices::PK_UnknownDevices_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_UnknownDevices;}
string Row_UnknownDevices::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_UnknownDevices::MacAddress_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MacAddress;}
string Row_UnknownDevices::IPAddress_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IPAddress;}
string Row_UnknownDevices::VendorModelId_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VendorModelId;}
string Row_UnknownDevices::SerialNumber_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SerialNumber;}
long int Row_UnknownDevices::FK_Device_PC_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Device_PC;}
long int Row_UnknownDevices::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_UnknownDevices::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_UnknownDevices::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_UnknownDevices::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_UnknownDevices::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_UnknownDevices::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_UnknownDevices::PK_UnknownDevices_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_UnknownDevices = val; is_modified=true; is_null[0]=false;}
void Row_UnknownDevices::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_UnknownDevices::MacAddress_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MacAddress = val; is_modified=true; is_null[2]=false;}
void Row_UnknownDevices::IPAddress_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IPAddress = val; is_modified=true; is_null[3]=false;}
void Row_UnknownDevices::VendorModelId_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VendorModelId = val; is_modified=true; is_null[4]=false;}
void Row_UnknownDevices::SerialNumber_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SerialNumber = val; is_modified=true; is_null[5]=false;}
void Row_UnknownDevices::FK_Device_PC_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Device_PC = val; is_modified=true; is_null[6]=false;}
void Row_UnknownDevices::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[7]=false;}
void Row_UnknownDevices::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[8]=false;}
void Row_UnknownDevices::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[9]=false;}
void Row_UnknownDevices::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[10]=false;}
void Row_UnknownDevices::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[11]=false;}
void Row_UnknownDevices::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[12]=false;}

		
bool Row_UnknownDevices::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_UnknownDevices::MacAddress_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_UnknownDevices::IPAddress_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_UnknownDevices::VendorModelId_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_UnknownDevices::SerialNumber_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_UnknownDevices::FK_Device_PC_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_UnknownDevices::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_UnknownDevices::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_UnknownDevices::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_UnknownDevices::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_UnknownDevices::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}

			
void Row_UnknownDevices::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_UnknownDevices::MacAddress_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_UnknownDevices::IPAddress_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_UnknownDevices::VendorModelId_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_UnknownDevices::SerialNumber_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_UnknownDevices::FK_Device_PC_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_UnknownDevices::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_UnknownDevices::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_UnknownDevices::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_UnknownDevices::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_UnknownDevices::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
	

string Row_UnknownDevices::PK_UnknownDevices_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_UnknownDevices);

return buf;
}

string Row_UnknownDevices::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)30,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_UnknownDevices::MacAddress_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[35];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_MacAddress.c_str(), (unsigned long) min((size_t)17,m_MacAddress.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_UnknownDevices::IPAddress_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[31];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_IPAddress.c_str(), (unsigned long) min((size_t)15,m_IPAddress.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_UnknownDevices::VendorModelId_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_VendorModelId.c_str(), (unsigned long) min((size_t)60,m_VendorModelId.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_UnknownDevices::SerialNumber_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_SerialNumber.c_str(), (unsigned long) min((size_t)255,m_SerialNumber.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_UnknownDevices::FK_Device_PC_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Device_PC);

return buf;
}

string Row_UnknownDevices::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_UnknownDevices::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_UnknownDevices::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_UnknownDevices::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_UnknownDevices::psc_mod_asSQL()
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

string Row_UnknownDevices::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_UnknownDevices::Key::Key(long int in_PK_UnknownDevices)
{
			pk_PK_UnknownDevices = in_PK_UnknownDevices;
	
}

Table_UnknownDevices::Key::Key(Row_UnknownDevices *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_UnknownDevices = pRow->m_PK_UnknownDevices;
	
}		

bool Table_UnknownDevices::Key_Less::operator()(const Table_UnknownDevices::Key &key1, const Table_UnknownDevices::Key &key2) const
{
			if (key1.pk_PK_UnknownDevices!=key2.pk_PK_UnknownDevices)
return key1.pk_PK_UnknownDevices<key2.pk_PK_UnknownDevices;
else
return false;	
}	

bool Table_UnknownDevices::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_UnknownDevices *pRow = (Row_UnknownDevices *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_UnknownDevices_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->MacAddress_asSQL()+", "+pRow->IPAddress_asSQL()+", "+pRow->VendorModelId_asSQL()+", "+pRow->SerialNumber_asSQL()+", "+pRow->FK_Device_PC_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into UnknownDevices (`PK_UnknownDevices`, `Description`, `MacAddress`, `IPAddress`, `VendorModelId`, `SerialNumber`, `FK_Device_PC`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_UnknownDevices=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_UnknownDevices is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_UnknownDevices);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_UnknownDevices* pRow = (Row_UnknownDevices*) (*i).second;	
		SingleLongKey key(pRow->m_PK_UnknownDevices);

		char tmp_PK_UnknownDevices[32];
sprintf(tmp_PK_UnknownDevices, "%li", key.pk);


string condition;
condition = condition + "`PK_UnknownDevices`=" + tmp_PK_UnknownDevices;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_UnknownDevices`="+pRow->PK_UnknownDevices_asSQL()+", `Description`="+pRow->Description_asSQL()+", `MacAddress`="+pRow->MacAddress_asSQL()+", `IPAddress`="+pRow->IPAddress_asSQL()+", `VendorModelId`="+pRow->VendorModelId_asSQL()+", `SerialNumber`="+pRow->SerialNumber_asSQL()+", `FK_Device_PC`="+pRow->FK_Device_PC_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update UnknownDevices set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_UnknownDevices* pRow = (Row_UnknownDevices*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_UnknownDevices* pRow = (Row_UnknownDevices*) (*i).second;	

		char tmp_PK_UnknownDevices[32];
sprintf(tmp_PK_UnknownDevices, "%li", key.pk);


string condition;
condition = condition + "`PK_UnknownDevices`=" + tmp_PK_UnknownDevices;

	
		string query = "delete from UnknownDevices where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_UnknownDevices*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_UnknownDevices::GetRows(string where_statement,vector<class Row_UnknownDevices*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `UnknownDevices`.* from UnknownDevices " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `UnknownDevices`.* from UnknownDevices where " + where_statement;
	else
		query = "select `UnknownDevices`.* from UnknownDevices";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_UnknownDevices *pRow = new Row_UnknownDevices(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_UnknownDevices = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_UnknownDevices));
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
pRow->m_MacAddress = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_MacAddress = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IPAddress = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_IPAddress = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_VendorModelId = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_VendorModelId = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SerialNumber = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_SerialNumber = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Device_PC = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Device_PC));
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

		SingleLongKey key(pRow->m_PK_UnknownDevices);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_UnknownDevices *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_UnknownDevices* Table_UnknownDevices::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_UnknownDevices *pRow = new Row_UnknownDevices(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_UnknownDevices* Table_UnknownDevices::GetRow(long int in_PK_UnknownDevices)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_UnknownDevices);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_UnknownDevices*) (*i).second;
	//we have to fetch row
	Row_UnknownDevices* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_UnknownDevices* Table_UnknownDevices::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_UnknownDevices[32];
sprintf(tmp_PK_UnknownDevices, "%li", key.pk);


string condition;
condition = condition + "`PK_UnknownDevices`=" + tmp_PK_UnknownDevices;


	string query = "select * from UnknownDevices where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_UnknownDevices::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_UnknownDevices *pRow = new Row_UnknownDevices(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_UnknownDevices = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_UnknownDevices));
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
pRow->m_MacAddress = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_MacAddress = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_IPAddress = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_IPAddress = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_VendorModelId = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_VendorModelId = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SerialNumber = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_SerialNumber = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Device_PC = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Device_PC));
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


class Row_Device* Row_UnknownDevices::FK_Device_PC_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
return pTable->GetRow(m_FK_Device_PC);
}





