/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_MediaType_pschmask_H__
#define __Table_MediaType_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_MediaType_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_MediaType_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_MediaType_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_MediaType_pschmask();

private:		
	friend class Row_MediaType_pschmask;
	struct Key
	{
		friend class Row_MediaType_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_MediaType_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_MediaType_pschmask::Key &key1, const Table_MediaType_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_MediaType_pschmask*> *rows);
	class Row_MediaType_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_MediaType_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_MediaType_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_MediaType_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_MediaType_pschmask::Key;
		friend class Table_MediaType_pschmask;
	private:
		Table_MediaType_pschmask *table;
		
		short int m_PK_MediaType;
short int m_Define;
short int m_Description;
short int m_FK_DesignObj;
short int m_DCEAware;
short int m_Extensions;
short int m_Subdirectory;
short int m_IsExternalTransmission;
short int m_FK_Pipe;
short int m_CanBookmark;
short int m_EK_AttributeType_DefaultSort;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[17];
	
	public:
		short int PK_MediaType_get();
short int Define_get();
short int Description_get();
short int FK_DesignObj_get();
short int DCEAware_get();
short int Extensions_get();
short int Subdirectory_get();
short int IsExternalTransmission_get();
short int FK_Pipe_get();
short int CanBookmark_get();
short int EK_AttributeType_DefaultSort_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_MediaType_set(short int val);
void Define_set(short int val);
void Description_set(short int val);
void FK_DesignObj_set(short int val);
void DCEAware_set(short int val);
void Extensions_set(short int val);
void Subdirectory_set(short int val);
void IsExternalTransmission_set(short int val);
void FK_Pipe_set(short int val);
void CanBookmark_set(short int val);
void EK_AttributeType_DefaultSort_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_MediaType_isNull();
bool Define_isNull();
bool Description_isNull();
bool FK_DesignObj_isNull();
bool DCEAware_isNull();
bool Extensions_isNull();
bool Subdirectory_isNull();
bool IsExternalTransmission_isNull();
bool FK_Pipe_isNull();
bool CanBookmark_isNull();
bool EK_AttributeType_DefaultSort_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_MediaType_setNull(bool val);
void Define_setNull(bool val);
void Description_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void DCEAware_setNull(bool val);
void Extensions_setNull(bool val);
void Subdirectory_setNull(bool val);
void IsExternalTransmission_setNull(bool val);
void FK_Pipe_setNull(bool val);
void CanBookmark_setNull(bool val);
void EK_AttributeType_DefaultSort_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_MediaType_pschmask(Table_MediaType_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_MediaType_pschmask *Table_MediaType_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObj* FK_DesignObj_getrow();
class Row_Pipe* FK_Pipe_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_MediaType+ m_Define+ m_Description+ m_FK_DesignObj+ m_DCEAware+ m_Extensions+ m_Subdirectory+ m_IsExternalTransmission+ m_FK_Pipe+ m_CanBookmark+ m_EK_AttributeType_DefaultSort+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_MediaType_asSQL();
string Define_asSQL();
string Description_asSQL();
string FK_DesignObj_asSQL();
string DCEAware_asSQL();
string Extensions_asSQL();
string Subdirectory_asSQL();
string IsExternalTransmission_asSQL();
string FK_Pipe_asSQL();
string CanBookmark_asSQL();
string EK_AttributeType_DefaultSort_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

