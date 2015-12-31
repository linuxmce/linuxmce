/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CHRONICLE_H__
#define __Table_CHRONICLE_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CHRONICLE.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CHRONICLE : public TableBase , SingleLongKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_CHRONICLE(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_CHRONICLE();

private:		
	friend class Row_CHRONICLE;
	struct Key
	{
		friend class Row_CHRONICLE;
		long int pk_id;

		
		Key(long int in_id);
	
		Key(class Row_CHRONICLE *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CHRONICLE::Key &key1, const Table_CHRONICLE::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CHRONICLE*> *rows);
	class Row_CHRONICLE* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_CHRONICLE* GetRow(long int in_id);
	

private:	
	
		
	class Row_CHRONICLE* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CHRONICLE : public TableRow, public SerializeClass
	{
		friend struct Table_CHRONICLE::Key;
		friend class Table_CHRONICLE;
	private:
		Table_CHRONICLE *table;
		
		long int m_id;
string m_hash;
string m_title;
string m_channel_id;
string m_starttime;
long int m_duration;

		bool is_null[6];
	
	public:
		long int id_get();
string hash_get();
string title_get();
string channel_id_get();
string starttime_get();
long int duration_get();

		
		void id_set(long int val);
void hash_set(string val);
void title_set(string val);
void channel_id_set(string val);
void starttime_set(string val);
void duration_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_CHRONICLE(Table_CHRONICLE *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CHRONICLE *Table_CHRONICLE_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_id+ m_hash+ m_title+ m_channel_id+ m_starttime+ m_duration;
		}
	private:
		void SetDefaultValues();
		
		string id_asSQL();
string hash_asSQL();
string title_asSQL();
string channel_id_asSQL();
string starttime_asSQL();
string duration_asSQL();

	};

#endif

