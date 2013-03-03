/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_License_H__
#define __Table_License_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_License.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_License : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_License(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_License();

private:		
	friend class Row_License;
	struct Key
	{
		friend class Row_License;
		long int pk_PK_License;

		
		Key(long int in_PK_License);
	
		Key(class Row_License *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_License::Key &key1, const Table_License::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_License*> *rows);
	class Row_License* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_License* GetRow(long int in_PK_License);
	

private:	
	
		
	class Row_License* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_License : public TableRow, public SerializeClass
	{
		friend struct Table_License::Key;
		friend class Table_License;
	private:
		Table_License *table;
		
		long int m_PK_License;
string m_Description;
string m_Define;
string m_Summary;
string m_URL;
short int m_RequiresPayment;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		long int PK_License_get();
string Description_get();
string Define_get();
string Summary_get();
string URL_get();
short int RequiresPayment_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_License_set(long int val);
void Description_set(string val);
void Define_set(string val);
void Summary_set(string val);
void URL_set(string val);
void RequiresPayment_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Summary_isNull();
bool URL_isNull();
bool RequiresPayment_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Summary_setNull(bool val);
void URL_setNull(bool val);
void RequiresPayment_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_License(Table_License *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_License *Table_License_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Package_FK_License_getrows(vector <class Row_Package*> *rows);
void PaidLicense_FK_License_getrows(vector <class Row_PaidLicense*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_License+ m_Description+ m_Define+ m_Summary+ m_URL+ m_RequiresPayment+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_License_asSQL();
string Description_asSQL();
string Define_asSQL();
string Summary_asSQL();
string URL_asSQL();
string RequiresPayment_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

