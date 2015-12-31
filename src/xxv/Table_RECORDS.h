/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_RECORDS_H__
#define __Table_RECORDS_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RECORDS.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_RECORDS : public TableBase , SingleLong64KeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_RECORDS(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_RECORDS();

private:		
	friend class Row_RECORDS;
	struct Key
	{
		friend class Row_RECORDS;
		u_int64_t pk_eventid;

		
		Key(u_int64_t in_eventid);
	
		Key(class Row_RECORDS *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RECORDS::Key &key1, const Table_RECORDS::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_RECORDS*> *rows);
	class Row_RECORDS* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_RECORDS* GetRow(u_int64_t in_eventid);
	

private:	
	
		
	class Row_RECORDS* FetchRow(SingleLong64Key &key);
		
			
};

class DECLSPECIFIER Row_RECORDS : public TableRow, public SerializeClass
	{
		friend struct Table_RECORDS::Key;
		friend class Table_RECORDS;
	private:
		Table_RECORDS *table;
		
		u_int64_t m_eventid;
long int m_RecordId;
string m_Path;
short int m_Prio;
short int m_Lifetime;
short int m_State;
string m_Marks;
string m_Type;
string m_addtime;

		bool is_null[9];
	
	public:
		u_int64_t eventid_get();
long int RecordId_get();
string Path_get();
short int Prio_get();
short int Lifetime_get();
short int State_get();
string Marks_get();
string Type_get();
string addtime_get();

		
		void eventid_set(u_int64_t val);
void RecordId_set(long int val);
void Path_set(string val);
void Prio_set(short int val);
void Lifetime_set(short int val);
void State_set(short int val);
void Marks_set(string val);
void Type_set(string val);
void addtime_set(string val);

		
		bool Marks_isNull();
bool Type_isNull();

			
		void Marks_setNull(bool val);
void Type_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RECORDS(Table_RECORDS *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RECORDS *Table_RECORDS_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_eventid+ m_RecordId+ m_Path+ m_Prio+ m_Lifetime+ m_State+ m_Marks+ m_Type+ m_addtime;
		}
	private:
		void SetDefaultValues();
		
		string eventid_asSQL();
string RecordId_asSQL();
string Path_asSQL();
string Prio_asSQL();
string Lifetime_asSQL();
string State_asSQL();
string Marks_asSQL();
string Type_asSQL();
string addtime_asSQL();

	};

#endif

