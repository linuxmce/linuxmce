/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Notification_H__
#define __Table_Notification_H__

#include "TableRow.h"
#include "Database_pluto_security.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Notification.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Notification : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_security *database;
	struct Key;	//forward declaration
	
public:
	Table_Notification(Database_pluto_security *pDatabase):database(pDatabase)
	{
	};
	~Table_Notification();

private:		
	friend class Row_Notification;
	struct Key
	{
		friend class Row_Notification;
		long int pk_PK_Notification;

		
		Key(long int in_PK_Notification);
	
		Key(class Row_Notification *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Notification::Key &key1, const Table_Notification::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Notification*> *rows);
	class Row_Notification* AddRow();
	Database_pluto_security *Database_pluto_security_get() { return database; }
	
		
	class Row_Notification* GetRow(long int in_PK_Notification);
	

private:	
	
		
	class Row_Notification* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Notification : public TableRow, public SerializeClass
	{
		friend struct Table_Notification::Key;
		friend class Table_Notification;
	private:
		Table_Notification *table;
		
		long int m_PK_Notification;
long int m_FK_Alert;
string m_NotificationTime;
string m_Info;
string m_Result;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[11];
	
	public:
		long int PK_Notification_get();
long int FK_Alert_get();
string NotificationTime_get();
string Info_get();
string Result_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Notification_set(long int val);
void FK_Alert_set(long int val);
void NotificationTime_set(string val);
void Info_set(string val);
void Result_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Info_isNull();
bool Result_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void Info_setNull(bool val);
void Result_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Notification(Table_Notification *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Notification *Table_Notification_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Alert* FK_Alert_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Notification+ m_FK_Alert+ m_NotificationTime+ m_Info+ m_Result+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Notification_asSQL();
string FK_Alert_asSQL();
string NotificationTime_asSQL();
string Info_asSQL();
string Result_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

