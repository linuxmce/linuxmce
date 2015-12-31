/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Alias_H__
#define __Table_Alias_H__

#include "TableRow.h"
#include "Database_pluto_speech.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Alias.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Alias : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_speech *database;
	struct Key;	//forward declaration
	
public:
	Table_Alias(Database_pluto_speech *pDatabase):database(pDatabase)
	{
	};
	~Table_Alias();

private:		
	friend class Row_Alias;
	struct Key
	{
		friend class Row_Alias;
		long int pk_PK_Alias;

		
		Key(long int in_PK_Alias);
	
		Key(class Row_Alias *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Alias::Key &key1, const Table_Alias::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Alias*> *rows);
	class Row_Alias* AddRow();
	Database_pluto_speech *Database_pluto_speech_get() { return database; }
	
		
	class Row_Alias* GetRow(long int in_PK_Alias);
	

private:	
	
		
	class Row_Alias* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Alias : public TableRow, public SerializeClass
	{
		friend struct Table_Alias::Key;
		friend class Table_Alias;
	private:
		Table_Alias *table;
		
		long int m_PK_Alias;
short int m_SimpleAdvanced;
string m_Keyword_Input;
string m_Keyword_Meaning;

		bool is_null[4];
	
	public:
		long int PK_Alias_get();
short int SimpleAdvanced_get();
string Keyword_Input_get();
string Keyword_Meaning_get();

		
		void PK_Alias_set(long int val);
void SimpleAdvanced_set(short int val);
void Keyword_Input_set(string val);
void Keyword_Meaning_set(string val);

		
		bool SimpleAdvanced_isNull();
bool Keyword_Input_isNull();
bool Keyword_Meaning_isNull();

			
		void SimpleAdvanced_setNull(bool val);
void Keyword_Input_setNull(bool val);
void Keyword_Meaning_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Alias(Table_Alias *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Alias *Table_Alias_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Alias+ m_SimpleAdvanced+ m_Keyword_Input+ m_Keyword_Meaning;
		}
	private:
		void SetDefaultValues();
		
		string PK_Alias_asSQL();
string SimpleAdvanced_asSQL();
string Keyword_Input_asSQL();
string Keyword_Meaning_asSQL();

	};

#endif

