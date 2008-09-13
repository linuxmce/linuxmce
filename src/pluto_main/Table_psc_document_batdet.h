/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_psc_document_batdet_H__
#define __Table_psc_document_batdet_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_document_batdet.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_psc_document_batdet : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_psc_document_batdet(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_psc_document_batdet();

private:		
	friend class Row_psc_document_batdet;
	struct Key
	{
		friend class Row_psc_document_batdet;
		long int pk_PK_psc_document_batdet;

		
		Key(long int in_PK_psc_document_batdet);
	
		Key(class Row_psc_document_batdet *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_document_batdet::Key &key1, const Table_psc_document_batdet::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_psc_document_batdet*> *rows);
	class Row_psc_document_batdet* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_psc_document_batdet* GetRow(long int in_PK_psc_document_batdet);
	

private:	
	
		
	class Row_psc_document_batdet* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_psc_document_batdet : public TableRow, public SerializeClass
	{
		friend struct Table_psc_document_batdet::Key;
		friend class Table_psc_document_batdet;
	private:
		Table_psc_document_batdet *table;
		
		long int m_PK_psc_document_batdet;
long int m_FK_psc_document_bathdr;
string m_Tablename;
long int m_New;
long int m_Deleted;
long int m_Modified;
long int m_FK_psc_document_bathdr_orig;
long int m_FK_psc_document_bathdr_auth;
long int m_FK_psc_document_bathdr_unauth;

		bool is_null[9];
	
	public:
		long int PK_psc_document_batdet_get();
long int FK_psc_document_bathdr_get();
string Tablename_get();
long int New_get();
long int Deleted_get();
long int Modified_get();
long int FK_psc_document_bathdr_orig_get();
long int FK_psc_document_bathdr_auth_get();
long int FK_psc_document_bathdr_unauth_get();

		
		void PK_psc_document_batdet_set(long int val);
void FK_psc_document_bathdr_set(long int val);
void Tablename_set(string val);
void New_set(long int val);
void Deleted_set(long int val);
void Modified_set(long int val);
void FK_psc_document_bathdr_orig_set(long int val);
void FK_psc_document_bathdr_auth_set(long int val);
void FK_psc_document_bathdr_unauth_set(long int val);

		
		
			
			
	
		void Delete();
		void Reload();		
	
		Row_psc_document_batdet(Table_psc_document_batdet *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_document_batdet *Table_psc_document_batdet_get() { return table; };

		// Return the rows for foreign keys 
		class Row_psc_document_bathdr* FK_psc_document_bathdr_getrow();
class Row_psc_document_bathdr* FK_psc_document_bathdr_orig_getrow();
class Row_psc_document_bathdr* FK_psc_document_bathdr_auth_getrow();
class Row_psc_document_bathdr* FK_psc_document_bathdr_unauth_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_document_batdet+ m_FK_psc_document_bathdr+ m_Tablename+ m_New+ m_Deleted+ m_Modified+ m_FK_psc_document_bathdr_orig+ m_FK_psc_document_bathdr_auth+ m_FK_psc_document_bathdr_unauth;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_document_batdet_asSQL();
string FK_psc_document_bathdr_asSQL();
string Tablename_asSQL();
string New_asSQL();
string Deleted_asSQL();
string Modified_asSQL();
string FK_psc_document_bathdr_orig_asSQL();
string FK_psc_document_bathdr_auth_asSQL();
string FK_psc_document_bathdr_unauth_asSQL();

	};

#endif

