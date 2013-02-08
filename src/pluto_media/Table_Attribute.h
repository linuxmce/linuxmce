/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Attribute_H__
#define __Table_Attribute_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Attribute.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Attribute : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Attribute(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Attribute();

private:		
	friend class Row_Attribute;
	struct Key
	{
		friend class Row_Attribute;
		long int pk_PK_Attribute;

		
		Key(long int in_PK_Attribute);
	
		Key(class Row_Attribute *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Attribute::Key &key1, const Table_Attribute::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Attribute*> *rows);
	class Row_Attribute* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Attribute* GetRow(long int in_PK_Attribute);
	

private:	
	
		
	class Row_Attribute* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Attribute : public TableRow, public SerializeClass
	{
		friend struct Table_Attribute::Key;
		friend class Table_Attribute;
	private:
		Table_Attribute *table;
		
		long int m_PK_Attribute;
long int m_FK_AttributeType;
string m_Name;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[9];
	
	public:
		long int PK_Attribute_get();
long int FK_AttributeType_get();
string Name_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Attribute_set(long int val);
void FK_AttributeType_set(long int val);
void Name_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_AttributeType_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_AttributeType_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Attribute(Table_Attribute *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Attribute *Table_Attribute_get() { return table; };

		// Return the rows for foreign keys 
		class Row_AttributeType* FK_AttributeType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Attribute_Settings_FK_Attribute_getrows(vector <class Row_Attribute_Settings*> *rows);
void CoverArtScan_FK_Attribute_getrows(vector <class Row_CoverArtScan*> *rows);
void Disc_Attribute_FK_Attribute_getrows(vector <class Row_Disc_Attribute*> *rows);
void Download_Attribute_FK_Attribute_getrows(vector <class Row_Download_Attribute*> *rows);
void File_Attribute_FK_Attribute_getrows(vector <class Row_File_Attribute*> *rows);
void LongAttribute_FK_Attribute_getrows(vector <class Row_LongAttribute*> *rows);
void Picture_Attribute_FK_Attribute_getrows(vector <class Row_Picture_Attribute*> *rows);
void SearchToken_Attribute_FK_Attribute_getrows(vector <class Row_SearchToken_Attribute*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Attribute+ m_FK_AttributeType+ m_Name+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Attribute_asSQL();
string FK_AttributeType_asSQL();
string Name_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

