/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_EventHandler_H__
#define __Table_EventHandler_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EventHandler.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_EventHandler : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_EventHandler(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_EventHandler();

private:		
	friend class Row_EventHandler;
	struct Key
	{
		friend class Row_EventHandler;
		long int pk_PK_EventHandler;

		
		Key(long int in_PK_EventHandler);
	
		Key(class Row_EventHandler *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EventHandler::Key &key1, const Table_EventHandler::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_EventHandler*> *rows);
	class Row_EventHandler* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventHandler* GetRow(long int in_PK_EventHandler);
	

private:	
	
		
	class Row_EventHandler* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_EventHandler : public TableRow, public SerializeClass
	{
		friend struct Table_EventHandler::Key;
		friend class Table_EventHandler;
	private:
		Table_EventHandler *table;
		
		long int m_PK_EventHandler;
long int m_FK_Event;
long int m_TimedEvent;
string m_Description;
long int m_FK_Criteria;
long int m_FK_Installation;
long int m_FK_CommandGroup;
short int m_UserCreated;
long int m_FK_CannedEvents;
short int m_Disabled;
long int m_FK_Template;
long int m_TemplateParm1;
long int m_TemplateParm2;
long int m_OncePerSeconds;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[20];
	
	public:
		long int PK_EventHandler_get();
long int FK_Event_get();
long int TimedEvent_get();
string Description_get();
long int FK_Criteria_get();
long int FK_Installation_get();
long int FK_CommandGroup_get();
short int UserCreated_get();
long int FK_CannedEvents_get();
short int Disabled_get();
long int FK_Template_get();
long int TemplateParm1_get();
long int TemplateParm2_get();
long int OncePerSeconds_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_EventHandler_set(long int val);
void FK_Event_set(long int val);
void TimedEvent_set(long int val);
void Description_set(string val);
void FK_Criteria_set(long int val);
void FK_Installation_set(long int val);
void FK_CommandGroup_set(long int val);
void UserCreated_set(short int val);
void FK_CannedEvents_set(long int val);
void Disabled_set(short int val);
void FK_Template_set(long int val);
void TemplateParm1_set(long int val);
void TemplateParm2_set(long int val);
void OncePerSeconds_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Event_isNull();
bool TimedEvent_isNull();
bool FK_Criteria_isNull();
bool FK_Installation_isNull();
bool FK_CommandGroup_isNull();
bool FK_CannedEvents_isNull();
bool Disabled_isNull();
bool FK_Template_isNull();
bool TemplateParm1_isNull();
bool TemplateParm2_isNull();
bool OncePerSeconds_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Event_setNull(bool val);
void TimedEvent_setNull(bool val);
void FK_Criteria_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_CommandGroup_setNull(bool val);
void FK_CannedEvents_setNull(bool val);
void Disabled_setNull(bool val);
void FK_Template_setNull(bool val);
void TemplateParm1_setNull(bool val);
void TemplateParm2_setNull(bool val);
void OncePerSeconds_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EventHandler(Table_EventHandler *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EventHandler *Table_EventHandler_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Event* FK_Event_getrow();
class Row_Criteria* FK_Criteria_getrow();
class Row_Installation* FK_Installation_getrow();
class Row_CommandGroup* FK_CommandGroup_getrow();
class Row_CannedEvents* FK_CannedEvents_getrow();
class Row_Template* FK_Template_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_EventHandler+ m_FK_Event+ m_TimedEvent+ m_Description+ m_FK_Criteria+ m_FK_Installation+ m_FK_CommandGroup+ m_UserCreated+ m_FK_CannedEvents+ m_Disabled+ m_FK_Template+ m_TemplateParm1+ m_TemplateParm2+ m_OncePerSeconds+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_EventHandler_asSQL();
string FK_Event_asSQL();
string TimedEvent_asSQL();
string Description_asSQL();
string FK_Criteria_asSQL();
string FK_Installation_asSQL();
string FK_CommandGroup_asSQL();
string UserCreated_asSQL();
string FK_CannedEvents_asSQL();
string Disabled_asSQL();
string FK_Template_asSQL();
string TemplateParm1_asSQL();
string TemplateParm2_asSQL();
string OncePerSeconds_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

