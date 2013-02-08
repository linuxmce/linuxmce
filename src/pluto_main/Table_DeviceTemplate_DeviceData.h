/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_DeviceData_H__
#define __Table_DeviceTemplate_DeviceData_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_DeviceData : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_DeviceData(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_DeviceData();

private:		
	friend class Row_DeviceTemplate_DeviceData;
	struct Key
	{
		friend class Row_DeviceTemplate_DeviceData;
		long int pk_FK_DeviceTemplate;
long int pk_FK_DeviceData;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_DeviceData);
	
		Key(class Row_DeviceTemplate_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_DeviceData::Key &key1, const Table_DeviceTemplate_DeviceData::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_DeviceData*> *rows);
	class Row_DeviceTemplate_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_DeviceData* GetRow(long int in_FK_DeviceTemplate, long int in_FK_DeviceData);
	

private:	
	
		
	class Row_DeviceTemplate_DeviceData* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_DeviceData::Key;
		friend class Table_DeviceTemplate_DeviceData;
	private:
		Table_DeviceTemplate_DeviceData *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_DeviceData;
string m_IK_DeviceData;
short int m_Required;
short int m_AllowedToModify;
short int m_SetByDevice;
string m_Description;
short int m_UseDeviceTemplateDefault;
short int m_ShowInWizard;
string m_ShortDescription;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_DeviceData_get();
string IK_DeviceData_get();
short int Required_get();
short int AllowedToModify_get();
short int SetByDevice_get();
string Description_get();
short int UseDeviceTemplateDefault_get();
short int ShowInWizard_get();
string ShortDescription_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_DeviceData_set(long int val);
void IK_DeviceData_set(string val);
void Required_set(short int val);
void AllowedToModify_set(short int val);
void SetByDevice_set(short int val);
void Description_set(string val);
void UseDeviceTemplateDefault_set(short int val);
void ShowInWizard_set(short int val);
void ShortDescription_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool IK_DeviceData_isNull();
bool Required_isNull();
bool AllowedToModify_isNull();
bool Description_isNull();
bool ShortDescription_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void IK_DeviceData_setNull(bool val);
void Required_setNull(bool val);
void AllowedToModify_setNull(bool val);
void Description_setNull(bool val);
void ShortDescription_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_DeviceData(Table_DeviceTemplate_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_DeviceData *Table_DeviceTemplate_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceData* FK_DeviceData_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_DeviceData+ m_IK_DeviceData+ m_Required+ m_AllowedToModify+ m_SetByDevice+ m_Description+ m_UseDeviceTemplateDefault+ m_ShowInWizard+ m_ShortDescription+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_DeviceData_asSQL();
string IK_DeviceData_asSQL();
string Required_asSQL();
string AllowedToModify_asSQL();
string SetByDevice_asSQL();
string Description_asSQL();
string UseDeviceTemplateDefault_asSQL();
string ShowInWizard_asSQL();
string ShortDescription_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

