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
 * @file sqlCVSProcessor.h
 * @brief header file for the request processor class specific to the sqlCVS program
 * @author
 *
 */

#ifndef sqlCVSprocessor_h
#define sqlCVSprocessor_h

#include "RA/RA_Processor.h"
#include "CommonFunctions.h"
#include "Database.h"

namespace sqlCVS
{
	class Table;
	class Repository;
}

extern sqlCVSprocessor *g_psqlCVSprocessor;

using namespace sqlCVS;

/**
 * @brief request action processor class
 */
class sqlCVSprocessor : public RA_Processor
{
public:
	time_t tTime_Creation;
	time_t tTime_LastActivity;
	class RAServerSocket *m_pRAServerSocket;
	sqlCVS::Table *m_pTable;	/**< The table we are currently committing */
	vector<string> *m_pvectFields;	/**< The fields of the table @todo ask */
	int m_i_psc_batch;
	sqlCVS::Repository *m_pRepository; /**< The repository we're currently committing */
	bool m_bSupervisor,m_bAllAnonymous;
	int m_iNew,m_iMod,m_iDel,m_ipsc_user_default;
	string m_sComments;  /**< Optional comments to be included with this transaction */

	int m_psc_bathdr_orig,m_psc_bathdr_auth,m_psc_bathdr_unauth,m_psc_user_needs_to_authorize;
	
	/**< This will create a transaction for every processor that will rollback automatically if the processor goes away with an R_CloseTransaction */
	SafetyTransaction st;

	void LogActivityTime()
	{
		tTime_LastActivity = time(NULL);
		//printf("last Activitity Time set to %i", tTime_LastActivity);
	}

	/** @brief constructor assigning null to all the pointers */
	sqlCVSprocessor( RAServerSocket *pRAServerSocket ) : 
		RA_Processor( 0, 1, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount ), 
		st(g_GlobalConfig.m_pDatabase) 
	{ 
		m_pTable=NULL; m_pvectFields=NULL; m_i_psc_batch=1; m_pRepository=NULL; m_bSupervisor=false; m_bAllAnonymous=true; m_iNew=m_iMod=m_iDel=0; 
		m_psc_bathdr_orig=m_psc_bathdr_auth=m_psc_bathdr_unauth=m_ipsc_user_default=m_psc_user_needs_to_authorize=0;
		tTime_LastActivity = tTime_Creation = time(NULL);  m_pRAServerSocket=pRAServerSocket;
	}
	
	virtual ~sqlCVSprocessor( ) 
	{ 
		if( st.m_bIsOpen_get() ) 
			st.Rollback(); 
		else
			RecordChangesToTable();

		if( g_psqlCVSprocessor==this )
		{
			g_psqlCVSprocessor=NULL; 
			printf("Clearing g_psqlCVSprocessor %p\n",this);
		}
	}

	/** @brief Creates a batch for holding unauthorized transactions.  psc_user_needs_to_authorize is the user who is needed
		to authorize the transaction, or if null, that means any user can do it.  The function will return the id of the batch
		to hold the unauthorized transaction.  The batch # will be negative, meaning it was never a 'real' batch.  When a new
		unauthorized batch is requested and one has already been opened and psc_user_needs_to_authorize is 0 or the same as the
		prior, it will return the same batch id, which is stored in m_psc_bathdr_unauth. */
	int UnauthorizedBatch(int psc_user_needs_to_authorize);

	/** @brief As a table is updated the counters (New/Mod/Del) are incremented.  When we have finished updating a table, this will record those changes into the batch */
	void RecordChangesToTable();
};

#endif
