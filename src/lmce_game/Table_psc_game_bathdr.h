/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_psc_game_bathdr_H__
#define __Table_psc_game_bathdr_H__

#include "TableRow.h"
#include "Database_lmce_game.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_psc_game_bathdr.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_psc_game_bathdr : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_game *database;
	struct Key;	//forward declaration
	
public:
	Table_psc_game_bathdr(Database_lmce_game *pDatabase):database(pDatabase)
	{
	};
	~Table_psc_game_bathdr();

private:		
	friend class Row_psc_game_bathdr;
	struct Key
	{
		friend class Row_psc_game_bathdr;
		long int pk_PK_psc_game_bathdr;

		
		Key(long int in_PK_psc_game_bathdr);
	
		Key(class Row_psc_game_bathdr *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_psc_game_bathdr::Key &key1, const Table_psc_game_bathdr::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_psc_game_bathdr*> *rows);
	class Row_psc_game_bathdr* AddRow();
	Database_lmce_game *Database_lmce_game_get() { return database; }
	
		
	class Row_psc_game_bathdr* GetRow(long int in_PK_psc_game_bathdr);
	

private:	
	
		
	class Row_psc_game_bathdr* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_psc_game_bathdr : public TableRow, public SerializeClass
	{
		friend struct Table_psc_game_bathdr::Key;
		friend class Table_psc_game_bathdr;
	private:
		Table_psc_game_bathdr *table;
		
		long int m_PK_psc_game_bathdr;
string m_IPAddress;
string m_date;
string m_comments;

		bool is_null[4];
	
	public:
		long int PK_psc_game_bathdr_get();
string IPAddress_get();
string date_get();
string comments_get();

		
		void PK_psc_game_bathdr_set(long int val);
void IPAddress_set(string val);
void date_set(string val);
void comments_set(string val);

		
		bool IPAddress_isNull();
bool date_isNull();
bool comments_isNull();

			
		void IPAddress_setNull(bool val);
void date_setNull(bool val);
void comments_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_psc_game_bathdr(Table_psc_game_bathdr *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_psc_game_bathdr *Table_psc_game_bathdr_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void psc_game_batdet_FK_psc_game_bathdr_getrows(vector <class Row_psc_game_batdet*> *rows);
void psc_game_batdet_FK_psc_game_bathdr_orig_getrows(vector <class Row_psc_game_batdet*> *rows);
void psc_game_batdet_FK_psc_game_bathdr_auth_getrows(vector <class Row_psc_game_batdet*> *rows);
void psc_game_batdet_FK_psc_game_bathdr_unauth_getrows(vector <class Row_psc_game_batdet*> *rows);
void psc_game_batuser_FK_psc_game_bathdr_getrows(vector <class Row_psc_game_batuser*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_psc_game_bathdr+ m_IPAddress+ m_date+ m_comments;
		}
	private:
		void SetDefaultValues();
		
		string PK_psc_game_bathdr_asSQL();
string IPAddress_asSQL();
string date_asSQL();
string comments_asSQL();

	};

#endif

