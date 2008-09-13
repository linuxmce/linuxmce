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
#include "Table_DeviceTemplate_pschist.h"
#include "Table_DeviceCategory.h"
#include "Table_Manufacturer.h"
#include "Table_StabilityStatus.h"
#include "Table_Package.h"
#include "Table_Users.h"
#include "Table_OperatingSystem.h"
#include "Table_Distro.h"
#include "Table_Licensing.h"
#include "Table_InfraredGroup.h"
#include "Table_CommMethod.h"
#include "Table_PnpLevel.h"
#include "Table_Document.h"



void Database_pluto_main::CreateTable_DeviceTemplate_pschist()
{
	tblDeviceTemplate_pschist = new Table_DeviceTemplate_pschist(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate_pschist()
{
	if( tblDeviceTemplate_pschist )
		delete tblDeviceTemplate_pschist;
}

bool Database_pluto_main::Commit_DeviceTemplate_pschist(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDeviceTemplate_pschist->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DeviceTemplate_pschist::~Table_DeviceTemplate_pschist()
{
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DeviceTemplate_pschist *pRow = (Row_DeviceTemplate_pschist *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DeviceTemplate_pschist *pRow = (Row_DeviceTemplate_pschist *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DeviceTemplate_pschist::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DeviceTemplate_pschist *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DeviceTemplate_pschist *) *i != this); i++);
			
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

void Row_DeviceTemplate_pschist::Reload()
{
	Row_DeviceTemplate_pschist *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		Row_DeviceTemplate_pschist *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate_pschist::Row_DeviceTemplate_pschist(Table_DeviceTemplate_pschist *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate_pschist::SetDefaultValues()
{
	is_null[0] = true;
m_PK_DeviceTemplate = 0;
is_null[1] = true;
is_null[2] = true;
is_null[3] = true;
m_FK_DeviceCategory = 0;
is_null[4] = true;
m_FK_Manufacturer = 0;
is_null[5] = true;
is_null[6] = true;
m_ImplementsDCE = 0;
is_null[7] = true;
m_IsEmbedded = 0;
is_null[8] = true;
is_null[9] = true;
m_IsPlugAndPlay = 0;
is_null[10] = true;
m_IsIPBased = 0;
is_null[11] = true;
m_IsPlugIn = 0;
is_null[12] = true;
m_IRFrequency = 0;
is_null[13] = true;
m_FK_StabilityStatus = 0;
is_null[14] = true;
m_FK_Package = 0;
is_null[15] = true;
is_null[16] = true;
m_FK_Users_Maintainer = 0;
is_null[17] = true;
m_FK_OperatingSystem = 0;
is_null[18] = true;
m_FK_Distro = 0;
is_null[19] = true;
m_InheritsMacFromPC = 0;
is_null[20] = true;
is_null[21] = true;
is_null[22] = true;
is_null[23] = true;
is_null[24] = true;
m_FK_Licensing = 0;
is_null[25] = true;
m_FK_InfraredGroup = 0;
is_null[26] = true;
m_FK_CommMethod = 0;
is_null[27] = true;
m_FK_PnpLevel = 0;
is_null[28] = true;
m_psc_id = 0;
is_null[29] = false;
m_psc_batch = 0;
is_null[30] = false;
is_null[31] = true;
m_psc_user = 0;
is_null[32] = true;
m_psc_frozen = 0;
is_null[33] = true;
m_psc_toc = 0;
is_null[34] = false;
is_null[35] = true;
m_psc_restrict = 0;
is_null[36] = true;
m_FK_Document = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DeviceTemplate_pschist::PK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DeviceTemplate;}
string Row_DeviceTemplate_pschist::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_DeviceTemplate_pschist::Comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Comments;}
long int Row_DeviceTemplate_pschist::FK_DeviceCategory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceCategory;}
long int Row_DeviceTemplate_pschist::FK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Manufacturer;}
string Row_DeviceTemplate_pschist::Define_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Define;}
short int Row_DeviceTemplate_pschist::ImplementsDCE_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ImplementsDCE;}
short int Row_DeviceTemplate_pschist::IsEmbedded_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsEmbedded;}
string Row_DeviceTemplate_pschist::CommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CommandLine;}
short int Row_DeviceTemplate_pschist::IsPlugAndPlay_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsPlugAndPlay;}
short int Row_DeviceTemplate_pschist::IsIPBased_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsIPBased;}
short int Row_DeviceTemplate_pschist::IsPlugIn_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsPlugIn;}
long int Row_DeviceTemplate_pschist::IRFrequency_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IRFrequency;}
long int Row_DeviceTemplate_pschist::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_StabilityStatus;}
long int Row_DeviceTemplate_pschist::FK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Package;}
string Row_DeviceTemplate_pschist::DestinationDir_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DestinationDir;}
long int Row_DeviceTemplate_pschist::FK_Users_Maintainer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Users_Maintainer;}
long int Row_DeviceTemplate_pschist::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_OperatingSystem;}
long int Row_DeviceTemplate_pschist::FK_Distro_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Distro;}
short int Row_DeviceTemplate_pschist::InheritsMacFromPC_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_InheritsMacFromPC;}
string Row_DeviceTemplate_pschist::ConfigureScript_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ConfigureScript;}
string Row_DeviceTemplate_pschist::ManufacturerURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ManufacturerURL;}
string Row_DeviceTemplate_pschist::InternalURLSuffix_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_InternalURLSuffix;}
string Row_DeviceTemplate_pschist::WizardURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_WizardURL;}
long int Row_DeviceTemplate_pschist::FK_Licensing_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Licensing;}
long int Row_DeviceTemplate_pschist::FK_InfraredGroup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_InfraredGroup;}
long int Row_DeviceTemplate_pschist::FK_CommMethod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommMethod;}
short int Row_DeviceTemplate_pschist::FK_PnpLevel_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PnpLevel;}
string Row_DeviceTemplate_pschist::HelpURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HelpURL;}
long int Row_DeviceTemplate_pschist::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DeviceTemplate_pschist::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DeviceTemplate_pschist::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DeviceTemplate_pschist::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DeviceTemplate_pschist::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
short int Row_DeviceTemplate_pschist::psc_toc_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_toc;}
long int Row_DeviceTemplate_pschist::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}
long int Row_DeviceTemplate_pschist::FK_Document_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Document;}

		
void Row_DeviceTemplate_pschist::PK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DeviceTemplate = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate_pschist::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate_pschist::Comments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Comments = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate_pschist::FK_DeviceCategory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceCategory = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate_pschist::FK_Manufacturer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Manufacturer = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate_pschist::Define_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Define = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate_pschist::ImplementsDCE_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ImplementsDCE = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate_pschist::IsEmbedded_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsEmbedded = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate_pschist::CommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CommandLine = val; is_modified=true; is_null[8]=false;}
void Row_DeviceTemplate_pschist::IsPlugAndPlay_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsPlugAndPlay = val; is_modified=true; is_null[9]=false;}
void Row_DeviceTemplate_pschist::IsIPBased_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsIPBased = val; is_modified=true; is_null[10]=false;}
void Row_DeviceTemplate_pschist::IsPlugIn_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsPlugIn = val; is_modified=true; is_null[11]=false;}
void Row_DeviceTemplate_pschist::IRFrequency_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IRFrequency = val; is_modified=true; is_null[12]=false;}
void Row_DeviceTemplate_pschist::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[13]=false;}
void Row_DeviceTemplate_pschist::FK_Package_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Package = val; is_modified=true; is_null[14]=false;}
void Row_DeviceTemplate_pschist::DestinationDir_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DestinationDir = val; is_modified=true; is_null[15]=false;}
void Row_DeviceTemplate_pschist::FK_Users_Maintainer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Users_Maintainer = val; is_modified=true; is_null[16]=false;}
void Row_DeviceTemplate_pschist::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[17]=false;}
void Row_DeviceTemplate_pschist::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Distro = val; is_modified=true; is_null[18]=false;}
void Row_DeviceTemplate_pschist::InheritsMacFromPC_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_InheritsMacFromPC = val; is_modified=true; is_null[19]=false;}
void Row_DeviceTemplate_pschist::ConfigureScript_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ConfigureScript = val; is_modified=true; is_null[20]=false;}
void Row_DeviceTemplate_pschist::ManufacturerURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ManufacturerURL = val; is_modified=true; is_null[21]=false;}
void Row_DeviceTemplate_pschist::InternalURLSuffix_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_InternalURLSuffix = val; is_modified=true; is_null[22]=false;}
void Row_DeviceTemplate_pschist::WizardURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_WizardURL = val; is_modified=true; is_null[23]=false;}
void Row_DeviceTemplate_pschist::FK_Licensing_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Licensing = val; is_modified=true; is_null[24]=false;}
void Row_DeviceTemplate_pschist::FK_InfraredGroup_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_InfraredGroup = val; is_modified=true; is_null[25]=false;}
void Row_DeviceTemplate_pschist::FK_CommMethod_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommMethod = val; is_modified=true; is_null[26]=false;}
void Row_DeviceTemplate_pschist::FK_PnpLevel_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PnpLevel = val; is_modified=true; is_null[27]=false;}
void Row_DeviceTemplate_pschist::HelpURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HelpURL = val; is_modified=true; is_null[28]=false;}
void Row_DeviceTemplate_pschist::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[29]=false;}
void Row_DeviceTemplate_pschist::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[30]=false;}
void Row_DeviceTemplate_pschist::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[31]=false;}
void Row_DeviceTemplate_pschist::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[32]=false;}
void Row_DeviceTemplate_pschist::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[33]=false;}
void Row_DeviceTemplate_pschist::psc_toc_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_toc = val; is_modified=true; is_null[34]=false;}
void Row_DeviceTemplate_pschist::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[35]=false;}
void Row_DeviceTemplate_pschist::FK_Document_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Document = val; is_modified=true; is_null[36]=false;}

		
bool Row_DeviceTemplate_pschist::PK_DeviceTemplate_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[0];}
bool Row_DeviceTemplate_pschist::Description_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[1];}
bool Row_DeviceTemplate_pschist::Comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DeviceTemplate_pschist::FK_DeviceCategory_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[3];}
bool Row_DeviceTemplate_pschist::FK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DeviceTemplate_pschist::Define_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[5];}
bool Row_DeviceTemplate_pschist::ImplementsDCE_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[6];}
bool Row_DeviceTemplate_pschist::IsEmbedded_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_DeviceTemplate_pschist::CommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_DeviceTemplate_pschist::IsPlugAndPlay_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[9];}
bool Row_DeviceTemplate_pschist::IsIPBased_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[10];}
bool Row_DeviceTemplate_pschist::IsPlugIn_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[11];}
bool Row_DeviceTemplate_pschist::IRFrequency_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DeviceTemplate_pschist::FK_StabilityStatus_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DeviceTemplate_pschist::FK_Package_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_DeviceTemplate_pschist::DestinationDir_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_DeviceTemplate_pschist::FK_Users_Maintainer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[16];}
bool Row_DeviceTemplate_pschist::FK_OperatingSystem_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DeviceTemplate_pschist::FK_Distro_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DeviceTemplate_pschist::InheritsMacFromPC_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_DeviceTemplate_pschist::ConfigureScript_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_DeviceTemplate_pschist::ManufacturerURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_DeviceTemplate_pschist::InternalURLSuffix_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_DeviceTemplate_pschist::WizardURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}
bool Row_DeviceTemplate_pschist::FK_Licensing_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_DeviceTemplate_pschist::FK_InfraredGroup_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}
bool Row_DeviceTemplate_pschist::FK_CommMethod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}
bool Row_DeviceTemplate_pschist::FK_PnpLevel_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[27];}
bool Row_DeviceTemplate_pschist::HelpURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[28];}
bool Row_DeviceTemplate_pschist::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[31];}
bool Row_DeviceTemplate_pschist::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[32];}
bool Row_DeviceTemplate_pschist::psc_mod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[33];}
bool Row_DeviceTemplate_pschist::psc_toc_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[34];}
bool Row_DeviceTemplate_pschist::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[35];}
bool Row_DeviceTemplate_pschist::FK_Document_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[36];}

			
void Row_DeviceTemplate_pschist::PK_DeviceTemplate_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[0]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::Description_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[1]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::Comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_DeviceCategory_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[3]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::Define_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[5]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::ImplementsDCE_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[6]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::IsEmbedded_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::CommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::IsPlugAndPlay_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[9]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::IsIPBased_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[10]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::IsPlugIn_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[11]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::IRFrequency_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_StabilityStatus_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_Package_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::DestinationDir_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_Users_Maintainer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[16]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_OperatingSystem_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_Distro_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::InheritsMacFromPC_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::ConfigureScript_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::ManufacturerURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::InternalURLSuffix_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::WizardURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_Licensing_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_InfraredGroup_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_CommMethod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_PnpLevel_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[27]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::HelpURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[28]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[31]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[32]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::psc_mod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[33]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::psc_toc_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[34]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[35]=val;
is_modified=true;
}
void Row_DeviceTemplate_pschist::FK_Document_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[36]=val;
is_modified=true;
}
	

string Row_DeviceTemplate_pschist::PK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate_pschist::Description_asSQL()
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

string Row_DeviceTemplate_pschist::Comments_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[2])
return "NULL";

char *buf = new char[5000000];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Comments.c_str(), (unsigned long) min((size_t)-1,m_Comments.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::FK_DeviceCategory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceCategory);

return buf;
}

string Row_DeviceTemplate_pschist::FK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Manufacturer);

return buf;
}

string Row_DeviceTemplate_pschist::Define_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[61];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Define.c_str(), (unsigned long) min((size_t)30,m_Define.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::ImplementsDCE_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ImplementsDCE);

return buf;
}

string Row_DeviceTemplate_pschist::IsEmbedded_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsEmbedded);

return buf;
}

string Row_DeviceTemplate_pschist::CommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_CommandLine.c_str(), (unsigned long) min((size_t)100,m_CommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::IsPlugAndPlay_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsPlugAndPlay);

return buf;
}

string Row_DeviceTemplate_pschist::IsIPBased_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsIPBased);

return buf;
}

string Row_DeviceTemplate_pschist::IsPlugIn_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsPlugIn);

return buf;
}

string Row_DeviceTemplate_pschist::IRFrequency_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_IRFrequency);

return buf;
}

string Row_DeviceTemplate_pschist::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_DeviceTemplate_pschist::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_DeviceTemplate_pschist::DestinationDir_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DestinationDir.c_str(), (unsigned long) min((size_t)50,m_DestinationDir.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::FK_Users_Maintainer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_Maintainer);

return buf;
}

string Row_DeviceTemplate_pschist::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}

string Row_DeviceTemplate_pschist::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_DeviceTemplate_pschist::InheritsMacFromPC_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_InheritsMacFromPC);

return buf;
}

string Row_DeviceTemplate_pschist::ConfigureScript_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ConfigureScript.c_str(), (unsigned long) min((size_t)100,m_ConfigureScript.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::ManufacturerURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char *buf = new char[201];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ManufacturerURL.c_str(), (unsigned long) min((size_t)100,m_ManufacturerURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::InternalURLSuffix_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_InternalURLSuffix.c_str(), (unsigned long) min((size_t)50,m_InternalURLSuffix.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::WizardURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char *buf = new char[101];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_WizardURL.c_str(), (unsigned long) min((size_t)50,m_WizardURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::FK_Licensing_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Licensing);

return buf;
}

string Row_DeviceTemplate_pschist::FK_InfraredGroup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_InfraredGroup);

return buf;
}

string Row_DeviceTemplate_pschist::FK_CommMethod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommMethod);

return buf;
}

string Row_DeviceTemplate_pschist::FK_PnpLevel_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[27])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_FK_PnpLevel);

return buf;
}

string Row_DeviceTemplate_pschist::HelpURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[28])
return "NULL";

char *buf = new char[511];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_HelpURL.c_str(), (unsigned long) min((size_t)255,m_HelpURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[29])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceTemplate_pschist::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[30])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceTemplate_pschist::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[31])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceTemplate_pschist::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[32])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceTemplate_pschist::psc_mod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[33])
return "NULL";

char *buf = new char[39];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_psc_mod.c_str(), (unsigned long) min((size_t)19,m_psc_mod.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate_pschist::psc_toc_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[34])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_toc);

return buf;
}

string Row_DeviceTemplate_pschist::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[35])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}

string Row_DeviceTemplate_pschist::FK_Document_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[36])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Document);

return buf;
}




Table_DeviceTemplate_pschist::Key::Key(long int in_psc_id, long int in_psc_batch)
{
			pk_psc_id = in_psc_id;
pk_psc_batch = in_psc_batch;
	
}

Table_DeviceTemplate_pschist::Key::Key(Row_DeviceTemplate_pschist *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_psc_id = pRow->m_psc_id;
pk_psc_batch = pRow->m_psc_batch;
	
}		

bool Table_DeviceTemplate_pschist::Key_Less::operator()(const Table_DeviceTemplate_pschist::Key &key1, const Table_DeviceTemplate_pschist::Key &key2) const
{
			if (key1.pk_psc_id!=key2.pk_psc_id)
return key1.pk_psc_id<key2.pk_psc_id;
else
if (key1.pk_psc_batch!=key2.pk_psc_batch)
return key1.pk_psc_batch<key2.pk_psc_batch;
else
return false;	
}	

bool Table_DeviceTemplate_pschist::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate_pschist *pRow = (Row_DeviceTemplate_pschist *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DeviceTemplate_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->FK_DeviceCategory_asSQL()+", "+pRow->FK_Manufacturer_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->ImplementsDCE_asSQL()+", "+pRow->IsEmbedded_asSQL()+", "+pRow->CommandLine_asSQL()+", "+pRow->IsPlugAndPlay_asSQL()+", "+pRow->IsIPBased_asSQL()+", "+pRow->IsPlugIn_asSQL()+", "+pRow->IRFrequency_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->DestinationDir_asSQL()+", "+pRow->FK_Users_Maintainer_asSQL()+", "+pRow->FK_OperatingSystem_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->InheritsMacFromPC_asSQL()+", "+pRow->ConfigureScript_asSQL()+", "+pRow->ManufacturerURL_asSQL()+", "+pRow->InternalURLSuffix_asSQL()+", "+pRow->WizardURL_asSQL()+", "+pRow->FK_Licensing_asSQL()+", "+pRow->FK_InfraredGroup_asSQL()+", "+pRow->FK_CommMethod_asSQL()+", "+pRow->FK_PnpLevel_asSQL()+", "+pRow->HelpURL_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_toc_asSQL()+", "+pRow->psc_restrict_asSQL()+", "+pRow->FK_Document_asSQL();

	
		string query = "insert into DeviceTemplate_pschist (`PK_DeviceTemplate`, `Description`, `Comments`, `FK_DeviceCategory`, `FK_Manufacturer`, `Define`, `ImplementsDCE`, `IsEmbedded`, `CommandLine`, `IsPlugAndPlay`, `IsIPBased`, `IsPlugIn`, `IRFrequency`, `FK_StabilityStatus`, `FK_Package`, `DestinationDir`, `FK_Users_Maintainer`, `FK_OperatingSystem`, `FK_Distro`, `InheritsMacFromPC`, `ConfigureScript`, `ManufacturerURL`, `InternalURLSuffix`, `WizardURL`, `FK_Licensing`, `FK_InfraredGroup`, `FK_CommMethod`, `FK_PnpLevel`, `HelpURL`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_toc`, `psc_restrict`, `FK_Document`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DeviceTemplate_pschist* pRow = (Row_DeviceTemplate_pschist*) (*i).second;	
		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DeviceTemplate`="+pRow->PK_DeviceTemplate_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Comments`="+pRow->Comments_asSQL()+", `FK_DeviceCategory`="+pRow->FK_DeviceCategory_asSQL()+", `FK_Manufacturer`="+pRow->FK_Manufacturer_asSQL()+", `Define`="+pRow->Define_asSQL()+", `ImplementsDCE`="+pRow->ImplementsDCE_asSQL()+", `IsEmbedded`="+pRow->IsEmbedded_asSQL()+", `CommandLine`="+pRow->CommandLine_asSQL()+", `IsPlugAndPlay`="+pRow->IsPlugAndPlay_asSQL()+", `IsIPBased`="+pRow->IsIPBased_asSQL()+", `IsPlugIn`="+pRow->IsPlugIn_asSQL()+", `IRFrequency`="+pRow->IRFrequency_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `FK_Package`="+pRow->FK_Package_asSQL()+", `DestinationDir`="+pRow->DestinationDir_asSQL()+", `FK_Users_Maintainer`="+pRow->FK_Users_Maintainer_asSQL()+", `FK_OperatingSystem`="+pRow->FK_OperatingSystem_asSQL()+", `FK_Distro`="+pRow->FK_Distro_asSQL()+", `InheritsMacFromPC`="+pRow->InheritsMacFromPC_asSQL()+", `ConfigureScript`="+pRow->ConfigureScript_asSQL()+", `ManufacturerURL`="+pRow->ManufacturerURL_asSQL()+", `InternalURLSuffix`="+pRow->InternalURLSuffix_asSQL()+", `WizardURL`="+pRow->WizardURL_asSQL()+", `FK_Licensing`="+pRow->FK_Licensing_asSQL()+", `FK_InfraredGroup`="+pRow->FK_InfraredGroup_asSQL()+", `FK_CommMethod`="+pRow->FK_CommMethod_asSQL()+", `FK_PnpLevel`="+pRow->FK_PnpLevel_asSQL()+", `HelpURL`="+pRow->HelpURL_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_toc`="+pRow->psc_toc_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL()+", `FK_Document`="+pRow->FK_Document_asSQL();

	
		string query = "update DeviceTemplate_pschist set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DeviceTemplate_pschist* pRow = (Row_DeviceTemplate_pschist*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		DoubleLongKey key = (*i).first;
		Row_DeviceTemplate_pschist* pRow = (Row_DeviceTemplate_pschist*) (*i).second;	

		char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;

	
		string query = "delete from DeviceTemplate_pschist where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DeviceTemplate_pschist*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DeviceTemplate_pschist::GetRows(string where_statement,vector<class Row_DeviceTemplate_pschist*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DeviceTemplate_pschist`.* from DeviceTemplate_pschist " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DeviceTemplate_pschist`.* from DeviceTemplate_pschist where " + where_statement;
	else
		query = "select `DeviceTemplate_pschist`.* from DeviceTemplate_pschist";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DeviceTemplate_pschist *pRow = new Row_DeviceTemplate_pschist(this);
		
		if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate));
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
pRow->m_Comments = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Comments = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Manufacturer));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Define = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ImplementsDCE = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_ImplementsDCE));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsEmbedded = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsEmbedded));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_CommandLine = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_CommandLine = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_IsPlugAndPlay = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_IsPlugAndPlay));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsIPBased = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsIPBased));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsPlugIn = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsPlugIn));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_IRFrequency = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_IRFrequency));
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
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_Package));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_DestinationDir = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_DestinationDir = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Users_Maintainer = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Users_Maintainer));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_Distro));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_InheritsMacFromPC = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_InheritsMacFromPC));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_ConfigureScript = "";
}
else
{
pRow->is_null[20]=false;
pRow->m_ConfigureScript = string(row[20],lengths[20]);
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_ManufacturerURL = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_ManufacturerURL = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_InternalURLSuffix = "";
}
else
{
pRow->is_null[22]=false;
pRow->m_InternalURLSuffix = string(row[22],lengths[22]);
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_WizardURL = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_WizardURL = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_FK_Licensing = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_FK_Licensing));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_FK_InfraredGroup = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_FK_InfraredGroup));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_FK_CommMethod = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_FK_CommMethod));
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_FK_PnpLevel = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%hi", &(pRow->m_FK_PnpLevel));
}

if (row[28] == NULL)
{
pRow->is_null[28]=true;
pRow->m_HelpURL = "";
}
else
{
pRow->is_null[28]=false;
pRow->m_HelpURL = string(row[28],lengths[28]);
}

if (row[29] == NULL)
{
pRow->is_null[29]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[29]=false;
sscanf(row[29], "%li", &(pRow->m_psc_id));
}

if (row[30] == NULL)
{
pRow->is_null[30]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[30]=false;
sscanf(row[30], "%li", &(pRow->m_psc_batch));
}

if (row[31] == NULL)
{
pRow->is_null[31]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[31]=false;
sscanf(row[31], "%li", &(pRow->m_psc_user));
}

if (row[32] == NULL)
{
pRow->is_null[32]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[32]=false;
sscanf(row[32], "%hi", &(pRow->m_psc_frozen));
}

if (row[33] == NULL)
{
pRow->is_null[33]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[33]=false;
pRow->m_psc_mod = string(row[33],lengths[33]);
}

if (row[34] == NULL)
{
pRow->is_null[34]=true;
pRow->m_psc_toc = 0;
}
else
{
pRow->is_null[34]=false;
sscanf(row[34], "%hi", &(pRow->m_psc_toc));
}

if (row[35] == NULL)
{
pRow->is_null[35]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[35]=false;
sscanf(row[35], "%li", &(pRow->m_psc_restrict));
}

if (row[36] == NULL)
{
pRow->is_null[36]=true;
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[36]=false;
sscanf(row[36], "%li", &(pRow->m_FK_Document));
}



		//checking for duplicates

		DoubleLongKey key(pRow->m_psc_id,pRow->m_psc_batch);
		
		map<DoubleLongKey, class TableRow*, DoubleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DeviceTemplate_pschist *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DeviceTemplate_pschist* Table_DeviceTemplate_pschist::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DeviceTemplate_pschist *pRow = new Row_DeviceTemplate_pschist(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate_pschist* Table_DeviceTemplate_pschist::GetRow(long int in_psc_id, long int in_psc_batch)
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
		return (Row_DeviceTemplate_pschist*) (*i).second;
	//we have to fetch row
	Row_DeviceTemplate_pschist* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate_pschist* Table_DeviceTemplate_pschist::FetchRow(DoubleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_psc_id[32];
sprintf(tmp_psc_id, "%li", key.pk1);

char tmp_psc_batch[32];
sprintf(tmp_psc_batch, "%li", key.pk2);


string condition;
condition = condition + "`psc_id`=" + tmp_psc_id+" AND "+"`psc_batch`=" + tmp_psc_batch;


	string query = "select * from DeviceTemplate_pschist where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate_pschist::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DeviceTemplate_pschist *pRow = new Row_DeviceTemplate_pschist(this);
		
	if (row[0] == NULL)
{
pRow->is_null[0]=true;
pRow->m_PK_DeviceTemplate = 0;
}
else
{
pRow->is_null[0]=false;
sscanf(row[0], "%li", &(pRow->m_PK_DeviceTemplate));
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
pRow->m_Comments = "";
}
else
{
pRow->is_null[2]=false;
pRow->m_Comments = string(row[2],lengths[2]);
}

if (row[3] == NULL)
{
pRow->is_null[3]=true;
pRow->m_FK_DeviceCategory = 0;
}
else
{
pRow->is_null[3]=false;
sscanf(row[3], "%li", &(pRow->m_FK_DeviceCategory));
}

if (row[4] == NULL)
{
pRow->is_null[4]=true;
pRow->m_FK_Manufacturer = 0;
}
else
{
pRow->is_null[4]=false;
sscanf(row[4], "%li", &(pRow->m_FK_Manufacturer));
}

if (row[5] == NULL)
{
pRow->is_null[5]=true;
pRow->m_Define = "";
}
else
{
pRow->is_null[5]=false;
pRow->m_Define = string(row[5],lengths[5]);
}

if (row[6] == NULL)
{
pRow->is_null[6]=true;
pRow->m_ImplementsDCE = 0;
}
else
{
pRow->is_null[6]=false;
sscanf(row[6], "%hi", &(pRow->m_ImplementsDCE));
}

if (row[7] == NULL)
{
pRow->is_null[7]=true;
pRow->m_IsEmbedded = 0;
}
else
{
pRow->is_null[7]=false;
sscanf(row[7], "%hi", &(pRow->m_IsEmbedded));
}

if (row[8] == NULL)
{
pRow->is_null[8]=true;
pRow->m_CommandLine = "";
}
else
{
pRow->is_null[8]=false;
pRow->m_CommandLine = string(row[8],lengths[8]);
}

if (row[9] == NULL)
{
pRow->is_null[9]=true;
pRow->m_IsPlugAndPlay = 0;
}
else
{
pRow->is_null[9]=false;
sscanf(row[9], "%hi", &(pRow->m_IsPlugAndPlay));
}

if (row[10] == NULL)
{
pRow->is_null[10]=true;
pRow->m_IsIPBased = 0;
}
else
{
pRow->is_null[10]=false;
sscanf(row[10], "%hi", &(pRow->m_IsIPBased));
}

if (row[11] == NULL)
{
pRow->is_null[11]=true;
pRow->m_IsPlugIn = 0;
}
else
{
pRow->is_null[11]=false;
sscanf(row[11], "%hi", &(pRow->m_IsPlugIn));
}

if (row[12] == NULL)
{
pRow->is_null[12]=true;
pRow->m_IRFrequency = 0;
}
else
{
pRow->is_null[12]=false;
sscanf(row[12], "%li", &(pRow->m_IRFrequency));
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
pRow->m_FK_Package = 0;
}
else
{
pRow->is_null[14]=false;
sscanf(row[14], "%li", &(pRow->m_FK_Package));
}

if (row[15] == NULL)
{
pRow->is_null[15]=true;
pRow->m_DestinationDir = "";
}
else
{
pRow->is_null[15]=false;
pRow->m_DestinationDir = string(row[15],lengths[15]);
}

if (row[16] == NULL)
{
pRow->is_null[16]=true;
pRow->m_FK_Users_Maintainer = 0;
}
else
{
pRow->is_null[16]=false;
sscanf(row[16], "%li", &(pRow->m_FK_Users_Maintainer));
}

if (row[17] == NULL)
{
pRow->is_null[17]=true;
pRow->m_FK_OperatingSystem = 0;
}
else
{
pRow->is_null[17]=false;
sscanf(row[17], "%li", &(pRow->m_FK_OperatingSystem));
}

if (row[18] == NULL)
{
pRow->is_null[18]=true;
pRow->m_FK_Distro = 0;
}
else
{
pRow->is_null[18]=false;
sscanf(row[18], "%li", &(pRow->m_FK_Distro));
}

if (row[19] == NULL)
{
pRow->is_null[19]=true;
pRow->m_InheritsMacFromPC = 0;
}
else
{
pRow->is_null[19]=false;
sscanf(row[19], "%hi", &(pRow->m_InheritsMacFromPC));
}

if (row[20] == NULL)
{
pRow->is_null[20]=true;
pRow->m_ConfigureScript = "";
}
else
{
pRow->is_null[20]=false;
pRow->m_ConfigureScript = string(row[20],lengths[20]);
}

if (row[21] == NULL)
{
pRow->is_null[21]=true;
pRow->m_ManufacturerURL = "";
}
else
{
pRow->is_null[21]=false;
pRow->m_ManufacturerURL = string(row[21],lengths[21]);
}

if (row[22] == NULL)
{
pRow->is_null[22]=true;
pRow->m_InternalURLSuffix = "";
}
else
{
pRow->is_null[22]=false;
pRow->m_InternalURLSuffix = string(row[22],lengths[22]);
}

if (row[23] == NULL)
{
pRow->is_null[23]=true;
pRow->m_WizardURL = "";
}
else
{
pRow->is_null[23]=false;
pRow->m_WizardURL = string(row[23],lengths[23]);
}

if (row[24] == NULL)
{
pRow->is_null[24]=true;
pRow->m_FK_Licensing = 0;
}
else
{
pRow->is_null[24]=false;
sscanf(row[24], "%li", &(pRow->m_FK_Licensing));
}

if (row[25] == NULL)
{
pRow->is_null[25]=true;
pRow->m_FK_InfraredGroup = 0;
}
else
{
pRow->is_null[25]=false;
sscanf(row[25], "%li", &(pRow->m_FK_InfraredGroup));
}

if (row[26] == NULL)
{
pRow->is_null[26]=true;
pRow->m_FK_CommMethod = 0;
}
else
{
pRow->is_null[26]=false;
sscanf(row[26], "%li", &(pRow->m_FK_CommMethod));
}

if (row[27] == NULL)
{
pRow->is_null[27]=true;
pRow->m_FK_PnpLevel = 0;
}
else
{
pRow->is_null[27]=false;
sscanf(row[27], "%hi", &(pRow->m_FK_PnpLevel));
}

if (row[28] == NULL)
{
pRow->is_null[28]=true;
pRow->m_HelpURL = "";
}
else
{
pRow->is_null[28]=false;
pRow->m_HelpURL = string(row[28],lengths[28]);
}

if (row[29] == NULL)
{
pRow->is_null[29]=true;
pRow->m_psc_id = 0;
}
else
{
pRow->is_null[29]=false;
sscanf(row[29], "%li", &(pRow->m_psc_id));
}

if (row[30] == NULL)
{
pRow->is_null[30]=true;
pRow->m_psc_batch = 0;
}
else
{
pRow->is_null[30]=false;
sscanf(row[30], "%li", &(pRow->m_psc_batch));
}

if (row[31] == NULL)
{
pRow->is_null[31]=true;
pRow->m_psc_user = 0;
}
else
{
pRow->is_null[31]=false;
sscanf(row[31], "%li", &(pRow->m_psc_user));
}

if (row[32] == NULL)
{
pRow->is_null[32]=true;
pRow->m_psc_frozen = 0;
}
else
{
pRow->is_null[32]=false;
sscanf(row[32], "%hi", &(pRow->m_psc_frozen));
}

if (row[33] == NULL)
{
pRow->is_null[33]=true;
pRow->m_psc_mod = "";
}
else
{
pRow->is_null[33]=false;
pRow->m_psc_mod = string(row[33],lengths[33]);
}

if (row[34] == NULL)
{
pRow->is_null[34]=true;
pRow->m_psc_toc = 0;
}
else
{
pRow->is_null[34]=false;
sscanf(row[34], "%hi", &(pRow->m_psc_toc));
}

if (row[35] == NULL)
{
pRow->is_null[35]=true;
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[35]=false;
sscanf(row[35], "%li", &(pRow->m_psc_restrict));
}

if (row[36] == NULL)
{
pRow->is_null[36]=true;
pRow->m_FK_Document = 0;
}
else
{
pRow->is_null[36]=false;
sscanf(row[36], "%li", &(pRow->m_FK_Document));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DeviceCategory* Row_DeviceTemplate_pschist::FK_DeviceCategory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceCategory *pTable = table->database->DeviceCategory_get();
return pTable->GetRow(m_FK_DeviceCategory);
}
class Row_Manufacturer* Row_DeviceTemplate_pschist::FK_Manufacturer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Manufacturer *pTable = table->database->Manufacturer_get();
return pTable->GetRow(m_FK_Manufacturer);
}
class Row_StabilityStatus* Row_DeviceTemplate_pschist::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_Package* Row_DeviceTemplate_pschist::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Users* Row_DeviceTemplate_pschist::FK_Users_Maintainer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Users *pTable = table->database->Users_get();
return pTable->GetRow(m_FK_Users_Maintainer);
}
class Row_OperatingSystem* Row_DeviceTemplate_pschist::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}
class Row_Distro* Row_DeviceTemplate_pschist::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}
class Row_Licensing* Row_DeviceTemplate_pschist::FK_Licensing_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Licensing *pTable = table->database->Licensing_get();
return pTable->GetRow(m_FK_Licensing);
}
class Row_InfraredGroup* Row_DeviceTemplate_pschist::FK_InfraredGroup_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_InfraredGroup *pTable = table->database->InfraredGroup_get();
return pTable->GetRow(m_FK_InfraredGroup);
}
class Row_CommMethod* Row_DeviceTemplate_pschist::FK_CommMethod_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommMethod *pTable = table->database->CommMethod_get();
return pTable->GetRow(m_FK_CommMethod);
}
class Row_PnpLevel* Row_DeviceTemplate_pschist::FK_PnpLevel_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PnpLevel *pTable = table->database->PnpLevel_get();
return pTable->GetRow(m_FK_PnpLevel);
}
class Row_Document* Row_DeviceTemplate_pschist::FK_Document_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Document *pTable = table->database->Document_get();
return pTable->GetRow(m_FK_Document);
}





