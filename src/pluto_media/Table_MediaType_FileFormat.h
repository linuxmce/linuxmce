/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_MediaType_FileFormat_H__
#define __Table_MediaType_FileFormat_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_MediaType_FileFormat.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_MediaType_FileFormat : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_MediaType_FileFormat(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_MediaType_FileFormat();

private:		
	friend class Row_MediaType_FileFormat;
	struct Key
	{
		friend class Row_MediaType_FileFormat;
		long int pk_EK_MediaType;
long int pk_FK_FileFormat;

		
		Key(long int in_EK_MediaType, long int in_FK_FileFormat);
	
		Key(class Row_MediaType_FileFormat *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_MediaType_FileFormat::Key &key1, const Table_MediaType_FileFormat::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_MediaType_FileFormat*> *rows);
	class Row_MediaType_FileFormat* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_MediaType_FileFormat* GetRow(long int in_EK_MediaType, long int in_FK_FileFormat);
	

private:	
	
		
	class Row_MediaType_FileFormat* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_MediaType_FileFormat : public TableRow, public SerializeClass
	{
		friend struct Table_MediaType_FileFormat::Key;
		friend class Table_MediaType_FileFormat;
	private:
		Table_MediaType_FileFormat *table;
		
		long int m_EK_MediaType;
long int m_FK_FileFormat;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[8];
	
	public:
		long int EK_MediaType_get();
long int FK_FileFormat_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void EK_MediaType_set(long int val);
void FK_FileFormat_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_MediaType_FileFormat(Table_MediaType_FileFormat *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_MediaType_FileFormat *Table_MediaType_FileFormat_get() { return table; };

		// Return the rows for foreign keys 
		class Row_FileFormat* FK_FileFormat_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_EK_MediaType+ m_FK_FileFormat+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string EK_MediaType_asSQL();
string FK_FileFormat_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

