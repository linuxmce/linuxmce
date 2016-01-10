/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_OLDEPG_H__
#define __Table_OLDEPG_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_OLDEPG.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_OLDEPG : public TableBase , SingleLong64KeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_OLDEPG(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_OLDEPG();

private:		
	friend class Row_OLDEPG;
	struct Key
	{
		friend class Row_OLDEPG;
		u_int64_t pk_eventid;

		
		Key(u_int64_t in_eventid);
	
		Key(class Row_OLDEPG *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_OLDEPG::Key &key1, const Table_OLDEPG::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_OLDEPG*> *rows);
	class Row_OLDEPG* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_OLDEPG* GetRow(u_int64_t in_eventid);
	

private:	
	
		
	class Row_OLDEPG* FetchRow(SingleLong64Key &key);
		
			
};

class DECLSPECIFIER Row_OLDEPG : public TableRow, public SerializeClass
	{
		friend struct Table_OLDEPG::Key;
		friend class Table_OLDEPG;
	private:
		Table_OLDEPG *table;
		
		u_int64_t m_eventid;
string m_title;
string m_subtitle;
string m_description;
string m_channel_id;
string m_starttime;
long int m_duration;
short int m_tableid;
string m_image;
short int m_version;
string m_video;
string m_audio;
string m_addtime;
string m_vpstime;

		bool is_null[14];
	
	public:
		u_int64_t eventid_get();
string title_get();
string subtitle_get();
string description_get();
string channel_id_get();
string starttime_get();
long int duration_get();
short int tableid_get();
string image_get();
short int version_get();
string video_get();
string audio_get();
string addtime_get();
string vpstime_get();

		
		void eventid_set(u_int64_t val);
void title_set(string val);
void subtitle_set(string val);
void description_set(string val);
void channel_id_set(string val);
void starttime_set(string val);
void duration_set(long int val);
void tableid_set(short int val);
void image_set(string val);
void version_set(short int val);
void video_set(string val);
void audio_set(string val);
void addtime_set(string val);
void vpstime_set(string val);

		
		bool subtitle_isNull();
bool description_isNull();
bool tableid_isNull();
bool image_isNull();
bool version_isNull();
bool video_isNull();
bool audio_isNull();
bool vpstime_isNull();

			
		void subtitle_setNull(bool val);
void description_setNull(bool val);
void tableid_setNull(bool val);
void image_setNull(bool val);
void version_setNull(bool val);
void video_setNull(bool val);
void audio_setNull(bool val);
void vpstime_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_OLDEPG(Table_OLDEPG *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_OLDEPG *Table_OLDEPG_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_eventid+ m_title+ m_subtitle+ m_description+ m_channel_id+ m_starttime+ m_duration+ m_tableid+ m_image+ m_version+ m_video+ m_audio+ m_addtime+ m_vpstime;
		}
	private:
		void SetDefaultValues();
		
		string eventid_asSQL();
string title_asSQL();
string subtitle_asSQL();
string description_asSQL();
string channel_id_asSQL();
string starttime_asSQL();
string duration_asSQL();
string tableid_asSQL();
string image_asSQL();
string version_asSQL();
string video_asSQL();
string audio_asSQL();
string addtime_asSQL();
string vpstime_asSQL();

	};

#endif

