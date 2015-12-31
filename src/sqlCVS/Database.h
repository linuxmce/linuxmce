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
 * @file Database.h
 * @brief header file for the Database, SafetyDBLock, SafetyTransaction classes
 * @author
 *
 */
 
#ifndef Database_h
#define Database_h

#include "mysql.h"
#include <iostream>
#include "Repository.h"
#include "Database.h"
#include "Table.h"
#include "PlutoUtils/MySQLHelper_moved.h"

namespace sqlCVS
{
	/**
	 * @brief class modelling the database
	 * @todo complete documentation
	 */
	 
	class Database : public MySqlHelper
	{
		MapRepository m_mapRepository;
		MapTable m_mapTable;
		bool m_bInvalid;

	public:
		/**
		 * @brief Whenever we have to prompt for anything, this flag will be set to true and we will confirm changes 
		 * If the user specified everything on the command line, then we will not ask for confirmation
		 */
		bool m_bInteractiveMode;

		/** @brief constructor */
		
		Database( string db_host, string db_user, string db_pass, string db_name, int db_port );

		/** @brief Called by the constructor, this fills in all the maps for repositories and tables */
		void LoadDatabaseStructure();

		/** @brief This will take a string of repositories and tables, and add the actual names to the lists */
		void GetRepositoriesTables( );
		
		/** @brief Displays a summary of all the rows changed in the database.  Called by the 'diff' command */
		void ShowChanges();

		/** @brief Revert changes from a mask file */
		void Revert();

		/** @brief Change a primary key and propagate changes */
		void ChangeKey();

		/** @brief Rollback an entire batch */
		void RollbackBatch();

		/** 
		 * @ brief This will create the repository 
		 */
		
		void CreateRepository( );
		
		/**
		 * @brief brief documentation
		 * @todo complete documentation
		 */
		 
		int PromptForTablesInRepository( Repository *pRepository, MapTable &mapTable );
		
		/** @brief This will return the maximum length of the tablename */
		size_t MaxTablenameLength( );

		/** 
		 * @brief brief documentation
		 * @todo complete documentation
		 */
		 
		MYSQL *MYSQL_get( ) { return m_pMySQL; }

		/**
		 * @brief This will search in the repository 
		 */
		 
		class Repository *m_mapRepository_Find( string sRepository ) 
		{
		 MapRepository::iterator it = m_mapRepository.find( sRepository ); 
		 return it==m_mapRepository.end( ) ? NULL : ( *it ).second; 
		 }
		
		/**
		 * @brief This will search in the Table
		 */
		 
		class Table *m_mapTable_Find( string sTable ) 
		{ 
		MapTable::iterator it = m_mapTable.find( sTable ); 
		return it==m_mapTable.end( ) ? NULL : ( *it ).second; 
		}
		
		/**
		 * @brief This will remove a record from the Table
		 */
		 
		void m_mapTable_Remove( string sTable ) 
		{ 
		MapTable::iterator it = m_mapTable.find( sTable ); 
		if( it!=m_mapTable.end( ) ) m_mapTable.erase( it ); 
		}
		
		/**
		 * @brief  This will add a record in the Table
		 */
		 
		void m_mapTable_Add( string sTable, class Table *pTable ) { m_mapTable[sTable]=pTable; }

		/**
		 *  @brief This will search the tables reffered to in the Field paramater
		 */
		 
		class Table *GetTableFromForeignKey( class Field *pField );
		
		/**
		 *  @brief This will search the tables reffered to in the Table parameter
		 */
		 		
		class Table *GetTableFromForeignKeyString( Table *pTable, string sField );

		/**
		 *  @brief This lists all the tables and what repositories they belong to
		 */
		void ListTables( );

		/**
		 *  @brief List all the repositories and what tables they have
		 */
		void ListRepositories( );

		/**
		 *  @brief Turn on/off history tracking for all tables
		 */
		void HasFullHistory_set_all( bool bOn );

		/**
		 *  @brief Reset all the tables, updating the psc_id, clearing out the 'mod', recreating the system tables, etc.
		 */
		void Reset_all();

		/**
		 *  @brief Reset all the system tables
		 */
		void Reset_sys();

		/**
		 *  @brief Update the psc_id fields for any records that are new
		 */
		void Update_psc();

		/**
		 *  @brief If this is a server, update the last_psc_id and last_psc_batch in the psc_xxx_tables
		 */
		void Update_lastpsc();

		/**
		 *  @brief Approve an existing unauthorized batch
		 */
		void Approve( bool bReject=false );

		/**
		 *  @brief MySql's transaction handling.  This only works with the InnoDB engine
		 */
		void StartTransaction( );
		void Commit( );
		void Rollback( );

		void CheckIn( bool bFirstLoop );
		void Update( );
		void Dump( );
		void Import( );
		void Import( string sRepository, Repository *pRepository );
		int PromptForRepositories( bool bOneOnly=false );
		int PromptForSqlCvsFiles( );
		void GetTablesToCheckIn( );
		int ConfirmUsersToCheckIn( );
		int ConfirmUsersToUpdate( );
		bool ConfirmRecordsToCheckIn( );

		 /**
		  * @brief  List all unauthorized batches
		  */
		 void ListUnauthorizedBatches();

		 /**
		  * @brief  List the contents of a batch
		  */
		void ListBatchContents();

		string Name_get( ) { return m_sMySQLDBName; }
		bool bIsInvalid( ) { return m_bInvalid; }
	};

	/**
	 * @brief  This will manage the transactions in the database
	 */
	 
	class SafetyTransaction
	{
		Database *m_pDatabase;	/**< Points to the database */
		bool m_bIsOpen;		/**< This will say whether the database is opened or not */
	public:
		/** @brief constructor */
		SafetyTransaction( class Database *pDatabase ) { m_pDatabase=pDatabase; m_pDatabase->StartTransaction( ); m_bIsOpen=true; printf("Starting transaction: %p\n",this); }
		
		/** @brief destructor */
		~SafetyTransaction( ) 
		{ 
			if( m_bIsOpen )
			{
				printf("Transaction %p is open.  Rolling back\n",this);
				cerr << "System error: Rolling back an incomplete transaction" << endl;
				m_pDatabase->Rollback( );
			}
		}
		
		/**
		 * @brief This will roll back the transaction
		 */
		 void Rollback( ) 
		 {
			 if( m_bIsOpen )
			 {
				printf("Transaction %p is open.  Explicit rolling back\n",this);
				m_pDatabase->Rollback( ); 
				m_bIsOpen=false; 
			 }
			 else
				printf("Transaction %p is closed.  Ignoring explicit rolling back\n",this);
		 }
		 
		 /**
		  * @brief  This will commit the transaction
		  */
		 void Commit( ) 
		 { 
			 if( m_bIsOpen )
			 {
				printf("Transaction %p is open.  Explicit Commit\n",this);
				m_pDatabase->Commit( ); 
				m_bIsOpen=false; 
			 }
			 else
				printf("Transaction %p is closed.  Ignoring explicit Commit\n",this);
		}
		 /**
		  * @brief  True if the transaction is open or pending and has not been rolledback or committed
		  */
		 bool m_bIsOpen_get() { return m_bIsOpen; }
	};
}
#endif
