/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Users_H__
#define __Table_Users_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Users.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Users : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Users(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Users();

private:		
	friend class Row_Users;
	struct Key
	{
		friend class Row_Users;
		long int pk_PK_Users;

		
		Key(long int in_PK_Users);
	
		Key(class Row_Users *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Users::Key &key1, const Table_Users::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Users*> *rows);
	class Row_Users* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Users* GetRow(long int in_PK_Users);
	

private:	
	
		
	class Row_Users* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Users : public TableRow, public SerializeClass
	{
		friend struct Table_Users::Key;
		friend class Table_Users;
	private:
		Table_Users *table;
		
		long int m_PK_Users;
string m_UserName;
string m_Password;
string m_PINCode;
short int m_HasMailbox;
short int m_AccessGeneralMailbox;
long int m_Extension;
string m_FirstName;
string m_LastName;
string m_Nickname;
long int m_ExtensionRingTimeout;
string m_ForwardEmail;
short int m_FK_Language;
long int m_FK_Installation_Main;
short int m_HideFromOrbiter;
short int m_Staff;
string m_Password_Unix;
string m_Password_Samba;
long int m_FK_UserMode;
long int m_EK_Dealer;
long int m_EK_Manufacturer;
short int m_RequirePinToSelect;
long int m_Use_VPN;
string m_VPN_Ip;
string m_Connect_to;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[31];
	
	public:
		long int PK_Users_get();
string UserName_get();
string Password_get();
string PINCode_get();
short int HasMailbox_get();
short int AccessGeneralMailbox_get();
long int Extension_get();
string FirstName_get();
string LastName_get();
string Nickname_get();
long int ExtensionRingTimeout_get();
string ForwardEmail_get();
short int FK_Language_get();
long int FK_Installation_Main_get();
short int HideFromOrbiter_get();
short int Staff_get();
string Password_Unix_get();
string Password_Samba_get();
long int FK_UserMode_get();
long int EK_Dealer_get();
long int EK_Manufacturer_get();
short int RequirePinToSelect_get();
long int Use_VPN_get();
string VPN_Ip_get();
string Connect_to_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Users_set(long int val);
void UserName_set(string val);
void Password_set(string val);
void PINCode_set(string val);
void HasMailbox_set(short int val);
void AccessGeneralMailbox_set(short int val);
void Extension_set(long int val);
void FirstName_set(string val);
void LastName_set(string val);
void Nickname_set(string val);
void ExtensionRingTimeout_set(long int val);
void ForwardEmail_set(string val);
void FK_Language_set(short int val);
void FK_Installation_Main_set(long int val);
void HideFromOrbiter_set(short int val);
void Staff_set(short int val);
void Password_Unix_set(string val);
void Password_Samba_set(string val);
void FK_UserMode_set(long int val);
void EK_Dealer_set(long int val);
void EK_Manufacturer_set(long int val);
void RequirePinToSelect_set(short int val);
void Use_VPN_set(long int val);
void VPN_Ip_set(string val);
void Connect_to_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Extension_isNull();
bool FirstName_isNull();
bool LastName_isNull();
bool Nickname_isNull();
bool ForwardEmail_isNull();
bool FK_Language_isNull();
bool FK_Installation_Main_isNull();
bool Staff_isNull();
bool Password_Unix_isNull();
bool Password_Samba_isNull();
bool EK_Dealer_isNull();
bool EK_Manufacturer_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Extension_setNull(bool val);
void FirstName_setNull(bool val);
void LastName_setNull(bool val);
void Nickname_setNull(bool val);
void ForwardEmail_setNull(bool val);
void FK_Language_setNull(bool val);
void FK_Installation_Main_setNull(bool val);
void Staff_setNull(bool val);
void Password_Unix_setNull(bool val);
void Password_Samba_setNull(bool val);
void EK_Dealer_setNull(bool val);
void EK_Manufacturer_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Users(Table_Users *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Users *Table_Users_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Language* FK_Language_getrow();
class Row_Installation* FK_Installation_Main_getrow();
class Row_UserMode* FK_UserMode_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_Users_Maintainer_getrows(vector <class Row_DeviceTemplate*> *rows);
void Device_Users_FK_Users_getrows(vector <class Row_Device_Users*> *rows);
void Document_Comment_FK_Users_getrows(vector <class Row_Document_Comment*> *rows);
void Installation_Users_FK_Users_getrows(vector <class Row_Installation_Users*> *rows);
void Orbiter_Users_PasswordReq_FK_Users_getrows(vector <class Row_Orbiter_Users_PasswordReq*> *rows);
void Package_Users_FK_Users_getrows(vector <class Row_Package_Users*> *rows);
void Room_Users_FK_Users_getrows(vector <class Row_Room_Users*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Users+ m_UserName+ m_Password+ m_PINCode+ m_HasMailbox+ m_AccessGeneralMailbox+ m_Extension+ m_FirstName+ m_LastName+ m_Nickname+ m_ExtensionRingTimeout+ m_ForwardEmail+ m_FK_Language+ m_FK_Installation_Main+ m_HideFromOrbiter+ m_Staff+ m_Password_Unix+ m_Password_Samba+ m_FK_UserMode+ m_EK_Dealer+ m_EK_Manufacturer+ m_RequirePinToSelect+ m_Use_VPN+ m_VPN_Ip+ m_Connect_to+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Users_asSQL();
string UserName_asSQL();
string Password_asSQL();
string PINCode_asSQL();
string HasMailbox_asSQL();
string AccessGeneralMailbox_asSQL();
string Extension_asSQL();
string FirstName_asSQL();
string LastName_asSQL();
string Nickname_asSQL();
string ExtensionRingTimeout_asSQL();
string ForwardEmail_asSQL();
string FK_Language_asSQL();
string FK_Installation_Main_asSQL();
string HideFromOrbiter_asSQL();
string Staff_asSQL();
string Password_Unix_asSQL();
string Password_Samba_asSQL();
string FK_UserMode_asSQL();
string EK_Dealer_asSQL();
string EK_Manufacturer_asSQL();
string RequirePinToSelect_asSQL();
string Use_VPN_asSQL();
string VPN_Ip_asSQL();
string Connect_to_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

