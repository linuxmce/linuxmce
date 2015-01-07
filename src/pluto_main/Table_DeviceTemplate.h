/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_H__
#define __Table_DeviceTemplate_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate();

private:		
	friend class Row_DeviceTemplate;
	struct Key
	{
		friend class Row_DeviceTemplate;
		long int pk_PK_DeviceTemplate;

		
		Key(long int in_PK_DeviceTemplate);
	
		Key(class Row_DeviceTemplate *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate::Key &key1, const Table_DeviceTemplate::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate*> *rows);
	class Row_DeviceTemplate* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate* GetRow(long int in_PK_DeviceTemplate);
	

private:	
	
		
	class Row_DeviceTemplate* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate::Key;
		friend class Table_DeviceTemplate;
	private:
		Table_DeviceTemplate *table;
		
		long int m_PK_DeviceTemplate;
string m_Description;
string m_Comments;
long int m_FK_DeviceCategory;
long int m_FK_Manufacturer;
string m_Define;
short int m_ImplementsDCE;
short int m_IsEmbedded;
string m_CommandLine;
short int m_IsPlugAndPlay;
short int m_IsIPBased;
short int m_IsPlugIn;
long int m_IRFrequency;
long int m_FK_StabilityStatus;
long int m_FK_Package;
string m_DestinationDir;
long int m_FK_Users_Maintainer;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
short int m_InheritsMacFromPC;
string m_ConfigureScript;
string m_ManufacturerURL;
string m_InternalURLSuffix;
string m_WizardURL;
long int m_FK_Licensing;
long int m_FK_InfraredGroup;
long int m_FK_CommMethod;
long int m_FK_PnpLevel;
string m_HelpURL;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[35];
	
	public:
		long int PK_DeviceTemplate_get();
string Description_get();
string Comments_get();
long int FK_DeviceCategory_get();
long int FK_Manufacturer_get();
string Define_get();
short int ImplementsDCE_get();
short int IsEmbedded_get();
string CommandLine_get();
short int IsPlugAndPlay_get();
short int IsIPBased_get();
short int IsPlugIn_get();
long int IRFrequency_get();
long int FK_StabilityStatus_get();
long int FK_Package_get();
string DestinationDir_get();
long int FK_Users_Maintainer_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
short int InheritsMacFromPC_get();
string ConfigureScript_get();
string ManufacturerURL_get();
string InternalURLSuffix_get();
string WizardURL_get();
long int FK_Licensing_get();
long int FK_InfraredGroup_get();
long int FK_CommMethod_get();
long int FK_PnpLevel_get();
string HelpURL_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DeviceTemplate_set(long int val);
void Description_set(string val);
void Comments_set(string val);
void FK_DeviceCategory_set(long int val);
void FK_Manufacturer_set(long int val);
void Define_set(string val);
void ImplementsDCE_set(short int val);
void IsEmbedded_set(short int val);
void CommandLine_set(string val);
void IsPlugAndPlay_set(short int val);
void IsIPBased_set(short int val);
void IsPlugIn_set(short int val);
void IRFrequency_set(long int val);
void FK_StabilityStatus_set(long int val);
void FK_Package_set(long int val);
void DestinationDir_set(string val);
void FK_Users_Maintainer_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void InheritsMacFromPC_set(short int val);
void ConfigureScript_set(string val);
void ManufacturerURL_set(string val);
void InternalURLSuffix_set(string val);
void WizardURL_set(string val);
void FK_Licensing_set(long int val);
void FK_InfraredGroup_set(long int val);
void FK_CommMethod_set(long int val);
void FK_PnpLevel_set(long int val);
void HelpURL_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Comments_isNull();
bool FK_Manufacturer_isNull();
bool IsEmbedded_isNull();
bool CommandLine_isNull();
bool IRFrequency_isNull();
bool FK_StabilityStatus_isNull();
bool FK_Package_isNull();
bool DestinationDir_isNull();
bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool InheritsMacFromPC_isNull();
bool ConfigureScript_isNull();
bool ManufacturerURL_isNull();
bool InternalURLSuffix_isNull();
bool WizardURL_isNull();
bool FK_Licensing_isNull();
bool FK_InfraredGroup_isNull();
bool FK_CommMethod_isNull();
bool FK_PnpLevel_isNull();
bool HelpURL_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Comments_setNull(bool val);
void FK_Manufacturer_setNull(bool val);
void IsEmbedded_setNull(bool val);
void CommandLine_setNull(bool val);
void IRFrequency_setNull(bool val);
void FK_StabilityStatus_setNull(bool val);
void FK_Package_setNull(bool val);
void DestinationDir_setNull(bool val);
void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void InheritsMacFromPC_setNull(bool val);
void ConfigureScript_setNull(bool val);
void ManufacturerURL_setNull(bool val);
void InternalURLSuffix_setNull(bool val);
void WizardURL_setNull(bool val);
void FK_Licensing_setNull(bool val);
void FK_InfraredGroup_setNull(bool val);
void FK_CommMethod_setNull(bool val);
void FK_PnpLevel_setNull(bool val);
void HelpURL_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate(Table_DeviceTemplate *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate *Table_DeviceTemplate_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();
class Row_StabilityStatus* FK_StabilityStatus_getrow();
class Row_Package* FK_Package_getrow();
class Row_Users* FK_Users_Maintainer_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();
class Row_Licensing* FK_Licensing_getrow();
class Row_InfraredGroup* FK_InfraredGroup_getrow();
class Row_CommMethod* FK_CommMethod_getrow();
class Row_PnpLevel* FK_PnpLevel_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_D_Command_FK_DeviceTemplate_getrows(vector <class Row_CommandGroup_D_Command*> *rows);
void ConfigType_FK_DeviceTemplate_getrows(vector <class Row_ConfigType*> *rows);
void DHCPDevice_FK_DeviceTemplate_getrows(vector <class Row_DHCPDevice*> *rows);
void Device_FK_DeviceTemplate_getrows(vector <class Row_Device*> *rows);
void DeviceTemplate_AV_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_AV*> *rows);
void DeviceTemplate_DSPMode_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DSPMode*> *rows);
void DeviceTemplate_DesignObj_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DesignObj*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia*> *rows);
void DeviceTemplate_DeviceCommandGroup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceCommandGroup*> *rows);
void DeviceTemplate_DeviceData_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceData*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_FK_DeviceTemplate_ControlledVia_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia*> *rows);
void DeviceTemplate_DeviceTemplate_Related_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_Related*> *rows);
void DeviceTemplate_DeviceTemplate_Related_FK_DeviceTemplate_Related_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_Related*> *rows);
void DeviceTemplate_Event_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Event*> *rows);
void DeviceTemplate_InfraredGroup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_InfraredGroup*> *rows);
void DeviceTemplate_Input_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Input*> *rows);
void DeviceTemplate_MediaType_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_MediaType*> *rows);
void DeviceTemplate_Output_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_Output*> *rows);
void DeviceTemplate_PageSetup_FK_DeviceTemplate_getrows(vector <class Row_DeviceTemplate_PageSetup*> *rows);
void InfraredGroup_Command_FK_DeviceTemplate_getrows(vector <class Row_InfraredGroup_Command*> *rows);
void InstallWizard_FK_DeviceTemplate_getrows(vector <class Row_InstallWizard*> *rows);
void InstallWizard_Distro_FK_DeviceTemplate_Required_getrows(vector <class Row_InstallWizard_Distro*> *rows);
void PnpQueue_FK_DeviceTemplate_getrows(vector <class Row_PnpQueue*> *rows);
void Screen_DesignObj_FK_DeviceTemplate_getrows(vector <class Row_Screen_DesignObj*> *rows);
void StartupScript_FK_DeviceTemplate_getrows(vector <class Row_StartupScript*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceTemplate+ m_Description+ m_Comments+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Define+ m_ImplementsDCE+ m_IsEmbedded+ m_CommandLine+ m_IsPlugAndPlay+ m_IsIPBased+ m_IsPlugIn+ m_IRFrequency+ m_FK_StabilityStatus+ m_FK_Package+ m_DestinationDir+ m_FK_Users_Maintainer+ m_FK_OperatingSystem+ m_FK_Distro+ m_InheritsMacFromPC+ m_ConfigureScript+ m_ManufacturerURL+ m_InternalURLSuffix+ m_WizardURL+ m_FK_Licensing+ m_FK_InfraredGroup+ m_FK_CommMethod+ m_FK_PnpLevel+ m_HelpURL+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceTemplate_asSQL();
string Description_asSQL();
string Comments_asSQL();
string FK_DeviceCategory_asSQL();
string FK_Manufacturer_asSQL();
string Define_asSQL();
string ImplementsDCE_asSQL();
string IsEmbedded_asSQL();
string CommandLine_asSQL();
string IsPlugAndPlay_asSQL();
string IsIPBased_asSQL();
string IsPlugIn_asSQL();
string IRFrequency_asSQL();
string FK_StabilityStatus_asSQL();
string FK_Package_asSQL();
string DestinationDir_asSQL();
string FK_Users_Maintainer_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string InheritsMacFromPC_asSQL();
string ConfigureScript_asSQL();
string ManufacturerURL_asSQL();
string InternalURLSuffix_asSQL();
string WizardURL_asSQL();
string FK_Licensing_asSQL();
string FK_InfraredGroup_asSQL();
string FK_CommMethod_asSQL();
string FK_PnpLevel_asSQL();
string HelpURL_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

