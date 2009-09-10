/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CoverArtScan_H__
#define __Table_CoverArtScan_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CoverArtScan.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CoverArtScan : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_CoverArtScan(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_CoverArtScan();

private:		
	friend class Row_CoverArtScan;
	struct Key
	{
		friend class Row_CoverArtScan;
		long int pk_PK_CoverArtScan;

		
		Key(long int in_PK_CoverArtScan);
	
		Key(class Row_CoverArtScan *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CoverArtScan::Key &key1, const Table_CoverArtScan::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CoverArtScan*> *rows);
	class Row_CoverArtScan* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_CoverArtScan* GetRow(long int in_PK_CoverArtScan);
	

private:	
	
		
	class Row_CoverArtScan* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CoverArtScan : public TableRow, public SerializeClass
	{
		friend struct Table_CoverArtScan::Key;
		friend class Table_CoverArtScan;
	private:
		Table_CoverArtScan *table;
		
		long int m_PK_CoverArtScan;
long int m_FK_File;
long int m_FK_Disc;
long int m_FK_Attribute;
string m_Engine;
string m_Keyword1Type;
string m_Keyword1Search;
string m_Keyword2Type;
string m_Keyword2Search;
string m_Keyword3Type;
string m_Keyword3Search;
short int m_Scanned;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[18];
	
	public:
		long int PK_CoverArtScan_get();
long int FK_File_get();
long int FK_Disc_get();
long int FK_Attribute_get();
string Engine_get();
string Keyword1Type_get();
string Keyword1Search_get();
string Keyword2Type_get();
string Keyword2Search_get();
string Keyword3Type_get();
string Keyword3Search_get();
short int Scanned_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_CoverArtScan_set(long int val);
void FK_File_set(long int val);
void FK_Disc_set(long int val);
void FK_Attribute_set(long int val);
void Engine_set(string val);
void Keyword1Type_set(string val);
void Keyword1Search_set(string val);
void Keyword2Type_set(string val);
void Keyword2Search_set(string val);
void Keyword3Type_set(string val);
void Keyword3Search_set(string val);
void Scanned_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_File_isNull();
bool FK_Disc_isNull();
bool FK_Attribute_isNull();
bool Engine_isNull();
bool Keyword1Type_isNull();
bool Keyword1Search_isNull();
bool Keyword2Type_isNull();
bool Keyword2Search_isNull();
bool Keyword3Type_isNull();
bool Keyword3Search_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_File_setNull(bool val);
void FK_Disc_setNull(bool val);
void FK_Attribute_setNull(bool val);
void Engine_setNull(bool val);
void Keyword1Type_setNull(bool val);
void Keyword1Search_setNull(bool val);
void Keyword2Type_setNull(bool val);
void Keyword2Search_setNull(bool val);
void Keyword3Type_setNull(bool val);
void Keyword3Search_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CoverArtScan(Table_CoverArtScan *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CoverArtScan *Table_CoverArtScan_get() { return table; };

		// Return the rows for foreign keys 
		class Row_File* FK_File_getrow();
class Row_Disc* FK_Disc_getrow();
class Row_Attribute* FK_Attribute_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CoverArtScanEntry_FK_CoverArtScan_getrows(vector <class Row_CoverArtScanEntry*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CoverArtScan+ m_FK_File+ m_FK_Disc+ m_FK_Attribute+ m_Engine+ m_Keyword1Type+ m_Keyword1Search+ m_Keyword2Type+ m_Keyword2Search+ m_Keyword3Type+ m_Keyword3Search+ m_Scanned+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_CoverArtScan_asSQL();
string FK_File_asSQL();
string FK_Disc_asSQL();
string FK_Attribute_asSQL();
string Engine_asSQL();
string Keyword1Type_asSQL();
string Keyword1Search_asSQL();
string Keyword2Type_asSQL();
string Keyword2Search_asSQL();
string Keyword3Type_asSQL();
string Keyword3Search_asSQL();
string Scanned_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

