/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_FloorplanObjectType_pschmask_H__
#define __Table_FloorplanObjectType_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_FloorplanObjectType_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_FloorplanObjectType_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_FloorplanObjectType_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_FloorplanObjectType_pschmask();

private:		
	friend class Row_FloorplanObjectType_pschmask;
	struct Key
	{
		friend class Row_FloorplanObjectType_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_FloorplanObjectType_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_FloorplanObjectType_pschmask::Key &key1, const Table_FloorplanObjectType_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_FloorplanObjectType_pschmask*> *rows);
	class Row_FloorplanObjectType_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_FloorplanObjectType_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_FloorplanObjectType_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_FloorplanObjectType_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_FloorplanObjectType_pschmask::Key;
		friend class Table_FloorplanObjectType_pschmask;
	private:
		Table_FloorplanObjectType_pschmask *table;
		
		short int m_PK_FloorplanObjectType;
short int m_FK_FloorplanType;
short int m_Description;
short int m_Define;
short int m_Direction;
short int m_FK_DesignObj_Control;
short int m_Filename;
short int m_FillX;
short int m_FillY;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[15];
	
	public:
		short int PK_FloorplanObjectType_get();
short int FK_FloorplanType_get();
short int Description_get();
short int Define_get();
short int Direction_get();
short int FK_DesignObj_Control_get();
short int Filename_get();
short int FillX_get();
short int FillY_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_FloorplanObjectType_set(short int val);
void FK_FloorplanType_set(short int val);
void Description_set(short int val);
void Define_set(short int val);
void Direction_set(short int val);
void FK_DesignObj_Control_set(short int val);
void Filename_set(short int val);
void FillX_set(short int val);
void FillY_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_FloorplanObjectType_isNull();
bool FK_FloorplanType_isNull();
bool Description_isNull();
bool Define_isNull();
bool Direction_isNull();
bool FK_DesignObj_Control_isNull();
bool Filename_isNull();
bool FillX_isNull();
bool FillY_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_FloorplanObjectType_setNull(bool val);
void FK_FloorplanType_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void Direction_setNull(bool val);
void FK_DesignObj_Control_setNull(bool val);
void Filename_setNull(bool val);
void FillX_setNull(bool val);
void FillY_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_FloorplanObjectType_pschmask(Table_FloorplanObjectType_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_FloorplanObjectType_pschmask *Table_FloorplanObjectType_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_FloorplanType* FK_FloorplanType_getrow();
class Row_DesignObj* FK_DesignObj_Control_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_FloorplanObjectType+ m_FK_FloorplanType+ m_Description+ m_Define+ m_Direction+ m_FK_DesignObj_Control+ m_Filename+ m_FillX+ m_FillY+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_FloorplanObjectType_asSQL();
string FK_FloorplanType_asSQL();
string Description_asSQL();
string Define_asSQL();
string Direction_asSQL();
string FK_DesignObj_Control_asSQL();
string Filename_asSQL();
string FillX_asSQL();
string FillY_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

