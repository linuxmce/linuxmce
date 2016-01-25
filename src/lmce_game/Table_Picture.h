/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Picture_H__
#define __Table_Picture_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Picture.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Picture : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Picture(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Picture();

private:		
	friend class Row_Picture;
	struct Key
	{
		friend class Row_Picture;
		long int pk_PK_Picture;

		
		Key(long int in_PK_Picture);
	
		Key(class Row_Picture *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Picture::Key &key1, const Table_Picture::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Picture*> *rows);
	class Row_Picture* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Picture* GetRow(long int in_PK_Picture);
	

private:	
	
		
	class Row_Picture* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Picture : public TableRow, public SerializeClass
	{
		friend struct Table_Picture::Key;
		friend class Table_Picture;
	private:
		Table_Picture *table;
		
		long int m_PK_Picture;
string m_URL;

		bool is_null[2];
	
	public:
		long int PK_Picture_get();
string URL_get();

		
		void PK_Picture_set(long int val);
void URL_set(string val);

		
		bool URL_isNull();

			
		void URL_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Picture(Table_Picture *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Picture *Table_Picture_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void GameSystem_Picture_FK_Picture_getrows(vector <class Row_GameSystem_Picture*> *rows);
void Game_GameSystem_Picture_FK_Picture_getrows(vector <class Row_Game_GameSystem_Picture*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Picture+ m_URL;
		}
	private:
		void SetDefaultValues();
		
		string PK_Picture_asSQL();
string URL_asSQL();

	};

#endif

