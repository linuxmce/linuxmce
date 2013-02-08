/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_PostalCode_H__
#define __Table_PostalCode_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PostalCode.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PostalCode : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_PostalCode(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_PostalCode();

private:		
	friend class Row_PostalCode;
	struct Key
	{
		friend class Row_PostalCode;
		long int pk_PK_PostalCode;

		
		Key(long int in_PK_PostalCode);
	
		Key(class Row_PostalCode *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PostalCode::Key &key1, const Table_PostalCode::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_PostalCode*> *rows);
	class Row_PostalCode* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PostalCode* GetRow(long int in_PK_PostalCode);
	

private:	
	
		
	class Row_PostalCode* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PostalCode : public TableRow, public SerializeClass
	{
		friend struct Table_PostalCode::Key;
		friend class Table_PostalCode;
	private:
		Table_PostalCode *table;
		
		long int m_PK_PostalCode;
string m_PostalCode;
string m_Long;
string m_Lat;
string m_City;
string m_State;
string m_County;
long int m_FK_City;
long int m_FK_Country;

		bool is_null[9];
	
	public:
		long int PK_PostalCode_get();
string PostalCode_get();
string Long_get();
string Lat_get();
string City_get();
string State_get();
string County_get();
long int FK_City_get();
long int FK_Country_get();

		
		void PK_PostalCode_set(long int val);
void PostalCode_set(string val);
void Long_set(string val);
void Lat_set(string val);
void City_set(string val);
void State_set(string val);
void County_set(string val);
void FK_City_set(long int val);
void FK_Country_set(long int val);

		
		bool Lat_isNull();
bool City_isNull();
bool State_isNull();
bool County_isNull();
bool FK_City_isNull();

			
		void Lat_setNull(bool val);
void City_setNull(bool val);
void State_setNull(bool val);
void County_setNull(bool val);
void FK_City_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PostalCode(Table_PostalCode *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PostalCode *Table_PostalCode_get() { return table; };

		// Return the rows for foreign keys 
		class Row_City* FK_City_getrow();
class Row_Country* FK_Country_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Installation_FK_PostalCode_getrows(vector <class Row_Installation*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PostalCode+ m_PostalCode+ m_Long+ m_Lat+ m_City+ m_State+ m_County+ m_FK_City+ m_FK_Country;
		}
	private:
		void SetDefaultValues();
		
		string PK_PostalCode_asSQL();
string PostalCode_asSQL();
string Long_asSQL();
string Lat_asSQL();
string City_asSQL();
string State_asSQL();
string County_asSQL();
string FK_City_asSQL();
string FK_Country_asSQL();

	};

#endif

