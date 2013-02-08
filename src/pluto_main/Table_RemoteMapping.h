/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_RemoteMapping_H__
#define __Table_RemoteMapping_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_RemoteMapping.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_RemoteMapping : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_RemoteMapping(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_RemoteMapping();

private:		
	friend class Row_RemoteMapping;
	struct Key
	{
		friend class Row_RemoteMapping;
		long int pk_PK_RemoteMapping;

		
		Key(long int in_PK_RemoteMapping);
	
		Key(class Row_RemoteMapping *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_RemoteMapping::Key &key1, const Table_RemoteMapping::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_RemoteMapping*> *rows);
	class Row_RemoteMapping* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_RemoteMapping* GetRow(long int in_PK_RemoteMapping);
	

private:	
	
		
	class Row_RemoteMapping* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_RemoteMapping : public TableRow, public SerializeClass
	{
		friend struct Table_RemoteMapping::Key;
		friend class Table_RemoteMapping;
	private:
		Table_RemoteMapping *table;
		
		long int m_PK_RemoteMapping;
string m_Description;
string m_ScreenType;
string m_RemoteLayout;
string m_Mapping;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_RemoteMapping_get();
string Description_get();
string ScreenType_get();
string RemoteLayout_get();
string Mapping_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_RemoteMapping_set(long int val);
void Description_set(string val);
void ScreenType_set(string val);
void RemoteLayout_set(string val);
void Mapping_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool ScreenType_isNull();
bool RemoteLayout_isNull();
bool Mapping_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void ScreenType_setNull(bool val);
void RemoteLayout_setNull(bool val);
void Mapping_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_RemoteMapping(Table_RemoteMapping *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_RemoteMapping *Table_RemoteMapping_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_RemoteMapping+ m_Description+ m_ScreenType+ m_RemoteLayout+ m_Mapping+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_RemoteMapping_asSQL();
string Description_asSQL();
string ScreenType_asSQL();
string RemoteLayout_asSQL();
string Mapping_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

