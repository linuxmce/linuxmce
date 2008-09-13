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
#include "Table_DesignObj_pschist.h"
#include "Table_DesignObjType.h"
#include "Table_DesignObjCategory.h"
#include "Table_DesignObj.h"
#include "Table_StabilityStatus.h"
#include "Table_Document.h"
#include "Table_DesignObj.h"
#include "Table_Skin.h"



void Database_pluto_main::CreateTable_DesignObj_pschist()
{
	tblDesignObj_pschist = new Table_DesignObj_pschist(this);
}

void Database_pluto_main::DeleteTable_DesignObj_pschist()
{
	if( tblDesignObj_pschist )
		delete tblDesignObj_pschist;
}

bool Database_pluto_main::Commit_DesignObj_pschist(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDesignObj_pschist->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DesignObj_pschist::~Table_DesignObj_pschist()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DesignObj_pschist *pRow = (Row_DesignObj_pschist *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DesignObj_pschist *pRow = (Row_DesignObj_pschist *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DesignObj_pschist::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DesignObj_pschist *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DesignObj_pschist *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
			map<DoubleLongKey, TableRow*, DoubleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
}

void Row_DesignObj_pschist::Reload()
{
	Row_DesignObj_pschist *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_DesignObj_pschist *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObj_pschist::Row_DesignObj_pschist(Table_DesignObj_pschist *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObj_pschist::SetDefaultValues()
{
	is_null[0] = true;
m_PK_DesignObj = 0;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
m_FK_DesignObjType = 0;
is_null[4] = true;
m_FK_DesignObjCategory = 0;
is_null[5] = true;
m_FK_DesignObj_IncludeIfOtherIncluded = 0;
is_null[6] = true;
m_Priority = 0;
m_KeepGraphicInCache = 0;
is_null[7] = false;
m_AlwaysInclude = 0;
is_null[8] = false;
is_null[9] = true;
m_CantGoBack = 0;
is_null[10] = true;
m_CommandsProcessedAtServer = 0;
is_null[11] = true;
m_TimeoutSeconds = 0;
is_null[12] = true;
m_Animate = 0;
is_null[13] = true;
m_FK_StabilityStatus = 0;
is_null[14] = true;
m_FK_Document = 0;
m_CanGoBackToSameScreen = 0;
is_null[15] = false;
is_null[16] = true;
is_null[17] = true;
m_FK_DesignObj_SubstForSkin = 0;
is_null[18] = true;
m_FK_Skin_SubstForSkin = 0;
m_psc_id = 0;
is_null[19] = false;
m_psc_batch = 0;
is_null[20] = false;
is_null[21] = true;
m_psc_user = 0;
is_null[22] = true;
m_psc_frozen = 0;
is_null[23] = true;
m_psc_toc = 0;
is_null[24] = false;
is_null[25] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DesignObj_pschist::PK_DesignObj_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DesignObj;}
string Row_DesignObj_pschist::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_DesignObj_pschist::Define_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Define;}
long int Row_DesignObj_pschist::FK_DesignObjType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjType;}
long int Row_DesignObj_pschist::FK_DesignObjCategory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjCategory;}
long int Row_DesignObj_pschist::FK_DesignObj_IncludeIfOtherIncluded_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_IncludeIfOtherIncluded;}
short int Row_DesignObj_pschist::Priority_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Priority;}
short int Row_DesignObj_pschist::KeepGraphicInCache_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_KeepGraphicInCache;}
short int Row_DesignObj_pschist::AlwaysInclude_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AlwaysInclude;}
short int Row_DesignObj_pschist::CantGoBack_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CantGoBack;}
short int Row_DesignObj_pschist::CommandsProcessedAtServer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CommandsProcessedAtServer;}
short int Row_DesignObj_pschist::TimeoutSeconds_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_TimeoutSeconds;}
short int Row_DesignObj_pschist::Animate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Animate;}
long int Row_DesignObj_pschist::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_StabilityStatus;}
long int Row_DesignObj_pschist::FK_Document_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Document;}
short int Row_DesignObj_pschist::CanGoBackToSameScreen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CanGoBackToSameScreen;}
string Row_DesignObj_pschist::ScreenType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ScreenType;}
long int Row_DesignObj_pschist::FK_DesignObj_SubstForSkin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_SubstForSkin;}
long int Row_DesignObj_pschist::FK_Skin_SubstForSkin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin_SubstForSkin;}
long int Row_DesignObj_pschist::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DesignObj_pschist::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DesignObj_pschist::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DesignObj_pschist::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DesignObj_pschist::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
short int Row_DesignObj_pschist::psc_toc_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_toc;}
long int Row_DesignObj_pschist::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DesignObj_pschist::PK_DesignObj_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DesignObj = val; is_modified=true; is_null[0]=false;}
void Row_DesignObj_pschist::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_DesignObj_pschist::Define_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Define = val; is_modified=true; is_null[2]=false;}
void Row_DesignObj_pschist::FK_DesignObjType_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjType = val; is_modified=true; is_null[3]=false;}
void Row_DesignObj_pschist::FK_DesignObjCategory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjCategory = val; is_modified=true; is_null[4]=false;}
void Row_DesignObj_pschist::FK_DesignObj_IncludeIfOtherIncluded_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_IncludeIfOtherIncluded = val; is_modified=true; is_null[5]=false;}
void Row_DesignObj_pschist::Priority_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Priority = val; is_modified=true; is_null[6]=false;}
void Row_DesignObj_pschist::KeepGraphicInCache_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_KeepGraphicInCache = val; is_modified=true; is_null[7]=false;}
void Row_DesignObj_pschist::AlwaysInclude_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AlwaysInclude = val; is_modified=true; is_null[8]=false;}
void Row_DesignObj_pschist::CantGoBack_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CantGoBack = val; is_modified=true; is_null[9]=false;}
void Row_DesignObj_pschist::CommandsProcessedAtServer_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CommandsProcessedAtServer = val; is_modified=true; is_null[10]=false;}
void Row_DesignObj_pschist::TimeoutSeconds_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_TimeoutSeconds = val; is_modified=true; is_null[11]=false;}
void Row_DesignObj_pschist::Animate_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Animate = val; is_modified=true; is_null[12]=false;}
void Row_DesignObj_pschist::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[13]=false;}
void Row_DesignObj_pschist::FK_Document_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Document = val; is_modified=true; is_null[14]=false;}
void Row_DesignObj_pschist::CanGoBackToSameScreen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CanGoBackToSameScreen = val; is_modified=true; is_null[15]=false;}
void Row_DesignObj_pschist::ScreenType_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ScreenType = val; is_modified=true; is_null[16]=false;}
void Row_DesignObj_pschist::FK_DesignObj_SubstForSkin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_SubstForSkin = val; is_modified=true; is_null[17]=false;}
void Row_DesignObj_pschist::FK_Skin_SubstForSkin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin_SubstForSkin = val; is_modified=true; is_null[18]=false;}
void Row_DesignObj_pschist::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[19]=false;}
void Row_DesignObj_pschist::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[20]=false;}
void Row_DesignObj_pschist::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[21]=false;}
void Row_DesignObj_pschist::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[22]=false;}
void Row_DesignObj_pschist::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[23]=false;}
void Row_DesignObj_pschist::psc_toc_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_toc = val; is_modified=true; is_null[24]=false;}
void Row_DesignObj_pschist::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[25]=false;}

		
bool Row_DesignObj_pschist::PK_DesignObj_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_DesignObj_pschist::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DesignObj_pschist::Define_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DesignObj_pschist::FK_DesignObjType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DesignObj_pschist::FK_DesignObjCategory_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DesignObj_pschist::FK_DesignObj_IncludeIfOtherIncluded_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_DesignObj_pschist::Priority_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_DesignObj_pschist::CantGoBack_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DesignObj_pschist::CommandsProcessedAtServer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DesignObj_pschist::TimeoutSeconds_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DesignObj_pschist::Animate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DesignObj_pschist::FK_StabilityStatus_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DesignObj_pschist::FK_Document_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_DesignObj_pschist::ScreenType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_DesignObj_pschist::FK_DesignObj_SubstForSkin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DesignObj_pschist::FK_Skin_SubstForSkin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DesignObj_pschist::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_DesignObj_pschist::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_DesignObj_pschist::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}
bool Row_DesignObj_pschist::psc_toc_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_DesignObj_pschist::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}

			
void Row_DesignObj_pschist::PK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_DesignObj_pschist::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DesignObj_pschist::Define_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_DesignObjType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_DesignObjCategory_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_DesignObj_IncludeIfOtherIncluded_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DesignObj_pschist::Priority_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_DesignObj_pschist::CantGoBack_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DesignObj_pschist::CommandsProcessedAtServer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DesignObj_pschist::TimeoutSeconds_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DesignObj_pschist::Animate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_StabilityStatus_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_Document_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_DesignObj_pschist::ScreenType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_DesignObj_SubstForSkin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DesignObj_pschist::FK_Skin_SubstForSkin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DesignObj_pschist::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_DesignObj_pschist::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_DesignObj_pschist::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
void Row_DesignObj_pschist::psc_toc_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_DesignObj_pschist::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
	

string Row_DesignObj_pschist::PK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DesignObj);

return buf;
}

string Row_DesignObj_pschist::Description_asSQL()
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

string Row_DesignObj_pschist::Define_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[41];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Define.c_str(), (unsigned long) min((size_t)20,m_Define.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObj_pschist::FK_DesignObjType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjType);

return buf;
}

string Row_DesignObj_pschist::FK_DesignObjCategory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjCategory);

return buf;
}

string Row_DesignObj_pschist::FK_DesignObj_IncludeIfOtherIncluded_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_IncludeIfOtherIncluded);

return buf;
}

string Row_DesignObj_pschist::Priority_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Priority);

return buf;
}

string Row_DesignObj_pschist::KeepGraphicInCache_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_KeepGraphicInCache);

return buf;
}

string Row_DesignObj_pschist::AlwaysInclude_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AlwaysInclude);

return buf;
}

string Row_DesignObj_pschist::CantGoBack_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CantGoBack);

return buf;
}

string Row_DesignObj_pschist::CommandsProcessedAtServer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CommandsProcessedAtServer);

return buf;
}

string Row_DesignObj_pschist::TimeoutSeconds_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_TimeoutSeconds);

return buf;
}

string Row_DesignObj_pschist::Animate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Animate);

return buf;
}

string Row_DesignObj_pschist::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_DesignObj_pschist::FK_Document_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Document);

return buf;
}

string Row_DesignObj_pschist::CanGoBackToSameScreen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CanGoBackToSameScreen);

return buf;
}

string Row_DesignObj_pschist::ScreenType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char *buf = new char[3];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ScreenType.c_str(), (unsigned long) min((size_t)1,m_ScreenType.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObj_pschist::FK_DesignObj_SubstForSkin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_SubstForSkin);

return buf;
}

string Row_DesignObj_pschist::FK_Skin_SubstForSkin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin_SubstForSkin);

return buf;
}

string Row_DesignObj_pschist::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObj_pschist::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObj_pschist::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DesignObj_pschist::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObj_pschist::psc_mod_asSQL()
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

string Row_DesignObj_pschist::psc_toc_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_toc);

return buf;
}

string Row_DesignObj_pschist::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DesignObj_pschist::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_DesignObj_pschist::Key::Key(Row_DesignObj_pschist *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_DesignObj_pschist::Key_Less::operator()(const Table_DesignObj_pschist::Key &key1, const Table_DesignObj_pschist::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_DesignObj_pschist::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DesignObj_pschist *pRow = (Row_DesignObj_pschist *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DesignObj_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->FK_DesignObjType_asSQL()+", "+pRow->FK_DesignObjCategory_asSQL()+", "+pRow->FK_DesignObj_IncludeIfOtherIncluded_asSQL()+", "+pRow->Priority_asSQL()+", "+pRow->KeepGraphicInCache_asSQL()+", "+pRow->AlwaysInclude_asSQL()+", "+pRow->CantGoBack_asSQL()+", "+pRow->CommandsProcessedAtServer_asSQL()+", "+pRow->TimeoutSeconds_asSQL()+", "+pRow->Animate_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->FK_Document_asSQL()+", "+pRow->CanGoBackToSameScreen_asSQL()+", "+pRow->ScreenType_asSQL()+", "+pRow->FK_DesignObj_SubstForSkin_asSQL()+", "+pRow->FK_Skin_SubstForSkin_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_toc_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DesignObj_pschist (`PK_DesignObj`, `Description`, `Define`, `FK_DesignObjType`, `FK_DesignObjCategory`, `FK_DesignObj_IncludeIfOtherIncluded`, `Priority`, `KeepGraphicInCache`, `AlwaysInclude`, `CantGoBack`, `CommandsProcessedAtServer`, `TimeoutSeconds`, `Animate`, `FK_StabilityStatus`, `FK_Document`, `CanGoBackToSameScreen`, `ScreenType`, `FK_DesignObj_SubstForSkin`, `FK_Skin_SubstForSkin`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_toc`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
			DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DesignObj_pschist* pRow = (Row_DesignObj_pschist*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DesignObj`="+pRow->PK_DesignObj_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Define`="+pRow->Define_asSQL()+", `FK_DesignObjType`="+pRow->FK_DesignObjType_asSQL()+", `FK_DesignObjCategory`="+pRow->FK_DesignObjCategory_asSQL()+", `FK_DesignObj_IncludeIfOtherIncluded`="+pRow->FK_DesignObj_IncludeIfOtherIncluded_asSQL()+", `Priority`="+pRow->Priority_asSQL()+", `KeepGraphicInCache`="+pRow->KeepGraphicInCache_asSQL()+", `AlwaysInclude`="+pRow->AlwaysInclude_asSQL()+", `CantGoBack`="+pRow->CantGoBack_asSQL()+", `CommandsProcessedAtServer`="+pRow->CommandsProcessedAtServer_asSQL()+", `TimeoutSeconds`="+pRow->TimeoutSeconds_asSQL()+", `Animate`="+pRow->Animate_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `FK_Document`="+pRow->FK_Document_asSQL()+", `CanGoBackToSameScreen`="+pRow->CanGoBackToSameScreen_asSQL()+", `ScreenType`="+pRow->ScreenType_asSQL()+", `FK_DesignObj_SubstForSkin`="+pRow->FK_DesignObj_SubstForSkin_asSQL()+", `FK_Skin_SubstForSkin`="+pRow->FK_Skin_SubstForSkin_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_toc`="+pRow->psc_toc_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DesignObj_pschist set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObj_pschist* pRow = (Row_DesignObj_pschist*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DesignObj_pschist* pRow = (Row_DesignObj_pschist*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from DesignObj_pschist where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DesignObj_pschist*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DesignObj_pschist::GetRows(string where_statement,vector<class Row_DesignObj_pschist*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DesignObj_pschist`.* from DesignObj_pschist " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DesignObj_pschist`.* from DesignObj_pschist where " + where_statement;
	else
		query = "select `DesignObj_pschist`.* from DesignObj_pschist";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DesignObj_pschist *pRow = new Row_DesignObj_pschist(this);
		
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
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_Document));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_CanGoBackToSameScreen = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_CanGoBackToSameScreen));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_ScreenType = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_ScreenType = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_DesignObj_SubstForSkin = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_DesignObj_SubstForSkin));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_Skin_SubstForSkin = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_Skin_SubstForSkin));
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
pRow->m_psc_toc = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%hi", &(pRow->m_psc_toc));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DesignObj_pschist *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DesignObj_pschist* Table_DesignObj_pschist::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DesignObj_pschist *pRow = new Row_DesignObj_pschist(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObj_pschist* Table_DesignObj_pschist::GetRow(long int in_psc_id, long int in_psc_batch)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	DoubleLongKey row_key(in_psc_id, in_psc_batch);

	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DesignObj_pschist*) (*i).second;
	//we have to fetch row
	Row_DesignObj_pschist* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObj_pschist* Table_DesignObj_pschist::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from DesignObj_pschist where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObj_pschist::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DesignObj_pschist *pRow = new Row_DesignObj_pschist(this);
		
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
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_Document));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_CanGoBackToSameScreen = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_CanGoBackToSameScreen));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_ScreenType = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_ScreenType = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_DesignObj_SubstForSkin = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_DesignObj_SubstForSkin));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_Skin_SubstForSkin = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_Skin_SubstForSkin));
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
pRow->m_psc_toc = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%hi", &(pRow->m_psc_toc));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DesignObjType* Row_DesignObj_pschist::FK_DesignObjType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjType *pTable = table->database->DesignObjType_get();
return pTable->GetRow(m_FK_DesignObjType);
}
class Row_DesignObjCategory* Row_DesignObj_pschist::FK_DesignObjCategory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjCategory *pTable = table->database->DesignObjCategory_get();
return pTable->GetRow(m_FK_DesignObjCategory);
}
class Row_DesignObj* Row_DesignObj_pschist::FK_DesignObj_IncludeIfOtherIncluded_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_IncludeIfOtherIncluded);
}
class Row_StabilityStatus* Row_DesignObj_pschist::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_Document* Row_DesignObj_pschist::FK_Document_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Document *pTable = table->database->Document_get();
return pTable->GetRow(m_FK_Document);
}
class Row_DesignObj* Row_DesignObj_pschist::FK_DesignObj_SubstForSkin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_SubstForSkin);
}
class Row_Skin* Row_DesignObj_pschist::FK_Skin_SubstForSkin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin_SubstForSkin);
}





