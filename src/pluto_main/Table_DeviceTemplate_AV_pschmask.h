/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_AV_pschmask_H__
#define __Table_DeviceTemplate_AV_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_AV_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_AV_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_AV_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_AV_pschmask();

private:		
	friend class Row_DeviceTemplate_AV_pschmask;
	struct Key
	{
		friend class Row_DeviceTemplate_AV_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DeviceTemplate_AV_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_AV_pschmask::Key &key1, const Table_DeviceTemplate_AV_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_AV_pschmask*> *rows);
	class Row_DeviceTemplate_AV_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_AV_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DeviceTemplate_AV_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_AV_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_AV_pschmask::Key;
		friend class Table_DeviceTemplate_AV_pschmask;
	private:
		Table_DeviceTemplate_AV_pschmask *table;
		
		short int m_FK_DeviceTemplate;
short int m_UsesIR;
short int m_IR_PowerDelay;
short int m_IR_ModeDelay;
short int m_DigitDelay;
short int m_TogglePower;
short int m_ToggleDSP;
short int m_ToggleInput;
short int m_ToggleOutput;
short int m_NumericEntry;
short int m_MonitorInputChanges;
short int m_RepeatIR;
short int m_RepeatVolume;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[19];
	
	public:
		short int FK_DeviceTemplate_get();
short int UsesIR_get();
short int IR_PowerDelay_get();
short int IR_ModeDelay_get();
short int DigitDelay_get();
short int TogglePower_get();
short int ToggleDSP_get();
short int ToggleInput_get();
short int ToggleOutput_get();
short int NumericEntry_get();
short int MonitorInputChanges_get();
short int RepeatIR_get();
short int RepeatVolume_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_DeviceTemplate_set(short int val);
void UsesIR_set(short int val);
void IR_PowerDelay_set(short int val);
void IR_ModeDelay_set(short int val);
void DigitDelay_set(short int val);
void TogglePower_set(short int val);
void ToggleDSP_set(short int val);
void ToggleInput_set(short int val);
void ToggleOutput_set(short int val);
void NumericEntry_set(short int val);
void MonitorInputChanges_set(short int val);
void RepeatIR_set(short int val);
void RepeatVolume_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
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
bool MonitorInputChanges_isNull();
bool RepeatIR_isNull();
bool RepeatVolume_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
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
void MonitorInputChanges_setNull(bool val);
void RepeatIR_setNull(bool val);
void RepeatVolume_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_AV_pschmask(Table_DeviceTemplate_AV_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_AV_pschmask *Table_DeviceTemplate_AV_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_UsesIR+ m_IR_PowerDelay+ m_IR_ModeDelay+ m_DigitDelay+ m_TogglePower+ m_ToggleDSP+ m_ToggleInput+ m_ToggleOutput+ m_NumericEntry+ m_MonitorInputChanges+ m_RepeatIR+ m_RepeatVolume+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
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
string psc_restrict_asSQL();

	};

#endif

