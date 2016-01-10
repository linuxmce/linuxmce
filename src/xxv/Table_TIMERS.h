/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_TIMERS_H__
#define __Table_TIMERS_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_TIMERS.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_TIMERS : public TableBase , SingleLongKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_TIMERS(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_TIMERS();

private:		
	friend class Row_TIMERS;
	struct Key
	{
		friend class Row_TIMERS;
		long int pk_Id;

		
		Key(long int in_Id);
	
		Key(class Row_TIMERS *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_TIMERS::Key &key1, const Table_TIMERS::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_TIMERS*> *rows);
	class Row_TIMERS* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_TIMERS* GetRow(long int in_Id);
	

private:	
	
		
	class Row_TIMERS* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_TIMERS : public TableRow, public SerializeClass
	{
		friend struct Table_TIMERS::Key;
		friend class Table_TIMERS;
	private:
		Table_TIMERS *table;
		
		long int m_Id;
string m_Status;
string m_ChannelID;
string m_Day;
long int m_Start;
long int m_Stop;
short int m_Priority;
short int m_Lifetime;
string m_File;
string m_Summary;
string m_NextStartTime;
string m_NextStopTime;
string m_Collision;
u_int64_t m_NextEpgId;
long int m_AutotimerId;
string m_addtime;
string m_Checked;

		bool is_null[17];
	
	public:
		long int Id_get();
string Status_get();
string ChannelID_get();
string Day_get();
long int Start_get();
long int Stop_get();
short int Priority_get();
short int Lifetime_get();
string File_get();
string Summary_get();
string NextStartTime_get();
string NextStopTime_get();
string Collision_get();
u_int64_t NextEpgId_get();
long int AutotimerId_get();
string addtime_get();
string Checked_get();

		
		void Id_set(long int val);
void Status_set(string val);
void ChannelID_set(string val);
void Day_set(string val);
void Start_set(long int val);
void Stop_set(long int val);
void Priority_set(short int val);
void Lifetime_set(short int val);
void File_set(string val);
void Summary_set(string val);
void NextStartTime_set(string val);
void NextStopTime_set(string val);
void Collision_set(string val);
void NextEpgId_set(u_int64_t val);
void AutotimerId_set(long int val);
void addtime_set(string val);
void Checked_set(string val);

		
		bool Status_isNull();
bool Day_isNull();
bool Start_isNull();
bool Stop_isNull();
bool Priority_isNull();
bool Lifetime_isNull();
bool File_isNull();
bool Summary_isNull();
bool NextStartTime_isNull();
bool NextStopTime_isNull();
bool Collision_isNull();
bool NextEpgId_isNull();
bool AutotimerId_isNull();
bool Checked_isNull();

			
		void Status_setNull(bool val);
void Day_setNull(bool val);
void Start_setNull(bool val);
void Stop_setNull(bool val);
void Priority_setNull(bool val);
void Lifetime_setNull(bool val);
void File_setNull(bool val);
void Summary_setNull(bool val);
void NextStartTime_setNull(bool val);
void NextStopTime_setNull(bool val);
void Collision_setNull(bool val);
void NextEpgId_setNull(bool val);
void AutotimerId_setNull(bool val);
void Checked_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_TIMERS(Table_TIMERS *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_TIMERS *Table_TIMERS_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_Id+ m_Status+ m_ChannelID+ m_Day+ m_Start+ m_Stop+ m_Priority+ m_Lifetime+ m_File+ m_Summary+ m_NextStartTime+ m_NextStopTime+ m_Collision+ m_NextEpgId+ m_AutotimerId+ m_addtime+ m_Checked;
		}
	private:
		void SetDefaultValues();
		
		string Id_asSQL();
string Status_asSQL();
string ChannelID_asSQL();
string Day_asSQL();
string Start_asSQL();
string Stop_asSQL();
string Priority_asSQL();
string Lifetime_asSQL();
string File_asSQL();
string Summary_asSQL();
string NextStartTime_asSQL();
string NextStopTime_asSQL();
string Collision_asSQL();
string NextEpgId_asSQL();
string AutotimerId_asSQL();
string addtime_asSQL();
string Checked_asSQL();

	};

#endif

