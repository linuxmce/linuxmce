/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Package_pschist_H__
#define __Table_Package_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Package_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Package_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Package_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Package_pschist();

private:		
	friend class Row_Package_pschist;
	struct Key
	{
		friend class Row_Package_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_Package_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Package_pschist::Key &key1, const Table_Package_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Package_pschist*> *rows);
	class Row_Package_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Package_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_Package_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Package_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_Package_pschist::Key;
		friend class Table_Package_pschist;
	private:
		Table_Package_pschist *table;
		
		long int m_PK_Package;
string m_Description;
long int m_FK_PackageType;
long int m_FK_Package_Sourcecode;
short int m_IsSource;
short int m_NonExecutable;
string m_HomePage;
long int m_FK_License;
long int m_FK_Manufacturer;
string m_HelpURL;
string m_HelpURL_UsersManual;
string m_HelpURL_ProgrammersGuide;
string m_Comments;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;
long int m_FK_Document;
long int m_FK_Document_ProgrammersGuide;
long int m_FK_Document_UsersManual;

		bool is_null[23];
	
	public:
		long int PK_Package_get();
string Description_get();
long int FK_PackageType_get();
long int FK_Package_Sourcecode_get();
short int IsSource_get();
short int NonExecutable_get();
string HomePage_get();
long int FK_License_get();
long int FK_Manufacturer_get();
string HelpURL_get();
string HelpURL_UsersManual_get();
string HelpURL_ProgrammersGuide_get();
string Comments_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();
long int FK_Document_get();
long int FK_Document_ProgrammersGuide_get();
long int FK_Document_UsersManual_get();

		
		void PK_Package_set(long int val);
void Description_set(string val);
void FK_PackageType_set(long int val);
void FK_Package_Sourcecode_set(long int val);
void IsSource_set(short int val);
void NonExecutable_set(short int val);
void HomePage_set(string val);
void FK_License_set(long int val);
void FK_Manufacturer_set(long int val);
void HelpURL_set(string val);
void HelpURL_UsersManual_set(string val);
void HelpURL_ProgrammersGuide_set(string val);
void Comments_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);
void FK_Document_set(long int val);
void FK_Document_ProgrammersGuide_set(long int val);
void FK_Document_UsersManual_set(long int val);

		
		bool PK_Package_isNull();
bool Description_isNull();
bool FK_PackageType_isNull();
bool FK_Package_Sourcecode_isNull();
bool IsSource_isNull();
bool NonExecutable_isNull();
bool HomePage_isNull();
bool FK_License_isNull();
bool FK_Manufacturer_isNull();
bool HelpURL_isNull();
bool HelpURL_UsersManual_isNull();
bool HelpURL_ProgrammersGuide_isNull();
bool Comments_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();
bool FK_Document_isNull();
bool FK_Document_ProgrammersGuide_isNull();
bool FK_Document_UsersManual_isNull();

			
		void PK_Package_setNull(bool val);
void Description_setNull(bool val);
void FK_PackageType_setNull(bool val);
void FK_Package_Sourcecode_setNull(bool val);
void IsSource_setNull(bool val);
void NonExecutable_setNull(bool val);
void HomePage_setNull(bool val);
void FK_License_setNull(bool val);
void FK_Manufacturer_setNull(bool val);
void HelpURL_setNull(bool val);
void HelpURL_UsersManual_setNull(bool val);
void HelpURL_ProgrammersGuide_setNull(bool val);
void Comments_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
void FK_Document_setNull(bool val);
void FK_Document_ProgrammersGuide_setNull(bool val);
void FK_Document_UsersManual_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Package_pschist(Table_Package_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Package_pschist *Table_Package_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_PackageType* FK_PackageType_getrow();
class Row_Package* FK_Package_Sourcecode_getrow();
class Row_License* FK_License_getrow();
class Row_Manufacturer* FK_Manufacturer_getrow();
class Row_Document* FK_Document_getrow();
class Row_Document* FK_Document_ProgrammersGuide_getrow();
class Row_Document* FK_Document_UsersManual_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Package+ m_Description+ m_FK_PackageType+ m_FK_Package_Sourcecode+ m_IsSource+ m_NonExecutable+ m_HomePage+ m_FK_License+ m_FK_Manufacturer+ m_HelpURL+ m_HelpURL_UsersManual+ m_HelpURL_ProgrammersGuide+ m_Comments+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict+ m_FK_Document+ m_FK_Document_ProgrammersGuide+ m_FK_Document_UsersManual;
		}
	private:
		void SetDefaultValues();
		
		string PK_Package_asSQL();
string Description_asSQL();
string FK_PackageType_asSQL();
string FK_Package_Sourcecode_asSQL();
string IsSource_asSQL();
string NonExecutable_asSQL();
string HomePage_asSQL();
string FK_License_asSQL();
string FK_Manufacturer_asSQL();
string HelpURL_asSQL();
string HelpURL_UsersManual_asSQL();
string HelpURL_ProgrammersGuide_asSQL();
string Comments_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();
string FK_Document_asSQL();
string FK_Document_ProgrammersGuide_asSQL();
string FK_Document_UsersManual_asSQL();

	};

#endif

