/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Software_Source_H__
#define __Table_Software_Source_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Software_Source.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Software_Source : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Software_Source(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Software_Source();

private:		
	friend class Row_Software_Source;
	struct Key
	{
		friend class Row_Software_Source;
		long int pk_PK_Software_Source;

		
		Key(long int in_PK_Software_Source);
	
		Key(class Row_Software_Source *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Software_Source::Key &key1, const Table_Software_Source::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Software_Source*> *rows);
	class Row_Software_Source* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Software_Source* GetRow(long int in_PK_Software_Source);
	

private:	
	
		
	class Row_Software_Source* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Software_Source : public TableRow, public SerializeClass
	{
		friend struct Table_Software_Source::Key;
		friend class Table_Software_Source;
	private:
		Table_Software_Source *table;
		
		long int m_PK_Software_Source;
long int m_FK_Software;
string m_Downloadurl;
string m_Version;
string m_Distro;
string m_Required_Version_Min;
string m_Required_Version_Max;
string m_RepositoryName;
string m_Iconstr;
string m_Title;
string m_Description;
string m_HomeURL;
string m_Category;
string m_Misc;
string m_Target;
string m_Importance;
float m_Rating;
short int m_Virus_Free;
string m_PC_Type;
string m_Sum_md5;
string m_Sum_sha;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[27];
	
	public:
		long int PK_Software_Source_get();
long int FK_Software_get();
string Downloadurl_get();
string Version_get();
string Distro_get();
string Required_Version_Min_get();
string Required_Version_Max_get();
string RepositoryName_get();
string Iconstr_get();
string Title_get();
string Description_get();
string HomeURL_get();
string Category_get();
string Misc_get();
string Target_get();
string Importance_get();
float Rating_get();
short int Virus_Free_get();
string PC_Type_get();
string Sum_md5_get();
string Sum_sha_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Software_Source_set(long int val);
void FK_Software_set(long int val);
void Downloadurl_set(string val);
void Version_set(string val);
void Distro_set(string val);
void Required_Version_Min_set(string val);
void Required_Version_Max_set(string val);
void RepositoryName_set(string val);
void Iconstr_set(string val);
void Title_set(string val);
void Description_set(string val);
void HomeURL_set(string val);
void Category_set(string val);
void Misc_set(string val);
void Target_set(string val);
void Importance_set(string val);
void Rating_set(float val);
void Virus_Free_set(short int val);
void PC_Type_set(string val);
void Sum_md5_set(string val);
void Sum_sha_set(string val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Downloadurl_isNull();
bool Version_isNull();
bool Distro_isNull();
bool Required_Version_Min_isNull();
bool Required_Version_Max_isNull();
bool RepositoryName_isNull();
bool Iconstr_isNull();
bool Title_isNull();
bool Description_isNull();
bool HomeURL_isNull();
bool Category_isNull();
bool Misc_isNull();
bool Target_isNull();
bool Importance_isNull();
bool PC_Type_isNull();
bool Sum_md5_isNull();
bool Sum_sha_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Downloadurl_setNull(bool val);
void Version_setNull(bool val);
void Distro_setNull(bool val);
void Required_Version_Min_setNull(bool val);
void Required_Version_Max_setNull(bool val);
void RepositoryName_setNull(bool val);
void Iconstr_setNull(bool val);
void Title_setNull(bool val);
void Description_setNull(bool val);
void HomeURL_setNull(bool val);
void Category_setNull(bool val);
void Misc_setNull(bool val);
void Target_setNull(bool val);
void Importance_setNull(bool val);
void PC_Type_setNull(bool val);
void Sum_md5_setNull(bool val);
void Sum_sha_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Software_Source(Table_Software_Source *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Software_Source *Table_Software_Source_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Software* FK_Software_getrow();


		// Return the rows in other tables with foreign keys pointing here
		void Software_Device_FK_Software_Source_getrows(vector <class Row_Software_Device*> *rows);


		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Software_Source+ m_FK_Software+ m_Downloadurl+ m_Version+ m_Distro+ m_Required_Version_Min+ m_Required_Version_Max+ m_RepositoryName+ m_Iconstr+ m_Title+ m_Description+ m_HomeURL+ m_Category+ m_Misc+ m_Target+ m_Importance+ m_Rating+ m_Virus_Free+ m_PC_Type+ m_Sum_md5+ m_Sum_sha+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Software_Source_asSQL();
string FK_Software_asSQL();
string Downloadurl_asSQL();
string Version_asSQL();
string Distro_asSQL();
string Required_Version_Min_asSQL();
string Required_Version_Max_asSQL();
string RepositoryName_asSQL();
string Iconstr_asSQL();
string Title_asSQL();
string Description_asSQL();
string HomeURL_asSQL();
string Category_asSQL();
string Misc_asSQL();
string Target_asSQL();
string Importance_asSQL();
string Rating_asSQL();
string Virus_Free_asSQL();
string PC_Type_asSQL();
string Sum_md5_asSQL();
string Sum_sha_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

