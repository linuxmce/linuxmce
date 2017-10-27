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
#include "Table_StyleVariation.h"
#include "Table_Style.h"
#include "Table_Skin.h"
#include "Table_UI.h"
#include "Table_HorizAlignment.h"
#include "Table_VertAlignment.h"



void Database_pluto_main::CreateTable_StyleVariation()
{
	tblStyleVariation = new Table_StyleVariation(this);
}

void Database_pluto_main::DeleteTable_StyleVariation()
{
	if( tblStyleVariation )
		delete tblStyleVariation;
}

bool Database_pluto_main::Commit_StyleVariation(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblStyleVariation->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_StyleVariation::~Table_StyleVariation()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_StyleVariation *pRow = (Row_StyleVariation *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_StyleVariation *pRow = (Row_StyleVariation *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_StyleVariation::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_StyleVariation *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_StyleVariation *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_StyleVariation);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_StyleVariation::Reload()
{
	Row_StyleVariation *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_StyleVariation);
		Row_StyleVariation *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_StyleVariation::Row_StyleVariation(Table_StyleVariation *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_StyleVariation::SetDefaultValues()
{
	m_PK_StyleVariation = 0;
is_null[0] = false;
m_FK_Style = 0;
is_null[1] = false;
is_null[2] = true;
m_FK_Skin = 0;
is_null[3] = true;
m_FK_UI = 0;
m_Font = "";
is_null[4] = false;
is_null[5] = true;
m_ForeColor = 0;
m_PixelHeight = 0;
is_null[6] = false;
m_Bold = 0;
is_null[7] = false;
m_Italic = 0;
is_null[8] = false;
m_Underline = 0;
is_null[9] = false;
is_null[10] = true;
m_ShadowX = 0;
is_null[11] = true;
m_ShadowY = 0;
is_null[12] = true;
m_ShadowColor = 0;
m_BorderStyle = 0;
is_null[13] = false;
m_FK_HorizAlignment = 0;
is_null[14] = false;
m_FK_VertAlignment = 0;
is_null[15] = false;
is_null[16] = true;
m_BackColor = 0;
is_null[17] = true;
m_psc_id = 0;
is_null[18] = true;
m_psc_batch = 0;
is_null[19] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[20] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[21] = false;
is_null[22] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_StyleVariation::PK_StyleVariation_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_StyleVariation;}
long int Row_StyleVariation::FK_Style_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Style;}
long int Row_StyleVariation::FK_Skin_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Skin;}
long int Row_StyleVariation::FK_UI_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_UI;}
string Row_StyleVariation::Font_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Font;}
long int Row_StyleVariation::ForeColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ForeColor;}
long int Row_StyleVariation::PixelHeight_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PixelHeight;}
short int Row_StyleVariation::Bold_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Bold;}
short int Row_StyleVariation::Italic_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Italic;}
short int Row_StyleVariation::Underline_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Underline;}
long int Row_StyleVariation::ShadowX_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShadowX;}
long int Row_StyleVariation::ShadowY_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShadowY;}
long int Row_StyleVariation::ShadowColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ShadowColor;}
long int Row_StyleVariation::BorderStyle_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_BorderStyle;}
long int Row_StyleVariation::FK_HorizAlignment_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_HorizAlignment;}
long int Row_StyleVariation::FK_VertAlignment_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_VertAlignment;}
long int Row_StyleVariation::BackColor_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_BackColor;}
long int Row_StyleVariation::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_StyleVariation::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_StyleVariation::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_StyleVariation::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_StyleVariation::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_StyleVariation::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_StyleVariation::PK_StyleVariation_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_StyleVariation = val; is_modified=true; is_null[0]=false;}
void Row_StyleVariation::FK_Style_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Style = val; is_modified=true; is_null[1]=false;}
void Row_StyleVariation::FK_Skin_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Skin = val; is_modified=true; is_null[2]=false;}
void Row_StyleVariation::FK_UI_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_UI = val; is_modified=true; is_null[3]=false;}
void Row_StyleVariation::Font_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Font = val; is_modified=true; is_null[4]=false;}
void Row_StyleVariation::ForeColor_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ForeColor = val; is_modified=true; is_null[5]=false;}
void Row_StyleVariation::PixelHeight_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PixelHeight = val; is_modified=true; is_null[6]=false;}
void Row_StyleVariation::Bold_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Bold = val; is_modified=true; is_null[7]=false;}
void Row_StyleVariation::Italic_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Italic = val; is_modified=true; is_null[8]=false;}
void Row_StyleVariation::Underline_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Underline = val; is_modified=true; is_null[9]=false;}
void Row_StyleVariation::ShadowX_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShadowX = val; is_modified=true; is_null[10]=false;}
void Row_StyleVariation::ShadowY_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShadowY = val; is_modified=true; is_null[11]=false;}
void Row_StyleVariation::ShadowColor_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ShadowColor = val; is_modified=true; is_null[12]=false;}
void Row_StyleVariation::BorderStyle_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_BorderStyle = val; is_modified=true; is_null[13]=false;}
void Row_StyleVariation::FK_HorizAlignment_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_HorizAlignment = val; is_modified=true; is_null[14]=false;}
void Row_StyleVariation::FK_VertAlignment_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_VertAlignment = val; is_modified=true; is_null[15]=false;}
void Row_StyleVariation::BackColor_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_BackColor = val; is_modified=true; is_null[16]=false;}
void Row_StyleVariation::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[17]=false;}
void Row_StyleVariation::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[18]=false;}
void Row_StyleVariation::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[19]=false;}
void Row_StyleVariation::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[20]=false;}
void Row_StyleVariation::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[21]=false;}
void Row_StyleVariation::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[22]=false;}

		
bool Row_StyleVariation::FK_Skin_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_StyleVariation::FK_UI_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_StyleVariation::ForeColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_StyleVariation::ShadowX_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_StyleVariation::ShadowY_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_StyleVariation::ShadowColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_StyleVariation::BackColor_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_StyleVariation::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_StyleVariation::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_StyleVariation::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_StyleVariation::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_StyleVariation::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}

			
void Row_StyleVariation::FK_Skin_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_StyleVariation::FK_UI_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_StyleVariation::ForeColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_StyleVariation::ShadowX_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_StyleVariation::ShadowY_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_StyleVariation::ShadowColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_StyleVariation::BackColor_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_StyleVariation::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_StyleVariation::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_StyleVariation::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_StyleVariation::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_StyleVariation::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
	

string Row_StyleVariation::PK_StyleVariation_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_StyleVariation);

return buf;
}

string Row_StyleVariation::FK_Style_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Style);

return buf;
}

string Row_StyleVariation::FK_Skin_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Skin);

return buf;
}

string Row_StyleVariation::FK_UI_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_UI);

return buf;
}

string Row_StyleVariation::Font_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char *buf = new char[181];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Font.c_str(), (unsigned long) min((size_t)90,m_Font.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_StyleVariation::ForeColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ForeColor);

return buf;
}

string Row_StyleVariation::PixelHeight_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PixelHeight);

return buf;
}

string Row_StyleVariation::Bold_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Bold);

return buf;
}

string Row_StyleVariation::Italic_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Italic);

return buf;
}

string Row_StyleVariation::Underline_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Underline);

return buf;
}

string Row_StyleVariation::ShadowX_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ShadowX);

return buf;
}

string Row_StyleVariation::ShadowY_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ShadowY);

return buf;
}

string Row_StyleVariation::ShadowColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ShadowColor);

return buf;
}

string Row_StyleVariation::BorderStyle_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_BorderStyle);

return buf;
}

string Row_StyleVariation::FK_HorizAlignment_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_HorizAlignment);

return buf;
}

string Row_StyleVariation::FK_VertAlignment_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_VertAlignment);

return buf;
}

string Row_StyleVariation::BackColor_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_BackColor);

return buf;
}

string Row_StyleVariation::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_StyleVariation::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_StyleVariation::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_StyleVariation::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_StyleVariation::psc_mod_asSQL()
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

string Row_StyleVariation::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_StyleVariation::Key::Key(long int in_PK_StyleVariation)
{
			pk_PK_StyleVariation = in_PK_StyleVariation;
	
}

Table_StyleVariation::Key::Key(Row_StyleVariation *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_StyleVariation = pRow->m_PK_StyleVariation;
	
}		

bool Table_StyleVariation::Key_Less::operator()(const Table_StyleVariation::Key &key1, const Table_StyleVariation::Key &key2) const
{
			if (key1.pk_PK_StyleVariation!=key2.pk_PK_StyleVariation)
return key1.pk_PK_StyleVariation<key2.pk_PK_StyleVariation;
else
return false;	
}	

bool Table_StyleVariation::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_StyleVariation *pRow = (Row_StyleVariation *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_StyleVariation_asSQL()+", "+pRow->FK_Style_asSQL()+", "+pRow->FK_Skin_asSQL()+", "+pRow->FK_UI_asSQL()+", "+pRow->Font_asSQL()+", "+pRow->ForeColor_asSQL()+", "+pRow->PixelHeight_asSQL()+", "+pRow->Bold_asSQL()+", "+pRow->Italic_asSQL()+", "+pRow->Underline_asSQL()+", "+pRow->ShadowX_asSQL()+", "+pRow->ShadowY_asSQL()+", "+pRow->ShadowColor_asSQL()+", "+pRow->BorderStyle_asSQL()+", "+pRow->FK_HorizAlignment_asSQL()+", "+pRow->FK_VertAlignment_asSQL()+", "+pRow->BackColor_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into StyleVariation (`PK_StyleVariation`, `FK_Style`, `FK_Skin`, `FK_UI`, `Font`, `ForeColor`, `PixelHeight`, `Bold`, `Italic`, `Underline`, `ShadowX`, `ShadowY`, `ShadowColor`, `BorderStyle`, `FK_HorizAlignment`, `FK_VertAlignment`, `BackColor`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_StyleVariation=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_StyleVariation is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_StyleVariation);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_StyleVariation* pRow = (Row_StyleVariation*) (*i).second;	
		SingleLongKey key(pRow->m_PK_StyleVariation);

		char tmp_PK_StyleVariation[32];
sprintf(tmp_PK_StyleVariation, "%li", key.pk);


string condition;
condition = condition + "`PK_StyleVariation`=" + tmp_PK_StyleVariation;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_StyleVariation`="+pRow->PK_StyleVariation_asSQL()+", `FK_Style`="+pRow->FK_Style_asSQL()+", `FK_Skin`="+pRow->FK_Skin_asSQL()+", `FK_UI`="+pRow->FK_UI_asSQL()+", `Font`="+pRow->Font_asSQL()+", `ForeColor`="+pRow->ForeColor_asSQL()+", `PixelHeight`="+pRow->PixelHeight_asSQL()+", `Bold`="+pRow->Bold_asSQL()+", `Italic`="+pRow->Italic_asSQL()+", `Underline`="+pRow->Underline_asSQL()+", `ShadowX`="+pRow->ShadowX_asSQL()+", `ShadowY`="+pRow->ShadowY_asSQL()+", `ShadowColor`="+pRow->ShadowColor_asSQL()+", `BorderStyle`="+pRow->BorderStyle_asSQL()+", `FK_HorizAlignment`="+pRow->FK_HorizAlignment_asSQL()+", `FK_VertAlignment`="+pRow->FK_VertAlignment_asSQL()+", `BackColor`="+pRow->BackColor_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update StyleVariation set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_StyleVariation* pRow = (Row_StyleVariation*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_StyleVariation* pRow = (Row_StyleVariation*) (*i).second;	

		char tmp_PK_StyleVariation[32];
sprintf(tmp_PK_StyleVariation, "%li", key.pk);


string condition;
condition = condition + "`PK_StyleVariation`=" + tmp_PK_StyleVariation;

	
		string query = "delete from StyleVariation where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_StyleVariation*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_StyleVariation::GetRows(string where_statement,vector<class Row_StyleVariation*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `StyleVariation`.* from StyleVariation " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `StyleVariation`.* from StyleVariation where " + where_statement;
	else
		query = "select `StyleVariation`.* from StyleVariation";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_StyleVariation *pRow = new Row_StyleVariation(this);
		
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
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_StyleVariation);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_StyleVariation *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_StyleVariation* Table_StyleVariation::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_StyleVariation *pRow = new Row_StyleVariation(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_StyleVariation* Table_StyleVariation::GetRow(long int in_PK_StyleVariation)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_StyleVariation);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_StyleVariation*) (*i).second;
	//we have to fetch row
	Row_StyleVariation* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_StyleVariation* Table_StyleVariation::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_StyleVariation[32];
sprintf(tmp_PK_StyleVariation, "%li", key.pk);


string condition;
condition = condition + "`PK_StyleVariation`=" + tmp_PK_StyleVariation;


	string query = "select * from StyleVariation where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_StyleVariation::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_StyleVariation *pRow = new Row_StyleVariation(this);
		
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
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Style* Row_StyleVariation::FK_Style_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Style *pTable = table->database->Style_get();
return pTable->GetRow(m_FK_Style);
}
class Row_Skin* Row_StyleVariation::FK_Skin_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Skin *pTable = table->database->Skin_get();
return pTable->GetRow(m_FK_Skin);
}
class Row_UI* Row_StyleVariation::FK_UI_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_UI *pTable = table->database->UI_get();
return pTable->GetRow(m_FK_UI);
}
class Row_HorizAlignment* Row_StyleVariation::FK_HorizAlignment_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_HorizAlignment *pTable = table->database->HorizAlignment_get();
return pTable->GetRow(m_FK_HorizAlignment);
}
class Row_VertAlignment* Row_StyleVariation::FK_VertAlignment_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_VertAlignment *pTable = table->database->VertAlignment_get();
return pTable->GetRow(m_FK_VertAlignment);
}





