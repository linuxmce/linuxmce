/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_QuickStartTemplate_H__
#define __Table_QuickStartTemplate_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_QuickStartTemplate.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_QuickStartTemplate : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_QuickStartTemplate(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_QuickStartTemplate();

private:		
	friend class Row_QuickStartTemplate;
	struct Key
	{
		friend class Row_QuickStartTemplate;
		long int pk_PK_QuickStartTemplate;

		
		Key(long int in_PK_QuickStartTemplate);
	
		Key(class Row_QuickStartTemplate *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_QuickStartTemplate::Key &key1, const Table_QuickStartTemplate::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_QuickStartTemplate*> *rows);
	class Row_QuickStartTemplate* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_QuickStartTemplate* GetRow(long int in_PK_QuickStartTemplate);
	

private:	
	
		
	class Row_QuickStartTemplate* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_QuickStartTemplate : public TableRow, public SerializeClass
	{
		friend struct Table_QuickStartTemplate::Key;
		friend class Table_QuickStartTemplate;
	private:
		Table_QuickStartTemplate *table;
		
		long int m_PK_QuickStartTemplate;
string m_Description;
long int m_FK_QuickStartCategory;
long int m_FK_Package;
string m_Binary;
string m_Arguments;
string m_Homepage;
string m_Icon;
string m_WindowClass;
long int m_FK_DesignObj;
long int m_FK_DesignObj_OSD;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[17];
	
	public:
		long int PK_QuickStartTemplate_get();
string Description_get();
long int FK_QuickStartCategory_get();
long int FK_Package_get();
string Binary_get();
string Arguments_get();
string Homepage_get();
string Icon_get();
string WindowClass_get();
long int FK_DesignObj_get();
long int FK_DesignObj_OSD_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_QuickStartTemplate_set(long int val);
void Description_set(string val);
void FK_QuickStartCategory_set(long int val);
void FK_Package_set(long int val);
void Binary_set(string val);
void Arguments_set(string val);
void Homepage_set(string val);
void Icon_set(string val);
void WindowClass_set(string val);
void FK_DesignObj_set(long int val);
void FK_DesignObj_OSD_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool FK_QuickStartCategory_isNull();
bool FK_Package_isNull();
bool Binary_isNull();
bool Arguments_isNull();
bool Homepage_isNull();
bool Icon_isNull();
bool WindowClass_isNull();
bool FK_DesignObj_isNull();
bool FK_DesignObj_OSD_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void FK_QuickStartCategory_setNull(bool val);
void FK_Package_setNull(bool val);
void Binary_setNull(bool val);
void Arguments_setNull(bool val);
void Homepage_setNull(bool val);
void Icon_setNull(bool val);
void WindowClass_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void FK_DesignObj_OSD_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_QuickStartTemplate(Table_QuickStartTemplate *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_QuickStartTemplate *Table_QuickStartTemplate_get() { return table; };

		// Return the rows for foreign keys 
		class Row_QuickStartCategory* FK_QuickStartCategory_getrow();
class Row_Package* FK_Package_getrow();
class Row_DesignObj* FK_DesignObj_getrow();
class Row_DesignObj* FK_DesignObj_OSD_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Device_QuickStart_FK_QuickStartTemplate_getrows(vector <class Row_Device_QuickStart*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_QuickStartTemplate+ m_Description+ m_FK_QuickStartCategory+ m_FK_Package+ m_Binary+ m_Arguments+ m_Homepage+ m_Icon+ m_WindowClass+ m_FK_DesignObj+ m_FK_DesignObj_OSD+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_QuickStartTemplate_asSQL();
string Description_asSQL();
string FK_QuickStartCategory_asSQL();
string FK_Package_asSQL();
string Binary_asSQL();
string Arguments_asSQL();
string Homepage_asSQL();
string Icon_asSQL();
string WindowClass_asSQL();
string FK_DesignObj_asSQL();
string FK_DesignObj_OSD_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

