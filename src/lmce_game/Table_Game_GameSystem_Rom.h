/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Game_GameSystem_Rom_H__
#define __Table_Game_GameSystem_Rom_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Game_GameSystem_Rom.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Game_GameSystem_Rom : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Game_GameSystem_Rom(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Game_GameSystem_Rom();

private:		
	friend class Row_Game_GameSystem_Rom;
	struct Key
	{
		friend class Row_Game_GameSystem_Rom;
		long int pk_PK_Game_Rom;

		
		Key(long int in_PK_Game_Rom);
	
		Key(class Row_Game_GameSystem_Rom *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Game_GameSystem_Rom::Key &key1, const Table_Game_GameSystem_Rom::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Game_GameSystem_Rom*> *rows);
	class Row_Game_GameSystem_Rom* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Game_GameSystem_Rom* GetRow(long int in_PK_Game_Rom);
	

private:	
	
		
	class Row_Game_GameSystem_Rom* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Game_GameSystem_Rom : public TableRow, public SerializeClass
	{
		friend struct Table_Game_GameSystem_Rom::Key;
		friend class Table_Game_GameSystem_Rom;
	private:
		Table_Game_GameSystem_Rom *table;
		
		long int m_PK_Game_Rom;
long int m_FK_Game;
long int m_FK_GameSystem;
long int m_FK_Rom;

		bool is_null[4];
	
	public:
		long int PK_Game_Rom_get();
long int FK_Game_get();
long int FK_GameSystem_get();
long int FK_Rom_get();

		
		void PK_Game_Rom_set(long int val);
void FK_Game_set(long int val);
void FK_GameSystem_set(long int val);
void FK_Rom_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Game_GameSystem_Rom(Table_Game_GameSystem_Rom *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Game_GameSystem_Rom *Table_Game_GameSystem_Rom_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Game* FK_Game_getrow();
class Row_GameSystem* FK_GameSystem_getrow();
class Row_Rom* FK_Rom_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Game_Rom+ m_FK_Game+ m_FK_GameSystem+ m_FK_Rom;
		}
	private:
		void SetDefaultValues();
		
		string PK_Game_Rom_asSQL();
string FK_Game_asSQL();
string FK_GameSystem_asSQL();
string FK_Rom_asSQL();

	};

#endif

