/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Installation_Users_H__
#define __Table_Installation_Users_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Installation_Users.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Installation_Users : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Installation_Users(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Installation_Users();

private:		
	friend class Row_Installation_Users;
	struct Key
	{
		friend class Row_Installation_Users;
		long int pk_FK_Installation;
long int pk_FK_Users;

		
		Key(long int in_FK_Installation, long int in_FK_Users);
	
		Key(class Row_Installation_Users *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Installation_Users::Key &key1, const Table_Installation_Users::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Installation_Users*> *rows);
	class Row_Installation_Users* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Installation_Users* GetRow(long int in_FK_Installation, long int in_FK_Users);
	

private:	
	
		
	class Row_Installation_Users* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Installation_Users : public TableRow, public SerializeClass
	{
		friend struct Table_Installation_Users::Key;
		friend class Table_Installation_Users;
	private:
		Table_Installation_Users *table;
		
		long int m_FK_Installation;
long int m_FK_Users;
short int m_userCanModifyInstallation;
short int m_userCanChangeHouseMode;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int FK_Installation_get();
long int FK_Users_get();
short int userCanModifyInstallation_get();
short int userCanChangeHouseMode_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Installation_set(long int val);
void FK_Users_set(long int val);
void userCanModifyInstallation_set(short int val);
void userCanChangeHouseMode_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool userCanChangeHouseMode_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void userCanChangeHouseMode_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Installation_Users(Table_Installation_Users *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Installation_Users *Table_Installation_Users_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Installation* FK_Installation_getrow();
class Row_Users* FK_Users_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Installation+ m_FK_Users+ m_userCanModifyInstallation+ m_userCanChangeHouseMode+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Installation_asSQL();
string FK_Users_asSQL();
string userCanModifyInstallation_asSQL();
string userCanChangeHouseMode_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

