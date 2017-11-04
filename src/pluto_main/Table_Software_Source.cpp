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
#include "Table_Software_Source.h"
#include "Table_Software.h"

#include "Table_Software_Device.h"


void Database_pluto_main::CreateTable_Software_Source()
{
	tblSoftware_Source = new Table_Software_Source(this);
}

void Database_pluto_main::DeleteTable_Software_Source()
{
	if( tblSoftware_Source )
		delete tblSoftware_Source;
}

bool Database_pluto_main::Commit_Software_Source(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblSoftware_Source->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Software_Source::~Table_Software_Source()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Software_Source *pRow = (Row_Software_Source *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Software_Source *pRow = (Row_Software_Source *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Software_Source::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Software_Source *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Software_Source *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Software_Source);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Software_Source::Reload()
{
	Row_Software_Source *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Software_Source);
		Row_Software_Source *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Software_Source::Row_Software_Source(Table_Software_Source *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Software_Source::SetDefaultValues()
{
	m_PK_Software_Source = 0;
is_null[0] = false;
m_FK_Software = 0;
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
is_null[10] = true;
is_null[11] = true;
is_null[12] = true;
is_null[13] = true;
is_null[14] = true;
is_null[15] = true;
m_Rating = 0.00;
is_null[16] = false;
m_Virus_Free = 0;
is_null[17] = false;
m_PC_Type = "SM";
is_null[18] = false;
is_null[19] = true;
is_null[20] = true;
is_null[21] = true;
m_psc_id = 0;
is_null[22] = true;
m_psc_batch = 0;
is_null[23] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[24] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[25] = false;
is_null[26] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Software_Source::PK_Software_Source_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Software_Source;}
long int Row_Software_Source::FK_Software_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Software;}
string Row_Software_Source::Downloadurl_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Downloadurl;}
string Row_Software_Source::Version_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Version;}
string Row_Software_Source::Distro_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Distro;}
string Row_Software_Source::Required_Version_Min_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Required_Version_Min;}
string Row_Software_Source::Required_Version_Max_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Required_Version_Max;}
string Row_Software_Source::RepositoryName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RepositoryName;}
string Row_Software_Source::Iconstr_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Iconstr;}
string Row_Software_Source::Title_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Title;}
string Row_Software_Source::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_Software_Source::HomeURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HomeURL;}
string Row_Software_Source::Category_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Category;}
string Row_Software_Source::Misc_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Misc;}
string Row_Software_Source::Target_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Target;}
string Row_Software_Source::Importance_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Importance;}
float Row_Software_Source::Rating_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Rating;}
short int Row_Software_Source::Virus_Free_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Virus_Free;}
string Row_Software_Source::PC_Type_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PC_Type;}
string Row_Software_Source::Sum_md5_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Sum_md5;}
string Row_Software_Source::Sum_sha_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Sum_sha;}
long int Row_Software_Source::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Software_Source::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Software_Source::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Software_Source::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Software_Source::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Software_Source::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Software_Source::PK_Software_Source_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Software_Source = val; is_modified=true; is_null[0]=false;}
void Row_Software_Source::FK_Software_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Software = val; is_modified=true; is_null[1]=false;}
void Row_Software_Source::Downloadurl_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Downloadurl = val; is_modified=true; is_null[2]=false;}
void Row_Software_Source::Version_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Version = val; is_modified=true; is_null[3]=false;}
void Row_Software_Source::Distro_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Distro = val; is_modified=true; is_null[4]=false;}
void Row_Software_Source::Required_Version_Min_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Required_Version_Min = val; is_modified=true; is_null[5]=false;}
void Row_Software_Source::Required_Version_Max_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Required_Version_Max = val; is_modified=true; is_null[6]=false;}
void Row_Software_Source::RepositoryName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RepositoryName = val; is_modified=true; is_null[7]=false;}
void Row_Software_Source::Iconstr_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Iconstr = val; is_modified=true; is_null[8]=false;}
void Row_Software_Source::Title_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Title = val; is_modified=true; is_null[9]=false;}
void Row_Software_Source::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[10]=false;}
void Row_Software_Source::HomeURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HomeURL = val; is_modified=true; is_null[11]=false;}
void Row_Software_Source::Category_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Category = val; is_modified=true; is_null[12]=false;}
void Row_Software_Source::Misc_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Misc = val; is_modified=true; is_null[13]=false;}
void Row_Software_Source::Target_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Target = val; is_modified=true; is_null[14]=false;}
void Row_Software_Source::Importance_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Importance = val; is_modified=true; is_null[15]=false;}
void Row_Software_Source::Rating_set(float val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Rating = val; is_modified=true; is_null[16]=false;}
void Row_Software_Source::Virus_Free_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Virus_Free = val; is_modified=true; is_null[17]=false;}
void Row_Software_Source::PC_Type_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PC_Type = val; is_modified=true; is_null[18]=false;}
void Row_Software_Source::Sum_md5_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Sum_md5 = val; is_modified=true; is_null[19]=false;}
void Row_Software_Source::Sum_sha_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Sum_sha = val; is_modified=true; is_null[20]=false;}
void Row_Software_Source::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[21]=false;}
void Row_Software_Source::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[22]=false;}
void Row_Software_Source::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[23]=false;}
void Row_Software_Source::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[24]=false;}
void Row_Software_Source::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[25]=false;}
void Row_Software_Source::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[26]=false;}

		
bool Row_Software_Source::Downloadurl_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Software_Source::Version_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Software_Source::Distro_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Software_Source::Required_Version_Min_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Software_Source::Required_Version_Max_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Software_Source::RepositoryName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Software_Source::Iconstr_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Software_Source::Title_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Software_Source::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Software_Source::HomeURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Software_Source::Category_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Software_Source::Misc_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_Software_Source::Target_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_Software_Source::Importance_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Software_Source::PC_Type_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_Software_Source::Sum_md5_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_Software_Source::Sum_sha_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_Software_Source::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_Software_Source::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_Software_Source::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}
bool Row_Software_Source::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_Software_Source::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}

			
void Row_Software_Source::Downloadurl_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Software_Source::Version_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Software_Source::Distro_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Software_Source::Required_Version_Min_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Software_Source::Required_Version_Max_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Software_Source::RepositoryName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Software_Source::Iconstr_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Software_Source::Title_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Software_Source::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Software_Source::HomeURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Software_Source::Category_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Software_Source::Misc_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Software_Source::Target_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Software_Source::Importance_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Software_Source::PC_Type_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_Software_Source::Sum_md5_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Software_Source::Sum_sha_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Software_Source::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_Software_Source::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_Software_Source::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
void Row_Software_Source::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_Software_Source::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
	

string Row_Software_Source::PK_Software_Source_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Software_Source);

return buf;
}

string Row_Software_Source::FK_Software_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Software);

return buf;
}

string Row_Software_Source::Downloadurl_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Downloadurl.c_str(), (unsigned long) min((size_t)255,m_Downloadurl.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Version_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Version.c_str(), (unsigned long) min((size_t)10,m_Version.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Distro_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Distro.c_str(), (unsigned long) min((size_t)20,m_Distro.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Required_Version_Min_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[23];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Required_Version_Min.c_str(), (unsigned long) min((size_t)11,m_Required_Version_Min.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Required_Version_Max_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[23];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Required_Version_Max.c_str(), (unsigned long) min((size_t)11,m_Required_Version_Max.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::RepositoryName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_RepositoryName.c_str(), (unsigned long) min((size_t)100,m_RepositoryName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Iconstr_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Iconstr.c_str(), (unsigned long) min((size_t)65535,m_Iconstr.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Title_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Title.c_str(), (unsigned long) min((size_t)20,m_Title.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[131071];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)65535,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::HomeURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_HomeURL.c_str(), (unsigned long) min((size_t)100,m_HomeURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Category_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Category.c_str(), (unsigned long) min((size_t)20,m_Category.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Misc_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Misc.c_str(), (unsigned long) min((size_t)20,m_Misc.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Target_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Target.c_str(), (unsigned long) min((size_t)20,m_Target.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Importance_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Importance.c_str(), (unsigned long) min((size_t)20,m_Importance.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Rating_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%f", m_Rating);

return buf;
}

string Row_Software_Source::Virus_Free_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Virus_Free);

return buf;
}

string Row_Software_Source::PC_Type_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char *buf = new char[5];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_PC_Type.c_str(), (unsigned long) min((size_t)2,m_PC_Type.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Sum_md5_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char *buf = new char[67];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Sum_md5.c_str(), (unsigned long) min((size_t)33,m_Sum_md5.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::Sum_sha_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char *buf = new char[83];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Sum_sha.c_str(), (unsigned long) min((size_t)41,m_Sum_sha.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Software_Source::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Software_Source::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Software_Source::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Software_Source::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Software_Source::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Software_Source::Key::Key(long int in_PK_Software_Source)
{
			pk_PK_Software_Source = in_PK_Software_Source;
	
}

Table_Software_Source::Key::Key(Row_Software_Source *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Software_Source = pRow->m_PK_Software_Source;
	
}		

bool Table_Software_Source::Key_Less::operator()(const Table_Software_Source::Key &key1, const Table_Software_Source::Key &key2) const
{
			if (key1.pk_PK_Software_Source!=key2.pk_PK_Software_Source)
return key1.pk_PK_Software_Source<key2.pk_PK_Software_Source;
else
return false;	
}	

bool Table_Software_Source::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Software_Source *pRow = (Row_Software_Source *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Software_Source_asSQL()+", "+pRow->FK_Software_asSQL()+", "+pRow->Downloadurl_asSQL()+", "+pRow->Version_asSQL()+", "+pRow->Distro_asSQL()+", "+pRow->Required_Version_Min_asSQL()+", "+pRow->Required_Version_Max_asSQL()+", "+pRow->RepositoryName_asSQL()+", "+pRow->Iconstr_asSQL()+", "+pRow->Title_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->HomeURL_asSQL()+", "+pRow->Category_asSQL()+", "+pRow->Misc_asSQL()+", "+pRow->Target_asSQL()+", "+pRow->Importance_asSQL()+", "+pRow->Rating_asSQL()+", "+pRow->Virus_Free_asSQL()+", "+pRow->PC_Type_asSQL()+", "+pRow->Sum_md5_asSQL()+", "+pRow->Sum_sha_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Software_Source (`PK_Software_Source`, `FK_Software`, `Downloadurl`, `Version`, `Distro`, `Required_Version_Min`, `Required_Version_Max`, `RepositoryName`, `Iconstr`, `Title`, `Description`, `HomeURL`, `Category`, `Misc`, `Target`, `Importance`, `Rating`, `Virus_Free`, `PC_Type`, `Sum_md5`, `Sum_sha`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Software_Source=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Software_Source is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Software_Source);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Software_Source* pRow = (Row_Software_Source*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Software_Source);

		char tmp_PK_Software_Source[32];
sprintf(tmp_PK_Software_Source, "%li", key.pk);


string condition;
condition = condition + "`PK_Software_Source`=" + tmp_PK_Software_Source;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Software_Source`="+pRow->PK_Software_Source_asSQL()+", `FK_Software`="+pRow->FK_Software_asSQL()+", `Downloadurl`="+pRow->Downloadurl_asSQL()+", `Version`="+pRow->Version_asSQL()+", `Distro`="+pRow->Distro_asSQL()+", `Required_Version_Min`="+pRow->Required_Version_Min_asSQL()+", `Required_Version_Max`="+pRow->Required_Version_Max_asSQL()+", `RepositoryName`="+pRow->RepositoryName_asSQL()+", `Iconstr`="+pRow->Iconstr_asSQL()+", `Title`="+pRow->Title_asSQL()+", `Description`="+pRow->Description_asSQL()+", `HomeURL`="+pRow->HomeURL_asSQL()+", `Category`="+pRow->Category_asSQL()+", `Misc`="+pRow->Misc_asSQL()+", `Target`="+pRow->Target_asSQL()+", `Importance`="+pRow->Importance_asSQL()+", `Rating`="+pRow->Rating_asSQL()+", `Virus_Free`="+pRow->Virus_Free_asSQL()+", `PC_Type`="+pRow->PC_Type_asSQL()+", `Sum_md5`="+pRow->Sum_md5_asSQL()+", `Sum_sha`="+pRow->Sum_sha_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Software_Source set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Software_Source* pRow = (Row_Software_Source*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Software_Source* pRow = (Row_Software_Source*) (*i).second;	

		char tmp_PK_Software_Source[32];
sprintf(tmp_PK_Software_Source, "%li", key.pk);


string condition;
condition = condition + "`PK_Software_Source`=" + tmp_PK_Software_Source;

	
		string query = "delete from Software_Source where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Software_Source*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Software_Source::GetRows(string where_statement,vector<class Row_Software_Source*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Software_Source`.* from Software_Source " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Software_Source`.* from Software_Source where " + where_statement;
	else
		query = "select `Software_Source`.* from Software_Source";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Software_Source *pRow = new Row_Software_Source(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Software_Source = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Software_Source));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Software = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Software));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Downloadurl = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Downloadurl = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Version = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Version = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Distro = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Distro = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Required_Version_Min = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Required_Version_Min = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Required_Version_Max = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Required_Version_Max = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_RepositoryName = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_RepositoryName = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Iconstr = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Iconstr = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Title = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Title = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Description = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_HomeURL = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_HomeURL = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Category = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Category = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Misc = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Misc = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Target = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_Target = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Importance = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Importance = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Rating = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%f", &(pRow->m_Rating));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Virus_Free = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_Virus_Free));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_PC_Type = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_PC_Type = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_Sum_md5 = "";
}
else
{
pRow->is_null[19]=false;
pRow->m_Sum_md5 = string(row[19],lengths[19]);
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_Sum_sha = "";
}
else
{
pRow->is_null[20]=false;
pRow->m_Sum_sha = string(row[20],lengths[20]);
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%li", &(pRow->m_psc_id));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_batch));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_user));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%hi", &(pRow->m_psc_frozen));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[25]=false;
pRow->m_psc_mod = string(row[25],lengths[25]);
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Software_Source);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Software_Source *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Software_Source* Table_Software_Source::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Software_Source *pRow = new Row_Software_Source(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Software_Source* Table_Software_Source::GetRow(long int in_PK_Software_Source)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Software_Source);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Software_Source*) (*i).second;
	//we have to fetch row
	Row_Software_Source* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Software_Source* Table_Software_Source::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Software_Source[32];
sprintf(tmp_PK_Software_Source, "%li", key.pk);


string condition;
condition = condition + "`PK_Software_Source`=" + tmp_PK_Software_Source;


	string query = "select * from Software_Source where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Software_Source::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Software_Source *pRow = new Row_Software_Source(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Software_Source = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Software_Source));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Software = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Software));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Downloadurl = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Downloadurl = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Version = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Version = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Distro = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Distro = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Required_Version_Min = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Required_Version_Min = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Required_Version_Max = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Required_Version_Max = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_RepositoryName = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_RepositoryName = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Iconstr = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_Iconstr = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Title = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Title = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[10]=false;
pRow->m_Description = string(row[10],lengths[10]);
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_HomeURL = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_HomeURL = string(row[11],lengths[11]);
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Category = "";
}
else
{
pRow->is_null[12]=false;
pRow->m_Category = string(row[12],lengths[12]);
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_Misc = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_Misc = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Target = "";
}
else
{
pRow->is_null[14]=false;
pRow->m_Target = string(row[14],lengths[14]);
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Importance = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_Importance = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Rating = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%f", &(pRow->m_Rating));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Virus_Free = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_Virus_Free));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_PC_Type = "";
}
else
{
pRow->is_null[18]=false;
pRow->m_PC_Type = string(row[18],lengths[18]);
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_Sum_md5 = "";
}
else
{
pRow->is_null[19]=false;
pRow->m_Sum_md5 = string(row[19],lengths[19]);
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_Sum_sha = "";
}
else
{
pRow->is_null[20]=false;
pRow->m_Sum_sha = string(row[20],lengths[20]);
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%li", &(pRow->m_psc_id));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_batch));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_user));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%hi", &(pRow->m_psc_frozen));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[25]=false;
pRow->m_psc_mod = string(row[25],lengths[25]);
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Software* Row_Software_Source::FK_Software_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Software *pTable = table->database->Software_get();
return pTable->GetRow(m_FK_Software);
}


void Row_Software_Source::Software_Device_FK_Software_Source_getrows(vector <class Row_Software_Device*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Software_Device *pTable = table->database->Software_Device_get();
pTable->GetRows("`FK_Software_Source`=" + StringUtils::itos(m_PK_Software_Source),rows);
}



