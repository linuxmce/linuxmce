/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_psc_designer_tables_H__
#define __Table_psc_designer_tables_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_designer_tables.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_psc_designer_tables : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_psc_designer_tables(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_psc_designer_tables();

private:		
	friend class Row_psc_designer_tables;
	struct Key
	{
		friend class Row_psc_designer_tables;
		long int pk_PK_psc_designer_tables;

		
		Key(long int in_PK_psc_designer_tables);
	
		Key(class Row_psc_designer_tables *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_designer_tables::Key &key1, const Table_psc_designer_tables::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_psc_designer_tables*> *rows);
	class Row_psc_designer_tables* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_designer_tables* GetRow(long int in_PK_psc_designer_tables);
	

private:	
	
		
	class Row_psc_designer_tables* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_psc_designer_tables : public TableRow, public SerializeClass
	{
		friend struct Table_psc_designer_tables::Key;
		friend class Table_psc_designer_tables;
	private:
		Table_psc_designer_tables *table;
		
		long int m_PK_psc_designer_tables;
string m_Tablename;
string m_filter;
short int m_frozen;
short int m_anonymous;
long int m_last_psc_id;
long int m_last_psc_batch;

		bool is_null[7];
	
	public:
		long int PK_psc_designer_tables_get();
string Tablename_get();
string filter_get();
short int frozen_get();
short int anonymous_get();
long int last_psc_id_get();
long int last_psc_batch_get();

		
		void PK_psc_designer_tables_set(long int val);
void Tablename_set(string val);
void filter_set(string val);
void frozen_set(short int val);
void anonymous_set(short int val);
void last_psc_id_set(long int val);
void last_psc_batch_set(long int val);

		
		bool filter_isNull();

			
		void filter_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_designer_tables(Table_psc_designer_tables *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_designer_tables *Table_psc_designer_tables_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_designer_tables+ m_Tablename+ m_filter+ m_frozen+ m_anonymous+ m_last_psc_id+ m_last_psc_batch;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_designer_tables_asSQL();
string Tablename_asSQL();
string filter_asSQL();
string frozen_asSQL();
string anonymous_asSQL();
string last_psc_id_asSQL();
string last_psc_batch_asSQL();

	};

#endif

