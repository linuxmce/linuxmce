/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_ParameterType_H__
#define __Table_ParameterType_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ParameterType.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_ParameterType : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_ParameterType(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_ParameterType();

private:		
	friend class Row_ParameterType;
	struct Key
	{
		friend class Row_ParameterType;
		long int pk_PK_ParameterType;

		
		Key(long int in_PK_ParameterType);
	
		Key(class Row_ParameterType *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ParameterType::Key &key1, const Table_ParameterType::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_ParameterType*> *rows);
	class Row_ParameterType* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_ParameterType* GetRow(long int in_PK_ParameterType);
	

private:	
	
		
	class Row_ParameterType* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_ParameterType : public TableRow, public SerializeClass
	{
		friend struct Table_ParameterType::Key;
		friend class Table_ParameterType;
	private:
		Table_ParameterType *table;
		
		long int m_PK_ParameterType;
string m_Description;
string m_Define;
string m_CPPType;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int PK_ParameterType_get();
string Description_get();
string Define_get();
string CPPType_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_ParameterType_set(long int val);
void Description_set(string val);
void Define_set(string val);
void CPPType_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Define_isNull();
bool CPPType_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Define_setNull(bool val);
void CPPType_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_ParameterType(Table_ParameterType *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ParameterType *Table_ParameterType_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CommandParameter_FK_ParameterType_getrows(vector <class Row_CommandParameter*> *rows);
void CriteriaParmList_FK_ParameterType_getrows(vector <class Row_CriteriaParmList*> *rows);
void DesignObjParameter_FK_ParameterType_getrows(vector <class Row_DesignObjParameter*> *rows);
void DeviceData_FK_ParameterType_getrows(vector <class Row_DeviceData*> *rows);
void EventParameter_FK_ParameterType_getrows(vector <class Row_EventParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_ParameterType+ m_Description+ m_Define+ m_CPPType+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_ParameterType_asSQL();
string Description_asSQL();
string Define_asSQL();
string CPPType_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

