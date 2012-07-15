/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_psc_game_repset_H__
#define __Table_psc_game_repset_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_game_repset.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_psc_game_repset : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_psc_game_repset(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_psc_game_repset();

private:		
	friend class Row_psc_game_repset;
	struct Key
	{
		friend class Row_psc_game_repset;
		long int pk_PK_psc_game_repset;

		
		Key(long int in_PK_psc_game_repset);
	
		Key(class Row_psc_game_repset *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_game_repset::Key &key1, const Table_psc_game_repset::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_psc_game_repset*> *rows);
	class Row_psc_game_repset* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_psc_game_repset* GetRow(long int in_PK_psc_game_repset);
	

private:	
	
		
	class Row_psc_game_repset* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_psc_game_repset : public TableRow, public SerializeClass
	{
		friend struct Table_psc_game_repset::Key;
		friend class Table_psc_game_repset;
	private:
		Table_psc_game_repset *table;
		
		long int m_PK_psc_game_repset;
string m_Setting;
string m_Value;

		bool is_null[3];
	
	public:
		long int PK_psc_game_repset_get();
string Setting_get();
string Value_get();

		
		void PK_psc_game_repset_set(long int val);
void Setting_set(string val);
void Value_set(string val);

		
		bool Value_isNull();

			
		void Value_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_game_repset(Table_psc_game_repset *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_game_repset *Table_psc_game_repset_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_game_repset+ m_Setting+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_game_repset_asSQL();
string Setting_asSQL();
string Value_asSQL();

	};

#endif

