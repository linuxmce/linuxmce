/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CHANNELGROUPS_H__
#define __Table_CHANNELGROUPS_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CHANNELGROUPS.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CHANNELGROUPS : public TableBase , SingleLongKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_CHANNELGROUPS(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_CHANNELGROUPS();

private:		
	friend class Row_CHANNELGROUPS;
	struct Key
	{
		friend class Row_CHANNELGROUPS;
		long int pk_Id;

		
		Key(long int in_Id);
	
		Key(class Row_CHANNELGROUPS *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CHANNELGROUPS::Key &key1, const Table_CHANNELGROUPS::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CHANNELGROUPS*> *rows);
	class Row_CHANNELGROUPS* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_CHANNELGROUPS* GetRow(long int in_Id);
	

private:	
	
		
	class Row_CHANNELGROUPS* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CHANNELGROUPS : public TableRow, public SerializeClass
	{
		friend struct Table_CHANNELGROUPS::Key;
		friend class Table_CHANNELGROUPS;
	private:
		Table_CHANNELGROUPS *table;
		
		long int m_Id;
string m_Name;
long int m_Counter;

		bool is_null[3];
	
	public:
		long int Id_get();
string Name_get();
long int Counter_get();

		
		void Id_set(long int val);
void Name_set(string val);
void Counter_set(long int val);

		
		bool Name_isNull();
bool Counter_isNull();

			
		void Name_setNull(bool val);
void Counter_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CHANNELGROUPS(Table_CHANNELGROUPS *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CHANNELGROUPS *Table_CHANNELGROUPS_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_Id+ m_Name+ m_Counter;
		}
	private:
		void SetDefaultValues();
		
		string Id_asSQL();
string Name_asSQL();
string Counter_asSQL();

	};

#endif

