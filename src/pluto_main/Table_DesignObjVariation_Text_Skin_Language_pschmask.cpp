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
#include "Table_DesignObjVariation_Text_Skin_Language_pschmask.h"
#include "Table_DesignObjVariation_Text.h"
#include "Table_Skin.h"
#include "Table_Language.h"
#include "Table_HorizAlignment.h"
#include "Table_VertAlignment.h"
#include "Table_Style.h"



void Database_pluto_main::CreateTable_DesignObjVariation_Text_Skin_Language_pschmask()
{
	tblDesignObjVariation_Text_Skin_Language_pschmask = new Table_DesignObjVariation_Text_Skin_Language_pschmask(this);
}

void Database_pluto_main::DeleteTable_DesignObjVariation_Text_Skin_Language_pschmask()
{
	if( tblDesignObjVariation_Text_Skin_Language_pschmask )
		delete tblDesignObjVariation_Text_Skin_Language_pschmask;
}

bool Database_pluto_main::Commit_DesignObjVariation_Text_Skin_Language_pschmask(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDesignObjVariation_Text_Skin_Language_pschmask->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DesignObjVariation_Text_Skin_Language_pschmask::~Table_DesignObjVariation_Text_Skin_Language_pschmask()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DesignObjVariation_Text_Skin_Language_pschmask::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DesignObjVariation_Text_Skin_Language_pschmask *) *i != this); i++);
			
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

void Row_DesignObjVariation_Text_Skin_Language_pschmask::Reload()
{
	Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DesignObjVariation_Text_Skin_Language_pschmask::Row_DesignObjVariation_Text_Skin_Language_pschmask(Table_DesignObjVariation_Text_Skin_Language_pschmask *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DesignObjVariation_Text_Skin_Language_pschmask::SetDefaultValues()
{
	is_null[0] = true;
m_PK_DesignObjVariation_Text_Skin_Language = 0;
is_null[1] = true;
m_FK_DesignObjVariation_Text = 0;
is_null[2] = true;
m_FK_Skin = 0;
is_null[3] = true;
m_FK_Language = 0;
is_null[4] = true;
m_X = 0;
is_null[5] = true;
m_Y = 0;
is_null[6] = true;
m_Width = 0;
is_null[7] = true;
m_Height = 0;
is_null[8] = true;
m_Rotate = 0;
is_null[9] = true;
m_Opacity = 0;
is_null[10] = true;
m_FK_HorizAlignment = 0;
is_null[11] = true;
m_FK_VertAlignment = 0;
is_null[12] = true;
m_FK_Style = 0;
is_null[13] = true;
m_PlainBackgroundColor = 0;
m_psc_id = 0;
is_null[14] = false;
m_psc_batch = 0;
is_null[15] = false;
is_null[16] = true;
m_psc_user = 0;
is_null[17] = true;
m_psc_frozen = 0;
is_null[18] = true;
is_null[19] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

short int Row_DesignObjVariation_Text_Skin_Language_pschmask::PK_DesignObjVariation_Text_Skin_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DesignObjVariation_Text_Skin_Language;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_DesignObjVariation_Text_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DesignObjVariation_Text;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Language;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::X_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_X;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::Y_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Y;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::Width_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Width;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::Height_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Height;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::Rotate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Rotate;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::Opacity_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Opacity;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_HorizAlignment_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_HorizAlignment;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_VertAlignment_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_VertAlignment;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Style_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Style;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::PlainBackgroundColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PlainBackgroundColor;}
long int Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DesignObjVariation_Text_Skin_Language_pschmask::PK_DesignObjVariation_Text_Skin_Language_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DesignObjVariation_Text_Skin_Language = val; is_modified=true; is_null[0]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_DesignObjVariation_Text_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DesignObjVariation_Text = val; is_modified=true; is_null[1]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Skin_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin = val; is_modified=true; is_null[2]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Language_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Language = val; is_modified=true; is_null[3]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::X_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_X = val; is_modified=true; is_null[4]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Y_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Y = val; is_modified=true; is_null[5]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Width_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Width = val; is_modified=true; is_null[6]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Height_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Height = val; is_modified=true; is_null[7]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Rotate_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Rotate = val; is_modified=true; is_null[8]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Opacity_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Opacity = val; is_modified=true; is_null[9]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_HorizAlignment_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_HorizAlignment = val; is_modified=true; is_null[10]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_VertAlignment_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_VertAlignment = val; is_modified=true; is_null[11]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Style_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Style = val; is_modified=true; is_null[12]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::PlainBackgroundColor_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PlainBackgroundColor = val; is_modified=true; is_null[13]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[14]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[15]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_user_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[16]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[17]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[18]=false;}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[19]=false;}

		
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::PK_DesignObjVariation_Text_Skin_Language_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_DesignObjVariation_Text_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Skin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Language_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::X_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::Y_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::Width_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::Height_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::Rotate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::Opacity_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_HorizAlignment_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_VertAlignment_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Style_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::PlainBackgroundColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}

			
void Row_DesignObjVariation_Text_Skin_Language_pschmask::PK_DesignObjVariation_Text_Skin_Language_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_DesignObjVariation_Text_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Skin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Language_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::X_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Y_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Width_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Height_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Rotate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::Opacity_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_HorizAlignment_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_VertAlignment_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Style_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::PlainBackgroundColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
	

string Row_DesignObjVariation_Text_Skin_Language_pschmask::PK_DesignObjVariation_Text_Skin_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PK_DesignObjVariation_Text_Skin_Language);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_DesignObjVariation_Text_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_DesignObjVariation_Text);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Skin);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Language);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::X_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_X);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::Y_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Y);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::Width_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Width);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::Height_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Height);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::Rotate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Rotate);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::Opacity_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Opacity);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_HorizAlignment_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_HorizAlignment);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_VertAlignment_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_VertAlignment);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Style_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Style);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::PlainBackgroundColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PlainBackgroundColor);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_user);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_mod_asSQL()
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

string Row_DesignObjVariation_Text_Skin_Language_pschmask::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DesignObjVariation_Text_Skin_Language_pschmask::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_DesignObjVariation_Text_Skin_Language_pschmask::Key::Key(Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_DesignObjVariation_Text_Skin_Language_pschmask::Key_Less::operator()(const Table_DesignObjVariation_Text_Skin_Language_pschmask::Key &key1, const Table_DesignObjVariation_Text_Skin_Language_pschmask::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_DesignObjVariation_Text_Skin_Language_pschmask::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DesignObjVariation_Text_Skin_Language_asSQL()+", "+pRow->FK_DesignObjVariation_Text_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->FK_Language_asSQL()+", "+pRow->X_asSQL()+", "+pRow->Y_asSQL()+", "+pRow->Width_asSQL()+", "+pRow->Height_asSQL()+", "+pRow->Rotate_asSQL()+", "+pRow->Opacity_asSQL()+", "+pRow->FK_HorizAlignment_asSQL()+", "+pRow->FK_VertAlignment_asSQL()+", "+pRow->FK_Style_asSQL()+", "+pRow->PlainBackgroundColor_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DesignObjVariation_Text_Skin_Language_pschmask (`PK_DesignObjVariation_Text_Skin_Language`, `FK_DesignObjVariation_Text`, `FK_Skin`, `FK_Language`, `X`, `Y`, `Width`, `Height`, `Rotate`, `Opacity`, `FK_HorizAlignment`, `FK_VertAlignment`, `FK_Style`, `PlainBackgroundColor`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObjVariation_Text_Skin_Language_pschmask* pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DesignObjVariation_Text_Skin_Language`="+pRow->PK_DesignObjVariation_Text_Skin_Language_asSQL()+", `FK_DesignObjVariation_Text`="+pRow->FK_DesignObjVariation_Text_asSQL()+", `FK_Skin`="+pRow->FK_Skin_asSQL()+", `FK_Language`="+pRow->FK_Language_asSQL()+", `X`="+pRow->X_asSQL()+", `Y`="+pRow->Y_asSQL()+", `Width`="+pRow->Width_asSQL()+", `Height`="+pRow->Height_asSQL()+", `Rotate`="+pRow->Rotate_asSQL()+", `Opacity`="+pRow->Opacity_asSQL()+", `FK_HorizAlignment`="+pRow->FK_HorizAlignment_asSQL()+", `FK_VertAlignment`="+pRow->FK_VertAlignment_asSQL()+", `FK_Style`="+pRow->FK_Style_asSQL()+", `PlainBackgroundColor`="+pRow->PlainBackgroundColor_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DesignObjVariation_Text_Skin_Language_pschmask set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DesignObjVariation_Text_Skin_Language_pschmask* pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DesignObjVariation_Text_Skin_Language_pschmask* pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from DesignObjVariation_Text_Skin_Language_pschmask where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DesignObjVariation_Text_Skin_Language_pschmask::GetRows(string where_statement,vector<class Row_DesignObjVariation_Text_Skin_Language_pschmask*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DesignObjVariation_Text_Skin_Language_pschmask`.* from DesignObjVariation_Text_Skin_Language_pschmask " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DesignObjVariation_Text_Skin_Language_pschmask`.* from DesignObjVariation_Text_Skin_Language_pschmask where " + where_statement;
	else
		query = "select `DesignObjVariation_Text_Skin_Language_pschmask`.* from DesignObjVariation_Text_Skin_Language_pschmask";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = new Row_DesignObjVariation_Text_Skin_Language_pschmask(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation_Text_Skin_Language = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_DesignObjVariation_Text_Skin_Language));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjVariation_Text = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_FK_DesignObjVariation_Text));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_FK_Skin));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_FK_Language));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_X = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_X));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Y = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_Y));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Width));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Height));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Rotate = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_Rotate));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Opacity = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Opacity));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_HorizAlignment = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_FK_HorizAlignment));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_VertAlignment = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_FK_VertAlignment));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_FK_Style));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_PlainBackgroundColor = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_PlainBackgroundColor));
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
sscanf(row[16], "%hi", &(pRow->m_psc_user));
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

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DesignObjVariation_Text_Skin_Language_pschmask *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DesignObjVariation_Text_Skin_Language_pschmask* Table_DesignObjVariation_Text_Skin_Language_pschmask::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = new Row_DesignObjVariation_Text_Skin_Language_pschmask(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DesignObjVariation_Text_Skin_Language_pschmask* Table_DesignObjVariation_Text_Skin_Language_pschmask::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_DesignObjVariation_Text_Skin_Language_pschmask*) (*i).second;
	//we have to fetch row
	Row_DesignObjVariation_Text_Skin_Language_pschmask* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DesignObjVariation_Text_Skin_Language_pschmask* Table_DesignObjVariation_Text_Skin_Language_pschmask::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from DesignObjVariation_Text_Skin_Language_pschmask where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DesignObjVariation_Text_Skin_Language_pschmask::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DesignObjVariation_Text_Skin_Language_pschmask *pRow = new Row_DesignObjVariation_Text_Skin_Language_pschmask(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DesignObjVariation_Text_Skin_Language = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_DesignObjVariation_Text_Skin_Language));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DesignObjVariation_Text = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_FK_DesignObjVariation_Text));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_Skin = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_FK_Skin));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_FK_Language));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_X = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_X));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Y = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_Y));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Width = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Width));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Height = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Height));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Rotate = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_Rotate));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Opacity = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Opacity));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_HorizAlignment = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_FK_HorizAlignment));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_VertAlignment = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_FK_VertAlignment));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_FK_Style));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_PlainBackgroundColor = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_PlainBackgroundColor));
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
sscanf(row[16], "%hi", &(pRow->m_psc_user));
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


class Row_DesignObjVariation_Text* Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_DesignObjVariation_Text_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DesignObjVariation_Text *pTable = table->database->DesignObjVariation_Text_get();
return pTable->GetRow(m_FK_DesignObjVariation_Text);
}
class Row_Skin* Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}
class Row_Language* Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Language_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language);
}
class Row_HorizAlignment* Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_HorizAlignment_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_HorizAlignment *pTable = table->database->HorizAlignment_get();
return pTable->GetRow(m_FK_HorizAlignment);
}
class Row_VertAlignment* Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_VertAlignment_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_VertAlignment *pTable = table->database->VertAlignment_get();
return pTable->GetRow(m_FK_VertAlignment);
}
class Row_Style* Row_DesignObjVariation_Text_Skin_Language_pschmask::FK_Style_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style);
}





