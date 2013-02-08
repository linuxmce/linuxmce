/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Attribute_Settings_H__
#define __Table_Attribute_Settings_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Attribute_Settings.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Attribute_Settings : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_Attribute_Settings(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_Attribute_Settings();

private:		
	friend class Row_Attribute_Settings;
	struct Key
	{
		friend class Row_Attribute_Settings;
		long int pk_FK_Attribute;

		
		Key(long int in_FK_Attribute);
	
		Key(class Row_Attribute_Settings *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Attribute_Settings::Key &key1, const Table_Attribute_Settings::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Attribute_Settings*> *rows);
	class Row_Attribute_Settings* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_Attribute_Settings* GetRow(long int in_FK_Attribute);
	

private:	
	
		
	class Row_Attribute_Settings* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Attribute_Settings : public TableRow, public SerializeClass
	{
		friend struct Table_Attribute_Settings::Key;
		friend class Table_Attribute_Settings;
	private:
		Table_Attribute_Settings *table;
		
		long int m_FK_Attribute;
string m_VideoSetting;
string m_AudioSetting;
string m_Commands;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int FK_Attribute_get();
string VideoSetting_get();
string AudioSetting_get();
string Commands_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_Attribute_set(long int val);
void VideoSetting_set(string val);
void AudioSetting_set(string val);
void Commands_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool VideoSetting_isNull();
bool AudioSetting_isNull();
bool Commands_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void VideoSetting_setNull(bool val);
void AudioSetting_setNull(bool val);
void Commands_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Attribute_Settings(Table_Attribute_Settings *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Attribute_Settings *Table_Attribute_Settings_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Attribute* FK_Attribute_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_Attribute+ m_VideoSetting+ m_AudioSetting+ m_Commands+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_Attribute_asSQL();
string VideoSetting_asSQL();
string AudioSetting_asSQL();
string Commands_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

