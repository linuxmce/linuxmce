/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_UserRouting_H__
#define __Table_UserRouting_H__

#include "TableRow.h"
#include "Database_pluto_telecom.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_UserRouting.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_UserRouting : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_telecom *database;
	struct Key;	//forward declaration
	
public:
	Table_UserRouting(Database_pluto_telecom *pDatabase):database(pDatabase)
	{
	};
	~Table_UserRouting();

private:		
	friend class Row_UserRouting;
	struct Key
	{
		friend class Row_UserRouting;
		long int pk_PK_UserRouting;

		
		Key(long int in_PK_UserRouting);
	
		Key(class Row_UserRouting *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_UserRouting::Key &key1, const Table_UserRouting::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_UserRouting*> *rows);
	class Row_UserRouting* AddRow();
	Database_pluto_telecom *Database_pluto_telecom_get() { return database; }
	
		
	class Row_UserRouting* GetRow(long int in_PK_UserRouting);
	

private:	
	
		
	class Row_UserRouting* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_UserRouting : public TableRow, public SerializeClass
	{
		friend struct Table_UserRouting::Key;
		friend class Table_UserRouting;
	private:
		Table_UserRouting *table;
		
		long int m_PK_UserRouting;
long int m_EK_Users;
long int m_EK_UserMode;
short int m_SendStatusFirst;
short int m_IsPriorityCaller;
long int m_StepOrder;
string m_Routing;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[13];
	
	public:
		long int PK_UserRouting_get();
long int EK_Users_get();
long int EK_UserMode_get();
short int SendStatusFirst_get();
short int IsPriorityCaller_get();
long int StepOrder_get();
string Routing_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_UserRouting_set(long int val);
void EK_Users_set(long int val);
void EK_UserMode_set(long int val);
void SendStatusFirst_set(short int val);
void IsPriorityCaller_set(short int val);
void StepOrder_set(long int val);
void Routing_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool SendStatusFirst_isNull();
bool IsPriorityCaller_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void SendStatusFirst_setNull(bool val);
void IsPriorityCaller_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_UserRouting(Table_UserRouting *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_UserRouting *Table_UserRouting_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_UserRouting+ m_EK_Users+ m_EK_UserMode+ m_SendStatusFirst+ m_IsPriorityCaller+ m_StepOrder+ m_Routing+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_UserRouting_asSQL();
string EK_Users_asSQL();
string EK_UserMode_asSQL();
string SendStatusFirst_asSQL();
string IsPriorityCaller_asSQL();
string StepOrder_asSQL();
string Routing_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

