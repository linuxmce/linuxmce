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
#include "Table_DesignObj.h"
#include "Table_DesignObjType.h"
#include "Table_DesignObjCategory.h"
#include "Table_DesignObj.h"
#include "Table_StabilityStatus.h"
#include "Table_DesignObj.h"
#include "Table_Skin.h"

#include "Table_CachedScreens.h"
#include "Table_CommandGroup.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_DesignObjVariation.h"
#include "Table_DesignObjVariation.h"
#include "Table_DesignObjVariation_DesignObj.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_Zone.h"
#include "Table_Device.h"
#include "Table_DeviceTemplate_DesignObj.h"
#include "Table_DeviceTemplate_MediaType_DesignObj.h"
#include "Table_FloorplanObjectType.h"
#include "Table_MediaType.h"
#include "Table_MediaType_DesignObj.h"
#include "Table_QuickStartTemplate.h"
#include "Table_QuickStartTemplate.h"
#include "Table_Screen_DesignObj.h"


void Database_pluto_main::CreateTable_DesignObj()
{
	tblDesignObj = new Table_DesignObj(this);
}

void Database_pluto_main::DeleteTable_DesignObj()
{
	if( tblDesignObj )
		delete tblDesignObj;
}

bool Database_pluto_main::Commit_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDesignObj->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DesignObj::~Table_DesignObj()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DesignObj *pRow = (Row_DesignObj *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DesignObj *pRow = (Row_DesignObj *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DesignObj::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DesignObj *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DesignObj *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_DesignObj);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_DesignObj::Reload()
{
	Row_DesignObj *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_DesignObj);
		Row_DesignObj *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObj::Row_DesignObj(Table_DesignObj *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObj::SetDefaultValues()
{
	m_PK_DesignObj = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
m_FK_DesignObjType = 0;
is_null[3] = false;
m_FK_DesignObjCategory = 0;
is_null[4] = false;
is_null[5] = true;
m_FK_DesignObj_IncludeIfOtherIncluded = 0;
m_Priority = 0;
is_null[6] = false;
m_KeepGraphicInCache = 0;
is_null[7] = false;
m_AlwaysInclude = 0;
is_null[8] = false;
m_CantGoBack = 0;
is_null[9] = false;
m_CommandsProcessedAtServer = 0;
is_null[10] = false;
is_null[11] = true;
m_TimeoutSeconds = 0;
m_Animate = 0;
is_null[12] = false;
m_FK_StabilityStatus = 1;
is_null[13] = false;
m_CanGoBackToSameScreen = 0;
is_null[14] = false;
is_null[15] = true;
is_null[16] = true;
m_FK_DesignObj_SubstForSkin = 0;
is_null[17] = true;
m_FK_Skin_SubstForSkin = 0;
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

long int Row_DesignObj::PK_DesignObj_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DesignObj;}
string Row_DesignObj::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_DesignObj::Define_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Define;}
long int Row_DesignObj::FK_DesignObjType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjType;}
long int Row_DesignObj::FK_DesignObjCategory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjCategory;}
long int Row_DesignObj::FK_DesignObj_IncludeIfOtherIncluded_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_IncludeIfOtherIncluded;}
short int Row_DesignObj::Priority_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Priority;}
short int Row_DesignObj::KeepGraphicInCache_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_KeepGraphicInCache;}
short int Row_DesignObj::AlwaysInclude_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AlwaysInclude;}
short int Row_DesignObj::CantGoBack_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CantGoBack;}
short int Row_DesignObj::CommandsProcessedAtServer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CommandsProcessedAtServer;}
short int Row_DesignObj::TimeoutSeconds_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TimeoutSeconds;}
short int Row_DesignObj::Animate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Animate;}
long int Row_DesignObj::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_StabilityStatus;}
short int Row_DesignObj::CanGoBackToSameScreen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CanGoBackToSameScreen;}
string Row_DesignObj::ScreenType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ScreenType;}
long int Row_DesignObj::FK_DesignObj_SubstForSkin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_SubstForSkin;}
long int Row_DesignObj::FK_Skin_SubstForSkin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin_SubstForSkin;}
long int Row_DesignObj::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DesignObj::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DesignObj::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DesignObj::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DesignObj::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DesignObj::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DesignObj::PK_DesignObj_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DesignObj = val; is_modified=true; is_null[0]=false;}
void Row_DesignObj::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_DesignObj::Define_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_DesignObj::FK_DesignObjType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjType = val; is_modified=true; is_null[3]=false;}
void Row_DesignObj::FK_DesignObjCategory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjCategory = val; is_modified=true; is_null[4]=false;}
void Row_DesignObj::FK_DesignObj_IncludeIfOtherIncluded_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_IncludeIfOtherIncluded = val; is_modified=true; is_null[5]=false;}
void Row_DesignObj::Priority_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Priority = val; is_modified=true; is_null[6]=false;}
void Row_DesignObj::KeepGraphicInCache_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_KeepGraphicInCache = val; is_modified=true; is_null[7]=false;}
void Row_DesignObj::AlwaysInclude_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AlwaysInclude = val; is_modified=true; is_null[8]=false;}
void Row_DesignObj::CantGoBack_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CantGoBack = val; is_modified=true; is_null[9]=false;}
void Row_DesignObj::CommandsProcessedAtServer_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CommandsProcessedAtServer = val; is_modified=true; is_null[10]=false;}
void Row_DesignObj::TimeoutSeconds_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TimeoutSeconds = val; is_modified=true; is_null[11]=false;}
void Row_DesignObj::Animate_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Animate = val; is_modified=true; is_null[12]=false;}
void Row_DesignObj::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[13]=false;}
void Row_DesignObj::CanGoBackToSameScreen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CanGoBackToSameScreen = val; is_modified=true; is_null[14]=false;}
void Row_DesignObj::ScreenType_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ScreenType = val; is_modified=true; is_null[15]=false;}
void Row_DesignObj::FK_DesignObj_SubstForSkin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_SubstForSkin = val; is_modified=true; is_null[16]=false;}
void Row_DesignObj::FK_Skin_SubstForSkin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin_SubstForSkin = val; is_modified=true; is_null[17]=false;}
void Row_DesignObj::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[18]=false;}
void Row_DesignObj::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[19]=false;}
void Row_DesignObj::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[20]=false;}
void Row_DesignObj::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[21]=false;}
void Row_DesignObj::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[22]=false;}
void Row_DesignObj::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[23]=false;}

		
bool Row_DesignObj::Define_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DesignObj::FK_DesignObj_IncludeIfOtherIncluded_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_DesignObj::TimeoutSeconds_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DesignObj::ScreenType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_DesignObj::FK_DesignObj_SubstForSkin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_DesignObj::FK_Skin_SubstForSkin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DesignObj::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DesignObj::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_DesignObj::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_DesignObj::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_DesignObj::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}

			
void Row_DesignObj::Define_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DesignObj::FK_DesignObj_IncludeIfOtherIncluded_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DesignObj::TimeoutSeconds_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DesignObj::ScreenType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_DesignObj::FK_DesignObj_SubstForSkin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_DesignObj::FK_Skin_SubstForSkin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DesignObj::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DesignObj::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_DesignObj::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_DesignObj::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_DesignObj::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
	

string Row_DesignObj::PK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DesignObj);

return buf;
}

string Row_DesignObj::Description_asSQL()
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

string Row_DesignObj::Define_asSQL()
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

string Row_DesignObj::FK_DesignObjType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjType);

return buf;
}

string Row_DesignObj::FK_DesignObjCategory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjCategory);

return buf;
}

string Row_DesignObj::FK_DesignObj_IncludeIfOtherIncluded_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_IncludeIfOtherIncluded);

return buf;
}

string Row_DesignObj::Priority_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Priority);

return buf;
}

string Row_DesignObj::KeepGraphicInCache_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_KeepGraphicInCache);

return buf;
}

string Row_DesignObj::AlwaysInclude_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AlwaysInclude);

return buf;
}

string Row_DesignObj::CantGoBack_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CantGoBack);

return buf;
}

string Row_DesignObj::CommandsProcessedAtServer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CommandsProcessedAtServer);

return buf;
}

string Row_DesignObj::TimeoutSeconds_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_TimeoutSeconds);

return buf;
}

string Row_DesignObj::Animate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Animate);

return buf;
}

string Row_DesignObj::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_DesignObj::CanGoBackToSameScreen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CanGoBackToSameScreen);

return buf;
}

string Row_DesignObj::ScreenType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ScreenType.c_str(), (unsigned long) min((size_t)1,m_ScreenType.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObj::FK_DesignObj_SubstForSkin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_SubstForSkin);

return buf;
}

string Row_DesignObj::FK_Skin_SubstForSkin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin_SubstForSkin);

return buf;
}

string Row_DesignObj::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObj::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObj::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DesignObj::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObj::psc_mod_asSQL()
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

string Row_DesignObj::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DesignObj::Key::Key(long int in_PK_DesignObj)
{
			pk_PK_DesignObj = in_PK_DesignObj;
	
}

Table_DesignObj::Key::Key(Row_DesignObj *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_DesignObj = pRow->m_PK_DesignObj;
	
}		

bool Table_DesignObj::Key_Less::operator()(const Table_DesignObj::Key &key1, const Table_DesignObj::Key &key2) const
{
			if (key1.pk_PK_DesignObj!=key2.pk_PK_DesignObj)
return key1.pk_PK_DesignObj<key2.pk_PK_DesignObj;
else
return false;	
}	

bool Table_DesignObj::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DesignObj *pRow = (Row_DesignObj *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DesignObj_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->FK_DesignObjType_asSQL()+", "+pRow->FK_DesignObjCategory_asSQL()+", "+pRow->FK_DesignObj_IncludeIfOtherIncluded_asSQL()+", "+pRow->Priority_asSQL()+", "+pRow->KeepGraphicInCache_asSQL()+", "+pRow->AlwaysInclude_asSQL()+", "+pRow->CantGoBack_asSQL()+", "+pRow->CommandsProcessedAtServer_asSQL()+", "+pRow->TimeoutSeconds_asSQL()+", "+pRow->Animate_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->CanGoBackToSameScreen_asSQL()+", "+pRow->ScreenType_asSQL()+", "+pRow->FK_DesignObj_SubstForSkin_asSQL()+", "+pRow->FK_Skin_SubstForSkin_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DesignObj (`PK_DesignObj`, `Description`, `Define`, `FK_DesignObjType`, `FK_DesignObjCategory`, `FK_DesignObj_IncludeIfOtherIncluded`, `Priority`, `KeepGraphicInCache`, `AlwaysInclude`, `CantGoBack`, `CommandsProcessedAtServer`, `TimeoutSeconds`, `Animate`, `FK_StabilityStatus`, `CanGoBackToSameScreen`, `ScreenType`, `FK_DesignObj_SubstForSkin`, `FK_Skin_SubstForSkin`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_DesignObj=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_DesignObj is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_DesignObj);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DesignObj* pRow = (Row_DesignObj*) (*i).second;	
		SingleLongKey key(pRow->m_PK_DesignObj);

		char tmp_PK_DesignObj[32];
sprintf(tmp_PK_DesignObj, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObj`=" + tmp_PK_DesignObj;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DesignObj`="+pRow->PK_DesignObj_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `FK_DesignObjType`="+pRow->FK_DesignObjType_asSQL()+", `FK_DesignObjCategory`="+pRow->FK_DesignObjCategory_asSQL()+", `FK_DesignObj_IncludeIfOtherIncluded`="+pRow->FK_DesignObj_IncludeIfOtherIncluded_asSQL()+", `Priority`="+pRow->Priority_asSQL()+", `KeepGraphicInCache`="+pRow->KeepGraphicInCache_asSQL()+", `AlwaysInclude`="+pRow->AlwaysInclude_asSQL()+", `CantGoBack`="+pRow->CantGoBack_asSQL()+", `CommandsProcessedAtServer`="+pRow->CommandsProcessedAtServer_asSQL()+", `TimeoutSeconds`="+pRow->TimeoutSeconds_asSQL()+", `Animate`="+pRow->Animate_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `CanGoBackToSameScreen`="+pRow->CanGoBackToSameScreen_asSQL()+", `ScreenType`="+pRow->ScreenType_asSQL()+", `FK_DesignObj_SubstForSkin`="+pRow->FK_DesignObj_SubstForSkin_asSQL()+", `FK_Skin_SubstForSkin`="+pRow->FK_Skin_SubstForSkin_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DesignObj set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObj* pRow = (Row_DesignObj*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_DesignObj* pRow = (Row_DesignObj*) (*i).second;	

		char tmp_PK_DesignObj[32];
sprintf(tmp_PK_DesignObj, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObj`=" + tmp_PK_DesignObj;

	
		string query = "delete from DesignObj where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DesignObj*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DesignObj::GetRows(string where_statement,vector<class Row_DesignObj*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DesignObj`.* from DesignObj " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DesignObj`.* from DesignObj where " + where_statement;
	else
		query = "select `DesignObj`.* from DesignObj";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DesignObj *pRow = new Row_DesignObj(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObj = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DesignObj));
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
pRow->m_FK_DesignObjType = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObjType));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_DesignObjCategory = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DesignObjCategory));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_DesignObj_IncludeIfOtherIncluded = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_DesignObj_IncludeIfOtherIncluded));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Priority = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Priority));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_KeepGraphicInCache = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_KeepGraphicInCache));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_AlwaysInclude = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_AlwaysInclude));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_CantGoBack = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_CantGoBack));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_CommandsProcessedAtServer = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_CommandsProcessedAtServer));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_TimeoutSeconds = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_TimeoutSeconds));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Animate = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_Animate));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_CanGoBackToSameScreen = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_CanGoBackToSameScreen));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_ScreenType = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_ScreenType = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_DesignObj_SubstForSkin = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_DesignObj_SubstForSkin));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_Skin_SubstForSkin = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_Skin_SubstForSkin));
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

		SingleLongKey key(pRow->m_PK_DesignObj);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DesignObj *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DesignObj* Table_DesignObj::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DesignObj *pRow = new Row_DesignObj(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObj* Table_DesignObj::GetRow(long int in_PK_DesignObj)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_DesignObj);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DesignObj*) (*i).second;
	//we have to fetch row
	Row_DesignObj* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObj* Table_DesignObj::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_DesignObj[32];
sprintf(tmp_PK_DesignObj, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObj`=" + tmp_PK_DesignObj;


	string query = "select * from DesignObj where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DesignObj *pRow = new Row_DesignObj(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObj = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DesignObj));
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
pRow->m_FK_DesignObjType = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DesignObjType));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_DesignObjCategory = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_DesignObjCategory));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_DesignObj_IncludeIfOtherIncluded = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_FK_DesignObj_IncludeIfOtherIncluded));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Priority = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Priority));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_KeepGraphicInCache = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_KeepGraphicInCache));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_AlwaysInclude = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_AlwaysInclude));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_CantGoBack = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_CantGoBack));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_CommandsProcessedAtServer = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_CommandsProcessedAtServer));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_TimeoutSeconds = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_TimeoutSeconds));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Animate = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_Animate));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_FK_StabilityStatus));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_CanGoBackToSameScreen = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_CanGoBackToSameScreen));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_ScreenType = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_ScreenType = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_DesignObj_SubstForSkin = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_DesignObj_SubstForSkin));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_Skin_SubstForSkin = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_Skin_SubstForSkin));
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


class Row_DesignObjType* Row_DesignObj::FK_DesignObjType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjType *pTable = table->database->DesignObjType_get();
return pTable->GetRow(m_FK_DesignObjType);
}
class Row_DesignObjCategory* Row_DesignObj::FK_DesignObjCategory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjCategory *pTable = table->database->DesignObjCategory_get();
return pTable->GetRow(m_FK_DesignObjCategory);
}
class Row_DesignObj* Row_DesignObj::FK_DesignObj_IncludeIfOtherIncluded_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_IncludeIfOtherIncluded);
}
class Row_StabilityStatus* Row_DesignObj::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_DesignObj* Row_DesignObj::FK_DesignObj_SubstForSkin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_SubstForSkin);
}
class Row_Skin* Row_DesignObj::FK_Skin_SubstForSkin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin_SubstForSkin);
}


void Row_DesignObj::CachedScreens_FK_DesignObj_getrows(vector <class Row_CachedScreens*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CachedScreens *pTable = table->database->CachedScreens_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::CommandGroup_FK_DesignObj_getrows(vector <class Row_CommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup *pTable = table->database->CommandGroup_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObj_FK_DesignObj_IncludeIfOtherIncluded_getrows(vector <class Row_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
pTable->GetRows("`FK_DesignObj_IncludeIfOtherIncluded`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObj_FK_DesignObj_SubstForSkin_getrows(vector <class Row_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
pTable->GetRows("`FK_DesignObj_SubstForSkin`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_FK_DesignObj_getrows(vector <class Row_DesignObjVariation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation *pTable = table->database->DesignObjVariation_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_FK_DesignObj_Goto_getrows(vector <class Row_DesignObjVariation*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation *pTable = table->database->DesignObjVariation_get();
pTable->GetRows("`FK_DesignObj_Goto`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_DesignObj_FK_DesignObj_Child_getrows(vector <class Row_DesignObjVariation_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj *pTable = table->database->DesignObjVariation_DesignObj_get();
pTable->GetRows("`FK_DesignObj_Child`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_InsteadOf_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_DesignObj_InsteadOf`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Up_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_DesignObj_Up`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Down_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_DesignObj_Down`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Left_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_DesignObj_Left`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_DesignObj_Skin_Language_FK_DesignObj_Right_getrows(vector <class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj_Skin_Language *pTable = table->database->DesignObjVariation_DesignObj_Skin_Language_get();
pTable->GetRows("`FK_DesignObj_Right`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DesignObjVariation_Zone_FK_DesignObj_Goto_getrows(vector <class Row_DesignObjVariation_Zone*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_Zone *pTable = table->database->DesignObjVariation_Zone_get();
pTable->GetRows("`FK_DesignObj_Goto`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::Device_FK_DesignObj_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DeviceTemplate_DesignObj_FK_DesignObj_getrows(vector <class Row_DeviceTemplate_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DesignObj *pTable = table->database->DeviceTemplate_DesignObj_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::DeviceTemplate_MediaType_DesignObj_FK_DesignObj_Popup_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_MediaType_DesignObj *pTable = table->database->DeviceTemplate_MediaType_DesignObj_get();
pTable->GetRows("`FK_DesignObj_Popup`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::FloorplanObjectType_FK_DesignObj_Control_getrows(vector <class Row_FloorplanObjectType*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_FloorplanObjectType *pTable = table->database->FloorplanObjectType_get();
pTable->GetRows("`FK_DesignObj_Control`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::MediaType_FK_DesignObj_getrows(vector <class Row_MediaType*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_MediaType *pTable = table->database->MediaType_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::MediaType_DesignObj_FK_DesignObj_Popup_getrows(vector <class Row_MediaType_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_MediaType_DesignObj *pTable = table->database->MediaType_DesignObj_get();
pTable->GetRows("`FK_DesignObj_Popup`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::QuickStartTemplate_FK_DesignObj_getrows(vector <class Row_QuickStartTemplate*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_QuickStartTemplate *pTable = table->database->QuickStartTemplate_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::QuickStartTemplate_FK_DesignObj_OSD_getrows(vector <class Row_QuickStartTemplate*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_QuickStartTemplate *pTable = table->database->QuickStartTemplate_get();
pTable->GetRows("`FK_DesignObj_OSD`=" + StringUtils::itos(m_PK_DesignObj),rows);
}
void Row_DesignObj::Screen_DesignObj_FK_DesignObj_getrows(vector <class Row_Screen_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen_DesignObj *pTable = table->database->Screen_DesignObj_get();
pTable->GetRows("`FK_DesignObj`=" + StringUtils::itos(m_PK_DesignObj),rows);
}



