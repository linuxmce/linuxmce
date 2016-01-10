/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_AlertType_H__
#define __Table_AlertType_H__

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_AlertType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_AlertType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
	Table_AlertType(Database_pluto_security *pDatabase):database(pDatabase)
	{
	};
	~Table_AlertType();

private:		
	friend class Row_AlertType;
	struct Key
	{
		friend class Row_AlertType;
		long int pk_PK_AlertType;

		
		Key(long int in_PK_AlertType);
	
		Key(class Row_AlertType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_AlertType::Key &key1, const Table_AlertType::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_AlertType*> *rows);
	class Row_AlertType* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_AlertType* GetRow(long int in_PK_AlertType);
	

private:	
	
		
	class Row_AlertType* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_AlertType : public TableRow, public SerializeClass
	{
		friend struct Table_AlertType::Key;
		friend class Table_AlertType;
	private:
		Table_AlertType *table;
		
		long int m_PK_AlertType;
short int m_DelayBeforeAlarm;
short int m_ExitDelay;
long int m_AlarmDuration;
long int m_PoolAlerts;
string m_Description;
string m_Define;
long int m_PhoneExtension;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[14];
	
	public:
		long int PK_AlertType_get();
short int DelayBeforeAlarm_get();
short int ExitDelay_get();
long int AlarmDuration_get();
long int PoolAlerts_get();
string Description_get();
string Define_get();
long int PhoneExtension_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_AlertType_set(long int val);
void DelayBeforeAlarm_set(short int val);
void ExitDelay_set(short int val);
void AlarmDuration_set(long int val);
void PoolAlerts_set(long int val);
void Description_set(string val);
void Define_set(string val);
void PhoneExtension_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool DelayBeforeAlarm_isNull();
bool ExitDelay_isNull();
bool AlarmDuration_isNull();
bool PoolAlerts_isNull();
bool Define_isNull();
bool PhoneExtension_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void DelayBeforeAlarm_setNull(bool val);
void ExitDelay_setNull(bool val);
void AlarmDuration_setNull(bool val);
void PoolAlerts_setNull(bool val);
void Define_setNull(bool val);
void PhoneExtension_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_AlertType(Table_AlertType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_AlertType *Table_AlertType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Alert_FK_AlertType_getrows(vector <class Row_Alert*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_AlertType+ m_DelayBeforeAlarm+ m_ExitDelay+ m_AlarmDuration+ m_PoolAlerts+ m_Description+ m_Define+ m_PhoneExtension+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_AlertType_asSQL();
string DelayBeforeAlarm_asSQL();
string ExitDelay_asSQL();
string AlarmDuration_asSQL();
string PoolAlerts_asSQL();
string Description_asSQL();
string Define_asSQL();
string PhoneExtension_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

