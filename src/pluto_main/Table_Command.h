/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Command_H__
#define __Table_Command_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Command.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Command : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Command(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Command();

private:		
	friend class Row_Command;
	struct Key
	{
		friend class Row_Command;
		long int pk_PK_Command;

		
		Key(long int in_PK_Command);
	
		Key(class Row_Command *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Command::Key &key1, const Table_Command::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Command*> *rows);
	class Row_Command* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Command* GetRow(long int in_PK_Command);
	

private:	
	
		
	class Row_Command* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Command : public TableRow, public SerializeClass
	{
		friend struct Table_Command::Key;
		friend class Table_Command;
	private:
		Table_Command *table;
		
		long int m_PK_Command;
string m_Description;
string m_Define;
long int m_FK_CommandCategory;
short int m_AVCommand;
string m_Comments;
short int m_Log;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[13];
	
	public:
		long int PK_Command_get();
string Description_get();
string Define_get();
long int FK_CommandCategory_get();
short int AVCommand_get();
string Comments_get();
short int Log_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Command_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_CommandCategory_set(long int val);
void AVCommand_set(short int val);
void Comments_set(string val);
void Log_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Define_isNull();
bool Comments_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Define_setNull(bool val);
void Comments_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Command(Table_Command *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Command *Table_Command_get() { return table; };

		// Return the rows for foreign keys 
		class Row_CommandCategory* FK_CommandCategory_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void CommandGroup_Command_FK_Command_getrows(vector <class Row_CommandGroup_Command*> *rows);
void CommandGroup_D_Command_FK_Command_getrows(vector <class Row_CommandGroup_D_Command*> *rows);
void Command_CommandParameter_FK_Command_getrows(vector <class Row_Command_CommandParameter*> *rows);
void Command_Pipe_FK_Command_getrows(vector <class Row_Command_Pipe*> *rows);
void DeviceCommandGroup_Command_FK_Command_getrows(vector <class Row_DeviceCommandGroup_Command*> *rows);
void DeviceTemplate_DSPMode_FK_Command_getrows(vector <class Row_DeviceTemplate_DSPMode*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Command_Input_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceCategory_ControlledVia_Pipe_FK_Command_Output_getrows(vector <class Row_DeviceTemplate_DeviceCategory_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Command_Input_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);
void DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_FK_Command_Output_getrows(vector <class Row_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe*> *rows);
void DeviceTemplate_Input_FK_Command_getrows(vector <class Row_DeviceTemplate_Input*> *rows);
void DeviceTemplate_Output_FK_Command_getrows(vector <class Row_DeviceTemplate_Output*> *rows);
void Device_Command_FK_Command_getrows(vector <class Row_Device_Command*> *rows);
void Device_Device_Pipe_FK_Command_Input_getrows(vector <class Row_Device_Device_Pipe*> *rows);
void Device_Device_Pipe_FK_Command_Output_getrows(vector <class Row_Device_Device_Pipe*> *rows);
void InfraredGroup_Command_FK_Command_getrows(vector <class Row_InfraredGroup_Command*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Command+ m_Description+ m_Define+ m_FK_CommandCategory+ m_AVCommand+ m_Comments+ m_Log+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Command_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_CommandCategory_asSQL();
string AVCommand_asSQL();
string Comments_asSQL();
string Log_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

