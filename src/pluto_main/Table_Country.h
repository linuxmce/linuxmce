/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Country_H__
#define __Table_Country_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Country.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Country : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Country(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Country();

private:		
	friend class Row_Country;
	struct Key
	{
		friend class Row_Country;
		long int pk_PK_Country;

		
		Key(long int in_PK_Country);
	
		Key(class Row_Country *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Country::Key &key1, const Table_Country::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Country*> *rows);
	class Row_Country* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Country* GetRow(long int in_PK_Country);
	

private:	
	
		
	class Row_Country* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Country : public TableRow, public SerializeClass
	{
		friend struct Table_Country::Key;
		friend class Table_Country;
	private:
		Table_Country *table;
		
		long int m_PK_Country;
string m_Description;
string m_Define;
short int m_UsePostalCode;
string m_country_code;

		bool is_null[5];
	
	public:
		long int PK_Country_get();
string Description_get();
string Define_get();
short int UsePostalCode_get();
string country_code_get();

		
		void PK_Country_set(long int val);
void Description_set(string val);
void Define_set(string val);
void UsePostalCode_set(short int val);
void country_code_set(string val);

		
		bool country_code_isNull();

			
		void country_code_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Country(Table_Country *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Country *Table_Country_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void City_FK_Country_getrows(vector <class Row_City*> *rows);
void Installation_FK_Country_getrows(vector <class Row_Installation*> *rows);
void PostalCode_FK_Country_getrows(vector <class Row_PostalCode*> *rows);
void Region_FK_Country_getrows(vector <class Row_Region*> *rows);
void RepositorySource_URL_FK_Country_getrows(vector <class Row_RepositorySource_URL*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Country+ m_Description+ m_Define+ m_UsePostalCode+ m_country_code;
		}
	private:
		void SetDefaultValues();
		
		string PK_Country_asSQL();
string Description_asSQL();
string Define_asSQL();
string UsePostalCode_asSQL();
string country_code_asSQL();

	};

#endif

