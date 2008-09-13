/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CriteriaParm_pschmask_H__
#define __Table_CriteriaParm_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaParm_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CriteriaParm_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CriteriaParm_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_CriteriaParm_pschmask();

private:		
	friend class Row_CriteriaParm_pschmask;
	struct Key
	{
		friend class Row_CriteriaParm_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_CriteriaParm_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaParm_pschmask::Key &key1, const Table_CriteriaParm_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CriteriaParm_pschmask*> *rows);
	class Row_CriteriaParm_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaParm_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_CriteriaParm_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CriteriaParm_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaParm_pschmask::Key;
		friend class Table_CriteriaParm_pschmask;
	private:
		Table_CriteriaParm_pschmask *table;
		
		short int m_PK_CriteriaParm;
short int m_FK_CriteriaParmNesting;
short int m_FK_CriteriaParmList;
short int m_Parm;
short int m_Operator;
short int m_Value;
short int m_FK_CannedEvents_CriteriaParmList;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[13];
	
	public:
		short int PK_CriteriaParm_get();
short int FK_CriteriaParmNesting_get();
short int FK_CriteriaParmList_get();
short int Parm_get();
short int Operator_get();
short int Value_get();
short int FK_CannedEvents_CriteriaParmList_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_CriteriaParm_set(short int val);
void FK_CriteriaParmNesting_set(short int val);
void FK_CriteriaParmList_set(short int val);
void Parm_set(short int val);
void Operator_set(short int val);
void Value_set(short int val);
void FK_CannedEvents_CriteriaParmList_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_CriteriaParm_isNull();
bool FK_CriteriaParmNesting_isNull();
bool FK_CriteriaParmList_isNull();
bool Parm_isNull();
bool Operator_isNull();
bool Value_isNull();
bool FK_CannedEvents_CriteriaParmList_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_CriteriaParm_setNull(bool val);
void FK_CriteriaParmNesting_setNull(bool val);
void FK_CriteriaParmList_setNull(bool val);
void Parm_setNull(bool val);
void Operator_setNull(bool val);
void Value_setNull(bool val);
void FK_CannedEvents_CriteriaParmList_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CriteriaParm_pschmask(Table_CriteriaParm_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaParm_pschmask *Table_CriteriaParm_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaParmNesting* FK_CriteriaParmNesting_getrow();
class Row_CriteriaParmList* FK_CriteriaParmList_getrow();
class Row_CannedEvents_CriteriaParmList* FK_CannedEvents_CriteriaParmList_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CriteriaParm+ m_FK_CriteriaParmNesting+ m_FK_CriteriaParmList+ m_Parm+ m_Operator+ m_Value+ m_FK_CannedEvents_CriteriaParmList+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_CriteriaParm_asSQL();
string FK_CriteriaParmNesting_asSQL();
string FK_CriteriaParmList_asSQL();
string Parm_asSQL();
string Operator_asSQL();
string Value_asSQL();
string FK_CannedEvents_CriteriaParmList_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

