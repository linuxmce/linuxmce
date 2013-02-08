/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Datapoints_H__
#define __Table_Datapoints_H__

#include "TableRow.h"
#include "Database_lmce_datalog.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Datapoints.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Datapoints : public TableBase , SingleLongKeyBase
{
private:
	Database_lmce_datalog *database;
	struct Key;	//forward declaration
	
public:
	Table_Datapoints(Database_lmce_datalog *pDatabase):database(pDatabase)
	{
	};
	~Table_Datapoints();

private:		
	friend class Row_Datapoints;
	struct Key
	{
		friend class Row_Datapoints;
		long int pk_PK_Datapoints;

		
		Key(long int in_PK_Datapoints);
	
		Key(class Row_Datapoints *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Datapoints::Key &key1, const Table_Datapoints::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Datapoints*> *rows);
	class Row_Datapoints* AddRow();
	Database_lmce_datalog *Database_lmce_datalog_get() { return database; }
	
		
	class Row_Datapoints* GetRow(long int in_PK_Datapoints);
	

private:	
	
		
	class Row_Datapoints* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Datapoints : public TableRow, public SerializeClass
	{
		friend struct Table_Datapoints::Key;
		friend class Table_Datapoints;
	private:
		Table_Datapoints *table;
		
		long int m_PK_Datapoints;
float m_Datapoint;
long int m_FK_Unit;
long int m_EK_Device;
string m_timestamp;

		bool is_null[5];
	
	public:
		long int PK_Datapoints_get();
float Datapoint_get();
long int FK_Unit_get();
long int EK_Device_get();
string timestamp_get();

		
		void PK_Datapoints_set(long int val);
void Datapoint_set(float val);
void FK_Unit_set(long int val);
void EK_Device_set(long int val);
void timestamp_set(string val);

		
		bool Datapoint_isNull();
bool timestamp_isNull();

			
		void Datapoint_setNull(bool val);
void timestamp_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Datapoints(Table_Datapoints *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Datapoints *Table_Datapoints_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Unit* FK_Unit_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Datapoints+ m_Datapoint+ m_FK_Unit+ m_EK_Device+ m_timestamp;
		}
	private:
		void SetDefaultValues();
		
		string PK_Datapoints_asSQL();
string Datapoint_asSQL();
string FK_Unit_asSQL();
string EK_Device_asSQL();
string timestamp_asSQL();

	};

#endif

