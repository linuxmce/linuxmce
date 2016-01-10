/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Keyword_H__
#define __Table_Keyword_H__

#include "TableRow.h"
#include "Database_pluto_speech.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Keyword.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Keyword : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_speech *database;
	struct Key;	//forward declaration
	
public:
	Table_Keyword(Database_pluto_speech *pDatabase):database(pDatabase)
	{
	};
	~Table_Keyword();

private:		
	friend class Row_Keyword;
	struct Key
	{
		friend class Row_Keyword;
		long int pk_PK_Keyword;

		
		Key(long int in_PK_Keyword);
	
		Key(class Row_Keyword *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Keyword::Key &key1, const Table_Keyword::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Keyword*> *rows);
	class Row_Keyword* AddRow();
	Database_pluto_speech *Database_pluto_speech_get() { return database; }
	
		
	class Row_Keyword* GetRow(long int in_PK_Keyword);
	

private:	
	
		
	class Row_Keyword* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Keyword : public TableRow, public SerializeClass
	{
		friend struct Table_Keyword::Key;
		friend class Table_Keyword;
	private:
		Table_Keyword *table;
		
		long int m_PK_Keyword;
short int m_SimpleAdvanced;
long int m_EK_Room;
string m_Keyword;
long int m_EK_CommandGroup;

		bool is_null[5];
	
	public:
		long int PK_Keyword_get();
short int SimpleAdvanced_get();
long int EK_Room_get();
string Keyword_get();
long int EK_CommandGroup_get();

		
		void PK_Keyword_set(long int val);
void SimpleAdvanced_set(short int val);
void EK_Room_set(long int val);
void Keyword_set(string val);
void EK_CommandGroup_set(long int val);

		
		bool SimpleAdvanced_isNull();
bool EK_Room_isNull();
bool Keyword_isNull();
bool EK_CommandGroup_isNull();

			
		void SimpleAdvanced_setNull(bool val);
void EK_Room_setNull(bool val);
void Keyword_setNull(bool val);
void EK_CommandGroup_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Keyword(Table_Keyword *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Keyword *Table_Keyword_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Keyword+ m_SimpleAdvanced+ m_EK_Room+ m_Keyword+ m_EK_CommandGroup;
		}
	private:
		void SetDefaultValues();
		
		string PK_Keyword_asSQL();
string SimpleAdvanced_asSQL();
string EK_Room_asSQL();
string Keyword_asSQL();
string EK_CommandGroup_asSQL();

	};

#endif

