/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Genre_NameHash_H__
#define __Table_Genre_NameHash_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Genre_NameHash.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Genre_NameHash : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Genre_NameHash(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Genre_NameHash();

private:		
	friend class Row_Genre_NameHash;
	struct Key
	{
		friend class Row_Genre_NameHash;
		long int pk_PK_Genre_NameHash;

		
		Key(long int in_PK_Genre_NameHash);
	
		Key(class Row_Genre_NameHash *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Genre_NameHash::Key &key1, const Table_Genre_NameHash::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Genre_NameHash*> *rows);
	class Row_Genre_NameHash* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Genre_NameHash* GetRow(long int in_PK_Genre_NameHash);
	

private:	
	
		
	class Row_Genre_NameHash* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Genre_NameHash : public TableRow, public SerializeClass
	{
		friend struct Table_Genre_NameHash::Key;
		friend class Table_Genre_NameHash;
	private:
		Table_Genre_NameHash *table;
		
		long int m_PK_Genre_NameHash;
long int m_FK_Genre;
string m_NameHash;

		bool is_null[3];
	
	public:
		long int PK_Genre_NameHash_get();
long int FK_Genre_get();
string NameHash_get();

		
		void PK_Genre_NameHash_set(long int val);
void FK_Genre_set(long int val);
void NameHash_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Genre_NameHash(Table_Genre_NameHash *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Genre_NameHash *Table_Genre_NameHash_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Genre* FK_Genre_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Genre_NameHash+ m_FK_Genre+ m_NameHash;
		}
	private:
		void SetDefaultValues();
		
		string PK_Genre_NameHash_asSQL();
string FK_Genre_asSQL();
string NameHash_asSQL();

	};

#endif

