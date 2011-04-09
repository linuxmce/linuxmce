/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CommandGroup_Command_H__
#define __Table_CommandGroup_Command_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CommandGroup_Command.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CommandGroup_Command : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_CommandGroup_Command(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_CommandGroup_Command();

private:		
	friend class Row_CommandGroup_Command;
	struct Key
	{
		friend class Row_CommandGroup_Command;
		long int pk_PK_CommandGroup_Command;

		
		Key(long int in_PK_CommandGroup_Command);
	
		Key(class Row_CommandGroup_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CommandGroup_Command::Key &key1, const Table_CommandGroup_Command::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CommandGroup_Command*> *rows);
	class Row_CommandGroup_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_CommandGroup_Command* GetRow(long int in_PK_CommandGroup_Command);
	

private:	
	
		
	class Row_CommandGroup_Command* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_CommandGroup_Command : public TableRow, public SerializeClass
	{
		friend struct Table_CommandGroup_Command::Key;
		friend class Table_CommandGroup_Command;
	private:
		Table_CommandGroup_Command *table;
		
		long int m_PK_CommandGroup_Command;
long int m_FK_CommandGroup;
long int m_FK_Command;
long int m_FK_Device;
long int m_FK_DeviceGroup;
short int m_TurnOff;
long int m_OrderNum;
short int m_DeliveryConfirmation;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[14];
	
	public:
		long int PK_CommandGroup_Command_get();
long int FK_CommandGroup_get();
long int FK_Command_get();
long int FK_Device_get();
long int FK_DeviceGroup_get();
short int TurnOff_get();
long int OrderNum_get();
short int DeliveryConfirmation_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_CommandGroup_Command_set(long int val);
void FK_CommandGroup_set(long int val);
void FK_Command_set(long int val);
void FK_Device_set(long int val);
void FK_DeviceGroup_set(long int val);
void TurnOff_set(short int val);
void OrderNum_set(long int val);
void DeliveryConfirmation_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Device_isNull();
bool FK_DeviceGroup_isNull();
bool OrderNum_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Device_setNull(bool val);
void FK_DeviceGroup_setNull(bool val);
void OrderNum_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CommandGroup_Command(Table_CommandGroup_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CommandGroup_Command *Table_CommandGroup_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandGroup* FK_CommandGroup_getrow();
class Row_Command* FK_Command_getrow();
class Row_Device* FK_Device_getrow();
class Row_DeviceGroup* FK_DeviceGroup_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_CommandParameter_FK_CommandGroup_Command_getrows(vector <class Row_CommandGroup_Command_CommandParameter*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_CommandGroup_Command+ m_FK_CommandGroup+ m_FK_Command+ m_FK_Device+ m_FK_DeviceGroup+ m_TurnOff+ m_OrderNum+ m_DeliveryConfirmation+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_CommandGroup_Command_asSQL();
string FK_CommandGroup_asSQL();
string FK_Command_asSQL();
string FK_Device_asSQL();
string FK_DeviceGroup_asSQL();
string TurnOff_asSQL();
string OrderNum_asSQL();
string DeliveryConfirmation_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

