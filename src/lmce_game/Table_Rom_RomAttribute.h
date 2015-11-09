/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Rom_RomAttribute_H__
#define __Table_Rom_RomAttribute_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Rom_RomAttribute.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Rom_RomAttribute : public TableBase , DoubleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_Rom_RomAttribute(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_Rom_RomAttribute();

private:		
	friend class Row_Rom_RomAttribute;
	struct Key
	{
		friend class Row_Rom_RomAttribute;
		long int pk_FK_Rom;
long int pk_FK_RomAttribute;

		
		Key(long int in_FK_Rom, long int in_FK_RomAttribute);
	
		Key(class Row_Rom_RomAttribute *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Rom_RomAttribute::Key &key1, const Table_Rom_RomAttribute::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Rom_RomAttribute*> *rows);
	class Row_Rom_RomAttribute* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_Rom_RomAttribute* GetRow(long int in_FK_Rom, long int in_FK_RomAttribute);
	

private:	
	
		
	class Row_Rom_RomAttribute* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Rom_RomAttribute : public TableRow, public SerializeClass
	{
		friend struct Table_Rom_RomAttribute::Key;
		friend class Table_Rom_RomAttribute;
	private:
		Table_Rom_RomAttribute *table;
		
		long int m_FK_Rom;
long int m_FK_RomAttribute;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[8];
	
	public:
		long int FK_Rom_get();
long int FK_RomAttribute_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Rom_set(long int val);
void FK_RomAttribute_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Rom_RomAttribute(Table_Rom_RomAttribute *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Rom_RomAttribute *Table_Rom_RomAttribute_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Rom* FK_Rom_getrow();
class Row_RomAttribute* FK_RomAttribute_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Rom+ m_FK_RomAttribute+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Rom_asSQL();
string FK_RomAttribute_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

