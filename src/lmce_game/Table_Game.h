/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Game_H__
#define __Table_Game_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Game.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Game : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Game(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Game();

private:		
	friend class Row_Game;
	struct Key
	{
		friend class Row_Game;
		long int pk_PK_Game;

		
		Key(long int in_PK_Game);
	
		Key(class Row_Game *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Game::Key &key1, const Table_Game::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Game*> *rows);
	class Row_Game* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Game* GetRow(long int in_PK_Game);
	

private:	
	
		
	class Row_Game* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Game : public TableRow, public SerializeClass
	{
		friend struct Table_Game::Key;
		friend class Table_Game;
	private:
		Table_Game *table;
		
		long int m_PK_Game;
string m_Define;
string m_FK_NameHash;
long int m_FK_Manufacturer;
long int m_FK_Genre;
long int m_FK_Year;

		bool is_null[6];
	
	public:
		long int PK_Game_get();
string Define_get();
string FK_NameHash_get();
long int FK_Manufacturer_get();
long int FK_Genre_get();
long int FK_Year_get();

		
		void PK_Game_set(long int val);
void Define_set(string val);
void FK_NameHash_set(string val);
void FK_Manufacturer_set(long int val);
void FK_Genre_set(long int val);
void FK_Year_set(long int val);

		
		bool Define_isNull();
bool FK_Year_isNull();

			
		void Define_setNull(bool val);
void FK_Year_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Game(Table_Game *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Game *Table_Game_get() { return table; };

		// Return the rows for foreign keys 
		class Row_NameHash* FK_NameHash_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();
class Row_Genre* FK_Genre_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Game_GameSystem_FK_Game_getrows(vector <class Row_Game_GameSystem*> *rows);
void Game_GameSystem_Configuration_FK_Game_getrows(vector <class Row_Game_GameSystem_Configuration*> *rows);
void Game_GameSystem_Picture_FK_Game_getrows(vector <class Row_Game_GameSystem_Picture*> *rows);
void Game_GameSystem_Rom_FK_Game_getrows(vector <class Row_Game_GameSystem_Rom*> *rows);
void Game_GameSystem_Rom_Configuration_FK_Game_getrows(vector <class Row_Game_GameSystem_Rom_Configuration*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Game+ m_Define+ m_FK_NameHash+ m_FK_Manufacturer+ m_FK_Genre+ m_FK_Year;
		}
	private:
		void SetDefaultValues();
		
		string PK_Game_asSQL();
string Define_asSQL();
string FK_NameHash_asSQL();
string FK_Manufacturer_asSQL();
string FK_Genre_asSQL();
string FK_Year_asSQL();

	};

#endif

