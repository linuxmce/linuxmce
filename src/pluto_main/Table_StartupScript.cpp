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
#include "Table_StartupScript.h"
#include "Table_DeviceTemplate.h"

#include "Table_Device_StartupScript.h"


void Database_pluto_main::CreateTable_StartupScript()
{
	tblStartupScript = new Table_StartupScript(this);
}

void Database_pluto_main::DeleteTable_StartupScript()
{
	if( tblStartupScript )
		delete tblStartupScript;
}

bool Database_pluto_main::Commit_StartupScript(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblStartupScript->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_StartupScript::~Table_StartupScript()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_StartupScript *pRow = (Row_StartupScript *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_StartupScript *pRow = (Row_StartupScript *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_StartupScript::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_StartupScript *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_StartupScript *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_StartupScript);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_StartupScript::Reload()
{
	Row_StartupScript *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_StartupScript);
		Row_StartupScript *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_StartupScript::Row_StartupScript(Table_StartupScript *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_StartupScript::SetDefaultValues()
{
	m_PK_StartupScript = 0;
is_null[0] = false;
m_When = "S";
is_null[1] = false;
m_Command = "";
is_null[2] = false;
m_Description = "";
is_null[3] = false;
m_ConfigureOnly = 0;
is_null[4] = false;
m_Parameter_Syntax = "";
is_null[5] = false;
m_Core_Boot_Order = 99;
is_null[6] = false;
m_Core_Background = 0;
is_null[7] = false;
m_Core_Enabled = 1;
is_null[8] = false;
m_Core_Parameter = "";
is_null[9] = false;
m_MD_Boot_Order = 99;
is_null[10] = false;
m_MD_Background = 0;
is_null[11] = false;
m_MD_Enabled = 1;
is_null[12] = false;
m_MD_Parameter = "";
is_null[13] = false;
m_Hybrid_Boot_Order = 99;
is_null[14] = false;
m_Hybrid_Background = 0;
is_null[15] = false;
m_Hybrid_Enabled = 1;
is_null[16] = false;
m_Hybrid_Parameter = "";
is_null[17] = false;
is_null[18] = true;
m_FK_DeviceTemplate = 0;
is_null[19] = true;
m_psc_id = 0;
is_null[20] = true;
m_psc_batch = 0;
is_null[21] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[22] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[23] = false;
is_null[24] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_StartupScript::PK_StartupScript_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_StartupScript;}
string Row_StartupScript::When_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_When;}
string Row_StartupScript::Command_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Command;}
string Row_StartupScript::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
short int Row_StartupScript::ConfigureOnly_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ConfigureOnly;}
string Row_StartupScript::Parameter_Syntax_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Parameter_Syntax;}
long int Row_StartupScript::Core_Boot_Order_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Core_Boot_Order;}
long int Row_StartupScript::Core_Background_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Core_Background;}
long int Row_StartupScript::Core_Enabled_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Core_Enabled;}
string Row_StartupScript::Core_Parameter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Core_Parameter;}
long int Row_StartupScript::MD_Boot_Order_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MD_Boot_Order;}
short int Row_StartupScript::MD_Background_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MD_Background;}
short int Row_StartupScript::MD_Enabled_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MD_Enabled;}
string Row_StartupScript::MD_Parameter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MD_Parameter;}
long int Row_StartupScript::Hybrid_Boot_Order_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Hybrid_Boot_Order;}
short int Row_StartupScript::Hybrid_Background_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Hybrid_Background;}
short int Row_StartupScript::Hybrid_Enabled_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Hybrid_Enabled;}
string Row_StartupScript::Hybrid_Parameter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Hybrid_Parameter;}
long int Row_StartupScript::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceTemplate;}
long int Row_StartupScript::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_StartupScript::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_StartupScript::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_StartupScript::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_StartupScript::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_StartupScript::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_StartupScript::PK_StartupScript_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_StartupScript = val; is_modified=true; is_null[0]=false;}
void Row_StartupScript::When_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_When = val; is_modified=true; is_null[1]=false;}
void Row_StartupScript::Command_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Command = val; is_modified=true; is_null[2]=false;}
void Row_StartupScript::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[3]=false;}
void Row_StartupScript::ConfigureOnly_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ConfigureOnly = val; is_modified=true; is_null[4]=false;}
void Row_StartupScript::Parameter_Syntax_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Parameter_Syntax = val; is_modified=true; is_null[5]=false;}
void Row_StartupScript::Core_Boot_Order_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Core_Boot_Order = val; is_modified=true; is_null[6]=false;}
void Row_StartupScript::Core_Background_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Core_Background = val; is_modified=true; is_null[7]=false;}
void Row_StartupScript::Core_Enabled_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Core_Enabled = val; is_modified=true; is_null[8]=false;}
void Row_StartupScript::Core_Parameter_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Core_Parameter = val; is_modified=true; is_null[9]=false;}
void Row_StartupScript::MD_Boot_Order_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MD_Boot_Order = val; is_modified=true; is_null[10]=false;}
void Row_StartupScript::MD_Background_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MD_Background = val; is_modified=true; is_null[11]=false;}
void Row_StartupScript::MD_Enabled_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MD_Enabled = val; is_modified=true; is_null[12]=false;}
void Row_StartupScript::MD_Parameter_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MD_Parameter = val; is_modified=true; is_null[13]=false;}
void Row_StartupScript::Hybrid_Boot_Order_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Hybrid_Boot_Order = val; is_modified=true; is_null[14]=false;}
void Row_StartupScript::Hybrid_Background_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Hybrid_Background = val; is_modified=true; is_null[15]=false;}
void Row_StartupScript::Hybrid_Enabled_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Hybrid_Enabled = val; is_modified=true; is_null[16]=false;}
void Row_StartupScript::Hybrid_Parameter_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Hybrid_Parameter = val; is_modified=true; is_null[17]=false;}
void Row_StartupScript::FK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[18]=false;}
void Row_StartupScript::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[19]=false;}
void Row_StartupScript::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[20]=false;}
void Row_StartupScript::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[21]=false;}
void Row_StartupScript::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[22]=false;}
void Row_StartupScript::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[23]=false;}
void Row_StartupScript::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[24]=false;}

		
bool Row_StartupScript::When_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_StartupScript::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_StartupScript::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_StartupScript::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_StartupScript::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_StartupScript::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_StartupScript::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}

			
void Row_StartupScript::When_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_StartupScript::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_StartupScript::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_StartupScript::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_StartupScript::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_StartupScript::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_StartupScript::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
	

string Row_StartupScript::PK_StartupScript_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_StartupScript);

return buf;
}

string Row_StartupScript::When_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[7];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_When.c_str(), (unsigned long) min((size_t)3,m_When.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::Command_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Command.c_str(), (unsigned long) min((size_t)300,m_Command.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[393211];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)196605,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::ConfigureOnly_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ConfigureOnly);

return buf;
}

string Row_StartupScript::Parameter_Syntax_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[393211];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Parameter_Syntax.c_str(), (unsigned long) min((size_t)196605,m_Parameter_Syntax.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::Core_Boot_Order_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Core_Boot_Order);

return buf;
}

string Row_StartupScript::Core_Background_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Core_Background);

return buf;
}

string Row_StartupScript::Core_Enabled_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Core_Enabled);

return buf;
}

string Row_StartupScript::Core_Parameter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Core_Parameter.c_str(), (unsigned long) min((size_t)300,m_Core_Parameter.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::MD_Boot_Order_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_MD_Boot_Order);

return buf;
}

string Row_StartupScript::MD_Background_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MD_Background);

return buf;
}

string Row_StartupScript::MD_Enabled_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MD_Enabled);

return buf;
}

string Row_StartupScript::MD_Parameter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_MD_Parameter.c_str(), (unsigned long) min((size_t)300,m_MD_Parameter.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::Hybrid_Boot_Order_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Hybrid_Boot_Order);

return buf;
}

string Row_StartupScript::Hybrid_Background_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Hybrid_Background);

return buf;
}

string Row_StartupScript::Hybrid_Enabled_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Hybrid_Enabled);

return buf;
}

string Row_StartupScript::Hybrid_Parameter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Hybrid_Parameter.c_str(), (unsigned long) min((size_t)300,m_Hybrid_Parameter.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceTemplate);

return buf;
}

string Row_StartupScript::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_StartupScript::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_StartupScript::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_StartupScript::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_StartupScript::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StartupScript::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_StartupScript::Key::Key(long int in_PK_StartupScript)
{
			pk_PK_StartupScript = in_PK_StartupScript;
	
}

Table_StartupScript::Key::Key(Row_StartupScript *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_StartupScript = pRow->m_PK_StartupScript;
	
}		

bool Table_StartupScript::Key_Less::operator()(const Table_StartupScript::Key &key1, const Table_StartupScript::Key &key2) const
{
			if (key1.pk_PK_StartupScript!=key2.pk_PK_StartupScript)
return key1.pk_PK_StartupScript<key2.pk_PK_StartupScript;
else
return false;	
}	

bool Table_StartupScript::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_StartupScript *pRow = (Row_StartupScript *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_StartupScript_asSQL()+", "+pRow->When_asSQL()+", "+pRow->Command_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->ConfigureOnly_asSQL()+", "+pRow->Parameter_Syntax_asSQL()+", "+pRow->Core_Boot_Order_asSQL()+", "+pRow->Core_Background_asSQL()+", "+pRow->Core_Enabled_asSQL()+", "+pRow->Core_Parameter_asSQL()+", "+pRow->MD_Boot_Order_asSQL()+", "+pRow->MD_Background_asSQL()+", "+pRow->MD_Enabled_asSQL()+", "+pRow->MD_Parameter_asSQL()+", "+pRow->Hybrid_Boot_Order_asSQL()+", "+pRow->Hybrid_Background_asSQL()+", "+pRow->Hybrid_Enabled_asSQL()+", "+pRow->Hybrid_Parameter_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into StartupScript (`PK_StartupScript`, `When`, `Command`, `Description`, `ConfigureOnly`, `Parameter_Syntax`, `Core_Boot_Order`, `Core_Background`, `Core_Enabled`, `Core_Parameter`, `MD_Boot_Order`, `MD_Background`, `MD_Enabled`, `MD_Parameter`, `Hybrid_Boot_Order`, `Hybrid_Background`, `Hybrid_Enabled`, `Hybrid_Parameter`, `FK_DeviceTemplate`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_StartupScript=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_StartupScript is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_StartupScript);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_StartupScript* pRow = (Row_StartupScript*) (*i).second;	
		SingleLongKey key(pRow->m_PK_StartupScript);

		char tmp_PK_StartupScript[32];
sprintf(tmp_PK_StartupScript, "%li", key.pk);


string condition;
condition = condition + "`PK_StartupScript`=" + tmp_PK_StartupScript;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_StartupScript`="+pRow->PK_StartupScript_asSQL()+", `When`="+pRow->When_asSQL()+", `Command`="+pRow->Command_asSQL()+", `Description`="+pRow->Description_asSQL()+", `ConfigureOnly`="+pRow->ConfigureOnly_asSQL()+", `Parameter_Syntax`="+pRow->Parameter_Syntax_asSQL()+", `Core_Boot_Order`="+pRow->Core_Boot_Order_asSQL()+", `Core_Background`="+pRow->Core_Background_asSQL()+", `Core_Enabled`="+pRow->Core_Enabled_asSQL()+", `Core_Parameter`="+pRow->Core_Parameter_asSQL()+", `MD_Boot_Order`="+pRow->MD_Boot_Order_asSQL()+", `MD_Background`="+pRow->MD_Background_asSQL()+", `MD_Enabled`="+pRow->MD_Enabled_asSQL()+", `MD_Parameter`="+pRow->MD_Parameter_asSQL()+", `Hybrid_Boot_Order`="+pRow->Hybrid_Boot_Order_asSQL()+", `Hybrid_Background`="+pRow->Hybrid_Background_asSQL()+", `Hybrid_Enabled`="+pRow->Hybrid_Enabled_asSQL()+", `Hybrid_Parameter`="+pRow->Hybrid_Parameter_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update StartupScript set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_StartupScript* pRow = (Row_StartupScript*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_StartupScript* pRow = (Row_StartupScript*) (*i).second;	

		char tmp_PK_StartupScript[32];
sprintf(tmp_PK_StartupScript, "%li", key.pk);


string condition;
condition = condition + "`PK_StartupScript`=" + tmp_PK_StartupScript;

	
		string query = "delete from StartupScript where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_StartupScript*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_StartupScript::GetRows(string where_statement,vector<class Row_StartupScript*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `StartupScript`.* from StartupScript " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `StartupScript`.* from StartupScript where " + where_statement;
	else
		query = "select `StartupScript`.* from StartupScript";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_StartupScript *pRow = new Row_StartupScript(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_StartupScript = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_StartupScript));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_When = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_When = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Command = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Command = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ConfigureOnly = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_ConfigureOnly));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Parameter_Syntax = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Parameter_Syntax = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Core_Boot_Order = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_Core_Boot_Order));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Core_Background = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Core_Background));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Core_Enabled = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_Core_Enabled));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Core_Parameter = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Core_Parameter = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_MD_Boot_Order = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_MD_Boot_Order));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_MD_Background = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_MD_Background));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_MD_Enabled = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_MD_Enabled));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_MD_Parameter = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_MD_Parameter = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Hybrid_Boot_Order = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_Hybrid_Boot_Order));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Hybrid_Background = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_Hybrid_Background));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Hybrid_Enabled = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_Hybrid_Enabled));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Hybrid_Parameter = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Hybrid_Parameter = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_id));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_psc_batch));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%li", &(pRow->m_psc_user));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%hi", &(pRow->m_psc_frozen));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_psc_mod = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_StartupScript);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_StartupScript *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_StartupScript* Table_StartupScript::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_StartupScript *pRow = new Row_StartupScript(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_StartupScript* Table_StartupScript::GetRow(long int in_PK_StartupScript)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_StartupScript);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_StartupScript*) (*i).second;
	//we have to fetch row
	Row_StartupScript* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_StartupScript* Table_StartupScript::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_StartupScript[32];
sprintf(tmp_PK_StartupScript, "%li", key.pk);


string condition;
condition = condition + "`PK_StartupScript`=" + tmp_PK_StartupScript;


	string query = "select * from StartupScript where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StartupScript::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_StartupScript *pRow = new Row_StartupScript(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_StartupScript = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_StartupScript));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_When = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_When = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Command = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Command = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Description = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_Description = string(row[3],lengths[3]);
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_ConfigureOnly = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_ConfigureOnly));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Parameter_Syntax = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Parameter_Syntax = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Core_Boot_Order = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_Core_Boot_Order));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Core_Background = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Core_Background));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Core_Enabled = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_Core_Enabled));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Core_Parameter = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Core_Parameter = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_MD_Boot_Order = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_MD_Boot_Order));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_MD_Background = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_MD_Background));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_MD_Enabled = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_MD_Enabled));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_MD_Parameter = "";
}
else
{
pRow->is_null[13]=false;
pRow->m_MD_Parameter = string(row[13],lengths[13]);
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_Hybrid_Boot_Order = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_Hybrid_Boot_Order));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Hybrid_Background = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_Hybrid_Background));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Hybrid_Enabled = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_Hybrid_Enabled));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Hybrid_Parameter = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Hybrid_Parameter = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_DeviceTemplate));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_id));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_psc_batch));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%li", &(pRow->m_psc_user));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%hi", &(pRow->m_psc_frozen));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_psc_mod = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DeviceTemplate* Row_StartupScript::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}


void Row_StartupScript::Device_StartupScript_FK_StartupScript_getrows(vector <class Row_Device_StartupScript*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device_StartupScript *pTable = table->database->Device_StartupScript_get();
pTable->GetRows("`FK_StartupScript`=" + StringUtils::itos(m_PK_StartupScript),rows);
}



