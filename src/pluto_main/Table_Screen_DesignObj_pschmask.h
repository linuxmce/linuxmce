/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Screen_DesignObj_pschmask_H__
#define __Table_Screen_DesignObj_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Screen_DesignObj_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Screen_DesignObj_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Screen_DesignObj_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Screen_DesignObj_pschmask();

private:		
	friend class Row_Screen_DesignObj_pschmask;
	struct Key
	{
		friend class Row_Screen_DesignObj_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Screen_DesignObj_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Screen_DesignObj_pschmask::Key &key1, const Table_Screen_DesignObj_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Screen_DesignObj_pschmask*> *rows);
	class Row_Screen_DesignObj_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Screen_DesignObj_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Screen_DesignObj_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Screen_DesignObj_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Screen_DesignObj_pschmask::Key;
		friend class Table_Screen_DesignObj_pschmask;
	private:
		Table_Screen_DesignObj_pschmask *table;
		
		short int m_PK_Screen_DesignObj;
short int m_FK_Screen;
short int m_FK_DesignObj;
short int m_FK_UI;
short int m_FK_Skin;
short int m_FK_DeviceTemplate;
short int m_AsPopup;
short int m_Popup_X;
short int m_Popup_Y;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_restrict;

		bool is_null[15];
	
	public:
		short int PK_Screen_DesignObj_get();
short int FK_Screen_get();
short int FK_DesignObj_get();
short int FK_UI_get();
short int FK_Skin_get();
short int FK_DeviceTemplate_get();
short int AsPopup_get();
short int Popup_X_get();
short int Popup_Y_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_restrict_get();

		
		void PK_Screen_DesignObj_set(short int val);
void FK_Screen_set(short int val);
void FK_DesignObj_set(short int val);
void FK_UI_set(short int val);
void FK_Skin_set(short int val);
void FK_DeviceTemplate_set(short int val);
void AsPopup_set(short int val);
void Popup_X_set(short int val);
void Popup_Y_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(short int val);

		
		bool PK_Screen_DesignObj_isNull();
bool FK_Screen_isNull();
bool FK_DesignObj_isNull();
bool FK_UI_isNull();
bool FK_Skin_isNull();
bool FK_DeviceTemplate_isNull();
bool AsPopup_isNull();
bool Popup_X_isNull();
bool Popup_Y_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_Screen_DesignObj_setNull(bool val);
void FK_Screen_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void FK_UI_setNull(bool val);
void FK_Skin_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void AsPopup_setNull(bool val);
void Popup_X_setNull(bool val);
void Popup_Y_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Screen_DesignObj_pschmask(Table_Screen_DesignObj_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Screen_DesignObj_pschmask *Table_Screen_DesignObj_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Screen* FK_Screen_getrow();
class Row_DesignObj* FK_DesignObj_getrow();
class Row_UI* FK_UI_getrow();
class Row_Skin* FK_Skin_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Screen_DesignObj+ m_FK_Screen+ m_FK_DesignObj+ m_FK_UI+ m_FK_Skin+ m_FK_DeviceTemplate+ m_AsPopup+ m_Popup_X+ m_Popup_Y+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Screen_DesignObj_asSQL();
string FK_Screen_asSQL();
string FK_DesignObj_asSQL();
string FK_UI_asSQL();
string FK_Skin_asSQL();
string FK_DeviceTemplate_asSQL();
string AsPopup_asSQL();
string Popup_X_asSQL();
string Popup_Y_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

