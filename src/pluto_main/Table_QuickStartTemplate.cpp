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
#include "Table_QuickStartTemplate.h"
#include "Table_QuickStartCategory.h"
#include "Table_Package.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"

#include "Table_Device_QuickStart.h"


void Database_pluto_main::CreateTable_QuickStartTemplate()
{
	tblQuickStartTemplate = new Table_QuickStartTemplate(this);
}

void Database_pluto_main::DeleteTable_QuickStartTemplate()
{
	if( tblQuickStartTemplate )
		delete tblQuickStartTemplate;
}

bool Database_pluto_main::Commit_QuickStartTemplate(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblQuickStartTemplate->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_QuickStartTemplate::~Table_QuickStartTemplate()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_QuickStartTemplate *pRow = (Row_QuickStartTemplate *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_QuickStartTemplate *pRow = (Row_QuickStartTemplate *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_QuickStartTemplate::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_QuickStartTemplate *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_QuickStartTemplate *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_QuickStartTemplate);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_QuickStartTemplate::Reload()
{
	Row_QuickStartTemplate *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_QuickStartTemplate);
		Row_QuickStartTemplate *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_QuickStartTemplate::Row_QuickStartTemplate(Table_QuickStartTemplate *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_QuickStartTemplate::SetDefaultValues()
{
	m_PK_QuickStartTemplate = 0;
is_null[0] = false;
is_null[1] = true;
is_null[2] = true;
m_FK_QuickStartCategory = 0;
is_null[3] = true;
m_FK_Package = 0;
is_null[4] = true;
is_null[5] = true;
is_null[6] = true;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
m_FK_DesignObj = 0;
is_null[10] = true;
m_FK_DesignObj_OSD = 0;
is_null[11] = true;
m_psc_id = 0;
is_null[12] = true;
m_psc_batch = 0;
is_null[13] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[14] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[15] = false;
is_null[16] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_QuickStartTemplate::PK_QuickStartTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_QuickStartTemplate;}
string Row_QuickStartTemplate::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_QuickStartTemplate::FK_QuickStartCategory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_QuickStartCategory;}
long int Row_QuickStartTemplate::FK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Package;}
string Row_QuickStartTemplate::Binary_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Binary;}
string Row_QuickStartTemplate::Arguments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Arguments;}
string Row_QuickStartTemplate::Homepage_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Homepage;}
string Row_QuickStartTemplate::Icon_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Icon;}
string Row_QuickStartTemplate::WindowClass_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_WindowClass;}
long int Row_QuickStartTemplate::FK_DesignObj_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj;}
long int Row_QuickStartTemplate::FK_DesignObj_OSD_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_OSD;}
long int Row_QuickStartTemplate::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_QuickStartTemplate::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_QuickStartTemplate::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_QuickStartTemplate::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_QuickStartTemplate::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_QuickStartTemplate::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_QuickStartTemplate::PK_QuickStartTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_QuickStartTemplate = val; is_modified=true; is_null[0]=false;}
void Row_QuickStartTemplate::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_QuickStartTemplate::FK_QuickStartCategory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_QuickStartCategory = val; is_modified=true; is_null[2]=false;}
void Row_QuickStartTemplate::FK_Package_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Package = val; is_modified=true; is_null[3]=false;}
void Row_QuickStartTemplate::Binary_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Binary = val; is_modified=true; is_null[4]=false;}
void Row_QuickStartTemplate::Arguments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Arguments = val; is_modified=true; is_null[5]=false;}
void Row_QuickStartTemplate::Homepage_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Homepage = val; is_modified=true; is_null[6]=false;}
void Row_QuickStartTemplate::Icon_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Icon = val; is_modified=true; is_null[7]=false;}
void Row_QuickStartTemplate::WindowClass_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_WindowClass = val; is_modified=true; is_null[8]=false;}
void Row_QuickStartTemplate::FK_DesignObj_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj = val; is_modified=true; is_null[9]=false;}
void Row_QuickStartTemplate::FK_DesignObj_OSD_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_OSD = val; is_modified=true; is_null[10]=false;}
void Row_QuickStartTemplate::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[11]=false;}
void Row_QuickStartTemplate::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[12]=false;}
void Row_QuickStartTemplate::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[13]=false;}
void Row_QuickStartTemplate::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[14]=false;}
void Row_QuickStartTemplate::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[15]=false;}
void Row_QuickStartTemplate::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[16]=false;}

		
bool Row_QuickStartTemplate::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_QuickStartTemplate::FK_QuickStartCategory_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_QuickStartTemplate::FK_Package_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_QuickStartTemplate::Binary_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_QuickStartTemplate::Arguments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_QuickStartTemplate::Homepage_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_QuickStartTemplate::Icon_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_QuickStartTemplate::WindowClass_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_QuickStartTemplate::FK_DesignObj_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_QuickStartTemplate::FK_DesignObj_OSD_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_QuickStartTemplate::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_QuickStartTemplate::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_QuickStartTemplate::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_QuickStartTemplate::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_QuickStartTemplate::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}

			
void Row_QuickStartTemplate::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_QuickStartTemplate::FK_QuickStartCategory_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_QuickStartTemplate::FK_Package_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_QuickStartTemplate::Binary_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_QuickStartTemplate::Arguments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_QuickStartTemplate::Homepage_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_QuickStartTemplate::Icon_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_QuickStartTemplate::WindowClass_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_QuickStartTemplate::FK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_QuickStartTemplate::FK_DesignObj_OSD_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_QuickStartTemplate::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_QuickStartTemplate::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_QuickStartTemplate::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_QuickStartTemplate::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_QuickStartTemplate::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
	

string Row_QuickStartTemplate::PK_QuickStartTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_QuickStartTemplate);

return buf;
}

string Row_QuickStartTemplate::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[301];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)150,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::FK_QuickStartCategory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_QuickStartCategory);

return buf;
}

string Row_QuickStartTemplate::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_QuickStartTemplate::Binary_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[421];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Binary.c_str(), (unsigned long) min((size_t)210,m_Binary.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::Arguments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Arguments.c_str(), (unsigned long) min((size_t)765,m_Arguments.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::Homepage_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Homepage.c_str(), (unsigned long) min((size_t)765,m_Homepage.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::Icon_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Icon.c_str(), (unsigned long) min((size_t)765,m_Icon.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::WindowClass_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_WindowClass.c_str(), (unsigned long) min((size_t)765,m_WindowClass.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::FK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj);

return buf;
}

string Row_QuickStartTemplate::FK_DesignObj_OSD_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_OSD);

return buf;
}

string Row_QuickStartTemplate::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_QuickStartTemplate::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_QuickStartTemplate::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_QuickStartTemplate::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_QuickStartTemplate::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_QuickStartTemplate::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_QuickStartTemplate::Key::Key(long int in_PK_QuickStartTemplate)
{
			pk_PK_QuickStartTemplate = in_PK_QuickStartTemplate;
	
}

Table_QuickStartTemplate::Key::Key(Row_QuickStartTemplate *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_QuickStartTemplate = pRow->m_PK_QuickStartTemplate;
	
}		

bool Table_QuickStartTemplate::Key_Less::operator()(const Table_QuickStartTemplate::Key &key1, const Table_QuickStartTemplate::Key &key2) const
{
			if (key1.pk_PK_QuickStartTemplate!=key2.pk_PK_QuickStartTemplate)
return key1.pk_PK_QuickStartTemplate<key2.pk_PK_QuickStartTemplate;
else
return false;	
}	

bool Table_QuickStartTemplate::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_QuickStartTemplate *pRow = (Row_QuickStartTemplate *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_QuickStartTemplate_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_QuickStartCategory_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->Binary_asSQL()+", "+pRow->Arguments_asSQL()+", "+pRow->Homepage_asSQL()+", "+pRow->Icon_asSQL()+", "+pRow->WindowClass_asSQL()+", "+pRow->FK_DesignObj_asSQL()+", "+pRow->FK_DesignObj_OSD_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into QuickStartTemplate (`PK_QuickStartTemplate`, `Description`, `FK_QuickStartCategory`, `FK_Package`, `Binary`, `Arguments`, `Homepage`, `Icon`, `WindowClass`, `FK_DesignObj`, `FK_DesignObj_OSD`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_QuickStartTemplate=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_QuickStartTemplate is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_QuickStartTemplate);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_QuickStartTemplate* pRow = (Row_QuickStartTemplate*) (*i).second;	
		SingleLongKey key(pRow->m_PK_QuickStartTemplate);

		char tmp_PK_QuickStartTemplate[32];
sprintf(tmp_PK_QuickStartTemplate, "%li", key.pk);


string condition;
condition = condition + "`PK_QuickStartTemplate`=" + tmp_PK_QuickStartTemplate;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_QuickStartTemplate`="+pRow->PK_QuickStartTemplate_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_QuickStartCategory`="+pRow->FK_QuickStartCategory_asSQL()+", `FK_Package`="+pRow->FK_Package_asSQL()+", `Binary`="+pRow->Binary_asSQL()+", `Arguments`="+pRow->Arguments_asSQL()+", `Homepage`="+pRow->Homepage_asSQL()+", `Icon`="+pRow->Icon_asSQL()+", `WindowClass`="+pRow->WindowClass_asSQL()+", `FK_DesignObj`="+pRow->FK_DesignObj_asSQL()+", `FK_DesignObj_OSD`="+pRow->FK_DesignObj_OSD_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update QuickStartTemplate set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_QuickStartTemplate* pRow = (Row_QuickStartTemplate*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_QuickStartTemplate* pRow = (Row_QuickStartTemplate*) (*i).second;	

		char tmp_PK_QuickStartTemplate[32];
sprintf(tmp_PK_QuickStartTemplate, "%li", key.pk);


string condition;
condition = condition + "`PK_QuickStartTemplate`=" + tmp_PK_QuickStartTemplate;

	
		string query = "delete from QuickStartTemplate where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_QuickStartTemplate*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_QuickStartTemplate::GetRows(string where_statement,vector<class Row_QuickStartTemplate*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `QuickStartTemplate`.* from QuickStartTemplate " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `QuickStartTemplate`.* from QuickStartTemplate where " + where_statement;
	else
		query = "select `QuickStartTemplate`.* from QuickStartTemplate";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_QuickStartTemplate *pRow = new Row_QuickStartTemplate(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_QuickStartTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_QuickStartTemplate));
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
pRow->m_FK_QuickStartCategory = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_QuickStartCategory));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Package));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Binary = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Binary = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Arguments = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Arguments = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Homepage = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Homepage = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Icon = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Icon = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_WindowClass = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_WindowClass = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DesignObj_OSD = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_DesignObj_OSD));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_id));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_batch));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_user));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_psc_frozen));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_psc_mod = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_QuickStartTemplate);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_QuickStartTemplate *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_QuickStartTemplate* Table_QuickStartTemplate::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_QuickStartTemplate *pRow = new Row_QuickStartTemplate(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_QuickStartTemplate* Table_QuickStartTemplate::GetRow(long int in_PK_QuickStartTemplate)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_QuickStartTemplate);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_QuickStartTemplate*) (*i).second;
	//we have to fetch row
	Row_QuickStartTemplate* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_QuickStartTemplate* Table_QuickStartTemplate::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_QuickStartTemplate[32];
sprintf(tmp_PK_QuickStartTemplate, "%li", key.pk);


string condition;
condition = condition + "`PK_QuickStartTemplate`=" + tmp_PK_QuickStartTemplate;


	string query = "select * from QuickStartTemplate where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_QuickStartTemplate::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_QuickStartTemplate *pRow = new Row_QuickStartTemplate(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_QuickStartTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_QuickStartTemplate));
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
pRow->m_FK_QuickStartCategory = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_QuickStartCategory));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Package));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_Binary = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Binary = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Arguments = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Arguments = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Homepage = "";
}
else
{
pRow->is_null[6]=false;
pRow->m_Homepage = string(row[6],lengths[6]);
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Icon = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_Icon = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_WindowClass = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_WindowClass = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DesignObj_OSD = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_DesignObj_OSD));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_psc_id));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_psc_batch));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_user));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_psc_frozen));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_psc_mod = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_QuickStartCategory* Row_QuickStartTemplate::FK_QuickStartCategory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_QuickStartCategory *pTable = table->database->QuickStartCategory_get();
return pTable->GetRow(m_FK_QuickStartCategory);
}
class Row_Package* Row_QuickStartTemplate::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_DesignObj* Row_QuickStartTemplate::FK_DesignObj_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj);
}
class Row_DesignObj* Row_QuickStartTemplate::FK_DesignObj_OSD_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_OSD);
}


void Row_QuickStartTemplate::Device_QuickStart_FK_QuickStartTemplate_getrows(vector <class Row_Device_QuickStart*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device_QuickStart *pTable = table->database->Device_QuickStart_get();
pTable->GetRows("`FK_QuickStartTemplate`=" + StringUtils::itos(m_PK_QuickStartTemplate),rows);
}



