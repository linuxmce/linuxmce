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
#include "Table_DesignObjVariation_pschmask.h"
#include "Table_DesignObj.h"
#include "Table_UI.h"
#include "Table_DesignObj.h"
#include "Table_CommandGroup_D.h"
#include "Table_CommandGroup_D.h"
#include "Table_CommandGroup_D.h"
#include "Table_CommandGroup_D.h"
#include "Table_CommandGroup_D.h"
#include "Table_CommandGroup_D.h"
#include "Table_CommandGroup_D.h"
#include "Table_EffectType.h"
#include "Table_Button.h"
#include "Table_Criteria.h"
#include "Table_StabilityStatus.h"



void Database_pluto_main::CreateTable_DesignObjVariation_pschmask()
{
	tblDesignObjVariation_pschmask = new Table_DesignObjVariation_pschmask(this);
}

void Database_pluto_main::DeleteTable_DesignObjVariation_pschmask()
{
	if( tblDesignObjVariation_pschmask )
		delete tblDesignObjVariation_pschmask;
}

bool Database_pluto_main::Commit_DesignObjVariation_pschmask(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDesignObjVariation_pschmask->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DesignObjVariation_pschmask::~Table_DesignObjVariation_pschmask()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DesignObjVariation_pschmask *pRow = (Row_DesignObjVariation_pschmask *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DesignObjVariation_pschmask *pRow = (Row_DesignObjVariation_pschmask *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DesignObjVariation_pschmask::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DesignObjVariation_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DesignObjVariation_pschmask *) *i != this); i++);
			
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

void Row_DesignObjVariation_pschmask::Reload()
{
	Row_DesignObjVariation_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_DesignObjVariation_pschmask *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObjVariation_pschmask::Row_DesignObjVariation_pschmask(Table_DesignObjVariation_pschmask *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObjVariation_pschmask::SetDefaultValues()
{
	is_null[0] = true;
m_PK_DesignObjVariation = 0;
is_null[1] = true;
m_FK_DesignObj = 0;
is_null[2] = true;
m_FK_UI = 0;
is_null[3] = true;
m_FK_DesignObj_Goto = 0;
is_null[4] = true;
m_FK_CommandGroup_D_OnActivate = 0;
is_null[5] = true;
m_FK_CommandGroup_D_OnLoad = 0;
is_null[6] = true;
m_FK_CommandGroup_D_OnUnload = 0;
is_null[7] = true;
m_FK_CommandGroup_D_OnTimeout = 0;
is_null[8] = true;
m_FK_CommandGroup_D_OnStartup = 0;
is_null[9] = true;
m_FK_CommandGroup_D_OnHighlight = 0;
is_null[10] = true;
m_FK_CommandGroup_D_OnUnhighlight = 0;
is_null[11] = true;
m_FK_EffectType_Selected_WithChange = 0;
is_null[12] = true;
m_FK_EffectType_Selected_NoChange = 0;
is_null[13] = true;
m_FK_EffectType_Highlighted = 0;
is_null[14] = true;
m_FK_Button = 0;
is_null[15] = true;
m_FK_Criteria_Orbiter = 0;
is_null[16] = true;
m_DontResetSelectedState = 0;
is_null[17] = true;
m_FK_StabilityStatus = 0;
is_null[18] = true;
m_RepeatMS = 0;
is_null[19] = true;
m_RepeatParm = 0;
is_null[20] = true;
m_PreserveTransparencies = 0;
m_psc_id = 0;
is_null[21] = false;
m_psc_batch = 0;
is_null[22] = false;
is_null[23] = true;
m_psc_user = 0;
is_null[24] = true;
m_psc_frozen = 0;
is_null[25] = true;
is_null[26] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

short int Row_DesignObjVariation_pschmask::PK_DesignObjVariation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DesignObjVariation;}
short int Row_DesignObjVariation_pschmask::FK_DesignObj_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj;}
short int Row_DesignObjVariation_pschmask::FK_UI_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_UI;}
short int Row_DesignObjVariation_pschmask::FK_DesignObj_Goto_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Goto;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnActivate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnActivate;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnLoad_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnLoad;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnload_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnUnload;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnTimeout_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnTimeout;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnStartup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnStartup;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnHighlight_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnHighlight;}
short int Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnhighlight_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommandGroup_D_OnUnhighlight;}
short int Row_DesignObjVariation_pschmask::FK_EffectType_Selected_WithChange_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_EffectType_Selected_WithChange;}
short int Row_DesignObjVariation_pschmask::FK_EffectType_Selected_NoChange_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_EffectType_Selected_NoChange;}
short int Row_DesignObjVariation_pschmask::FK_EffectType_Highlighted_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_EffectType_Highlighted;}
short int Row_DesignObjVariation_pschmask::FK_Button_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Button;}
short int Row_DesignObjVariation_pschmask::FK_Criteria_Orbiter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Criteria_Orbiter;}
short int Row_DesignObjVariation_pschmask::DontResetSelectedState_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DontResetSelectedState;}
short int Row_DesignObjVariation_pschmask::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_StabilityStatus;}
short int Row_DesignObjVariation_pschmask::RepeatMS_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RepeatMS;}
short int Row_DesignObjVariation_pschmask::RepeatParm_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RepeatParm;}
short int Row_DesignObjVariation_pschmask::PreserveTransparencies_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PreserveTransparencies;}
long int Row_DesignObjVariation_pschmask::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DesignObjVariation_pschmask::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
short int Row_DesignObjVariation_pschmask::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DesignObjVariation_pschmask::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DesignObjVariation_pschmask::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DesignObjVariation_pschmask::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DesignObjVariation_pschmask::PK_DesignObjVariation_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DesignObjVariation = val; is_modified=true; is_null[0]=false;}
void Row_DesignObjVariation_pschmask::FK_DesignObj_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj = val; is_modified=true; is_null[1]=false;}
void Row_DesignObjVariation_pschmask::FK_UI_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_UI = val; is_modified=true; is_null[2]=false;}
void Row_DesignObjVariation_pschmask::FK_DesignObj_Goto_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Goto = val; is_modified=true; is_null[3]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnActivate_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnActivate = val; is_modified=true; is_null[4]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnLoad_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnLoad = val; is_modified=true; is_null[5]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnload_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnUnload = val; is_modified=true; is_null[6]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnTimeout_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnTimeout = val; is_modified=true; is_null[7]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnStartup_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnStartup = val; is_modified=true; is_null[8]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnHighlight_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnHighlight = val; is_modified=true; is_null[9]=false;}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnhighlight_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommandGroup_D_OnUnhighlight = val; is_modified=true; is_null[10]=false;}
void Row_DesignObjVariation_pschmask::FK_EffectType_Selected_WithChange_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_EffectType_Selected_WithChange = val; is_modified=true; is_null[11]=false;}
void Row_DesignObjVariation_pschmask::FK_EffectType_Selected_NoChange_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_EffectType_Selected_NoChange = val; is_modified=true; is_null[12]=false;}
void Row_DesignObjVariation_pschmask::FK_EffectType_Highlighted_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_EffectType_Highlighted = val; is_modified=true; is_null[13]=false;}
void Row_DesignObjVariation_pschmask::FK_Button_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Button = val; is_modified=true; is_null[14]=false;}
void Row_DesignObjVariation_pschmask::FK_Criteria_Orbiter_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Criteria_Orbiter = val; is_modified=true; is_null[15]=false;}
void Row_DesignObjVariation_pschmask::DontResetSelectedState_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DontResetSelectedState = val; is_modified=true; is_null[16]=false;}
void Row_DesignObjVariation_pschmask::FK_StabilityStatus_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[17]=false;}
void Row_DesignObjVariation_pschmask::RepeatMS_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RepeatMS = val; is_modified=true; is_null[18]=false;}
void Row_DesignObjVariation_pschmask::RepeatParm_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RepeatParm = val; is_modified=true; is_null[19]=false;}
void Row_DesignObjVariation_pschmask::PreserveTransparencies_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PreserveTransparencies = val; is_modified=true; is_null[20]=false;}
void Row_DesignObjVariation_pschmask::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[21]=false;}
void Row_DesignObjVariation_pschmask::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[22]=false;}
void Row_DesignObjVariation_pschmask::psc_user_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[23]=false;}
void Row_DesignObjVariation_pschmask::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[24]=false;}
void Row_DesignObjVariation_pschmask::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[25]=false;}
void Row_DesignObjVariation_pschmask::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[26]=false;}

		
bool Row_DesignObjVariation_pschmask::PK_DesignObjVariation_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_DesignObjVariation_pschmask::FK_DesignObj_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DesignObjVariation_pschmask::FK_UI_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DesignObjVariation_pschmask::FK_DesignObj_Goto_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnActivate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnLoad_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnload_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnTimeout_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnStartup_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnHighlight_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnhighlight_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DesignObjVariation_pschmask::FK_EffectType_Selected_WithChange_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DesignObjVariation_pschmask::FK_EffectType_Selected_NoChange_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DesignObjVariation_pschmask::FK_EffectType_Highlighted_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DesignObjVariation_pschmask::FK_Button_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_DesignObjVariation_pschmask::FK_Criteria_Orbiter_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_DesignObjVariation_pschmask::DontResetSelectedState_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_DesignObjVariation_pschmask::FK_StabilityStatus_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DesignObjVariation_pschmask::RepeatMS_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DesignObjVariation_pschmask::RepeatParm_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_DesignObjVariation_pschmask::PreserveTransparencies_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_DesignObjVariation_pschmask::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}
bool Row_DesignObjVariation_pschmask::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_DesignObjVariation_pschmask::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}
bool Row_DesignObjVariation_pschmask::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}

			
void Row_DesignObjVariation_pschmask::PK_DesignObjVariation_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_UI_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_DesignObj_Goto_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnActivate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnLoad_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnload_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnTimeout_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnStartup_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnHighlight_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnhighlight_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_EffectType_Selected_WithChange_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_EffectType_Selected_NoChange_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_EffectType_Highlighted_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_Button_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_Criteria_Orbiter_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::DontResetSelectedState_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::FK_StabilityStatus_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::RepeatMS_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::RepeatParm_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::PreserveTransparencies_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
void Row_DesignObjVariation_pschmask::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
	

string Row_DesignObjVariation_pschmask::PK_DesignObjVariation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PK_DesignObjVariation);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_DesignObj);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_UI_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_UI);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_DesignObj_Goto_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_DesignObj_Goto);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnActivate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnActivate);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnLoad_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnLoad);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnload_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnUnload);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnTimeout_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnTimeout);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnStartup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnStartup);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnHighlight_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnHighlight);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnhighlight_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_CommandGroup_D_OnUnhighlight);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_EffectType_Selected_WithChange_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_EffectType_Selected_WithChange);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_EffectType_Selected_NoChange_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_EffectType_Selected_NoChange);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_EffectType_Highlighted_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_EffectType_Highlighted);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_Button_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Button);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_Criteria_Orbiter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Criteria_Orbiter);

return buf;
}

string Row_DesignObjVariation_pschmask::DontResetSelectedState_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DontResetSelectedState);

return buf;
}

string Row_DesignObjVariation_pschmask::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_StabilityStatus);

return buf;
}

string Row_DesignObjVariation_pschmask::RepeatMS_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RepeatMS);

return buf;
}

string Row_DesignObjVariation_pschmask::RepeatParm_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RepeatParm);

return buf;
}

string Row_DesignObjVariation_pschmask::PreserveTransparencies_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PreserveTransparencies);

return buf;
}

string Row_DesignObjVariation_pschmask::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObjVariation_pschmask::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObjVariation_pschmask::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_user);

return buf;
}

string Row_DesignObjVariation_pschmask::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObjVariation_pschmask::psc_mod_asSQL()
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

string Row_DesignObjVariation_pschmask::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DesignObjVariation_pschmask::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_DesignObjVariation_pschmask::Key::Key(Row_DesignObjVariation_pschmask *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_DesignObjVariation_pschmask::Key_Less::operator()(const Table_DesignObjVariation_pschmask::Key &key1, const Table_DesignObjVariation_pschmask::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_DesignObjVariation_pschmask::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DesignObjVariation_pschmask *pRow = (Row_DesignObjVariation_pschmask *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DesignObjVariation_asSQL()+", "+pRow->FK_DesignObj_asSQL()+", "+pRow->FK_UI_asSQL()+", "+pRow->FK_DesignObj_Goto_asSQL()+", "+pRow->FK_CommandGroup_D_OnActivate_asSQL()+", "+pRow->FK_CommandGroup_D_OnLoad_asSQL()+", "+pRow->FK_CommandGroup_D_OnUnload_asSQL()+", "+pRow->FK_CommandGroup_D_OnTimeout_asSQL()+", "+pRow->FK_CommandGroup_D_OnStartup_asSQL()+", "+pRow->FK_CommandGroup_D_OnHighlight_asSQL()+", "+pRow->FK_CommandGroup_D_OnUnhighlight_asSQL()+", "+pRow->FK_EffectType_Selected_WithChange_asSQL()+", "+pRow->FK_EffectType_Selected_NoChange_asSQL()+", "+pRow->FK_EffectType_Highlighted_asSQL()+", "+pRow->FK_Button_asSQL()+", "+pRow->FK_Criteria_Orbiter_asSQL()+", "+pRow->DontResetSelectedState_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->RepeatMS_asSQL()+", "+pRow->RepeatParm_asSQL()+", "+pRow->PreserveTransparencies_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DesignObjVariation_pschmask (`PK_DesignObjVariation`, `FK_DesignObj`, `FK_UI`, `FK_DesignObj_Goto`, `FK_CommandGroup_D_OnActivate`, `FK_CommandGroup_D_OnLoad`, `FK_CommandGroup_D_OnUnload`, `FK_CommandGroup_D_OnTimeout`, `FK_CommandGroup_D_OnStartup`, `FK_CommandGroup_D_OnHighlight`, `FK_CommandGroup_D_OnUnhighlight`, `FK_EffectType_Selected_WithChange`, `FK_EffectType_Selected_NoChange`, `FK_EffectType_Highlighted`, `FK_Button`, `FK_Criteria_Orbiter`, `DontResetSelectedState`, `FK_StabilityStatus`, `RepeatMS`, `RepeatParm`, `PreserveTransparencies`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObjVariation_pschmask* pRow = (Row_DesignObjVariation_pschmask*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DesignObjVariation`="+pRow->PK_DesignObjVariation_asSQL()+", `FK_DesignObj`="+pRow->FK_DesignObj_asSQL()+", `FK_UI`="+pRow->FK_UI_asSQL()+", `FK_DesignObj_Goto`="+pRow->FK_DesignObj_Goto_asSQL()+", `FK_CommandGroup_D_OnActivate`="+pRow->FK_CommandGroup_D_OnActivate_asSQL()+", `FK_CommandGroup_D_OnLoad`="+pRow->FK_CommandGroup_D_OnLoad_asSQL()+", `FK_CommandGroup_D_OnUnload`="+pRow->FK_CommandGroup_D_OnUnload_asSQL()+", `FK_CommandGroup_D_OnTimeout`="+pRow->FK_CommandGroup_D_OnTimeout_asSQL()+", `FK_CommandGroup_D_OnStartup`="+pRow->FK_CommandGroup_D_OnStartup_asSQL()+", `FK_CommandGroup_D_OnHighlight`="+pRow->FK_CommandGroup_D_OnHighlight_asSQL()+", `FK_CommandGroup_D_OnUnhighlight`="+pRow->FK_CommandGroup_D_OnUnhighlight_asSQL()+", `FK_EffectType_Selected_WithChange`="+pRow->FK_EffectType_Selected_WithChange_asSQL()+", `FK_EffectType_Selected_NoChange`="+pRow->FK_EffectType_Selected_NoChange_asSQL()+", `FK_EffectType_Highlighted`="+pRow->FK_EffectType_Highlighted_asSQL()+", `FK_Button`="+pRow->FK_Button_asSQL()+", `FK_Criteria_Orbiter`="+pRow->FK_Criteria_Orbiter_asSQL()+", `DontResetSelectedState`="+pRow->DontResetSelectedState_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `RepeatMS`="+pRow->RepeatMS_asSQL()+", `RepeatParm`="+pRow->RepeatParm_asSQL()+", `PreserveTransparencies`="+pRow->PreserveTransparencies_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DesignObjVariation_pschmask set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObjVariation_pschmask* pRow = (Row_DesignObjVariation_pschmask*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DesignObjVariation_pschmask* pRow = (Row_DesignObjVariation_pschmask*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from DesignObjVariation_pschmask where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DesignObjVariation_pschmask*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DesignObjVariation_pschmask::GetRows(string where_statement,vector<class Row_DesignObjVariation_pschmask*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DesignObjVariation_pschmask`.* from DesignObjVariation_pschmask " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DesignObjVariation_pschmask`.* from DesignObjVariation_pschmask where " + where_statement;
	else
		query = "select `DesignObjVariation_pschmask`.* from DesignObjVariation_pschmask";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DesignObjVariation_pschmask *pRow = new Row_DesignObjVariation_pschmask(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_DesignObjVariation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_FK_DesignObj));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_UI = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_FK_UI));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DesignObj_Goto = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_FK_DesignObj_Goto));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_CommandGroup_D_OnActivate = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_FK_CommandGroup_D_OnActivate));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_CommandGroup_D_OnLoad = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_FK_CommandGroup_D_OnLoad));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_CommandGroup_D_OnUnload = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_FK_CommandGroup_D_OnUnload));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_CommandGroup_D_OnTimeout = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FK_CommandGroup_D_OnTimeout));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_CommandGroup_D_OnStartup = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_FK_CommandGroup_D_OnStartup));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_CommandGroup_D_OnHighlight = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_FK_CommandGroup_D_OnHighlight));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_CommandGroup_D_OnUnhighlight = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_FK_CommandGroup_D_OnUnhighlight));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_EffectType_Selected_WithChange = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_FK_EffectType_Selected_WithChange));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_EffectType_Selected_NoChange = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_FK_EffectType_Selected_NoChange));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_EffectType_Highlighted = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_FK_EffectType_Highlighted));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_FK_Button = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_FK_Button));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_FK_Criteria_Orbiter = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_FK_Criteria_Orbiter));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_DontResetSelectedState = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_DontResetSelectedState));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_FK_StabilityStatus));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_RepeatMS = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_RepeatMS));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_RepeatParm = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_RepeatParm));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_PreserveTransparencies = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_PreserveTransparencies));
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
sscanf(row[23], "%hi", &(pRow->m_psc_user));
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

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DesignObjVariation_pschmask *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DesignObjVariation_pschmask* Table_DesignObjVariation_pschmask::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DesignObjVariation_pschmask *pRow = new Row_DesignObjVariation_pschmask(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObjVariation_pschmask* Table_DesignObjVariation_pschmask::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_DesignObjVariation_pschmask*) (*i).second;
	//we have to fetch row
	Row_DesignObjVariation_pschmask* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObjVariation_pschmask* Table_DesignObjVariation_pschmask::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from DesignObjVariation_pschmask where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_pschmask::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DesignObjVariation_pschmask *pRow = new Row_DesignObjVariation_pschmask(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_DesignObjVariation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObj = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_FK_DesignObj));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_UI = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_FK_UI));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DesignObj_Goto = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_FK_DesignObj_Goto));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_CommandGroup_D_OnActivate = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_FK_CommandGroup_D_OnActivate));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_FK_CommandGroup_D_OnLoad = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_FK_CommandGroup_D_OnLoad));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_FK_CommandGroup_D_OnUnload = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_FK_CommandGroup_D_OnUnload));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_CommandGroup_D_OnTimeout = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FK_CommandGroup_D_OnTimeout));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_CommandGroup_D_OnStartup = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_FK_CommandGroup_D_OnStartup));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_CommandGroup_D_OnHighlight = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_FK_CommandGroup_D_OnHighlight));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_CommandGroup_D_OnUnhighlight = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_FK_CommandGroup_D_OnUnhighlight));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_EffectType_Selected_WithChange = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_FK_EffectType_Selected_WithChange));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_EffectType_Selected_NoChange = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_FK_EffectType_Selected_NoChange));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_EffectType_Highlighted = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_FK_EffectType_Highlighted));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_FK_Button = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_FK_Button));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_FK_Criteria_Orbiter = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_FK_Criteria_Orbiter));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_DontResetSelectedState = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_DontResetSelectedState));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_StabilityStatus = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_FK_StabilityStatus));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_RepeatMS = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_RepeatMS));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_RepeatParm = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_RepeatParm));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_PreserveTransparencies = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_PreserveTransparencies));
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
sscanf(row[23], "%hi", &(pRow->m_psc_user));
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


class Row_DesignObj* Row_DesignObjVariation_pschmask::FK_DesignObj_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj);
}
class Row_UI* Row_DesignObjVariation_pschmask::FK_UI_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_UI *pTable = table->database->UI_get();
return pTable->GetRow(m_FK_UI);
}
class Row_DesignObj* Row_DesignObjVariation_pschmask::FK_DesignObj_Goto_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Goto);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnActivate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnActivate);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnLoad_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnLoad);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnload_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnUnload);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnTimeout_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnTimeout);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnStartup_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnStartup);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnHighlight_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnHighlight);
}
class Row_CommandGroup_D* Row_DesignObjVariation_pschmask::FK_CommandGroup_D_OnUnhighlight_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D *pTable = table->database->CommandGroup_D_get();
return pTable->GetRow(m_FK_CommandGroup_D_OnUnhighlight);
}
class Row_EffectType* Row_DesignObjVariation_pschmask::FK_EffectType_Highlighted_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_EffectType *pTable = table->database->EffectType_get();
return pTable->GetRow(m_FK_EffectType_Highlighted);
}
class Row_Button* Row_DesignObjVariation_pschmask::FK_Button_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Button *pTable = table->database->Button_get();
return pTable->GetRow(m_FK_Button);
}
class Row_Criteria* Row_DesignObjVariation_pschmask::FK_Criteria_Orbiter_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Criteria *pTable = table->database->Criteria_get();
return pTable->GetRow(m_FK_Criteria_Orbiter);
}
class Row_StabilityStatus* Row_DesignObjVariation_pschmask::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}





