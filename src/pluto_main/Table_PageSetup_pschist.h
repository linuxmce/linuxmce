/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_PageSetup_pschist_H__
#define __Table_PageSetup_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PageSetup_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PageSetup_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_PageSetup_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_PageSetup_pschist();

private:		
	friend class Row_PageSetup_pschist;
	struct Key
	{
		friend class Row_PageSetup_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_PageSetup_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PageSetup_pschist::Key &key1, const Table_PageSetup_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_PageSetup_pschist*> *rows);
	class Row_PageSetup_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PageSetup_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_PageSetup_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PageSetup_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_PageSetup_pschist::Key;
		friend class Table_PageSetup_pschist;
	private:
		Table_PageSetup_pschist *table;
		
		long int m_PK_PageSetup;
long int m_FK_PageSetup_Parent;
short int m_Website;
long int m_OrderNum;
long int m_FK_Package;
string m_Description;
string m_pageURL;
short int m_showInTopMenu;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[15];
	
	public:
		long int PK_PageSetup_get();
long int FK_PageSetup_Parent_get();
short int Website_get();
long int OrderNum_get();
long int FK_Package_get();
string Description_get();
string pageURL_get();
short int showInTopMenu_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void PK_PageSetup_set(long int val);
void FK_PageSetup_Parent_set(long int val);
void Website_set(short int val);
void OrderNum_set(long int val);
void FK_Package_set(long int val);
void Description_set(string val);
void pageURL_set(string val);
void showInTopMenu_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);

		
		bool PK_PageSetup_isNull();
bool FK_PageSetup_Parent_isNull();
bool Website_isNull();
bool OrderNum_isNull();
bool FK_Package_isNull();
bool Description_isNull();
bool pageURL_isNull();
bool showInTopMenu_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();

			
		void PK_PageSetup_setNull(bool val);
void FK_PageSetup_Parent_setNull(bool val);
void Website_setNull(bool val);
void OrderNum_setNull(bool val);
void FK_Package_setNull(bool val);
void Description_setNull(bool val);
void pageURL_setNull(bool val);
void showInTopMenu_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PageSetup_pschist(Table_PageSetup_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PageSetup_pschist *Table_PageSetup_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_PageSetup* FK_PageSetup_Parent_getrow();
class Row_Package* FK_Package_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PageSetup+ m_FK_PageSetup_Parent+ m_Website+ m_OrderNum+ m_FK_Package+ m_Description+ m_pageURL+ m_showInTopMenu+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_PageSetup_asSQL();
string FK_PageSetup_Parent_asSQL();
string Website_asSQL();
string OrderNum_asSQL();
string FK_Package_asSQL();
string Description_asSQL();
string pageURL_asSQL();
string showInTopMenu_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

