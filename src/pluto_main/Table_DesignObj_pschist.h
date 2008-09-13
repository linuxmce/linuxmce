/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DesignObj_pschist_H__
#define __Table_DesignObj_pschist_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObj_pschist.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DesignObj_pschist : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DesignObj_pschist(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DesignObj_pschist();

private:		
	friend class Row_DesignObj_pschist;
	struct Key
	{
		friend class Row_DesignObj_pschist;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DesignObj_pschist *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObj_pschist::Key &key1, const Table_DesignObj_pschist::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DesignObj_pschist*> *rows);
	class Row_DesignObj_pschist* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObj_pschist* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DesignObj_pschist* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DesignObj_pschist : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObj_pschist::Key;
		friend class Table_DesignObj_pschist;
	private:
		Table_DesignObj_pschist *table;
		
		long int m_PK_DesignObj;
string m_Description;
string m_Define;
long int m_FK_DesignObjType;
long int m_FK_DesignObjCategory;
long int m_FK_DesignObj_IncludeIfOtherIncluded;
short int m_Priority;
short int m_KeepGraphicInCache;
short int m_AlwaysInclude;
short int m_CantGoBack;
short int m_CommandsProcessedAtServer;
short int m_TimeoutSeconds;
short int m_Animate;
long int m_FK_StabilityStatus;
long int m_FK_Document;
short int m_CanGoBackToSameScreen;
string m_ScreenType;
long int m_FK_DesignObj_SubstForSkin;
long int m_FK_Skin_SubstForSkin;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
short int m_psc_toc;
long int m_psc_restrict;

		bool is_null[26];
	
	public:
		long int PK_DesignObj_get();
string Description_get();
string Define_get();
long int FK_DesignObjType_get();
long int FK_DesignObjCategory_get();
long int FK_DesignObj_IncludeIfOtherIncluded_get();
short int Priority_get();
short int KeepGraphicInCache_get();
short int AlwaysInclude_get();
short int CantGoBack_get();
short int CommandsProcessedAtServer_get();
short int TimeoutSeconds_get();
short int Animate_get();
long int FK_StabilityStatus_get();
long int FK_Document_get();
short int CanGoBackToSameScreen_get();
string ScreenType_get();
long int FK_DesignObj_SubstForSkin_get();
long int FK_Skin_SubstForSkin_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
short int psc_toc_get();
long int psc_restrict_get();

		
		void PK_DesignObj_set(long int val);
void Description_set(string val);
void Define_set(string val);
void FK_DesignObjType_set(long int val);
void FK_DesignObjCategory_set(long int val);
void FK_DesignObj_IncludeIfOtherIncluded_set(long int val);
void Priority_set(short int val);
void KeepGraphicInCache_set(short int val);
void AlwaysInclude_set(short int val);
void CantGoBack_set(short int val);
void CommandsProcessedAtServer_set(short int val);
void TimeoutSeconds_set(short int val);
void Animate_set(short int val);
void FK_StabilityStatus_set(long int val);
void FK_Document_set(long int val);
void CanGoBackToSameScreen_set(short int val);
void ScreenType_set(string val);
void FK_DesignObj_SubstForSkin_set(long int val);
void FK_Skin_SubstForSkin_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_toc_set(short int val);
void psc_restrict_set(long int val);

		
		bool PK_DesignObj_isNull();
bool Description_isNull();
bool Define_isNull();
bool FK_DesignObjType_isNull();
bool FK_DesignObjCategory_isNull();
bool FK_DesignObj_IncludeIfOtherIncluded_isNull();
bool Priority_isNull();
bool CantGoBack_isNull();
bool CommandsProcessedAtServer_isNull();
bool TimeoutSeconds_isNull();
bool Animate_isNull();
bool FK_StabilityStatus_isNull();
bool FK_Document_isNull();
bool ScreenType_isNull();
bool FK_DesignObj_SubstForSkin_isNull();
bool FK_Skin_SubstForSkin_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_toc_isNull();
bool psc_restrict_isNull();

			
		void PK_DesignObj_setNull(bool val);
void Description_setNull(bool val);
void Define_setNull(bool val);
void FK_DesignObjType_setNull(bool val);
void FK_DesignObjCategory_setNull(bool val);
void FK_DesignObj_IncludeIfOtherIncluded_setNull(bool val);
void Priority_setNull(bool val);
void CantGoBack_setNull(bool val);
void CommandsProcessedAtServer_setNull(bool val);
void TimeoutSeconds_setNull(bool val);
void Animate_setNull(bool val);
void FK_StabilityStatus_setNull(bool val);
void FK_Document_setNull(bool val);
void ScreenType_setNull(bool val);
void FK_DesignObj_SubstForSkin_setNull(bool val);
void FK_Skin_SubstForSkin_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_toc_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObj_pschist(Table_DesignObj_pschist *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObj_pschist *Table_DesignObj_pschist_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObjType* FK_DesignObjType_getrow();
class Row_DesignObjCategory* FK_DesignObjCategory_getrow();
class Row_DesignObj* FK_DesignObj_IncludeIfOtherIncluded_getrow();
class Row_StabilityStatus* FK_StabilityStatus_getrow();
class Row_Document* FK_Document_getrow();
class Row_DesignObj* FK_DesignObj_SubstForSkin_getrow();
class Row_Skin* FK_Skin_SubstForSkin_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObj+ m_Description+ m_Define+ m_FK_DesignObjType+ m_FK_DesignObjCategory+ m_FK_DesignObj_IncludeIfOtherIncluded+ m_Priority+ m_KeepGraphicInCache+ m_AlwaysInclude+ m_CantGoBack+ m_CommandsProcessedAtServer+ m_TimeoutSeconds+ m_Animate+ m_FK_StabilityStatus+ m_FK_Document+ m_CanGoBackToSameScreen+ m_ScreenType+ m_FK_DesignObj_SubstForSkin+ m_FK_Skin_SubstForSkin+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_toc+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObj_asSQL();
string Description_asSQL();
string Define_asSQL();
string FK_DesignObjType_asSQL();
string FK_DesignObjCategory_asSQL();
string FK_DesignObj_IncludeIfOtherIncluded_asSQL();
string Priority_asSQL();
string KeepGraphicInCache_asSQL();
string AlwaysInclude_asSQL();
string CantGoBack_asSQL();
string CommandsProcessedAtServer_asSQL();
string TimeoutSeconds_asSQL();
string Animate_asSQL();
string FK_StabilityStatus_asSQL();
string FK_Document_asSQL();
string CanGoBackToSameScreen_asSQL();
string ScreenType_asSQL();
string FK_DesignObj_SubstForSkin_asSQL();
string FK_Skin_SubstForSkin_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_toc_asSQL();
string psc_restrict_asSQL();

	};

#endif

