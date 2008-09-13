/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DesignObjVariation_pschmask_H__
#define __Table_DesignObjVariation_pschmask_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DesignObjVariation_pschmask.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DesignObjVariation_pschmask : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_DesignObjVariation_pschmask(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_DesignObjVariation_pschmask();

private:		
	friend class Row_DesignObjVariation_pschmask;
	struct Key
	{
		friend class Row_DesignObjVariation_pschmask;
		long int pk_psc_id;
long int pk_psc_batch;

		
		Key(long int in_psc_id, long int in_psc_batch);
	
		Key(class Row_DesignObjVariation_pschmask *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DesignObjVariation_pschmask::Key &key1, const Table_DesignObjVariation_pschmask::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DesignObjVariation_pschmask*> *rows);
	class Row_DesignObjVariation_pschmask* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_DesignObjVariation_pschmask* GetRow(long int in_psc_id, long int in_psc_batch);
	

private:	
	
		
	class Row_DesignObjVariation_pschmask* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DesignObjVariation_pschmask : public TableRow, public SerializeClass
	{
		friend struct Table_DesignObjVariation_pschmask::Key;
		friend class Table_DesignObjVariation_pschmask;
	private:
		Table_DesignObjVariation_pschmask *table;
		
		short int m_PK_DesignObjVariation;
short int m_FK_DesignObj;
short int m_FK_UI;
short int m_FK_DesignObj_Goto;
short int m_FK_CommandGroup_D_OnActivate;
short int m_FK_CommandGroup_D_OnLoad;
short int m_FK_CommandGroup_D_OnUnload;
short int m_FK_CommandGroup_D_OnTimeout;
short int m_FK_CommandGroup_D_OnStartup;
short int m_FK_CommandGroup_D_OnHighlight;
short int m_FK_CommandGroup_D_OnUnhighlight;
short int m_FK_EffectType_Selected_WithChange;
short int m_FK_EffectType_Selected_NoChange;
short int m_FK_EffectType_Highlighted;
short int m_FK_Button;
short int m_FK_Criteria_Orbiter;
short int m_DontResetSelectedState;
short int m_FK_StabilityStatus;
short int m_RepeatMS;
short int m_RepeatParm;
short int m_PreserveTransparencies;
long int m_psc_id;
long int m_psc_batch;
short int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[27];
	
	public:
		short int PK_DesignObjVariation_get();
short int FK_DesignObj_get();
short int FK_UI_get();
short int FK_DesignObj_Goto_get();
short int FK_CommandGroup_D_OnActivate_get();
short int FK_CommandGroup_D_OnLoad_get();
short int FK_CommandGroup_D_OnUnload_get();
short int FK_CommandGroup_D_OnTimeout_get();
short int FK_CommandGroup_D_OnStartup_get();
short int FK_CommandGroup_D_OnHighlight_get();
short int FK_CommandGroup_D_OnUnhighlight_get();
short int FK_EffectType_Selected_WithChange_get();
short int FK_EffectType_Selected_NoChange_get();
short int FK_EffectType_Highlighted_get();
short int FK_Button_get();
short int FK_Criteria_Orbiter_get();
short int DontResetSelectedState_get();
short int FK_StabilityStatus_get();
short int RepeatMS_get();
short int RepeatParm_get();
short int PreserveTransparencies_get();
long int psc_id_get();
long int psc_batch_get();
short int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_DesignObjVariation_set(short int val);
void FK_DesignObj_set(short int val);
void FK_UI_set(short int val);
void FK_DesignObj_Goto_set(short int val);
void FK_CommandGroup_D_OnActivate_set(short int val);
void FK_CommandGroup_D_OnLoad_set(short int val);
void FK_CommandGroup_D_OnUnload_set(short int val);
void FK_CommandGroup_D_OnTimeout_set(short int val);
void FK_CommandGroup_D_OnStartup_set(short int val);
void FK_CommandGroup_D_OnHighlight_set(short int val);
void FK_CommandGroup_D_OnUnhighlight_set(short int val);
void FK_EffectType_Selected_WithChange_set(short int val);
void FK_EffectType_Selected_NoChange_set(short int val);
void FK_EffectType_Highlighted_set(short int val);
void FK_Button_set(short int val);
void FK_Criteria_Orbiter_set(short int val);
void DontResetSelectedState_set(short int val);
void FK_StabilityStatus_set(short int val);
void RepeatMS_set(short int val);
void RepeatParm_set(short int val);
void PreserveTransparencies_set(short int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(short int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool PK_DesignObjVariation_isNull();
bool FK_DesignObj_isNull();
bool FK_UI_isNull();
bool FK_DesignObj_Goto_isNull();
bool FK_CommandGroup_D_OnActivate_isNull();
bool FK_CommandGroup_D_OnLoad_isNull();
bool FK_CommandGroup_D_OnUnload_isNull();
bool FK_CommandGroup_D_OnTimeout_isNull();
bool FK_CommandGroup_D_OnStartup_isNull();
bool FK_CommandGroup_D_OnHighlight_isNull();
bool FK_CommandGroup_D_OnUnhighlight_isNull();
bool FK_EffectType_Selected_WithChange_isNull();
bool FK_EffectType_Selected_NoChange_isNull();
bool FK_EffectType_Highlighted_isNull();
bool FK_Button_isNull();
bool FK_Criteria_Orbiter_isNull();
bool DontResetSelectedState_isNull();
bool FK_StabilityStatus_isNull();
bool RepeatMS_isNull();
bool RepeatParm_isNull();
bool PreserveTransparencies_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void PK_DesignObjVariation_setNull(bool val);
void FK_DesignObj_setNull(bool val);
void FK_UI_setNull(bool val);
void FK_DesignObj_Goto_setNull(bool val);
void FK_CommandGroup_D_OnActivate_setNull(bool val);
void FK_CommandGroup_D_OnLoad_setNull(bool val);
void FK_CommandGroup_D_OnUnload_setNull(bool val);
void FK_CommandGroup_D_OnTimeout_setNull(bool val);
void FK_CommandGroup_D_OnStartup_setNull(bool val);
void FK_CommandGroup_D_OnHighlight_setNull(bool val);
void FK_CommandGroup_D_OnUnhighlight_setNull(bool val);
void FK_EffectType_Selected_WithChange_setNull(bool val);
void FK_EffectType_Selected_NoChange_setNull(bool val);
void FK_EffectType_Highlighted_setNull(bool val);
void FK_Button_setNull(bool val);
void FK_Criteria_Orbiter_setNull(bool val);
void DontResetSelectedState_setNull(bool val);
void FK_StabilityStatus_setNull(bool val);
void RepeatMS_setNull(bool val);
void RepeatParm_setNull(bool val);
void PreserveTransparencies_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DesignObjVariation_pschmask(Table_DesignObjVariation_pschmask *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DesignObjVariation_pschmask *Table_DesignObjVariation_pschmask_get() { return table; };

		// Return the rows for foreign keys 
		class Row_DesignObj* FK_DesignObj_getrow();
class Row_UI* FK_UI_getrow();
class Row_DesignObj* FK_DesignObj_Goto_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnActivate_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnLoad_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnUnload_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnTimeout_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnStartup_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnHighlight_getrow();
class Row_CommandGroup_D* FK_CommandGroup_D_OnUnhighlight_getrow();
class Row_EffectType* FK_EffectType_Highlighted_getrow();
class Row_Button* FK_Button_getrow();
class Row_Criteria* FK_Criteria_Orbiter_getrow();
class Row_StabilityStatus* FK_StabilityStatus_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_DesignObjVariation+ m_FK_DesignObj+ m_FK_UI+ m_FK_DesignObj_Goto+ m_FK_CommandGroup_D_OnActivate+ m_FK_CommandGroup_D_OnLoad+ m_FK_CommandGroup_D_OnUnload+ m_FK_CommandGroup_D_OnTimeout+ m_FK_CommandGroup_D_OnStartup+ m_FK_CommandGroup_D_OnHighlight+ m_FK_CommandGroup_D_OnUnhighlight+ m_FK_EffectType_Selected_WithChange+ m_FK_EffectType_Selected_NoChange+ m_FK_EffectType_Highlighted+ m_FK_Button+ m_FK_Criteria_Orbiter+ m_DontResetSelectedState+ m_FK_StabilityStatus+ m_RepeatMS+ m_RepeatParm+ m_PreserveTransparencies+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_DesignObjVariation_asSQL();
string FK_DesignObj_asSQL();
string FK_UI_asSQL();
string FK_DesignObj_Goto_asSQL();
string FK_CommandGroup_D_OnActivate_asSQL();
string FK_CommandGroup_D_OnLoad_asSQL();
string FK_CommandGroup_D_OnUnload_asSQL();
string FK_CommandGroup_D_OnTimeout_asSQL();
string FK_CommandGroup_D_OnStartup_asSQL();
string FK_CommandGroup_D_OnHighlight_asSQL();
string FK_CommandGroup_D_OnUnhighlight_asSQL();
string FK_EffectType_Selected_WithChange_asSQL();
string FK_EffectType_Selected_NoChange_asSQL();
string FK_EffectType_Highlighted_asSQL();
string FK_Button_asSQL();
string FK_Criteria_Orbiter_asSQL();
string DontResetSelectedState_asSQL();
string FK_StabilityStatus_asSQL();
string RepeatMS_asSQL();
string RepeatParm_asSQL();
string PreserveTransparencies_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

