/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Skin_pschmask_H__
#define __Table_Skin_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Skin_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Skin_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Skin_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Skin_pschmask();

private:		
	friend class Row_Skin_pschmask;
	struct Key
	{
		friend class Row_Skin_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Skin_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Skin_pschmask::Key &key1, const Table_Skin_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Skin_pschmask*> *rows);
	class Row_Skin_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Skin_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Skin_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Skin_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Skin_pschmask::Key;
		friend class Table_Skin_pschmask;
	private:
		Table_Skin_pschmask *table;
		
		short int m_PK_Skin;
short int m_Description;
short int m_Define;
short int m_FK_UI;
short int m_MergeStandardVariation;
short int m_DataSubdirectory;
short int m_FK_Style;
short int m_FK_Skin_TextPlacement;
short int m_DrawTextBeforeChildren;
short int m_FK_StabilityStatus;
short int m_FK_Screen_MainMenu;
short int m_FK_Screen_Sleeping;
short int m_FK_Screen_ScreenSaver;
short int m_FK_Package;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[20];
	
	public:
		short int PK_Skin_get();
short int Description_get();
short int Define_get();
short int FK_UI_get();
short int MergeStandardVariation_get();
short int DataSubdirectory_get();
short int FK_Style_get();
short int FK_Skin_TextPlacement_get();
short int DrawTextBeforeChildren_get();
short int FK_StabilityStatus_get();
short int FK_Screen_MainMenu_get();
short int FK_Screen_Sleeping_get();
short int FK_Screen_ScreenSaver_get();
short int FK_Package_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Skin_set(short int val);
void Description_set(short int val);
void Define_set(short int val);
void FK_UI_set(short int val);
void MergeStandardVariation_set(short int val);
void DataSubdirectory_set(short int val);
void FK_Style_set(short int val);
void FK_Skin_TextPlacement_set(short int val);
void DrawTextBeforeChildren_set(short int val);
void FK_StabilityStatus_set(short int val);
void FK_Screen_MainMenu_set(short int val);
void FK_Screen_Sleeping_set(short int val);
void FK_Screen_ScreenSaver_set(short int val);
void FK_Package_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_Skin_isNull();
bool Description_isNull();
bool Define_isNull();
bool FK_UI_isNull();
bool MergeStandardVariation_isNull();
bool DataSubdirectory_isNull();
bool FK_Style_isNull();
bool FK_Skin_TextPlacement_isNull();
bool DrawTextBeforeChildren_isNull();
bool FK_StabilityStatus_isNull();
bool FK_Screen_MainMenu_isNull();
bool FK_Screen_Sleeping_isNull();
bool FK_Screen_ScreenSaver_isNull();
bool FK_Package_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_Skin_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void FK_UI_setNull(bool val);
void MergeStandardVariation_setNull(bool val);
void DataSubdirectory_setNull(bool val);
void FK_Style_setNull(bool val);
void FK_Skin_TextPlacement_setNull(bool val);
void DrawTextBeforeChildren_setNull(bool val);
void FK_StabilityStatus_setNull(bool val);
void FK_Screen_MainMenu_setNull(bool val);
void FK_Screen_Sleeping_setNull(bool val);
void FK_Screen_ScreenSaver_setNull(bool val);
void FK_Package_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Skin_pschmask(Table_Skin_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Skin_pschmask *Table_Skin_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_UI* FK_UI_getrow();
class Row_Style* FK_Style_getrow();
class Row_Skin* FK_Skin_TextPlacement_getrow();
class Row_StabilityStatus* FK_StabilityStatus_getrow();
class Row_Screen* FK_Screen_MainMenu_getrow();
class Row_Screen* FK_Screen_Sleeping_getrow();
class Row_Screen* FK_Screen_ScreenSaver_getrow();
class Row_Package* FK_Package_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Skin+ m_Description+ m_Define+ m_FK_UI+ m_MergeStandardVariation+ m_DataSubdirectory+ m_FK_Style+ m_FK_Skin_TextPlacement+ m_DrawTextBeforeChildren+ m_FK_StabilityStatus+ m_FK_Screen_MainMenu+ m_FK_Screen_Sleeping+ m_FK_Screen_ScreenSaver+ m_FK_Package+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Skin_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_UI_asSQL();
string MergeStandardVariation_asSQL();
string DataSubdirectory_asSQL();
string FK_Style_asSQL();
string FK_Skin_TextPlacement_asSQL();
string DrawTextBeforeChildren_asSQL();
string FK_StabilityStatus_asSQL();
string FK_Screen_MainMenu_asSQL();
string FK_Screen_Sleeping_asSQL();
string FK_Screen_ScreenSaver_asSQL();
string FK_Package_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

