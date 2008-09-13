/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Screen_pschmask_H__
#define __Table_Screen_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Screen_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Screen_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Screen_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Screen_pschmask();

private:		
	friend class Row_Screen_pschmask;
	struct Key
	{
		friend class Row_Screen_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Screen_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Screen_pschmask::Key &key1, const Table_Screen_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Screen_pschmask*> *rows);
	class Row_Screen_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Screen_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Screen_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Screen_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Screen_pschmask::Key;
		friend class Table_Screen_pschmask;
	private:
		Table_Screen_pschmask *table;
		
		short int m_PK_Screen;
short int m_Description;
string m_Define;
short int m_Comments;
short int m_SpecialHandingRecommended;
short int m_AlwaysInclude;
short int m_PreserveCallBacks;
short int m_GoBackToScreen;
short int m_AllowInterruptions;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_restrict;

		bool is_null[15];
	
	public:
		short int PK_Screen_get();
short int Description_get();
string Define_get();
short int Comments_get();
short int SpecialHandingRecommended_get();
short int AlwaysInclude_get();
short int PreserveCallBacks_get();
short int GoBackToScreen_get();
short int AllowInterruptions_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_restrict_get();

		
		void PK_Screen_set(short int val);
void Description_set(short int val);
void Define_set(string val);
void Comments_set(short int val);
void SpecialHandingRecommended_set(short int val);
void AlwaysInclude_set(short int val);
void PreserveCallBacks_set(short int val);
void GoBackToScreen_set(short int val);
void AllowInterruptions_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(short int val);

		
		bool PK_Screen_isNull();
bool Description_isNull();
bool Define_isNull();
bool Comments_isNull();
bool SpecialHandingRecommended_isNull();
bool AlwaysInclude_isNull();
bool PreserveCallBacks_isNull();
bool GoBackToScreen_isNull();
bool AllowInterruptions_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_Screen_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void Comments_setNull(bool val);
void SpecialHandingRecommended_setNull(bool val);
void AlwaysInclude_setNull(bool val);
void PreserveCallBacks_setNull(bool val);
void GoBackToScreen_setNull(bool val);
void AllowInterruptions_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Screen_pschmask(Table_Screen_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Screen_pschmask *Table_Screen_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Screen+ m_Description+ m_Define+ m_Comments+ m_SpecialHandingRecommended+ m_AlwaysInclude+ m_PreserveCallBacks+ m_GoBackToScreen+ m_AllowInterruptions+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Screen_asSQL();
string Description_asSQL();
string Define_asSQL();
string Comments_asSQL();
string SpecialHandingRecommended_asSQL();
string AlwaysInclude_asSQL();
string PreserveCallBacks_asSQL();
string GoBackToScreen_asSQL();
string AllowInterruptions_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

