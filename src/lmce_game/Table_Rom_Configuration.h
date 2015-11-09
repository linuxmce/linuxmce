/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Rom_Configuration_H__
#define __Table_Rom_Configuration_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Rom_Configuration.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Rom_Configuration : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Rom_Configuration(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Rom_Configuration();

private:		
	friend class Row_Rom_Configuration;
	struct Key
	{
		friend class Row_Rom_Configuration;
		long int pk_PK_Rom_Configuration;

		
		Key(long int in_PK_Rom_Configuration);
	
		Key(class Row_Rom_Configuration *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Rom_Configuration::Key &key1, const Table_Rom_Configuration::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Rom_Configuration*> *rows);
	class Row_Rom_Configuration* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Rom_Configuration* GetRow(long int in_PK_Rom_Configuration);
	

private:	
	
		
	class Row_Rom_Configuration* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Rom_Configuration : public TableRow, public SerializeClass
	{
		friend struct Table_Rom_Configuration::Key;
		friend class Table_Rom_Configuration;
	private:
		Table_Rom_Configuration *table;
		
		long int m_PK_Rom_Configuration;
long int m_FK_Rom;
long int m_FK_Configuration;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int PK_Rom_Configuration_get();
long int FK_Rom_get();
long int FK_Configuration_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Rom_Configuration_set(long int val);
void FK_Rom_set(long int val);
void FK_Configuration_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Rom_Configuration(Table_Rom_Configuration *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Rom_Configuration *Table_Rom_Configuration_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Rom* FK_Rom_getrow();
class Row_Configuration* FK_Configuration_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Rom_Configuration+ m_FK_Rom+ m_FK_Configuration+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Rom_Configuration_asSQL();
string FK_Rom_asSQL();
string FK_Configuration_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

