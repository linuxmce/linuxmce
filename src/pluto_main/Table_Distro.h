/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Distro_H__
#define __Table_Distro_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Distro.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Distro : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Distro(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Distro();

private:		
	friend class Row_Distro;
	struct Key
	{
		friend class Row_Distro;
		long int pk_PK_Distro;

		
		Key(long int in_PK_Distro);
	
		Key(class Row_Distro *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Distro::Key &key1, const Table_Distro::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Distro*> *rows);
	class Row_Distro* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Distro* GetRow(long int in_PK_Distro);
	

private:	
	
		
	class Row_Distro* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Distro : public TableRow, public SerializeClass
	{
		friend struct Table_Distro::Key;
		friend class Table_Distro;
	private:
		Table_Distro *table;
		
		long int m_PK_Distro;
string m_Description;
string m_Define;
long int m_FK_OperatingSystem;
string m_Installer;
string m_KickStartCD;
string m_Binaries;
string m_SourceCode;
short int m_Confirmed;
short int m_Core;
short int m_MediaDirector;
short int m_Orbiter;
string m_InstallerURL;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[19];
	
	public:
		long int PK_Distro_get();
string Description_get();
string Define_get();
long int FK_OperatingSystem_get();
string Installer_get();
string KickStartCD_get();
string Binaries_get();
string SourceCode_get();
short int Confirmed_get();
short int Core_get();
short int MediaDirector_get();
short int Orbiter_get();
string InstallerURL_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Distro_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_OperatingSystem_set(long int val);
void Installer_set(string val);
void KickStartCD_set(string val);
void Binaries_set(string val);
void SourceCode_set(string val);
void Confirmed_set(short int val);
void Core_set(short int val);
void MediaDirector_set(short int val);
void Orbiter_set(short int val);
void InstallerURL_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Define_isNull();
bool Installer_isNull();
bool KickStartCD_isNull();
bool Binaries_isNull();
bool SourceCode_isNull();
bool InstallerURL_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Define_setNull(bool val);
void Installer_setNull(bool val);
void KickStartCD_setNull(bool val);
void Binaries_setNull(bool val);
void SourceCode_setNull(bool val);
void InstallerURL_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Distro(Table_Distro *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Distro *Table_Distro_get() { return table; };

		// Return the rows for foreign keys 
		class Row_OperatingSystem* FK_OperatingSystem_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void DeviceTemplate_FK_Distro_getrows(vector <class Row_DeviceTemplate*> *rows);
void InstallWizard_Distro_FK_Distro_getrows(vector <class Row_InstallWizard_Distro*> *rows);
void Package_Compat_FK_Distro_getrows(vector <class Row_Package_Compat*> *rows);
void Package_Directory_FK_Distro_getrows(vector <class Row_Package_Directory*> *rows);
void Package_Directory_File_FK_Distro_getrows(vector <class Row_Package_Directory_File*> *rows);
void Package_Source_Compat_FK_Distro_getrows(vector <class Row_Package_Source_Compat*> *rows);
void RepositorySource_FK_Distro_getrows(vector <class Row_RepositorySource*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Distro+ m_Description+ m_Define+ m_FK_OperatingSystem+ m_Installer+ m_KickStartCD+ m_Binaries+ m_SourceCode+ m_Confirmed+ m_Core+ m_MediaDirector+ m_Orbiter+ m_InstallerURL+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Distro_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_OperatingSystem_asSQL();
string Installer_asSQL();
string KickStartCD_asSQL();
string Binaries_asSQL();
string SourceCode_asSQL();
string Confirmed_asSQL();
string Core_asSQL();
string MediaDirector_asSQL();
string Orbiter_asSQL();
string InstallerURL_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

