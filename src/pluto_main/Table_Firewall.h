/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#ifndef __Table_Firewall_H__
#define __Table_Firewall_H__

#include "TableRow.h"
#include "Database_pluto_main.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "Define_Firewall.h"
#include "SerializeClass/SerializeClass.h"

// If we declare the maps locally, the compiler will create multiple copies of them
// making the output files enormous.  The solution seems to be to create some predefined
// maps for the standard types of primary keys (single long, double long, etc.) and
// put them in a common base class, which is optionally included as tablebase below

class DECLSPECIFIER TableRow;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Table_Firewall : public TableBase , SingleLongKeyBase
{
private:
	Database_pluto_main *database;
	struct Key;	//forward declaration
	
public:
	Table_Firewall(Database_pluto_main *pDatabase):database(pDatabase)
	{
	};
	~Table_Firewall();

private:		
	friend class Row_Firewall;
	struct Key
	{
		friend class Row_Firewall;
		long int pk_PK_Firewall;

		
		Key(long int in_PK_Firewall);
	
		Key(class Row_Firewall *pRow);
	};
	struct Key_Less
	{			
		bool operator()(const Table_Firewall::Key &key1, const Table_Firewall::Key &key2) const;
	};	

	
	

public:				
	// Normally the framework never deletes any Row_X objects, since the application will
	// likely have pointers to them.  This means that if a Commit fails because a row
	// cannot be committed, all subsequent calls to Commit will also fail unless you fix
	// the rows since they will be re-attempted.  If you set either flag to true, the failed
	// row can be deleted.  Use with caution since your pointers become invalid!
	bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
	bool GetRows(string where_statement,vector<class Row_Firewall*> *rows);
	class Row_Firewall* AddRow();
	Database_pluto_main *Database_pluto_main_get() { return database; }
	
		
	class Row_Firewall* GetRow(long int in_PK_Firewall);
	

private:	
	
		
	class Row_Firewall* FetchRow(SingleLongKey &key);
		
			
};

class DECLSPECIFIER Row_Firewall : public TableRow, public SerializeClass
	{
		friend struct Table_Firewall::Key;
		friend class Table_Firewall;
	private:
		Table_Firewall *table;
		
		long int m_PK_Firewall;
long int m_Place;
string m_Matchname;
string m_IntIF;
string m_ExtIF;
string m_Protocol;
string m_SourcePort;
long int m_SourcePortEnd;
string m_DestinationPort;
string m_SourceIP;
string m_DestinationIP;
string m_RuleType;
long int m_Disabled;
long int m_Offline;
string m_RPolicy;
string m_Description;
long int m_Bantime;
long int m_psc_id;
long int m_psc_batch;
long int m_psc_user;
short int m_psc_frozen;
string m_psc_mod;
long int m_psc_restrict;

		bool is_null[23];
	
	public:
		long int PK_Firewall_get();
long int Place_get();
string Matchname_get();
string IntIF_get();
string ExtIF_get();
string Protocol_get();
string SourcePort_get();
long int SourcePortEnd_get();
string DestinationPort_get();
string SourceIP_get();
string DestinationIP_get();
string RuleType_get();
long int Disabled_get();
long int Offline_get();
string RPolicy_get();
string Description_get();
long int Bantime_get();
long int psc_id_get();
long int psc_batch_get();
long int psc_user_get();
short int psc_frozen_get();
string psc_mod_get();
long int psc_restrict_get();

		
		void PK_Firewall_set(long int val);
void Place_set(long int val);
void Matchname_set(string val);
void IntIF_set(string val);
void ExtIF_set(string val);
void Protocol_set(string val);
void SourcePort_set(string val);
void SourcePortEnd_set(long int val);
void DestinationPort_set(string val);
void SourceIP_set(string val);
void DestinationIP_set(string val);
void RuleType_set(string val);
void Disabled_set(long int val);
void Offline_set(long int val);
void RPolicy_set(string val);
void Description_set(string val);
void Bantime_set(long int val);
void psc_id_set(long int val);
void psc_batch_set(long int val);
void psc_user_set(long int val);
void psc_frozen_set(short int val);
void psc_mod_set(string val);
void psc_restrict_set(long int val);

		
		bool Matchname_isNull();
bool IntIF_isNull();
bool ExtIF_isNull();
bool SourcePort_isNull();
bool Disabled_isNull();
bool Offline_isNull();
bool RPolicy_isNull();
bool Description_isNull();
bool psc_id_isNull();
bool psc_batch_isNull();
bool psc_user_isNull();
bool psc_frozen_isNull();
bool psc_restrict_isNull();

			
		void Matchname_setNull(bool val);
void IntIF_setNull(bool val);
void ExtIF_setNull(bool val);
void SourcePort_setNull(bool val);
void Disabled_setNull(bool val);
void Offline_setNull(bool val);
void RPolicy_setNull(bool val);
void Description_setNull(bool val);
void psc_id_setNull(bool val);
void psc_batch_setNull(bool val);
void psc_user_setNull(bool val);
void psc_frozen_setNull(bool val);
void psc_restrict_setNull(bool val);
	
	
		void Delete();
		void Reload();		
	
		Row_Firewall(Table_Firewall *pTable);
	
		bool IsDeleted(){return is_deleted;};
		bool IsModified(){return is_modified;};			
		class Table_Firewall *Table_Firewall_get() { return table; };

		// Return the rows for foreign keys 
		

		// Return the rows in other tables with foreign keys pointing here
		

		// Setup binary serialization
		void SetupSerialization(int iSC_Version) {
			StartSerializeList() + m_PK_Firewall+ m_Place+ m_Matchname+ m_IntIF+ m_ExtIF+ m_Protocol+ m_SourcePort+ m_SourcePortEnd+ m_DestinationPort+ m_SourceIP+ m_DestinationIP+ m_RuleType+ m_Disabled+ m_Offline+ m_RPolicy+ m_Description+ m_Bantime+ m_psc_id+ m_psc_batch+ m_psc_user+ m_psc_frozen+ m_psc_mod+ m_psc_restrict;
		}
	private:
		void SetDefaultValues();
		
		string PK_Firewall_asSQL();
string Place_asSQL();
string Matchname_asSQL();
string IntIF_asSQL();
string ExtIF_asSQL();
string Protocol_asSQL();
string SourcePort_asSQL();
string SourcePortEnd_asSQL();
string DestinationPort_asSQL();
string SourceIP_asSQL();
string DestinationIP_asSQL();
string RuleType_asSQL();
string Disabled_asSQL();
string Offline_asSQL();
string RPolicy_asSQL();
string Description_asSQL();
string Bantime_asSQL();
string psc_id_asSQL();
string psc_batch_asSQL();
string psc_user_asSQL();
string psc_frozen_asSQL();
string psc_mod_asSQL();
string psc_restrict_asSQL();

	};

#endif

