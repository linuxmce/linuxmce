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
#include "Table_Skin.h"
#include "Table_UI.h"
#include "Table_Style.h"
#include "Table_Skin.h"
#include "Table_StabilityStatus.h"
#include "Table_Screen.h"
#include "Table_Screen.h"
#include "Table_Screen.h"
#include "Table_Package.h"

#include "Table_DesignObj.h"
#include "Table_DesignObjVariation_DesignObjParameter.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_Text_Skin_Language.h"
#include "Table_DeviceTemplate_MediaType_DesignObj.h"
#include "Table_EffectType_Effect_Skin.h"
#include "Table_MediaType_DesignObj.h"
#include "Table_Screen_DesignObj.h"
#include "Table_Skin.h"
#include "Table_StyleVariation.h"


void Database_pluto_main::CreateTable_Skin()
{
	tblSkin = new Table_Skin(this);
}

void Database_pluto_main::DeleteTable_Skin()
{
	if( tblSkin )
		delete tblSkin;
}

bool Database_pluto_main::Commit_Skin(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblSkin->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Skin::~Table_Skin()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Skin *pRow = (Row_Skin *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Skin *pRow = (Row_Skin *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Skin::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Skin *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Skin *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Skin);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Skin::Reload()
{
	Row_Skin *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Skin);
		Row_Skin *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Skin::Row_Skin(Table_Skin *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Skin::SetDefaultValues()
{
	m_PK_Skin = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
is_null[3] = true;
m_FK_UI = 0;
m_MergeStandardVariation = 1;
is_null[4] = false;
is_null[5] = true;
is_null[6] = true;
m_FK_Style = 0;
is_null[7] = true;
m_FK_Skin_TextPlacement = 0;
m_DrawTextBeforeChildren = 0;
is_null[8] = false;
m_FK_StabilityStatus = 1;
is_null[9] = false;
m_FK_Screen_MainMenu = 1;
is_null[10] = false;
m_FK_Screen_Sleeping = 29;
is_null[11] = false;
m_FK_Screen_ScreenSaver = 74;
is_null[12] = false;
is_null[13] = true;
m_FK_Package = 0;
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

long int Row_Skin::PK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Skin;}
string Row_Skin::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_Skin::Define_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Define;}
long int Row_Skin::FK_UI_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_UI;}
short int Row_Skin::MergeStandardVariation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_MergeStandardVariation;}
string Row_Skin::DataSubdirectory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DataSubdirectory;}
long int Row_Skin::FK_Style_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Style;}
long int Row_Skin::FK_Skin_TextPlacement_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin_TextPlacement;}
short int Row_Skin::DrawTextBeforeChildren_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DrawTextBeforeChildren;}
long int Row_Skin::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_StabilityStatus;}
long int Row_Skin::FK_Screen_MainMenu_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_MainMenu;}
long int Row_Skin::FK_Screen_Sleeping_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_Sleeping;}
long int Row_Skin::FK_Screen_ScreenSaver_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Screen_ScreenSaver;}
long int Row_Skin::FK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Package;}
long int Row_Skin::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Skin::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Skin::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Skin::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Skin::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Skin::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Skin::PK_Skin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Skin = val; is_modified=true; is_null[0]=false;}
void Row_Skin::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Skin::Define_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_Skin::FK_UI_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_UI = val; is_modified=true; is_null[3]=false;}
void Row_Skin::MergeStandardVariation_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_MergeStandardVariation = val; is_modified=true; is_null[4]=false;}
void Row_Skin::DataSubdirectory_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DataSubdirectory = val; is_modified=true; is_null[5]=false;}
void Row_Skin::FK_Style_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Style = val; is_modified=true; is_null[6]=false;}
void Row_Skin::FK_Skin_TextPlacement_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin_TextPlacement = val; is_modified=true; is_null[7]=false;}
void Row_Skin::DrawTextBeforeChildren_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DrawTextBeforeChildren = val; is_modified=true; is_null[8]=false;}
void Row_Skin::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[9]=false;}
void Row_Skin::FK_Screen_MainMenu_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_MainMenu = val; is_modified=true; is_null[10]=false;}
void Row_Skin::FK_Screen_Sleeping_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_Sleeping = val; is_modified=true; is_null[11]=false;}
void Row_Skin::FK_Screen_ScreenSaver_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Screen_ScreenSaver = val; is_modified=true; is_null[12]=false;}
void Row_Skin::FK_Package_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Package = val; is_modified=true; is_null[13]=false;}
void Row_Skin::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[14]=false;}
void Row_Skin::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[15]=false;}
void Row_Skin::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[16]=false;}
void Row_Skin::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[17]=false;}
void Row_Skin::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[18]=false;}
void Row_Skin::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[19]=false;}

		
bool Row_Skin::Define_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Skin::FK_UI_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Skin::DataSubdirectory_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Skin::FK_Style_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Skin::FK_Skin_TextPlacement_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Skin::FK_Package_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_Skin::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_Skin::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Skin::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_Skin::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_Skin::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}

			
void Row_Skin::Define_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Skin::FK_UI_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Skin::DataSubdirectory_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Skin::FK_Style_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Skin::FK_Skin_TextPlacement_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Skin::FK_Package_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Skin::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Skin::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Skin::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_Skin::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Skin::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
	

string Row_Skin::PK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Skin);

return buf;
}

string Row_Skin::Description_asSQL()
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

string Row_Skin::Define_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Define.c_str(), (unsigned long) min((size_t)30,m_Define.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Skin::FK_UI_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_UI);

return buf;
}

string Row_Skin::MergeStandardVariation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_MergeStandardVariation);

return buf;
}

string Row_Skin::DataSubdirectory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DataSubdirectory.c_str(), (unsigned long) min((size_t)30,m_DataSubdirectory.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Skin::FK_Style_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style);

return buf;
}

string Row_Skin::FK_Skin_TextPlacement_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin_TextPlacement);

return buf;
}

string Row_Skin::DrawTextBeforeChildren_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DrawTextBeforeChildren);

return buf;
}

string Row_Skin::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_Skin::FK_Screen_MainMenu_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_MainMenu);

return buf;
}

string Row_Skin::FK_Screen_Sleeping_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_Sleeping);

return buf;
}

string Row_Skin::FK_Screen_ScreenSaver_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Screen_ScreenSaver);

return buf;
}

string Row_Skin::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_Skin::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Skin::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Skin::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Skin::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Skin::psc_mod_asSQL()
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

string Row_Skin::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Skin::Key::Key(long int in_PK_Skin)
{
			pk_PK_Skin = in_PK_Skin;
	
}

Table_Skin::Key::Key(Row_Skin *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Skin = pRow->m_PK_Skin;
	
}		

bool Table_Skin::Key_Less::operator()(const Table_Skin::Key &key1, const Table_Skin::Key &key2) const
{
			if (key1.pk_PK_Skin!=key2.pk_PK_Skin)
return key1.pk_PK_Skin<key2.pk_PK_Skin;
else
return false;	
}	

bool Table_Skin::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Skin *pRow = (Row_Skin *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Skin_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->FK_UI_asSQL()+", "+pRow->MergeStandardVariation_asSQL()+", "+pRow->DataSubdirectory_asSQL()+", "+pRow->FK_Style_asSQL()+", "+pRow->FK_Skin_TextPlacement_asSQL()+", "+pRow->DrawTextBeforeChildren_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->FK_Screen_MainMenu_asSQL()+", "+pRow->FK_Screen_Sleeping_asSQL()+", "+pRow->FK_Screen_ScreenSaver_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Skin (`PK_Skin`, `Description`, `Define`, `FK_UI`, `MergeStandardVariation`, `DataSubdirectory`, `FK_Style`, `FK_Skin_TextPlacement`, `DrawTextBeforeChildren`, `FK_StabilityStatus`, `FK_Screen_MainMenu`, `FK_Screen_Sleeping`, `FK_Screen_ScreenSaver`, `FK_Package`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Skin=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Skin is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Skin);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Skin* pRow = (Row_Skin*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Skin);

		char tmp_PK_Skin[32];
sprintf(tmp_PK_Skin, "%li", key.pk);


string condition;
condition = condition + "`PK_Skin`=" + tmp_PK_Skin;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Skin`="+pRow->PK_Skin_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `FK_UI`="+pRow->FK_UI_asSQL()+", `MergeStandardVariation`="+pRow->MergeStandardVariation_asSQL()+", `DataSubdirectory`="+pRow->DataSubdirectory_asSQL()+", `FK_Style`="+pRow->FK_Style_asSQL()+", `FK_Skin_TextPlacement`="+pRow->FK_Skin_TextPlacement_asSQL()+", `DrawTextBeforeChildren`="+pRow->DrawTextBeforeChildren_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `FK_Screen_MainMenu`="+pRow->FK_Screen_MainMenu_asSQL()+", `FK_Screen_Sleeping`="+pRow->FK_Screen_Sleeping_asSQL()+", `FK_Screen_ScreenSaver`="+pRow->FK_Screen_ScreenSaver_asSQL()+", `FK_Package`="+pRow->FK_Package_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Skin set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Skin* pRow = (Row_Skin*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Skin* pRow = (Row_Skin*) (*i).second;	

		char tmp_PK_Skin[32];
sprintf(tmp_PK_Skin, "%li", key.pk);


string condition;
condition = condition + "`PK_Skin`=" + tmp_PK_Skin;

	
		string query = "delete from Skin where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Skin*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Skin::GetRows(string where_statement,vector<class Row_Skin*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Skin`.* from Skin " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Skin`.* from Skin where " + where_statement;
	else
		query = "select `Skin`.* from Skin";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Skin *pRow = new Row_Skin(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Skin = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Skin));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_UI = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_UI));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_MergeStandardVariation = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_MergeStandardVariation));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_DataSubdirectory = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_DataSubdirectory = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Style));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Skin_TextPlacement = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Skin_TextPlacement));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_DrawTextBeforeChildren = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_DrawTextBeforeChildren));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_Screen_MainMenu = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_Screen_MainMenu));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Screen_Sleeping = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Screen_Sleeping));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Screen_ScreenSaver = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Screen_ScreenSaver));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_Package));
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

		SingleLongKey key(pRow->m_PK_Skin);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Skin *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Skin* Table_Skin::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Skin *pRow = new Row_Skin(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Skin* Table_Skin::GetRow(long int in_PK_Skin)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Skin);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Skin*) (*i).second;
	//we have to fetch row
	Row_Skin* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Skin* Table_Skin::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Skin[32];
sprintf(tmp_PK_Skin, "%li", key.pk);


string condition;
condition = condition + "`PK_Skin`=" + tmp_PK_Skin;


	string query = "select * from Skin where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Skin::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Skin *pRow = new Row_Skin(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Skin = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Skin));
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
pRow->m_Define = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Define = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_UI = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_UI));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_MergeStandardVariation = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_MergeStandardVariation));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_DataSubdirectory = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_DataSubdirectory = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_FK_Style));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_Skin_TextPlacement = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_FK_Skin_TextPlacement));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_DrawTextBeforeChildren = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_DrawTextBeforeChildren));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_Screen_MainMenu = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_FK_Screen_MainMenu));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Screen_Sleeping = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_FK_Screen_Sleeping));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Screen_ScreenSaver = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_FK_Screen_ScreenSaver));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_Package));
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


class Row_UI* Row_Skin::FK_UI_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_UI *pTable = table->database->UI_get();
return pTable->GetRow(m_FK_UI);
}
class Row_Style* Row_Skin::FK_Style_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style);
}
class Row_Skin* Row_Skin::FK_Skin_TextPlacement_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin_TextPlacement);
}
class Row_StabilityStatus* Row_Skin::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_Screen* Row_Skin::FK_Screen_MainMenu_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen_MainMenu);
}
class Row_Screen* Row_Skin::FK_Screen_Sleeping_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen_Sleeping);
}
class Row_Screen* Row_Skin::FK_Screen_ScreenSaver_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen *pTable = table->database->Screen_get();
return pTable->GetRow(m_FK_Screen_ScreenSaver);
}
class Row_Package* Row_Skin::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}


void Row_Skin::DesignObj_FK_Skin_SubstForSkin_getrows(vector <class Row_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
pTable->GetRows("`FK_Skin_SubstForSkin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::DesignObjVariation_DesignObjParameter_FK_Skin_getrows(vector <class Row_DesignObjVariation_DesignObjParameter*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObjParameter *pTable = table->database->DesignObjVariation_DesignObjParameter_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::DesignObjVariation_DesignObj_Skin_Language_FK_Skin_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::DesignObjVariation_Text_Skin_Language_FK_Skin_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_Text_Skin_Language *pTable = table->database->DesignObjVariation_Text_Skin_Language_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::DeviceTemplate_MediaType_DesignObj_FK_Skin_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_MediaType_DesignObj *pTable = table->database->DeviceTemplate_MediaType_DesignObj_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::EffectType_Effect_Skin_FK_Skin_getrows(vector <class Row_EffectType_Effect_Skin*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_EffectType_Effect_Skin *pTable = table->database->EffectType_Effect_Skin_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::MediaType_DesignObj_FK_Skin_getrows(vector <class Row_MediaType_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_MediaType_DesignObj *pTable = table->database->MediaType_DesignObj_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::Screen_DesignObj_FK_Skin_getrows(vector <class Row_Screen_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen_DesignObj *pTable = table->database->Screen_DesignObj_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::Skin_FK_Skin_TextPlacement_getrows(vector <class Row_Skin*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
pTable->GetRows("`FK_Skin_TextPlacement`=" + StringUtils::itos(m_PK_Skin),rows);
}
void Row_Skin::StyleVariation_FK_Skin_getrows(vector <class Row_StyleVariation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StyleVariation *pTable = table->database->StyleVariation_get();
pTable->GetRows("`FK_Skin`=" + StringUtils::itos(m_PK_Skin),rows);
}



