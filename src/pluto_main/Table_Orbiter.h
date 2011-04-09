/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Orbiter_H__
#define __Table_Orbiter_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Orbiter.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Orbiter : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Orbiter(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Orbiter();

private:		
	friend class Row_Orbiter;
	struct Key
	{
		friend class Row_Orbiter;
		long int pk_PK_Orbiter;

		
		Key(long int in_PK_Orbiter);
	
		Key(class Row_Orbiter *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Orbiter::Key &key1, const Table_Orbiter::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Orbiter*> *rows);
	class Row_Orbiter* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Orbiter* GetRow(long int in_PK_Orbiter);
	

private:	
	
		
	class Row_Orbiter* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Orbiter : public TableRow, public SerializeClass
	{
		friend struct Table_Orbiter::Key;
		friend class Table_Orbiter;
	private:
		Table_Orbiter *table;
		
		long int m_PK_Orbiter;
string m_FloorplanInfo;
string m_Modification_LastGen;
short int m_Regen;
string m_Size;
short int m_RegenInProgress;
string m_RegenStatus;
long int m_RegenPercent;
string m_ScenariosFloorplans;
short int m_FirstRegen;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		long int PK_Orbiter_get();
string FloorplanInfo_get();
string Modification_LastGen_get();
short int Regen_get();
string Size_get();
short int RegenInProgress_get();
string RegenStatus_get();
long int RegenPercent_get();
string ScenariosFloorplans_get();
short int FirstRegen_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Orbiter_set(long int val);
void FloorplanInfo_set(string val);
void Modification_LastGen_set(string val);
void Regen_set(short int val);
void Size_set(string val);
void RegenInProgress_set(short int val);
void RegenStatus_set(string val);
void RegenPercent_set(long int val);
void ScenariosFloorplans_set(string val);
void FirstRegen_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FloorplanInfo_isNull();
bool Modification_LastGen_isNull();
bool Regen_isNull();
bool Size_isNull();
bool RegenInProgress_isNull();
bool RegenStatus_isNull();
bool RegenPercent_isNull();
bool ScenariosFloorplans_isNull();
bool FirstRegen_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FloorplanInfo_setNull(bool val);
void Modification_LastGen_setNull(bool val);
void Regen_setNull(bool val);
void Size_setNull(bool val);
void RegenInProgress_setNull(bool val);
void RegenStatus_setNull(bool val);
void RegenPercent_setNull(bool val);
void ScenariosFloorplans_setNull(bool val);
void FirstRegen_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Orbiter(Table_Orbiter *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Orbiter *Table_Orbiter_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CachedScreens_FK_Orbiter_getrows(vector <class Row_CachedScreens*> *rows);
void Device_Orbiter_FK_Orbiter_getrows(vector <class Row_Device_Orbiter*> *rows);
void Orbiter_Users_PasswordReq_FK_Orbiter_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows);
void Orbiter_Variable_FK_Orbiter_getrows(vector <class Row_Orbiter_Variable*> *rows);
void RemoteControl_FK_Orbiter_getrows(vector <class Row_RemoteControl*> *rows);
void Room_Users_FK_Orbiter_getrows(vector <class Row_Room_Users*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Orbiter+ m_FloorplanInfo+ m_Modification_LastGen+ m_Regen+ m_Size+ m_RegenInProgress+ m_RegenStatus+ m_RegenPercent+ m_ScenariosFloorplans+ m_FirstRegen+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
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
string FirstRegen_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

