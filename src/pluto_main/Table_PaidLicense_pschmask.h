/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_PaidLicense_pschmask_H__
#define __Table_PaidLicense_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PaidLicense_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PaidLicense_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_PaidLicense_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_PaidLicense_pschmask();

private:		
	friend class Row_PaidLicense_pschmask;
	struct Key
	{
		friend class Row_PaidLicense_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_PaidLicense_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PaidLicense_pschmask::Key &key1, const Table_PaidLicense_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_PaidLicense_pschmask*> *rows);
	class Row_PaidLicense_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PaidLicense_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_PaidLicense_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PaidLicense_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_PaidLicense_pschmask::Key;
		friend class Table_PaidLicense_pschmask;
	private:
		Table_PaidLicense_pschmask *table;
		
		short int m_PK_PaidLicense;
short int m_FK_Device;
short int m_UseWithoutLicense;
string m_ValidUntil;
short int m_FK_Package;
short int m_FK_License;
short int m_ReferenceNumber;
short int m_Username;
short int m_Password;
short int m_Key;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		short int PK_PaidLicense_get();
short int FK_Device_get();
short int UseWithoutLicense_get();
string ValidUntil_get();
short int FK_Package_get();
short int FK_License_get();
short int ReferenceNumber_get();
short int Username_get();
short int Password_get();
short int Key_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_PaidLicense_set(short int val);
void FK_Device_set(short int val);
void UseWithoutLicense_set(short int val);
void ValidUntil_set(string val);
void FK_Package_set(short int val);
void FK_License_set(short int val);
void ReferenceNumber_set(short int val);
void Username_set(short int val);
void Password_set(short int val);
void Key_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_PaidLicense_isNull();
bool FK_Device_isNull();
bool UseWithoutLicense_isNull();
bool ValidUntil_isNull();
bool FK_Package_isNull();
bool FK_License_isNull();
bool ReferenceNumber_isNull();
bool Username_isNull();
bool Password_isNull();
bool Key_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_PaidLicense_setNull(bool val);
void FK_Device_setNull(bool val);
void UseWithoutLicense_setNull(bool val);
void ValidUntil_setNull(bool val);
void FK_Package_setNull(bool val);
void FK_License_setNull(bool val);
void ReferenceNumber_setNull(bool val);
void Username_setNull(bool val);
void Password_setNull(bool val);
void Key_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PaidLicense_pschmask(Table_PaidLicense_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PaidLicense_pschmask *Table_PaidLicense_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Device* FK_Device_getrow();
class Row_Package* FK_Package_getrow();
class Row_License* FK_License_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PaidLicense+ m_FK_Device+ m_UseWithoutLicense+ m_ValidUntil+ m_FK_Package+ m_FK_License+ m_ReferenceNumber+ m_Username+ m_Password+ m_Key+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_PaidLicense_asSQL();
string FK_Device_asSQL();
string UseWithoutLicense_asSQL();
string ValidUntil_asSQL();
string FK_Package_asSQL();
string FK_License_asSQL();
string ReferenceNumber_asSQL();
string Username_asSQL();
string Password_asSQL();
string Key_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

