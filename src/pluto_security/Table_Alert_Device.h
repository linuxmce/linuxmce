/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Alert_Device_H__
#define __Table_Alert_Device_H__

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Alert_Device.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Alert_Device : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
	Table_Alert_Device(Database_pluto_security *pDatabase):database(pDatabase)
	{
	};
	~Table_Alert_Device();

private:		
	friend class Row_Alert_Device;
	struct Key
	{
		friend class Row_Alert_Device;
		long int pk_PK_Alert_Device;

		
		Key(long int in_PK_Alert_Device);
	
		Key(class Row_Alert_Device *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Alert_Device::Key &key1, const Table_Alert_Device::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Alert_Device*> *rows);
	class Row_Alert_Device* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_Alert_Device* GetRow(long int in_PK_Alert_Device);
	

private:	
	
		
	class Row_Alert_Device* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Alert_Device : public TableRow, public SerializeClass
	{
		friend struct Table_Alert_Device::Key;
		friend class Table_Alert_Device;
	private:
		Table_Alert_Device *table;
		
		long int m_PK_Alert_Device;
long int m_FK_Alert;
long int m_EK_Device;
string m_DetectionTime;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int PK_Alert_Device_get();
long int FK_Alert_get();
long int EK_Device_get();
string DetectionTime_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Alert_Device_set(long int val);
void FK_Alert_set(long int val);
void EK_Device_set(long int val);
void DetectionTime_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Alert_Device(Table_Alert_Device *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Alert_Device *Table_Alert_Device_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Alert* FK_Alert_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Alert_Device+ m_FK_Alert+ m_EK_Device+ m_DetectionTime+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Alert_Device_asSQL();
string FK_Alert_asSQL();
string EK_Device_asSQL();
string DetectionTime_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

