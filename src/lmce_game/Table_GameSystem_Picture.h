/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_GameSystem_Picture_H__
#define __Table_GameSystem_Picture_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_GameSystem_Picture.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_GameSystem_Picture : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_GameSystem_Picture(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_GameSystem_Picture();

private:		
	friend class Row_GameSystem_Picture;
	struct Key
	{
		friend class Row_GameSystem_Picture;
		long int pk_PK_GameSystem_Picture;

		
		Key(long int in_PK_GameSystem_Picture);
	
		Key(class Row_GameSystem_Picture *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_GameSystem_Picture::Key &key1, const Table_GameSystem_Picture::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_GameSystem_Picture*> *rows);
	class Row_GameSystem_Picture* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_GameSystem_Picture* GetRow(long int in_PK_GameSystem_Picture);
	

private:	
	
		
	class Row_GameSystem_Picture* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_GameSystem_Picture : public TableRow, public SerializeClass
	{
		friend struct Table_GameSystem_Picture::Key;
		friend class Table_GameSystem_Picture;
	private:
		Table_GameSystem_Picture *table;
		
		long int m_PK_GameSystem_Picture;
long int m_FK_GameSystem;
long int m_FK_Picture;

		bool is_null[3];
	
	public:
		long int PK_GameSystem_Picture_get();
long int FK_GameSystem_get();
long int FK_Picture_get();

		
		void PK_GameSystem_Picture_set(long int val);
void FK_GameSystem_set(long int val);
void FK_Picture_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_GameSystem_Picture(Table_GameSystem_Picture *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_GameSystem_Picture *Table_GameSystem_Picture_get() { return table; };

		// Return the rows for foreign keys 
		class Row_GameSystem* FK_GameSystem_getrow();
class Row_Picture* FK_Picture_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_GameSystem_Picture+ m_FK_GameSystem+ m_FK_Picture;
		}
	private:
		void SetDefaultValues();
		
		string PK_GameSystem_Picture_asSQL();
string FK_GameSystem_asSQL();
string FK_Picture_asSQL();

	};

#endif

