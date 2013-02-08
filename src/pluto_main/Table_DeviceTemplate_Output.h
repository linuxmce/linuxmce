/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DeviceTemplate_Output_H__
#define __Table_DeviceTemplate_Output_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DeviceTemplate_Output.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DeviceTemplate_Output : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DeviceTemplate_Output(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DeviceTemplate_Output();

private:		
	friend class Row_DeviceTemplate_Output;
	struct Key
	{
		friend class Row_DeviceTemplate_Output;
		long int pk_FK_DeviceTemplate;
long int pk_FK_Command;

		
		Key(long int in_FK_DeviceTemplate, long int in_FK_Command);
	
		Key(class Row_DeviceTemplate_Output *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DeviceTemplate_Output::Key &key1, const Table_DeviceTemplate_Output::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DeviceTemplate_Output*> *rows);
	class Row_DeviceTemplate_Output* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DeviceTemplate_Output* GetRow(long int in_FK_DeviceTemplate, long int in_FK_Command);
	

private:	
	
		
	class Row_DeviceTemplate_Output* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DeviceTemplate_Output : public TableRow, public SerializeClass
	{
		friend struct Table_DeviceTemplate_Output::Key;
		friend class Table_DeviceTemplate_Output;
	private:
		Table_DeviceTemplate_Output *table;
		
		long int m_FK_DeviceTemplate;
long int m_FK_Command;
long int m_FK_ConnectorType;
short int m_OrderNo;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[10];
	
	public:
		long int FK_DeviceTemplate_get();
long int FK_Command_get();
long int FK_ConnectorType_get();
short int OrderNo_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void FK_DeviceTemplate_set(long int val);
void FK_Command_set(long int val);
void FK_ConnectorType_set(long int val);
void OrderNo_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_ConnectorType_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_ConnectorType_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DeviceTemplate_Output(Table_DeviceTemplate_Output *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DeviceTemplate_Output *Table_DeviceTemplate_Output_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();
class Row_Command* FK_Command_getrow();
class Row_ConnectorType* FK_ConnectorType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_FK_DeviceTemplate+ m_FK_Command+ m_FK_ConnectorType+ m_OrderNo+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string FK_DeviceTemplate_asSQL();
string FK_Command_asSQL();
string FK_ConnectorType_asSQL();
string OrderNo_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

