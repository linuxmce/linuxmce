/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DesignObjVariation_DesignObjParameter_pschmask_H__
#define __Table_DesignObjVariation_DesignObjParameter_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_DesignObjParameter_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DesignObjVariation_DesignObjParameter_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DesignObjVariation_DesignObjParameter_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DesignObjVariation_DesignObjParameter_pschmask();

private:		
	friend class Row_DesignObjVariation_DesignObjParameter_pschmask;
	struct Key
	{
		friend class Row_DesignObjVariation_DesignObjParameter_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DesignObjVariation_DesignObjParameter_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_DesignObjParameter_pschmask::Key &key1, const Table_DesignObjVariation_DesignObjParameter_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_DesignObjParameter_pschmask*> *rows);
	class Row_DesignObjVariation_DesignObjParameter_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_DesignObjParameter_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DesignObjVariation_DesignObjParameter_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DesignObjVariation_DesignObjParameter_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_DesignObjParameter_pschmask::Key;
		friend class Table_DesignObjVariation_DesignObjParameter_pschmask;
	private:
		Table_DesignObjVariation_DesignObjParameter_pschmask *table;
		
		short int m_FK_DesignObjVariation;
short int m_FK_DesignObjParameter;
short int m_Value;
short int m_FK_Skin;
short int m_FK_Language;
short int m_Ignore;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		short int FK_DesignObjVariation_get();
short int FK_DesignObjParameter_get();
short int Value_get();
short int FK_Skin_get();
short int FK_Language_get();
short int Ignore_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_DesignObjVariation_set(short int val);
void FK_DesignObjParameter_set(short int val);
void Value_set(short int val);
void FK_Skin_set(short int val);
void FK_Language_set(short int val);
void Ignore_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_DesignObjVariation_isNull();
bool FK_DesignObjParameter_isNull();
bool Value_isNull();
bool FK_Skin_isNull();
bool FK_Language_isNull();
bool Ignore_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_DesignObjVariation_setNull(bool val);
void FK_DesignObjParameter_setNull(bool val);
void Value_setNull(bool val);
void FK_Skin_setNull(bool val);
void FK_Language_setNull(bool val);
void Ignore_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_DesignObjParameter_pschmask(Table_DesignObjVariation_DesignObjParameter_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_DesignObjParameter_pschmask *Table_DesignObjVariation_DesignObjParameter_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_DesignObjParameter* FK_DesignObjParameter_getrow();
class Row_Skin* FK_Skin_getrow();
class Row_Language* FK_Language_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DesignObjVariation+ m_FK_DesignObjParameter+ m_Value+ m_FK_Skin+ m_FK_Language+ m_Ignore+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_DesignObjVariation_asSQL();
string FK_DesignObjParameter_asSQL();
string Value_asSQL();
string FK_Skin_asSQL();
string FK_Language_asSQL();
string Ignore_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

