/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_ProviderSource_H__
#define __Table_ProviderSource_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_ProviderSource.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_ProviderSource : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_ProviderSource(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_ProviderSource();

private:		
	friend class Row_ProviderSource;
	struct Key
	{
		friend class Row_ProviderSource;
		long int pk_PK_ProviderSource;

		
		Key(long int in_PK_ProviderSource);
	
		Key(class Row_ProviderSource *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_ProviderSource::Key &key1, const Table_ProviderSource::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_ProviderSource*> *rows);
	class Row_ProviderSource* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_ProviderSource* GetRow(long int in_PK_ProviderSource);
	

private:	
	
		
	class Row_ProviderSource* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_ProviderSource : public TableRow, public SerializeClass
	{
		friend struct Table_ProviderSource::Key;
		friend class Table_ProviderSource;
	private:
		Table_ProviderSource *table;
		
		long int m_PK_ProviderSource;
string m_Description;
string m_Comments;
short int m_UserNamePassword;
long int m_EK_Country;
long int m_EK_MediaType;
string m_ProviderOrganization;
long int m_EK_Package;
string m_FillCommandLine;
short int m_FillHours;
string m_ProviderCommandLine;
string m_DeviceCommandLine;
string m_PackageCommandLine;
string m_LineupCommandLine;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[20];
	
	public:
		long int PK_ProviderSource_get();
string Description_get();
string Comments_get();
short int UserNamePassword_get();
long int EK_Country_get();
long int EK_MediaType_get();
string ProviderOrganization_get();
long int EK_Package_get();
string FillCommandLine_get();
short int FillHours_get();
string ProviderCommandLine_get();
string DeviceCommandLine_get();
string PackageCommandLine_get();
string LineupCommandLine_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_ProviderSource_set(long int val);
void Description_set(string val);
void Comments_set(string val);
void UserNamePassword_set(short int val);
void EK_Country_set(long int val);
void EK_MediaType_set(long int val);
void ProviderOrganization_set(string val);
void EK_Package_set(long int val);
void FillCommandLine_set(string val);
void FillHours_set(short int val);
void ProviderCommandLine_set(string val);
void DeviceCommandLine_set(string val);
void PackageCommandLine_set(string val);
void LineupCommandLine_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Description_isNull();
bool Comments_isNull();
bool EK_Country_isNull();
bool EK_MediaType_isNull();
bool ProviderOrganization_isNull();
bool EK_Package_isNull();
bool FillCommandLine_isNull();
bool FillHours_isNull();
bool ProviderCommandLine_isNull();
bool DeviceCommandLine_isNull();
bool PackageCommandLine_isNull();
bool LineupCommandLine_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void Description_setNull(bool val);
void Comments_setNull(bool val);
void EK_Country_setNull(bool val);
void EK_MediaType_setNull(bool val);
void ProviderOrganization_setNull(bool val);
void EK_Package_setNull(bool val);
void FillCommandLine_setNull(bool val);
void FillHours_setNull(bool val);
void ProviderCommandLine_setNull(bool val);
void DeviceCommandLine_setNull(bool val);
void PackageCommandLine_setNull(bool val);
void LineupCommandLine_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_ProviderSource(Table_ProviderSource *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_ProviderSource *Table_ProviderSource_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void MediaProvider_FK_ProviderSource_getrows(vector <class Row_MediaProvider*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_ProviderSource+ m_Description+ m_Comments+ m_UserNamePassword+ m_EK_Country+ m_EK_MediaType+ m_ProviderOrganization+ m_EK_Package+ m_FillCommandLine+ m_FillHours+ m_ProviderCommandLine+ m_DeviceCommandLine+ m_PackageCommandLine+ m_LineupCommandLine+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_ProviderSource_asSQL();
string Description_asSQL();
string Comments_asSQL();
string UserNamePassword_asSQL();
string EK_Country_asSQL();
string EK_MediaType_asSQL();
string ProviderOrganization_asSQL();
string EK_Package_asSQL();
string FillCommandLine_asSQL();
string FillHours_asSQL();
string ProviderCommandLine_asSQL();
string DeviceCommandLine_asSQL();
string PackageCommandLine_asSQL();
string LineupCommandLine_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

