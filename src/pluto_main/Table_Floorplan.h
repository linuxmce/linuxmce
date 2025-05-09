/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Floorplan_H__
#define __Table_Floorplan_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Floorplan.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Floorplan : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Floorplan(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Floorplan();

private:		
	friend class Row_Floorplan;
	struct Key
	{
		friend class Row_Floorplan;
		long int pk_FK_Installation;
long int pk_Page;

		
		Key(long int in_FK_Installation, long int in_Page);
	
		Key(class Row_Floorplan *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Floorplan::Key &key1, const Table_Floorplan::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Floorplan*> *rows);
	class Row_Floorplan* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Floorplan* GetRow(long int in_FK_Installation, long int in_Page);
	

private:	
	
		
	class Row_Floorplan* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Floorplan : public TableRow, public SerializeClass
	{
		friend struct Table_Floorplan::Key;
		friend class Table_Floorplan;
	private:
		Table_Floorplan *table;
		
		long int m_FK_Installation;
long int m_Page;
string m_Description;
long int m_FK_Icon;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int FK_Installation_get();
long int Page_get();
string Description_get();
long int FK_Icon_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Installation_set(long int val);
void Page_set(long int val);
void Description_set(string val);
void FK_Icon_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Icon_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Icon_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Floorplan(Table_Floorplan *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Floorplan *Table_Floorplan_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Installation* FK_Installation_getrow();
class Row_Icon* FK_Icon_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Installation+ m_Page+ m_Description+ m_FK_Icon+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Installation_asSQL();
string Page_asSQL();
string Description_asSQL();
string FK_Icon_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

