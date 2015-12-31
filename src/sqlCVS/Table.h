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
 * @file Table.h
 * @brief header file for the Table class
 * @author
 *
 */
 
#ifndef Table_h
#define Table_h

#include "mysql.h"
#include "Field.h"
#include "ChangedRow.h"
#include "Socket.h"
#include "CommonFunctions.h"

#include <iostream>

class RA_Processor;
class R_CommitRow;
class sqlCVSprocessor;
class R_UpdateTable;
class A_UpdateRow;

namespace sqlCVS
{
	typedef map<string, class Table *> MapTable;
	typedef list<class Table *> ListTable;

	/**
	 * @brief  brief description of the class
	 * @todo complete the documentation
	 */
	 
	class Table
	{
		friend class ChangedRow;
		friend class Repository;
		friend class Database;
		friend class ::R_CommitRow;
		friend class ::R_UpdateTable;

		class Database *m_pDatabase;  /**< m_pDatabase points to the database the table belongs to */

		map<int, ListChangedRow *> m_mapUsers2ChangedRowList;
		ListChangedRow *m_mapUsers2ChangedRowList_Find( int psc_user ) { map<int, ListChangedRow *>::iterator it = m_mapUsers2ChangedRowList.find( psc_user ); return it==m_mapUsers2ChangedRowList.end( ) ? NULL : ( *it ).second; }
		
		/**
		 * While doing a checkin, we may change the primary keys for the auto incr fields.  Yet the changed rows
		 * for tables that reference this one are already built, using the old primary key.  This map will allow
		 * the ChangedRow's WHERE clause builder to use the new ID, not the original one
		 */

		map<int,int> m_mapUncommittedAutoIncrChanges;

		/**
		 * When we determine the deletions we made locally, we also determine the rows deleted on the server side. 
		 * Store them here so we don't need to request them twice in the event of an update + sync.
		 */
		
		vector<int> m_vectRowsToDelete; 

		MapField m_mapField;
		class Field *m_mapField_Find( string sField ) { MapField::iterator it = m_mapField.find( sField ); return it==m_mapField.end( ) ? NULL : ( *it ).second; }
		string m_sName;
		class Repository *m_pRepository;
		bool m_bIsSystemTable;
		class Field *m_pField_id;
		class Field *m_pField_batch;
		class Field  *m_pField_user;
		class Field  *m_pField_frozen;
		class Field  *m_pField_mod;
		class Field  *m_pField_restrict;
		class Field  *m_pField_AutoIncrement;
		
		class Table *m_pTable_History;		/**< A pointer to our history table, if we have one,   */
		class Table *m_pTable_History_Mask;		/**< A pointer to our history mask table, if we have one.  This stores what fields were changed in each commit in the history table   */
		class Table *m_pTable_WeAreHistoryFor; 	/**<  or the table we are logging history for if we're a history table */
		class Table *m_pTable_WeAreHistoryMaskFor; 	/**<  or the table we are the history mask for if we're a history mask table */
		ListField m_listField_PrimaryKey;
		
		/** If users are not able to retrieve the entire database, this will be a filter <%=U%> will be replaced with the user ID */
		string m_sFilter; 

		/** If the table is frozen, then no records can be changed */
		bool m_bFrozen;

		/** If the table allows anonymous checkins from users without specifying a password */
		bool m_bAnonymous;

		/** For the server: 
		 *  Because we can't have 2 auto-increment fields, and the primary key may be one already, 
		 * we'll have to manually keep track of this 
		 */
		
		int m_psc_id_next;
		
		/** For the client: Keep track of the last psc_id we sync'd so we can figure out what we deleted locally */
		
		int m_psc_id_last_sync, m_psc_batch_last_sync;

		/** When we are doing an update, we may add a row in Table X which has a foreign key to 
		Table Y (say value 100).  But maybe in table Y there is already an old key with 100 in
		an unauthorized batch, and it hasn't yet been moved because Table X is being updated before
		Table Y.  Now, when we get to updating Table Y, we're going to re-locate the existing 100
		to a new number, and propagate the changes.  But we don't want the propagated changes to 
		affect the new row we just got in Table Y--only ones that were there already.  So each
		table stores new psc_id's added in the following variable. */
		string m_s_psc_id_new_this_update; 

	public:
int itmp_RowsToDelete;
		/** @brief constructor */
		
		Table( class Database *pDatabase, string sName );
		
		/** @brief destructor */
		~Table( );

		/** @brief Revert changes made locally */
		bool RevertAllChanges();
		bool RevertChange(ChangedRow *pChangedRow);
		bool RevertChange(int psc_id,enum TypeOfChange toc);

		/** @brief Displays a summary of all the rows changed in the table.  Called by the 'diff' command.  returns false means the user wants to quit */
		bool ShowChanges(bool bAllUsers=false);

		/** @brief Displays a summary of all the rows changed in the table by the given user.  Called by the 'diff' command.  returns false means the user wants to quit */
		bool ShowChanges(int psc_user);

		void ValidateTable();

		/** If the table is in the format some_text_something it returns 'something'--the text following the final _ */
		string GetTrailingString( string sName="" ); 
		
		 /** @brief 
		  * If this is one of the tables defining a repository ( ie psc_RepositoryName_myfunction ) it returns RepositoryName, 
		  * otherwise empty string 
		  */
		string DefinesRepository( );
		void SetRepository( class Repository *pRepository, bool bIsSystemTable ) 
		{ m_pRepository=pRepository; m_bIsSystemTable=bIsSystemTable; }

		void GetFields( );
		void GetDependencies( );
		bool ConfirmDependencies( );
		bool ConfirmDependency( ChangedRow *pChangedRow, Field *pField_Referring, Field *pField_ReferredTo );

		void MatchUpHistory( );
		bool HasFullHistory_get( ) { return m_pTable_History!=NULL; }
		void HasFullHistory_set( bool bOn );

		/** @brief
		 * This version is called by the server. It finds all new rows and modified rows changed after the specified batch
		 * and adds an UpdateRow action for each change. It does not need to send delete's because the client side
		 * will have already determined the deletes in DetermineDeletions
		 */
		 
		void GetChanges( R_UpdateTable *pR_UpdateTable );

		/** This version is called by the client side to get all the changes and send them to the server */
		
		void GetChanges( );

		bool psc_id_exists(int psc_id); // Returns true if the psc_id is already in the database
		bool ModifiedRow(int psc_id); // Returns true if the psc_id was modified

		/** Called by the client side to update itself with the server's changes */
		bool Update( RA_Processor &ra_Processor, DCE::Socket *pSocket );

		bool Dump( SerializeableStrings &str );

		void AddChangedRow( ChangedRow *pChangedRow );
		bool CheckIn( int psc_user, RA_Processor &ra_Processor, DCE::Socket *pSocket, enum TypeOfChange toc );
		bool DetermineDeletions( RA_Processor &ra_Processor, string Connection, DCE::Socket **ppSocket );
		void AddRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen ); /**< Server side add */
		void DeleteRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen, int &psc_user ); /**< Server side delete */
		void UpdateRow( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor, bool &bFrozen, int &psc_user ); /**< Server side update */
		void ApplyChangedRow(ChangedRow *pChangedRow); /**< Applies a changed row.  Used when authorizing a previously unauthorized batch */
		int ReassignAutoIncrValue(int iPrimaryKey);  /**< Gives the row with the past primary key a new auto-increment value, and returns that value */
		
		void UpdateRow( A_UpdateRow *pA_UpdateRow, R_UpdateTable *pR_UpdateTable, sqlCVSprocessor *psqlCVSprocessor ); /**< Client side update */
		void AddToHistory( R_CommitRow *pR_CommitRow, sqlCVSprocessor *psqlCVSprocessor,int psc_batch_in_history=0 );
		void AddToHistory( ChangedRow *pChangedRow );

		void PropagateUpdatedField( Field *pField_Changed, string NewValue, string OldValue, ChangedRow *pChangedRow );
		void PropagateUpdatedField( Field *pField_Changed, string NewValue, string OldValue, ChangedRow *pChangedRow, Field *pField_FK, string sIndirectFieldName="" );

		bool TrackChanges_get( ) { return m_pField_id && m_pField_batch && m_pField_user && m_pField_frozen && m_pField_mod && m_pField_restrict; }
		void TrackChanges_set( bool bOn );

		bool DoWeHaveBatch( int psc_batch );

		void VerifyIntegrity();

		bool bIsSystemTable_get( ) { return m_bIsSystemTable; }

		/** Give the psc_id, this will fill a map where the first string is the field name, and the second is the value of the field.
		If psc_batch_in_history is not 0, that means this batch is still unauthorized, and needs to be retrieved from the history 
		table, not the main table. */
		void GetCurrentValues(int psc_id,MapStringString *mapCurrentValues,int psc_batch_in_history=0);
		void GetBatchContents(int psc_batch,map<int,ChangedRow *> &mapChangedRow);

		/** @brief Accessors */
		string Name_get( ) { return m_sName; }
		class Repository *Repository_get( ) { return m_pRepository; }
	};
}

#endif
