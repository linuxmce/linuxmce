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
#include "Table_Package_pschmask.h"
#include "Table_PackageType.h"
#include "Table_Package.h"
#include "Table_License.h"
#include "Table_Manufacturer.h"
#include "Table_Document.h"
#include "Table_Document.h"
#include "Table_Document.h"



void Database_pluto_main::CreateTable_Package_pschmask()
{
	tblPackage_pschmask = new Table_Package_pschmask(this);
}

void Database_pluto_main::DeleteTable_Package_pschmask()
{
	if( tblPackage_pschmask )
		delete tblPackage_pschmask;
}

bool Database_pluto_main::Commit_Package_pschmask(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblPackage_pschmask->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Package_pschmask::~Table_Package_pschmask()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Package_pschmask *pRow = (Row_Package_pschmask *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Package_pschmask *pRow = (Row_Package_pschmask *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Package_pschmask::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Package_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Package_pschmask *) *i != this); i++);
			
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

void Row_Package_pschmask::Reload()
{
	Row_Package_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_Package_pschmask *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Package_pschmask::Row_Package_pschmask(Table_Package_pschmask *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Package_pschmask::SetDefaultValues()
{
	is_null[0] = true;
m_PK_Package = 0;
is_null[1] = true;
m_Description = 0;
is_null[2] = true;
m_FK_PackageType = 0;
is_null[3] = true;
m_FK_Package_Sourcecode = 0;
is_null[4] = true;
m_IsSource = 0;
is_null[5] = true;
m_NonExecutable = 0;
is_null[6] = true;
m_HomePage = 0;
is_null[7] = true;
m_FK_License = 0;
is_null[8] = true;
m_FK_Manufacturer = 0;
is_null[9] = true;
m_HelpURL = 0;
is_null[10] = true;
m_HelpURL_UsersManual = 0;
is_null[11] = true;
m_HelpURL_ProgrammersGuide = 0;
is_null[12] = true;
m_Comments = 0;
m_psc_id = 0;
is_null[13] = false;
m_psc_batch = 0;
is_null[14] = false;
is_null[15] = true;
m_psc_user = 0;
is_null[16] = true;
m_psc_frozen = 0;
is_null[17] = true;
is_null[18] = true;
m_psc_restrict = 0;
is_null[19] = true;
m_FK_Document = 0;
is_null[20] = true;
m_FK_Document_ProgrammersGuide = 0;
is_null[21] = true;
m_FK_Document_UsersManual = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

short int Row_Package_pschmask::PK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Package;}
short int Row_Package_pschmask::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
short int Row_Package_pschmask::FK_PackageType_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PackageType;}
short int Row_Package_pschmask::FK_Package_Sourcecode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Package_Sourcecode;}
short int Row_Package_pschmask::IsSource_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsSource;}
short int Row_Package_pschmask::NonExecutable_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_NonExecutable;}
short int Row_Package_pschmask::HomePage_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HomePage;}
short int Row_Package_pschmask::FK_License_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_License;}
short int Row_Package_pschmask::FK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Manufacturer;}
short int Row_Package_pschmask::HelpURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HelpURL;}
short int Row_Package_pschmask::HelpURL_UsersManual_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HelpURL_UsersManual;}
short int Row_Package_pschmask::HelpURL_ProgrammersGuide_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HelpURL_ProgrammersGuide;}
short int Row_Package_pschmask::Comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Comments;}
long int Row_Package_pschmask::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Package_pschmask::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
short int Row_Package_pschmask::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Package_pschmask::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Package_pschmask::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Package_pschmask::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}
short int Row_Package_pschmask::FK_Document_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Document;}
short int Row_Package_pschmask::FK_Document_ProgrammersGuide_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Document_ProgrammersGuide;}
short int Row_Package_pschmask::FK_Document_UsersManual_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Document_UsersManual;}

		
void Row_Package_pschmask::PK_Package_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Package = val; is_modified=true; is_null[0]=false;}
void Row_Package_pschmask::Description_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_Package_pschmask::FK_PackageType_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PackageType = val; is_modified=true; is_null[2]=false;}
void Row_Package_pschmask::FK_Package_Sourcecode_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Package_Sourcecode = val; is_modified=true; is_null[3]=false;}
void Row_Package_pschmask::IsSource_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsSource = val; is_modified=true; is_null[4]=false;}
void Row_Package_pschmask::NonExecutable_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_NonExecutable = val; is_modified=true; is_null[5]=false;}
void Row_Package_pschmask::HomePage_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HomePage = val; is_modified=true; is_null[6]=false;}
void Row_Package_pschmask::FK_License_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_License = val; is_modified=true; is_null[7]=false;}
void Row_Package_pschmask::FK_Manufacturer_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Manufacturer = val; is_modified=true; is_null[8]=false;}
void Row_Package_pschmask::HelpURL_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HelpURL = val; is_modified=true; is_null[9]=false;}
void Row_Package_pschmask::HelpURL_UsersManual_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HelpURL_UsersManual = val; is_modified=true; is_null[10]=false;}
void Row_Package_pschmask::HelpURL_ProgrammersGuide_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HelpURL_ProgrammersGuide = val; is_modified=true; is_null[11]=false;}
void Row_Package_pschmask::Comments_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Comments = val; is_modified=true; is_null[12]=false;}
void Row_Package_pschmask::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[13]=false;}
void Row_Package_pschmask::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[14]=false;}
void Row_Package_pschmask::psc_user_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[15]=false;}
void Row_Package_pschmask::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[16]=false;}
void Row_Package_pschmask::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[17]=false;}
void Row_Package_pschmask::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[18]=false;}
void Row_Package_pschmask::FK_Document_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Document = val; is_modified=true; is_null[19]=false;}
void Row_Package_pschmask::FK_Document_ProgrammersGuide_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Document_ProgrammersGuide = val; is_modified=true; is_null[20]=false;}
void Row_Package_pschmask::FK_Document_UsersManual_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Document_UsersManual = val; is_modified=true; is_null[21]=false;}

		
bool Row_Package_pschmask::PK_Package_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_Package_pschmask::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_Package_pschmask::FK_PackageType_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Package_pschmask::FK_Package_Sourcecode_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Package_pschmask::IsSource_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Package_pschmask::NonExecutable_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Package_pschmask::HomePage_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Package_pschmask::FK_License_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Package_pschmask::FK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Package_pschmask::HelpURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Package_pschmask::HelpURL_UsersManual_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Package_pschmask::HelpURL_ProgrammersGuide_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Package_pschmask::Comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Package_pschmask::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Package_pschmask::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_Package_pschmask::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_Package_pschmask::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_Package_pschmask::FK_Document_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_Package_pschmask::FK_Document_ProgrammersGuide_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_Package_pschmask::FK_Document_UsersManual_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}

			
void Row_Package_pschmask::PK_Package_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_Package_pschmask::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_PackageType_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_Package_Sourcecode_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Package_pschmask::IsSource_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Package_pschmask::NonExecutable_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Package_pschmask::HomePage_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_License_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Package_pschmask::HelpURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Package_pschmask::HelpURL_UsersManual_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Package_pschmask::HelpURL_ProgrammersGuide_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Package_pschmask::Comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Package_pschmask::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Package_pschmask::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_Package_pschmask::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Package_pschmask::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_Document_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_Document_ProgrammersGuide_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Package_pschmask::FK_Document_UsersManual_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
	

string Row_Package_pschmask::PK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PK_Package);

return buf;
}

string Row_Package_pschmask::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Description);

return buf;
}

string Row_Package_pschmask::FK_PackageType_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_PackageType);

return buf;
}

string Row_Package_pschmask::FK_Package_Sourcecode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Package_Sourcecode);

return buf;
}

string Row_Package_pschmask::IsSource_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsSource);

return buf;
}

string Row_Package_pschmask::NonExecutable_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_NonExecutable);

return buf;
}

string Row_Package_pschmask::HomePage_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HomePage);

return buf;
}

string Row_Package_pschmask::FK_License_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_License);

return buf;
}

string Row_Package_pschmask::FK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Manufacturer);

return buf;
}

string Row_Package_pschmask::HelpURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HelpURL);

return buf;
}

string Row_Package_pschmask::HelpURL_UsersManual_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HelpURL_UsersManual);

return buf;
}

string Row_Package_pschmask::HelpURL_ProgrammersGuide_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HelpURL_ProgrammersGuide);

return buf;
}

string Row_Package_pschmask::Comments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Comments);

return buf;
}

string Row_Package_pschmask::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Package_pschmask::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Package_pschmask::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_user);

return buf;
}

string Row_Package_pschmask::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Package_pschmask::psc_mod_asSQL()
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

string Row_Package_pschmask::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}

string Row_Package_pschmask::FK_Document_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Document);

return buf;
}

string Row_Package_pschmask::FK_Document_ProgrammersGuide_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Document_ProgrammersGuide);

return buf;
}

string Row_Package_pschmask::FK_Document_UsersManual_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Document_UsersManual);

return buf;
}




Table_Package_pschmask::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_Package_pschmask::Key::Key(Row_Package_pschmask *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_Package_pschmask::Key_Less::operator()(const Table_Package_pschmask::Key &key1, const Table_Package_pschmask::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_Package_pschmask::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Package_pschmask *pRow = (Row_Package_pschmask *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Package_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->FK_PackageType_asSQL()+", "+pRow->FK_Package_Sourcecode_asSQL()+", "+pRow->IsSource_asSQL()+", "+pRow->NonExecutable_asSQL()+", "+pRow->HomePage_asSQL()+", "+pRow->FK_License_asSQL()+", "+pRow->FK_Manufacturer_asSQL()+", "+pRow->HelpURL_asSQL()+", "+pRow->HelpURL_UsersManual_asSQL()+", "+pRow->HelpURL_ProgrammersGuide_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL()+", "+pRow->FK_Document_asSQL()+", "+pRow->FK_Document_ProgrammersGuide_asSQL()+", "+pRow->FK_Document_UsersManual_asSQL();

	
		string query = "insert into Package_pschmask (`PK_Package`, `Description`, `FK_PackageType`, `FK_Package_Sourcecode`, `IsSource`, `NonExecutable`, `HomePage`, `FK_License`, `FK_Manufacturer`, `HelpURL`, `HelpURL_UsersManual`, `HelpURL_ProgrammersGuide`, `Comments`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`, `FK_Document`, `FK_Document_ProgrammersGuide`, `FK_Document_UsersManual`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Package_pschmask* pRow = (Row_Package_pschmask*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Package`="+pRow->PK_Package_asSQL()+", `Description`="+pRow->Description_asSQL()+", `FK_PackageType`="+pRow->FK_PackageType_asSQL()+", `FK_Package_Sourcecode`="+pRow->FK_Package_Sourcecode_asSQL()+", `IsSource`="+pRow->IsSource_asSQL()+", `NonExecutable`="+pRow->NonExecutable_asSQL()+", `HomePage`="+pRow->HomePage_asSQL()+", `FK_License`="+pRow->FK_License_asSQL()+", `FK_Manufacturer`="+pRow->FK_Manufacturer_asSQL()+", `HelpURL`="+pRow->HelpURL_asSQL()+", `HelpURL_UsersManual`="+pRow->HelpURL_UsersManual_asSQL()+", `HelpURL_ProgrammersGuide`="+pRow->HelpURL_ProgrammersGuide_asSQL()+", `Comments`="+pRow->Comments_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL()+", `FK_Document`="+pRow->FK_Document_asSQL()+", `FK_Document_ProgrammersGuide`="+pRow->FK_Document_ProgrammersGuide_asSQL()+", `FK_Document_UsersManual`="+pRow->FK_Document_UsersManual_asSQL();

	
		string query = "update Package_pschmask set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Package_pschmask* pRow = (Row_Package_pschmask*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Package_pschmask* pRow = (Row_Package_pschmask*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from Package_pschmask where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Package_pschmask*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Package_pschmask::GetRows(string where_statement,vector<class Row_Package_pschmask*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Package_pschmask`.* from Package_pschmask " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Package_pschmask`.* from Package_pschmask where " + where_statement;
	else
		query = "select `Package_pschmask`.* from Package_pschmask";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Package_pschmask *pRow = new Row_Package_pschmask(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_Description));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_PackageType = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_FK_PackageType));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Package_Sourcecode = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_FK_Package_Sourcecode));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsSource = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsSource));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_NonExecutable = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_NonExecutable));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_HomePage = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_HomePage));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_License = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FK_License));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_FK_Manufacturer));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_HelpURL = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_HelpURL));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_HelpURL_UsersManual = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_HelpURL_UsersManual));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_HelpURL_ProgrammersGuide = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_HelpURL_ProgrammersGuide));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Comments = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_Comments));
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
sscanf(row[15], "%hi", &(pRow->m_psc_user));
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

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_FK_Document));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_FK_Document_ProgrammersGuide = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_FK_Document_ProgrammersGuide));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_FK_Document_UsersManual = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%hi", &(pRow->m_FK_Document_UsersManual));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Package_pschmask *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Package_pschmask* Table_Package_pschmask::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Package_pschmask *pRow = new Row_Package_pschmask(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Package_pschmask* Table_Package_pschmask::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_Package_pschmask*) (*i).second;
	//we have to fetch row
	Row_Package_pschmask* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Package_pschmask* Table_Package_pschmask::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from Package_pschmask where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Package_pschmask::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Package_pschmask *pRow = new Row_Package_pschmask(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Package = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_Package));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_Description = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_Description));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_FK_PackageType = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_FK_PackageType));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_Package_Sourcecode = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_FK_Package_Sourcecode));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_IsSource = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_IsSource));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_NonExecutable = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_NonExecutable));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_HomePage = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_HomePage));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FK_License = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FK_License));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_FK_Manufacturer));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_HelpURL = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_HelpURL));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_HelpURL_UsersManual = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_HelpURL_UsersManual));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_HelpURL_ProgrammersGuide = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_HelpURL_ProgrammersGuide));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Comments = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_Comments));
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
sscanf(row[15], "%hi", &(pRow->m_psc_user));
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

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_FK_Document));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_FK_Document_ProgrammersGuide = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_FK_Document_ProgrammersGuide));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_FK_Document_UsersManual = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%hi", &(pRow->m_FK_Document_UsersManual));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_PackageType* Row_Package_pschmask::FK_PackageType_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PackageType *pTable = table->database->PackageType_get();
return pTable->GetRow(m_FK_PackageType);
}
class Row_Package* Row_Package_pschmask::FK_Package_Sourcecode_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package_Sourcecode);
}
class Row_License* Row_Package_pschmask::FK_License_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_License *pTable = table->database->License_get();
return pTable->GetRow(m_FK_License);
}
class Row_Manufacturer* Row_Package_pschmask::FK_Manufacturer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Manufacturer *pTable = table->database->Manufacturer_get();
return pTable->GetRow(m_FK_Manufacturer);
}
class Row_Document* Row_Package_pschmask::FK_Document_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Document *pTable = table->database->Document_get();
return pTable->GetRow(m_FK_Document);
}
class Row_Document* Row_Package_pschmask::FK_Document_ProgrammersGuide_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Document *pTable = table->database->Document_get();
return pTable->GetRow(m_FK_Document_ProgrammersGuide);
}
class Row_Document* Row_Package_pschmask::FK_Document_UsersManual_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Document *pTable = table->database->Document_get();
return pTable->GetRow(m_FK_Document_UsersManual);
}





