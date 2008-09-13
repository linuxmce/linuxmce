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
#include "Table_Users_pschmask.h"
#include "Table_Language.h"
#include "Table_Installation.h"
#include "Table_UserMode.h"



void Database_pluto_main::CreateTable_Users_pschmask()
{
	tblUsers_pschmask = new Table_Users_pschmask(this);
}

void Database_pluto_main::DeleteTable_Users_pschmask()
{
	if( tblUsers_pschmask )
		delete tblUsers_pschmask;
}

bool Database_pluto_main::Commit_Users_pschmask(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblUsers_pschmask->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Users_pschmask::~Table_Users_pschmask()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Users_pschmask *pRow = (Row_Users_pschmask *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Users_pschmask *pRow = (Row_Users_pschmask *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Users_pschmask::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Users_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Users_pschmask *) *i != this); i++);
			
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

void Row_Users_pschmask::Reload()
{
	Row_Users_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_Users_pschmask *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Users_pschmask::Row_Users_pschmask(Table_Users_pschmask *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Users_pschmask::SetDefaultValues()
{
	is_null[0] = true;
m_PK_Users = 0;
is_null[1] = true;
m_UserName = 0;
is_null[2] = true;
m_Password = 0;
is_null[3] = true;
m_PINCode = 0;
is_null[4] = true;
m_HasMailbox = 0;
is_null[5] = true;
m_AccessGeneralMailbox = 0;
is_null[6] = true;
m_Extension = 0;
is_null[7] = true;
m_FirstName = 0;
is_null[8] = true;
m_LastName = 0;
is_null[9] = true;
m_Nickname = 0;
is_null[10] = true;
m_ExtensionRingTimeout = 0;
is_null[11] = true;
m_ForwardEmail = 0;
is_null[12] = true;
m_FK_Language = 0;
is_null[13] = true;
m_FK_Installation_Main = 0;
is_null[14] = true;
m_HideFromOrbiter = 0;
is_null[15] = true;
m_Staff = 0;
is_null[16] = true;
m_Password_Unix = 0;
is_null[17] = true;
m_Password_Samba = 0;
is_null[18] = true;
m_FK_UserMode = 0;
is_null[19] = true;
m_EK_Dealer = 0;
is_null[20] = true;
m_EK_Manufacturer = 0;
is_null[21] = true;
m_RequirePinToSelect = 0;
m_psc_id = 0;
is_null[22] = false;
m_psc_batch = 0;
is_null[23] = false;
is_null[24] = true;
m_psc_user = 0;
is_null[25] = true;
m_psc_frozen = 0;
is_null[26] = true;
is_null[27] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

short int Row_Users_pschmask::PK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Users;}
short int Row_Users_pschmask::UserName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UserName;}
short int Row_Users_pschmask::Password_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password;}
short int Row_Users_pschmask::PINCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PINCode;}
short int Row_Users_pschmask::HasMailbox_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HasMailbox;}
short int Row_Users_pschmask::AccessGeneralMailbox_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AccessGeneralMailbox;}
short int Row_Users_pschmask::Extension_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Extension;}
short int Row_Users_pschmask::FirstName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FirstName;}
short int Row_Users_pschmask::LastName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_LastName;}
short int Row_Users_pschmask::Nickname_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Nickname;}
short int Row_Users_pschmask::ExtensionRingTimeout_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ExtensionRingTimeout;}
short int Row_Users_pschmask::ForwardEmail_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ForwardEmail;}
short int Row_Users_pschmask::FK_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Language;}
short int Row_Users_pschmask::FK_Installation_Main_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Installation_Main;}
short int Row_Users_pschmask::HideFromOrbiter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HideFromOrbiter;}
short int Row_Users_pschmask::Staff_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Staff;}
short int Row_Users_pschmask::Password_Unix_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password_Unix;}
short int Row_Users_pschmask::Password_Samba_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password_Samba;}
short int Row_Users_pschmask::FK_UserMode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_UserMode;}
short int Row_Users_pschmask::EK_Dealer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Dealer;}
short int Row_Users_pschmask::EK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Manufacturer;}
short int Row_Users_pschmask::RequirePinToSelect_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RequirePinToSelect;}
long int Row_Users_pschmask::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Users_pschmask::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
short int Row_Users_pschmask::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Users_pschmask::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Users_pschmask::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Users_pschmask::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Users_pschmask::PK_Users_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Users = val; is_modified=true; is_null[0]=false;}
void Row_Users_pschmask::UserName_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UserName = val; is_modified=true; is_null[1]=false;}
void Row_Users_pschmask::Password_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password = val; is_modified=true; is_null[2]=false;}
void Row_Users_pschmask::PINCode_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PINCode = val; is_modified=true; is_null[3]=false;}
void Row_Users_pschmask::HasMailbox_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HasMailbox = val; is_modified=true; is_null[4]=false;}
void Row_Users_pschmask::AccessGeneralMailbox_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AccessGeneralMailbox = val; is_modified=true; is_null[5]=false;}
void Row_Users_pschmask::Extension_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Extension = val; is_modified=true; is_null[6]=false;}
void Row_Users_pschmask::FirstName_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FirstName = val; is_modified=true; is_null[7]=false;}
void Row_Users_pschmask::LastName_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_LastName = val; is_modified=true; is_null[8]=false;}
void Row_Users_pschmask::Nickname_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Nickname = val; is_modified=true; is_null[9]=false;}
void Row_Users_pschmask::ExtensionRingTimeout_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ExtensionRingTimeout = val; is_modified=true; is_null[10]=false;}
void Row_Users_pschmask::ForwardEmail_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ForwardEmail = val; is_modified=true; is_null[11]=false;}
void Row_Users_pschmask::FK_Language_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Language = val; is_modified=true; is_null[12]=false;}
void Row_Users_pschmask::FK_Installation_Main_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Installation_Main = val; is_modified=true; is_null[13]=false;}
void Row_Users_pschmask::HideFromOrbiter_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HideFromOrbiter = val; is_modified=true; is_null[14]=false;}
void Row_Users_pschmask::Staff_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Staff = val; is_modified=true; is_null[15]=false;}
void Row_Users_pschmask::Password_Unix_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password_Unix = val; is_modified=true; is_null[16]=false;}
void Row_Users_pschmask::Password_Samba_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password_Samba = val; is_modified=true; is_null[17]=false;}
void Row_Users_pschmask::FK_UserMode_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_UserMode = val; is_modified=true; is_null[18]=false;}
void Row_Users_pschmask::EK_Dealer_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Dealer = val; is_modified=true; is_null[19]=false;}
void Row_Users_pschmask::EK_Manufacturer_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Manufacturer = val; is_modified=true; is_null[20]=false;}
void Row_Users_pschmask::RequirePinToSelect_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RequirePinToSelect = val; is_modified=true; is_null[21]=false;}
void Row_Users_pschmask::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[22]=false;}
void Row_Users_pschmask::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[23]=false;}
void Row_Users_pschmask::psc_user_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[24]=false;}
void Row_Users_pschmask::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[25]=false;}
void Row_Users_pschmask::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[26]=false;}
void Row_Users_pschmask::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[27]=false;}

		
bool Row_Users_pschmask::PK_Users_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_Users_pschmask::UserName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_Users_pschmask::Password_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_Users_pschmask::PINCode_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_Users_pschmask::HasMailbox_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_Users_pschmask::AccessGeneralMailbox_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_Users_pschmask::Extension_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Users_pschmask::FirstName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Users_pschmask::LastName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Users_pschmask::Nickname_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Users_pschmask::ExtensionRingTimeout_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_Users_pschmask::ForwardEmail_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Users_pschmask::FK_Language_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Users_pschmask::FK_Installation_Main_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_Users_pschmask::HideFromOrbiter_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_Users_pschmask::Staff_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Users_pschmask::Password_Unix_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_Users_pschmask::Password_Samba_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_Users_pschmask::FK_UserMode_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_Users_pschmask::EK_Dealer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_Users_pschmask::EK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_Users_pschmask::RequirePinToSelect_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_Users_pschmask::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_Users_pschmask::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}
bool Row_Users_pschmask::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}
bool Row_Users_pschmask::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[27];}

			
void Row_Users_pschmask::PK_Users_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_Users_pschmask::UserName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_Users_pschmask::Password_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_Users_pschmask::PINCode_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_Users_pschmask::HasMailbox_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_Users_pschmask::AccessGeneralMailbox_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_Users_pschmask::Extension_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Users_pschmask::FirstName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Users_pschmask::LastName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Users_pschmask::Nickname_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Users_pschmask::ExtensionRingTimeout_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_Users_pschmask::ForwardEmail_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Users_pschmask::FK_Language_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Users_pschmask::FK_Installation_Main_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Users_pschmask::HideFromOrbiter_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_Users_pschmask::Staff_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Users_pschmask::Password_Unix_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_Users_pschmask::Password_Samba_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Users_pschmask::FK_UserMode_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_Users_pschmask::EK_Dealer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Users_pschmask::EK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Users_pschmask::RequirePinToSelect_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_Users_pschmask::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_Users_pschmask::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
void Row_Users_pschmask::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
void Row_Users_pschmask::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[27]=val;
is_modified=true;
}
	

string Row_Users_pschmask::PK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PK_Users);

return buf;
}

string Row_Users_pschmask::UserName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_UserName);

return buf;
}

string Row_Users_pschmask::Password_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Password);

return buf;
}

string Row_Users_pschmask::PINCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PINCode);

return buf;
}

string Row_Users_pschmask::HasMailbox_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HasMailbox);

return buf;
}

string Row_Users_pschmask::AccessGeneralMailbox_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AccessGeneralMailbox);

return buf;
}

string Row_Users_pschmask::Extension_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Extension);

return buf;
}

string Row_Users_pschmask::FirstName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FirstName);

return buf;
}

string Row_Users_pschmask::LastName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_LastName);

return buf;
}

string Row_Users_pschmask::Nickname_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Nickname);

return buf;
}

string Row_Users_pschmask::ExtensionRingTimeout_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ExtensionRingTimeout);

return buf;
}

string Row_Users_pschmask::ForwardEmail_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ForwardEmail);

return buf;
}

string Row_Users_pschmask::FK_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Language);

return buf;
}

string Row_Users_pschmask::FK_Installation_Main_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Installation_Main);

return buf;
}

string Row_Users_pschmask::HideFromOrbiter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HideFromOrbiter);

return buf;
}

string Row_Users_pschmask::Staff_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Staff);

return buf;
}

string Row_Users_pschmask::Password_Unix_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Password_Unix);

return buf;
}

string Row_Users_pschmask::Password_Samba_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Password_Samba);

return buf;
}

string Row_Users_pschmask::FK_UserMode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_UserMode);

return buf;
}

string Row_Users_pschmask::EK_Dealer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_EK_Dealer);

return buf;
}

string Row_Users_pschmask::EK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_EK_Manufacturer);

return buf;
}

string Row_Users_pschmask::RequirePinToSelect_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RequirePinToSelect);

return buf;
}

string Row_Users_pschmask::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Users_pschmask::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Users_pschmask::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_user);

return buf;
}

string Row_Users_pschmask::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Users_pschmask::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users_pschmask::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[27])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Users_pschmask::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_Users_pschmask::Key::Key(Row_Users_pschmask *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_Users_pschmask::Key_Less::operator()(const Table_Users_pschmask::Key &key1, const Table_Users_pschmask::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_Users_pschmask::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Users_pschmask *pRow = (Row_Users_pschmask *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Users_asSQL()+", "+pRow->UserName_asSQL()+", "+pRow->Password_asSQL()+", "+pRow->PINCode_asSQL()+", "+pRow->HasMailbox_asSQL()+", "+pRow->AccessGeneralMailbox_asSQL()+", "+pRow->Extension_asSQL()+", "+pRow->FirstName_asSQL()+", "+pRow->LastName_asSQL()+", "+pRow->Nickname_asSQL()+", "+pRow->ExtensionRingTimeout_asSQL()+", "+pRow->ForwardEmail_asSQL()+", "+pRow->FK_Language_asSQL()+", "+pRow->FK_Installation_Main_asSQL()+", "+pRow->HideFromOrbiter_asSQL()+", "+pRow->Staff_asSQL()+", "+pRow->Password_Unix_asSQL()+", "+pRow->Password_Samba_asSQL()+", "+pRow->FK_UserMode_asSQL()+", "+pRow->EK_Dealer_asSQL()+", "+pRow->EK_Manufacturer_asSQL()+", "+pRow->RequirePinToSelect_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Users_pschmask (`PK_Users`, `UserName`, `Password`, `PINCode`, `HasMailbox`, `AccessGeneralMailbox`, `Extension`, `FirstName`, `LastName`, `Nickname`, `ExtensionRingTimeout`, `ForwardEmail`, `FK_Language`, `FK_Installation_Main`, `HideFromOrbiter`, `Staff`, `Password_Unix`, `Password_Samba`, `FK_UserMode`, `EK_Dealer`, `EK_Manufacturer`, `RequirePinToSelect`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Users_pschmask* pRow = (Row_Users_pschmask*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Users`="+pRow->PK_Users_asSQL()+", `UserName`="+pRow->UserName_asSQL()+", `Password`="+pRow->Password_asSQL()+", `PINCode`="+pRow->PINCode_asSQL()+", `HasMailbox`="+pRow->HasMailbox_asSQL()+", `AccessGeneralMailbox`="+pRow->AccessGeneralMailbox_asSQL()+", `Extension`="+pRow->Extension_asSQL()+", `FirstName`="+pRow->FirstName_asSQL()+", `LastName`="+pRow->LastName_asSQL()+", `Nickname`="+pRow->Nickname_asSQL()+", `ExtensionRingTimeout`="+pRow->ExtensionRingTimeout_asSQL()+", `ForwardEmail`="+pRow->ForwardEmail_asSQL()+", `FK_Language`="+pRow->FK_Language_asSQL()+", `FK_Installation_Main`="+pRow->FK_Installation_Main_asSQL()+", `HideFromOrbiter`="+pRow->HideFromOrbiter_asSQL()+", `Staff`="+pRow->Staff_asSQL()+", `Password_Unix`="+pRow->Password_Unix_asSQL()+", `Password_Samba`="+pRow->Password_Samba_asSQL()+", `FK_UserMode`="+pRow->FK_UserMode_asSQL()+", `EK_Dealer`="+pRow->EK_Dealer_asSQL()+", `EK_Manufacturer`="+pRow->EK_Manufacturer_asSQL()+", `RequirePinToSelect`="+pRow->RequirePinToSelect_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Users_pschmask set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Users_pschmask* pRow = (Row_Users_pschmask*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_Users_pschmask* pRow = (Row_Users_pschmask*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from Users_pschmask where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Users_pschmask*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Users_pschmask::GetRows(string where_statement,vector<class Row_Users_pschmask*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Users_pschmask`.* from Users_pschmask " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Users_pschmask`.* from Users_pschmask where " + where_statement;
	else
		query = "select `Users_pschmask`.* from Users_pschmask";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Users_pschmask *pRow = new Row_Users_pschmask(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Users = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_Users));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_UserName = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_UserName));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Password = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Password));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_PINCode = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_PINCode));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_HasMailbox = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_HasMailbox));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_AccessGeneralMailbox = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_AccessGeneralMailbox));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Extension = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Extension));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FirstName = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FirstName));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_LastName = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_LastName));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Nickname = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Nickname));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ExtensionRingTimeout = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_ExtensionRingTimeout));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_ForwardEmail = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_ForwardEmail));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_FK_Language));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_Installation_Main = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_FK_Installation_Main));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_HideFromOrbiter = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_HideFromOrbiter));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Staff = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_Staff));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Password_Unix = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_Password_Unix));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Password_Samba = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_Password_Samba));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_UserMode = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_FK_UserMode));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_EK_Dealer = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_EK_Dealer));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_EK_Manufacturer = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_EK_Manufacturer));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_RequirePinToSelect = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%hi", &(pRow->m_RequirePinToSelect));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_id));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_batch));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%hi", &(pRow->m_psc_user));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%hi", &(pRow->m_psc_frozen));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[26]=false;
pRow->m_psc_mod = string(row[26],lengths[26]);
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Users_pschmask *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Users_pschmask* Table_Users_pschmask::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Users_pschmask *pRow = new Row_Users_pschmask(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Users_pschmask* Table_Users_pschmask::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_Users_pschmask*) (*i).second;
	//we have to fetch row
	Row_Users_pschmask* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Users_pschmask* Table_Users_pschmask::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from Users_pschmask where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users_pschmask::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Users_pschmask *pRow = new Row_Users_pschmask(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Users = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_Users));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_UserName = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_UserName));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Password = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Password));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_PINCode = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_PINCode));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_HasMailbox = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_HasMailbox));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_AccessGeneralMailbox = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_AccessGeneralMailbox));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Extension = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Extension));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FirstName = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_FirstName));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_LastName = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_LastName));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Nickname = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_Nickname));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ExtensionRingTimeout = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_ExtensionRingTimeout));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_ForwardEmail = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_ForwardEmail));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_FK_Language = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_FK_Language));
}

if (row[13] == NULL)
{
pRow->is_null[13]=true;
pRow->m_FK_Installation_Main = 0;
}
else
{
pRow->is_null[13]=false;
sscanf(row[13], "%hi", &(pRow->m_FK_Installation_Main));
}

if (row[14] == NULL)
{
pRow->is_null[14]=true;
pRow->m_HideFromOrbiter = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%hi", &(pRow->m_HideFromOrbiter));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_Staff = 0;
}
else
{
pRow->is_null[15]=false;
sscanf(row[15], "%hi", &(pRow->m_Staff));
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_Password_Unix = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%hi", &(pRow->m_Password_Unix));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Password_Samba = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%hi", &(pRow->m_Password_Samba));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_UserMode = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%hi", &(pRow->m_FK_UserMode));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_EK_Dealer = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_EK_Dealer));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_EK_Manufacturer = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%hi", &(pRow->m_EK_Manufacturer));
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_RequirePinToSelect = 0;
}
else
{
pRow->is_null[21]=false;
sscanf(row[21], "%hi", &(pRow->m_RequirePinToSelect));
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_psc_id));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[23]=false;
sscanf(row[23], "%li", &(pRow->m_psc_batch));
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%hi", &(pRow->m_psc_user));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%hi", &(pRow->m_psc_frozen));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[26]=false;
pRow->m_psc_mod = string(row[26],lengths[26]);
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Language* Row_Users_pschmask::FK_Language_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language);
}
class Row_Installation* Row_Users_pschmask::FK_Installation_Main_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation_Main);
}
class Row_UserMode* Row_Users_pschmask::FK_UserMode_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_UserMode *pTable = table->database->UserMode_get();
return pTable->GetRow(m_FK_UserMode);
}





