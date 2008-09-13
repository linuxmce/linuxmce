/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Installation_pschmask_H__
#define __Table_Installation_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Installation_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Installation_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Installation_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Installation_pschmask();

private:		
	friend class Row_Installation_pschmask;
	struct Key
	{
		friend class Row_Installation_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Installation_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Installation_pschmask::Key &key1, const Table_Installation_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Installation_pschmask*> *rows);
	class Row_Installation_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Installation_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Installation_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Installation_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Installation_pschmask::Key;
		friend class Table_Installation_pschmask;
	private:
		Table_Installation_pschmask *table;
		
		short int m_PK_Installation;
short int m_Description;
short int m_Name;
short int m_Address;
short int m_City;
short int m_State;
short int m_Zip;
short int m_FK_Country;
short int m_FK_City;
short int m_FK_PostalCode;
short int m_ActivationCode;
short int m_LastStatus;
short int m_LastAlive;
short int m_isActive;
short int m_FK_Version;
short int m_isMonitored;
short int m_FK_RepositoryType_Source;
short int m_FK_RepositoryType_Binaries;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[24];
	
	public:
		short int PK_Installation_get();
short int Description_get();
short int Name_get();
short int Address_get();
short int City_get();
short int State_get();
short int Zip_get();
short int FK_Country_get();
short int FK_City_get();
short int FK_PostalCode_get();
short int ActivationCode_get();
short int LastStatus_get();
short int LastAlive_get();
short int isActive_get();
short int FK_Version_get();
short int isMonitored_get();
short int FK_RepositoryType_Source_get();
short int FK_RepositoryType_Binaries_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Installation_set(short int val);
void Description_set(short int val);
void Name_set(short int val);
void Address_set(short int val);
void City_set(short int val);
void State_set(short int val);
void Zip_set(short int val);
void FK_Country_set(short int val);
void FK_City_set(short int val);
void FK_PostalCode_set(short int val);
void ActivationCode_set(short int val);
void LastStatus_set(short int val);
void LastAlive_set(short int val);
void isActive_set(short int val);
void FK_Version_set(short int val);
void isMonitored_set(short int val);
void FK_RepositoryType_Source_set(short int val);
void FK_RepositoryType_Binaries_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_Installation_isNull();
bool Description_isNull();
bool Name_isNull();
bool Address_isNull();
bool City_isNull();
bool State_isNull();
bool Zip_isNull();
bool FK_Country_isNull();
bool FK_City_isNull();
bool FK_PostalCode_isNull();
bool ActivationCode_isNull();
bool LastStatus_isNull();
bool LastAlive_isNull();
bool isActive_isNull();
bool FK_Version_isNull();
bool isMonitored_isNull();
bool FK_RepositoryType_Source_isNull();
bool FK_RepositoryType_Binaries_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_Installation_setNull(bool val);
void Description_setNull(bool val);
void Name_setNull(bool val);
void Address_setNull(bool val);
void City_setNull(bool val);
void State_setNull(bool val);
void Zip_setNull(bool val);
void FK_Country_setNull(bool val);
void FK_City_setNull(bool val);
void FK_PostalCode_setNull(bool val);
void ActivationCode_setNull(bool val);
void LastStatus_setNull(bool val);
void LastAlive_setNull(bool val);
void isActive_setNull(bool val);
void FK_Version_setNull(bool val);
void isMonitored_setNull(bool val);
void FK_RepositoryType_Source_setNull(bool val);
void FK_RepositoryType_Binaries_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Installation_pschmask(Table_Installation_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Installation_pschmask *Table_Installation_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Country* FK_Country_getrow();
class Row_City* FK_City_getrow();
class Row_PostalCode* FK_PostalCode_getrow();
class Row_Version* FK_Version_getrow();
class Row_RepositoryType* FK_RepositoryType_Source_getrow();
class Row_RepositoryType* FK_RepositoryType_Binaries_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Installation+ m_Description+ m_Name+ m_Address+ m_City+ m_State+ m_Zip+ m_FK_Country+ m_FK_City+ m_FK_PostalCode+ m_ActivationCode+ m_LastStatus+ m_LastAlive+ m_isActive+ m_FK_Version+ m_isMonitored+ m_FK_RepositoryType_Source+ m_FK_RepositoryType_Binaries+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Installation_asSQL();
string Description_asSQL();
string Name_asSQL();
string Address_asSQL();
string City_asSQL();
string State_asSQL();
string Zip_asSQL();
string FK_Country_asSQL();
string FK_City_asSQL();
string FK_PostalCode_asSQL();
string ActivationCode_asSQL();
string LastStatus_asSQL();
string LastAlive_asSQL();
string isActive_asSQL();
string FK_Version_asSQL();
string isMonitored_asSQL();
string FK_RepositoryType_Source_asSQL();
string FK_RepositoryType_Binaries_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

