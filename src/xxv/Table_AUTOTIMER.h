/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_AUTOTIMER_H__
#define __Table_AUTOTIMER_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_AUTOTIMER.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_AUTOTIMER : public TableBase , SingleLongKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_AUTOTIMER(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_AUTOTIMER();

private:		
	friend class Row_AUTOTIMER;
	struct Key
	{
		friend class Row_AUTOTIMER;
		long int pk_Id;

		
		Key(long int in_Id);
	
		Key(class Row_AUTOTIMER *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_AUTOTIMER::Key &key1, const Table_AUTOTIMER::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_AUTOTIMER*> *rows);
	class Row_AUTOTIMER* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_AUTOTIMER* GetRow(long int in_Id);
	

private:	
	
		
	class Row_AUTOTIMER* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_AUTOTIMER : public TableRow, public SerializeClass
	{
		friend struct Table_AUTOTIMER::Key;
		friend class Table_AUTOTIMER;
	private:
		Table_AUTOTIMER *table;
		
		long int m_Id;
string m_Activ;
string m_Done;
string m_Search;
string m_InFields;
string m_Channels;
string m_Start;
string m_Stop;
short int m_MinLength;
short int m_Priority;
short int m_Lifetime;
string m_Dir;
string m_VPS;
short int m_prevminutes;
short int m_afterminutes;
string m_Weekdays;
string m_startdate;
string m_stopdate;
long int m_count;

		bool is_null[19];
	
	public:
		long int Id_get();
string Activ_get();
string Done_get();
string Search_get();
string InFields_get();
string Channels_get();
string Start_get();
string Stop_get();
short int MinLength_get();
short int Priority_get();
short int Lifetime_get();
string Dir_get();
string VPS_get();
short int prevminutes_get();
short int afterminutes_get();
string Weekdays_get();
string startdate_get();
string stopdate_get();
long int count_get();

		
		void Id_set(long int val);
void Activ_set(string val);
void Done_set(string val);
void Search_set(string val);
void InFields_set(string val);
void Channels_set(string val);
void Start_set(string val);
void Stop_set(string val);
void MinLength_set(short int val);
void Priority_set(short int val);
void Lifetime_set(short int val);
void Dir_set(string val);
void VPS_set(string val);
void prevminutes_set(short int val);
void afterminutes_set(short int val);
void Weekdays_set(string val);
void startdate_set(string val);
void stopdate_set(string val);
void count_set(long int val);

		
		bool Activ_isNull();
bool Channels_isNull();
bool Start_isNull();
bool Stop_isNull();
bool MinLength_isNull();
bool Priority_isNull();
bool Lifetime_isNull();
bool Dir_isNull();
bool VPS_isNull();
bool prevminutes_isNull();
bool afterminutes_isNull();
bool Weekdays_isNull();
bool startdate_isNull();
bool stopdate_isNull();
bool count_isNull();

			
		void Activ_setNull(bool val);
void Channels_setNull(bool val);
void Start_setNull(bool val);
void Stop_setNull(bool val);
void MinLength_setNull(bool val);
void Priority_setNull(bool val);
void Lifetime_setNull(bool val);
void Dir_setNull(bool val);
void VPS_setNull(bool val);
void prevminutes_setNull(bool val);
void afterminutes_setNull(bool val);
void Weekdays_setNull(bool val);
void startdate_setNull(bool val);
void stopdate_setNull(bool val);
void count_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_AUTOTIMER(Table_AUTOTIMER *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_AUTOTIMER *Table_AUTOTIMER_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_Id+ m_Activ+ m_Done+ m_Search+ m_InFields+ m_Channels+ m_Start+ m_Stop+ m_MinLength+ m_Priority+ m_Lifetime+ m_Dir+ m_VPS+ m_prevminutes+ m_afterminutes+ m_Weekdays+ m_startdate+ m_stopdate+ m_count;
		}
	private:
		void SetDefaultValues();
		
		string Id_asSQL();
string Activ_asSQL();
string Done_asSQL();
string Search_asSQL();
string InFields_asSQL();
string Channels_asSQL();
string Start_asSQL();
string Stop_asSQL();
string MinLength_asSQL();
string Priority_asSQL();
string Lifetime_asSQL();
string Dir_asSQL();
string VPS_asSQL();
string prevminutes_asSQL();
string afterminutes_asSQL();
string Weekdays_asSQL();
string startdate_asSQL();
string stopdate_asSQL();
string count_asSQL();

	};

#endif

