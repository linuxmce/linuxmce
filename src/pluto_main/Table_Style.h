/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Style_H__
#define __Table_Style_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Style.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Style : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Style(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Style();

private:		
	friend class Row_Style;
	struct Key
	{
		friend class Row_Style;
		long int pk_PK_Style;

		
		Key(long int in_PK_Style);
	
		Key(class Row_Style *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Style::Key &key1, const Table_Style::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Style*> *rows);
	class Row_Style* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Style* GetRow(long int in_PK_Style);
	

private:	
	
		
	class Row_Style* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Style : public TableRow, public SerializeClass
	{
		friend struct Table_Style::Key;
		friend class Table_Style;
	private:
		Table_Style *table;
		
		long int m_PK_Style;
string m_Description;
long int m_FK_Style_Selected;
long int m_FK_Style_Highlighted;
long int m_FK_Style_Alt;
short int m_AlwaysIncludeOnOrbiter;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		long int PK_Style_get();
string Description_get();
long int FK_Style_Selected_get();
long int FK_Style_Highlighted_get();
long int FK_Style_Alt_get();
short int AlwaysIncludeOnOrbiter_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Style_set(long int val);
void Description_set(string val);
void FK_Style_Selected_set(long int val);
void FK_Style_Highlighted_set(long int val);
void FK_Style_Alt_set(long int val);
void AlwaysIncludeOnOrbiter_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool FK_Style_Selected_isNull();
bool FK_Style_Highlighted_isNull();
bool FK_Style_Alt_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void FK_Style_Selected_setNull(bool val);
void FK_Style_Highlighted_setNull(bool val);
void FK_Style_Alt_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Style(Table_Style *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Style *Table_Style_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Style* FK_Style_Selected_getrow();
class Row_Style* FK_Style_Highlighted_getrow();
class Row_Style* FK_Style_Alt_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DesignObjVariation_Text_Skin_Language_FK_Style_getrows(vector <class Row_DesignObjVariation_Text_Skin_Language*> *rows);
void Skin_FK_Style_getrows(vector <class Row_Skin*> *rows);
void Style_FK_Style_Selected_getrows(vector <class Row_Style*> *rows);
void Style_FK_Style_Highlighted_getrows(vector <class Row_Style*> *rows);
void Style_FK_Style_Alt_getrows(vector <class Row_Style*> *rows);
void StyleVariation_FK_Style_getrows(vector <class Row_StyleVariation*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Style+ m_Description+ m_FK_Style_Selected+ m_FK_Style_Highlighted+ m_FK_Style_Alt+ m_AlwaysIncludeOnOrbiter+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Style_asSQL();
string Description_asSQL();
string FK_Style_Selected_asSQL();
string FK_Style_Highlighted_asSQL();
string FK_Style_Alt_asSQL();
string AlwaysIncludeOnOrbiter_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

