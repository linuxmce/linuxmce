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
#include "Table_Users.h"
#include "Table_Language.h"
#include "Table_Installation.h"
#include "Table_UserMode.h"

#include "Table_DeviceTemplate.h"
#include "Table_Device_Users.h"
#include "Table_Document_Comment.h"
#include "Table_Installation_Users.h"
#include "Table_Orbiter_Users_PasswordReq.h"
#include "Table_Package_Users.h"
#include "Table_Room_Users.h"


void Database_pluto_main::CreateTable_Users()
{
	tblUsers = new Table_Users(this);
}

void Database_pluto_main::DeleteTable_Users()
{
	if( tblUsers )
		delete tblUsers;
}

bool Database_pluto_main::Commit_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblUsers->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_Users::~Table_Users()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_Users *pRow = (Row_Users *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_Users *pRow = (Row_Users *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_Users::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_Users *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_Users *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_Users);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_Users::Reload()
{
	Row_Users *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_Users);
		Row_Users *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_Users::Row_Users(Table_Users *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_Users::SetDefaultValues()
{
	m_PK_Users = 0;
is_null[0] = false;
m_UserName = "";
is_null[1] = false;
m_Password = "";
is_null[2] = false;
m_PINCode = "0";
is_null[3] = false;
m_HasMailbox = 0;
is_null[4] = false;
m_AccessGeneralMailbox = 0;
is_null[5] = false;
is_null[6] = true;
m_Extension = 0;
is_null[7] = true;
is_null[8] = true;
is_null[9] = true;
m_ExtensionRingTimeout = 0;
is_null[10] = false;
is_null[11] = true;
is_null[12] = true;
m_FK_Language = 0;
is_null[13] = true;
m_FK_Installation_Main = 0;
m_HideFromOrbiter = 0;
is_null[14] = false;
is_null[15] = true;
m_Staff = 0;
is_null[16] = true;
is_null[17] = true;
m_FK_UserMode = 1;
is_null[18] = false;
is_null[19] = true;
m_EK_Dealer = 0;
is_null[20] = true;
m_EK_Manufacturer = 0;
m_RequirePinToSelect = 0;
is_null[21] = false;
m_Use_VPN = 0;
is_null[22] = false;
m_VPN_Ip = "0";
is_null[23] = false;
m_Connect_to = "";
is_null[24] = false;
is_null[25] = true;
m_psc_id = 0;
is_null[26] = true;
m_psc_batch = 0;
is_null[27] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[28] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[29] = false;
is_null[30] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_Users::PK_Users_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_Users;}
string Row_Users::UserName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_UserName;}
string Row_Users::Password_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password;}
string Row_Users::PINCode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PINCode;}
short int Row_Users::HasMailbox_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HasMailbox;}
short int Row_Users::AccessGeneralMailbox_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_AccessGeneralMailbox;}
long int Row_Users::Extension_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Extension;}
string Row_Users::FirstName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FirstName;}
string Row_Users::LastName_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_LastName;}
string Row_Users::Nickname_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Nickname;}
long int Row_Users::ExtensionRingTimeout_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ExtensionRingTimeout;}
string Row_Users::ForwardEmail_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ForwardEmail;}
short int Row_Users::FK_Language_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Language;}
long int Row_Users::FK_Installation_Main_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Installation_Main;}
short int Row_Users::HideFromOrbiter_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HideFromOrbiter;}
short int Row_Users::Staff_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Staff;}
string Row_Users::Password_Unix_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password_Unix;}
string Row_Users::Password_Samba_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Password_Samba;}
long int Row_Users::FK_UserMode_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_UserMode;}
long int Row_Users::EK_Dealer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Dealer;}
long int Row_Users::EK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_EK_Manufacturer;}
short int Row_Users::RequirePinToSelect_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_RequirePinToSelect;}
long int Row_Users::Use_VPN_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Use_VPN;}
string Row_Users::VPN_Ip_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VPN_Ip;}
string Row_Users::Connect_to_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Connect_to;}
long int Row_Users::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_Users::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_Users::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_Users::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_Users::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_Users::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_Users::PK_Users_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_Users = val; is_modified=true; is_null[0]=false;}
void Row_Users::UserName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_UserName = val; is_modified=true; is_null[1]=false;}
void Row_Users::Password_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password = val; is_modified=true; is_null[2]=false;}
void Row_Users::PINCode_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PINCode = val; is_modified=true; is_null[3]=false;}
void Row_Users::HasMailbox_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HasMailbox = val; is_modified=true; is_null[4]=false;}
void Row_Users::AccessGeneralMailbox_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_AccessGeneralMailbox = val; is_modified=true; is_null[5]=false;}
void Row_Users::Extension_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Extension = val; is_modified=true; is_null[6]=false;}
void Row_Users::FirstName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FirstName = val; is_modified=true; is_null[7]=false;}
void Row_Users::LastName_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_LastName = val; is_modified=true; is_null[8]=false;}
void Row_Users::Nickname_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Nickname = val; is_modified=true; is_null[9]=false;}
void Row_Users::ExtensionRingTimeout_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ExtensionRingTimeout = val; is_modified=true; is_null[10]=false;}
void Row_Users::ForwardEmail_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ForwardEmail = val; is_modified=true; is_null[11]=false;}
void Row_Users::FK_Language_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Language = val; is_modified=true; is_null[12]=false;}
void Row_Users::FK_Installation_Main_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Installation_Main = val; is_modified=true; is_null[13]=false;}
void Row_Users::HideFromOrbiter_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HideFromOrbiter = val; is_modified=true; is_null[14]=false;}
void Row_Users::Staff_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Staff = val; is_modified=true; is_null[15]=false;}
void Row_Users::Password_Unix_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password_Unix = val; is_modified=true; is_null[16]=false;}
void Row_Users::Password_Samba_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Password_Samba = val; is_modified=true; is_null[17]=false;}
void Row_Users::FK_UserMode_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_UserMode = val; is_modified=true; is_null[18]=false;}
void Row_Users::EK_Dealer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Dealer = val; is_modified=true; is_null[19]=false;}
void Row_Users::EK_Manufacturer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_EK_Manufacturer = val; is_modified=true; is_null[20]=false;}
void Row_Users::RequirePinToSelect_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_RequirePinToSelect = val; is_modified=true; is_null[21]=false;}
void Row_Users::Use_VPN_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Use_VPN = val; is_modified=true; is_null[22]=false;}
void Row_Users::VPN_Ip_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VPN_Ip = val; is_modified=true; is_null[23]=false;}
void Row_Users::Connect_to_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Connect_to = val; is_modified=true; is_null[24]=false;}
void Row_Users::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[25]=false;}
void Row_Users::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[26]=false;}
void Row_Users::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[27]=false;}
void Row_Users::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[28]=false;}
void Row_Users::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[29]=false;}
void Row_Users::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[30]=false;}

		
bool Row_Users::Extension_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_Users::FirstName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_Users::LastName_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_Users::Nickname_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_Users::ForwardEmail_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_Users::FK_Language_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_Users::FK_Installation_Main_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_Users::Staff_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_Users::Password_Unix_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_Users::Password_Samba_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_Users::EK_Dealer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_Users::EK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_Users::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}
bool Row_Users::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}
bool Row_Users::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[27];}
bool Row_Users::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[28];}
bool Row_Users::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[30];}

			
void Row_Users::Extension_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_Users::FirstName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_Users::LastName_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_Users::Nickname_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_Users::ForwardEmail_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_Users::FK_Language_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_Users::FK_Installation_Main_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_Users::Staff_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_Users::Password_Unix_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_Users::Password_Samba_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_Users::EK_Dealer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_Users::EK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_Users::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
void Row_Users::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
void Row_Users::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[27]=val;
is_modified=true;
}
void Row_Users::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[28]=val;
is_modified=true;
}
void Row_Users::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[30]=val;
is_modified=true;
}
	

string Row_Users::PK_Users_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_Users);

return buf;
}

string Row_Users::UserName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[151];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_UserName.c_str(), (unsigned long) min((size_t)75,m_UserName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::Password_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[193];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Password.c_str(), (unsigned long) min((size_t)96,m_Password.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::PINCode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char *buf = new char[193];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_PINCode.c_str(), (unsigned long) min((size_t)96,m_PINCode.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::HasMailbox_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HasMailbox);

return buf;
}

string Row_Users::AccessGeneralMailbox_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_AccessGeneralMailbox);

return buf;
}

string Row_Users::Extension_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Extension);

return buf;
}

string Row_Users::FirstName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char *buf = new char[121];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_FirstName.c_str(), (unsigned long) min((size_t)60,m_FirstName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::LastName_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[181];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_LastName.c_str(), (unsigned long) min((size_t)90,m_LastName.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::Nickname_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char *buf = new char[91];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Nickname.c_str(), (unsigned long) min((size_t)45,m_Nickname.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::ExtensionRingTimeout_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_ExtensionRingTimeout);

return buf;
}

string Row_Users::ForwardEmail_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char *buf = new char[301];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ForwardEmail.c_str(), (unsigned long) min((size_t)150,m_ForwardEmail.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::FK_Language_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Language);

return buf;
}

string Row_Users::FK_Installation_Main_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Installation_Main);

return buf;
}

string Row_Users::HideFromOrbiter_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_HideFromOrbiter);

return buf;
}

string Row_Users::Staff_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Staff);

return buf;
}

string Row_Users::Password_Unix_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Password_Unix.c_str(), (unsigned long) min((size_t)300,m_Password_Unix.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::Password_Samba_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Password_Samba.c_str(), (unsigned long) min((size_t)300,m_Password_Samba.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::FK_UserMode_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_UserMode);

return buf;
}

string Row_Users::EK_Dealer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Dealer);

return buf;
}

string Row_Users::EK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_EK_Manufacturer);

return buf;
}

string Row_Users::RequirePinToSelect_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_RequirePinToSelect);

return buf;
}

string Row_Users::Use_VPN_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_Use_VPN);

return buf;
}

string Row_Users::VPN_Ip_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char *buf = new char[91];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_VPN_Ip.c_str(), (unsigned long) min((size_t)45,m_VPN_Ip.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::Connect_to_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char *buf = new char[25];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Connect_to.c_str(), (unsigned long) min((size_t)12,m_Connect_to.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_Users::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_Users::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[27])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_Users::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[28])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_Users::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[29])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_Users::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[30])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_Users::Key::Key(long int in_PK_Users)
{
			pk_PK_Users = in_PK_Users;
	
}

Table_Users::Key::Key(Row_Users *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_Users = pRow->m_PK_Users;
	
}		

bool Table_Users::Key_Less::operator()(const Table_Users::Key &key1, const Table_Users::Key &key2) const
{
			if (key1.pk_PK_Users!=key2.pk_PK_Users)
return key1.pk_PK_Users<key2.pk_PK_Users;
else
return false;	
}	

bool Table_Users::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_Users *pRow = (Row_Users *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_Users_asSQL()+", "+pRow->UserName_asSQL()+", "+pRow->Password_asSQL()+", "+pRow->PINCode_asSQL()+", "+pRow->HasMailbox_asSQL()+", "+pRow->AccessGeneralMailbox_asSQL()+", "+pRow->Extension_asSQL()+", "+pRow->FirstName_asSQL()+", "+pRow->LastName_asSQL()+", "+pRow->Nickname_asSQL()+", "+pRow->ExtensionRingTimeout_asSQL()+", "+pRow->ForwardEmail_asSQL()+", "+pRow->FK_Language_asSQL()+", "+pRow->FK_Installation_Main_asSQL()+", "+pRow->HideFromOrbiter_asSQL()+", "+pRow->Staff_asSQL()+", "+pRow->Password_Unix_asSQL()+", "+pRow->Password_Samba_asSQL()+", "+pRow->FK_UserMode_asSQL()+", "+pRow->EK_Dealer_asSQL()+", "+pRow->EK_Manufacturer_asSQL()+", "+pRow->RequirePinToSelect_asSQL()+", "+pRow->Use_VPN_asSQL()+", "+pRow->VPN_Ip_asSQL()+", "+pRow->Connect_to_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into Users (`PK_Users`, `UserName`, `Password`, `PINCode`, `HasMailbox`, `AccessGeneralMailbox`, `Extension`, `FirstName`, `LastName`, `Nickname`, `ExtensionRingTimeout`, `ForwardEmail`, `FK_Language`, `FK_Installation_Main`, `HideFromOrbiter`, `Staff`, `Password_Unix`, `Password_Samba`, `FK_UserMode`, `EK_Dealer`, `EK_Manufacturer`, `RequirePinToSelect`, `Use_VPN`, `VPN_Ip`, `Connect_to`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_Users=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_Users is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_Users);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_Users* pRow = (Row_Users*) (*i).second;	
		SingleLongKey key(pRow->m_PK_Users);

		char tmp_PK_Users[32];
sprintf(tmp_PK_Users, "%li", key.pk);


string condition;
condition = condition + "`PK_Users`=" + tmp_PK_Users;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_Users`="+pRow->PK_Users_asSQL()+", `UserName`="+pRow->UserName_asSQL()+", `Password`="+pRow->Password_asSQL()+", `PINCode`="+pRow->PINCode_asSQL()+", `HasMailbox`="+pRow->HasMailbox_asSQL()+", `AccessGeneralMailbox`="+pRow->AccessGeneralMailbox_asSQL()+", `Extension`="+pRow->Extension_asSQL()+", `FirstName`="+pRow->FirstName_asSQL()+", `LastName`="+pRow->LastName_asSQL()+", `Nickname`="+pRow->Nickname_asSQL()+", `ExtensionRingTimeout`="+pRow->ExtensionRingTimeout_asSQL()+", `ForwardEmail`="+pRow->ForwardEmail_asSQL()+", `FK_Language`="+pRow->FK_Language_asSQL()+", `FK_Installation_Main`="+pRow->FK_Installation_Main_asSQL()+", `HideFromOrbiter`="+pRow->HideFromOrbiter_asSQL()+", `Staff`="+pRow->Staff_asSQL()+", `Password_Unix`="+pRow->Password_Unix_asSQL()+", `Password_Samba`="+pRow->Password_Samba_asSQL()+", `FK_UserMode`="+pRow->FK_UserMode_asSQL()+", `EK_Dealer`="+pRow->EK_Dealer_asSQL()+", `EK_Manufacturer`="+pRow->EK_Manufacturer_asSQL()+", `RequirePinToSelect`="+pRow->RequirePinToSelect_asSQL()+", `Use_VPN`="+pRow->Use_VPN_asSQL()+", `VPN_Ip`="+pRow->VPN_Ip_asSQL()+", `Connect_to`="+pRow->Connect_to_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update Users set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_Users* pRow = (Row_Users*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_Users* pRow = (Row_Users*) (*i).second;	

		char tmp_PK_Users[32];
sprintf(tmp_PK_Users, "%li", key.pk);


string condition;
condition = condition + "`PK_Users`=" + tmp_PK_Users;

	
		string query = "delete from Users where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_Users*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_Users::GetRows(string where_statement,vector<class Row_Users*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `Users`.* from Users " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `Users`.* from Users where " + where_statement;
	else
		query = "select `Users`.* from Users";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_Users *pRow = new Row_Users(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Users = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Users));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_UserName = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_UserName = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Password = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Password = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_PINCode = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_PINCode = string(row[3],lengths[3]);
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
sscanf(row[6], "%li", &(pRow->m_Extension));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FirstName = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_FirstName = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_LastName = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_LastName = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Nickname = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Nickname = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ExtensionRingTimeout = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_ExtensionRingTimeout));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_ForwardEmail = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_ForwardEmail = string(row[11],lengths[11]);
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
sscanf(row[13], "%li", &(pRow->m_FK_Installation_Main));
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
pRow->m_Password_Unix = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_Password_Unix = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Password_Samba = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Password_Samba = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_UserMode = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_UserMode));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_EK_Dealer = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_EK_Dealer));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_EK_Manufacturer = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_EK_Manufacturer));
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
pRow->m_Use_VPN = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_Use_VPN));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_VPN_Ip = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_VPN_Ip = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_Connect_to = "";
}
else
{
pRow->is_null[24]=false;
pRow->m_Connect_to = string(row[24],lengths[24]);
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_psc_id));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_psc_batch));
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%li", &(pRow->m_psc_user));
}

if (row[28] == NULL)
{
pRow->is_null[28]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[28]=false;
sscanf(row[28], "%hi", &(pRow->m_psc_frozen));
}

if (row[29] == NULL)
{
pRow->is_null[29]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[29]=false;
pRow->m_psc_mod = string(row[29],lengths[29]);
}

if (row[30] == NULL)
{
pRow->is_null[30]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[30]=false;
sscanf(row[30], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_Users);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_Users *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_Users* Table_Users::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_Users *pRow = new Row_Users(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_Users* Table_Users::GetRow(long int in_PK_Users)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_Users);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_Users*) (*i).second;
	//we have to fetch row
	Row_Users* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_Users* Table_Users::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_Users[32];
sprintf(tmp_PK_Users, "%li", key.pk);


string condition;
condition = condition + "`PK_Users`=" + tmp_PK_Users;


	string query = "select * from Users where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_Users::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_Users *pRow = new Row_Users(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_Users = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_Users));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_UserName = "";
}
else
{
pRow->is_null[1]=false;
pRow->m_UserName = string(row[1],lengths[1]);
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Password = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Password = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_PINCode = "";
}
else
{
pRow->is_null[3]=false;
pRow->m_PINCode = string(row[3],lengths[3]);
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
sscanf(row[6], "%li", &(pRow->m_Extension));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_FirstName = "";
}
else
{
pRow->is_null[7]=false;
pRow->m_FirstName = string(row[7],lengths[7]);
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_LastName = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_LastName = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_Nickname = "";
}
else
{
pRow->is_null[9]=false;
pRow->m_Nickname = string(row[9],lengths[9]);
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_ExtensionRingTimeout = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%li", &(pRow->m_ExtensionRingTimeout));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_ForwardEmail = "";
}
else
{
pRow->is_null[11]=false;
pRow->m_ForwardEmail = string(row[11],lengths[11]);
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
sscanf(row[13], "%li", &(pRow->m_FK_Installation_Main));
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
pRow->m_Password_Unix = "";
}
else
{
pRow->is_null[16]=false;
pRow->m_Password_Unix = string(row[16],lengths[16]);
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_Password_Samba = "";
}
else
{
pRow->is_null[17]=false;
pRow->m_Password_Samba = string(row[17],lengths[17]);
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_UserMode = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_UserMode));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_EK_Dealer = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%li", &(pRow->m_EK_Dealer));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_EK_Manufacturer = 0;
}
else
{
pRow->is_null[20]=false;
sscanf(row[20], "%li", &(pRow->m_EK_Manufacturer));
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
pRow->m_Use_VPN = 0;
}
else
{
pRow->is_null[22]=false;
sscanf(row[22], "%li", &(pRow->m_Use_VPN));
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_VPN_Ip = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_VPN_Ip = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_Connect_to = "";
}
else
{
pRow->is_null[24]=false;
pRow->m_Connect_to = string(row[24],lengths[24]);
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_psc_id));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_psc_batch));
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%li", &(pRow->m_psc_user));
}

if (row[28] == NULL)
{
pRow->is_null[28]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[28]=false;
sscanf(row[28], "%hi", &(pRow->m_psc_frozen));
}

if (row[29] == NULL)
{
pRow->is_null[29]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[29]=false;
pRow->m_psc_mod = string(row[29],lengths[29]);
}

if (row[30] == NULL)
{
pRow->is_null[30]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[30]=false;
sscanf(row[30], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_Language* Row_Users::FK_Language_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Language *pTable = table->database->Language_get();
return pTable->GetRow(m_FK_Language);
}
class Row_Installation* Row_Users::FK_Installation_Main_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation *pTable = table->database->Installation_get();
return pTable->GetRow(m_FK_Installation_Main);
}
class Row_UserMode* Row_Users::FK_UserMode_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_UserMode *pTable = table->database->UserMode_get();
return pTable->GetRow(m_FK_UserMode);
}


void Row_Users::DeviceTemplate_FK_Users_Maintainer_getrows(vector <class Row_DeviceTemplate*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
pTable->GetRows("`FK_Users_Maintainer`=" + StringUtils::itos(m_PK_Users),rows);
}
void Row_Users::Device_Users_FK_Users_getrows(vector <class Row_Device_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device_Users *pTable = table->database->Device_Users_get();
pTable->GetRows("`FK_Users`=" + StringUtils::itos(m_PK_Users),rows);
}
void Row_Users::Document_Comment_FK_Users_getrows(vector <class Row_Document_Comment*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Document_Comment *pTable = table->database->Document_Comment_get();
pTable->GetRows("`FK_Users`=" + StringUtils::itos(m_PK_Users),rows);
}
void Row_Users::Installation_Users_FK_Users_getrows(vector <class Row_Installation_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Installation_Users *pTable = table->database->Installation_Users_get();
pTable->GetRows("`FK_Users`=" + StringUtils::itos(m_PK_Users),rows);
}
void Row_Users::Orbiter_Users_PasswordReq_FK_Users_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Orbiter_Users_PasswordReq *pTable = table->database->Orbiter_Users_PasswordReq_get();
pTable->GetRows("`FK_Users`=" + StringUtils::itos(m_PK_Users),rows);
}
void Row_Users::Package_Users_FK_Users_getrows(vector <class Row_Package_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package_Users *pTable = table->database->Package_Users_get();
pTable->GetRows("`FK_Users`=" + StringUtils::itos(m_PK_Users),rows);
}
void Row_Users::Room_Users_FK_Users_getrows(vector <class Row_Room_Users*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Room_Users *pTable = table->database->Room_Users_get();
pTable->GetRows("`FK_Users`=" + StringUtils::itos(m_PK_Users),rows);
}



