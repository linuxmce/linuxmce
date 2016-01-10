/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Contact_H__
#define __Table_Contact_H__

#include "TableRow.h"
#include "Database_pluto_telecom.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Contact.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Contact : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_telecom *database;
	struct Key;	//forward declaration
	
public:
	Table_Contact(Database_pluto_telecom *pDatabase):database(pDatabase)
	{
	};
	~Table_Contact();

private:		
	friend class Row_Contact;
	struct Key
	{
		friend class Row_Contact;
		long int pk_PK_Contact;

		
		Key(long int in_PK_Contact);
	
		Key(class Row_Contact *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Contact::Key &key1, const Table_Contact::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Contact*> *rows);
	class Row_Contact* AddRow();
	Database_pluto_telecom *Database_pluto_telecom_get() { return database; }
	
		
	class Row_Contact* GetRow(long int in_PK_Contact);
	

private:	
	
		
	class Row_Contact* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Contact : public TableRow, public SerializeClass
	{
		friend struct Table_Contact::Key;
		friend class Table_Contact;
	private:
		Table_Contact *table;
		
		long int m_PK_Contact;
string m_Name;
string m_Company;
string m_JobDescription;
string m_Title;
string m_Email;
string m_EntryID;
long int m_EK_Users;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[14];
	
	public:
		long int PK_Contact_get();
string Name_get();
string Company_get();
string JobDescription_get();
string Title_get();
string Email_get();
string EntryID_get();
long int EK_Users_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Contact_set(long int val);
void Name_set(string val);
void Company_set(string val);
void JobDescription_set(string val);
void Title_set(string val);
void Email_set(string val);
void EntryID_set(string val);
void EK_Users_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Name_isNull();
bool Company_isNull();
bool JobDescription_isNull();
bool Title_isNull();
bool Email_isNull();
bool EntryID_isNull();
bool EK_Users_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Name_setNull(bool val);
void Company_setNull(bool val);
void JobDescription_setNull(bool val);
void Title_setNull(bool val);
void Email_setNull(bool val);
void EntryID_setNull(bool val);
void EK_Users_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Contact(Table_Contact *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Contact *Table_Contact_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Contact_Users_Sync_FK_Contact_getrows(vector <class Row_Contact_Users_Sync*> *rows);
void PhoneNumber_FK_Contact_getrows(vector <class Row_PhoneNumber*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Contact+ m_Name+ m_Company+ m_JobDescription+ m_Title+ m_Email+ m_EntryID+ m_EK_Users+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Contact_asSQL();
string Name_asSQL();
string Company_asSQL();
string JobDescription_asSQL();
string Title_asSQL();
string Email_asSQL();
string EntryID_asSQL();
string EK_Users_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

