/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Version_H__
#define __Table_Version_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Version.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Version : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Version(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Version();

private:		
	friend class Row_Version;
	struct Key
	{
		friend class Row_Version;
		long int pk_PK_Version;

		
		Key(long int in_PK_Version);
	
		Key(class Row_Version *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Version::Key &key1, const Table_Version::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Version*> *rows);
	class Row_Version* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Version* GetRow(long int in_PK_Version);
	

private:	
	
		
	class Row_Version* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Version : public TableRow, public SerializeClass
	{
		friend struct Table_Version::Key;
		friend class Table_Version;
	private:
		Table_Version *table;
		
		long int m_PK_Version;
string m_VersionName;
string m_BuildName;
string m_Date;
string m_Description;
short int m_Repository;
string m_Comments;
string m_NextSteps;
long int m_SvnRevision;
string m_SvnBranch;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[16];
	
	public:
		long int PK_Version_get();
string VersionName_get();
string BuildName_get();
string Date_get();
string Description_get();
short int Repository_get();
string Comments_get();
string NextSteps_get();
long int SvnRevision_get();
string SvnBranch_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Version_set(long int val);
void VersionName_set(string val);
void BuildName_set(string val);
void Date_set(string val);
void Description_set(string val);
void Repository_set(short int val);
void Comments_set(string val);
void NextSteps_set(string val);
void SvnRevision_set(long int val);
void SvnBranch_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Date_isNull();
bool Description_isNull();
bool Repository_isNull();
bool Comments_isNull();
bool NextSteps_isNull();
bool SvnRevision_isNull();
bool SvnBranch_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Date_setNull(bool val);
void Description_setNull(bool val);
void Repository_setNull(bool val);
void Comments_setNull(bool val);
void NextSteps_setNull(bool val);
void SvnRevision_setNull(bool val);
void SvnBranch_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Version(Table_Version *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Version *Table_Version_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		void Installation_FK_Version_getrows(vector <class Row_Installation*> *rows);
void Schema_FK_Version_getrows(vector <class Row_Schema*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Version+ m_VersionName+ m_BuildName+ m_Date+ m_Description+ m_Repository+ m_Comments+ m_NextSteps+ m_SvnRevision+ m_SvnBranch+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Version_asSQL();
string VersionName_asSQL();
string BuildName_asSQL();
string Date_asSQL();
string Description_asSQL();
string Repository_asSQL();
string Comments_asSQL();
string NextSteps_asSQL();
string SvnRevision_asSQL();
string SvnBranch_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

