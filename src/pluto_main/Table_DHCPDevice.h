/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DHCPDevice_H__
#define __Table_DHCPDevice_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DHCPDevice.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DHCPDevice : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DHCPDevice(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DHCPDevice();

private:		
	friend class Row_DHCPDevice;
	struct Key
	{
		friend class Row_DHCPDevice;
		long int pk_PK_DHCPDevice;

		
		Key(long int in_PK_DHCPDevice);
	
		Key(class Row_DHCPDevice *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DHCPDevice::Key &key1, const Table_DHCPDevice::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DHCPDevice*> *rows);
	class Row_DHCPDevice* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DHCPDevice* GetRow(long int in_PK_DHCPDevice);
	

private:	
	
		
	class Row_DHCPDevice* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DHCPDevice : public TableRow, public SerializeClass
	{
		friend struct Table_DHCPDevice::Key;
		friend class Table_DHCPDevice;
	private:
		Table_DHCPDevice *table;
		
		long int m_PK_DHCPDevice;
long int m_FK_DeviceTemplate;
unsigned long long m_Mac_Range_Low;
unsigned long long m_Mac_Range_High;
string m_VendorModelID;
string m_SerialNumber;
string m_Parms;
string m_Category;
long int m_FK_PnpProtocol;
string m_PnpDetectionScript;
long int m_FK_DeviceCategory;
long int m_FK_Manufacturer;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[19];
	
	public:
		long int PK_DHCPDevice_get();
long int FK_DeviceTemplate_get();
unsigned long long Mac_Range_Low_get();
unsigned long long Mac_Range_High_get();
string VendorModelID_get();
string SerialNumber_get();
string Parms_get();
string Category_get();
long int FK_PnpProtocol_get();
string PnpDetectionScript_get();
long int FK_DeviceCategory_get();
long int FK_Manufacturer_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DHCPDevice_set(long int val);
void FK_DeviceTemplate_set(long int val);
void Mac_Range_Low_set(unsigned long long val);
void Mac_Range_High_set(unsigned long long val);
void VendorModelID_set(string val);
void SerialNumber_set(string val);
void Parms_set(string val);
void Category_set(string val);
void FK_PnpProtocol_set(long int val);
void PnpDetectionScript_set(string val);
void FK_DeviceCategory_set(long int val);
void FK_Manufacturer_set(long int val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_DeviceTemplate_isNull();
bool VendorModelID_isNull();
bool SerialNumber_isNull();
bool Parms_isNull();
bool Category_isNull();
bool FK_PnpProtocol_isNull();
bool PnpDetectionScript_isNull();
bool FK_DeviceCategory_isNull();
bool FK_Manufacturer_isNull();
bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_DeviceTemplate_setNull(bool val);
void VendorModelID_setNull(bool val);
void SerialNumber_setNull(bool val);
void Parms_setNull(bool val);
void Category_setNull(bool val);
void FK_PnpProtocol_setNull(bool val);
void PnpDetectionScript_setNull(bool val);
void FK_DeviceCategory_setNull(bool val);
void FK_Manufacturer_setNull(bool val);
void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DHCPDevice(Table_DHCPDevice *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DHCPDevice *Table_DHCPDevice_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_PnpProtocol* FK_PnpProtocol_getrow();
class Row_DeviceCategory* FK_DeviceCategory_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DHCPDevice_DeviceData_FK_DHCPDevice_getrows(vector <class Row_DHCPDevice_DeviceData*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DHCPDevice+ m_FK_DeviceTemplate+ m_Mac_Range_Low+ m_Mac_Range_High+ m_VendorModelID+ m_SerialNumber+ m_Parms+ m_Category+ m_FK_PnpProtocol+ m_PnpDetectionScript+ m_FK_DeviceCategory+ m_FK_Manufacturer+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DHCPDevice_asSQL();
string FK_DeviceTemplate_asSQL();
string Mac_Range_Low_asSQL();
string Mac_Range_High_asSQL();
string VendorModelID_asSQL();
string SerialNumber_asSQL();
string Parms_asSQL();
string Category_asSQL();
string FK_PnpProtocol_asSQL();
string PnpDetectionScript_asSQL();
string FK_DeviceCategory_asSQL();
string FK_Manufacturer_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

