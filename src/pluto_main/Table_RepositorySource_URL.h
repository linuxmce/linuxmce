/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_RepositorySource_URL_H__
#define __Table_RepositorySource_URL_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RepositorySource_URL.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_RepositorySource_URL : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_RepositorySource_URL(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_RepositorySource_URL();

private:		
	friend class Row_RepositorySource_URL;
	struct Key
	{
		friend class Row_RepositorySource_URL;
		long int pk_PK_RepositorySource_URL;

		
		Key(long int in_PK_RepositorySource_URL);
	
		Key(class Row_RepositorySource_URL *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RepositorySource_URL::Key &key1, const Table_RepositorySource_URL::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_RepositorySource_URL*> *rows);
	class Row_RepositorySource_URL* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RepositorySource_URL* GetRow(long int in_PK_RepositorySource_URL);
	

private:	
	
		
	class Row_RepositorySource_URL* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_RepositorySource_URL : public TableRow, public SerializeClass
	{
		friend struct Table_RepositorySource_URL::Key;
		friend class Table_RepositorySource_URL;
	private:
		Table_RepositorySource_URL *table;
		
		long int m_PK_RepositorySource_URL;
long int m_FK_RepositorySource;
string m_URL;
long int m_FK_Country;
string m_Username;
string m_Password;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[13];
	
	public:
		long int PK_RepositorySource_URL_get();
long int FK_RepositorySource_get();
string URL_get();
long int FK_Country_get();
string Username_get();
string Password_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_RepositorySource_URL_set(long int val);
void FK_RepositorySource_set(long int val);
void URL_set(string val);
void FK_Country_set(long int val);
void Username_set(string val);
void Password_set(string val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Country_isNull();
bool Username_isNull();
bool Comments_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Country_setNull(bool val);
void Username_setNull(bool val);
void Comments_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RepositorySource_URL(Table_RepositorySource_URL *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RepositorySource_URL *Table_RepositorySource_URL_get() { return table; };

		// Return the rows for foreign keys 
		class Row_RepositorySource* FK_RepositorySource_getrow();
class Row_Country* FK_Country_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Installation_RepositorySource_URL_FK_RepositorySource_URL_getrows(vector <class Row_Installation_RepositorySource_URL*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_RepositorySource_URL+ m_FK_RepositorySource+ m_URL+ m_FK_Country+ m_Username+ m_Password+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_RepositorySource_URL_asSQL();
string FK_RepositorySource_asSQL();
string URL_asSQL();
string FK_Country_asSQL();
string Username_asSQL();
string Password_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

