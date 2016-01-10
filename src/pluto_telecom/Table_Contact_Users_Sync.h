/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Contact_Users_Sync_H__
#define __Table_Contact_Users_Sync_H__

#include "TableRow.h"
#include "Database_pluto_telecom.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Contact_Users_Sync.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Contact_Users_Sync : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_telecom *database;
	struct Key;	//forward declaration
	
public:
	Table_Contact_Users_Sync(Database_pluto_telecom *pDatabase):database(pDatabase)
	{
	};
	~Table_Contact_Users_Sync();

private:		
	friend class Row_Contact_Users_Sync;
	struct Key
	{
		friend class Row_Contact_Users_Sync;
		long int pk_FK_Contact;
long int pk_EK_Users;

		
		Key(long int in_FK_Contact, long int in_EK_Users);
	
		Key(class Row_Contact_Users_Sync *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Contact_Users_Sync::Key &key1, const Table_Contact_Users_Sync::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Contact_Users_Sync*> *rows);
	class Row_Contact_Users_Sync* AddRow();
	Database_pluto_telecom *Database_pluto_telecom_get() { return database; }
	
		
	class Row_Contact_Users_Sync* GetRow(long int in_FK_Contact, long int in_EK_Users);
	

private:	
	
		
	class Row_Contact_Users_Sync* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Contact_Users_Sync : public TableRow, public SerializeClass
	{
		friend struct Table_Contact_Users_Sync::Key;
		friend class Table_Contact_Users_Sync;
	private:
		Table_Contact_Users_Sync *table;
		
		long int m_FK_Contact;
long int m_EK_Users;
string m_SyncID;
string m_LastModPluto;
string m_LastModSync;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int FK_Contact_get();
long int EK_Users_get();
string SyncID_get();
string LastModPluto_get();
string LastModSync_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Contact_set(long int val);
void EK_Users_set(long int val);
void SyncID_set(string val);
void LastModPluto_set(string val);
void LastModSync_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool SyncID_isNull();
bool LastModPluto_isNull();
bool LastModSync_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void SyncID_setNull(bool val);
void LastModPluto_setNull(bool val);
void LastModSync_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Contact_Users_Sync(Table_Contact_Users_Sync *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Contact_Users_Sync *Table_Contact_Users_Sync_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Contact* FK_Contact_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Contact+ m_EK_Users+ m_SyncID+ m_LastModPluto+ m_LastModSync+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Contact_asSQL();
string EK_Users_asSQL();
string SyncID_asSQL();
string LastModPluto_asSQL();
string LastModSync_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

