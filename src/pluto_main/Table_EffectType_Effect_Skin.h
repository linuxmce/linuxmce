/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_EffectType_Effect_Skin_H__
#define __Table_EffectType_Effect_Skin_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EffectType_Effect_Skin.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_EffectType_Effect_Skin : public TableBase , TripleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_EffectType_Effect_Skin(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_EffectType_Effect_Skin();

private:		
	friend class Row_EffectType_Effect_Skin;
	struct Key
	{
		friend class Row_EffectType_Effect_Skin;
		long int pk_FK_EffectType;
long int pk_FK_Effect;
long int pk_FK_Skin;

		
		Key(long int in_FK_EffectType, long int in_FK_Effect, long int in_FK_Skin);
	
		Key(class Row_EffectType_Effect_Skin *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EffectType_Effect_Skin::Key &key1, const Table_EffectType_Effect_Skin::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_EffectType_Effect_Skin*> *rows);
	class Row_EffectType_Effect_Skin* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EffectType_Effect_Skin* GetRow(long int in_FK_EffectType, long int in_FK_Effect, long int in_FK_Skin);
	

private:	
	
		
	class Row_EffectType_Effect_Skin* FetchRow(TripleLongKey &key);
		
			
};

class DECLSPECIFIER Row_EffectType_Effect_Skin : public TableRow, public SerializeClass
	{
		friend struct Table_EffectType_Effect_Skin::Key;
		friend class Table_EffectType_Effect_Skin;
	private:
		Table_EffectType_Effect_Skin *table;
		
		long int m_FK_EffectType;
long int m_FK_Effect;
long int m_FK_Skin;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int FK_EffectType_get();
long int FK_Effect_get();
long int FK_Skin_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_EffectType_set(long int val);
void FK_Effect_set(long int val);
void FK_Skin_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EffectType_Effect_Skin(Table_EffectType_Effect_Skin *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EffectType_Effect_Skin *Table_EffectType_Effect_Skin_get() { return table; };

		// Return the rows for foreign keys 
		class Row_EffectType* FK_EffectType_getrow();
class Row_Effect* FK_Effect_getrow();
class Row_Skin* FK_Skin_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_EffectType+ m_FK_Effect+ m_FK_Skin+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_EffectType_asSQL();
string FK_Effect_asSQL();
string FK_Skin_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

