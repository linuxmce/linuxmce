/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Orbiter_pschmask_H__
#define __Table_Orbiter_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Orbiter_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Orbiter_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Orbiter_pschmask();

private:		
	friend class Row_Orbiter_pschmask;
	struct Key
	{
		friend class Row_Orbiter_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Orbiter_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter_pschmask::Key &key1, const Table_Orbiter_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Orbiter_pschmask*> *rows);
	class Row_Orbiter_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Orbiter_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Orbiter_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter_pschmask::Key;
		friend class Table_Orbiter_pschmask;
	private:
		Table_Orbiter_pschmask *table;
		
		short int m_PK_Orbiter;
short int m_FloorplanInfo;
short int m_Modification_LastGen;
short int m_Regen;
short int m_Size;
short int m_RegenInProgress;
string m_RegenStatus;
long int m_RegenPercent;
short int m_ScenariosFloorplans;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;
short int m_FirstRegen;

		bool is_null[16];
	
	public:
		short int PK_Orbiter_get();
short int FloorplanInfo_get();
short int Modification_LastGen_get();
short int Regen_get();
short int Size_get();
short int RegenInProgress_get();
string RegenStatus_get();
long int RegenPercent_get();
short int ScenariosFloorplans_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();
short int FirstRegen_get();

		
		void PK_Orbiter_set(short int val);
void FloorplanInfo_set(short int val);
void Modification_LastGen_set(short int val);
void Regen_set(short int val);
void Size_set(short int val);
void RegenInProgress_set(short int val);
void RegenStatus_set(string val);
void RegenPercent_set(long int val);
void ScenariosFloorplans_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);
void FirstRegen_set(short int val);

		
		bool PK_Orbiter_isNull();
bool FloorplanInfo_isNull();
bool Modification_LastGen_isNull();
bool Regen_isNull();
bool Size_isNull();
bool RegenInProgress_isNull();
bool RegenStatus_isNull();
bool RegenPercent_isNull();
bool ScenariosFloorplans_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();
bool FirstRegen_isNull();

			
		void PK_Orbiter_setNull(bool val);
void FloorplanInfo_setNull(bool val);
void Modification_LastGen_setNull(bool val);
void Regen_setNull(bool val);
void Size_setNull(bool val);
void RegenInProgress_setNull(bool val);
void RegenStatus_setNull(bool val);
void RegenPercent_setNull(bool val);
void ScenariosFloorplans_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
void FirstRegen_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Orbiter_pschmask(Table_Orbiter_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter_pschmask *Table_Orbiter_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Orbiter+ m_FloorplanInfo+ m_Modification_LastGen+ m_Regen+ m_Size+ m_RegenInProgress+ m_RegenStatus+ m_RegenPercent+ m_ScenariosFloorplans+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict+ m_FirstRegen;
		}
	private:
		void SetDefaultValues();
		
		string PK_Orbiter_asSQL();
string FloorplanInfo_asSQL();
string Modification_LastGen_asSQL();
string Regen_asSQL();
string Size_asSQL();
string RegenInProgress_asSQL();
string RegenStatus_asSQL();
string RegenPercent_asSQL();
string ScenariosFloorplans_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();
string FirstRegen_asSQL();

	};

#endif

