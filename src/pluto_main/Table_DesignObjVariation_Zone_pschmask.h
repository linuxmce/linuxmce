/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DesignObjVariation_Zone_pschmask_H__
#define __Table_DesignObjVariation_Zone_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_Zone_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DesignObjVariation_Zone_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DesignObjVariation_Zone_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DesignObjVariation_Zone_pschmask();

private:		
	friend class Row_DesignObjVariation_Zone_pschmask;
	struct Key
	{
		friend class Row_DesignObjVariation_Zone_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DesignObjVariation_Zone_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_Zone_pschmask::Key &key1, const Table_DesignObjVariation_Zone_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_Zone_pschmask*> *rows);
	class Row_DesignObjVariation_Zone_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_Zone_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DesignObjVariation_Zone_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DesignObjVariation_Zone_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_Zone_pschmask::Key;
		friend class Table_DesignObjVariation_Zone_pschmask;
	private:
		Table_DesignObjVariation_Zone_pschmask *table;
		
		short int m_PK_DesignObjVariation_Zone;
short int m_FK_DesignObjVariation;
short int m_FK_Button;
short int m_FK_CommandGroup_D;
short int m_FK_DesignObj_Goto;
short int m_Description;
short int m_X;
short int m_Y;
short int m_Width;
short int m_Height;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		short int PK_DesignObjVariation_Zone_get();
short int FK_DesignObjVariation_get();
short int FK_Button_get();
short int FK_CommandGroup_D_get();
short int FK_DesignObj_Goto_get();
short int Description_get();
short int X_get();
short int Y_get();
short int Width_get();
short int Height_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DesignObjVariation_Zone_set(short int val);
void FK_DesignObjVariation_set(short int val);
void FK_Button_set(short int val);
void FK_CommandGroup_D_set(short int val);
void FK_DesignObj_Goto_set(short int val);
void Description_set(short int val);
void X_set(short int val);
void Y_set(short int val);
void Width_set(short int val);
void Height_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_DesignObjVariation_Zone_isNull();
bool FK_DesignObjVariation_isNull();
bool FK_Button_isNull();
bool FK_CommandGroup_D_isNull();
bool FK_DesignObj_Goto_isNull();
bool Description_isNull();
bool X_isNull();
bool Y_isNull();
bool Width_isNull();
bool Height_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_DesignObjVariation_Zone_setNull(bool val);
void FK_DesignObjVariation_setNull(bool val);
void FK_Button_setNull(bool val);
void FK_CommandGroup_D_setNull(bool val);
void FK_DesignObj_Goto_setNull(bool val);
void Description_setNull(bool val);
void X_setNull(bool val);
void Y_setNull(bool val);
void Width_setNull(bool val);
void Height_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_Zone_pschmask(Table_DesignObjVariation_Zone_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_Zone_pschmask *Table_DesignObjVariation_Zone_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation* FK_DesignObjVariation_getrow();
class Row_Button* FK_Button_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_getrow();
class Row_DesignObj* FK_DesignObj_Goto_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObjVariation_Zone+ m_FK_DesignObjVariation+ m_FK_Button+ m_FK_CommandGroup_D+ m_FK_DesignObj_Goto+ m_Description+ m_X+ m_Y+ m_Width+ m_Height+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_Zone_asSQL();
string FK_DesignObjVariation_asSQL();
string FK_Button_asSQL();
string FK_CommandGroup_D_asSQL();
string FK_DesignObj_Goto_asSQL();
string Description_asSQL();
string X_asSQL();
string Y_asSQL();
string Width_asSQL();
string Height_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

