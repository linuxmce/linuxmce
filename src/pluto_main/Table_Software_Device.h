/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Software_Device_H__
#define __Table_Software_Device_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Software_Device.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Software_Device : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Software_Device(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Software_Device();

private:		
	friend class Row_Software_Device;
	struct Key
	{
		friend class Row_Software_Device;
		long int pk_FK_Software;
long int pk_FK_Device;

		
		Key(long int in_FK_Software, long int in_FK_Device);
	
		Key(class Row_Software_Device *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Software_Device::Key &key1, const Table_Software_Device::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Software_Device*> *rows);
	class Row_Software_Device* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Software_Device* GetRow(long int in_FK_Software, long int in_FK_Device);
	

private:	
	
		
	class Row_Software_Device* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Software_Device : public TableRow, public SerializeClass
	{
		friend struct Table_Software_Device::Key;
		friend class Table_Software_Device;
	private:
		Table_Software_Device *table;
		
		long int m_FK_Software;
long int m_FK_Device;
long int m_FK_Software_Source;
string m_Status;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int FK_Software_get();
long int FK_Device_get();
long int FK_Software_Source_get();
string Status_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Software_set(long int val);
void FK_Device_set(long int val);
void FK_Software_Source_set(long int val);
void Status_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Software_Source_isNull();
bool Status_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Software_Source_setNull(bool val);
void Status_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Software_Device(Table_Software_Device *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Software_Device *Table_Software_Device_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Software* FK_Software_getrow();
class Row_Device* FK_Device_getrow();
class Row_Software_Source* FK_Software_Source_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Software+ m_FK_Device+ m_FK_Software_Source+ m_Status+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Software_asSQL();
string FK_Device_asSQL();
string FK_Software_Source_asSQL();
string Status_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

