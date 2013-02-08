/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_City_H__
#define __Table_City_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_City.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_City : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_City(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_City();

private:		
	friend class Row_City;
	struct Key
	{
		friend class Row_City;
		long int pk_PK_City;

		
		Key(long int in_PK_City);
	
		Key(class Row_City *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_City::Key &key1, const Table_City::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_City*> *rows);
	class Row_City* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_City* GetRow(long int in_PK_City);
	

private:	
	
		
	class Row_City* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_City : public TableRow, public SerializeClass
	{
		friend struct Table_City::Key;
		friend class Table_City;
	private:
		Table_City *table;
		
		long int m_PK_City;
long int m_FK_Country;
long int m_FK_Region;
string m_City;
float m_Latitude;
float m_Longitude;
short int m_DmaId;
string m_County;
string m_Code;
long int m_FK_TimeZone;

		bool is_null[10];
	
	public:
		long int PK_City_get();
long int FK_Country_get();
long int FK_Region_get();
string City_get();
float Latitude_get();
float Longitude_get();
short int DmaId_get();
string County_get();
string Code_get();
long int FK_TimeZone_get();

		
		void PK_City_set(long int val);
void FK_Country_set(long int val);
void FK_Region_set(long int val);
void City_set(string val);
void Latitude_set(float val);
void Longitude_set(float val);
void DmaId_set(short int val);
void County_set(string val);
void Code_set(string val);
void FK_TimeZone_set(long int val);

		
		bool FK_Region_isNull();
bool City_isNull();
bool Latitude_isNull();
bool Longitude_isNull();
bool DmaId_isNull();
bool County_isNull();
bool Code_isNull();
bool FK_TimeZone_isNull();

			
		void FK_Region_setNull(bool val);
void City_setNull(bool val);
void Latitude_setNull(bool val);
void Longitude_setNull(bool val);
void DmaId_setNull(bool val);
void County_setNull(bool val);
void Code_setNull(bool val);
void FK_TimeZone_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_City(Table_City *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_City *Table_City_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Country* FK_Country_getrow();
class Row_Region* FK_Region_getrow();
class Row_TimeZone* FK_TimeZone_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Installation_FK_City_getrows(vector <class Row_Installation*> *rows);
void PostalCode_FK_City_getrows(vector <class Row_PostalCode*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_City+ m_FK_Country+ m_FK_Region+ m_City+ m_Latitude+ m_Longitude+ m_DmaId+ m_County+ m_Code+ m_FK_TimeZone;
		}
	private:
		void SetDefaultValues();
		
		string PK_City_asSQL();
string FK_Country_asSQL();
string FK_Region_asSQL();
string City_asSQL();
string Latitude_asSQL();
string Longitude_asSQL();
string DmaId_asSQL();
string County_asSQL();
string Code_asSQL();
string FK_TimeZone_asSQL();

	};

#endif

