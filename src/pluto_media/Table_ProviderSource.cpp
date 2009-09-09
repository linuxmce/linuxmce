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
#include "Table_ProviderSource.h"

#include "Table_MediaProvider.h"


void Database_pluto_media::CreateTable_ProviderSource()
{
	tblProviderSource = new Table_ProviderSource(this);
}

void Database_pluto_media::DeleteTable_ProviderSource()
{
	if( tblProviderSource )
		delete tblProviderSource;
}

bool Database_pluto_media::Commit_ProviderSource(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblProviderSource->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_ProviderSource::~Table_ProviderSource()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_ProviderSource *pRow = (Row_ProviderSource *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_ProviderSource *pRow = (Row_ProviderSource *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_ProviderSource::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_ProviderSource *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_ProviderSource *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_ProviderSource);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_ProviderSource::Reload()
{
	Row_ProviderSource *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_ProviderSource);
		Row_ProviderSource *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_ProviderSource::Row_ProviderSource(Table_ProviderSource *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_ProviderSource::SetDefaultValues()
{
	m_PK_ProviderSource = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_UserNamePassword = 0;
is_null[3] = false;
is_null[4] = true;
m_EK_Country = 0;
is_null[5] = true;
m_EK_MediaType = 0;
is_null[6] = true;
is_null[7] = true;
m_EK_Package = 0;
is_null[8] = true;
is_null[9] = true;
m_FillHours = 0;
is_null[10] = true;
is_null[11] = true;
is_null[12] = true;
is_null[13] = true;
is_null[14] = true;
m_psc_id = 0;
is_null[15] = true;
m_psc_batch = 0;
is_null[16] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[17] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[18] = false;
is_null[19] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_ProviderSource::PK_ProviderSource_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_ProviderSource;}
string Row_ProviderSource::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_ProviderSource::Comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Comments;}
short int Row_ProviderSource::UserNamePassword_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UserNamePassword;}
long int Row_ProviderSource::EK_Country_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Country;}
long int Row_ProviderSource::EK_MediaType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_MediaType;}
string Row_ProviderSource::ProviderOrganization_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ProviderOrganization;}
long int Row_ProviderSource::EK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Package;}
string Row_ProviderSource::FillCommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FillCommandLine;}
short int Row_ProviderSource::FillHours_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FillHours;}
string Row_ProviderSource::ProviderCommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ProviderCommandLine;}
string Row_ProviderSource::DeviceCommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DeviceCommandLine;}
string Row_ProviderSource::PackageCommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PackageCommandLine;}
string Row_ProviderSource::LineupCommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_LineupCommandLine;}
long int Row_ProviderSource::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_ProviderSource::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_ProviderSource::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_ProviderSource::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_ProviderSource::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_ProviderSource::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_ProviderSource::PK_ProviderSource_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_ProviderSource = val; is_modified=true; is_null[0]=false;}
void Row_ProviderSource::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_ProviderSource::Comments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Comments = val; is_modified=true; is_null[2]=false;}
void Row_ProviderSource::UserNamePassword_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UserNamePassword = val; is_modified=true; is_null[3]=false;}
void Row_ProviderSource::EK_Country_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Country = val; is_modified=true; is_null[4]=false;}
void Row_ProviderSource::EK_MediaType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_MediaType = val; is_modified=true; is_null[5]=false;}
void Row_ProviderSource::ProviderOrganization_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ProviderOrganization = val; is_modified=true; is_null[6]=false;}
void Row_ProviderSource::EK_Package_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Package = val; is_modified=true; is_null[7]=false;}
void Row_ProviderSource::FillCommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FillCommandLine = val; is_modified=true; is_null[8]=false;}
void Row_ProviderSource::FillHours_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FillHours = val; is_modified=true; is_null[9]=false;}
void Row_ProviderSource::ProviderCommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ProviderCommandLine = val; is_modified=true; is_null[10]=false;}
void Row_ProviderSource::DeviceCommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DeviceCommandLine = val; is_modified=true; is_null[11]=false;}
void Row_ProviderSource::PackageCommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PackageCommandLine = val; is_modified=true; is_null[12]=false;}
void Row_ProviderSource::LineupCommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_LineupCommandLine = val; is_modified=true; is_null[13]=false;}
void Row_ProviderSource::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[14]=false;}
void Row_ProviderSource::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[15]=false;}
void Row_ProviderSource::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[16]=false;}
void Row_ProviderSource::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[17]=false;}
void Row_ProviderSource::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[18]=false;}
void Row_ProviderSource::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[19]=false;}

		
bool Row_ProviderSource::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_ProviderSource::Comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_ProviderSource::EK_Country_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_ProviderSource::EK_MediaType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_ProviderSource::ProviderOrganization_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_ProviderSource::EK_Package_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_ProviderSource::FillCommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_ProviderSource::FillHours_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_ProviderSource::ProviderCommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_ProviderSource::DeviceCommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_ProviderSource::PackageCommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_ProviderSource::LineupCommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_ProviderSource::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_ProviderSource::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_ProviderSource::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_ProviderSource::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_ProviderSource::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}

			
void Row_ProviderSource::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_ProviderSource::Comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_ProviderSource::EK_Country_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_ProviderSource::EK_MediaType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_ProviderSource::ProviderOrganization_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_ProviderSource::EK_Package_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_ProviderSource::FillCommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_ProviderSource::FillHours_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_ProviderSource::ProviderCommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_ProviderSource::DeviceCommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_ProviderSource::PackageCommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_ProviderSource::LineupCommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_ProviderSource::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_ProviderSource::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_ProviderSource::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_ProviderSource::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_ProviderSource::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
	

string Row_ProviderSource::PK_ProviderSource_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_ProviderSource);

return buf;
}

string Row_ProviderSource::Description_asSQL()
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

string Row_ProviderSource::Comments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Comments.c_str(), (unsigned long) min((size_t)65535,m_Comments.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::UserNamePassword_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_UserNamePassword);

return buf;
}

string Row_ProviderSource::EK_Country_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Country);

return buf;
}

string Row_ProviderSource::EK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_MediaType);

return buf;
}

string Row_ProviderSource::ProviderOrganization_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ProviderOrganization.c_str(), (unsigned long) min((size_t)1,m_ProviderOrganization.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::EK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Package);

return buf;
}

string Row_ProviderSource::FillCommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_FillCommandLine.c_str(), (unsigned long) min((size_t)255,m_FillCommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::FillHours_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FillHours);

return buf;
}

string Row_ProviderSource::ProviderCommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ProviderCommandLine.c_str(), (unsigned long) min((size_t)255,m_ProviderCommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::DeviceCommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DeviceCommandLine.c_str(), (unsigned long) min((size_t)255,m_DeviceCommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::PackageCommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_PackageCommandLine.c_str(), (unsigned long) min((size_t)255,m_PackageCommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::LineupCommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_LineupCommandLine.c_str(), (unsigned long) min((size_t)255,m_LineupCommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_ProviderSource::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_ProviderSource::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_ProviderSource::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_ProviderSource::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_ProviderSource::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_ProviderSource::Key::Key(long int in_PK_ProviderSource)
{
			pk_PK_ProviderSource = in_PK_ProviderSource;
	
}

Table_ProviderSource::Key::Key(Row_ProviderSource *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_ProviderSource = pRow->m_PK_ProviderSource;
	
}		

bool Table_ProviderSource::Key_Less::operator()(const Table_ProviderSource::Key &key1, const Table_ProviderSource::Key &key2) const
{
			if (key1.pk_PK_ProviderSource!=key2.pk_PK_ProviderSource)
return key1.pk_PK_ProviderSource<key2.pk_PK_ProviderSource;
else
return false;	
}	

bool Table_ProviderSource::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_ProviderSource *pRow = (Row_ProviderSource *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_ProviderSource_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->UserNamePassword_asSQL()+", "+pRow->EK_Country_asSQL()+", "+pRow->EK_MediaType_asSQL()+", "+pRow->ProviderOrganization_asSQL()+", "+pRow->EK_Package_asSQL()+", "+pRow->FillCommandLine_asSQL()+", "+pRow->FillHours_asSQL()+", "+pRow->ProviderCommandLine_asSQL()+", "+pRow->DeviceCommandLine_asSQL()+", "+pRow->PackageCommandLine_asSQL()+", "+pRow->LineupCommandLine_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into ProviderSource (`PK_ProviderSource`, `Description`, `Comments`, `UserNamePassword`, `EK_Country`, `EK_MediaType`, `ProviderOrganization`, `EK_Package`, `FillCommandLine`, `FillHours`, `ProviderCommandLine`, `DeviceCommandLine`, `PackageCommandLine`, `LineupCommandLine`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_ProviderSource=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_ProviderSource is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_ProviderSource);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_ProviderSource* pRow = (Row_ProviderSource*) (*i).second;	
		SingleLongKey key(pRow->m_PK_ProviderSource);

		char tmp_PK_ProviderSource[32];
sprintf(tmp_PK_ProviderSource, "%li", key.pk);


string condition;
condition = condition + "`PK_ProviderSource`=" + tmp_PK_ProviderSource;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_ProviderSource`="+pRow->PK_ProviderSource_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Comments`="+pRow->Comments_asSQL()+", `UserNamePassword`="+pRow->UserNamePassword_asSQL()+", `EK_Country`="+pRow->EK_Country_asSQL()+", `EK_MediaType`="+pRow->EK_MediaType_asSQL()+", `ProviderOrganization`="+pRow->ProviderOrganization_asSQL()+", `EK_Package`="+pRow->EK_Package_asSQL()+", `FillCommandLine`="+pRow->FillCommandLine_asSQL()+", `FillHours`="+pRow->FillHours_asSQL()+", `ProviderCommandLine`="+pRow->ProviderCommandLine_asSQL()+", `DeviceCommandLine`="+pRow->DeviceCommandLine_asSQL()+", `PackageCommandLine`="+pRow->PackageCommandLine_asSQL()+", `LineupCommandLine`="+pRow->LineupCommandLine_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update ProviderSource set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_ProviderSource* pRow = (Row_ProviderSource*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_ProviderSource* pRow = (Row_ProviderSource*) (*i).second;	

		char tmp_PK_ProviderSource[32];
sprintf(tmp_PK_ProviderSource, "%li", key.pk);


string condition;
condition = condition + "`PK_ProviderSource`=" + tmp_PK_ProviderSource;

	
		string query = "delete from ProviderSource where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_ProviderSource*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_ProviderSource::GetRows(string where_statement,vector<class Row_ProviderSource*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `ProviderSource`.* from ProviderSource " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `ProviderSource`.* from ProviderSource where " + where_statement;
	else
		query = "select `ProviderSource`.* from ProviderSource";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_ProviderSource *pRow = new Row_ProviderSource(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_ProviderSource = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_ProviderSource));
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
pRow->m_Comments = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Comments = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_UserNamePassword = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_UserNamePassword));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EK_Country = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_EK_Country));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_EK_MediaType));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ProviderOrganization = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_ProviderOrganization = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_EK_Package = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_EK_Package));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FillCommandLine = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_FillCommandLine = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FillHours = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_FillHours));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ProviderCommandLine = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_ProviderCommandLine = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_DeviceCommandLine = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_DeviceCommandLine = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_PackageCommandLine = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_PackageCommandLine = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_LineupCommandLine = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_LineupCommandLine = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_id));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_batch));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_psc_user));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_psc_frozen));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_psc_mod = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_ProviderSource);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_ProviderSource *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_ProviderSource* Table_ProviderSource::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_ProviderSource *pRow = new Row_ProviderSource(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_ProviderSource* Table_ProviderSource::GetRow(long int in_PK_ProviderSource)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_ProviderSource);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_ProviderSource*) (*i).second;
	//we have to fetch row
	Row_ProviderSource* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_ProviderSource* Table_ProviderSource::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_ProviderSource[32];
sprintf(tmp_PK_ProviderSource, "%li", key.pk);


string condition;
condition = condition + "`PK_ProviderSource`=" + tmp_PK_ProviderSource;


	string query = "select * from ProviderSource where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_ProviderSource::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_ProviderSource *pRow = new Row_ProviderSource(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_ProviderSource = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_ProviderSource));
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
pRow->m_Comments = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Comments = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_UserNamePassword = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_UserNamePassword));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_EK_Country = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_EK_Country));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_EK_MediaType = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_EK_MediaType));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ProviderOrganization = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_ProviderOrganization = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_EK_Package = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_EK_Package));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FillCommandLine = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_FillCommandLine = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FillHours = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_FillHours));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ProviderCommandLine = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_ProviderCommandLine = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_DeviceCommandLine = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_DeviceCommandLine = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_PackageCommandLine = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_PackageCommandLine = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_LineupCommandLine = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_LineupCommandLine = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_id));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_batch));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_psc_user));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_psc_frozen));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_psc_mod = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}




void Row_ProviderSource::MediaProvider_FK_ProviderSource_getrows(vector <class Row_MediaProvider*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_MediaProvider *pTable = table->database->MediaProvider_get();
pTable->GetRows("`FK_ProviderSource`=" + StringUtils::itos(m_PK_ProviderSource),rows);
}



