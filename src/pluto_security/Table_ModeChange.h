/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_ModeChange_H__
#define __Table_ModeChange_H__

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ModeChange.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_ModeChange : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
	Table_ModeChange(Database_pluto_security *pDatabase):database(pDatabase)
	{
	};
	~Table_ModeChange();

private:		
	friend class Row_ModeChange;
	struct Key
	{
		friend class Row_ModeChange;
		long int pk_PK_ModeChange;

		
		Key(long int in_PK_ModeChange);
	
		Key(class Row_ModeChange *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ModeChange::Key &key1, const Table_ModeChange::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_ModeChange*> *rows);
	class Row_ModeChange* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_ModeChange* GetRow(long int in_PK_ModeChange);
	

private:	
	
		
	class Row_ModeChange* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_ModeChange : public TableRow, public SerializeClass
	{
		friend struct Table_ModeChange::Key;
		friend class Table_ModeChange;
	private:
		Table_ModeChange *table;
		
		long int m_PK_ModeChange;
long int m_EK_HouseMode;
long int m_EK_DeviceGroup;
string m_ChangeTime;
long int m_EK_Users;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_ModeChange_get();
long int EK_HouseMode_get();
long int EK_DeviceGroup_get();
string ChangeTime_get();
long int EK_Users_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_ModeChange_set(long int val);
void EK_HouseMode_set(long int val);
void EK_DeviceGroup_set(long int val);
void ChangeTime_set(string val);
void EK_Users_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool EK_DeviceGroup_isNull();
bool ChangeTime_isNull();
bool EK_Users_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void EK_DeviceGroup_setNull(bool val);
void ChangeTime_setNull(bool val);
void EK_Users_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_ModeChange(Table_ModeChange *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ModeChange *Table_ModeChange_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_ModeChange+ m_EK_HouseMode+ m_EK_DeviceGroup+ m_ChangeTime+ m_EK_Users+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_ModeChange_asSQL();
string EK_HouseMode_asSQL();
string EK_DeviceGroup_asSQL();
string ChangeTime_asSQL();
string EK_Users_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

