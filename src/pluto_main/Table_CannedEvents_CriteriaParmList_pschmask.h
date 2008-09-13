/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CannedEvents_CriteriaParmList_pschmask_H__
#define __Table_CannedEvents_CriteriaParmList_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CannedEvents_CriteriaParmList_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CannedEvents_CriteriaParmList_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CannedEvents_CriteriaParmList_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_CannedEvents_CriteriaParmList_pschmask();

private:		
	friend class Row_CannedEvents_CriteriaParmList_pschmask;
	struct Key
	{
		friend class Row_CannedEvents_CriteriaParmList_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_CannedEvents_CriteriaParmList_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CannedEvents_CriteriaParmList_pschmask::Key &key1, const Table_CannedEvents_CriteriaParmList_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CannedEvents_CriteriaParmList_pschmask*> *rows);
	class Row_CannedEvents_CriteriaParmList_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CannedEvents_CriteriaParmList_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_CannedEvents_CriteriaParmList_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CannedEvents_CriteriaParmList_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_CannedEvents_CriteriaParmList_pschmask::Key;
		friend class Table_CannedEvents_CriteriaParmList_pschmask;
	private:
		Table_CannedEvents_CriteriaParmList_pschmask *table;
		
		short int m_PK_CannedEvents_CriteriaParmList;
short int m_FK_CannedEvents;
short int m_FK_CriteriaParmList;
short int m_Description;
short int m_Comments;
short int m_Operator;
short int m_Parm;
short int m_DefaultValue;
short int m_ExtraInfo;
short int m_Required;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		short int PK_CannedEvents_CriteriaParmList_get();
short int FK_CannedEvents_get();
short int FK_CriteriaParmList_get();
short int Description_get();
short int Comments_get();
short int Operator_get();
short int Parm_get();
short int DefaultValue_get();
short int ExtraInfo_get();
short int Required_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_CannedEvents_CriteriaParmList_set(short int val);
void FK_CannedEvents_set(short int val);
void FK_CriteriaParmList_set(short int val);
void Description_set(short int val);
void Comments_set(short int val);
void Operator_set(short int val);
void Parm_set(short int val);
void DefaultValue_set(short int val);
void ExtraInfo_set(short int val);
void Required_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_CannedEvents_CriteriaParmList_isNull();
bool FK_CannedEvents_isNull();
bool FK_CriteriaParmList_isNull();
bool Description_isNull();
bool Comments_isNull();
bool Operator_isNull();
bool Parm_isNull();
bool DefaultValue_isNull();
bool ExtraInfo_isNull();
bool Required_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_CannedEvents_CriteriaParmList_setNull(bool val);
void FK_CannedEvents_setNull(bool val);
void FK_CriteriaParmList_setNull(bool val);
void Description_setNull(bool val);
void Comments_setNull(bool val);
void Operator_setNull(bool val);
void Parm_setNull(bool val);
void DefaultValue_setNull(bool val);
void ExtraInfo_setNull(bool val);
void Required_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CannedEvents_CriteriaParmList_pschmask(Table_CannedEvents_CriteriaParmList_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CannedEvents_CriteriaParmList_pschmask *Table_CannedEvents_CriteriaParmList_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CannedEvents* FK_CannedEvents_getrow();
class Row_CriteriaParmList* FK_CriteriaParmList_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CannedEvents_CriteriaParmList+ m_FK_CannedEvents+ m_FK_CriteriaParmList+ m_Description+ m_Comments+ m_Operator+ m_Parm+ m_DefaultValue+ m_ExtraInfo+ m_Required+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_CannedEvents_CriteriaParmList_asSQL();
string FK_CannedEvents_asSQL();
string FK_CriteriaParmList_asSQL();
string Description_asSQL();
string Comments_asSQL();
string Operator_asSQL();
string Parm_asSQL();
string DefaultValue_asSQL();
string ExtraInfo_asSQL();
string Required_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

