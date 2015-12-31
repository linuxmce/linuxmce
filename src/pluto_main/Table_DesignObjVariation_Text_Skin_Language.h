/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DesignObjVariation_Text_Skin_Language_H__
#define __Table_DesignObjVariation_Text_Skin_Language_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_Text_Skin_Language.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DesignObjVariation_Text_Skin_Language : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DesignObjVariation_Text_Skin_Language(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DesignObjVariation_Text_Skin_Language();

private:		
	friend class Row_DesignObjVariation_Text_Skin_Language;
	struct Key
	{
		friend class Row_DesignObjVariation_Text_Skin_Language;
		long int pk_PK_DesignObjVariation_Text_Skin_Language;

		
		Key(long int in_PK_DesignObjVariation_Text_Skin_Language);
	
		Key(class Row_DesignObjVariation_Text_Skin_Language *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_Text_Skin_Language::Key &key1, const Table_DesignObjVariation_Text_Skin_Language::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_Text_Skin_Language*> *rows);
	class Row_DesignObjVariation_Text_Skin_Language* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_Text_Skin_Language* GetRow(long int in_PK_DesignObjVariation_Text_Skin_Language);
	

private:	
	
		
	class Row_DesignObjVariation_Text_Skin_Language* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DesignObjVariation_Text_Skin_Language : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_Text_Skin_Language::Key;
		friend class Table_DesignObjVariation_Text_Skin_Language;
	private:
		Table_DesignObjVariation_Text_Skin_Language *table;
		
		long int m_PK_DesignObjVariation_Text_Skin_Language;
long int m_FK_DesignObjVariation_Text;
long int m_FK_Skin;
long int m_FK_Language;
long int m_X;
long int m_Y;
long int m_Width;
long int m_Height;
long int m_Rotate;
long int m_Opacity;
long int m_FK_HorizAlignment;
long int m_FK_VertAlignment;
long int m_FK_Style;
long int m_PlainBackgroundColor;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[20];
	
	public:
		long int PK_DesignObjVariation_Text_Skin_Language_get();
long int FK_DesignObjVariation_Text_get();
long int FK_Skin_get();
long int FK_Language_get();
long int X_get();
long int Y_get();
long int Width_get();
long int Height_get();
long int Rotate_get();
long int Opacity_get();
long int FK_HorizAlignment_get();
long int FK_VertAlignment_get();
long int FK_Style_get();
long int PlainBackgroundColor_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DesignObjVariation_Text_Skin_Language_set(long int val);
void FK_DesignObjVariation_Text_set(long int val);
void FK_Skin_set(long int val);
void FK_Language_set(long int val);
void X_set(long int val);
void Y_set(long int val);
void Width_set(long int val);
void Height_set(long int val);
void Rotate_set(long int val);
void Opacity_set(long int val);
void FK_HorizAlignment_set(long int val);
void FK_VertAlignment_set(long int val);
void FK_Style_set(long int val);
void PlainBackgroundColor_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Skin_isNull();
bool FK_Language_isNull();
bool Rotate_isNull();
bool Opacity_isNull();
bool FK_HorizAlignment_isNull();
bool FK_VertAlignment_isNull();
bool FK_Style_isNull();
bool PlainBackgroundColor_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Skin_setNull(bool val);
void FK_Language_setNull(bool val);
void Rotate_setNull(bool val);
void Opacity_setNull(bool val);
void FK_HorizAlignment_setNull(bool val);
void FK_VertAlignment_setNull(bool val);
void FK_Style_setNull(bool val);
void PlainBackgroundColor_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_Text_Skin_Language(Table_DesignObjVariation_Text_Skin_Language *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_Text_Skin_Language *Table_DesignObjVariation_Text_Skin_Language_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjVariation_Text* FK_DesignObjVariation_Text_getrow();
class Row_Skin* FK_Skin_getrow();
class Row_Language* FK_Language_getrow();
class Row_HorizAlignment* FK_HorizAlignment_getrow();
class Row_VertAlignment* FK_VertAlignment_getrow();
class Row_Style* FK_Style_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObjVariation_Text_Skin_Language+ m_FK_DesignObjVariation_Text+ m_FK_Skin+ m_FK_Language+ m_X+ m_Y+ m_Width+ m_Height+ m_Rotate+ m_Opacity+ m_FK_HorizAlignment+ m_FK_VertAlignment+ m_FK_Style+ m_PlainBackgroundColor+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_Text_Skin_Language_asSQL();
string FK_DesignObjVariation_Text_asSQL();
string FK_Skin_asSQL();
string FK_Language_asSQL();
string X_asSQL();
string Y_asSQL();
string Width_asSQL();
string Height_asSQL();
string Rotate_asSQL();
string Opacity_asSQL();
string FK_HorizAlignment_asSQL();
string FK_VertAlignment_asSQL();
string FK_Style_asSQL();
string PlainBackgroundColor_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

