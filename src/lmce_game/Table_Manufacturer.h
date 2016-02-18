/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Manufacturer_H__
#define __Table_Manufacturer_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Manufacturer.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Manufacturer : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Manufacturer(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Manufacturer();

private:		
	friend class Row_Manufacturer;
	struct Key
	{
		friend class Row_Manufacturer;
		long int pk_PK_Manufacturer;

		
		Key(long int in_PK_Manufacturer);
	
		Key(class Row_Manufacturer *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Manufacturer::Key &key1, const Table_Manufacturer::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Manufacturer*> *rows);
	class Row_Manufacturer* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Manufacturer* GetRow(long int in_PK_Manufacturer);
	

private:	
	
		
	class Row_Manufacturer* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Manufacturer : public TableRow, public SerializeClass
	{
		friend struct Table_Manufacturer::Key;
		friend class Table_Manufacturer;
	private:
		Table_Manufacturer *table;
		
		long int m_PK_Manufacturer;
long int m_FK_NameHash;

		bool is_null[2];
	
	public:
		long int PK_Manufacturer_get();
long int FK_NameHash_get();

		
		void PK_Manufacturer_set(long int val);
void FK_NameHash_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Manufacturer(Table_Manufacturer *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Manufacturer *Table_Manufacturer_get() { return table; };

		// Return the rows for foreign keys 
		class Row_NameHash* FK_NameHash_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Game_GameSystem_FK_Manufacturer_getrows(vector <class Row_Game_GameSystem*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Manufacturer+ m_FK_NameHash;
		}
	private:
		void SetDefaultValues();
		
		string PK_Manufacturer_asSQL();
string FK_NameHash_asSQL();

	};

#endif

