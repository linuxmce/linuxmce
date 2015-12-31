/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Manufacturer_H__
#define __Table_Manufacturer_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Manufacturer.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Manufacturer : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Manufacturer(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Manufacturer();

private:		
	friend class Row_Manufacturer;
	struct Key
	{
		friend class Row_Manufacturer;
		long int pk_PK_Manufacturer;

		
		Key(long int in_PK_Manufacturer);
	
		Key(class Row_Manufacturer *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Manufacturer::Key &key1, const Table_Manufacturer::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Manufacturer*> *rows);
	class Row_Manufacturer* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Manufacturer* GetRow(long int in_PK_Manufacturer);
	

private:	
	
		
	class Row_Manufacturer* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Manufacturer : public TableRow, public SerializeClass
	{
		friend struct Table_Manufacturer::Key;
		friend class Table_Manufacturer;
	private:
		Table_Manufacturer *table;
		
		long int m_PK_Manufacturer;
string m_Description;
long int m_IRFrequency;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int PK_Manufacturer_get();
string Description_get();
long int IRFrequency_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Manufacturer_set(long int val);
void Description_set(string val);
void IRFrequency_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool IRFrequency_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void IRFrequency_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Manufacturer(Table_Manufacturer *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Manufacturer *Table_Manufacturer_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DHCPDevice_FK_Manufacturer_getrows(vector <class Row_DHCPDevice*> *rows);
void DeviceTemplate_FK_Manufacturer_getrows(vector <class Row_DeviceTemplate*> *rows);
void InfraredGroup_FK_Manufacturer_getrows(vector <class Row_InfraredGroup*> *rows);
void Package_FK_Manufacturer_getrows(vector <class Row_Package*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Manufacturer+ m_Description+ m_IRFrequency+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Manufacturer_asSQL();
string Description_asSQL();
string IRFrequency_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

