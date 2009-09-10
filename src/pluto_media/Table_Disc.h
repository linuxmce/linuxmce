/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Disc_H__
#define __Table_Disc_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Disc.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Disc : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Disc(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Disc();

private:		
	friend class Row_Disc;
	struct Key
	{
		friend class Row_Disc;
		long int pk_PK_Disc;

		
		Key(long int in_PK_Disc);
	
		Key(class Row_Disc *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Disc::Key &key1, const Table_Disc::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Disc*> *rows);
	class Row_Disc* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Disc* GetRow(long int in_PK_Disc);
	

private:	
	
		
	class Row_Disc* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Disc : public TableRow, public SerializeClass
	{
		friend struct Table_Disc::Key;
		friend class Table_Disc;
	private:
		Table_Disc *table;
		
		long int m_PK_Disc;
string m_ID;
long int m_EK_MediaType;
long int m_FK_MediaSubType;
long int m_FK_FileFormat;
long int m_EK_Users_Private;
string m_DateLastViewed;
short int m_IsNew;
long int m_FK_File;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[15];
	
	public:
		long int PK_Disc_get();
string ID_get();
long int EK_MediaType_get();
long int FK_MediaSubType_get();
long int FK_FileFormat_get();
long int EK_Users_Private_get();
string DateLastViewed_get();
short int IsNew_get();
long int FK_File_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Disc_set(long int val);
void ID_set(string val);
void EK_MediaType_set(long int val);
void FK_MediaSubType_set(long int val);
void FK_FileFormat_set(long int val);
void EK_Users_Private_set(long int val);
void DateLastViewed_set(string val);
void IsNew_set(short int val);
void FK_File_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool ID_isNull();
bool FK_MediaSubType_isNull();
bool FK_FileFormat_isNull();
bool EK_Users_Private_isNull();
bool DateLastViewed_isNull();
bool FK_File_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void ID_setNull(bool val);
void FK_MediaSubType_setNull(bool val);
void FK_FileFormat_setNull(bool val);
void EK_Users_Private_setNull(bool val);
void DateLastViewed_setNull(bool val);
void FK_File_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Disc(Table_Disc *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Disc *Table_Disc_get() { return table; };

		// Return the rows for foreign keys 
		class Row_MediaSubType* FK_MediaSubType_getrow();
class Row_FileFormat* FK_FileFormat_getrow();
class Row_File* FK_File_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Bookmark_FK_Disc_getrows(vector <class Row_Bookmark*> *rows);
void CoverArtScan_FK_Disc_getrows(vector <class Row_CoverArtScan*> *rows);
void DiscLocation_FK_Disc_getrows(vector <class Row_DiscLocation*> *rows);
void Disc_Attribute_FK_Disc_getrows(vector <class Row_Disc_Attribute*> *rows);
void Disc_Users_FK_Disc_getrows(vector <class Row_Disc_Users*> *rows);
void LongAttribute_FK_Disc_getrows(vector <class Row_LongAttribute*> *rows);
void Picture_Disc_FK_Disc_getrows(vector <class Row_Picture_Disc*> *rows);
void RipStatus_FK_Disc_getrows(vector <class Row_RipStatus*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Disc+ m_ID+ m_EK_MediaType+ m_FK_MediaSubType+ m_FK_FileFormat+ m_EK_Users_Private+ m_DateLastViewed+ m_IsNew+ m_FK_File+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Disc_asSQL();
string ID_asSQL();
string EK_MediaType_asSQL();
string FK_MediaSubType_asSQL();
string FK_FileFormat_asSQL();
string EK_Users_Private_asSQL();
string DateLastViewed_asSQL();
string IsNew_asSQL();
string FK_File_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

