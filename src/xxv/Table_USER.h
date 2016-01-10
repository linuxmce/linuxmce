/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_USER_H__
#define __Table_USER_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_USER.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_USER : public TableBase , SingleLongKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_USER(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_USER();

private:		
	friend class Row_USER;
	struct Key
	{
		friend class Row_USER;
		long int pk_Id;

		
		Key(long int in_Id);
	
		Key(class Row_USER *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_USER::Key &key1, const Table_USER::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_USER*> *rows);
	class Row_USER* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_USER* GetRow(long int in_Id);
	

private:	
	
		
	class Row_USER* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_USER : public TableRow, public SerializeClass
	{
		friend struct Table_USER::Key;
		friend class Table_USER;
	private:
		Table_USER *table;
		
		long int m_Id;
string m_Name;
string m_Password;
string m_Level;
string m_Prefs;
string m_UserPrefs;
string m_Deny;
short int m_MaxLifeTime;
short int m_MaxPriority;

		bool is_null[9];
	
	public:
		long int Id_get();
string Name_get();
string Password_get();
string Level_get();
string Prefs_get();
string UserPrefs_get();
string Deny_get();
short int MaxLifeTime_get();
short int MaxPriority_get();

		
		void Id_set(long int val);
void Name_set(string val);
void Password_set(string val);
void Level_set(string val);
void Prefs_set(string val);
void UserPrefs_set(string val);
void Deny_set(string val);
void MaxLifeTime_set(short int val);
void MaxPriority_set(short int val);

		
		bool Prefs_isNull();
bool UserPrefs_isNull();
bool Deny_isNull();
bool MaxLifeTime_isNull();
bool MaxPriority_isNull();

			
		void Prefs_setNull(bool val);
void UserPrefs_setNull(bool val);
void Deny_setNull(bool val);
void MaxLifeTime_setNull(bool val);
void MaxPriority_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_USER(Table_USER *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_USER *Table_USER_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_Id+ m_Name+ m_Password+ m_Level+ m_Prefs+ m_UserPrefs+ m_Deny+ m_MaxLifeTime+ m_MaxPriority;
		}
	private:
		void SetDefaultValues();
		
		string Id_asSQL();
string Name_asSQL();
string Password_asSQL();
string Level_asSQL();
string Prefs_asSQL();
string UserPrefs_asSQL();
string Deny_asSQL();
string MaxLifeTime_asSQL();
string MaxPriority_asSQL();

	};

#endif

