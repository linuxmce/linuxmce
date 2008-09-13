/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_EntertainArea_pschmask_H__
#define __Table_EntertainArea_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EntertainArea_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_EntertainArea_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_EntertainArea_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_EntertainArea_pschmask();

private:		
	friend class Row_EntertainArea_pschmask;
	struct Key
	{
		friend class Row_EntertainArea_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_EntertainArea_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EntertainArea_pschmask::Key &key1, const Table_EntertainArea_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_EntertainArea_pschmask*> *rows);
	class Row_EntertainArea_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EntertainArea_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_EntertainArea_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_EntertainArea_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_EntertainArea_pschmask::Key;
		friend class Table_EntertainArea_pschmask;
	private:
		Table_EntertainArea_pschmask *table;
		
		short int m_PK_EntertainArea;
short int m_FK_Room;
short int m_Only1Stream;
short int m_Description;
short int m_Private;
short int m_FK_FloorplanObjectType;
short int m_FloorplanInfo;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[13];
	
	public:
		short int PK_EntertainArea_get();
short int FK_Room_get();
short int Only1Stream_get();
short int Description_get();
short int Private_get();
short int FK_FloorplanObjectType_get();
short int FloorplanInfo_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_EntertainArea_set(short int val);
void FK_Room_set(short int val);
void Only1Stream_set(short int val);
void Description_set(short int val);
void Private_set(short int val);
void FK_FloorplanObjectType_set(short int val);
void FloorplanInfo_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_EntertainArea_isNull();
bool FK_Room_isNull();
bool Only1Stream_isNull();
bool Description_isNull();
bool Private_isNull();
bool FK_FloorplanObjectType_isNull();
bool FloorplanInfo_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_EntertainArea_setNull(bool val);
void FK_Room_setNull(bool val);
void Only1Stream_setNull(bool val);
void Description_setNull(bool val);
void Private_setNull(bool val);
void FK_FloorplanObjectType_setNull(bool val);
void FloorplanInfo_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EntertainArea_pschmask(Table_EntertainArea_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EntertainArea_pschmask *Table_EntertainArea_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Room* FK_Room_getrow();
class Row_FloorplanObjectType* FK_FloorplanObjectType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_EntertainArea+ m_FK_Room+ m_Only1Stream+ m_Description+ m_Private+ m_FK_FloorplanObjectType+ m_FloorplanInfo+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_EntertainArea_asSQL();
string FK_Room_asSQL();
string Only1Stream_asSQL();
string Description_asSQL();
string Private_asSQL();
string FK_FloorplanObjectType_asSQL();
string FloorplanInfo_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

