/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_PhoneNumber_H__
#define __Table_PhoneNumber_H__

#include "TableRow.h"
#include "Database_pluto_telecom.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_PhoneNumber.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_PhoneNumber : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_telecom *database;
	struct Key;	//forward declaration
	
public:
	Table_PhoneNumber(Database_pluto_telecom *pDatabase):database(pDatabase)
	{
	};
	~Table_PhoneNumber();

private:		
	friend class Row_PhoneNumber;
	struct Key
	{
		friend class Row_PhoneNumber;
		long int pk_PK_PhoneNumber;

		
		Key(long int in_PK_PhoneNumber);
	
		Key(class Row_PhoneNumber *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_PhoneNumber::Key &key1, const Table_PhoneNumber::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_PhoneNumber*> *rows);
	class Row_PhoneNumber* AddRow();
	Database_pluto_telecom *Database_pluto_telecom_get() { return database; }
	
		
	class Row_PhoneNumber* GetRow(long int in_PK_PhoneNumber);
	

private:	
	
		
	class Row_PhoneNumber* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_PhoneNumber : public TableRow, public SerializeClass
	{
		friend struct Table_PhoneNumber::Key;
		friend class Table_PhoneNumber;
	private:
		Table_PhoneNumber *table;
		
		long int m_PK_PhoneNumber;
long int m_FK_Contact;
long int m_FK_PhoneType;
string m_CountryCode;
string m_AreaCode;
string m_PhoneNumber;
string m_Extension;
string m_DialAs;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[14];
	
	public:
		long int PK_PhoneNumber_get();
long int FK_Contact_get();
long int FK_PhoneType_get();
string CountryCode_get();
string AreaCode_get();
string PhoneNumber_get();
string Extension_get();
string DialAs_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_PhoneNumber_set(long int val);
void FK_Contact_set(long int val);
void FK_PhoneType_set(long int val);
void CountryCode_set(string val);
void AreaCode_set(string val);
void PhoneNumber_set(string val);
void Extension_set(string val);
void DialAs_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Contact_isNull();
bool FK_PhoneType_isNull();
bool CountryCode_isNull();
bool AreaCode_isNull();
bool PhoneNumber_isNull();
bool Extension_isNull();
bool DialAs_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void FK_Contact_setNull(bool val);
void FK_PhoneType_setNull(bool val);
void CountryCode_setNull(bool val);
void AreaCode_setNull(bool val);
void PhoneNumber_setNull(bool val);
void Extension_setNull(bool val);
void DialAs_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_PhoneNumber(Table_PhoneNumber *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_PhoneNumber *Table_PhoneNumber_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Contact* FK_Contact_getrow();
class Row_PhoneType* FK_PhoneType_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void IncomingLog_FK_PhoneNumber_getrows(vector <class Row_IncomingLog*> *rows);
void OutgoingLog_FK_PhoneNumber_getrows(vector <class Row_OutgoingLog*> *rows);
void PhoneNumber_Users_Sync_FK_PhoneNumber_getrows(vector <class Row_PhoneNumber_Users_Sync*> *rows);
void SpeedDial_FK_PhoneNumber_getrows(vector <class Row_SpeedDial*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_PhoneNumber+ m_FK_Contact+ m_FK_PhoneType+ m_CountryCode+ m_AreaCode+ m_PhoneNumber+ m_Extension+ m_DialAs+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_PhoneNumber_asSQL();
string FK_Contact_asSQL();
string FK_PhoneType_asSQL();
string CountryCode_asSQL();
string AreaCode_asSQL();
string PhoneNumber_asSQL();
string Extension_asSQL();
string DialAs_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

