/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_DiscLocation_H__
#define __Table_DiscLocation_H__

#include "TableRow.h"
#include "Database_pluto_media.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_DiscLocation.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_DiscLocation : public TableBase , DoubleLongKeyBase
{
private:
	Database_pluto_media *database;
	struct Key;	//forward declaration
	
public:
	Table_DiscLocation(Database_pluto_media *pDatabase):database(pDatabase)
	{
	};
	~Table_DiscLocation();

private:		
	friend class Row_DiscLocation;
	struct Key
	{
		friend class Row_DiscLocation;
		long int pk_EK_Device;
long int pk_Slot;

		
		Key(long int in_EK_Device, long int in_Slot);
	
		Key(class Row_DiscLocation *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_DiscLocation::Key &key1, const Table_DiscLocation::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_DiscLocation*> *rows);
	class Row_DiscLocation* AddRow();
	Database_pluto_media *Database_pluto_media_get() { return database; }
	
		
	class Row_DiscLocation* GetRow(long int in_EK_Device, long int in_Slot);
	

private:	
	
		
	class Row_DiscLocation* FetchRow(DoubleLongKey &key);
		
			
};

class DECLSPECIFIER Row_DiscLocation : public TableRow, public SerializeClass
	{
		friend struct Table_DiscLocation::Key;
		friend class Table_DiscLocation;
	private:
		Table_DiscLocation *table;
		
		long int m_EK_Device;
long int m_Slot;
long int m_FK_Disc;
string m_Type;
long int m_EK_Device_Ripping;
long int m_RipJob;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[12];
	
	public:
		long int EK_Device_get();
long int Slot_get();
long int FK_Disc_get();
string Type_get();
long int EK_Device_Ripping_get();
long int RipJob_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void EK_Device_set(long int val);
void Slot_set(long int val);
void FK_Disc_set(long int val);
void Type_set(string val);
void EK_Device_Ripping_set(long int val);
void RipJob_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool FK_Disc_isNull();
bool Type_isNull();
bool EK_Device_Ripping_isNull();
bool RipJob_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_mod_isNull();
bool psc_restrict_isNull();

			
		void FK_Disc_setNull(bool val);
void Type_setNull(bool val);
void EK_Device_Ripping_setNull(bool val);
void RipJob_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_mod_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_DiscLocation(Table_DiscLocation *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_DiscLocation *Table_DiscLocation_get() { return table; };

		// Return the rows for foreign keys 
		class Row_Disc* FK_Disc_getrow();


		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_EK_Device+ m_Slot+ m_FK_Disc+ m_Type+ m_EK_Device_Ripping+ m_RipJob+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string EK_Device_asSQL();
string Slot_asSQL();
string FK_Disc_asSQL();
string Type_asSQL();
string EK_Device_Ripping_asSQL();
string RipJob_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

