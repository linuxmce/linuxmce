/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Configuration_H__
#define __Table_Configuration_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Configuration.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Configuration : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Configuration(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Configuration();

private:		
	friend class Row_Configuration;
	struct Key
	{
		friend class Row_Configuration;
		long int pk_PK_Configuration;

		
		Key(long int in_PK_Configuration);
	
		Key(class Row_Configuration *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Configuration::Key &key1, const Table_Configuration::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Configuration*> *rows);
	class Row_Configuration* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Configuration* GetRow(long int in_PK_Configuration);
	

private:	
	
		
	class Row_Configuration* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Configuration : public TableRow, public SerializeClass
	{
		friend struct Table_Configuration::Key;
		friend class Table_Configuration;
	private:
		Table_Configuration *table;
		
		long int m_PK_Configuration;
string m_Define;
long int m_FK_GameSystem;
string m_Description;
string m_Configuration;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_Configuration_get();
string Define_get();
long int FK_GameSystem_get();
string Description_get();
string Configuration_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Configuration_set(long int val);
void Define_set(string val);
void FK_GameSystem_set(long int val);
void Description_set(string val);
void Configuration_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Define_isNull();
bool Description_isNull();
bool Configuration_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void Define_setNull(bool val);
void Description_setNull(bool val);
void Configuration_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Configuration(Table_Configuration *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Configuration *Table_Configuration_get() { return table; };

		// Return the rows for foreign keys 
		class Row_GameSystem* FK_GameSystem_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Rom_FK_Configuration_getrows(vector <class Row_Rom*> *rows);
void Rom_Configuration_FK_Configuration_getrows(vector <class Row_Rom_Configuration*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Configuration+ m_Define+ m_FK_GameSystem+ m_Description+ m_Configuration+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Configuration_asSQL();
string Define_asSQL();
string FK_GameSystem_asSQL();
string Description_asSQL();
string Configuration_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

