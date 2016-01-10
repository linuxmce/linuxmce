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
 * @file ChangedRow.h
 * @brief header file for the Dependency, DependetRow and ChangedRow classes
 * @author
 *
 */

  
#ifndef ChangedRow_h
#define ChangedRow_h

#include "mysql.h"
#include <iostream>
#include "CommonFunctions.h"

namespace sqlCVS
{
	/**
	 * @brief  This is a general dependency between 2 tables
	 */
	 
	class Dependency
	{
		class Field *m_pField_Dependent;
		class Field *m_pFieldinfo_Master;
		class Field *m_pField_Indirect;
	
		/**
	 	 * @brief constructor
	 	 */	
	 
		Dependency(class Field *pField_Dependent,class Field *pField_Master,class Field *pField_Indirect)
		{
			m_pField_Dependent=pField_Dependent;
			m_pFieldinfo_Master=pField_Master;
			m_pField_Indirect=pField_Indirect;
		}
	};

	/**
	 * @brief This is a particular row that depends on another
	 */

	class DependentRow
	{
		class Dependency *m_pDependency;  /**< The general dependency */
		
		class ChangedRow *m_pChangedRow_Master; /**< points to a changed row */
		
		class ChangedRow *m_pChangedRow_Dependent;   /**< points to a changed row */
		
		int m_psc_id_Dependent;
		int m_psc_id_Master;

		/**
		 * @brief constructor
		 */
		 
		DependentRow(class Dependency *pDependency)
		{
			m_pDependency=pDependency;
			m_pChangedRow_Dependent=NULL;
			m_psc_id_Dependent=-1;

			m_pChangedRow_Master=NULL;
			m_psc_id_Master=-1;
		}
	};
	
	typedef list<DependentRow *> ListDependentRow;

	/**
	 * @brief This is a row that has been changed on the client
	 */
	 
	class ChangedRow
	{
	public:
		int m_psc_id;		/**< the id of the row */
		int m_psc_batch;	
		int m_psc_user;         /**< the user */
		int m_psc_restrict;
		int m_iBeforeTransmit_iAutoIncrID;  /** The AutoIncr ID before we send it.  May not be the original because it could have been moved around a few times */
		int m_iOriginalAutoIncrID; /** The original AutoIncr ID before any changes.  This is used by m_mapUncommittedAutoIncrChanges to keep track of all changes */
		int m_iNewAutoIncrID;
		bool m_bCommitted;     /**< whether or not the change was committed */
		bool m_bFrozen;  		/**< The row could not be committed because it's frozen */
		bool m_bReverted;  		/**< The user reverted the change */
		int m_psc_user_needs_to_authorize; /**< If not 0 the row could not be committed because this user needs to authorize it */
		int m_psc_batch_new; /**< The new batch after a checkin */
		int m_psc_id_new; /**< The new psc_id after a checkin */
		class Table *m_pTable; /**< points to a Table */

		/**< This class has different uses.  Sometimes it's used to just to keep pointers to changed rows, in which case m_vectPrimaryKey
			is used.  Other times it stores all the information for modified fields in m_mapFieldValues */
		vector<string> m_vectPrimaryKey;
		MapStringString m_mapFieldValues;

		TypeOfChange m_eTypeOfChange;	/**< the type of the change committed */

		/**
		 * @brief constructor
		 */
		 
		ChangedRow(class Table *pTable, TypeOfChange eTypeOfChange, int psc_id, int psc_batch, int psc_user, int psc_restrict, int iBeforeTransmit_iAutoIncrID, vector<string> &vectPrimaryKey)
		{
			m_pTable=pTable;
			m_vectPrimaryKey = vectPrimaryKey;
			m_eTypeOfChange = eTypeOfChange;
			m_psc_id=psc_id; m_psc_batch=psc_batch; m_psc_user=psc_user; m_psc_restrict=psc_restrict;
			m_iOriginalAutoIncrID=m_iBeforeTransmit_iAutoIncrID=iBeforeTransmit_iAutoIncrID;
			m_iNewAutoIncrID=-1;
			m_bCommitted=false;
			m_bReverted=m_bFrozen=false;
			m_psc_user_needs_to_authorize=m_psc_batch_new=m_psc_id_new=0;
cout << "changed row " << " psc_id: " << m_psc_id << " has keys:" << (int) m_vectPrimaryKey.size();
for(size_t s=0;s<m_vectPrimaryKey.size();++s)
cout << " " << m_vectPrimaryKey[s] << ", ";
cout << endl;
		}
		
		/**
		 * @brief This constructor is used ONLY for rows that were deleted on the client side
		 */
		 
		ChangedRow(class Table *pTable, int psc_id)
		{
			m_pTable=pTable;
			m_eTypeOfChange = toc_Delete;
			m_psc_id=psc_id;
			m_bCommitted=false;
			m_bReverted=m_bFrozen=false;
			m_psc_user=m_psc_user_needs_to_authorize=m_psc_batch=m_psc_batch_new=m_psc_id_new=m_psc_restrict=0;
			m_iBeforeTransmit_iAutoIncrID=m_iNewAutoIncrID=0;;
		}

		/**
		 * @brief This constructor is used for keeping track of changes in a batch
		 */

		ChangedRow( class Table *pTable, TypeOfChange eTypeOfChange, int psc_id, int psc_batch )
		{
			m_pTable=pTable;
			m_eTypeOfChange = eTypeOfChange;
			m_psc_id=psc_id;
			m_psc_batch=psc_batch;
			m_bCommitted=true;  // This is storing info on a batch.  It's already been committed
			m_bReverted=m_bFrozen=false;
			m_psc_user=m_psc_user_needs_to_authorize=m_psc_batch_new=m_psc_id_new=m_psc_restrict=0;
			m_iBeforeTransmit_iAutoIncrID=m_iNewAutoIncrID=0;;
		}

		/** 
		 * @brief  brief documentation
		 * @todo complete documentation
		 */
		 
		string GetWhereClause();
	};
	typedef list<ChangedRow *> ListChangedRow;
}

#endif
