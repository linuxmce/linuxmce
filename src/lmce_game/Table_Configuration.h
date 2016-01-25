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
string m_Comments;
string m_Configuration;

		bool is_null[4];
	
	public:
		long int PK_Configuration_get();
string Define_get();
string Comments_get();
string Configuration_get();

		
		void PK_Configuration_set(long int val);
void Define_set(string val);
void Comments_set(string val);
void Configuration_set(string val);

		
		bool Define_isNull();
bool Comments_isNull();
bool Configuration_isNull();

			
		void Define_setNull(bool val);
void Comments_setNull(bool val);
void Configuration_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Configuration(Table_Configuration *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Configuration *Table_Configuration_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Game_GameSystem_Configuration_FK_Configuration_getrows(vector <class Row_Game_GameSystem_Configuration*> *rows);
void Game_GameSystem_Rom_Configuration_FK_Configuration_getrows(vector <class Row_Game_GameSystem_Rom_Configuration*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Configuration+ m_Define+ m_Comments+ m_Configuration;
		}
	private:
		void SetDefaultValues();
		
		string PK_Configuration_asSQL();
string Define_asSQL();
string Comments_asSQL();
string Configuration_asSQL();

	};

#endif

