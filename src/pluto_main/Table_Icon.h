/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Icon_H__
#define __Table_Icon_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Icon.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Icon : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Icon(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Icon();

private:		
	friend class Row_Icon;
	struct Key
	{
		friend class Row_Icon;
		long int pk_PK_Icon;

		
		Key(long int in_PK_Icon);
	
		Key(class Row_Icon *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Icon::Key &key1, const Table_Icon::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Icon*> *rows);
	class Row_Icon* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Icon* GetRow(long int in_PK_Icon);
	

private:	
	
		
	class Row_Icon* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Icon : public TableRow, public SerializeClass
	{
		friend struct Table_Icon::Key;
		friend class Table_Icon;
	private:
		Table_Icon *table;
		
		long int m_PK_Icon;
string m_Define;
string m_Description;
long int m_TransparentColor;
string m_MainFileName;
string m_SelectedFileName;
string m_AltFileNames;
string m_BackgroundFileName;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[14];
	
	public:
		long int PK_Icon_get();
string Define_get();
string Description_get();
long int TransparentColor_get();
string MainFileName_get();
string SelectedFileName_get();
string AltFileNames_get();
string BackgroundFileName_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Icon_set(long int val);
void Define_set(string val);
void Description_set(string val);
void TransparentColor_set(long int val);
void MainFileName_set(string val);
void SelectedFileName_set(string val);
void AltFileNames_set(string val);
void BackgroundFileName_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool TransparentColor_isNull();
bool SelectedFileName_isNull();
bool AltFileNames_isNull();
bool BackgroundFileName_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void TransparentColor_setNull(bool val);
void SelectedFileName_setNull(bool val);
void AltFileNames_setNull(bool val);
void BackgroundFileName_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Icon(Table_Icon *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Icon *Table_Icon_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_FK_Icon_getrows(vector <class Row_CommandGroup*> *rows);
void Floorplan_FK_Icon_getrows(vector <class Row_Floorplan*> *rows);
void Room_FK_Icon_getrows(vector <class Row_Room*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Icon+ m_Define+ m_Description+ m_TransparentColor+ m_MainFileName+ m_SelectedFileName+ m_AltFileNames+ m_BackgroundFileName+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Icon_asSQL();
string Define_asSQL();
string Description_asSQL();
string TransparentColor_asSQL();
string MainFileName_asSQL();
string SelectedFileName_asSQL();
string AltFileNames_asSQL();
string BackgroundFileName_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

