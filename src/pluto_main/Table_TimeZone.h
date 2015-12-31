/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_TimeZone_H__
#define __Table_TimeZone_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_TimeZone.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_TimeZone : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_TimeZone(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_TimeZone();

private:		
	friend class Row_TimeZone;
	struct Key
	{
		friend class Row_TimeZone;
		long int pk_PK_TimeZone;

		
		Key(long int in_PK_TimeZone);
	
		Key(class Row_TimeZone *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_TimeZone::Key &key1, const Table_TimeZone::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_TimeZone*> *rows);
	class Row_TimeZone* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_TimeZone* GetRow(long int in_PK_TimeZone);
	

private:	
	
		
	class Row_TimeZone* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_TimeZone : public TableRow, public SerializeClass
	{
		friend struct Table_TimeZone::Key;
		friend class Table_TimeZone;
	private:
		Table_TimeZone *table;
		
		long int m_PK_TimeZone;
string m_ZoneName;
string m_Coordinate;
double m_TimeZoneOffset;
double m_SummerTimeOffset;
string m_Comments;
string m_CountryCode;

		bool is_null[7];
	
	public:
		long int PK_TimeZone_get();
string ZoneName_get();
string Coordinate_get();
double TimeZoneOffset_get();
double SummerTimeOffset_get();
string Comments_get();
string CountryCode_get();

		
		void PK_TimeZone_set(long int val);
void ZoneName_set(string val);
void Coordinate_set(string val);
void TimeZoneOffset_set(double val);
void SummerTimeOffset_set(double val);
void Comments_set(string val);
void CountryCode_set(string val);

		
		bool ZoneName_isNull();
bool Coordinate_isNull();
bool TimeZoneOffset_isNull();
bool SummerTimeOffset_isNull();
bool Comments_isNull();
bool CountryCode_isNull();

			
		void ZoneName_setNull(bool val);
void Coordinate_setNull(bool val);
void TimeZoneOffset_setNull(bool val);
void SummerTimeOffset_setNull(bool val);
void Comments_setNull(bool val);
void CountryCode_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_TimeZone(Table_TimeZone *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_TimeZone *Table_TimeZone_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void City_FK_TimeZone_getrows(vector <class Row_City*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_TimeZone+ m_ZoneName+ m_Coordinate+ m_TimeZoneOffset+ m_SummerTimeOffset+ m_Comments+ m_CountryCode;
		}
	private:
		void SetDefaultValues();
		
		string PK_TimeZone_asSQL();
string ZoneName_asSQL();
string Coordinate_asSQL();
string TimeZoneOffset_asSQL();
string SummerTimeOffset_asSQL();
string Comments_asSQL();
string CountryCode_asSQL();

	};

#endif

