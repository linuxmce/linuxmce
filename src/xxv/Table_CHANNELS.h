/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_CHANNELS_H__
#define __Table_CHANNELS_H__

#include "TableRow.h"
#include "Database_xxv.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_CHANNELS.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_CHANNELS : public TableBase , SingleStringKeyBase
{
private:
	Database_xxv *database;
	struct Key;	//forward declaration
	
public:
	Table_CHANNELS(Database_xxv *pDatabase):database(pDatabase)
	{
	};
	~Table_CHANNELS();

private:		
	friend class Row_CHANNELS;
	struct Key
	{
		friend class Row_CHANNELS;
		string pk_Id;

		
		Key(string in_Id);
	
		Key(class Row_CHANNELS *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_CHANNELS::Key &key1, const Table_CHANNELS::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_CHANNELS*> *rows);
	class Row_CHANNELS* AddRow();
	Database_xxv *Database_xxv_get() { return database; }
	
		
	class Row_CHANNELS* GetRow(string in_Id);
	

private:	
	
		
	class Row_CHANNELS* FetchRow(SingleStringKey &key);
		
			
};

class DECLSPECIFIER Row_CHANNELS : public TableRow, public SerializeClass
	{
		friend struct Table_CHANNELS::Key;
		friend class Table_CHANNELS;
	private:
		Table_CHANNELS *table;
		
		string m_Id;
string m_Name;
long int m_Frequency;
string m_Parameters;
string m_Source;
long int m_Srate;
string m_VPID;
string m_APID;
string m_TPID;
string m_CA;
long int m_SID;
long int m_NID;
long int m_TID;
long int m_RID;
long int m_GRP;
long int m_POS;

		bool is_null[16];
	
	public:
		string Id_get();
string Name_get();
long int Frequency_get();
string Parameters_get();
string Source_get();
long int Srate_get();
string VPID_get();
string APID_get();
string TPID_get();
string CA_get();
long int SID_get();
long int NID_get();
long int TID_get();
long int RID_get();
long int GRP_get();
long int POS_get();

		
		void Id_set(string val);
void Name_set(string val);
void Frequency_set(long int val);
void Parameters_set(string val);
void Source_set(string val);
void Srate_set(long int val);
void VPID_set(string val);
void APID_set(string val);
void TPID_set(string val);
void CA_set(string val);
void SID_set(long int val);
void NID_set(long int val);
void TID_set(long int val);
void RID_set(long int val);
void GRP_set(long int val);
void POS_set(long int val);

		
		bool Parameters_isNull();
bool Source_isNull();
bool Srate_isNull();
bool VPID_isNull();
bool APID_isNull();
bool TPID_isNull();
bool CA_isNull();
bool SID_isNull();
bool NID_isNull();
bool TID_isNull();
bool RID_isNull();
bool GRP_isNull();

			
		void Parameters_setNull(bool val);
void Source_setNull(bool val);
void Srate_setNull(bool val);
void VPID_setNull(bool val);
void APID_setNull(bool val);
void TPID_setNull(bool val);
void CA_setNull(bool val);
void SID_setNull(bool val);
void NID_setNull(bool val);
void TID_setNull(bool val);
void RID_setNull(bool val);
void GRP_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_CHANNELS(Table_CHANNELS *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_CHANNELS *Table_CHANNELS_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_Id+ m_Name+ m_Frequency+ m_Parameters+ m_Source+ m_Srate+ m_VPID+ m_APID+ m_TPID+ m_CA+ m_SID+ m_NID+ m_TID+ m_RID+ m_GRP+ m_POS;
		}
	private:
		void SetDefaultValues();
		
		string Id_asSQL();
string Name_asSQL();
string Frequency_asSQL();
string Parameters_asSQL();
string Source_asSQL();
string Srate_asSQL();
string VPID_asSQL();
string APID_asSQL();
string TPID_asSQL();
string CA_asSQL();
string SID_asSQL();
string NID_asSQL();
string TID_asSQL();
string RID_asSQL();
string GRP_asSQL();
string POS_asSQL();

	};

#endif

