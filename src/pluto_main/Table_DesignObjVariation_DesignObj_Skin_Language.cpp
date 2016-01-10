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
#include "Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "Table_DesignObjVariation_DesignObj.h"
#include "Table_Skin.h"
#include "Table_Language.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"
#include "Table_DesignObj.h"



void Database_pluto_main::CreateTable_DesignObjVariation_DesignObj_Skin_Language()
{
	tblDesignObjVariation_DesignObj_Skin_Language = new Table_DesignObjVariation_DesignObj_Skin_Language(this);
}

void Database_pluto_main::DeleteTable_DesignObjVariation_DesignObj_Skin_Language()
{
	if( tblDesignObjVariation_DesignObj_Skin_Language )
		delete tblDesignObjVariation_DesignObj_Skin_Language;
}

bool Database_pluto_main::Commit_DesignObjVariation_DesignObj_Skin_Language(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDesignObjVariation_DesignObj_Skin_Language->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DesignObjVariation_DesignObj_Skin_Language::~Table_DesignObjVariation_DesignObj_Skin_Language()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DesignObjVariation_DesignObj_Skin_Language *pRow = (Row_DesignObjVariation_DesignObj_Skin_Language *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DesignObjVariation_DesignObj_Skin_Language *pRow = (Row_DesignObjVariation_DesignObj_Skin_Language *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DesignObjVariation_DesignObj_Skin_Language::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DesignObjVariation_DesignObj_Skin_Language *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DesignObjVariation_DesignObj_Skin_Language *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_DesignObjVariation_DesignObj_Skin_Language::Reload()
{
	Row_DesignObjVariation_DesignObj_Skin_Language *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language);
		Row_DesignObjVariation_DesignObj_Skin_Language *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObjVariation_DesignObj_Skin_Language::Row_DesignObjVariation_DesignObj_Skin_Language(Table_DesignObjVariation_DesignObj_Skin_Language *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObjVariation_DesignObj_Skin_Language::SetDefaultValues()
{
	m_PK_DesignObjVariation_DesignObj_Skin_Language = 0;
is_null[0] = false;
is_null[1] = true;
m_FK_DesignObjVariation_DesignObj = 0;
is_null[2] = true;
m_FK_Skin = 0;
is_null[3] = true;
m_FK_Language = 0;
m_DisplayOrder = 0;
is_null[4] = false;
m_X = 0;
is_null[5] = false;
m_Y = 0;
is_null[6] = false;
is_null[7] = true;
m_Width = 0;
is_null[8] = true;
m_Height = 0;
is_null[9] = true;
m_FK_DesignObj_InsteadOf = 0;
m_CanBeHidden = 0;
is_null[10] = false;
m_HideByDefault = 0;
is_null[11] = false;
m_RegenerateForEachScreen = 0;
is_null[12] = false;
m_DisplayChildrenBeforeText = 0;
is_null[13] = false;
m_DisplayChildrenBehindBackground = 0;
is_null[14] = false;
m_DontMergeBackground = 0;
is_null[15] = false;
m_IsTabStop = 0;
is_null[16] = false;
is_null[17] = true;
m_FK_DesignObj_Up = 0;
is_null[18] = true;
m_FK_DesignObj_Down = 0;
is_null[19] = true;
m_FK_DesignObj_Left = 0;
is_null[20] = true;
m_FK_DesignObj_Right = 0;
is_null[21] = true;
is_null[22] = true;
m_Ignore = 0;
is_null[23] = false;
is_null[24] = true;
m_psc_id = 0;
is_null[25] = true;
m_psc_batch = 0;
is_null[26] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[27] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[28] = false;
is_null[29] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DesignObjVariation_DesignObj_Skin_Language::PK_DesignObjVariation_DesignObj_Skin_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DesignObjVariation_DesignObj_Skin_Language;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObjVariation_DesignObj_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjVariation_DesignObj;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Language;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::DisplayOrder_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DisplayOrder;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::X_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_X;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::Y_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Y;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::Width_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Width;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::Height_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Height;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_InsteadOf_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_InsteadOf;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::CanBeHidden_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CanBeHidden;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::HideByDefault_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HideByDefault;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::RegenerateForEachScreen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RegenerateForEachScreen;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::DisplayChildrenBeforeText_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DisplayChildrenBeforeText;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::DisplayChildrenBehindBackground_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DisplayChildrenBehindBackground;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::DontMergeBackground_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DontMergeBackground;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::IsTabStop_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsTabStop;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Up_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Up;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Down_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Down;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Left_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Left;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Right_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObj_Right;}
string Row_DesignObjVariation_DesignObj_Skin_Language::sFK_DesignObj_TiedTo_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_sFK_DesignObj_TiedTo;}
string Row_DesignObjVariation_DesignObj_Skin_Language::VisibleStates_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VisibleStates;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::Ignore_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Ignore;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DesignObjVariation_DesignObj_Skin_Language::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DesignObjVariation_DesignObj_Skin_Language::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DesignObjVariation_DesignObj_Skin_Language::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DesignObjVariation_DesignObj_Skin_Language::PK_DesignObjVariation_DesignObj_Skin_Language_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DesignObjVariation_DesignObj_Skin_Language = val; is_modified=true; is_null[0]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObjVariation_DesignObj_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjVariation_DesignObj = val; is_modified=true; is_null[1]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_Skin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin = val; is_modified=true; is_null[2]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_Language_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Language = val; is_modified=true; is_null[3]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::DisplayOrder_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DisplayOrder = val; is_modified=true; is_null[4]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::X_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_X = val; is_modified=true; is_null[5]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::Y_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Y = val; is_modified=true; is_null[6]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::Width_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Width = val; is_modified=true; is_null[7]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::Height_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Height = val; is_modified=true; is_null[8]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_InsteadOf_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_InsteadOf = val; is_modified=true; is_null[9]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::CanBeHidden_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CanBeHidden = val; is_modified=true; is_null[10]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::HideByDefault_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HideByDefault = val; is_modified=true; is_null[11]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::RegenerateForEachScreen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RegenerateForEachScreen = val; is_modified=true; is_null[12]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::DisplayChildrenBeforeText_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DisplayChildrenBeforeText = val; is_modified=true; is_null[13]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::DisplayChildrenBehindBackground_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DisplayChildrenBehindBackground = val; is_modified=true; is_null[14]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::DontMergeBackground_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DontMergeBackground = val; is_modified=true; is_null[15]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::IsTabStop_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsTabStop = val; is_modified=true; is_null[16]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Up_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Up = val; is_modified=true; is_null[17]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Down_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Down = val; is_modified=true; is_null[18]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Left_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Left = val; is_modified=true; is_null[19]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Right_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObj_Right = val; is_modified=true; is_null[20]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::sFK_DesignObj_TiedTo_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_sFK_DesignObj_TiedTo = val; is_modified=true; is_null[21]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::VisibleStates_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VisibleStates = val; is_modified=true; is_null[22]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::Ignore_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Ignore = val; is_modified=true; is_null[23]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[24]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[25]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[26]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[27]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[28]=false;}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[29]=false;}

		
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObjVariation_DesignObj_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_Skin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_Language_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::Width_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::Height_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_InsteadOf_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Up_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Down_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Left_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Right_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::sFK_DesignObj_TiedTo_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::VisibleStates_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[27];}
bool Row_DesignObjVariation_DesignObj_Skin_Language::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[29];}

			
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObjVariation_DesignObj_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_Skin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_Language_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::Width_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::Height_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_InsteadOf_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Up_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Down_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Left_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Right_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::sFK_DesignObj_TiedTo_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::VisibleStates_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[27]=val;
is_modified=true;
}
void Row_DesignObjVariation_DesignObj_Skin_Language::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[29]=val;
is_modified=true;
}
	

string Row_DesignObjVariation_DesignObj_Skin_Language::PK_DesignObjVariation_DesignObj_Skin_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DesignObjVariation_DesignObj_Skin_Language);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObjVariation_DesignObj_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObjVariation_DesignObj);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Language);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::DisplayOrder_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_DisplayOrder);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::X_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_X);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::Y_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Y);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::Width_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Width);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::Height_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Height);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_InsteadOf_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_InsteadOf);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::CanBeHidden_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_CanBeHidden);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::HideByDefault_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HideByDefault);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::RegenerateForEachScreen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RegenerateForEachScreen);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::DisplayChildrenBeforeText_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DisplayChildrenBeforeText);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::DisplayChildrenBehindBackground_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DisplayChildrenBehindBackground);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::DontMergeBackground_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_DontMergeBackground);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::IsTabStop_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsTabStop);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Up_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Up);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Down_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Down);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Left_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Left);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Right_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DesignObj_Right);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::sFK_DesignObj_TiedTo_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char *buf = new char[29];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_sFK_DesignObj_TiedTo.c_str(), (unsigned long) min((size_t)14,m_sFK_DesignObj_TiedTo.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::VisibleStates_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char *buf = new char[21];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_VisibleStates.c_str(), (unsigned long) min((size_t)10,m_VisibleStates.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::Ignore_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Ignore);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[27])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[28])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DesignObjVariation_DesignObj_Skin_Language::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[29])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DesignObjVariation_DesignObj_Skin_Language::Key::Key(long int in_PK_DesignObjVariation_DesignObj_Skin_Language)
{
			pk_PK_DesignObjVariation_DesignObj_Skin_Language = in_PK_DesignObjVariation_DesignObj_Skin_Language;
	
}

Table_DesignObjVariation_DesignObj_Skin_Language::Key::Key(Row_DesignObjVariation_DesignObj_Skin_Language *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_DesignObjVariation_DesignObj_Skin_Language = pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language;
	
}		

bool Table_DesignObjVariation_DesignObj_Skin_Language::Key_Less::operator()(const Table_DesignObjVariation_DesignObj_Skin_Language::Key &key1, const Table_DesignObjVariation_DesignObj_Skin_Language::Key &key2) const
{
			if (key1.pk_PK_DesignObjVariation_DesignObj_Skin_Language!=key2.pk_PK_DesignObjVariation_DesignObj_Skin_Language)
return key1.pk_PK_DesignObjVariation_DesignObj_Skin_Language<key2.pk_PK_DesignObjVariation_DesignObj_Skin_Language;
else
return false;	
}	

bool Table_DesignObjVariation_DesignObj_Skin_Language::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DesignObjVariation_DesignObj_Skin_Language *pRow = (Row_DesignObjVariation_DesignObj_Skin_Language *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DesignObjVariation_DesignObj_Skin_Language_asSQL()+", "+pRow->FK_DesignObjVariation_DesignObj_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->FK_Language_asSQL()+", "+pRow->DisplayOrder_asSQL()+", "+pRow->X_asSQL()+", "+pRow->Y_asSQL()+", "+pRow->Width_asSQL()+", "+pRow->Height_asSQL()+", "+pRow->FK_DesignObj_InsteadOf_asSQL()+", "+pRow->CanBeHidden_asSQL()+", "+pRow->HideByDefault_asSQL()+", "+pRow->RegenerateForEachScreen_asSQL()+", "+pRow->DisplayChildrenBeforeText_asSQL()+", "+pRow->DisplayChildrenBehindBackground_asSQL()+", "+pRow->DontMergeBackground_asSQL()+", "+pRow->IsTabStop_asSQL()+", "+pRow->FK_DesignObj_Up_asSQL()+", "+pRow->FK_DesignObj_Down_asSQL()+", "+pRow->FK_DesignObj_Left_asSQL()+", "+pRow->FK_DesignObj_Right_asSQL()+", "+pRow->sFK_DesignObj_TiedTo_asSQL()+", "+pRow->VisibleStates_asSQL()+", "+pRow->Ignore_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DesignObjVariation_DesignObj_Skin_Language (`PK_DesignObjVariation_DesignObj_Skin_Language`, `FK_DesignObjVariation_DesignObj`, `FK_Skin`, `FK_Language`, `DisplayOrder`, `X`, `Y`, `Width`, `Height`, `FK_DesignObj_InsteadOf`, `CanBeHidden`, `HideByDefault`, `RegenerateForEachScreen`, `DisplayChildrenBeforeText`, `DisplayChildrenBehindBackground`, `DontMergeBackground`, `IsTabStop`, `FK_DesignObj_Up`, `FK_DesignObj_Down`, `FK_DesignObj_Left`, `FK_DesignObj_Right`, `sFK_DesignObj_TiedTo`, `VisibleStates`, `Ignore`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_DesignObjVariation_DesignObj_Skin_Language is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DesignObjVariation_DesignObj_Skin_Language* pRow = (Row_DesignObjVariation_DesignObj_Skin_Language*) (*i).second;	
		SingleLongKey key(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language);

		char tmp_PK_DesignObjVariation_DesignObj_Skin_Language[32];
sprintf(tmp_PK_DesignObjVariation_DesignObj_Skin_Language, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObjVariation_DesignObj_Skin_Language`=" + tmp_PK_DesignObjVariation_DesignObj_Skin_Language;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DesignObjVariation_DesignObj_Skin_Language`="+pRow->PK_DesignObjVariation_DesignObj_Skin_Language_asSQL()+", `FK_DesignObjVariation_DesignObj`="+pRow->FK_DesignObjVariation_DesignObj_asSQL()+", `FK_Skin`="+pRow->FK_Skin_asSQL()+", `FK_Language`="+pRow->FK_Language_asSQL()+", `DisplayOrder`="+pRow->DisplayOrder_asSQL()+", `X`="+pRow->X_asSQL()+", `Y`="+pRow->Y_asSQL()+", `Width`="+pRow->Width_asSQL()+", `Height`="+pRow->Height_asSQL()+", `FK_DesignObj_InsteadOf`="+pRow->FK_DesignObj_InsteadOf_asSQL()+", `CanBeHidden`="+pRow->CanBeHidden_asSQL()+", `HideByDefault`="+pRow->HideByDefault_asSQL()+", `RegenerateForEachScreen`="+pRow->RegenerateForEachScreen_asSQL()+", `DisplayChildrenBeforeText`="+pRow->DisplayChildrenBeforeText_asSQL()+", `DisplayChildrenBehindBackground`="+pRow->DisplayChildrenBehindBackground_asSQL()+", `DontMergeBackground`="+pRow->DontMergeBackground_asSQL()+", `IsTabStop`="+pRow->IsTabStop_asSQL()+", `FK_DesignObj_Up`="+pRow->FK_DesignObj_Up_asSQL()+", `FK_DesignObj_Down`="+pRow->FK_DesignObj_Down_asSQL()+", `FK_DesignObj_Left`="+pRow->FK_DesignObj_Left_asSQL()+", `FK_DesignObj_Right`="+pRow->FK_DesignObj_Right_asSQL()+", `sFK_DesignObj_TiedTo`="+pRow->sFK_DesignObj_TiedTo_asSQL()+", `VisibleStates`="+pRow->VisibleStates_asSQL()+", `Ignore`="+pRow->Ignore_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DesignObjVariation_DesignObj_Skin_Language set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObjVariation_DesignObj_Skin_Language* pRow = (Row_DesignObjVariation_DesignObj_Skin_Language*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_DesignObjVariation_DesignObj_Skin_Language* pRow = (Row_DesignObjVariation_DesignObj_Skin_Language*) (*i).second;	

		char tmp_PK_DesignObjVariation_DesignObj_Skin_Language[32];
sprintf(tmp_PK_DesignObjVariation_DesignObj_Skin_Language, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObjVariation_DesignObj_Skin_Language`=" + tmp_PK_DesignObjVariation_DesignObj_Skin_Language;

	
		string query = "delete from DesignObjVariation_DesignObj_Skin_Language where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DesignObjVariation_DesignObj_Skin_Language*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DesignObjVariation_DesignObj_Skin_Language::GetRows(string where_statement,vector<class Row_DesignObjVariation_DesignObj_Skin_Language*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DesignObjVariation_DesignObj_Skin_Language`.* from DesignObjVariation_DesignObj_Skin_Language " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DesignObjVariation_DesignObj_Skin_Language`.* from DesignObjVariation_DesignObj_Skin_Language where " + where_statement;
	else
		query = "select `DesignObjVariation_DesignObj_Skin_Language`.* from DesignObjVariation_DesignObj_Skin_Language";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DesignObjVariation_DesignObj_Skin_Language *pRow = new Row_DesignObjVariation_DesignObj_Skin_Language(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjVariation_DesignObj = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObjVariation_DesignObj));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Skin));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Language));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DisplayOrder = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_DisplayOrder));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_X = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_X));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Y = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_Y));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Width));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_Height));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj_InsteadOf = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj_InsteadOf));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_CanBeHidden = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_CanBeHidden));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_HideByDefault = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_HideByDefault));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_RegenerateForEachScreen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_RegenerateForEachScreen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_DisplayChildrenBeforeText = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_DisplayChildrenBeforeText));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_DisplayChildrenBehindBackground = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_DisplayChildrenBehindBackground));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_DontMergeBackground = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_DontMergeBackground));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_IsTabStop = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_IsTabStop));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_DesignObj_Up = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_DesignObj_Up));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_DesignObj_Down = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_DesignObj_Down));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_FK_DesignObj_Left = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_FK_DesignObj_Left));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_FK_DesignObj_Right = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_FK_DesignObj_Right));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_sFK_DesignObj_TiedTo = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_sFK_DesignObj_TiedTo = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_VisibleStates = "";
}
else
{
pRow->is_null[22]=false;
pRow->m_VisibleStates = string(row[22],lengths[22]);
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_Ignore = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%hi", &(pRow->m_Ignore));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_id));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_psc_batch));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_psc_user));
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%hi", &(pRow->m_psc_frozen));
}

if (row[28] == NULL)
{
pRow->is_null[28]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[28]=false;
pRow->m_psc_mod = string(row[28],lengths[28]);
}

if (row[29] == NULL)
{
pRow->is_null[29]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[29]=false;
sscanf(row[29], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DesignObjVariation_DesignObj_Skin_Language *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DesignObjVariation_DesignObj_Skin_Language* Table_DesignObjVariation_DesignObj_Skin_Language::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DesignObjVariation_DesignObj_Skin_Language *pRow = new Row_DesignObjVariation_DesignObj_Skin_Language(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObjVariation_DesignObj_Skin_Language* Table_DesignObjVariation_DesignObj_Skin_Language::GetRow(long int in_PK_DesignObjVariation_DesignObj_Skin_Language)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_DesignObjVariation_DesignObj_Skin_Language);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DesignObjVariation_DesignObj_Skin_Language*) (*i).second;
	//we have to fetch row
	Row_DesignObjVariation_DesignObj_Skin_Language* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObjVariation_DesignObj_Skin_Language* Table_DesignObjVariation_DesignObj_Skin_Language::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_DesignObjVariation_DesignObj_Skin_Language[32];
sprintf(tmp_PK_DesignObjVariation_DesignObj_Skin_Language, "%li", key.pk);


string condition;
condition = condition + "`PK_DesignObjVariation_DesignObj_Skin_Language`=" + tmp_PK_DesignObjVariation_DesignObj_Skin_Language;


	string query = "select * from DesignObjVariation_DesignObj_Skin_Language where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_DesignObj_Skin_Language::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DesignObjVariation_DesignObj_Skin_Language *pRow = new Row_DesignObjVariation_DesignObj_Skin_Language(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DesignObjVariation_DesignObj_Skin_Language));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjVariation_DesignObj = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_DesignObjVariation_DesignObj));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%li", &(pRow->m_FK_Skin));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_Language));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_DisplayOrder = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_DisplayOrder));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_X = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_X));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Y = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_Y));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%li", &(pRow->m_Width));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%li", &(pRow->m_Height));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_FK_DesignObj_InsteadOf = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%li", &(pRow->m_FK_DesignObj_InsteadOf));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_CanBeHidden = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_CanBeHidden));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_HideByDefault = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_HideByDefault));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_RegenerateForEachScreen = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_RegenerateForEachScreen));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_DisplayChildrenBeforeText = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_DisplayChildrenBeforeText));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_DisplayChildrenBehindBackground = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_DisplayChildrenBehindBackground));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_DontMergeBackground = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_DontMergeBackground));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_IsTabStop = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_IsTabStop));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_DesignObj_Up = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_DesignObj_Up));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_DesignObj_Down = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_DesignObj_Down));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_FK_DesignObj_Left = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_FK_DesignObj_Left));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_FK_DesignObj_Right = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_FK_DesignObj_Right));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_sFK_DesignObj_TiedTo = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_sFK_DesignObj_TiedTo = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_VisibleStates = "";
}
else
{
pRow->is_null[22]=false;
pRow->m_VisibleStates = string(row[22],lengths[22]);
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_Ignore = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%hi", &(pRow->m_Ignore));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_psc_id));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_psc_batch));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_psc_user));
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%hi", &(pRow->m_psc_frozen));
}

if (row[28] == NULL)
{
pRow->is_null[28]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[28]=false;
pRow->m_psc_mod = string(row[28],lengths[28]);
}

if (row[29] == NULL)
{
pRow->is_null[29]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[29]=false;
sscanf(row[29], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DesignObjVariation_DesignObj* Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObjVariation_DesignObj_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_DesignObj *pTable = table->database->DesignObjVariation_DesignObj_get();
return pTable->GetRow(m_FK_DesignObjVariation_DesignObj);
}
class Row_Skin* Row_DesignObjVariation_DesignObj_Skin_Language::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}
class Row_Language* Row_DesignObjVariation_DesignObj_Skin_Language::FK_Language_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language);
}
class Row_DesignObj* Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_InsteadOf_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_InsteadOf);
}
class Row_DesignObj* Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Up_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Up);
}
class Row_DesignObj* Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Down_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Down);
}
class Row_DesignObj* Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Left_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Left);
}
class Row_DesignObj* Row_DesignObjVariation_DesignObj_Skin_Language::FK_DesignObj_Right_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObj *pTable = table->database->DesignObj_get();
return pTable->GetRow(m_FK_DesignObj_Right);
}





