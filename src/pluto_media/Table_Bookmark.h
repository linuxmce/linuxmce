/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Bookmark_H__
#define __Table_Bookmark_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Bookmark.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Bookmark : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Bookmark(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Bookmark();

private:		
	friend class Row_Bookmark;
	struct Key
	{
		friend class Row_Bookmark;
		long int pk_PK_Bookmark;

		
		Key(long int in_PK_Bookmark);
	
		Key(class Row_Bookmark *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Bookmark::Key &key1, const Table_Bookmark::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Bookmark*> *rows);
	class Row_Bookmark* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Bookmark* GetRow(long int in_PK_Bookmark);
	

private:	
	
		
	class Row_Bookmark* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Bookmark : public TableRow, public SerializeClass
	{
		friend struct Table_Bookmark::Key;
		friend class Table_Bookmark;
	private:
		Table_Bookmark *table;
		
		long int m_PK_Bookmark;
long int m_FK_File;
long int m_FK_Disc;
long int m_FK_Playlist;
long int m_FK_MediaProvider;
long int m_EK_MediaType;
long int m_FK_Picture;
long int m_EK_Users;
string m_Description;
string m_Position;
short int m_IsAutoResume;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[17];
	
	public:
		long int PK_Bookmark_get();
long int FK_File_get();
long int FK_Disc_get();
long int FK_Playlist_get();
long int FK_MediaProvider_get();
long int EK_MediaType_get();
long int FK_Picture_get();
long int EK_Users_get();
string Description_get();
string Position_get();
short int IsAutoResume_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Bookmark_set(long int val);
void FK_File_set(long int val);
void FK_Disc_set(long int val);
void FK_Playlist_set(long int val);
void FK_MediaProvider_set(long int val);
void EK_MediaType_set(long int val);
void FK_Picture_set(long int val);
void EK_Users_set(long int val);
void Description_set(string val);
void Position_set(string val);
void IsAutoResume_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_File_isNull();
bool FK_Disc_isNull();
bool FK_Playlist_isNull();
bool FK_MediaProvider_isNull();
bool FK_Picture_isNull();
bool EK_Users_isNull();
bool Description_isNull();
bool Position_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_File_setNull(bool val);
void FK_Disc_setNull(bool val);
void FK_Playlist_setNull(bool val);
void FK_MediaProvider_setNull(bool val);
void FK_Picture_setNull(bool val);
void EK_Users_setNull(bool val);
void Description_setNull(bool val);
void Position_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Bookmark(Table_Bookmark *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Bookmark *Table_Bookmark_get() { return table; };

		// Return the rows for foreign keys 
		class Row_File* FK_File_getrow();
class Row_Disc* FK_Disc_getrow();
class Row_Playlist* FK_Playlist_getrow();
class Row_MediaProvider* FK_MediaProvider_getrow();
class Row_Picture* FK_Picture_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void PlaylistEntry_FK_Bookmark_getrows(vector <class Row_PlaylistEntry*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Bookmark+ m_FK_File+ m_FK_Disc+ m_FK_Playlist+ m_FK_MediaProvider+ m_EK_MediaType+ m_FK_Picture+ m_EK_Users+ m_Description+ m_Position+ m_IsAutoResume+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Bookmark_asSQL();
string FK_File_asSQL();
string FK_Disc_asSQL();
string FK_Playlist_asSQL();
string FK_MediaProvider_asSQL();
string EK_MediaType_asSQL();
string FK_Picture_asSQL();
string EK_Users_asSQL();
string Description_asSQL();
string Position_asSQL();
string IsAutoResume_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

