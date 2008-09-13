/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_EventHandler_pschmask_H__
#define __Table_EventHandler_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_EventHandler_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_EventHandler_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_EventHandler_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_EventHandler_pschmask();

private:		
	friend class Row_EventHandler_pschmask;
	struct Key
	{
		friend class Row_EventHandler_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_EventHandler_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_EventHandler_pschmask::Key &key1, const Table_EventHandler_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_EventHandler_pschmask*> *rows);
	class Row_EventHandler_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_EventHandler_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_EventHandler_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_EventHandler_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_EventHandler_pschmask::Key;
		friend class Table_EventHandler_pschmask;
	private:
		Table_EventHandler_pschmask *table;
		
		short int m_PK_EventHandler;
short int m_FK_Event;
short int m_TimedEvent;
short int m_Description;
short int m_FK_Criteria;
short int m_FK_Installation;
short int m_FK_CommandGroup;
short int m_UserCreated;
short int m_FK_CannedEvents;
short int m_Disabled;
short int m_FK_Template;
short int m_TemplateParm1;
short int m_TemplateParm2;
short int m_OncePerSeconds;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[20];
	
	public:
		short int PK_EventHandler_get();
short int FK_Event_get();
short int TimedEvent_get();
short int Description_get();
short int FK_Criteria_get();
short int FK_Installation_get();
short int FK_CommandGroup_get();
short int UserCreated_get();
short int FK_CannedEvents_get();
short int Disabled_get();
short int FK_Template_get();
short int TemplateParm1_get();
short int TemplateParm2_get();
short int OncePerSeconds_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_EventHandler_set(short int val);
void FK_Event_set(short int val);
void TimedEvent_set(short int val);
void Description_set(short int val);
void FK_Criteria_set(short int val);
void FK_Installation_set(short int val);
void FK_CommandGroup_set(short int val);
void UserCreated_set(short int val);
void FK_CannedEvents_set(short int val);
void Disabled_set(short int val);
void FK_Template_set(short int val);
void TemplateParm1_set(short int val);
void TemplateParm2_set(short int val);
void OncePerSeconds_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_EventHandler_isNull();
bool FK_Event_isNull();
bool TimedEvent_isNull();
bool Description_isNull();
bool FK_Criteria_isNull();
bool FK_Installation_isNull();
bool FK_CommandGroup_isNull();
bool UserCreated_isNull();
bool FK_CannedEvents_isNull();
bool Disabled_isNull();
bool FK_Template_isNull();
bool TemplateParm1_isNull();
bool TemplateParm2_isNull();
bool OncePerSeconds_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_EventHandler_setNull(bool val);
void FK_Event_setNull(bool val);
void TimedEvent_setNull(bool val);
void Description_setNull(bool val);
void FK_Criteria_setNull(bool val);
void FK_Installation_setNull(bool val);
void FK_CommandGroup_setNull(bool val);
void UserCreated_setNull(bool val);
void FK_CannedEvents_setNull(bool val);
void Disabled_setNull(bool val);
void FK_Template_setNull(bool val);
void TemplateParm1_setNull(bool val);
void TemplateParm2_setNull(bool val);
void OncePerSeconds_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_EventHandler_pschmask(Table_EventHandler_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_EventHandler_pschmask *Table_EventHandler_pschmask_get() { return table; };

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

