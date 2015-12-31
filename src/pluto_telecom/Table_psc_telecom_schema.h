/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_psc_telecom_schema_H__
#define __Table_psc_telecom_schema_H__

#include "TableRow.h"
#include "Database_pluto_telecom.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_telecom_schema.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_psc_telecom_schema : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_telecom *database;
	struct Key;	//forward declaration
	
public:
	Table_psc_telecom_schema(Database_pluto_telecom *pDatabase):database(pDatabase)
	{
	};
	~Table_psc_telecom_schema();

private:		
	friend class Row_psc_telecom_schema;
	struct Key
	{
		friend class Row_psc_telecom_schema;
		long int pk_PK_psc_telecom_schema;

		
		Key(long int in_PK_psc_telecom_schema);
	
		Key(class Row_psc_telecom_schema *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_telecom_schema::Key &key1, const Table_psc_telecom_schema::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_psc_telecom_schema*> *rows);
	class Row_psc_telecom_schema* AddRow();
	Database_pluto_telecom *Database_pluto_telecom_get() { return database; }
	
		
	class Row_psc_telecom_schema* GetRow(long int in_PK_psc_telecom_schema);
	

private:	
	
		
	class Row_psc_telecom_schema* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_psc_telecom_schema : public TableRow, public SerializeClass
	{
		friend struct Table_psc_telecom_schema::Key;
		friend class Table_psc_telecom_schema;
	private:
		Table_psc_telecom_schema *table;
		
		long int m_PK_psc_telecom_schema;
string m_Value;

		bool is_null[2];
	
	public:
		long int PK_psc_telecom_schema_get();
string Value_get();

		
		void PK_psc_telecom_schema_set(long int val);
void Value_set(string val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_telecom_schema(Table_psc_telecom_schema *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_telecom_schema *Table_psc_telecom_schema_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_telecom_schema+ m_Value;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_telecom_schema_asSQL();
string Value_asSQL();

	};

#endif

