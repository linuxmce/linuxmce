/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_PlaylistEntry_H__
#define __Table_PlaylistEntry_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PlaylistEntry.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PlaylistEntry : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_PlaylistEntry(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_PlaylistEntry();

private:		
	friend class Row_PlaylistEntry;
	struct Key
	{
		friend class Row_PlaylistEntry;
		long int pk_PK_PlaylistEntry;

		
		Key(long int in_PK_PlaylistEntry);
	
		Key(class Row_PlaylistEntry *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PlaylistEntry::Key &key1, const Table_PlaylistEntry::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_PlaylistEntry*> *rows);
	class Row_PlaylistEntry* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_PlaylistEntry* GetRow(long int in_PK_PlaylistEntry);
	

private:	
	
		
	class Row_PlaylistEntry* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PlaylistEntry : public TableRow, public SerializeClass
	{
		friend struct Table_PlaylistEntry::Key;
		friend class Table_PlaylistEntry;
	private:
		Table_PlaylistEntry *table;
		
		long int m_PK_PlaylistEntry;
long int m_FK_Playlist;
long int m_FK_File;
long int m_FK_Bookmark;
string m_Path;
string m_Filename;
long int m_Order;
long int m_Duration;
long int m_EK_CommandGroup_Start;
long int m_EK_CommandGroup_Stop;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		long int PK_PlaylistEntry_get();
long int FK_Playlist_get();
long int FK_File_get();
long int FK_Bookmark_get();
string Path_get();
string Filename_get();
long int Order_get();
long int Duration_get();
long int EK_CommandGroup_Start_get();
long int EK_CommandGroup_Stop_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_PlaylistEntry_set(long int val);
void FK_Playlist_set(long int val);
void FK_File_set(long int val);
void FK_Bookmark_set(long int val);
void Path_set(string val);
void Filename_set(string val);
void Order_set(long int val);
void Duration_set(long int val);
void EK_CommandGroup_Start_set(long int val);
void EK_CommandGroup_Stop_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_File_isNull();
bool FK_Bookmark_isNull();
bool Path_isNull();
bool Filename_isNull();
bool Order_isNull();
bool Duration_isNull();
bool EK_CommandGroup_Start_isNull();
bool EK_CommandGroup_Stop_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_File_setNull(bool val);
void FK_Bookmark_setNull(bool val);
void Path_setNull(bool val);
void Filename_setNull(bool val);
void Order_setNull(bool val);
void Duration_setNull(bool val);
void EK_CommandGroup_Start_setNull(bool val);
void EK_CommandGroup_Stop_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PlaylistEntry(Table_PlaylistEntry *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PlaylistEntry *Table_PlaylistEntry_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Playlist* FK_Playlist_getrow();
class Row_File* FK_File_getrow();
class Row_Bookmark* FK_Bookmark_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PlaylistEntry+ m_FK_Playlist+ m_FK_File+ m_FK_Bookmark+ m_Path+ m_Filename+ m_Order+ m_Duration+ m_EK_CommandGroup_Start+ m_EK_CommandGroup_Stop+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_PlaylistEntry_asSQL();
string FK_Playlist_asSQL();
string FK_File_asSQL();
string FK_Bookmark_asSQL();
string Path_asSQL();
string Filename_asSQL();
string Order_asSQL();
string Duration_asSQL();
string EK_CommandGroup_Start_asSQL();
string EK_CommandGroup_Stop_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

