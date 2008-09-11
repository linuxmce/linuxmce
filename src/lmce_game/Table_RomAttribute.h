/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_RomAttribute_H__
#define __Table_RomAttribute_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RomAttribute.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_RomAttribute : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_RomAttribute(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_RomAttribute();

private:		
	friend class Row_RomAttribute;
	struct Key
	{
		friend class Row_RomAttribute;
		long int pk_PK_RomAttribute;

		
		Key(long int in_PK_RomAttribute);
	
		Key(class Row_RomAttribute *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RomAttribute::Key &key1, const Table_RomAttribute::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_RomAttribute*> *rows);
	class Row_RomAttribute* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_RomAttribute* GetRow(long int in_PK_RomAttribute);
	

private:	
	
		
	class Row_RomAttribute* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_RomAttribute : public TableRow, public SerializeClass
	{
		friend struct Table_RomAttribute::Key;
		friend class Table_RomAttribute;
	private:
		Table_RomAttribute *table;
		
		long int m_PK_RomAttribute;
long int m_FK_RomAttributeType;
long int m_FK_GameSystem;
string m_Name;

		bool is_null[4];
	
	public:
		long int PK_RomAttribute_get();
long int FK_RomAttributeType_get();
long int FK_GameSystem_get();
string Name_get();

		
		void PK_RomAttribute_set(long int val);
void FK_RomAttributeType_set(long int val);
void FK_GameSystem_set(long int val);
void Name_set(string val);

		
		bool Name_isNull();

			
		void Name_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RomAttribute(Table_RomAttribute *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RomAttribute *Table_RomAttribute_get() { return table; };

		// Return the rows for foreign keys 
		class Row_RomAttributeType* FK_RomAttributeType_getrow();
class Row_GameSystem* FK_GameSystem_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Rom_RomAttribute_FK_RomAttribute_getrows(vector <class Row_Rom_RomAttribute*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_RomAttribute+ m_FK_RomAttributeType+ m_FK_GameSystem+ m_Name;
		}
	private:
		void SetDefaultValues();
		
		string PK_RomAttribute_asSQL();
string FK_RomAttributeType_asSQL();
string FK_GameSystem_asSQL();
string Name_asSQL();

	};

#endif

