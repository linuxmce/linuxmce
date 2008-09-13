/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CommandGroup_D_Command_pschist_H__
#define __Table_CommandGroup_D_Command_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_D_Command_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CommandGroup_D_Command_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CommandGroup_D_Command_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_CommandGroup_D_Command_pschist();

private:		
	friend class Row_CommandGroup_D_Command_pschist;
	struct Key
	{
		friend class Row_CommandGroup_D_Command_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_CommandGroup_D_Command_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_D_Command_pschist::Key &key1, const Table_CommandGroup_D_Command_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CommandGroup_D_Command_pschist*> *rows);
	class Row_CommandGroup_D_Command_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_D_Command_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_CommandGroup_D_Command_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CommandGroup_D_Command_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_D_Command_pschist::Key;
		friend class Table_CommandGroup_D_Command_pschist;
	private:
		Table_CommandGroup_D_Command_pschist *table;
		
		long int m_PK_CommandGroup_D_Command;
long int m_FK_CommandGroup_D;
long int m_FK_Command;
long int m_FK_DeviceTemplate;
long int m_FK_DeviceCategory;
short int m_BroadcastLevel;
short int m_RelativeToSender;
long int m_OrderNum;
short int m_DeliveryConfirmation;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		long int PK_CommandGroup_D_Command_get();
long int FK_CommandGroup_D_get();
long int FK_Command_get();
long int FK_DeviceTemplate_get();
long int FK_DeviceCategory_get();
short int BroadcastLevel_get();
short int RelativeToSender_get();
long int OrderNum_get();
short int DeliveryConfirmation_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void PK_CommandGroup_D_Command_set(long int val);
void FK_CommandGroup_D_set(long int val);
void FK_Command_set(long int val);
void FK_DeviceTemplate_set(long int val);
void FK_DeviceCategory_set(long int val);
void BroadcastLevel_set(short int val);
void RelativeToSender_set(short int val);
void OrderNum_set(long int val);
void DeliveryConfirmation_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);

		
		bool PK_CommandGroup_D_Command_isNull();
bool FK_CommandGroup_D_isNull();
bool FK_Command_isNull();
bool FK_DeviceTemplate_isNull();
bool FK_DeviceCategory_isNull();
bool BroadcastLevel_isNull();
bool RelativeToSender_isNull();
bool OrderNum_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();

			
		void PK_CommandGroup_D_Command_setNull(bool val);
void FK_CommandGroup_D_setNull(bool val);
void FK_Command_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void FK_DeviceCategory_setNull(bool val);
void BroadcastLevel_setNull(bool val);
void RelativeToSender_setNull(bool val);
void OrderNum_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CommandGroup_D_Command_pschist(Table_CommandGroup_D_Command_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_D_Command_pschist *Table_CommandGroup_D_Command_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandGroup_D* FK_CommandGroup_D_getrow();
class Row_Command* FK_Command_getrow();
class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_DeviceCategory* FK_DeviceCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CommandGroup_D_Command+ m_FK_CommandGroup_D+ m_FK_Command+ m_FK_DeviceTemplate+ m_FK_DeviceCategory+ m_BroadcastLevel+ m_RelativeToSender+ m_OrderNum+ m_DeliveryConfirmation+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_CommandGroup_D_Command_asSQL();
string FK_CommandGroup_D_asSQL();
string FK_Command_asSQL();
string FK_DeviceTemplate_asSQL();
string FK_DeviceCategory_asSQL();
string BroadcastLevel_asSQL();
string RelativeToSender_asSQL();
string OrderNum_asSQL();
string DeliveryConfirmation_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

