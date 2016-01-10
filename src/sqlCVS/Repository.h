/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *
 * @file Repository.h
 * @brief header file for the Repository class
 * @author
 *
 */
 
#ifndef Repository_h
#define Repository_h

#include "mysql.h"
#include <iostream>
#include "Table.h"
#include "ChangedRow.h"

class R_ApproveBatch;
class RA_Request;

namespace sqlCVS
{
	/**
	 * @brief class modelling the repository
	 * @todo complete documentation
	 */
	 
	class Repository
	{
		friend class Database;
		
		/** @brief All our system tables */		
		class Table *m_pTable_Setting;
		class Table *m_pTable_BatchHeader;
		class Table *m_pTable_BatchUser;
		class Table *m_pTable_BatchDetail;
		class Table *m_pTable_Tables;
		class Table *m_pTable_Schema;
		
		string m_sName;
		MapTable m_mapTable; 

	public:
		class Database *m_pDatabase;

		/** @brief constructor */
		Repository( class Database *pDatabase, string sName )
		{
			m_pDatabase=pDatabase;
			m_sName=sName;
			m_pTable_Setting=m_pTable_BatchHeader=m_pTable_BatchUser=m_pTable_BatchDetail=m_pTable_Tables=m_pTable_Schema=NULL;
		}
		
		/**
		 *  @brief This lists all the tables and what repositories they belong to
		 */
		void ListTables( );

		/** @brief Displays a summary of all the rows changed in the repository.  Called by the 'diff' command.  returns false means the user wants to quit */
		bool ShowChanges();

		string Name_get( ) { return m_sName; }
		void AddDefinitionTable( class Table *pTable );
		void MatchUpTables( );
		void Setup( );
		void Remove( );
		bool CheckIn( );
		void RollbackBatch();
		void RollbackBatch(Table *pTable,int psc_id,int psc_toc,int psc_batch,sqlCVSprocessor *p_sqlCVSprocessor);
		bool Update( );
		void Dump( );
		bool DetermineDeletions( );
		void AddTablesToMap( );
		int CreateBatch( sqlCVSprocessor *psqlCVSprocessor, map<int,ValidatedUser *> *mapValidatedUsers );
		int psc_id_last_sync_get( Table *pTable );
		void psc_id_last_sync_set( Table *pTable, int psc_id );
		int psc_batch_last_sync_get( Table *pTable );

		/* psc_batch_last_sync must be set *ONLY* by an incoming update in response to an 'update' command.
		We had a logic flaw when we updated psc_batch_last_sync to be the largest number in the table.  
		Say the last psc_batch I updated is 2.  I never got 3 and 4.  Then I do a checkin
		and that's batch 5.  If we ask for all changes>5, that means it thought we deleted
		the rows for batches 3 and 4.  If we say give all changes>2, then we'll get batch 5, which 
		we already did.  We need to send a list of batches to exclude--those in our local list, >2 which we already
		checked in ourselves, and only update psc_batch_last_sync during an update */
		void psc_batch_last_sync_set( Table *pTable, int psc_id );

		bool HasChangedRecords();  /** @brief True if there are changed records in any of the tables */

		/** @brief Rebuilds all the system tables in case something changed */
		void ResetSystemTables();

		class Table *CreateSettingTable( );
		class Table *CreateBatchHeaderTable( );
		class Table *CreateBatchUserTable( );
		class Table *CreateBatchDetailTable( );
		class Table *CreateTablesTable( );
		class Table *CreateSchemaTable( );
	
		void VerifyIntegrity() { }

		string GetSetting(string Setting,string Default);
		void SetSetting(string Setting,string Value);
		void ImportTable(string sTableName,SerializeableStrings &str,size_t &pos,Table *pTable,int ipsc_id_last,int ipsc_batch_last);
		bool UpdateSchema(int PriorSchema);
		void UpdateClientSchema(RA_Request *pRA_Request,int iSchemaVersion);
		bool ProcessSchemaUpdate(string sSQLCommand);
		int GetSchemaVersion() { return atoi(GetSetting("schema","0").c_str()); };

		bool ApproveBatch(R_ApproveBatch *pR_ApproveBatch,sqlCVSprocessor *psqlCVSprocessor);

		class Table *m_mapTable_Find( string sTable ) { MapTable::iterator it = m_mapTable.find( sTable ); return it==m_mapTable.end( ) ? NULL : ( *it ).second; }
	};
	
	typedef map<string, Repository *> MapRepository;
	typedef list<Repository *> ListRepository;
}

#endif
