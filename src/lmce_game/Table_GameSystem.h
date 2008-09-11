/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_GameSystem_H__
#define __Table_GameSystem_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_GameSystem.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_GameSystem : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_GameSystem(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_GameSystem();

private:		
	friend class Row_GameSystem;
	struct Key
	{
		friend class Row_GameSystem;
		long int pk_PK_GameSystem;

		
		Key(long int in_PK_GameSystem);
	
		Key(class Row_GameSystem *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_GameSystem::Key &key1, const Table_GameSystem::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_GameSystem*> *rows);
	class Row_GameSystem* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_GameSystem* GetRow(long int in_PK_GameSystem);
	

private:	
	
		
	class Row_GameSystem* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_GameSystem : public TableRow, public SerializeClass
	{
		friend struct Table_GameSystem::Key;
		friend class Table_GameSystem;
	private:
		Table_GameSystem *table;
		
		long int m_PK_GameSystem;
string m_Define;
string m_Description;

		bool is_null[3];
	
	public:
		long int PK_GameSystem_get();
string Define_get();
string Description_get();

		
		void PK_GameSystem_set(long int val);
void Define_set(string val);
void Description_set(string val);

		
		bool Define_isNull();
bool Description_isNull();

			
		void Define_setNull(bool val);
void Description_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_GameSystem(Table_GameSystem *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_GameSystem *Table_GameSystem_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Rom_FK_GameSystem_getrows(vector <class Row_Rom*> *rows);
void RomAttribute_FK_GameSystem_getrows(vector <class Row_RomAttribute*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_GameSystem+ m_Define+ m_Description;
		}
	private:
		void SetDefaultValues();
		
		string PK_GameSystem_asSQL();
string Define_asSQL();
string Description_asSQL();

	};

#endif

