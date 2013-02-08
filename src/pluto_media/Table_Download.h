/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Download_H__
#define __Table_Download_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Download.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Download : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Download(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Download();

private:		
	friend class Row_Download;
	struct Key
	{
		friend class Row_Download;
		long int pk_PK_Download;

		
		Key(long int in_PK_Download);
	
		Key(class Row_Download *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Download::Key &key1, const Table_Download::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Download*> *rows);
	class Row_Download* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Download* GetRow(long int in_PK_Download);
	

private:	
	
		
	class Row_Download* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Download : public TableRow, public SerializeClass
	{
		friend struct Table_Download::Key;
		friend class Table_Download;
	private:
		Table_Download *table;
		
		long int m_PK_Download;
long int m_FK_DownloadSource;
long int m_EK_MediaType;
long int m_FK_MediaSubType;
long int m_FK_FileFormat;
string m_MRL;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		long int PK_Download_get();
long int FK_DownloadSource_get();
long int EK_MediaType_get();
long int FK_MediaSubType_get();
long int FK_FileFormat_get();
string MRL_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Download_set(long int val);
void FK_DownloadSource_set(long int val);
void EK_MediaType_set(long int val);
void FK_MediaSubType_set(long int val);
void FK_FileFormat_set(long int val);
void MRL_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_MediaSubType_isNull();
bool FK_FileFormat_isNull();
bool MRL_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_MediaSubType_setNull(bool val);
void FK_FileFormat_setNull(bool val);
void MRL_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Download(Table_Download *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Download *Table_Download_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DownloadSource* FK_DownloadSource_getrow();
class Row_MediaSubType* FK_MediaSubType_getrow();
class Row_FileFormat* FK_FileFormat_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Download_Attribute_FK_Download_getrows(vector <class Row_Download_Attribute*> *rows);
void Picture_Download_FK_Download_getrows(vector <class Row_Picture_Download*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Download+ m_FK_DownloadSource+ m_EK_MediaType+ m_FK_MediaSubType+ m_FK_FileFormat+ m_MRL+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Download_asSQL();
string FK_DownloadSource_asSQL();
string EK_MediaType_asSQL();
string FK_MediaSubType_asSQL();
string FK_FileFormat_asSQL();
string MRL_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

