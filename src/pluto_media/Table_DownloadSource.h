/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DownloadSource_H__
#define __Table_DownloadSource_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DownloadSource.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DownloadSource : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_DownloadSource(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_DownloadSource();

private:		
	friend class Row_DownloadSource;
	struct Key
	{
		friend class Row_DownloadSource;
		long int pk_PK_DownloadSource;

		
		Key(long int in_PK_DownloadSource);
	
		Key(class Row_DownloadSource *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DownloadSource::Key &key1, const Table_DownloadSource::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DownloadSource*> *rows);
	class Row_DownloadSource* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_DownloadSource* GetRow(long int in_PK_DownloadSource);
	

private:	
	
		
	class Row_DownloadSource* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DownloadSource : public TableRow, public SerializeClass
	{
		friend struct Table_DownloadSource::Key;
		friend class Table_DownloadSource;
	private:
		Table_DownloadSource *table;
		
		long int m_PK_DownloadSource;
string m_Description;
string m_GrabberScript;
long int m_EK_DesignObj_Popup;
long int m_EK_Screen_Order;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_DownloadSource_get();
string Description_get();
string GrabberScript_get();
long int EK_DesignObj_Popup_get();
long int EK_Screen_Order_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DownloadSource_set(long int val);
void Description_set(string val);
void GrabberScript_set(string val);
void EK_DesignObj_Popup_set(long int val);
void EK_Screen_Order_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool GrabberScript_isNull();
bool EK_DesignObj_Popup_isNull();
bool EK_Screen_Order_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void GrabberScript_setNull(bool val);
void EK_DesignObj_Popup_setNull(bool val);
void EK_Screen_Order_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DownloadSource(Table_DownloadSource *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DownloadSource *Table_DownloadSource_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Download_FK_DownloadSource_getrows(vector <class Row_Download*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DownloadSource+ m_Description+ m_GrabberScript+ m_EK_DesignObj_Popup+ m_EK_Screen_Order+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DownloadSource_asSQL();
string Description_asSQL();
string GrabberScript_asSQL();
string EK_DesignObj_Popup_asSQL();
string EK_Screen_Order_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

