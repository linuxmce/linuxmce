/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Installation_pschist_H__
#define __Table_Installation_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Installation_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Installation_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Installation_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Installation_pschist();

private:		
	friend class Row_Installation_pschist;
	struct Key
	{
		friend class Row_Installation_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Installation_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Installation_pschist::Key &key1, const Table_Installation_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Installation_pschist*> *rows);
	class Row_Installation_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Installation_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Installation_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Installation_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_Installation_pschist::Key;
		friend class Table_Installation_pschist;
	private:
		Table_Installation_pschist *table;
		
		long int m_PK_Installation;
string m_Description;
string m_Name;
string m_Address;
string m_City;
string m_State;
string m_Zip;
long int m_FK_Country;
long int m_FK_City;
long int m_FK_PostalCode;
string m_ActivationCode;
string m_LastStatus;
string m_LastAlive;
short int m_isActive;
long int m_FK_Version;
short int m_isMonitored;
long int m_FK_RepositoryType_Source;
long int m_FK_RepositoryType_Binaries;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[25];
	
	public:
		long int PK_Installation_get();
string Description_get();
string Name_get();
string Address_get();
string City_get();
string State_get();
string Zip_get();
long int FK_Country_get();
long int FK_City_get();
long int FK_PostalCode_get();
string ActivationCode_get();
string LastStatus_get();
string LastAlive_get();
short int isActive_get();
long int FK_Version_get();
short int isMonitored_get();
long int FK_RepositoryType_Source_get();
long int FK_RepositoryType_Binaries_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void PK_Installation_set(long int val);
void Description_set(string val);
void Name_set(string val);
void Address_set(string val);
void City_set(string val);
void State_set(string val);
void Zip_set(string val);
void FK_Country_set(long int val);
void FK_City_set(long int val);
void FK_PostalCode_set(long int val);
void ActivationCode_set(string val);
void LastStatus_set(string val);
void LastAlive_set(string val);
void isActive_set(short int val);
void FK_Version_set(long int val);
void isMonitored_set(short int val);
void FK_RepositoryType_Source_set(long int val);
void FK_RepositoryType_Binaries_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
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
bool psc_toc_isNull();
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
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Installation_pschist(Table_Installation_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Installation_pschist *Table_Installation_pschist_get() { return table; };

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
			StartSerializeList() + m_PK_Installation+ m_Description+ m_Name+ m_Address+ m_City+ m_State+ m_Zip+ m_FK_Country+ m_FK_City+ m_FK_PostalCode+ m_ActivationCode+ m_LastStatus+ m_LastAlive+ m_isActive+ m_FK_Version+ m_isMonitored+ m_FK_RepositoryType_Source+ m_FK_RepositoryType_Binaries+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
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
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

