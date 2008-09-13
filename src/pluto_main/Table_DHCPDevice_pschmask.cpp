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
#include "Table_DHCPDevice_pschmask.h"
#include "Table_DeviceTemplate.h"
#include "Table_PnpProtocol.h"
#include "Table_DeviceCategory.h"
#include "Table_Manufacturer.h"



void Database_pluto_main::CreateTable_DHCPDevice_pschmask()
{
	tblDHCPDevice_pschmask = new Table_DHCPDevice_pschmask(this);
}

void Database_pluto_main::DeleteTable_DHCPDevice_pschmask()
{
	if( tblDHCPDevice_pschmask )
		delete tblDHCPDevice_pschmask;
}

bool Database_pluto_main::Commit_DHCPDevice_pschmask(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDHCPDevice_pschmask->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DHCPDevice_pschmask::~Table_DHCPDevice_pschmask()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DHCPDevice_pschmask *pRow = (Row_DHCPDevice_pschmask *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DHCPDevice_pschmask *pRow = (Row_DHCPDevice_pschmask *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DHCPDevice_pschmask::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DHCPDevice_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DHCPDevice_pschmask *) *i != this); i++);
			
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

void Row_DHCPDevice_pschmask::Reload()
{
	Row_DHCPDevice_pschmask *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_DHCPDevice_pschmask *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DHCPDevice_pschmask::Row_DHCPDevice_pschmask(Table_DHCPDevice_pschmask *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DHCPDevice_pschmask::SetDefaultValues()
{
	is_null[0] = true;
m_PK_DHCPDevice = 0;
is_null[1] = true;
m_FK_DeviceTemplate = 0;
is_null[2] = true;
m_Mac_Range_Low = 0;
is_null[3] = true;
m_Mac_Range_High = 0;
is_null[4] = true;
m_VendorModelID = 0;
is_null[5] = true;
m_SerialNumber = 0;
is_null[6] = true;
m_Parms = 0;
is_null[7] = true;
m_Category = 0;
is_null[8] = true;
m_FK_PnpProtocol = 0;
is_null[9] = true;
m_PnpDetectionScript = 0;
is_null[10] = true;
m_FK_DeviceCategory = 0;
is_null[11] = true;
m_FK_Manufacturer = 0;
is_null[12] = true;
m_Description = 0;
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


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

short int Row_DHCPDevice_pschmask::PK_DHCPDevice_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DHCPDevice;}
short int Row_DHCPDevice_pschmask::FK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceTemplate;}
short int Row_DHCPDevice_pschmask::Mac_Range_Low_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Mac_Range_Low;}
short int Row_DHCPDevice_pschmask::Mac_Range_High_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Mac_Range_High;}
short int Row_DHCPDevice_pschmask::VendorModelID_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_VendorModelID;}
short int Row_DHCPDevice_pschmask::SerialNumber_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_SerialNumber;}
short int Row_DHCPDevice_pschmask::Parms_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Parms;}
short int Row_DHCPDevice_pschmask::Category_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Category;}
short int Row_DHCPDevice_pschmask::FK_PnpProtocol_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PnpProtocol;}
short int Row_DHCPDevice_pschmask::PnpDetectionScript_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PnpDetectionScript;}
short int Row_DHCPDevice_pschmask::FK_DeviceCategory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceCategory;}
short int Row_DHCPDevice_pschmask::FK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Manufacturer;}
short int Row_DHCPDevice_pschmask::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
long int Row_DHCPDevice_pschmask::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DHCPDevice_pschmask::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
short int Row_DHCPDevice_pschmask::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DHCPDevice_pschmask::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DHCPDevice_pschmask::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DHCPDevice_pschmask::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DHCPDevice_pschmask::PK_DHCPDevice_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DHCPDevice = val; is_modified=true; is_null[0]=false;}
void Row_DHCPDevice_pschmask::FK_DeviceTemplate_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceTemplate = val; is_modified=true; is_null[1]=false;}
void Row_DHCPDevice_pschmask::Mac_Range_Low_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Mac_Range_Low = val; is_modified=true; is_null[2]=false;}
void Row_DHCPDevice_pschmask::Mac_Range_High_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Mac_Range_High = val; is_modified=true; is_null[3]=false;}
void Row_DHCPDevice_pschmask::VendorModelID_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_VendorModelID = val; is_modified=true; is_null[4]=false;}
void Row_DHCPDevice_pschmask::SerialNumber_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_SerialNumber = val; is_modified=true; is_null[5]=false;}
void Row_DHCPDevice_pschmask::Parms_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Parms = val; is_modified=true; is_null[6]=false;}
void Row_DHCPDevice_pschmask::Category_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Category = val; is_modified=true; is_null[7]=false;}
void Row_DHCPDevice_pschmask::FK_PnpProtocol_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PnpProtocol = val; is_modified=true; is_null[8]=false;}
void Row_DHCPDevice_pschmask::PnpDetectionScript_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PnpDetectionScript = val; is_modified=true; is_null[9]=false;}
void Row_DHCPDevice_pschmask::FK_DeviceCategory_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceCategory = val; is_modified=true; is_null[10]=false;}
void Row_DHCPDevice_pschmask::FK_Manufacturer_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Manufacturer = val; is_modified=true; is_null[11]=false;}
void Row_DHCPDevice_pschmask::Description_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[12]=false;}
void Row_DHCPDevice_pschmask::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[13]=false;}
void Row_DHCPDevice_pschmask::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[14]=false;}
void Row_DHCPDevice_pschmask::psc_user_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[15]=false;}
void Row_DHCPDevice_pschmask::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[16]=false;}
void Row_DHCPDevice_pschmask::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[17]=false;}
void Row_DHCPDevice_pschmask::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[18]=false;}

		
bool Row_DHCPDevice_pschmask::PK_DHCPDevice_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_DHCPDevice_pschmask::FK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DHCPDevice_pschmask::Mac_Range_Low_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DHCPDevice_pschmask::Mac_Range_High_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DHCPDevice_pschmask::VendorModelID_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DHCPDevice_pschmask::SerialNumber_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_DHCPDevice_pschmask::Parms_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_DHCPDevice_pschmask::Category_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_DHCPDevice_pschmask::FK_PnpProtocol_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_DHCPDevice_pschmask::PnpDetectionScript_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DHCPDevice_pschmask::FK_DeviceCategory_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DHCPDevice_pschmask::FK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DHCPDevice_pschmask::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DHCPDevice_pschmask::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_DHCPDevice_pschmask::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_DHCPDevice_pschmask::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DHCPDevice_pschmask::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}

			
void Row_DHCPDevice_pschmask::PK_DHCPDevice_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::FK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::Mac_Range_Low_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::Mac_Range_High_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::VendorModelID_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::SerialNumber_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::Parms_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::Category_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::FK_PnpProtocol_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::PnpDetectionScript_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::FK_DeviceCategory_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::FK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DHCPDevice_pschmask::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
	

string Row_DHCPDevice_pschmask::PK_DHCPDevice_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PK_DHCPDevice);

return buf;
}

string Row_DHCPDevice_pschmask::FK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_DeviceTemplate);

return buf;
}

string Row_DHCPDevice_pschmask::Mac_Range_Low_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Mac_Range_Low);

return buf;
}

string Row_DHCPDevice_pschmask::Mac_Range_High_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Mac_Range_High);

return buf;
}

string Row_DHCPDevice_pschmask::VendorModelID_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_VendorModelID);

return buf;
}

string Row_DHCPDevice_pschmask::SerialNumber_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_SerialNumber);

return buf;
}

string Row_DHCPDevice_pschmask::Parms_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Parms);

return buf;
}

string Row_DHCPDevice_pschmask::Category_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Category);

return buf;
}

string Row_DHCPDevice_pschmask::FK_PnpProtocol_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_PnpProtocol);

return buf;
}

string Row_DHCPDevice_pschmask::PnpDetectionScript_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_PnpDetectionScript);

return buf;
}

string Row_DHCPDevice_pschmask::FK_DeviceCategory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_DeviceCategory);

return buf;
}

string Row_DHCPDevice_pschmask::FK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_Manufacturer);

return buf;
}

string Row_DHCPDevice_pschmask::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_Description);

return buf;
}

string Row_DHCPDevice_pschmask::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DHCPDevice_pschmask::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DHCPDevice_pschmask::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_user);

return buf;
}

string Row_DHCPDevice_pschmask::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DHCPDevice_pschmask::psc_mod_asSQL()
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

string Row_DHCPDevice_pschmask::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DHCPDevice_pschmask::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_DHCPDevice_pschmask::Key::Key(Row_DHCPDevice_pschmask *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_DHCPDevice_pschmask::Key_Less::operator()(const Table_DHCPDevice_pschmask::Key &key1, const Table_DHCPDevice_pschmask::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_DHCPDevice_pschmask::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DHCPDevice_pschmask *pRow = (Row_DHCPDevice_pschmask *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DHCPDevice_asSQL()+", "+pRow->FK_DeviceTemplate_asSQL()+", "+pRow->Mac_Range_Low_asSQL()+", "+pRow->Mac_Range_High_asSQL()+", "+pRow->VendorModelID_asSQL()+", "+pRow->SerialNumber_asSQL()+", "+pRow->Parms_asSQL()+", "+pRow->Category_asSQL()+", "+pRow->FK_PnpProtocol_asSQL()+", "+pRow->PnpDetectionScript_asSQL()+", "+pRow->FK_DeviceCategory_asSQL()+", "+pRow->FK_Manufacturer_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DHCPDevice_pschmask (`PK_DHCPDevice`, `FK_DeviceTemplate`, `Mac_Range_Low`, `Mac_Range_High`, `VendorModelID`, `SerialNumber`, `Parms`, `Category`, `FK_PnpProtocol`, `PnpDetectionScript`, `FK_DeviceCategory`, `FK_Manufacturer`, `Description`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DHCPDevice_pschmask* pRow = (Row_DHCPDevice_pschmask*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DHCPDevice`="+pRow->PK_DHCPDevice_asSQL()+", `FK_DeviceTemplate`="+pRow->FK_DeviceTemplate_asSQL()+", `Mac_Range_Low`="+pRow->Mac_Range_Low_asSQL()+", `Mac_Range_High`="+pRow->Mac_Range_High_asSQL()+", `VendorModelID`="+pRow->VendorModelID_asSQL()+", `SerialNumber`="+pRow->SerialNumber_asSQL()+", `Parms`="+pRow->Parms_asSQL()+", `Category`="+pRow->Category_asSQL()+", `FK_PnpProtocol`="+pRow->FK_PnpProtocol_asSQL()+", `PnpDetectionScript`="+pRow->PnpDetectionScript_asSQL()+", `FK_DeviceCategory`="+pRow->FK_DeviceCategory_asSQL()+", `FK_Manufacturer`="+pRow->FK_Manufacturer_asSQL()+", `Description`="+pRow->Description_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DHCPDevice_pschmask set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DHCPDevice_pschmask* pRow = (Row_DHCPDevice_pschmask*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DHCPDevice_pschmask* pRow = (Row_DHCPDevice_pschmask*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from DHCPDevice_pschmask where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DHCPDevice_pschmask*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DHCPDevice_pschmask::GetRows(string where_statement,vector<class Row_DHCPDevice_pschmask*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DHCPDevice_pschmask`.* from DHCPDevice_pschmask " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DHCPDevice_pschmask`.* from DHCPDevice_pschmask where " + where_statement;
	else
		query = "select `DHCPDevice_pschmask`.* from DHCPDevice_pschmask";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DHCPDevice_pschmask *pRow = new Row_DHCPDevice_pschmask(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DHCPDevice = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_DHCPDevice));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_FK_DeviceTemplate));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Mac_Range_Low = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Mac_Range_Low));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Mac_Range_High = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Mac_Range_High));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_VendorModelID = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_VendorModelID));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SerialNumber = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_SerialNumber));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Parms = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Parms));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Category = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Category));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_PnpProtocol = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_FK_PnpProtocol));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_PnpDetectionScript = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_PnpDetectionScript));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_FK_DeviceCategory));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_FK_Manufacturer));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Description = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_Description));
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



		//checking for duplicates

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DHCPDevice_pschmask *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DHCPDevice_pschmask* Table_DHCPDevice_pschmask::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DHCPDevice_pschmask *pRow = new Row_DHCPDevice_pschmask(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DHCPDevice_pschmask* Table_DHCPDevice_pschmask::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_DHCPDevice_pschmask*) (*i).second;
	//we have to fetch row
	Row_DHCPDevice_pschmask* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DHCPDevice_pschmask* Table_DHCPDevice_pschmask::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from DHCPDevice_pschmask where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DHCPDevice_pschmask::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DHCPDevice_pschmask *pRow = new Row_DHCPDevice_pschmask(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DHCPDevice = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%hi", &(pRow->m_PK_DHCPDevice));
}

if (row[1] == NULL)
{
pRow->is_null[1]=true;
pRow->m_FK_DeviceTemplate = 0;
}
else
{
pRow->is_null[1]=false;
sscanf(row[1], "%hi", &(pRow->m_FK_DeviceTemplate));
}

if (row[2] == NULL)
{
pRow->is_null[2]=true;
pRow->m_Mac_Range_Low = 0;
}
else
{
pRow->is_null[2]=false;
sscanf(row[2], "%hi", &(pRow->m_Mac_Range_Low));
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_Mac_Range_High = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%hi", &(pRow->m_Mac_Range_High));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_VendorModelID = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%hi", &(pRow->m_VendorModelID));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_SerialNumber = 0;
}
else
{
pRow->is_null[5]=false;
sscanf(row[5], "%hi", &(pRow->m_SerialNumber));
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_Parms = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_Parms));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_Category = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_Category));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_FK_PnpProtocol = 0;
}
else
{
pRow->is_null[8]=false;
sscanf(row[8], "%hi", &(pRow->m_FK_PnpProtocol));
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_PnpDetectionScript = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_PnpDetectionScript));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_FK_DeviceCategory));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_FK_Manufacturer));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_Description = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%hi", &(pRow->m_Description));
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



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DeviceTemplate* Row_DHCPDevice_pschmask::FK_DeviceTemplate_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate *pTable = table->database->DeviceTemplate_get();
return pTable->GetRow(m_FK_DeviceTemplate);
}
class Row_PnpProtocol* Row_DHCPDevice_pschmask::FK_PnpProtocol_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PnpProtocol *pTable = table->database->PnpProtocol_get();
return pTable->GetRow(m_FK_PnpProtocol);
}
class Row_DeviceCategory* Row_DHCPDevice_pschmask::FK_DeviceCategory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceCategory *pTable = table->database->DeviceCategory_get();
return pTable->GetRow(m_FK_DeviceCategory);
}
class Row_Manufacturer* Row_DHCPDevice_pschmask::FK_Manufacturer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Manufacturer *pTable = table->database->Manufacturer_get();
return pTable->GetRow(m_FK_Manufacturer);
}





