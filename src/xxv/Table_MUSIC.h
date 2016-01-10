/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_MUSIC_H__
#define __Table_MUSIC_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_MUSIC.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_MUSIC : public TableBase , SingleLongKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_MUSIC(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_MUSIC();

private:		
	friend class Row_MUSIC;
	struct Key
	{
		friend class Row_MUSIC;
		long int pk_Id;

		
		Key(long int in_Id);
	
		Key(class Row_MUSIC *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_MUSIC::Key &key1, const Table_MUSIC::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_MUSIC*> *rows);
	class Row_MUSIC* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_MUSIC* GetRow(long int in_Id);
	

private:	
	
		
	class Row_MUSIC* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_MUSIC : public TableRow, public SerializeClass
	{
		friend struct Table_MUSIC::Key;
		friend class Table_MUSIC;
	private:
		Table_MUSIC *table;
		
		long int m_Id;
string m_FILE;
string m_ARTIST;
string m_ALBUM;
string m_TITLE;
string m_COMMENT;
string m_TRACKNUM;
short int m_YEAR;
string m_GENRE;
short int m_BITRATE;
string m_FREQUENCY;
long int m_SECS;

		bool is_null[12];
	
	public:
		long int Id_get();
string FILE_get();
string ARTIST_get();
string ALBUM_get();
string TITLE_get();
string COMMENT_get();
string TRACKNUM_get();
short int YEAR_get();
string GENRE_get();
short int BITRATE_get();
string FREQUENCY_get();
long int SECS_get();

		
		void Id_set(long int val);
void FILE_set(string val);
void ARTIST_set(string val);
void ALBUM_set(string val);
void TITLE_set(string val);
void COMMENT_set(string val);
void TRACKNUM_set(string val);
void YEAR_set(short int val);
void GENRE_set(string val);
void BITRATE_set(short int val);
void FREQUENCY_set(string val);
void SECS_set(long int val);

		
		bool ARTIST_isNull();
bool ALBUM_isNull();
bool TITLE_isNull();
bool COMMENT_isNull();
bool TRACKNUM_isNull();
bool YEAR_isNull();
bool GENRE_isNull();
bool BITRATE_isNull();
bool FREQUENCY_isNull();

			
		void ARTIST_setNull(bool val);
void ALBUM_setNull(bool val);
void TITLE_setNull(bool val);
void COMMENT_setNull(bool val);
void TRACKNUM_setNull(bool val);
void YEAR_setNull(bool val);
void GENRE_setNull(bool val);
void BITRATE_setNull(bool val);
void FREQUENCY_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_MUSIC(Table_MUSIC *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_MUSIC *Table_MUSIC_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_Id+ m_FILE+ m_ARTIST+ m_ALBUM+ m_TITLE+ m_COMMENT+ m_TRACKNUM+ m_YEAR+ m_GENRE+ m_BITRATE+ m_FREQUENCY+ m_SECS;
		}
	private:
		void SetDefaultValues();
		
		string Id_asSQL();
string FILE_asSQL();
string ARTIST_asSQL();
string ALBUM_asSQL();
string TITLE_asSQL();
string COMMENT_asSQL();
string TRACKNUM_asSQL();
string YEAR_asSQL();
string GENRE_asSQL();
string BITRATE_asSQL();
string FREQUENCY_asSQL();
string SECS_asSQL();

	};

#endif

