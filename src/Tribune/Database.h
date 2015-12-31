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

#include <iostream>
//#include "Repository.h"
#include "Database.h"
//#include "Table.h"
#include "PlutoUtils/DBHelper.h"

namespace Tribune
{
	/**
	 * @brief class modelling the database
	 * @todo complete documentation
	 */
	 
	class Database : public DBHelper
	{
		bool m_bInvalid;

	public:
		/** @brief constructor */
		
		Database( string db_host, string db_user, string db_pass, string db_name, int db_port );
		 
		DB_LINK *DB_LINK_get( ) { return m_pDB; }

		void StartTransaction( );
		void Commit( );
		void Rollback( );

		string Name_get( ) { return m_sDBDBName; }
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
