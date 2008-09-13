/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_StartupScript_pschmask_H__
#define __Table_StartupScript_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_StartupScript_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_StartupScript_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_StartupScript_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_StartupScript_pschmask();

private:		
	friend class Row_StartupScript_pschmask;
	struct Key
	{
		friend class Row_StartupScript_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_StartupScript_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_StartupScript_pschmask::Key &key1, const Table_StartupScript_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_StartupScript_pschmask*> *rows);
	class Row_StartupScript_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_StartupScript_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_StartupScript_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_StartupScript_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_StartupScript_pschmask::Key;
		friend class Table_StartupScript_pschmask;
	private:
		Table_StartupScript_pschmask *table;
		
		short int m_PK_StartupScript;
short int m_When;
short int m_Command;
short int m_Description;
short int m_ConfigureOnly;
short int m_Parameter_Syntax;
short int m_Core_Boot_Order;
short int m_Core_Background;
short int m_Core_Enabled;
short int m_Core_Parameter;
short int m_MD_Boot_Order;
short int m_MD_Background;
short int m_MD_Enabled;
short int m_MD_Parameter;
short int m_Hybrid_Boot_Order;
short int m_Hybrid_Background;
short int m_Hybrid_Enabled;
short int m_Hybrid_Parameter;
short int m_FK_DeviceTemplate;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[25];
	
	public:
		short int PK_StartupScript_get();
short int When_get();
short int Command_get();
short int Description_get();
short int ConfigureOnly_get();
short int Parameter_Syntax_get();
short int Core_Boot_Order_get();
short int Core_Background_get();
short int Core_Enabled_get();
short int Core_Parameter_get();
short int MD_Boot_Order_get();
short int MD_Background_get();
short int MD_Enabled_get();
short int MD_Parameter_get();
short int Hybrid_Boot_Order_get();
short int Hybrid_Background_get();
short int Hybrid_Enabled_get();
short int Hybrid_Parameter_get();
short int FK_DeviceTemplate_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_StartupScript_set(short int val);
void When_set(short int val);
void Command_set(short int val);
void Description_set(short int val);
void ConfigureOnly_set(short int val);
void Parameter_Syntax_set(short int val);
void Core_Boot_Order_set(short int val);
void Core_Background_set(short int val);
void Core_Enabled_set(short int val);
void Core_Parameter_set(short int val);
void MD_Boot_Order_set(short int val);
void MD_Background_set(short int val);
void MD_Enabled_set(short int val);
void MD_Parameter_set(short int val);
void Hybrid_Boot_Order_set(short int val);
void Hybrid_Background_set(short int val);
void Hybrid_Enabled_set(short int val);
void Hybrid_Parameter_set(short int val);
void FK_DeviceTemplate_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_StartupScript_isNull();
bool When_isNull();
bool Command_isNull();
bool Description_isNull();
bool ConfigureOnly_isNull();
bool Parameter_Syntax_isNull();
bool Core_Boot_Order_isNull();
bool Core_Background_isNull();
bool Core_Enabled_isNull();
bool Core_Parameter_isNull();
bool MD_Boot_Order_isNull();
bool MD_Background_isNull();
bool MD_Enabled_isNull();
bool MD_Parameter_isNull();
bool Hybrid_Boot_Order_isNull();
bool Hybrid_Background_isNull();
bool Hybrid_Enabled_isNull();
bool Hybrid_Parameter_isNull();
bool FK_DeviceTemplate_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_StartupScript_setNull(bool val);
void When_setNull(bool val);
void Command_setNull(bool val);
void Description_setNull(bool val);
void ConfigureOnly_setNull(bool val);
void Parameter_Syntax_setNull(bool val);
void Core_Boot_Order_setNull(bool val);
void Core_Background_setNull(bool val);
void Core_Enabled_setNull(bool val);
void Core_Parameter_setNull(bool val);
void MD_Boot_Order_setNull(bool val);
void MD_Background_setNull(bool val);
void MD_Enabled_setNull(bool val);
void MD_Parameter_setNull(bool val);
void Hybrid_Boot_Order_setNull(bool val);
void Hybrid_Background_setNull(bool val);
void Hybrid_Enabled_setNull(bool val);
void Hybrid_Parameter_setNull(bool val);
void FK_DeviceTemplate_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_StartupScript_pschmask(Table_StartupScript_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_StartupScript_pschmask *Table_StartupScript_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DeviceTemplate* FK_DeviceTemplate_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_StartupScript+ m_When+ m_Command+ m_Description+ m_ConfigureOnly+ m_Parameter_Syntax+ m_Core_Boot_Order+ m_Core_Background+ m_Core_Enabled+ m_Core_Parameter+ m_MD_Boot_Order+ m_MD_Background+ m_MD_Enabled+ m_MD_Parameter+ m_Hybrid_Boot_Order+ m_Hybrid_Background+ m_Hybrid_Enabled+ m_Hybrid_Parameter+ m_FK_DeviceTemplate+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_StartupScript_asSQL();
string When_asSQL();
string Command_asSQL();
string Description_asSQL();
string ConfigureOnly_asSQL();
string Parameter_Syntax_asSQL();
string Core_Boot_Order_asSQL();
string Core_Background_asSQL();
string Core_Enabled_asSQL();
string Core_Parameter_asSQL();
string MD_Boot_Order_asSQL();
string MD_Background_asSQL();
string MD_Enabled_asSQL();
string MD_Parameter_asSQL();
string Hybrid_Boot_Order_asSQL();
string Hybrid_Background_asSQL();
string Hybrid_Enabled_asSQL();
string Hybrid_Parameter_asSQL();
string FK_DeviceTemplate_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

