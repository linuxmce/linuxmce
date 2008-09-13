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
#include "Table_StyleVariation_pschist.h"
#include "Table_Style.h"
#include "Table_Skin.h"
#include "Table_UI.h"
#include "Table_HorizAlignment.h"
#include "Table_VertAlignment.h"



void Database_pluto_main::CreateTable_StyleVariation_pschist()
{
	tblStyleVariation_pschist = new Table_StyleVariation_pschist(this);
}

void Database_pluto_main::DeleteTable_StyleVariation_pschist()
{
	if( tblStyleVariation_pschist )
		delete tblStyleVariation_pschist;
}

bool Database_pluto_main::Commit_StyleVariation_pschist(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblStyleVariation_pschist->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_StyleVariation_pschist::~Table_StyleVariation_pschist()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_StyleVariation_pschist *pRow = (Row_StyleVariation_pschist *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_StyleVariation_pschist *pRow = (Row_StyleVariation_pschist *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_StyleVariation_pschist::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_StyleVariation_pschist *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_StyleVariation_pschist *) *i != this); i++);
			
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

void Row_StyleVariation_pschist::Reload()
{
	Row_StyleVariation_pschist *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_StyleVariation_pschist *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_StyleVariation_pschist::Row_StyleVariation_pschist(Table_StyleVariation_pschist *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_StyleVariation_pschist::SetDefaultValues()
{
	is_null[0] = true;
m_PK_StyleVariation = 0;
is_null[1] = true;
m_FK_Style = 0;
is_null[2] = true;
m_FK_Skin = 0;
is_null[3] = true;
m_FK_UI = 0;
is_null[4] = true;
is_null[5] = true;
m_ForeColor = 0;
is_null[6] = true;
m_PixelHeight = 0;
is_null[7] = true;
m_Bold = 0;
is_null[8] = true;
m_Italic = 0;
is_null[9] = true;
m_Underline = 0;
is_null[10] = true;
m_ShadowX = 0;
is_null[11] = true;
m_ShadowY = 0;
is_null[12] = true;
m_ShadowColor = 0;
is_null[13] = true;
m_BorderStyle = 0;
is_null[14] = true;
m_FK_HorizAlignment = 0;
is_null[15] = true;
m_FK_VertAlignment = 0;
is_null[16] = true;
m_BackColor = 0;
m_psc_id = 0;
is_null[17] = false;
m_psc_batch = 0;
is_null[18] = false;
is_null[19] = true;
m_psc_user = 0;
is_null[20] = true;
m_psc_frozen = 0;
is_null[21] = true;
m_psc_toc = 0;
is_null[22] = false;
is_null[23] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_StyleVariation_pschist::PK_StyleVariation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_StyleVariation;}
long int Row_StyleVariation_pschist::FK_Style_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Style;}
long int Row_StyleVariation_pschist::FK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin;}
long int Row_StyleVariation_pschist::FK_UI_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_UI;}
string Row_StyleVariation_pschist::Font_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Font;}
long int Row_StyleVariation_pschist::ForeColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ForeColor;}
long int Row_StyleVariation_pschist::PixelHeight_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PixelHeight;}
short int Row_StyleVariation_pschist::Bold_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Bold;}
short int Row_StyleVariation_pschist::Italic_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Italic;}
short int Row_StyleVariation_pschist::Underline_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Underline;}
long int Row_StyleVariation_pschist::ShadowX_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShadowX;}
long int Row_StyleVariation_pschist::ShadowY_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShadowY;}
long int Row_StyleVariation_pschist::ShadowColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShadowColor;}
long int Row_StyleVariation_pschist::BorderStyle_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_BorderStyle;}
long int Row_StyleVariation_pschist::FK_HorizAlignment_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_HorizAlignment;}
long int Row_StyleVariation_pschist::FK_VertAlignment_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_VertAlignment;}
long int Row_StyleVariation_pschist::BackColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_BackColor;}
long int Row_StyleVariation_pschist::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_StyleVariation_pschist::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_StyleVariation_pschist::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_StyleVariation_pschist::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_StyleVariation_pschist::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
short int Row_StyleVariation_pschist::psc_toc_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_toc;}
long int Row_StyleVariation_pschist::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_StyleVariation_pschist::PK_StyleVariation_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_StyleVariation = val; is_modified=true; is_null[0]=false;}
void Row_StyleVariation_pschist::FK_Style_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Style = val; is_modified=true; is_null[1]=false;}
void Row_StyleVariation_pschist::FK_Skin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin = val; is_modified=true; is_null[2]=false;}
void Row_StyleVariation_pschist::FK_UI_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_UI = val; is_modified=true; is_null[3]=false;}
void Row_StyleVariation_pschist::Font_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Font = val; is_modified=true; is_null[4]=false;}
void Row_StyleVariation_pschist::ForeColor_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ForeColor = val; is_modified=true; is_null[5]=false;}
void Row_StyleVariation_pschist::PixelHeight_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PixelHeight = val; is_modified=true; is_null[6]=false;}
void Row_StyleVariation_pschist::Bold_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Bold = val; is_modified=true; is_null[7]=false;}
void Row_StyleVariation_pschist::Italic_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Italic = val; is_modified=true; is_null[8]=false;}
void Row_StyleVariation_pschist::Underline_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Underline = val; is_modified=true; is_null[9]=false;}
void Row_StyleVariation_pschist::ShadowX_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShadowX = val; is_modified=true; is_null[10]=false;}
void Row_StyleVariation_pschist::ShadowY_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShadowY = val; is_modified=true; is_null[11]=false;}
void Row_StyleVariation_pschist::ShadowColor_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShadowColor = val; is_modified=true; is_null[12]=false;}
void Row_StyleVariation_pschist::BorderStyle_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_BorderStyle = val; is_modified=true; is_null[13]=false;}
void Row_StyleVariation_pschist::FK_HorizAlignment_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_HorizAlignment = val; is_modified=true; is_null[14]=false;}
void Row_StyleVariation_pschist::FK_VertAlignment_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_VertAlignment = val; is_modified=true; is_null[15]=false;}
void Row_StyleVariation_pschist::BackColor_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_BackColor = val; is_modified=true; is_null[16]=false;}
void Row_StyleVariation_pschist::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[17]=false;}
void Row_StyleVariation_pschist::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[18]=false;}
void Row_StyleVariation_pschist::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[19]=false;}
void Row_StyleVariation_pschist::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[20]=false;}
void Row_StyleVariation_pschist::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[21]=false;}
void Row_StyleVariation_pschist::psc_toc_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_toc = val; is_modified=true; is_null[22]=false;}
void Row_StyleVariation_pschist::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[23]=false;}

		
bool Row_StyleVariation_pschist::PK_StyleVariation_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_StyleVariation_pschist::FK_Style_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_StyleVariation_pschist::FK_Skin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_StyleVariation_pschist::FK_UI_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_StyleVariation_pschist::Font_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_StyleVariation_pschist::ForeColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_StyleVariation_pschist::PixelHeight_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_StyleVariation_pschist::Bold_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_StyleVariation_pschist::Italic_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_StyleVariation_pschist::Underline_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_StyleVariation_pschist::ShadowX_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_StyleVariation_pschist::ShadowY_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_StyleVariation_pschist::ShadowColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_StyleVariation_pschist::BorderStyle_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_StyleVariation_pschist::FK_HorizAlignment_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_StyleVariation_pschist::FK_VertAlignment_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_StyleVariation_pschist::BackColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_StyleVariation_pschist::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_StyleVariation_pschist::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_StyleVariation_pschist::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_StyleVariation_pschist::psc_toc_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_StyleVariation_pschist::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}

			
void Row_StyleVariation_pschist::PK_StyleVariation_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::FK_Style_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::FK_Skin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::FK_UI_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::Font_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::ForeColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::PixelHeight_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::Bold_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::Italic_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::Underline_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::ShadowX_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::ShadowY_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::ShadowColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::BorderStyle_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::FK_HorizAlignment_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::FK_VertAlignment_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::BackColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::psc_toc_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_StyleVariation_pschist::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
	

string Row_StyleVariation_pschist::PK_StyleVariation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_StyleVariation);

return buf;
}

string Row_StyleVariation_pschist::FK_Style_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style);

return buf;
}

string Row_StyleVariation_pschist::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin);

return buf;
}

string Row_StyleVariation_pschist::FK_UI_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_UI);

return buf;
}

string Row_StyleVariation_pschist::Font_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Font.c_str(), (unsigned long) min((size_t)30,m_Font.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StyleVariation_pschist::ForeColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ForeColor);

return buf;
}

string Row_StyleVariation_pschist::PixelHeight_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PixelHeight);

return buf;
}

string Row_StyleVariation_pschist::Bold_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Bold);

return buf;
}

string Row_StyleVariation_pschist::Italic_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Italic);

return buf;
}

string Row_StyleVariation_pschist::Underline_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Underline);

return buf;
}

string Row_StyleVariation_pschist::ShadowX_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ShadowX);

return buf;
}

string Row_StyleVariation_pschist::ShadowY_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ShadowY);

return buf;
}

string Row_StyleVariation_pschist::ShadowColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ShadowColor);

return buf;
}

string Row_StyleVariation_pschist::BorderStyle_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_BorderStyle);

return buf;
}

string Row_StyleVariation_pschist::FK_HorizAlignment_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_HorizAlignment);

return buf;
}

string Row_StyleVariation_pschist::FK_VertAlignment_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_VertAlignment);

return buf;
}

string Row_StyleVariation_pschist::BackColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_BackColor);

return buf;
}

string Row_StyleVariation_pschist::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_StyleVariation_pschist::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_StyleVariation_pschist::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_StyleVariation_pschist::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_StyleVariation_pschist::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StyleVariation_pschist::psc_toc_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_toc);

return buf;
}

string Row_StyleVariation_pschist::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_StyleVariation_pschist::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_StyleVariation_pschist::Key::Key(Row_StyleVariation_pschist *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_StyleVariation_pschist::Key_Less::operator()(const Table_StyleVariation_pschist::Key &key1, const Table_StyleVariation_pschist::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_StyleVariation_pschist::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_StyleVariation_pschist *pRow = (Row_StyleVariation_pschist *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_StyleVariation_asSQL()+", "+pRow->FK_Style_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->FK_UI_asSQL()+", "+pRow->Font_asSQL()+", "+pRow->ForeColor_asSQL()+", "+pRow->PixelHeight_asSQL()+", "+pRow->Bold_asSQL()+", "+pRow->Italic_asSQL()+", "+pRow->Underline_asSQL()+", "+pRow->ShadowX_asSQL()+", "+pRow->ShadowY_asSQL()+", "+pRow->ShadowColor_asSQL()+", "+pRow->BorderStyle_asSQL()+", "+pRow->FK_HorizAlignment_asSQL()+", "+pRow->FK_VertAlignment_asSQL()+", "+pRow->BackColor_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_toc_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into StyleVariation_pschist (`PK_StyleVariation`, `FK_Style`, `FK_Skin`, `FK_UI`, `Font`, `ForeColor`, `PixelHeight`, `Bold`, `Italic`, `Underline`, `ShadowX`, `ShadowY`, `ShadowColor`, `BorderStyle`, `FK_HorizAlignment`, `FK_VertAlignment`, `BackColor`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_toc`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_StyleVariation_pschist* pRow = (Row_StyleVariation_pschist*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_StyleVariation`="+pRow->PK_StyleVariation_asSQL()+", `FK_Style`="+pRow->FK_Style_asSQL()+", `FK_Skin`="+pRow->FK_Skin_asSQL()+", `FK_UI`="+pRow->FK_UI_asSQL()+", `Font`="+pRow->Font_asSQL()+", `ForeColor`="+pRow->ForeColor_asSQL()+", `PixelHeight`="+pRow->PixelHeight_asSQL()+", `Bold`="+pRow->Bold_asSQL()+", `Italic`="+pRow->Italic_asSQL()+", `Underline`="+pRow->Underline_asSQL()+", `ShadowX`="+pRow->ShadowX_asSQL()+", `ShadowY`="+pRow->ShadowY_asSQL()+", `ShadowColor`="+pRow->ShadowColor_asSQL()+", `BorderStyle`="+pRow->BorderStyle_asSQL()+", `FK_HorizAlignment`="+pRow->FK_HorizAlignment_asSQL()+", `FK_VertAlignment`="+pRow->FK_VertAlignment_asSQL()+", `BackColor`="+pRow->BackColor_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_toc`="+pRow->psc_toc_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update StyleVariation_pschist set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_StyleVariation_pschist* pRow = (Row_StyleVariation_pschist*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_StyleVariation_pschist* pRow = (Row_StyleVariation_pschist*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from StyleVariation_pschist where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_StyleVariation_pschist*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_StyleVariation_pschist::GetRows(string where_statement,vector<class Row_StyleVariation_pschist*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `StyleVariation_pschist`.* from StyleVariation_pschist " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `StyleVariation_pschist`.* from StyleVariation_pschist where " + where_statement;
	else
		query = "select `StyleVariation_pschist`.* from StyleVariation_pschist";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_StyleVariation_pschist *pRow = new Row_StyleVariation_pschist(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_StyleVariation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_StyleVariation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Style));
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
pRow->m_Font = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Font = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ForeColor = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_ForeColor));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_PixelHeight = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_PixelHeight));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Bold = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Bold));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Italic = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_Italic));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Underline = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Underline));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ShadowX = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_ShadowX));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_ShadowY = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_ShadowY));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_ShadowColor = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_ShadowColor));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_BorderStyle = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_BorderStyle));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_FK_HorizAlignment = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_HorizAlignment));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_FK_VertAlignment = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_FK_VertAlignment));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_BackColor = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_BackColor));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_id));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_batch));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_user));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_psc_frozen));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_psc_mod = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_toc = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%hi", &(pRow->m_psc_toc));
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

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_StyleVariation_pschist *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_StyleVariation_pschist* Table_StyleVariation_pschist::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_StyleVariation_pschist *pRow = new Row_StyleVariation_pschist(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_StyleVariation_pschist* Table_StyleVariation_pschist::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_StyleVariation_pschist*) (*i).second;
	//we have to fetch row
	Row_StyleVariation_pschist* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_StyleVariation_pschist* Table_StyleVariation_pschist::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from StyleVariation_pschist where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation_pschist::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_StyleVariation_pschist *pRow = new Row_StyleVariation_pschist(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_StyleVariation = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_StyleVariation));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_Style = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%li", &(pRow->m_FK_Style));
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
pRow->m_Font = "";
}
else
{
pRow->is_null[4]=false;
pRow->m_Font = string(row[4],lengths[4]);
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_ForeColor = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%li", &(pRow->m_ForeColor));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_PixelHeight = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%li", &(pRow->m_PixelHeight));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Bold = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Bold));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_Italic = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_Italic));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Underline = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Underline));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ShadowX = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_ShadowX));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_ShadowY = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%li", &(pRow->m_ShadowY));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_ShadowColor = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_ShadowColor));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_BorderStyle = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%li", &(pRow->m_BorderStyle));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_FK_HorizAlignment = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_HorizAlignment));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_FK_VertAlignment = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%li", &(pRow->m_FK_VertAlignment));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_BackColor = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_BackColor));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_psc_id));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_psc_batch));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_psc_user));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_psc_frozen));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_psc_mod = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_toc = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%hi", &(pRow->m_psc_toc));
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


class Row_Style* Row_StyleVariation_pschist::FK_Style_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style);
}
class Row_Skin* Row_StyleVariation_pschist::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}
class Row_UI* Row_StyleVariation_pschist::FK_UI_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_UI *pTable = table->database->UI_get();
return pTable->GetRow(m_FK_UI);
}
class Row_HorizAlignment* Row_StyleVariation_pschist::FK_HorizAlignment_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_HorizAlignment *pTable = table->database->HorizAlignment_get();
return pTable->GetRow(m_FK_HorizAlignment);
}
class Row_VertAlignment* Row_StyleVariation_pschist::FK_VertAlignment_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_VertAlignment *pTable = table->database->VertAlignment_get();
return pTable->GetRow(m_FK_VertAlignment);
}





