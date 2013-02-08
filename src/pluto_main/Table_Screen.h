/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Screen_H__
#define __Table_Screen_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Screen.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Screen : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Screen(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Screen();

private:		
	friend class Row_Screen;
	struct Key
	{
		friend class Row_Screen;
		long int pk_PK_Screen;

		
		Key(long int in_PK_Screen);
	
		Key(class Row_Screen *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Screen::Key &key1, const Table_Screen::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Screen*> *rows);
	class Row_Screen* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Screen* GetRow(long int in_PK_Screen);
	

private:	
	
		
	class Row_Screen* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Screen : public TableRow, public SerializeClass
	{
		friend struct Table_Screen::Key;
		friend class Table_Screen;
	private:
		Table_Screen *table;
		
		long int m_PK_Screen;
string m_Description;
string m_Define;
string m_Comments;
short int m_SpecialHandingRecommended;
short int m_AlwaysInclude;
short int m_PreserveCallBacks;
short int m_GoBackToScreen;
short int m_AllowInterruptions;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[15];
	
	public:
		long int PK_Screen_get();
string Description_get();
string Define_get();
string Comments_get();
short int SpecialHandingRecommended_get();
short int AlwaysInclude_get();
short int PreserveCallBacks_get();
short int GoBackToScreen_get();
short int AllowInterruptions_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Screen_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Comments_set(string val);
void SpecialHandingRecommended_set(short int val);
void AlwaysInclude_set(short int val);
void PreserveCallBacks_set(short int val);
void GoBackToScreen_set(short int val);
void AllowInterruptions_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool Define_isNull();
bool Comments_isNull();
bool SpecialHandingRecommended_isNull();
bool AlwaysInclude_isNull();
bool PreserveCallBacks_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void Define_setNull(bool val);
void Comments_setNull(bool val);
void SpecialHandingRecommended_setNull(bool val);
void AlwaysInclude_setNull(bool val);
void PreserveCallBacks_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Screen(Table_Screen *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Screen *Table_Screen_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_MediaType_DesignObj_FK_Screen_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows);
void DeviceTemplate_MediaType_DesignObj_FK_Screen_FileList_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows);
void DeviceTemplate_MediaType_DesignObj_FK_Screen_OSD_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows);
void DeviceTemplate_MediaType_DesignObj_FK_Screen_Alt_getrows(vector <class Row_DeviceTemplate_MediaType_DesignObj*> *rows);
void MediaType_DesignObj_FK_Screen_getrows(vector <class Row_MediaType_DesignObj*> *rows);
void MediaType_DesignObj_FK_Screen_FileList_getrows(vector <class Row_MediaType_DesignObj*> *rows);
void MediaType_DesignObj_FK_Screen_OSD_getrows(vector <class Row_MediaType_DesignObj*> *rows);
void MediaType_DesignObj_FK_Screen_Alt_getrows(vector <class Row_MediaType_DesignObj*> *rows);
void Screen_CommandParameter_FK_Screen_getrows(vector <class Row_Screen_CommandParameter*> *rows);
void Screen_DesignObj_FK_Screen_getrows(vector <class Row_Screen_DesignObj*> *rows);
void Skin_FK_Screen_MainMenu_getrows(vector <class Row_Skin*> *rows);
void Skin_FK_Screen_Sleeping_getrows(vector <class Row_Skin*> *rows);
void Skin_FK_Screen_ScreenSaver_getrows(vector <class Row_Skin*> *rows);


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

