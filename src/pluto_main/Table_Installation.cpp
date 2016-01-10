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
#include "Table_Installation.h"
#include "Table_Country.h"
#include "Table_City.h"
#include "Table_PostalCode.h"
#include "Table_Version.h"
#include "Table_RepositoryType.h"
#include "Table_RepositoryType.h"

#include "Table_CommandGroup.h"
#include "Table_Criteria.h"
#include "Table_Device.h"
#include "Table_DeviceGroup.h"
#include "Table_EventHandler.h"
#include "Table_Floorplan.h"
#include "Table_Household_Installation.h"
#include "Table_InfraredGroup_Command_Preferred.h"
#include "Table_Installation_RepositorySource_URL.h"
#include "Table_Installation_Users.h"
#include "Table_Room.h"
#include "Table_SetupStep.h"
#include "Table_Users.h"


void Database_pluto_main::CreateTable_Installation()
{
	tblInstallation = new Table_Installation(this);
}

void Database_pluto_main::DeleteTable_Installation()
{
	if( tblInstallation )
		delete tblInstallation;
}

bool Database_pluto_main::Commit_Installation(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblInstallation->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Installation::~Table_Installation()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Installation *pRow = (Row_Installation *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Installation *pRow = (Row_Installation *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Installation::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Installation *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Installation *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Installation);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Installation::Reload()
{
	Row_Installation *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Installation);
		Row_Installation *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Installation::Row_Installation(Table_Installation *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Installation::SetDefaultValues()
{
	m_PK_Installation = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
m_FK_Country = 0;
is_null[8] = true;
m_FK_City = 0;
is_null[9] = true;
m_FK_PostalCode = 0;
is_null[10] = true;
is_null[11] = true;
is_null[12] = true;
m_isActive = 0;
is_null[13] = false;
is_null[14] = true;
m_FK_Version = 0;
is_null[15] = true;
m_isMonitored = 0;
m_FK_RepositoryType_Source = 4;
is_null[16] = false;
m_FK_RepositoryType_Binaries = 1;
is_null[17] = false;
is_null[18] = true;
m_psc_id = 0;
is_null[19] = true;
m_psc_batch = 0;
is_null[20] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[21] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[22] = false;
is_null[23] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Installation::PK_Installation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Installation;}
string Row_Installation::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_Installation::Name_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Name;}
string Row_Installation::Address_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Address;}
string Row_Installation::City_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_City;}
string Row_Installation::State_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_State;}
string Row_Installation::Zip_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Zip;}
long int Row_Installation::FK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Country;}
long int Row_Installation::FK_City_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_City;}
long int Row_Installation::FK_PostalCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PostalCode;}
string Row_Installation::ActivationCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ActivationCode;}
string Row_Installation::LastStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_LastStatus;}
string Row_Installation::LastAlive_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_LastAlive;}
short int Row_Installation::isActive_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_isActive;}
long int Row_Installation::FK_Version_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Version;}
short int Row_Installation::isMonitored_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_isMonitored;}
long int Row_Installation::FK_RepositoryType_Source_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_RepositoryType_Source;}
long int Row_Installation::FK_RepositoryType_Binaries_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_RepositoryType_Binaries;}
long int Row_Installation::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Installation::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Installation::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Installation::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Installation::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Installation::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Installation::PK_Installation_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Installation = val; is_modified=true; is_null[0]=false;}
void Row_Installation::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Installation::Name_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Name = val; is_modified=true; is_null[2]=false;}
void Row_Installation::Address_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Address = val; is_modified=true; is_null[3]=false;}
void Row_Installation::City_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_City = val; is_modified=true; is_null[4]=false;}
void Row_Installation::State_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_State = val; is_modified=true; is_null[5]=false;}
void Row_Installation::Zip_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Zip = val; is_modified=true; is_null[6]=false;}
void Row_Installation::FK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Country = val; is_modified=true; is_null[7]=false;}
void Row_Installation::FK_City_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_City = val; is_modified=true; is_null[8]=false;}
void Row_Installation::FK_PostalCode_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PostalCode = val; is_modified=true; is_null[9]=false;}
void Row_Installation::ActivationCode_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ActivationCode = val; is_modified=true; is_null[10]=false;}
void Row_Installation::LastStatus_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_LastStatus = val; is_modified=true; is_null[11]=false;}
void Row_Installation::LastAlive_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_LastAlive = val; is_modified=true; is_null[12]=false;}
void Row_Installation::isActive_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_isActive = val; is_modified=true; is_null[13]=false;}
void Row_Installation::FK_Version_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Version = val; is_modified=true; is_null[14]=false;}
void Row_Installation::isMonitored_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_isMonitored = val; is_modified=true; is_null[15]=false;}
void Row_Installation::FK_RepositoryType_Source_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_RepositoryType_Source = val; is_modified=true; is_null[16]=false;}
void Row_Installation::FK_RepositoryType_Binaries_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_RepositoryType_Binaries = val; is_modified=true; is_null[17]=false;}
void Row_Installation::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[18]=false;}
void Row_Installation::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[19]=false;}
void Row_Installation::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[20]=false;}
void Row_Installation::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[21]=false;}
void Row_Installation::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[22]=false;}
void Row_Installation::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[23]=false;}

		
bool Row_Installation::Name_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Installation::Address_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Installation::City_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Installation::State_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Installation::Zip_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Installation::FK_Country_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Installation::FK_City_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Installation::FK_PostalCode_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Installation::ActivationCode_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Installation::LastStatus_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Installation::LastAlive_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Installation::FK_Version_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_Installation::isMonitored_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Installation::FK_RepositoryType_Source_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_Installation::FK_RepositoryType_Binaries_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_Installation::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_Installation::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_Installation::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_Installation::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_Installation::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}

			
void Row_Installation::Name_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Installation::Address_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Installation::City_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Installation::State_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Installation::Zip_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Installation::FK_Country_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Installation::FK_City_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Installation::FK_PostalCode_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Installation::ActivationCode_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Installation::LastStatus_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Installation::LastAlive_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Installation::FK_Version_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Installation::isMonitored_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Installation::FK_RepositoryType_Source_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_Installation::FK_RepositoryType_Binaries_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Installation::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_Installation::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Installation::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Installation::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_Installation::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
	

string Row_Installation::PK_Installation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Installation);

return buf;
}

string Row_Installation::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)20,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::Name_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Name.c_str(), (unsigned long) min((size_t)30,m_Name.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::Address_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Address.c_str(), (unsigned long) min((size_t)50,m_Address.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::City_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_City.c_str(), (unsigned long) min((size_t)20,m_City.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::State_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_State.c_str(), (unsigned long) min((size_t)10,m_State.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::Zip_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Zip.c_str(), (unsigned long) min((size_t)10,m_Zip.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::FK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Country);

return buf;
}

string Row_Installation::FK_City_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_City);

return buf;
}

string Row_Installation::FK_PostalCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_PostalCode);

return buf;
}

string Row_Installation::ActivationCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ActivationCode.c_str(), (unsigned long) min((size_t)50,m_ActivationCode.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::LastStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_LastStatus.c_str(), (unsigned long) min((size_t)65535,m_LastStatus.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::LastAlive_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_LastAlive.c_str(), (unsigned long) min((size_t)19,m_LastAlive.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::isActive_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_isActive);

return buf;
}

string Row_Installation::FK_Version_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Version);

return buf;
}

string Row_Installation::isMonitored_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_isMonitored);

return buf;
}

string Row_Installation::FK_RepositoryType_Source_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RepositoryType_Source);

return buf;
}

string Row_Installation::FK_RepositoryType_Binaries_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_RepositoryType_Binaries);

return buf;
}

string Row_Installation::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Installation::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Installation::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Installation::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Installation::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Installation::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Installation::Key::Key(long int in_PK_Installation)
{
			pk_PK_Installation = in_PK_Installation;
	
}

Table_Installation::Key::Key(Row_Installation *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Installation = pRow->m_PK_Installation;
	
}		

bool Table_Installation::Key_Less::operator()(const Table_Installation::Key &key1, const Table_Installation::Key &key2) const
{
			if (key1.pk_PK_Installation!=key2.pk_PK_Installation)
return key1.pk_PK_Installation<key2.pk_PK_Installation;
else
return false;	
}	

bool Table_Installation::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Installation *pRow = (Row_Installation *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Installation_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Name_asSQL()+", "+pRow->Address_asSQL()+", "+pRow->City_asSQL()+", "+pRow->State_asSQL()+", "+pRow->Zip_asSQL()+", "+pRow->FK_Country_asSQL()+", "+pRow->FK_City_asSQL()+", "+pRow->FK_PostalCode_asSQL()+", "+pRow->ActivationCode_asSQL()+", "+pRow->LastStatus_asSQL()+", "+pRow->LastAlive_asSQL()+", "+pRow->isActive_asSQL()+", "+pRow->FK_Version_asSQL()+", "+pRow->isMonitored_asSQL()+", "+pRow->FK_RepositoryType_Source_asSQL()+", "+pRow->FK_RepositoryType_Binaries_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Installation (`PK_Installation`, `Description`, `Name`, `Address`, `City`, `State`, `Zip`, `FK_Country`, `FK_City`, `FK_PostalCode`, `ActivationCode`, `LastStatus`, `LastAlive`, `isActive`, `FK_Version`, `isMonitored`, `FK_RepositoryType_Source`, `FK_RepositoryType_Binaries`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Installation=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Installation is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Installation);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Installation* pRow = (Row_Installation*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Installation);

		char tmp_PK_Installation[32];
sprintf(tmp_PK_Installation, "%li", key.pk);


string condition;
condition = condition + "`PK_Installation`=" + tmp_PK_Installation;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Installation`="+pRow->PK_Installation_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Name`="+pRow->Name_asSQL()+", `Address`="+pRow->Address_asSQL()+", `City`="+pRow->City_asSQL()+", `State`="+pRow->State_asSQL()+", `Zip`="+pRow->Zip_asSQL()+", `FK_Country`="+pRow->FK_Country_asSQL()+", `FK_City`="+pRow->FK_City_asSQL()+", `FK_PostalCode`="+pRow->FK_PostalCode_asSQL()+", `ActivationCode`="+pRow->ActivationCode_asSQL()+", `LastStatus`="+pRow->LastStatus_asSQL()+", `LastAlive`="+pRow->LastAlive_asSQL()+", `isActive`="+pRow->isActive_asSQL()+", `FK_Version`="+pRow->FK_Version_asSQL()+", `isMonitored`="+pRow->isMonitored_asSQL()+", `FK_RepositoryType_Source`="+pRow->FK_RepositoryType_Source_asSQL()+", `FK_RepositoryType_Binaries`="+pRow->FK_RepositoryType_Binaries_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Installation set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Installation* pRow = (Row_Installation*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Installation* pRow = (Row_Installation*) (*i).second;	

		char tmp_PK_Installation[32];
sprintf(tmp_PK_Installation, "%li", key.pk);


string condition;
condition = condition + "`PK_Installation`=" + tmp_PK_Installation;

	
		string query = "delete from Installation where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Installation*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Installation::GetRows(string where_statement,vector<class Row_Installation*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Installation`.* from Installation " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Installation`.* from Installation where " + where_statement;
	else
		query = "select `Installation`.* from Installation";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Installation *pRow = new Row_Installation(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Installation));
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
pRow->m_Name = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Name = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Address = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Address = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_City = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_City = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_State = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Zip = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Zip = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Country));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_City = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_City));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_PostalCode = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_PostalCode));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ActivationCode = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_ActivationCode = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_LastStatus = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_LastStatus = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_LastAlive = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_LastAlive = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_isActive = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_isActive));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_Version));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_isMonitored = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_isMonitored));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_RepositoryType_Source = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_RepositoryType_Source));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_RepositoryType_Binaries = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_RepositoryType_Binaries));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_id));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_batch));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_psc_user));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%hi", &(pRow->m_psc_frozen));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[22]=false;
pRow->m_psc_mod = string(row[22],lengths[22]);
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Installation);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Installation *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Installation* Table_Installation::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Installation *pRow = new Row_Installation(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Installation* Table_Installation::GetRow(long int in_PK_Installation)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Installation);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Installation*) (*i).second;
	//we have to fetch row
	Row_Installation* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Installation* Table_Installation::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Installation[32];
sprintf(tmp_PK_Installation, "%li", key.pk);


string condition;
condition = condition + "`PK_Installation`=" + tmp_PK_Installation;


	string query = "select * from Installation where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Installation::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Installation *pRow = new Row_Installation(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Installation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Installation));
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
pRow->m_Name = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Name = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Address = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Address = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_City = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_City = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_State = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_State = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Zip = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Zip = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Country = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Country));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_City = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_City));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_PostalCode = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_PostalCode));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ActivationCode = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_ActivationCode = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_LastStatus = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_LastStatus = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_LastAlive = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_LastAlive = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_isActive = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_isActive));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_FK_Version = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_Version));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_isMonitored = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_isMonitored));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_RepositoryType_Source = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_RepositoryType_Source));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_RepositoryType_Binaries = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_RepositoryType_Binaries));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_id));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_batch));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_psc_user));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%hi", &(pRow->m_psc_frozen));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[22]=false;
pRow->m_psc_mod = string(row[22],lengths[22]);
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Country* Row_Installation::FK_Country_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Country *pTable = table->database->Country_get();
return pTable->GetRow(m_FK_Country);
}
class Row_City* Row_Installation::FK_City_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_City *pTable = table->database->City_get();
return pTable->GetRow(m_FK_City);
}
class Row_PostalCode* Row_Installation::FK_PostalCode_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PostalCode *pTable = table->database->PostalCode_get();
return pTable->GetRow(m_FK_PostalCode);
}
class Row_Version* Row_Installation::FK_Version_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Version *pTable = table->database->Version_get();
return pTable->GetRow(m_FK_Version);
}
class Row_RepositoryType* Row_Installation::FK_RepositoryType_Source_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RepositoryType *pTable = table->database->RepositoryType_get();
return pTable->GetRow(m_FK_RepositoryType_Source);
}
class Row_RepositoryType* Row_Installation::FK_RepositoryType_Binaries_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RepositoryType *pTable = table->database->RepositoryType_get();
return pTable->GetRow(m_FK_RepositoryType_Binaries);
}


void Row_Installation::CommandGroup_FK_Installation_getrows(vector <class Row_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup *pTable = table->database->CommandGroup_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Criteria_FK_Installation_getrows(vector <class Row_Criteria*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Criteria *pTable = table->database->Criteria_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Device_FK_Installation_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::DeviceGroup_FK_Installation_getrows(vector <class Row_DeviceGroup*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceGroup *pTable = table->database->DeviceGroup_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::EventHandler_FK_Installation_getrows(vector <class Row_EventHandler*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_EventHandler *pTable = table->database->EventHandler_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Floorplan_FK_Installation_getrows(vector <class Row_Floorplan*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Floorplan *pTable = table->database->Floorplan_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Household_Installation_FK_Installation_getrows(vector <class Row_Household_Installation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Household_Installation *pTable = table->database->Household_Installation_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::InfraredGroup_Command_Preferred_FK_Installation_getrows(vector <class Row_InfraredGroup_Command_Preferred*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_InfraredGroup_Command_Preferred *pTable = table->database->InfraredGroup_Command_Preferred_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Installation_RepositorySource_URL_FK_Installation_getrows(vector <class Row_Installation_RepositorySource_URL*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation_RepositorySource_URL *pTable = table->database->Installation_RepositorySource_URL_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Installation_Users_FK_Installation_getrows(vector <class Row_Installation_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation_Users *pTable = table->database->Installation_Users_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Room_FK_Installation_getrows(vector <class Row_Room*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Room *pTable = table->database->Room_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::SetupStep_FK_Installation_getrows(vector <class Row_SetupStep*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_SetupStep *pTable = table->database->SetupStep_get();
pTable->GetRows("`FK_Installation`=" + StringUtils::itos(m_PK_Installation),rows);
}
void Row_Installation::Users_FK_Installation_Main_getrows(vector <class Row_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Users *pTable = table->database->Users_get();
pTable->GetRows("`FK_Installation_Main`=" + StringUtils::itos(m_PK_Installation),rows);
}



