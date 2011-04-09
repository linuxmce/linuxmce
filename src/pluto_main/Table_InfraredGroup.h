/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_InfraredGroup_H__
#define __Table_InfraredGroup_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_InfraredGroup.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_InfraredGroup : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_InfraredGroup(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_InfraredGroup();

private:		
	friend class Row_InfraredGroup;
	struct Key
	{
		friend class Row_InfraredGroup;
		long int pk_PK_InfraredGroup;

		
		Key(long int in_PK_InfraredGroup);
	
		Key(class Row_InfraredGroup *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_InfraredGroup::Key &key1, const Table_InfraredGroup::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_InfraredGroup*> *rows);
	class Row_InfraredGroup* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_InfraredGroup* GetRow(long int in_PK_InfraredGroup);
	

private:	
	
		
	class Row_InfraredGroup* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_InfraredGroup : public TableRow, public SerializeClass
	{
		friend struct Table_InfraredGroup::Key;
		friend class Table_InfraredGroup;
	private:
		Table_InfraredGroup *table;
		
		long int m_PK_InfraredGroup;
long int m_FK_DeviceCategory;
long int m_FK_Manufacturer;
string m_Description;
long int m_FK_CommMethod;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_InfraredGroup_get();
long int FK_DeviceCategory_get();
long int FK_Manufacturer_get();
string Description_get();
long int FK_CommMethod_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_InfraredGroup_set(long int val);
void FK_DeviceCategory_set(long int val);
void FK_Manufacturer_set(long int val);
void Description_set(string val);
void FK_CommMethod_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_InfraredGroup(Table_InfraredGroup *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_InfraredGroup *Table_InfraredGroup_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();
class Row_CommMethod* FK_CommMethod_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_InfraredGroup_getrows(vector <class Row_DeviceTemplate*> *rows);
void DeviceTemplate_InfraredGroup_FK_InfraredGroup_getrows(vector <class Row_DeviceTemplate_InfraredGroup*> *rows);
void InfraredGroup_Command_FK_InfraredGroup_getrows(vector <class Row_InfraredGroup_Command*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_InfraredGroup+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Description+ m_FK_CommMethod+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_InfraredGroup_asSQL();
string FK_DeviceCategory_asSQL();
string FK_Manufacturer_asSQL();
string Description_asSQL();
string FK_CommMethod_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

