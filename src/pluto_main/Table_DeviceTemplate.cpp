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
#include "Table_DeviceTemplate.h"
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

#include "Table_CommandGroup_D_Command.h"
#include "Table_ConfigType.h"
#include "Table_DHCPDevice.h"
#include "Table_Device.h"
#include "Table_DeviceTemplate_AV.h"
#include "Table_DeviceTemplate_DSPMode.h"
#include "Table_DeviceTemplate_DesignObj.h"
#include "Table_DeviceTemplate_DeviceCategory_ControlledVia.h"
#include "Table_DeviceTemplate_DeviceCommandGroup.h"
#include "Table_DeviceTemplate_DeviceData.h"
#include "Table_DeviceTemplate_DeviceTemplate_ControlledVia.h"
#include "Table_DeviceTemplate_DeviceTemplate_ControlledVia.h"
#include "Table_DeviceTemplate_DeviceTemplate_Related.h"
#include "Table_DeviceTemplate_DeviceTemplate_Related.h"
#include "Table_DeviceTemplate_Event.h"
#include "Table_DeviceTemplate_InfraredGroup.h"
#include "Table_DeviceTemplate_Input.h"
#include "Table_DeviceTemplate_MediaType.h"
#include "Table_DeviceTemplate_Output.h"
#include "Table_DeviceTemplate_PageSetup.h"
#include "Table_InfraredGroup_Command.h"
#include "Table_InstallWizard.h"
#include "Table_InstallWizard_Distro.h"
#include "Table_PnpQueue.h"
#include "Table_Screen_DesignObj.h"
#include "Table_StartupScript.h"


void Database_pluto_main::CreateTable_DeviceTemplate()
{
	tblDeviceTemplate = new Table_DeviceTemplate(this);
}

void Database_pluto_main::DeleteTable_DeviceTemplate()
{
	if( tblDeviceTemplate )
		delete tblDeviceTemplate;
}

bool Database_pluto_main::Commit_DeviceTemplate(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	return tblDeviceTemplate->Commit(bDeleteFailedModifiedRow,bDeleteFailedInsertRow);
}

Table_DeviceTemplate::~Table_DeviceTemplate()
{
	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator it;
	for(it=cachedRows.begin();it!=cachedRows.end();++it)
	{
		Row_DeviceTemplate *pRow = (Row_DeviceTemplate *) (*it).second;
		delete pRow;
	}

	for(it=deleted_cachedRows.begin();it!=deleted_cachedRows.end();++it)
	{
		Row_DeviceTemplate *pRow = (Row_DeviceTemplate *) (*it).second;
		delete pRow;
	}

	size_t i;
	for(i=0;i<addedRows.size();++i)
		delete addedRows[i];
	for(i=0;i<deleted_addedRows.size();++i)
		delete deleted_addedRows[i];
}


void Row_DeviceTemplate::Delete()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	Row_DeviceTemplate *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row
	
	if (!is_deleted)
	{
		if (is_added)	
		{	
			vector<TableRow*>::iterator i;	
			for (i = table->addedRows.begin(); (i!=table->addedRows.end()) && ( (Row_DeviceTemplate *) *i != this); i++);
			
			if (i!=	table->addedRows.end())
				table->addedRows.erase(i);
		
			table->deleted_addedRows.push_back(this);
			is_deleted = true;	
		}
		else
		{
			SingleLongKey key(pRow->m_PK_DeviceTemplate);
			map<SingleLongKey, TableRow*, SingleLongKey_Less>::iterator i = table->cachedRows.find(key);
			if (i!=table->cachedRows.end())
				table->cachedRows.erase(i);
						
			table->deleted_cachedRows[key] = this;
			is_deleted = true;	
		}	
	}
}

void Row_DeviceTemplate::Reload()
{
	Row_DeviceTemplate *pRow = this; // Needed so we will have only 1 version of get_primary_fields_assign_from_row

	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
	
	
	if (!is_added)
	{
		SingleLongKey key(pRow->m_PK_DeviceTemplate);
		Row_DeviceTemplate *pRow = table->FetchRow(key);
		
		if (pRow!=NULL)
		{
			*this = *pRow;	
			
			delete pRow;		
		}	
	}	
	
}

Row_DeviceTemplate::Row_DeviceTemplate(Table_DeviceTemplate *pTable):table(pTable)
{
	SetDefaultValues();
}

void Row_DeviceTemplate::SetDefaultValues()
{
	m_PK_DeviceTemplate = 0;
is_null[0] = false;
m_Description = "";
is_null[1] = false;
is_null[2] = true;
m_FK_DeviceCategory = 0;
is_null[3] = false;
is_null[4] = true;
m_FK_Manufacturer = 0;
m_Define = "";
is_null[5] = false;
m_ImplementsDCE = 0;
is_null[6] = false;
m_IsEmbedded = 0;
is_null[7] = false;
is_null[8] = true;
m_IsPlugAndPlay = 0;
is_null[9] = false;
m_IsIPBased = 0;
is_null[10] = false;
m_IsPlugIn = 0;
is_null[11] = false;
is_null[12] = true;
m_IRFrequency = 0;
is_null[13] = true;
m_FK_StabilityStatus = 0;
is_null[14] = true;
m_FK_Package = 0;
is_null[15] = true;
m_FK_Users_Maintainer = 1;
is_null[16] = false;
is_null[17] = true;
m_FK_OperatingSystem = 0;
is_null[18] = true;
m_FK_Distro = 0;
m_InheritsMacFromPC = 0;
is_null[19] = false;
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
is_null[29] = true;
m_psc_id = 0;
is_null[30] = true;
m_psc_batch = 0;
is_null[31] = true;
m_psc_user = 0;
m_psc_frozen = 0;
is_null[32] = false;
m_psc_mod = "0000-00-00 00:00:00";
is_null[33] = false;
is_null[34] = true;
m_psc_restrict = 0;


	is_added=false;
	is_deleted=false;
	is_modified=false;
}

long int Row_DeviceTemplate::PK_DeviceTemplate_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_PK_DeviceTemplate;}
string Row_DeviceTemplate::Description_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Description;}
string Row_DeviceTemplate::Comments_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Comments;}
long int Row_DeviceTemplate::FK_DeviceCategory_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_DeviceCategory;}
long int Row_DeviceTemplate::FK_Manufacturer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Manufacturer;}
string Row_DeviceTemplate::Define_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_Define;}
short int Row_DeviceTemplate::ImplementsDCE_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ImplementsDCE;}
short int Row_DeviceTemplate::IsEmbedded_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsEmbedded;}
string Row_DeviceTemplate::CommandLine_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_CommandLine;}
short int Row_DeviceTemplate::IsPlugAndPlay_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsPlugAndPlay;}
short int Row_DeviceTemplate::IsIPBased_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsIPBased;}
short int Row_DeviceTemplate::IsPlugIn_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IsPlugIn;}
long int Row_DeviceTemplate::IRFrequency_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_IRFrequency;}
long int Row_DeviceTemplate::FK_StabilityStatus_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_StabilityStatus;}
long int Row_DeviceTemplate::FK_Package_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Package;}
string Row_DeviceTemplate::DestinationDir_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_DestinationDir;}
long int Row_DeviceTemplate::FK_Users_Maintainer_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Users_Maintainer;}
long int Row_DeviceTemplate::FK_OperatingSystem_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_OperatingSystem;}
long int Row_DeviceTemplate::FK_Distro_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Distro;}
short int Row_DeviceTemplate::InheritsMacFromPC_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_InheritsMacFromPC;}
string Row_DeviceTemplate::ConfigureScript_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ConfigureScript;}
string Row_DeviceTemplate::ManufacturerURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_ManufacturerURL;}
string Row_DeviceTemplate::InternalURLSuffix_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_InternalURLSuffix;}
string Row_DeviceTemplate::WizardURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_WizardURL;}
long int Row_DeviceTemplate::FK_Licensing_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_Licensing;}
long int Row_DeviceTemplate::FK_InfraredGroup_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_InfraredGroup;}
long int Row_DeviceTemplate::FK_CommMethod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_CommMethod;}
long int Row_DeviceTemplate::FK_PnpLevel_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_FK_PnpLevel;}
string Row_DeviceTemplate::HelpURL_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_HelpURL;}
long int Row_DeviceTemplate::psc_id_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_id;}
long int Row_DeviceTemplate::psc_batch_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_batch;}
long int Row_DeviceTemplate::psc_user_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_user;}
short int Row_DeviceTemplate::psc_frozen_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_frozen;}
string Row_DeviceTemplate::psc_mod_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_mod;}
long int Row_DeviceTemplate::psc_restrict_get(){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return m_psc_restrict;}

		
void Row_DeviceTemplate::PK_DeviceTemplate_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_PK_DeviceTemplate = val; is_modified=true; is_null[0]=false;}
void Row_DeviceTemplate::Description_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Description = val; is_modified=true; is_null[1]=false;}
void Row_DeviceTemplate::Comments_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Comments = val; is_modified=true; is_null[2]=false;}
void Row_DeviceTemplate::FK_DeviceCategory_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_DeviceCategory = val; is_modified=true; is_null[3]=false;}
void Row_DeviceTemplate::FK_Manufacturer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Manufacturer = val; is_modified=true; is_null[4]=false;}
void Row_DeviceTemplate::Define_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_Define = val; is_modified=true; is_null[5]=false;}
void Row_DeviceTemplate::ImplementsDCE_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ImplementsDCE = val; is_modified=true; is_null[6]=false;}
void Row_DeviceTemplate::IsEmbedded_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsEmbedded = val; is_modified=true; is_null[7]=false;}
void Row_DeviceTemplate::CommandLine_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_CommandLine = val; is_modified=true; is_null[8]=false;}
void Row_DeviceTemplate::IsPlugAndPlay_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsPlugAndPlay = val; is_modified=true; is_null[9]=false;}
void Row_DeviceTemplate::IsIPBased_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsIPBased = val; is_modified=true; is_null[10]=false;}
void Row_DeviceTemplate::IsPlugIn_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IsPlugIn = val; is_modified=true; is_null[11]=false;}
void Row_DeviceTemplate::IRFrequency_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_IRFrequency = val; is_modified=true; is_null[12]=false;}
void Row_DeviceTemplate::FK_StabilityStatus_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_StabilityStatus = val; is_modified=true; is_null[13]=false;}
void Row_DeviceTemplate::FK_Package_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Package = val; is_modified=true; is_null[14]=false;}
void Row_DeviceTemplate::DestinationDir_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_DestinationDir = val; is_modified=true; is_null[15]=false;}
void Row_DeviceTemplate::FK_Users_Maintainer_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Users_Maintainer = val; is_modified=true; is_null[16]=false;}
void Row_DeviceTemplate::FK_OperatingSystem_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_OperatingSystem = val; is_modified=true; is_null[17]=false;}
void Row_DeviceTemplate::FK_Distro_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Distro = val; is_modified=true; is_null[18]=false;}
void Row_DeviceTemplate::InheritsMacFromPC_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_InheritsMacFromPC = val; is_modified=true; is_null[19]=false;}
void Row_DeviceTemplate::ConfigureScript_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ConfigureScript = val; is_modified=true; is_null[20]=false;}
void Row_DeviceTemplate::ManufacturerURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_ManufacturerURL = val; is_modified=true; is_null[21]=false;}
void Row_DeviceTemplate::InternalURLSuffix_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_InternalURLSuffix = val; is_modified=true; is_null[22]=false;}
void Row_DeviceTemplate::WizardURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_WizardURL = val; is_modified=true; is_null[23]=false;}
void Row_DeviceTemplate::FK_Licensing_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_Licensing = val; is_modified=true; is_null[24]=false;}
void Row_DeviceTemplate::FK_InfraredGroup_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_InfraredGroup = val; is_modified=true; is_null[25]=false;}
void Row_DeviceTemplate::FK_CommMethod_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_CommMethod = val; is_modified=true; is_null[26]=false;}
void Row_DeviceTemplate::FK_PnpLevel_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_FK_PnpLevel = val; is_modified=true; is_null[27]=false;}
void Row_DeviceTemplate::HelpURL_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_HelpURL = val; is_modified=true; is_null[28]=false;}
void Row_DeviceTemplate::psc_id_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_id = val; is_modified=true; is_null[29]=false;}
void Row_DeviceTemplate::psc_batch_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_batch = val; is_modified=true; is_null[30]=false;}
void Row_DeviceTemplate::psc_user_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_user = val; is_modified=true; is_null[31]=false;}
void Row_DeviceTemplate::psc_frozen_set(short int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_frozen = val; is_modified=true; is_null[32]=false;}
void Row_DeviceTemplate::psc_mod_set(string val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_mod = val; is_modified=true; is_null[33]=false;}
void Row_DeviceTemplate::psc_restrict_set(long int val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

m_psc_restrict = val; is_modified=true; is_null[34]=false;}

		
bool Row_DeviceTemplate::Comments_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[2];}
bool Row_DeviceTemplate::FK_Manufacturer_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[4];}
bool Row_DeviceTemplate::IsEmbedded_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[7];}
bool Row_DeviceTemplate::CommandLine_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[8];}
bool Row_DeviceTemplate::IRFrequency_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[12];}
bool Row_DeviceTemplate::FK_StabilityStatus_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[13];}
bool Row_DeviceTemplate::FK_Package_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[14];}
bool Row_DeviceTemplate::DestinationDir_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[15];}
bool Row_DeviceTemplate::FK_OperatingSystem_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[17];}
bool Row_DeviceTemplate::FK_Distro_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[18];}
bool Row_DeviceTemplate::InheritsMacFromPC_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[19];}
bool Row_DeviceTemplate::ConfigureScript_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[20];}
bool Row_DeviceTemplate::ManufacturerURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[21];}
bool Row_DeviceTemplate::InternalURLSuffix_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[22];}
bool Row_DeviceTemplate::WizardURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[23];}
bool Row_DeviceTemplate::FK_Licensing_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[24];}
bool Row_DeviceTemplate::FK_InfraredGroup_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[25];}
bool Row_DeviceTemplate::FK_CommMethod_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[26];}
bool Row_DeviceTemplate::FK_PnpLevel_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[27];}
bool Row_DeviceTemplate::HelpURL_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[28];}
bool Row_DeviceTemplate::psc_id_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[29];}
bool Row_DeviceTemplate::psc_batch_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[30];}
bool Row_DeviceTemplate::psc_user_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[31];}
bool Row_DeviceTemplate::psc_frozen_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[32];}
bool Row_DeviceTemplate::psc_restrict_isNull() {PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

return is_null[34];}

			
void Row_DeviceTemplate::Comments_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[2]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_Manufacturer_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[4]=val;
is_modified=true;
}
void Row_DeviceTemplate::IsEmbedded_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[7]=val;
is_modified=true;
}
void Row_DeviceTemplate::CommandLine_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[8]=val;
is_modified=true;
}
void Row_DeviceTemplate::IRFrequency_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[12]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_StabilityStatus_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[13]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_Package_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[14]=val;
is_modified=true;
}
void Row_DeviceTemplate::DestinationDir_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[15]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_OperatingSystem_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[17]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_Distro_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[18]=val;
is_modified=true;
}
void Row_DeviceTemplate::InheritsMacFromPC_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[19]=val;
is_modified=true;
}
void Row_DeviceTemplate::ConfigureScript_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[20]=val;
is_modified=true;
}
void Row_DeviceTemplate::ManufacturerURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[21]=val;
is_modified=true;
}
void Row_DeviceTemplate::InternalURLSuffix_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[22]=val;
is_modified=true;
}
void Row_DeviceTemplate::WizardURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[23]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_Licensing_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[24]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_InfraredGroup_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[25]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_CommMethod_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[26]=val;
is_modified=true;
}
void Row_DeviceTemplate::FK_PnpLevel_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[27]=val;
is_modified=true;
}
void Row_DeviceTemplate::HelpURL_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[28]=val;
is_modified=true;
}
void Row_DeviceTemplate::psc_id_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[29]=val;
is_modified=true;
}
void Row_DeviceTemplate::psc_batch_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[30]=val;
is_modified=true;
}
void Row_DeviceTemplate::psc_user_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[31]=val;
is_modified=true;
}
void Row_DeviceTemplate::psc_frozen_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[32]=val;
is_modified=true;
}
void Row_DeviceTemplate::psc_restrict_setNull(bool val){PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);
is_null[34]=val;
is_modified=true;
}
	

string Row_DeviceTemplate::PK_DeviceTemplate_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[0])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_PK_DeviceTemplate);

return buf;
}

string Row_DeviceTemplate::Description_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[1])
return "NULL";

char *buf = new char[181];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Description.c_str(), (unsigned long) min((size_t)90,m_Description.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::Comments_asSQL()
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

string Row_DeviceTemplate::FK_DeviceCategory_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[3])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_DeviceCategory);

return buf;
}

string Row_DeviceTemplate::FK_Manufacturer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[4])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Manufacturer);

return buf;
}

string Row_DeviceTemplate::Define_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[5])
return "NULL";

char *buf = new char[181];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_Define.c_str(), (unsigned long) min((size_t)90,m_Define.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::ImplementsDCE_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[6])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_ImplementsDCE);

return buf;
}

string Row_DeviceTemplate::IsEmbedded_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[7])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsEmbedded);

return buf;
}

string Row_DeviceTemplate::CommandLine_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[8])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_CommandLine.c_str(), (unsigned long) min((size_t)300,m_CommandLine.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::IsPlugAndPlay_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[9])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsPlugAndPlay);

return buf;
}

string Row_DeviceTemplate::IsIPBased_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[10])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsIPBased);

return buf;
}

string Row_DeviceTemplate::IsPlugIn_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[11])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_IsPlugIn);

return buf;
}

string Row_DeviceTemplate::IRFrequency_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[12])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_IRFrequency);

return buf;
}

string Row_DeviceTemplate::FK_StabilityStatus_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[13])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_StabilityStatus);

return buf;
}

string Row_DeviceTemplate::FK_Package_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[14])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Package);

return buf;
}

string Row_DeviceTemplate::DestinationDir_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[15])
return "NULL";

char *buf = new char[301];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_DestinationDir.c_str(), (unsigned long) min((size_t)150,m_DestinationDir.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::FK_Users_Maintainer_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[16])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Users_Maintainer);

return buf;
}

string Row_DeviceTemplate::FK_OperatingSystem_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[17])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_OperatingSystem);

return buf;
}

string Row_DeviceTemplate::FK_Distro_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[18])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Distro);

return buf;
}

string Row_DeviceTemplate::InheritsMacFromPC_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[19])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_InheritsMacFromPC);

return buf;
}

string Row_DeviceTemplate::ConfigureScript_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[20])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ConfigureScript.c_str(), (unsigned long) min((size_t)300,m_ConfigureScript.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::ManufacturerURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[21])
return "NULL";

char *buf = new char[601];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_ManufacturerURL.c_str(), (unsigned long) min((size_t)300,m_ManufacturerURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::InternalURLSuffix_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[22])
return "NULL";

char *buf = new char[301];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_InternalURLSuffix.c_str(), (unsigned long) min((size_t)150,m_InternalURLSuffix.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::WizardURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[23])
return "NULL";

char *buf = new char[301];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_WizardURL.c_str(), (unsigned long) min((size_t)150,m_WizardURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::FK_Licensing_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[24])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_Licensing);

return buf;
}

string Row_DeviceTemplate::FK_InfraredGroup_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[25])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_InfraredGroup);

return buf;
}

string Row_DeviceTemplate::FK_CommMethod_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[26])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_CommMethod);

return buf;
}

string Row_DeviceTemplate::FK_PnpLevel_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[27])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_FK_PnpLevel);

return buf;
}

string Row_DeviceTemplate::HelpURL_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[28])
return "NULL";

char *buf = new char[1531];
db_wrapper_real_escape_string(table->database->m_pDB, buf, m_HelpURL.c_str(), (unsigned long) min((size_t)765,m_HelpURL.size()));
string s=string()+"\""+buf+"\"";
delete[] buf;
return s;
}

string Row_DeviceTemplate::psc_id_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[29])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_id);

return buf;
}

string Row_DeviceTemplate::psc_batch_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[30])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_batch);

return buf;
}

string Row_DeviceTemplate::psc_user_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[31])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_user);

return buf;
}

string Row_DeviceTemplate::psc_frozen_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[32])
return "NULL";

char buf[32];
sprintf(buf, "%hi", m_psc_frozen);

return buf;
}

string Row_DeviceTemplate::psc_mod_asSQL()
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

string Row_DeviceTemplate::psc_restrict_asSQL()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

if (is_null[34])
return "NULL";

char buf[32];
sprintf(buf, "%li", m_psc_restrict);

return buf;
}




Table_DeviceTemplate::Key::Key(long int in_PK_DeviceTemplate)
{
			pk_PK_DeviceTemplate = in_PK_DeviceTemplate;
	
}

Table_DeviceTemplate::Key::Key(Row_DeviceTemplate *pRow)
{
			PLUTO_SAFETY_LOCK_ERRORSONLY(sl,pRow->table->database->m_DBMutex);

			pk_PK_DeviceTemplate = pRow->m_PK_DeviceTemplate;
	
}		

bool Table_DeviceTemplate::Key_Less::operator()(const Table_DeviceTemplate::Key &key1, const Table_DeviceTemplate::Key &key2) const
{
			if (key1.pk_PK_DeviceTemplate!=key2.pk_PK_DeviceTemplate)
return key1.pk_PK_DeviceTemplate<key2.pk_PK_DeviceTemplate;
else
return false;	
}	

bool Table_DeviceTemplate::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
	bool bSuccessful=true;
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

//insert added
	while (!addedRows.empty())
	{
		vector<TableRow*>::iterator i = addedRows.begin();
	
		Row_DeviceTemplate *pRow = (Row_DeviceTemplate *)*i;
	
		
string values_list_comma_separated;
values_list_comma_separated = values_list_comma_separated + pRow->PK_DeviceTemplate_asSQL()+", "+pRow->Description_asSQL()+", "+pRow->Comments_asSQL()+", "+pRow->FK_DeviceCategory_asSQL()+", "+pRow->FK_Manufacturer_asSQL()+", "+pRow->Define_asSQL()+", "+pRow->ImplementsDCE_asSQL()+", "+pRow->IsEmbedded_asSQL()+", "+pRow->CommandLine_asSQL()+", "+pRow->IsPlugAndPlay_asSQL()+", "+pRow->IsIPBased_asSQL()+", "+pRow->IsPlugIn_asSQL()+", "+pRow->IRFrequency_asSQL()+", "+pRow->FK_StabilityStatus_asSQL()+", "+pRow->FK_Package_asSQL()+", "+pRow->DestinationDir_asSQL()+", "+pRow->FK_Users_Maintainer_asSQL()+", "+pRow->FK_OperatingSystem_asSQL()+", "+pRow->FK_Distro_asSQL()+", "+pRow->InheritsMacFromPC_asSQL()+", "+pRow->ConfigureScript_asSQL()+", "+pRow->ManufacturerURL_asSQL()+", "+pRow->InternalURLSuffix_asSQL()+", "+pRow->WizardURL_asSQL()+", "+pRow->FK_Licensing_asSQL()+", "+pRow->FK_InfraredGroup_asSQL()+", "+pRow->FK_CommMethod_asSQL()+", "+pRow->FK_PnpLevel_asSQL()+", "+pRow->HelpURL_asSQL()+", "+pRow->psc_id_asSQL()+", "+pRow->psc_batch_asSQL()+", "+pRow->psc_user_asSQL()+", "+pRow->psc_frozen_asSQL()+", "+pRow->psc_restrict_asSQL();

	
		string query = "insert into DeviceTemplate (`PK_DeviceTemplate`, `Description`, `Comments`, `FK_DeviceCategory`, `FK_Manufacturer`, `Define`, `ImplementsDCE`, `IsEmbedded`, `CommandLine`, `IsPlugAndPlay`, `IsIPBased`, `IsPlugIn`, `IRFrequency`, `FK_StabilityStatus`, `FK_Package`, `DestinationDir`, `FK_Users_Maintainer`, `FK_OperatingSystem`, `FK_Distro`, `InheritsMacFromPC`, `ConfigureScript`, `ManufacturerURL`, `InternalURLSuffix`, `WizardURL`, `FK_Licensing`, `FK_InfraredGroup`, `FK_CommMethod`, `FK_PnpLevel`, `HelpURL`, `psc_id`, `psc_batch`, `psc_user`, `psc_frozen`, `psc_restrict`) values ("+
			values_list_comma_separated+")";
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());
			
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::Commit Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		pRow->m_PK_DeviceTemplate=id;
else 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PK_DeviceTemplate is auto increment but has no value %s",database->m_sLastDBError.c_str());	
			
			addedRows.erase(i);
			SingleLongKey key(pRow->m_PK_DeviceTemplate);	
			cachedRows[key] = pRow;
					
			
			pRow->is_added = false;	
			pRow->is_modified = false;	
		}	
				
	}	


//update modified
	

	for (map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.begin(); i!= cachedRows.end(); i++)
		if	(((*i).second)->is_modified_get())
	{
		Row_DeviceTemplate* pRow = (Row_DeviceTemplate*) (*i).second;	
		SingleLongKey key(pRow->m_PK_DeviceTemplate);

		char tmp_PK_DeviceTemplate[32];
sprintf(tmp_PK_DeviceTemplate, "%li", key.pk);


string condition;
condition = condition + "`PK_DeviceTemplate`=" + tmp_PK_DeviceTemplate;
	
			
		
string update_values_list;
update_values_list = update_values_list + "`PK_DeviceTemplate`="+pRow->PK_DeviceTemplate_asSQL()+", `Description`="+pRow->Description_asSQL()+", `Comments`="+pRow->Comments_asSQL()+", `FK_DeviceCategory`="+pRow->FK_DeviceCategory_asSQL()+", `FK_Manufacturer`="+pRow->FK_Manufacturer_asSQL()+", `Define`="+pRow->Define_asSQL()+", `ImplementsDCE`="+pRow->ImplementsDCE_asSQL()+", `IsEmbedded`="+pRow->IsEmbedded_asSQL()+", `CommandLine`="+pRow->CommandLine_asSQL()+", `IsPlugAndPlay`="+pRow->IsPlugAndPlay_asSQL()+", `IsIPBased`="+pRow->IsIPBased_asSQL()+", `IsPlugIn`="+pRow->IsPlugIn_asSQL()+", `IRFrequency`="+pRow->IRFrequency_asSQL()+", `FK_StabilityStatus`="+pRow->FK_StabilityStatus_asSQL()+", `FK_Package`="+pRow->FK_Package_asSQL()+", `DestinationDir`="+pRow->DestinationDir_asSQL()+", `FK_Users_Maintainer`="+pRow->FK_Users_Maintainer_asSQL()+", `FK_OperatingSystem`="+pRow->FK_OperatingSystem_asSQL()+", `FK_Distro`="+pRow->FK_Distro_asSQL()+", `InheritsMacFromPC`="+pRow->InheritsMacFromPC_asSQL()+", `ConfigureScript`="+pRow->ConfigureScript_asSQL()+", `ManufacturerURL`="+pRow->ManufacturerURL_asSQL()+", `InternalURLSuffix`="+pRow->InternalURLSuffix_asSQL()+", `WizardURL`="+pRow->WizardURL_asSQL()+", `FK_Licensing`="+pRow->FK_Licensing_asSQL()+", `FK_InfraredGroup`="+pRow->FK_InfraredGroup_asSQL()+", `FK_CommMethod`="+pRow->FK_CommMethod_asSQL()+", `FK_PnpLevel`="+pRow->FK_PnpLevel_asSQL()+", `HelpURL`="+pRow->HelpURL_asSQL()+", `psc_id`="+pRow->psc_id_asSQL()+", `psc_batch`="+pRow->psc_batch_asSQL()+", `psc_user`="+pRow->psc_user_asSQL()+", `psc_frozen`="+pRow->psc_frozen_asSQL()+", `psc_restrict`="+pRow->psc_restrict_asSQL();

	
		string query = "update DeviceTemplate set " + update_values_list + " where " + condition;
			
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::Commit Cannot perform update query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
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
		Row_DeviceTemplate* pRow = (Row_DeviceTemplate*) (*i);
		delete pRow;
		deleted_addedRows.erase(i);
	}	


//delete deleted cached
	
	while (!deleted_cachedRows.empty())
	{	
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = deleted_cachedRows.begin();
	
		SingleLongKey key = (*i).first;
		Row_DeviceTemplate* pRow = (Row_DeviceTemplate*) (*i).second;	

		char tmp_PK_DeviceTemplate[32];
sprintf(tmp_PK_DeviceTemplate, "%li", key.pk);


string condition;
condition = condition + "`PK_DeviceTemplate`=" + tmp_PK_DeviceTemplate;

	
		string query = "delete from DeviceTemplate where " + condition;
		
		if (db_wrapper_query(database->m_pDB, query.c_str()))
		{	
			database->m_sLastDBError = db_wrapper_error(database->m_pDB);
			cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
			bool bResult=database->DBConnect(true);
			int iResult2=-1;
			if( bResult )
				iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::Commit Cannot perform delete query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
			if( iResult2!=0 )  // We can keep going if the time it worked
				return false;
		}	
		
		pRow = (Row_DeviceTemplate*) (*i).second;;
		delete pRow;
		deleted_cachedRows.erase(key);
	}
	
	return bSuccessful;
}

bool Table_DeviceTemplate::GetRows(string where_statement,vector<class Row_DeviceTemplate*> *rows)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	string query;
	if( StringUtils::StartsWith(where_statement,"where ",true) || 
		StringUtils::StartsWith(where_statement,"join ",true) ||
		StringUtils::StartsWith(where_statement,"left ",true) ||
		StringUtils::StartsWith(where_statement,"right ",true) ||
		StringUtils::StartsWith(where_statement,"full ",true) ||
		StringUtils::StartsWith(where_statement,"outer ",true) )
		query = "select `DeviceTemplate`.* from DeviceTemplate " + where_statement;
	else if( StringUtils::StartsWith(where_statement,"select ",true) )
		query = where_statement;
	else if( where_statement.size() )
		query = "select `DeviceTemplate`.* from DeviceTemplate where " + where_statement;
	else
		query = "select `DeviceTemplate`.* from DeviceTemplate";
		
	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::GetRows Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return false;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::GetRows db_wrapper_store_result returned NULL handler");
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		return false;
	}	
	
	DB_ROW row;
						
		
	while ((row = db_wrapper_fetch_row(res)) != NULL)
	{	
		unsigned long *lengths = db_wrapper_fetch_lengths(res);

		Row_DeviceTemplate *pRow = new Row_DeviceTemplate(this);
		
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
sscanf(row[27], "%li", &(pRow->m_FK_PnpLevel));
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
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[34]=false;
sscanf(row[34], "%li", &(pRow->m_psc_restrict));
}



		//checking for duplicates

		SingleLongKey key(pRow->m_PK_DeviceTemplate);
		
		map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i = cachedRows.find(key);
			
		if (i!=cachedRows.end())
		{
			delete pRow;
			pRow = (Row_DeviceTemplate *)(*i).second;
		}

		rows->push_back(pRow);
		
		cachedRows[key] = pRow;
	}

	db_wrapper_free_result(res);			
		
	return true;					
}

Row_DeviceTemplate* Table_DeviceTemplate::AddRow()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	Row_DeviceTemplate *pRow = new Row_DeviceTemplate(this);
	pRow->is_added=true;
	addedRows.push_back(pRow);
	return pRow;		
}



Row_DeviceTemplate* Table_DeviceTemplate::GetRow(long int in_PK_DeviceTemplate)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	SingleLongKey row_key(in_PK_DeviceTemplate);

	map<SingleLongKey, class TableRow*, SingleLongKey_Less>::iterator i;
	i = deleted_cachedRows.find(row_key);	
		
	//row was deleted	
	if (i!=deleted_cachedRows.end())
		return NULL;
	
	i = cachedRows.find(row_key);
	
	//row is cached
	if (i!=cachedRows.end())
		return (Row_DeviceTemplate*) (*i).second;
	//we have to fetch row
	Row_DeviceTemplate* pRow = FetchRow(row_key);

	if (pRow!=NULL)
		cachedRows[row_key] = pRow;
	return pRow;	
}



Row_DeviceTemplate* Table_DeviceTemplate::FetchRow(SingleLongKey &key)
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl,database->m_DBMutex);

	//defines the string query for the value of key
	char tmp_PK_DeviceTemplate[32];
sprintf(tmp_PK_DeviceTemplate, "%li", key.pk);


string condition;
condition = condition + "`PK_DeviceTemplate`=" + tmp_PK_DeviceTemplate;


	string query = "select * from DeviceTemplate where " + condition;		

	if (db_wrapper_query(database->m_pDB, query.c_str()))
	{	
		database->m_sLastDBError = db_wrapper_error(database->m_pDB);
		cerr << "Cannot perform query: [" << query << "] " << database->m_sLastDBError << endl;
		bool bResult=database->DBConnect(true);
		int iResult2=-1;
		if( bResult )
			iResult2 = db_wrapper_query(database->m_pDB, query.c_str());

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::FetchRow Cannot perform query [%s] %s reconnect: %d result2: %d",query.c_str(),database->m_sLastDBError.c_str(),(int) bResult, iResult2);
		if( iResult2!=0 )  // We can keep going if the time it worked
			return NULL;
	}	

	DB_RES *res = db_wrapper_store_result(database->m_pDB);
	
	if (!res)
	{
		cerr << "db_wrapper_store_result returned NULL handler" << endl;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Table_DeviceTemplate::FetchRow db_wrapper_store_result returned NULL handler");
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

	Row_DeviceTemplate *pRow = new Row_DeviceTemplate(this);
		
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
sscanf(row[27], "%li", &(pRow->m_FK_PnpLevel));
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
pRow->m_psc_restrict = 0;
}
else
{
pRow->is_null[34]=false;
sscanf(row[34], "%li", &(pRow->m_psc_restrict));
}



	db_wrapper_free_result(res);			
	
	return pRow;						
}


class Row_DeviceCategory* Row_DeviceTemplate::FK_DeviceCategory_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceCategory *pTable = table->database->DeviceCategory_get();
return pTable->GetRow(m_FK_DeviceCategory);
}
class Row_Manufacturer* Row_DeviceTemplate::FK_Manufacturer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Manufacturer *pTable = table->database->Manufacturer_get();
return pTable->GetRow(m_FK_Manufacturer);
}
class Row_StabilityStatus* Row_DeviceTemplate::FK_StabilityStatus_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StabilityStatus *pTable = table->database->StabilityStatus_get();
return pTable->GetRow(m_FK_StabilityStatus);
}
class Row_Package* Row_DeviceTemplate::FK_Package_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Package *pTable = table->database->Package_get();
return pTable->GetRow(m_FK_Package);
}
class Row_Users* Row_DeviceTemplate::FK_Users_Maintainer_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Users *pTable = table->database->Users_get();
return pTable->GetRow(m_FK_Users_Maintainer);
}
class Row_OperatingSystem* Row_DeviceTemplate::FK_OperatingSystem_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_OperatingSystem *pTable = table->database->OperatingSystem_get();
return pTable->GetRow(m_FK_OperatingSystem);
}
class Row_Distro* Row_DeviceTemplate::FK_Distro_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Distro *pTable = table->database->Distro_get();
return pTable->GetRow(m_FK_Distro);
}
class Row_Licensing* Row_DeviceTemplate::FK_Licensing_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Licensing *pTable = table->database->Licensing_get();
return pTable->GetRow(m_FK_Licensing);
}
class Row_InfraredGroup* Row_DeviceTemplate::FK_InfraredGroup_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_InfraredGroup *pTable = table->database->InfraredGroup_get();
return pTable->GetRow(m_FK_InfraredGroup);
}
class Row_CommMethod* Row_DeviceTemplate::FK_CommMethod_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommMethod *pTable = table->database->CommMethod_get();
return pTable->GetRow(m_FK_CommMethod);
}
class Row_PnpLevel* Row_DeviceTemplate::FK_PnpLevel_getrow()
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PnpLevel *pTable = table->database->PnpLevel_get();
return pTable->GetRow(m_FK_PnpLevel);
}


void Row_DeviceTemplate::CommandGroup_D_Command_FK_DeviceTemplate_getrows(vector <class Row_CommandGroup_D_Command*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_CommandGroup_D_Command *pTable = table->database->CommandGroup_D_Command_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::ConfigType_FK_DeviceTemplate_getrows(vector <class Row_ConfigType*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_ConfigType *pTable = table->database->ConfigType_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DHCPDevice_FK_DeviceTemplate_getrows(vector <class Row_DHCPDevice*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DHCPDevice *pTable = table->database->DHCPDevice_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::Device_FK_DeviceTemplate_getrows(vector <class Row_Device*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Device *pTable = table->database->Device_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_AV_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_AV*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_AV *pTable = table->database->DeviceTemplate_AV_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DSPMode_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DSPMode*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DSPMode *pTable = table->database->DeviceTemplate_DSPMode_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DesignObj_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DesignObj *pTable = table->database->DeviceTemplate_DesignObj_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceCategory_ControlledVia_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceCategory_ControlledVia *pTable = table->database->DeviceTemplate_DeviceCategory_ControlledVia_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceCommandGroup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceCommandGroup*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceCommandGroup *pTable = table->database->DeviceTemplate_DeviceCommandGroup_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceData_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceData*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceData *pTable = table->database->DeviceTemplate_DeviceData_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceTemplate_ControlledVia *pTable = table->database->DeviceTemplate_DeviceTemplate_ControlledVia_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_ControlledVia_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceTemplate_ControlledVia *pTable = table->database->DeviceTemplate_DeviceTemplate_ControlledVia_get();
pTable->GetRows("`FK_DeviceTemplate_ControlledVia`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_Related_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_Related*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceTemplate_Related *pTable = table->database->DeviceTemplate_DeviceTemplate_Related_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_DeviceTemplate_Related_FK_DeviceTemplate_Related_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_Related*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_DeviceTemplate_Related *pTable = table->database->DeviceTemplate_DeviceTemplate_Related_get();
pTable->GetRows("`FK_DeviceTemplate_Related`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Event_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Event*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_Event *pTable = table->database->DeviceTemplate_Event_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_InfraredGroup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_InfraredGroup*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_InfraredGroup *pTable = table->database->DeviceTemplate_InfraredGroup_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Input_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Input*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_Input *pTable = table->database->DeviceTemplate_Input_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_MediaType*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_MediaType *pTable = table->database->DeviceTemplate_MediaType_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_Output_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Output*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_Output *pTable = table->database->DeviceTemplate_Output_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::DeviceTemplate_PageSetup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_PageSetup*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_DeviceTemplate_PageSetup *pTable = table->database->DeviceTemplate_PageSetup_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::InfraredGroup_Command_FK_DeviceTemplate_getrows(vector <class Row_InfraredGroup_Command*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_InfraredGroup_Command *pTable = table->database->InfraredGroup_Command_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::InstallWizard_FK_DeviceTemplate_getrows(vector <class Row_InstallWizard*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_InstallWizard *pTable = table->database->InstallWizard_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::InstallWizard_Distro_FK_DeviceTemplate_Required_getrows(vector <class Row_InstallWizard_Distro*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_InstallWizard_Distro *pTable = table->database->InstallWizard_Distro_get();
pTable->GetRows("`FK_DeviceTemplate_Required`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::PnpQueue_FK_DeviceTemplate_getrows(vector <class Row_PnpQueue*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_PnpQueue *pTable = table->database->PnpQueue_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::Screen_DesignObj_FK_DeviceTemplate_getrows(vector <class Row_Screen_DesignObj*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_Screen_DesignObj *pTable = table->database->Screen_DesignObj_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}
void Row_DeviceTemplate::StartupScript_FK_DeviceTemplate_getrows(vector <class Row_StartupScript*> *rows)
{
PLUTO_SAFETY_LOCK_ERRORSONLY(sl,table->database->m_DBMutex);

class Table_StartupScript *pTable = table->database->StartupScript_get();
pTable->GetRows("`FK_DeviceTemplate`=" + StringUtils::itos(m_PK_DeviceTemplate),rows);
}



