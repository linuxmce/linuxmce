/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_AV_pschist_H__
#define __Table_DeviceTemplate_AV_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_AV_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_AV_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_AV_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_AV_pschist();

private:		
	friend class Row_DeviceTemplate_AV_pschist;
	struct Key
	{
		friend class Row_DeviceTemplate_AV_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DeviceTemplate_AV_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_AV_pschist::Key &key1, const Table_DeviceTemplate_AV_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_AV_pschist*> *rows);
	class Row_DeviceTemplate_AV_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_AV_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DeviceTemplate_AV_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_AV_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_AV_pschist::Key;
		friend class Table_DeviceTemplate_AV_pschist;
	private:
		Table_DeviceTemplate_AV_pschist *table;
		
		long int m_FK_DeviceTemplate;
short int m_UsesIR;
long int m_IR_PowerDelay;
long int m_IR_ModeDelay;
long int m_DigitDelay;
long int m_TogglePower;
long int m_ToggleDSP;
long int m_ToggleInput;
long int m_ToggleOutput;
string m_NumericEntry;
short int m_MonitorInputChanges;
long int m_RepeatIR;
long int m_RepeatVolume;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[20];
	
	public:
		long int FK_DeviceTemplate_get();
short int UsesIR_get();
long int IR_PowerDelay_get();
long int IR_ModeDelay_get();
long int DigitDelay_get();
long int TogglePower_get();
long int ToggleDSP_get();
long int ToggleInput_get();
long int ToggleOutput_get();
string NumericEntry_get();
short int MonitorInputChanges_get();
long int RepeatIR_get();
long int RepeatVolume_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void FK_DeviceTemplate_set(long int val);
void UsesIR_set(short int val);
void IR_PowerDelay_set(long int val);
void IR_ModeDelay_set(long int val);
void DigitDelay_set(long int val);
void TogglePower_set(long int val);
void ToggleDSP_set(long int val);
void ToggleInput_set(long int val);
void ToggleOutput_set(long int val);
void NumericEntry_set(string val);
void MonitorInputChanges_set(short int val);
void RepeatIR_set(long int val);
void RepeatVolume_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);

		
		bool FK_DeviceTemplate_isNull();
bool UsesIR_isNull();
bool IR_PowerDelay_isNull();
bool IR_ModeDelay_isNull();
bool DigitDelay_isNull();
bool TogglePower_isNull();
bool ToggleDSP_isNull();
bool ToggleInput_isNull();
bool ToggleOutput_isNull();
bool NumericEntry_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();

			
		void FK_DeviceTemplate_setNull(bool val);
void UsesIR_setNull(bool val);
void IR_PowerDelay_setNull(bool val);
void IR_ModeDelay_setNull(bool val);
void DigitDelay_setNull(bool val);
void TogglePower_setNull(bool val);
void ToggleDSP_setNull(bool val);
void ToggleInput_setNull(bool val);
void ToggleOutput_setNull(bool val);
void NumericEntry_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_AV_pschist(Table_DeviceTemplate_AV_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_AV_pschist *Table_DeviceTemplate_AV_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_UsesIR+ m_IR_PowerDelay+ m_IR_ModeDelay+ m_DigitDelay+ m_TogglePower+ m_ToggleDSP+ m_ToggleInput+ m_ToggleOutput+ m_NumericEntry+ m_MonitorInputChanges+ m_RepeatIR+ m_RepeatVolume+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string UsesIR_asSQL();
string IR_PowerDelay_asSQL();
string IR_ModeDelay_asSQL();
string DigitDelay_asSQL();
string TogglePower_asSQL();
string ToggleDSP_asSQL();
string ToggleInput_asSQL();
string ToggleOutput_asSQL();
string NumericEntry_asSQL();
string MonitorInputChanges_asSQL();
string RepeatIR_asSQL();
string RepeatVolume_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

