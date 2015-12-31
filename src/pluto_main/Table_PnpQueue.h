/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_PnpQueue_H__
#define __Table_PnpQueue_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PnpQueue.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PnpQueue : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_PnpQueue(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_PnpQueue();

private:		
	friend class Row_PnpQueue;
	struct Key
	{
		friend class Row_PnpQueue;
		long int pk_PK_PnpQueue;

		
		Key(long int in_PK_PnpQueue);
	
		Key(class Row_PnpQueue *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PnpQueue::Key &key1, const Table_PnpQueue::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_PnpQueue*> *rows);
	class Row_PnpQueue* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_PnpQueue* GetRow(long int in_PK_PnpQueue);
	

private:	
	
		
	class Row_PnpQueue* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PnpQueue : public TableRow, public SerializeClass
	{
		friend struct Table_PnpQueue::Key;
		friend class Table_PnpQueue;
	private:
		Table_PnpQueue *table;
		
		long int m_PK_PnpQueue;
string m_DetectedDate;
string m_VendorModelId;
string m_Path;
string m_IPaddress;
string m_MACaddress;
string m_Category;
string m_SerialNumber;
long int m_FK_CommMethod;
long int m_FK_PnpProtocol;
long int m_FK_DeviceTemplate;
long int m_FK_Device_Created;
long int m_FK_Device_Reported;
string m_Signature;
long int m_Removed;
short int m_Stage;
short int m_Processed;
string m_Parms;
string m_Description;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[25];
	
	public:
		long int PK_PnpQueue_get();
string DetectedDate_get();
string VendorModelId_get();
string Path_get();
string IPaddress_get();
string MACaddress_get();
string Category_get();
string SerialNumber_get();
long int FK_CommMethod_get();
long int FK_PnpProtocol_get();
long int FK_DeviceTemplate_get();
long int FK_Device_Created_get();
long int FK_Device_Reported_get();
string Signature_get();
long int Removed_get();
short int Stage_get();
short int Processed_get();
string Parms_get();
string Description_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_PnpQueue_set(long int val);
void DetectedDate_set(string val);
void VendorModelId_set(string val);
void Path_set(string val);
void IPaddress_set(string val);
void MACaddress_set(string val);
void Category_set(string val);
void SerialNumber_set(string val);
void FK_CommMethod_set(long int val);
void FK_PnpProtocol_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_Device_Created_set(long int val);
void FK_Device_Reported_set(long int val);
void Signature_set(string val);
void Removed_set(long int val);
void Stage_set(short int val);
void Processed_set(short int val);
void Parms_set(string val);
void Description_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool DetectedDate_isNull();
bool VendorModelId_isNull();
bool Path_isNull();
bool IPaddress_isNull();
bool MACaddress_isNull();
bool Category_isNull();
bool SerialNumber_isNull();
bool FK_CommMethod_isNull();
bool FK_PnpProtocol_isNull();
bool FK_DeviceTemplate_isNull();
bool FK_Device_Created_isNull();
bool FK_Device_Reported_isNull();
bool Signature_isNull();
bool Removed_isNull();
bool Stage_isNull();
bool Processed_isNull();
bool Parms_isNull();
bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void DetectedDate_setNull(bool val);
void VendorModelId_setNull(bool val);
void Path_setNull(bool val);
void IPaddress_setNull(bool val);
void MACaddress_setNull(bool val);
void Category_setNull(bool val);
void SerialNumber_setNull(bool val);
void FK_CommMethod_setNull(bool val);
void FK_PnpProtocol_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void FK_Device_Created_setNull(bool val);
void FK_Device_Reported_setNull(bool val);
void Signature_setNull(bool val);
void Removed_setNull(bool val);
void Stage_setNull(bool val);
void Processed_setNull(bool val);
void Parms_setNull(bool val);
void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PnpQueue(Table_PnpQueue *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PnpQueue *Table_PnpQueue_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommMethod* FK_CommMethod_getrow();
class Row_PnpProtocol* FK_PnpProtocol_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Device* FK_Device_Created_getrow();
class Row_Device* FK_Device_Reported_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PnpQueue+ m_DetectedDate+ m_VendorModelId+ m_Path+ m_IPaddress+ m_MACaddress+ m_Category+ m_SerialNumber+ m_FK_CommMethod+ m_FK_PnpProtocol+ m_FK_DeviceTemplate+ m_FK_Device_Created+ m_FK_Device_Reported+ m_Signature+ m_Removed+ m_Stage+ m_Processed+ m_Parms+ m_Description+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_PnpQueue_asSQL();
string DetectedDate_asSQL();
string VendorModelId_asSQL();
string Path_asSQL();
string IPaddress_asSQL();
string MACaddress_asSQL();
string Category_asSQL();
string SerialNumber_asSQL();
string FK_CommMethod_asSQL();
string FK_PnpProtocol_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_Device_Created_asSQL();
string FK_Device_Reported_asSQL();
string Signature_asSQL();
string Removed_asSQL();
string Stage_asSQL();
string Processed_asSQL();
string Parms_asSQL();
string Description_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

