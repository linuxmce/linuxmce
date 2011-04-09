/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_FAQ_H__
#define __Table_FAQ_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_FAQ.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_FAQ : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_FAQ(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_FAQ();

private:		
	friend class Row_FAQ;
	struct Key
	{
		friend class Row_FAQ;
		long int pk_PK_FAQ;

		
		Key(long int in_PK_FAQ);
	
		Key(class Row_FAQ *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_FAQ::Key &key1, const Table_FAQ::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_FAQ*> *rows);
	class Row_FAQ* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_FAQ* GetRow(long int in_PK_FAQ);
	

private:	
	
		
	class Row_FAQ* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_FAQ : public TableRow, public SerializeClass
	{
		friend struct Table_FAQ::Key;
		friend class Table_FAQ;
	private:
		Table_FAQ *table;
		
		long int m_PK_FAQ;
long int m_FK_Package;
string m_Summary;
string m_Content;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int PK_FAQ_get();
long int FK_Package_get();
string Summary_get();
string Content_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_FAQ_set(long int val);
void FK_Package_set(long int val);
void Summary_set(string val);
void Content_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Package_isNull();
bool Summary_isNull();
bool Content_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Package_setNull(bool val);
void Summary_setNull(bool val);
void Content_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_FAQ(Table_FAQ *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FAQ *Table_FAQ_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Package* FK_Package_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_FAQ+ m_FK_Package+ m_Summary+ m_Content+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_FAQ_asSQL();
string FK_Package_asSQL();
string Summary_asSQL();
string Content_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

