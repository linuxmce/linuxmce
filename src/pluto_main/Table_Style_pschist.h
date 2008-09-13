/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Style_pschist_H__
#define __Table_Style_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Style_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Style_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Style_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Style_pschist();

private:		
	friend class Row_Style_pschist;
	struct Key
	{
		friend class Row_Style_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Style_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Style_pschist::Key &key1, const Table_Style_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Style_pschist*> *rows);
	class Row_Style_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Style_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Style_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Style_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_Style_pschist::Key;
		friend class Table_Style_pschist;
	private:
		Table_Style_pschist *table;
		
		long int m_PK_Style;
string m_Description;
long int m_FK_Style_Selected;
long int m_FK_Style_Highlighted;
long int m_FK_Style_Alt;
short int m_AlwaysIncludeOnOrbiter;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[13];
	
	public:
		long int PK_Style_get();
string Description_get();
long int FK_Style_Selected_get();
long int FK_Style_Highlighted_get();
long int FK_Style_Alt_get();
short int AlwaysIncludeOnOrbiter_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void PK_Style_set(long int val);
void Description_set(string val);
void FK_Style_Selected_set(long int val);
void FK_Style_Highlighted_set(long int val);
void FK_Style_Alt_set(long int val);
void AlwaysIncludeOnOrbiter_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);

		
		bool PK_Style_isNull();
bool Description_isNull();
bool FK_Style_Selected_isNull();
bool FK_Style_Highlighted_isNull();
bool FK_Style_Alt_isNull();
bool AlwaysIncludeOnOrbiter_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();

			
		void PK_Style_setNull(bool val);
void Description_setNull(bool val);
void FK_Style_Selected_setNull(bool val);
void FK_Style_Highlighted_setNull(bool val);
void FK_Style_Alt_setNull(bool val);
void AlwaysIncludeOnOrbiter_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Style_pschist(Table_Style_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Style_pschist *Table_Style_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Style* FK_Style_Selected_getrow();
class Row_Style* FK_Style_Highlighted_getrow();
class Row_Style* FK_Style_Alt_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Style+ m_Description+ m_FK_Style_Selected+ m_FK_Style_Highlighted+ m_FK_Style_Alt+ m_AlwaysIncludeOnOrbiter+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Style_asSQL();
string Description_asSQL();
string FK_Style_Selected_asSQL();
string FK_Style_Highlighted_asSQL();
string FK_Style_Alt_asSQL();
string AlwaysIncludeOnOrbiter_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

