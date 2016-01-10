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
#include "Table_DeviceTemplate_DeviceData.h"
#include "Table_DeviceTemplate.h"
#include "Table_DeviceData.h"



void Database_pluto_main::CreateTable_DeviceTemplate_DeviceData()
{
	tblDeviceTemplate_DeviceData = new Table_DeviceTemplate_DeviceData(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_DeviceData()
{
	if( tblDeviceTemplate_DeviceData )
		delete tblDeviceTemplate_DeviceData;
}

bool Database_pluto_main::Commit_DeviceTemplate_DeviceData(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDeviceTemplate_DeviceData->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DeviceTemplate_DeviceData::~Table_DeviceTemplate_DeviceData()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DeviceTemplate_DeviceData *pRow = (Row_DeviceTemplate_DeviceData *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DeviceTemplate_DeviceData *pRow = (Row_DeviceTemplate_DeviceData *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DeviceTemplate_DeviceData::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DeviceTemplate_DeviceData *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DeviceTemplate_DeviceData *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_FK_DeviceTemplate,pRow->m_FK_DeviceData);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_DeviceTemplate_DeviceData::Reload()
{
	Row_DeviceTemplate_DeviceData *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_FK_DeviceTemplate,pRow->m_FK_DeviceData);
		Row_DeviceTemplate_DeviceData *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_DeviceData::Row_DeviceTemplate_DeviceData(Table_DeviceTemplate_DeviceData *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_DeviceData::SetDefaultValues()
{
	m_FK_DeviceTemplate = 0;
is_null[0] = false;
m_FK_DeviceData = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
m_Required = 0;
is_null[4] = true;
m_AllowedToModify = 0;
m_SetByDevice = 0;
is_null[5] = false;
is_null[6] = true;
m_UseDeviceTemplateDefault = 0;
is_null[7] = false;
m_ShowInWizard = 1;
is_null[8] = false;
is_null[9] = true;
is_null[10] = true;
m_psc_id = 0;
is_null[11] = true;
m_psc_batch = 0;
is_null[12] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[13] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[14] = false;
is_null[15] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DeviceTemplate_DeviceData::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceTemplate;}
long int Row_DeviceTemplate_DeviceData::FK_DeviceData_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceData;}
string Row_DeviceTemplate_DeviceData::IK_DeviceData_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IK_DeviceData;}
short int Row_DeviceTemplate_DeviceData::Required_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Required;}
short int Row_DeviceTemplate_DeviceData::AllowedToModify_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AllowedToModify;}
short int Row_DeviceTemplate_DeviceData::SetByDevice_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SetByDevice;}
string Row_DeviceTemplate_DeviceData::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
short int Row_DeviceTemplate_DeviceData::UseDeviceTemplateDefault_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UseDeviceTemplateDefault;}
short int Row_DeviceTemplate_DeviceData::ShowInWizard_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShowInWizard;}
string Row_DeviceTemplate_DeviceData::ShortDescription_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShortDescription;}
long int Row_DeviceTemplate_DeviceData::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DeviceTemplate_DeviceData::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DeviceTemplate_DeviceData::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DeviceTemplate_DeviceData::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DeviceTemplate_DeviceData::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DeviceTemplate_DeviceData::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DeviceTemplate_DeviceData::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_DeviceData::FK_DeviceData_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceData = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_DeviceData::IK_DeviceData_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IK_DeviceData = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_DeviceData::Required_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Required = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_DeviceData::AllowedToModify_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AllowedToModify = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_DeviceData::SetByDevice_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SetByDevice = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_DeviceData::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate_DeviceData::UseDeviceTemplateDefault_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UseDeviceTemplateDefault = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate_DeviceData::ShowInWizard_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShowInWizard = val; is_modified=true; is_null[8]=false;}
void Row_DeviceTemplate_DeviceData::ShortDescription_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShortDescription = val; is_modified=true; is_null[9]=false;}
void Row_DeviceTemplate_DeviceData::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[10]=false;}
void Row_DeviceTemplate_DeviceData::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[11]=false;}
void Row_DeviceTemplate_DeviceData::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[12]=false;}
void Row_DeviceTemplate_DeviceData::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[13]=false;}
void Row_DeviceTemplate_DeviceData::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[14]=false;}
void Row_DeviceTemplate_DeviceData::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[15]=false;}

		
bool Row_DeviceTemplate_DeviceData::IK_DeviceData_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DeviceTemplate_DeviceData::Required_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DeviceTemplate_DeviceData::AllowedToModify_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DeviceTemplate_DeviceData::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_DeviceTemplate_DeviceData::ShortDescription_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DeviceTemplate_DeviceData::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DeviceTemplate_DeviceData::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DeviceTemplate_DeviceData::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DeviceTemplate_DeviceData::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DeviceTemplate_DeviceData::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}

			
void Row_DeviceTemplate_DeviceData::IK_DeviceData_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::Required_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::AllowedToModify_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::ShortDescription_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DeviceTemplate_DeviceData::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
	

string Row_DeviceTemplate_DeviceData::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate_DeviceData::FK_DeviceData_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceData);

return buf;
}

string Row_DeviceTemplate_DeviceData::IK_DeviceData_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_IK_DeviceData.c_str(), (unsigned long) min((size_t)65535,m_IK_DeviceData.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_DeviceData::Required_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Required);

return buf;
}

string Row_DeviceTemplate_DeviceData::AllowedToModify_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AllowedToModify);

return buf;
}

string Row_DeviceTemplate_DeviceData::SetByDevice_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_SetByDevice);

return buf;
}

string Row_DeviceTemplate_DeviceData::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)65535,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_DeviceData::UseDeviceTemplateDefault_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_UseDeviceTemplateDefault);

return buf;
}

string Row_DeviceTemplate_DeviceData::ShowInWizard_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ShowInWizard);

return buf;
}

string Row_DeviceTemplate_DeviceData::ShortDescription_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[51];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ShortDescription.c_str(), (unsigned long) min((size_t)25,m_ShortDescription.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_DeviceData::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceTemplate_DeviceData::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceTemplate_DeviceData::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceTemplate_DeviceData::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceTemplate_DeviceData::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_DeviceData::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DeviceTemplate_DeviceData::Key::Key(long int in_FK_DeviceTemplate, long int in_FK_DeviceData)
{
			pk_FK_DeviceTemplate = in_FK_DeviceTemplate;
pk_FK_DeviceData = in_FK_DeviceData;
	
}

Table_DeviceTemplate_DeviceData::Key::Key(Row_DeviceTemplate_DeviceData *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_FK_DeviceTemplate = pRow->m_FK_DeviceTemplate;
pk_FK_DeviceData = pRow->m_FK_DeviceData;
	
}		

bool Table_DeviceTemplate_DeviceData::Key_Less::operator()(const Table_DeviceTemplate_DeviceData::Key &key1, const Table_DeviceTemplate_DeviceData::Key &key2) const
{
			if (key1.pk_FK_DeviceTemplate!=key2.pk_FK_DeviceTemplate)
return key1.pk_FK_DeviceTemplate<key2.pk_FK_DeviceTemplate;
else
if (key1.pk_FK_DeviceData!=key2.pk_FK_DeviceData)
return key1.pk_FK_DeviceData<key2.pk_FK_DeviceData;
else
return false;	
}	

bool Table_DeviceTemplate_DeviceData::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_DeviceData *pRow = (Row_DeviceTemplate_DeviceData *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->FK_DeviceTemplate_asSQL()+", "+pRow->FK_DeviceData_asSQL()+", "+pRow->IK_DeviceData_asSQL()+", "+pRow->Required_asSQL()+", "+pRow->AllowedToModify_asSQL()+", "+pRow->SetByDevice_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->UseDeviceTemplateDefault_asSQL()+", "+pRow->ShowInWizard_asSQL()+", "+pRow->ShortDescription_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DeviceTemplate_DeviceData (`FK_DeviceTemplate`, `FK_DeviceData`, `IK_DeviceData`, `Required`, `AllowedToModify`, `SetByDevice`, `Description`, `UseDeviceTemplateDefault`, `ShowInWizard`, `ShortDescription`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			DoubleLongKey key(pRow->m_FK_DeviceTemplate,pRow->m_FK_DeviceData);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DeviceTemplate_DeviceData* pRow = (Row_DeviceTemplate_DeviceData*) (*i).second;	
		DoubleLongKey key(pRow->m_FK_DeviceTemplate,pRow->m_FK_DeviceData);

		char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk1);

char tmp_FK_DeviceData[32];
sprintf(tmp_FK_DeviceData, "%li", key.pk2);


string condition;
condition = condition + "`FK_DeviceTemplate`=" + tmp_FK_DeviceTemplate+" AND "+"`FK_DeviceData`=" + tmp_FK_DeviceData;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `FK_DeviceData`="+pRow->FK_DeviceData_asSQL()+", `IK_DeviceData`="+pRow->IK_DeviceData_asSQL()+", `Required`="+pRow->Required_asSQL()+", `AllowedToModify`="+pRow->AllowedToModify_asSQL()+", `SetByDevice`="+pRow->SetByDevice_asSQL()+", `Description`="+pRow->Description_asSQL()+", `UseDeviceTemplateDefault`="+pRow->UseDeviceTemplateDefault_asSQL()+", `ShowInWizard`="+pRow->ShowInWizard_asSQL()+", `ShortDescription`="+pRow->ShortDescription_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DeviceTemplate_DeviceData set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DeviceTemplate_DeviceData* pRow = (Row_DeviceTemplate_DeviceData*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DeviceTemplate_DeviceData* pRow = (Row_DeviceTemplate_DeviceData*) (*i).second;	

		char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk1);

char tmp_FK_DeviceData[32];
sprintf(tmp_FK_DeviceData, "%li", key.pk2);


string condition;
condition = condition + "`FK_DeviceTemplate`=" + tmp_FK_DeviceTemplate+" AND "+"`FK_DeviceData`=" + tmp_FK_DeviceData;

	
		string query = "delete from DeviceTemplate_DeviceData where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DeviceTemplate_DeviceData*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DeviceTemplate_DeviceData::GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceData*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DeviceTemplate_DeviceData`.* from DeviceTemplate_DeviceData " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DeviceTemplate_DeviceData`.* from DeviceTemplate_DeviceData where " + where_statement;
	else
		query = "select `DeviceTemplate_DeviceData`.* from DeviceTemplate_DeviceData";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DeviceTemplate_DeviceData *pRow = new Row_DeviceTemplate_DeviceData(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DeviceData = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DeviceData));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_IK_DeviceData = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_IK_DeviceData = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Required = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Required));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_AllowedToModify = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_AllowedToModify));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SetByDevice = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_SetByDevice));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Description = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_UseDeviceTemplateDefault = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_UseDeviceTemplateDefault));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_ShowInWizard = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_ShowInWizard));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_ShortDescription = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_ShortDescription = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_id));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_batch));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_user));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_psc_frozen));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_psc_mod = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_FK_DeviceTemplate,pRow->m_FK_DeviceData);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DeviceTemplate_DeviceData *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DeviceTemplate_DeviceData* Table_DeviceTemplate_DeviceData::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DeviceTemplate_DeviceData *pRow = new Row_DeviceTemplate_DeviceData(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_DeviceData* Table_DeviceTemplate_DeviceData::GetRow(long int in_FK_DeviceTemplate, long int in_FK_DeviceData)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	DoubleLongKey row_key(in_FK_DeviceTemplate, in_FK_DeviceData);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DeviceTemplate_DeviceData*) (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_DeviceData* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_DeviceData* Table_DeviceTemplate_DeviceData::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_FK_DeviceTemplate[32];
sprintf(tmp_FK_DeviceTemplate, "%li", key.pk1);

char tmp_FK_DeviceData[32];
sprintf(tmp_FK_DeviceData, "%li", key.pk2);


string condition;
condition = condition + "`FK_DeviceTemplate`=" + tmp_FK_DeviceTemplate+" AND "+"`FK_DeviceData`=" + tmp_FK_DeviceData;


	string query = "select * from DeviceTemplate_DeviceData where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_DeviceData::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DeviceTemplate_DeviceData *pRow = new Row_DeviceTemplate_DeviceData(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DeviceData = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DeviceData));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_IK_DeviceData = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_IK_DeviceData = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Required = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Required));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_AllowedToModify = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_AllowedToModify));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SetByDevice = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_SetByDevice));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Description = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_UseDeviceTemplateDefault = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_UseDeviceTemplateDefault));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_ShowInWizard = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_ShowInWizard));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_ShortDescription = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_ShortDescription = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_psc_id));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_batch));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_user));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_psc_frozen));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_psc_mod = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DeviceTemplate* Row_DeviceTemplate_DeviceData::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_DeviceData* Row_DeviceTemplate_DeviceData::FK_DeviceData_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceData *pTable = table->database->DeviceData_get();
return pTable->GetRow(m_FK_DeviceData);
}





