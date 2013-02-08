/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Package_Source_Compat_H__
#define __Table_Package_Source_Compat_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Source_Compat.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Package_Source_Compat : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Package_Source_Compat(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Package_Source_Compat();

private:		
	friend class Row_Package_Source_Compat;
	struct Key
	{
		friend class Row_Package_Source_Compat;
		long int pk_PK_Package_Source_Compat;

		
		Key(long int in_PK_Package_Source_Compat);
	
		Key(class Row_Package_Source_Compat *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Source_Compat::Key &key1, const Table_Package_Source_Compat::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Package_Source_Compat*> *rows);
	class Row_Package_Source_Compat* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Source_Compat* GetRow(long int in_PK_Package_Source_Compat);
	

private:	
	
		
	class Row_Package_Source_Compat* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Package_Source_Compat : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Source_Compat::Key;
		friend class Table_Package_Source_Compat;
	private:
		Table_Package_Source_Compat *table;
		
		long int m_PK_Package_Source_Compat;
long int m_FK_Package_Source;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
short int m_MustBuildFromSource;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		long int PK_Package_Source_Compat_get();
long int FK_Package_Source_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
short int MustBuildFromSource_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Package_Source_Compat_set(long int val);
void FK_Package_Source_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void MustBuildFromSource_set(short int val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool MustBuildFromSource_isNull();
bool Comments_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void MustBuildFromSource_setNull(bool val);
void Comments_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_Source_Compat(Table_Package_Source_Compat *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Source_Compat *Table_Package_Source_Compat_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package_Source* FK_Package_Source_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Package_Source_Compat+ m_FK_Package_Source+ m_FK_OperatingSystem+ m_FK_Distro+ m_MustBuildFromSource+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_Source_Compat_asSQL();
string FK_Package_Source_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string MustBuildFromSource_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

