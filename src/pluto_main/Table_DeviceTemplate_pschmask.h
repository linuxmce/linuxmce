/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_pschmask_H__
#define __Table_DeviceTemplate_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_pschmask();

private:		
	friend class Row_DeviceTemplate_pschmask;
	struct Key
	{
		friend class Row_DeviceTemplate_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DeviceTemplate_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_pschmask::Key &key1, const Table_DeviceTemplate_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_pschmask*> *rows);
	class Row_DeviceTemplate_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DeviceTemplate_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_pschmask::Key;
		friend class Table_DeviceTemplate_pschmask;
	private:
		Table_DeviceTemplate_pschmask *table;
		
		short int m_PK_DeviceTemplate;
short int m_Description;
short int m_Comments;
short int m_FK_DeviceCategory;
short int m_FK_Manufacturer;
short int m_Define;
short int m_ImplementsDCE;
short int m_IsEmbedded;
short int m_CommandLine;
short int m_IsPlugAndPlay;
short int m_IsIPBased;
short int m_IsPlugIn;
short int m_IRFrequency;
short int m_FK_StabilityStatus;
short int m_FK_Package;
short int m_DestinationDir;
short int m_FK_Users_Maintainer;
short int m_FK_OperatingSystem;
short int m_FK_Distro;
short int m_InheritsMacFromPC;
short int m_ConfigureScript;
short int m_ManufacturerURL;
short int m_InternalURLSuffix;
short int m_WizardURL;
short int m_FK_Licensing;
short int m_FK_InfraredGroup;
short int m_FK_CommMethod;
long int m_FK_PnpLevel;
short int m_HelpURL;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;
short int m_FK_Document;

		bool is_null[36];
	
	public:
		short int PK_DeviceTemplate_get();
short int Description_get();
short int Comments_get();
short int FK_DeviceCategory_get();
short int FK_Manufacturer_get();
short int Define_get();
short int ImplementsDCE_get();
short int IsEmbedded_get();
short int CommandLine_get();
short int IsPlugAndPlay_get();
short int IsIPBased_get();
short int IsPlugIn_get();
short int IRFrequency_get();
short int FK_StabilityStatus_get();
short int FK_Package_get();
short int DestinationDir_get();
short int FK_Users_Maintainer_get();
short int FK_OperatingSystem_get();
short int FK_Distro_get();
short int InheritsMacFromPC_get();
short int ConfigureScript_get();
short int ManufacturerURL_get();
short int InternalURLSuffix_get();
short int WizardURL_get();
short int FK_Licensing_get();
short int FK_InfraredGroup_get();
short int FK_CommMethod_get();
long int FK_PnpLevel_get();
short int HelpURL_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();
short int FK_Document_get();

		
		void PK_DeviceTemplate_set(short int val);
void Description_set(short int val);
void Comments_set(short int val);
void FK_DeviceCategory_set(short int val);
void FK_Manufacturer_set(short int val);
void Define_set(short int val);
void ImplementsDCE_set(short int val);
void IsEmbedded_set(short int val);
void CommandLine_set(short int val);
void IsPlugAndPlay_set(short int val);
void IsIPBased_set(short int val);
void IsPlugIn_set(short int val);
void IRFrequency_set(short int val);
void FK_StabilityStatus_set(short int val);
void FK_Package_set(short int val);
void DestinationDir_set(short int val);
void FK_Users_Maintainer_set(short int val);
void FK_OperatingSystem_set(short int val);
void FK_Distro_set(short int val);
void InheritsMacFromPC_set(short int val);
void ConfigureScript_set(short int val);
void ManufacturerURL_set(short int val);
void InternalURLSuffix_set(short int val);
void WizardURL_set(short int val);
void FK_Licensing_set(short int val);
void FK_InfraredGroup_set(short int val);
void FK_CommMethod_set(short int val);
void FK_PnpLevel_set(long int val);
void HelpURL_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);
void FK_Document_set(short int val);

		
		bool PK_DeviceTemplate_isNull();
bool Description_isNull();
bool Comments_isNull();
bool FK_DeviceCategory_isNull();
bool FK_Manufacturer_isNull();
bool Define_isNull();
bool ImplementsDCE_isNull();
bool IsEmbedded_isNull();
bool CommandLine_isNull();
bool IsPlugAndPlay_isNull();
bool IsIPBased_isNull();
bool IsPlugIn_isNull();
bool IRFrequency_isNull();
bool FK_StabilityStatus_isNull();
bool FK_Package_isNull();
bool DestinationDir_isNull();
bool FK_Users_Maintainer_isNull();
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
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();
bool FK_Document_isNull();

			
		void PK_DeviceTemplate_setNull(bool val);
void Description_setNull(bool val);
void Comments_setNull(bool val);
void FK_DeviceCategory_setNull(bool val);
void FK_Manufacturer_setNull(bool val);
void Define_setNull(bool val);
void ImplementsDCE_setNull(bool val);
void IsEmbedded_setNull(bool val);
void CommandLine_setNull(bool val);
void IsPlugAndPlay_setNull(bool val);
void IsIPBased_setNull(bool val);
void IsPlugIn_setNull(bool val);
void IRFrequency_setNull(bool val);
void FK_StabilityStatus_setNull(bool val);
void FK_Package_setNull(bool val);
void DestinationDir_setNull(bool val);
void FK_Users_Maintainer_setNull(bool val);
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
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
void FK_Document_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_pschmask(Table_DeviceTemplate_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_pschmask *Table_DeviceTemplate_pschmask_get() { return table; };

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
class Row_Document* FK_Document_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceTemplate+ m_Description+ m_Comments+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Define+ m_ImplementsDCE+ m_IsEmbedded+ m_CommandLine+ m_IsPlugAndPlay+ m_IsIPBased+ m_IsPlugIn+ m_IRFrequency+ m_FK_StabilityStatus+ m_FK_Package+ m_DestinationDir+ m_FK_Users_Maintainer+ m_FK_OperatingSystem+ m_FK_Distro+ m_InheritsMacFromPC+ m_ConfigureScript+ m_ManufacturerURL+ m_InternalURLSuffix+ m_WizardURL+ m_FK_Licensing+ m_FK_InfraredGroup+ m_FK_CommMethod+ m_FK_PnpLevel+ m_HelpURL+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict+ m_FK_Document;
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
string FK_Document_asSQL();

	};

#endif

