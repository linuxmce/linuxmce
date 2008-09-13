/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Device_pschmask_H__
#define __Table_Device_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Device_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Device_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Device_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Device_pschmask();

private:		
	friend class Row_Device_pschmask;
	struct Key
	{
		friend class Row_Device_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Device_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Device_pschmask::Key &key1, const Table_Device_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Device_pschmask*> *rows);
	class Row_Device_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Device_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Device_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Device_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_Device_pschmask::Key;
		friend class Table_Device_pschmask;
	private:
		Table_Device_pschmask *table;
		
		short int m_PK_Device;
short int m_Disabled;
short int m_FK_Room;
short int m_FK_Installation;
short int m_FK_DesignObj;
short int m_Description;
short int m_FK_DeviceTemplate;
short int m_FK_Device_ControlledVia;
short int m_IPaddress;
short int m_MACaddress;
short int m_IgnoreOnOff;
short int m_FK_Device_RouteTo;
short int m_NeedConfigure;
short int m_State;
short int m_Status;
short int m_PingTest;
short int m_EK_LicensedDevice;
short int m_IsNewDevice;
short int m_ManuallyConfigureEA;
short int m_Registered;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[26];
	
	public:
		short int PK_Device_get();
short int Disabled_get();
short int FK_Room_get();
short int FK_Installation_get();
short int FK_DesignObj_get();
short int Description_get();
short int FK_DeviceTemplate_get();
short int FK_Device_ControlledVia_get();
short int IPaddress_get();
short int MACaddress_get();
short int IgnoreOnOff_get();
short int FK_Device_RouteTo_get();
short int NeedConfigure_get();
short int State_get();
short int Status_get();
short int PingTest_get();
short int EK_LicensedDevice_get();
short int IsNewDevice_get();
short int ManuallyConfigureEA_get();
short int Registered_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Device_set(short int val);
void Disabled_set(short int val);
void FK_Room_set(short int val);
void FK_Installation_set(short int val);
void FK_DesignObj_set(short int val);
void Description_set(short int val);
void FK_DeviceTemplate_set(short int val);
void FK_Device_ControlledVia_set(short int val);
void IPaddress_set(short int val);
void MACaddress_set(short int val);
void IgnoreOnOff_set(short int val);
void FK_Device_RouteTo_set(short int val);
void NeedConfigure_set(short int val);
void State_set(short int val);
void Status_set(short int val);
void PingTest_set(short int val);
void EK_LicensedDevice_set(short int val);
void IsNewDevice_set(short int val);
void ManuallyConfigureEA_set(short int val);
void Registered_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_Device_isNull();
bool Disabled_isNull();
bool FK_Room_isNull();
bool FK_Installation_isNull();
bool FK_DesignObj_isNull();
bool Description_isNull();
bool FK_DeviceTemplate_isNull();
bool FK_Device_ControlledVia_isNull();
bool IPaddress_isNull();
bool MACaddress_isNull();
bool IgnoreOnOff_isNull();
bool FK_Device_RouteTo_isNull();
bool NeedConfigure_isNull();
bool State_isNull();
bool Status_isNull();
bool PingTest_isNull();
bool EK_LicensedDevice_isNull();
bool IsNewDevice_isNull();
bool ManuallyConfigureEA_isNull();
bool Registered_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_Device_setNull(bool val);
void Disabled_setNull(bool val);
void FK_Room_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void Description_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void FK_Device_ControlledVia_setNull(bool val);
void IPaddress_setNull(bool val);
void MACaddress_setNull(bool val);
void IgnoreOnOff_setNull(bool val);
void FK_Device_RouteTo_setNull(bool val);
void NeedConfigure_setNull(bool val);
void State_setNull(bool val);
void Status_setNull(bool val);
void PingTest_setNull(bool val);
void EK_LicensedDevice_setNull(bool val);
void IsNewDevice_setNull(bool val);
void ManuallyConfigureEA_setNull(bool val);
void Registered_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Device_pschmask(Table_Device_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Device_pschmask *Table_Device_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Room* FK_Room_getrow();
class Row_Installation* FK_Installation_getrow();
class Row_DesignObj* FK_DesignObj_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Device* FK_Device_ControlledVia_getrow();
class Row_Device* FK_Device_RouteTo_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Device+ m_Disabled+ m_FK_Room+ m_FK_Installation+ m_FK_DesignObj+ m_Description+ m_FK_DeviceTemplate+ m_FK_Device_ControlledVia+ m_IPaddress+ m_MACaddress+ m_IgnoreOnOff+ m_FK_Device_RouteTo+ m_NeedConfigure+ m_State+ m_Status+ m_PingTest+ m_EK_LicensedDevice+ m_IsNewDevice+ m_ManuallyConfigureEA+ m_Registered+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Device_asSQL();
string Disabled_asSQL();
string FK_Room_asSQL();
string FK_Installation_asSQL();
string FK_DesignObj_asSQL();
string Description_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_Device_ControlledVia_asSQL();
string IPaddress_asSQL();
string MACaddress_asSQL();
string IgnoreOnOff_asSQL();
string FK_Device_RouteTo_asSQL();
string NeedConfigure_asSQL();
string State_asSQL();
string Status_asSQL();
string PingTest_asSQL();
string EK_LicensedDevice_asSQL();
string IsNewDevice_asSQL();
string ManuallyConfigureEA_asSQL();
string Registered_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

