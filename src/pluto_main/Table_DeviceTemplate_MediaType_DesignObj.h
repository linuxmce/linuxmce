/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_MediaType_DesignObj_H__
#define __Table_DeviceTemplate_MediaType_DesignObj_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_MediaType_DesignObj.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_MediaType_DesignObj : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_MediaType_DesignObj(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_MediaType_DesignObj();

private:		
	friend class Row_DeviceTemplate_MediaType_DesignObj;
	struct Key
	{
		friend class Row_DeviceTemplate_MediaType_DesignObj;
		long int pk_PK_DeviceTemplate_MediaType_DesignObj;

		
		Key(long int in_PK_DeviceTemplate_MediaType_DesignObj);
	
		Key(class Row_DeviceTemplate_MediaType_DesignObj *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_MediaType_DesignObj::Key &key1, const Table_DeviceTemplate_MediaType_DesignObj::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_MediaType_DesignObj*> *rows);
	class Row_DeviceTemplate_MediaType_DesignObj* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_MediaType_DesignObj* GetRow(long int in_PK_DeviceTemplate_MediaType_DesignObj);
	

private:	
	
		
	class Row_DeviceTemplate_MediaType_DesignObj* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_MediaType_DesignObj : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_MediaType_DesignObj::Key;
		friend class Table_DeviceTemplate_MediaType_DesignObj;
	private:
		Table_DeviceTemplate_MediaType_DesignObj *table;
		
		long int m_PK_DeviceTemplate_MediaType_DesignObj;
long int m_FK_DeviceTemplate_MediaType;
long int m_FK_Screen;
long int m_FK_DesignObj_Popup;
long int m_FK_Screen_FileList;
long int m_FK_Screen_OSD;
long int m_FK_Screen_Alt;
long int m_FK_Screen_Alt_OSD;
long int m_FK_Screen_OSD_Speed;
long int m_FK_Screen_OSD_Track;
long int m_FK_Skin;
long int m_UIVersion;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[19];
	
	public:
		long int PK_DeviceTemplate_MediaType_DesignObj_get();
long int FK_DeviceTemplate_MediaType_get();
long int FK_Screen_get();
long int FK_DesignObj_Popup_get();
long int FK_Screen_FileList_get();
long int FK_Screen_OSD_get();
long int FK_Screen_Alt_get();
long int FK_Screen_Alt_OSD_get();
long int FK_Screen_OSD_Speed_get();
long int FK_Screen_OSD_Track_get();
long int FK_Skin_get();
long int UIVersion_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DeviceTemplate_MediaType_DesignObj_set(long int val);
void FK_DeviceTemplate_MediaType_set(long int val);
void FK_Screen_set(long int val);
void FK_DesignObj_Popup_set(long int val);
void FK_Screen_FileList_set(long int val);
void FK_Screen_OSD_set(long int val);
void FK_Screen_Alt_set(long int val);
void FK_Screen_Alt_OSD_set(long int val);
void FK_Screen_OSD_Speed_set(long int val);
void FK_Screen_OSD_Track_set(long int val);
void FK_Skin_set(long int val);
void UIVersion_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_DesignObj_Popup_isNull();
bool FK_Screen_FileList_isNull();
bool FK_Screen_OSD_isNull();
bool FK_Screen_Alt_isNull();
bool FK_Screen_Alt_OSD_isNull();
bool FK_Screen_OSD_Speed_isNull();
bool FK_Screen_OSD_Track_isNull();
bool FK_Skin_isNull();
bool UIVersion_isNull();
bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_DesignObj_Popup_setNull(bool val);
void FK_Screen_FileList_setNull(bool val);
void FK_Screen_OSD_setNull(bool val);
void FK_Screen_Alt_setNull(bool val);
void FK_Screen_Alt_OSD_setNull(bool val);
void FK_Screen_OSD_Speed_setNull(bool val);
void FK_Screen_OSD_Track_setNull(bool val);
void FK_Skin_setNull(bool val);
void UIVersion_setNull(bool val);
void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_MediaType_DesignObj(Table_DeviceTemplate_MediaType_DesignObj *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_MediaType_DesignObj *Table_DeviceTemplate_MediaType_DesignObj_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate_MediaType* FK_DeviceTemplate_MediaType_getrow();
class Row_Screen* FK_Screen_getrow();
class Row_DesignObj* FK_DesignObj_Popup_getrow();
class Row_Screen* FK_Screen_FileList_getrow();
class Row_Screen* FK_Screen_OSD_getrow();
class Row_Screen* FK_Screen_Alt_getrow();
class Row_Skin* FK_Skin_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void RemoteControl_FK_DeviceTemplate_MediaType_DesignObj_getrows(vector <class Row_RemoteControl*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DeviceTemplate_MediaType_DesignObj+ m_FK_DeviceTemplate_MediaType+ m_FK_Screen+ m_FK_DesignObj_Popup+ m_FK_Screen_FileList+ m_FK_Screen_OSD+ m_FK_Screen_Alt+ m_FK_Screen_Alt_OSD+ m_FK_Screen_OSD_Speed+ m_FK_Screen_OSD_Track+ m_FK_Skin+ m_UIVersion+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DeviceTemplate_MediaType_DesignObj_asSQL();
string FK_DeviceTemplate_MediaType_asSQL();
string FK_Screen_asSQL();
string FK_DesignObj_Popup_asSQL();
string FK_Screen_FileList_asSQL();
string FK_Screen_OSD_asSQL();
string FK_Screen_Alt_asSQL();
string FK_Screen_Alt_OSD_asSQL();
string FK_Screen_OSD_Speed_asSQL();
string FK_Screen_OSD_Track_asSQL();
string FK_Skin_asSQL();
string UIVersion_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

