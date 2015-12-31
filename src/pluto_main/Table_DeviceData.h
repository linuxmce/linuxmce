/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceData_H__
#define __Table_DeviceData_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceData.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceData : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceData(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceData();

private:		
	friend class Row_DeviceData;
	struct Key
	{
		friend class Row_DeviceData;
		long int pk_PK_DeviceData;

		
		Key(long int in_PK_DeviceData);
	
		Key(class Row_DeviceData *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceData::Key &key1, const Table_DeviceData::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceData*> *rows);
	class Row_DeviceData* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceData* GetRow(long int in_PK_DeviceData);
	

private:	
	
		
	class Row_DeviceData* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceData : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceData::Key;
		friend class Table_DeviceData;
	private:
		Table_DeviceData *table;
		
		long int m_PK_DeviceData;
string m_Description;
string m_Define;
long int m_FK_ParameterType;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int PK_DeviceData_get();
string Description_get();
string Define_get();
long int FK_ParameterType_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DeviceData_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_ParameterType_set(long int val);
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
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceData(Table_DeviceData *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceData *Table_DeviceData_get() { return table; };

		// Return the rows for foreign keys 
		class Row_ParameterType* FK_ParameterType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DHCPDevice_DeviceData_FK_DeviceData_getrows(vector <class Row_DHCPDevice_DeviceData*> *rows);
void DeviceCategory_DeviceData_FK_DeviceData_getrows(vector <class Row_DeviceCategory_DeviceData*> *rows);
void DeviceTemplate_DeviceData_FK_DeviceData_getrows(vector <class Row_DeviceTemplate_DeviceData*> *rows);
void Device_DeviceData_FK_DeviceData_getrows(vector <class Row_Device_DeviceData*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceData+ m_Description+ m_Define+ m_FK_ParameterType+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceData_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_ParameterType_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

