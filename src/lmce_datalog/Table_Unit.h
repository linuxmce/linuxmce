/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Unit_H__
#define __Table_Unit_H__

#include "TableRow.h"
#include "Database_lmce_datalog.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Unit.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Unit : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_datalog *database;
	struct Key;	//forward declaration
	
public:
	Table_Unit(Database_lmce_datalog *pDatabase):database(pDatabase)
	{
	};
	~Table_Unit();

private:		
	friend class Row_Unit;
	struct Key
	{
		friend class Row_Unit;
		long int pk_PK_Unit;

		
		Key(long int in_PK_Unit);
	
		Key(class Row_Unit *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Unit::Key &key1, const Table_Unit::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Unit*> *rows);
	class Row_Unit* AddRow();
	Database_lmce_datalog *Database_lmce_datalog_get() { return database; }
	
		
	class Row_Unit* GetRow(long int in_PK_Unit);
	

private:	
	
		
	class Row_Unit* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Unit : public TableRow, public SerializeClass
	{
		friend struct Table_Unit::Key;
		friend class Table_Unit;
	private:
		Table_Unit *table;
		
		long int m_PK_Unit;
string m_Description;
string m_Unit;
long int m_Precision;

		bool is_null[4];
	
	public:
		long int PK_Unit_get();
string Description_get();
string Unit_get();
long int Precision_get();

		
		void PK_Unit_set(long int val);
void Description_set(string val);
void Unit_set(string val);
void Precision_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_Unit(Table_Unit *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Unit *Table_Unit_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Datapoints_FK_Unit_getrows(vector <class Row_Datapoints*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Unit+ m_Description+ m_Unit+ m_Precision;
		}
	private:
		void SetDefaultValues();
		
		string PK_Unit_asSQL();
string Description_asSQL();
string Unit_asSQL();
string Precision_asSQL();

	};

#endif

