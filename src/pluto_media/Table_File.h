/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_File_H__
#define __Table_File_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_File.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_File : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_File(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_File();

private:		
	friend class Row_File;
	struct Key
	{
		friend class Row_File;
		long int pk_PK_File;

		
		Key(long int in_PK_File);
	
		Key(class Row_File *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_File::Key &key1, const Table_File::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_File*> *rows);
	class Row_File* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_File* GetRow(long int in_PK_File);
	

private:	
	
		
	class Row_File* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_File : public TableRow, public SerializeClass
	{
		friend struct Table_File::Key;
		friend class Table_File;
	private:
		Table_File *table;
		
		long int m_PK_File;
long int m_EK_MediaType;
long int m_FK_MediaSubType;
long int m_FK_FileFormat;
long int m_FK_FileGroup;
string m_DateAdded;
string m_Path;
string m_Filename;
long int m_Missing;
long int m_IsDirectory;
long int m_EK_Users_Private;
long int m_EK_Device;
string m_ModificationDate;
long int m_AttrCount;
string m_AttrDate;
string m_DateLastViewed;
short int m_IsNew;
short int m_Ignore;
long int m_INode;
string m_MD5;
string m_Source;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[27];
	
	public:
		long int PK_File_get();
long int EK_MediaType_get();
long int FK_MediaSubType_get();
long int FK_FileFormat_get();
long int FK_FileGroup_get();
string DateAdded_get();
string Path_get();
string Filename_get();
long int Missing_get();
long int IsDirectory_get();
long int EK_Users_Private_get();
long int EK_Device_get();
string ModificationDate_get();
long int AttrCount_get();
string AttrDate_get();
string DateLastViewed_get();
short int IsNew_get();
short int Ignore_get();
long int INode_get();
string MD5_get();
string Source_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_File_set(long int val);
void EK_MediaType_set(long int val);
void FK_MediaSubType_set(long int val);
void FK_FileFormat_set(long int val);
void FK_FileGroup_set(long int val);
void DateAdded_set(string val);
void Path_set(string val);
void Filename_set(string val);
void Missing_set(long int val);
void IsDirectory_set(long int val);
void EK_Users_Private_set(long int val);
void EK_Device_set(long int val);
void ModificationDate_set(string val);
void AttrCount_set(long int val);
void AttrDate_set(string val);
void DateLastViewed_set(string val);
void IsNew_set(short int val);
void Ignore_set(short int val);
void INode_set(long int val);
void MD5_set(string val);
void Source_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool EK_MediaType_isNull();
bool FK_MediaSubType_isNull();
bool FK_FileFormat_isNull();
bool FK_FileGroup_isNull();
bool DateAdded_isNull();
bool EK_Users_Private_isNull();
bool EK_Device_isNull();
bool ModificationDate_isNull();
bool AttrCount_isNull();
bool AttrDate_isNull();
bool DateLastViewed_isNull();
bool INode_isNull();
bool MD5_isNull();
bool Source_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void EK_MediaType_setNull(bool val);
void FK_MediaSubType_setNull(bool val);
void FK_FileFormat_setNull(bool val);
void FK_FileGroup_setNull(bool val);
void DateAdded_setNull(bool val);
void EK_Users_Private_setNull(bool val);
void EK_Device_setNull(bool val);
void ModificationDate_setNull(bool val);
void AttrCount_setNull(bool val);
void AttrDate_setNull(bool val);
void DateLastViewed_setNull(bool val);
void INode_setNull(bool val);
void MD5_setNull(bool val);
void Source_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_File(Table_File *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_File *Table_File_get() { return table; };

		// Return the rows for foreign keys 
		class Row_MediaSubType* FK_MediaSubType_getrow();
class Row_FileFormat* FK_FileFormat_getrow();
class Row_FileGroup* FK_FileGroup_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Bookmark_FK_File_getrows(vector <class Row_Bookmark*> *rows);
void CoverArtScan_FK_File_getrows(vector <class Row_CoverArtScan*> *rows);
void Disc_FK_File_getrows(vector <class Row_Disc*> *rows);
void File_Attribute_FK_File_getrows(vector <class Row_File_Attribute*> *rows);
void File_Users_FK_File_getrows(vector <class Row_File_Users*> *rows);
void LongAttribute_FK_File_getrows(vector <class Row_LongAttribute*> *rows);
void Picture_File_FK_File_getrows(vector <class Row_Picture_File*> *rows);
void PlaylistEntry_FK_File_getrows(vector <class Row_PlaylistEntry*> *rows);
void RipStatus_FK_File_getrows(vector <class Row_RipStatus*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_File+ m_EK_MediaType+ m_FK_MediaSubType+ m_FK_FileFormat+ m_FK_FileGroup+ m_DateAdded+ m_Path+ m_Filename+ m_Missing+ m_IsDirectory+ m_EK_Users_Private+ m_EK_Device+ m_ModificationDate+ m_AttrCount+ m_AttrDate+ m_DateLastViewed+ m_IsNew+ m_Ignore+ m_INode+ m_MD5+ m_Source+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_File_asSQL();
string EK_MediaType_asSQL();
string FK_MediaSubType_asSQL();
string FK_FileFormat_asSQL();
string FK_FileGroup_asSQL();
string DateAdded_asSQL();
string Path_asSQL();
string Filename_asSQL();
string Missing_asSQL();
string IsDirectory_asSQL();
string EK_Users_Private_asSQL();
string EK_Device_asSQL();
string ModificationDate_asSQL();
string AttrCount_asSQL();
string AttrDate_asSQL();
string DateLastViewed_asSQL();
string IsNew_asSQL();
string Ignore_asSQL();
string INode_asSQL();
string MD5_asSQL();
string Source_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

