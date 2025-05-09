/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DHCPDevice_DeviceData_H__
#define __Table_DHCPDevice_DeviceData_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DHCPDevice_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DHCPDevice_DeviceData : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DHCPDevice_DeviceData(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DHCPDevice_DeviceData();

private:		
	friend class Row_DHCPDevice_DeviceData;
	struct Key
	{
		friend class Row_DHCPDevice_DeviceData;
		long int pk_FK_DHCPDevice;
long int pk_FK_DeviceData;

		
		Key(long int in_FK_DHCPDevice, long int in_FK_DeviceData);
	
		Key(class Row_DHCPDevice_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DHCPDevice_DeviceData::Key &key1, const Table_DHCPDevice_DeviceData::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DHCPDevice_DeviceData*> *rows);
	class Row_DHCPDevice_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DHCPDevice_DeviceData* GetRow(long int in_FK_DHCPDevice, long int in_FK_DeviceData);
	

private:	
	
		
	class Row_DHCPDevice_DeviceData* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DHCPDevice_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_DHCPDevice_DeviceData::Key;
		friend class Table_DHCPDevice_DeviceData;
	private:
		Table_DHCPDevice_DeviceData *table;
		
		long int m_FK_DHCPDevice;
long int m_FK_DeviceData;
string m_IK_DeviceData;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int FK_DHCPDevice_get();
long int FK_DeviceData_get();
string IK_DeviceData_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_DHCPDevice_set(long int val);
void FK_DeviceData_set(long int val);
void IK_DeviceData_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool IK_DeviceData_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void IK_DeviceData_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DHCPDevice_DeviceData(Table_DHCPDevice_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DHCPDevice_DeviceData *Table_DHCPDevice_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DHCPDevice* FK_DHCPDevice_getrow();
class Row_DeviceData* FK_DeviceData_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DHCPDevice+ m_FK_DeviceData+ m_IK_DeviceData+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_DHCPDevice_asSQL();
string FK_DeviceData_asSQL();
string IK_DeviceData_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

