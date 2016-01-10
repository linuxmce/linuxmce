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
#include "Table_MediaType_DesignObj.h"
#include "Table_MediaType.h"
#include "Table_Screen.h"
#include "Table_DesignObj.h"
#include "Table_Screen.h"
#include "Table_Screen.h"
#include "Table_Screen.h"
#include "Table_Skin.h"

#include "Table_RemoteControl.h"


void Database_pluto_main::CreateTable_MediaType_DesignObj()
{
	tblMediaType_DesignObj = new Table_MediaType_DesignObj(this);
}

void Database_pluto_main::DeleteTable_MediaType_DesignObj()
{
	if( tblMediaType_DesignObj )
		delete tblMediaType_DesignObj;
}

bool Database_pluto_main::Commit_MediaType_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblMediaType_DesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_MediaType_DesignObj::~Table_MediaType_DesignObj()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_MediaType_DesignObj *pRow = (Row_MediaType_DesignObj *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_MediaType_DesignObj *pRow = (Row_MediaType_DesignObj *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_MediaType_DesignObj::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_MediaType_DesignObj *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_MediaType_DesignObj *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_MediaType_DesignObj);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_MediaType_DesignObj::Reload()
{
	Row_MediaType_DesignObj *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_MediaType_DesignObj);
		Row_MediaType_DesignObj *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_MediaType_DesignObj::Row_MediaType_DesignObj(Table_MediaType_DesignObj *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_MediaType_DesignObj::SetDefaultValues()
{
	m_PK_MediaType_DesignObj = 0;
is_null[0] = false;
m_FK_MediaType = 0;
is_null[1] = false;
m_FK_Screen = 0;
is_null[2] = false;
is_null[3] = true;
m_FK_DesignObj_Popup = 0;
is_null[4] = true;
m_FK_Screen_FileList = 0;
is_null[5] = true;
m_FK_Screen_OSD = 0;
is_null[6] = true;
m_FK_Screen_Alt = 0;
is_null[7] = true;
m_FK_Screen_Alt_OSD = 0;
is_null[8] = true;
m_FK_Screen_OSD_Speed = 0;
is_null[9] = true;
m_FK_Screen_OSD_Track = 0;
is_null[10] = true;
is_null[11] = true;
m_FK_Skin = 0;
is_null[12] = true;
m_UIVersion = 0;
is_null[13] = true;
m_psc_id = 0;
is_null[14] = true;
m_psc_batch = 0;
is_null[15] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[16] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[17] = false;
is_null[18] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_MediaType_DesignObj::PK_MediaType_DesignObj_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_MediaType_DesignObj;}
long int Row_MediaType_DesignObj::FK_MediaType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_MediaType;}
long int Row_MediaType_DesignObj::FK_Screen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen;}
long int Row_MediaType_DesignObj::FK_DesignObj_Popup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Popup;}
long int Row_MediaType_DesignObj::FK_Screen_FileList_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_FileList;}
long int Row_MediaType_DesignObj::FK_Screen_OSD_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_OSD;}
long int Row_MediaType_DesignObj::FK_Screen_Alt_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_Alt;}
long int Row_MediaType_DesignObj::FK_Screen_Alt_OSD_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_Alt_OSD;}
long int Row_MediaType_DesignObj::FK_Screen_OSD_Speed_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_OSD_Speed;}
long int Row_MediaType_DesignObj::FK_Screen_OSD_Track_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_OSD_Track;}
string Row_MediaType_DesignObj::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_MediaType_DesignObj::FK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin;}
long int Row_MediaType_DesignObj::UIVersion_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UIVersion;}
long int Row_MediaType_DesignObj::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_MediaType_DesignObj::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_MediaType_DesignObj::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_MediaType_DesignObj::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_MediaType_DesignObj::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_MediaType_DesignObj::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_MediaType_DesignObj::PK_MediaType_DesignObj_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_MediaType_DesignObj = val; is_modified=true; is_null[0]=false;}
void Row_MediaType_DesignObj::FK_MediaType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_MediaType = val; is_modified=true; is_null[1]=false;}
void Row_MediaType_DesignObj::FK_Screen_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen = val; is_modified=true; is_null[2]=false;}
void Row_MediaType_DesignObj::FK_DesignObj_Popup_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Popup = val; is_modified=true; is_null[3]=false;}
void Row_MediaType_DesignObj::FK_Screen_FileList_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_FileList = val; is_modified=true; is_null[4]=false;}
void Row_MediaType_DesignObj::FK_Screen_OSD_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_OSD = val; is_modified=true; is_null[5]=false;}
void Row_MediaType_DesignObj::FK_Screen_Alt_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_Alt = val; is_modified=true; is_null[6]=false;}
void Row_MediaType_DesignObj::FK_Screen_Alt_OSD_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_Alt_OSD = val; is_modified=true; is_null[7]=false;}
void Row_MediaType_DesignObj::FK_Screen_OSD_Speed_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_OSD_Speed = val; is_modified=true; is_null[8]=false;}
void Row_MediaType_DesignObj::FK_Screen_OSD_Track_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_OSD_Track = val; is_modified=true; is_null[9]=false;}
void Row_MediaType_DesignObj::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[10]=false;}
void Row_MediaType_DesignObj::FK_Skin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin = val; is_modified=true; is_null[11]=false;}
void Row_MediaType_DesignObj::UIVersion_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UIVersion = val; is_modified=true; is_null[12]=false;}
void Row_MediaType_DesignObj::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[13]=false;}
void Row_MediaType_DesignObj::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[14]=false;}
void Row_MediaType_DesignObj::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[15]=false;}
void Row_MediaType_DesignObj::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[16]=false;}
void Row_MediaType_DesignObj::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[17]=false;}
void Row_MediaType_DesignObj::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[18]=false;}

		
bool Row_MediaType_DesignObj::FK_DesignObj_Popup_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_MediaType_DesignObj::FK_Screen_FileList_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_MediaType_DesignObj::FK_Screen_OSD_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_MediaType_DesignObj::FK_Screen_Alt_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_MediaType_DesignObj::FK_Screen_Alt_OSD_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_MediaType_DesignObj::FK_Screen_OSD_Speed_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_MediaType_DesignObj::FK_Screen_OSD_Track_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_MediaType_DesignObj::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_MediaType_DesignObj::FK_Skin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_MediaType_DesignObj::UIVersion_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_MediaType_DesignObj::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_MediaType_DesignObj::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_MediaType_DesignObj::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_MediaType_DesignObj::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_MediaType_DesignObj::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}

			
void Row_MediaType_DesignObj::FK_DesignObj_Popup_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Screen_FileList_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Screen_OSD_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Screen_Alt_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Screen_Alt_OSD_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Screen_OSD_Speed_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Screen_OSD_Track_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::FK_Skin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::UIVersion_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_MediaType_DesignObj::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
	

string Row_MediaType_DesignObj::PK_MediaType_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_MediaType_DesignObj);

return buf;
}

string Row_MediaType_DesignObj::FK_MediaType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_MediaType);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen);

return buf;
}

string Row_MediaType_DesignObj::FK_DesignObj_Popup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Popup);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_FileList_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_FileList);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_OSD_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_OSD);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_Alt_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_Alt);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_Alt_OSD_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_Alt_OSD);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_OSD_Speed_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_OSD_Speed);

return buf;
}

string Row_MediaType_DesignObj::FK_Screen_OSD_Track_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_OSD_Track);

return buf;
}

string Row_MediaType_DesignObj::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)50,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MediaType_DesignObj::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin);

return buf;
}

string Row_MediaType_DesignObj::UIVersion_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_UIVersion);

return buf;
}

string Row_MediaType_DesignObj::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_MediaType_DesignObj::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_MediaType_DesignObj::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_MediaType_DesignObj::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_MediaType_DesignObj::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_MediaType_DesignObj::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_MediaType_DesignObj::Key::Key(long int in_PK_MediaType_DesignObj)
{
			pk_PK_MediaType_DesignObj = in_PK_MediaType_DesignObj;
	
}

Table_MediaType_DesignObj::Key::Key(Row_MediaType_DesignObj *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_MediaType_DesignObj = pRow->m_PK_MediaType_DesignObj;
	
}		

bool Table_MediaType_DesignObj::Key_Less::operator()(const Table_MediaType_DesignObj::Key &key1, const Table_MediaType_DesignObj::Key &key2) const
{
			if (key1.pk_PK_MediaType_DesignObj!=key2.pk_PK_MediaType_DesignObj)
return key1.pk_PK_MediaType_DesignObj<key2.pk_PK_MediaType_DesignObj;
else
return false;	
}	

bool Table_MediaType_DesignObj::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_MediaType_DesignObj *pRow = (Row_MediaType_DesignObj *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_MediaType_DesignObj_asSQL()+", "+pRow->FK_MediaType_asSQL()+", "+pRow->FK_Screen_asSQL()+", "+pRow->FK_DesignObj_Popup_asSQL()+", "+pRow->FK_Screen_FileList_asSQL()+", "+pRow->FK_Screen_OSD_asSQL()+", "+pRow->FK_Screen_Alt_asSQL()+", "+pRow->FK_Screen_Alt_OSD_asSQL()+", "+pRow->FK_Screen_OSD_Speed_asSQL()+", "+pRow->FK_Screen_OSD_Track_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->UIVersion_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into MediaType_DesignObj (`PK_MediaType_DesignObj`, `FK_MediaType`, `FK_Screen`, `FK_DesignObj_Popup`, `FK_Screen_FileList`, `FK_Screen_OSD`, `FK_Screen_Alt`, `FK_Screen_Alt_OSD`, `FK_Screen_OSD_Speed`, `FK_Screen_OSD_Track`, `Description`, `FK_Skin`, `UIVersion`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_MediaType_DesignObj=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_MediaType_DesignObj is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_MediaType_DesignObj);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_MediaType_DesignObj* pRow = (Row_MediaType_DesignObj*) (*i).second;	
		SingleLongKey key(pRow->m_PK_MediaType_DesignObj);

		char tmp_PK_MediaType_DesignObj[32];
sprintf(tmp_PK_MediaType_DesignObj, "%li", key.pk);


string condition;
condition = condition + "`PK_MediaType_DesignObj`=" + tmp_PK_MediaType_DesignObj;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_MediaType_DesignObj`="+pRow->PK_MediaType_DesignObj_asSQL()+", `FK_MediaType`="+pRow->FK_MediaType_asSQL()+", `FK_Screen`="+pRow->FK_Screen_asSQL()+", `FK_DesignObj_Popup`="+pRow->FK_DesignObj_Popup_asSQL()+", `FK_Screen_FileList`="+pRow->FK_Screen_FileList_asSQL()+", `FK_Screen_OSD`="+pRow->FK_Screen_OSD_asSQL()+", `FK_Screen_Alt`="+pRow->FK_Screen_Alt_asSQL()+", `FK_Screen_Alt_OSD`="+pRow->FK_Screen_Alt_OSD_asSQL()+", `FK_Screen_OSD_Speed`="+pRow->FK_Screen_OSD_Speed_asSQL()+", `FK_Screen_OSD_Track`="+pRow->FK_Screen_OSD_Track_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_Skin`="+pRow->FK_Skin_asSQL()+", `UIVersion`="+pRow->UIVersion_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update MediaType_DesignObj set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_MediaType_DesignObj* pRow = (Row_MediaType_DesignObj*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_MediaType_DesignObj* pRow = (Row_MediaType_DesignObj*) (*i).second;	

		char tmp_PK_MediaType_DesignObj[32];
sprintf(tmp_PK_MediaType_DesignObj, "%li", key.pk);


string condition;
condition = condition + "`PK_MediaType_DesignObj`=" + tmp_PK_MediaType_DesignObj;

	
		string query = "delete from MediaType_DesignObj where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_MediaType_DesignObj*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_MediaType_DesignObj::GetRows(string where_statement,vector<class Row_MediaType_DesignObj*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `MediaType_DesignObj`.* from MediaType_DesignObj " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `MediaType_DesignObj`.* from MediaType_DesignObj where " + where_statement;
	else
		query = "select `MediaType_DesignObj`.* from MediaType_DesignObj";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_MediaType_DesignObj *pRow = new Row_MediaType_DesignObj(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_MediaType_DesignObj = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_MediaType_DesignObj));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_MediaType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_MediaType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Screen = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Screen));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DesignObj_Popup = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObj_Popup));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Screen_FileList = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Screen_FileList));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_Screen_OSD = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_Screen_OSD));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Screen_Alt = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Screen_Alt));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Screen_Alt_OSD = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Screen_Alt_OSD));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Screen_OSD_Speed = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Screen_OSD_Speed));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Screen_OSD_Track = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Screen_OSD_Track));
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
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Skin));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_UIVersion = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_UIVersion));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_id));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_batch));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_user));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_psc_frozen));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_psc_mod = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_MediaType_DesignObj);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_MediaType_DesignObj *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_MediaType_DesignObj* Table_MediaType_DesignObj::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_MediaType_DesignObj *pRow = new Row_MediaType_DesignObj(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_MediaType_DesignObj* Table_MediaType_DesignObj::GetRow(long int in_PK_MediaType_DesignObj)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_MediaType_DesignObj);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_MediaType_DesignObj*) (*i).second;
	//we have to fetch row
	Row_MediaType_DesignObj* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_MediaType_DesignObj* Table_MediaType_DesignObj::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_MediaType_DesignObj[32];
sprintf(tmp_PK_MediaType_DesignObj, "%li", key.pk);


string condition;
condition = condition + "`PK_MediaType_DesignObj`=" + tmp_PK_MediaType_DesignObj;


	string query = "select * from MediaType_DesignObj where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_MediaType_DesignObj::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_MediaType_DesignObj *pRow = new Row_MediaType_DesignObj(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_MediaType_DesignObj = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_MediaType_DesignObj));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_MediaType = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_MediaType));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Screen = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Screen));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DesignObj_Popup = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObj_Popup));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Screen_FileList = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Screen_FileList));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_Screen_OSD = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_Screen_OSD));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Screen_Alt = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Screen_Alt));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Screen_Alt_OSD = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Screen_Alt_OSD));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Screen_OSD_Speed = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_FK_Screen_OSD_Speed));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_Screen_OSD_Track = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_Screen_OSD_Track));
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
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Skin));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_UIVersion = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_UIVersion));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_psc_id));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_psc_batch));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_psc_user));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_psc_frozen));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_psc_mod = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_MediaType* Row_MediaType_DesignObj::FK_MediaType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_MediaType *pTable = table->database->MediaType_get();
return pTable->GetRow(m_FK_MediaType);
}
class Row_Screen* Row_MediaType_DesignObj::FK_Screen_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen);
}
class Row_DesignObj* Row_MediaType_DesignObj::FK_DesignObj_Popup_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Popup);
}
class Row_Screen* Row_MediaType_DesignObj::FK_Screen_FileList_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen_FileList);
}
class Row_Screen* Row_MediaType_DesignObj::FK_Screen_OSD_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen_OSD);
}
class Row_Screen* Row_MediaType_DesignObj::FK_Screen_Alt_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen_Alt);
}
class Row_Skin* Row_MediaType_DesignObj::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}


void Row_MediaType_DesignObj::RemoteControl_FK_MediaType_DesignObj_getrows(vector <class Row_RemoteControl*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_RemoteControl *pTable = table->database->RemoteControl_get();
pTable->GetRows("`FK_MediaType_DesignObj`=" + StringUtils::itos(m_PK_MediaType_DesignObj),rows);
}



