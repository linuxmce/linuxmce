/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CriteriaParmNesting_H__
#define __Table_CriteriaParmNesting_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CriteriaParmNesting.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CriteriaParmNesting : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CriteriaParmNesting(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_CriteriaParmNesting();

private:		
	friend class Row_CriteriaParmNesting;
	struct Key
	{
		friend class Row_CriteriaParmNesting;
		long int pk_PK_CriteriaParmNesting;

		
		Key(long int in_PK_CriteriaParmNesting);
	
		Key(class Row_CriteriaParmNesting *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CriteriaParmNesting::Key &key1, const Table_CriteriaParmNesting::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CriteriaParmNesting*> *rows);
	class Row_CriteriaParmNesting* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CriteriaParmNesting* GetRow(long int in_PK_CriteriaParmNesting);
	

private:	
	
		
	class Row_CriteriaParmNesting* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CriteriaParmNesting : public TableRow, public SerializeClass
	{
		friend struct Table_CriteriaParmNesting::Key;
		friend class Table_CriteriaParmNesting;
	private:
		Table_CriteriaParmNesting *table;
		
		long int m_PK_CriteriaParmNesting;
long int m_FK_CriteriaParmNesting_Parent;
short int m_IsAnd;
short int m_IsNot;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int PK_CriteriaParmNesting_get();
long int FK_CriteriaParmNesting_Parent_get();
short int IsAnd_get();
short int IsNot_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_CriteriaParmNesting_set(long int val);
void FK_CriteriaParmNesting_Parent_set(long int val);
void IsAnd_set(short int val);
void IsNot_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_CriteriaParmNesting_Parent_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_CriteriaParmNesting_Parent_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CriteriaParmNesting(Table_CriteriaParmNesting *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CriteriaParmNesting *Table_CriteriaParmNesting_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CriteriaParmNesting* FK_CriteriaParmNesting_Parent_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Criteria_FK_CriteriaParmNesting_getrows(vector <class Row_Criteria*> *rows);
void CriteriaParm_FK_CriteriaParmNesting_getrows(vector <class Row_CriteriaParm*> *rows);
void CriteriaParmNesting_FK_CriteriaParmNesting_Parent_getrows(vector <class Row_CriteriaParmNesting*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CriteriaParmNesting+ m_FK_CriteriaParmNesting_Parent+ m_IsAnd+ m_IsNot+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_CriteriaParmNesting_asSQL();
string FK_CriteriaParmNesting_Parent_asSQL();
string IsAnd_asSQL();
string IsNot_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

