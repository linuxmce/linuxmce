/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_InstallWizard_Distro_H__
#define __Table_InstallWizard_Distro_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InstallWizard_Distro.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_InstallWizard_Distro : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_InstallWizard_Distro(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_InstallWizard_Distro();

private:		
	friend class Row_InstallWizard_Distro;
	struct Key
	{
		friend class Row_InstallWizard_Distro;
		long int pk_PK_InstallWizard_Distro;

		
		Key(long int in_PK_InstallWizard_Distro);
	
		Key(class Row_InstallWizard_Distro *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InstallWizard_Distro::Key &key1, const Table_InstallWizard_Distro::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_InstallWizard_Distro*> *rows);
	class Row_InstallWizard_Distro* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InstallWizard_Distro* GetRow(long int in_PK_InstallWizard_Distro);
	

private:	
	
		
	class Row_InstallWizard_Distro* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_InstallWizard_Distro : public TableRow, public SerializeClass
	{
		friend struct Table_InstallWizard_Distro::Key;
		friend class Table_InstallWizard_Distro;
	private:
		Table_InstallWizard_Distro *table;
		
		long int m_PK_InstallWizard_Distro;
long int m_FK_InstallWizard;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
long int m_FK_DeviceTemplate_Required;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		long int PK_InstallWizard_Distro_get();
long int FK_InstallWizard_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
long int FK_DeviceTemplate_Required_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_InstallWizard_Distro_set(long int val);
void FK_InstallWizard_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void FK_DeviceTemplate_Required_set(long int val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_InstallWizard_isNull();
bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool FK_DeviceTemplate_Required_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_InstallWizard_setNull(bool val);
void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void FK_DeviceTemplate_Required_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InstallWizard_Distro(Table_InstallWizard_Distro *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InstallWizard_Distro *Table_InstallWizard_Distro_get() { return table; };

		// Return the rows for foreign keys 
		class Row_InstallWizard* FK_InstallWizard_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_Required_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_InstallWizard_Distro+ m_FK_InstallWizard+ m_FK_OperatingSystem+ m_FK_Distro+ m_FK_DeviceTemplate_Required+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_InstallWizard_Distro_asSQL();
string FK_InstallWizard_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string FK_DeviceTemplate_Required_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

