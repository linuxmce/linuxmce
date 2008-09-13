/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_RepositorySource_pschist_H__
#define __Table_RepositorySource_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RepositorySource_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_RepositorySource_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_RepositorySource_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_RepositorySource_pschist();

private:		
	friend class Row_RepositorySource_pschist;
	struct Key
	{
		friend class Row_RepositorySource_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_RepositorySource_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RepositorySource_pschist::Key &key1, const Table_RepositorySource_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_RepositorySource_pschist*> *rows);
	class Row_RepositorySource_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RepositorySource_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_RepositorySource_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_RepositorySource_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_RepositorySource_pschist::Key;
		friend class Table_RepositorySource_pschist;
	private:
		Table_RepositorySource_pschist *table;
		
		long int m_PK_RepositorySource;
long int m_FK_OperatingSystem;
long int m_FK_Distro;
long int m_FK_RepositoryType;
string m_Description;
string m_Define;
string m_Instructions;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[14];
	
	public:
		long int PK_RepositorySource_get();
long int FK_OperatingSystem_get();
long int FK_Distro_get();
long int FK_RepositoryType_get();
string Description_get();
string Define_get();
string Instructions_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void PK_RepositorySource_set(long int val);
void FK_OperatingSystem_set(long int val);
void FK_Distro_set(long int val);
void FK_RepositoryType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Instructions_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);

		
		bool PK_RepositorySource_isNull();
bool FK_OperatingSystem_isNull();
bool FK_Distro_isNull();
bool FK_RepositoryType_isNull();
bool Description_isNull();
bool Define_isNull();
bool Instructions_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();

			
		void PK_RepositorySource_setNull(bool val);
void FK_OperatingSystem_setNull(bool val);
void FK_Distro_setNull(bool val);
void FK_RepositoryType_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void Instructions_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RepositorySource_pschist(Table_RepositorySource_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RepositorySource_pschist *Table_RepositorySource_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_OperatingSystem* FK_OperatingSystem_getrow();
class Row_Distro* FK_Distro_getrow();
class Row_RepositoryType* FK_RepositoryType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_RepositorySource+ m_FK_OperatingSystem+ m_FK_Distro+ m_FK_RepositoryType+ m_Description+ m_Define+ m_Instructions+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_RepositorySource_asSQL();
string FK_OperatingSystem_asSQL();
string FK_Distro_asSQL();
string FK_RepositoryType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Instructions_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

