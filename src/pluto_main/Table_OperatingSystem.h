/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_OperatingSystem_H__
#define __Table_OperatingSystem_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_OperatingSystem.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_OperatingSystem : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_OperatingSystem(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_OperatingSystem();

private:		
	friend class Row_OperatingSystem;
	struct Key
	{
		friend class Row_OperatingSystem;
		long int pk_PK_OperatingSystem;

		
		Key(long int in_PK_OperatingSystem);
	
		Key(class Row_OperatingSystem *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_OperatingSystem::Key &key1, const Table_OperatingSystem::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_OperatingSystem*> *rows);
	class Row_OperatingSystem* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_OperatingSystem* GetRow(long int in_PK_OperatingSystem);
	

private:	
	
		
	class Row_OperatingSystem* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_OperatingSystem : public TableRow, public SerializeClass
	{
		friend struct Table_OperatingSystem::Key;
		friend class Table_OperatingSystem;
	private:
		Table_OperatingSystem *table;
		
		long int m_PK_OperatingSystem;
string m_Description;
string m_Define;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int PK_OperatingSystem_get();
string Description_get();
string Define_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_OperatingSystem_set(long int val);
void Description_set(string val);
void Define_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Define_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Define_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_OperatingSystem(Table_OperatingSystem *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_OperatingSystem *Table_OperatingSystem_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_OperatingSystem_getrows(vector <class Row_DeviceTemplate*> *rows);
void Distro_FK_OperatingSystem_getrows(vector <class Row_Distro*> *rows);
void InstallWizard_Distro_FK_OperatingSystem_getrows(vector <class Row_InstallWizard_Distro*> *rows);
void Package_Compat_FK_OperatingSystem_getrows(vector <class Row_Package_Compat*> *rows);
void Package_Directory_FK_OperatingSystem_getrows(vector <class Row_Package_Directory*> *rows);
void Package_Directory_File_FK_OperatingSystem_getrows(vector <class Row_Package_Directory_File*> *rows);
void Package_Source_Compat_FK_OperatingSystem_getrows(vector <class Row_Package_Source_Compat*> *rows);
void RepositorySource_FK_OperatingSystem_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_OperatingSystem+ m_Description+ m_Define+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_OperatingSystem_asSQL();
string Description_asSQL();
string Define_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

