/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Package_Directory_pschmask_H__
#define __Table_Package_Directory_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_Directory_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Package_Directory_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Package_Directory_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Package_Directory_pschmask();

private:		
	friend class Row_Package_Directory_pschmask;
	struct Key
	{
		friend class Row_Package_Directory_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Package_Directory_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_Directory_pschmask::Key &key1, const Table_Package_Directory_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Package_Directory_pschmask*> *rows);
	class Row_Package_Directory_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_Directory_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Package_Directory_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Package_Directory_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Package_Directory_pschmask::Key;
		friend class Table_Package_Directory_pschmask;
	private:
		Table_Package_Directory_pschmask *table;
		
		short int m_PK_Package_Directory;
short int m_FK_Package;
short int m_FK_Directory;
short int m_FK_OperatingSystem;
short int m_FK_Distro;
short int m_Path;
short int m_InputPath;
short int m_FlipSource;
short int m_GenerateDoxygen;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[15];
	
	public:
		short int PK_Package_Directory_get();
short int FK_Package_get();
short int FK_Directory_get();
short int FK_OperatingSystem_get();
short int FK_Distro_get();
short int Path_get();
short int InputPath_get();
short int FlipSource_get();
short int GenerateDoxygen_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Package_Directory_set(short int val);
void FK_Package_set(short int val);
void FK_Directory_set(short int val);
void FK_OperatingSystem_set(short int val);
void FK_Distro_set(short int val);
void Path_set(short int val);
void InputPath_set(short int val);
void FlipSource_set(short int val);
void GenerateDoxygen_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_Package_Directory_isNull();
bool FK_Package_isNull();
bool FK_Directory_isNull();
bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool Path_isNull();
bool InputPath_isNull();
bool FlipSource_isNull();
bool GenerateDoxygen_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_Package_Directory_setNull(bool val);
void FK_Package_setNull(bool val);
void FK_Directory_setNull(bool val);
void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void Path_setNull(bool val);
void InputPath_setNull(bool val);
void FlipSource_setNull(bool val);
void GenerateDoxygen_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_Directory_pschmask(Table_Package_Directory_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_Directory_pschmask *Table_Package_Directory_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();
class Row_Directory* FK_Directory_getrow();
class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Package_Directory+ m_FK_Package+ m_FK_Directory+ m_FK_OperatingSystem+ m_FK_Distro+ m_Path+ m_InputPath+ m_FlipSource+ m_GenerateDoxygen+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_Directory_asSQL();
string FK_Package_asSQL();
string FK_Directory_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string Path_asSQL();
string InputPath_asSQL();
string FlipSource_asSQL();
string GenerateDoxygen_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

